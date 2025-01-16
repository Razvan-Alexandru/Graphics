#include "Raytracer.h"
#include "Ray.h"
#include "../external/json.hpp"
#include "../external/simpleppm.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <Eigen/Dense>

class Vector3f;

using namespace std;

Eigen::Vector3f findPointC(Output& output, int width, int height, float delta, Eigen::Vector3f& rVector);
float intersectSphere(Ray& r, Geometry& object);
float intersectRectangle(Ray& r, Geometry& object);
Eigen::Vector3f findNormal(Eigen::Vector3f& pointOfIntersection, Geometry* objectptr);
bool isInShadow(vector<Geometry>& objects, Geometry* objectptr, Ray& rayToLight, float distanceToLight);

RayTracer::RayTracer(const nlohmann::json j) : scene(j) {}

void RayTracer::run() {
    for(auto output : scene.outputs) {

        for(auto light : scene.lights) {
            if(light.type == "area" && !light.usecentre) {
                output.antialiasing = false;
                break;
            }
        }

        if (output.globalillum) {
            output.antialiasing = false;
            for (auto light : scene.lights) {
                light.usecentre = true;
            }
        }

        vector<double> buffer;
        
        if(!output.globalillum && !output.antialiasing) {
            cout << "Rendeing Direct Illumination!" << endl;
            buffer = renderDirectIllumination(output);
            std::cerr << "\nDone.\n";
        } 

        else if(output.globalillum) {
            cout << "Rendering Global Illumination!" << endl;
            buffer = renderGlobalIllumination(output);
            std::cerr << "\nDone.\n";
        }

        else if(output.antialiasing) {
            cout << "Rendering Anti-Aliasing!" << endl;
            buffer = renderAntiAliasing(output);
            std::cerr << "\nDone.\n";
        }

        else { cout << "Something went wrong!" << endl; }
        
        save_ppm(output.filename, buffer, output.size[0], output.size[1]);
    }
}

vector<double> RayTracer::renderDirectIllumination(Output& output) {
    
    vector<double> buffer(3 * output.size[0] * output.size[1]);

    // STEP 1 : Get the point C which is at the top-left corner of the viewport
    // lookat, up and right vectors and the fov (in degrees) define the camera
    // delta is the size of a pixel
    // Point A is the centre of the viewport,
    // Point B is the top-centre point of the viewpoert, and
    // Point C is the top-left corner of the viewport

    int height = output.size[1];
    int width = output.size[0];
    float delta = (2 * tan(output.fov / 2)) / height;
    Eigen::Vector3f rVector = (output.lookat).cross(output.up).normalized();

    Eigen::Vector3f pointC = findPointC(output, width, height, delta, rVector);

    // STEP 2 : using a nested for loop, go through each pixels left-to-right and top-to-bottom
    // Each ray has to pass to the middle of each pixels
    // A pixel has coordinates (x, y), where x is the horizontal axis, and y is the vertical axis
    // pointOnPixel is the middle part of each pixel, 
    // We start at point C and we go right 1 delta for every pixel in each row, 
    // and 1 pixel down for every pixels in each column
    // from that we can go halh a delta right and bottom to get the middle of the pixel

    for(int y = 0; y < height; y++) {
        int prog = (y + 1) * 100 / height;
		std::cerr << "\rProgression: " << prog << " %" << std::flush;
        for(int x = 0; x < width; x++) {
            Eigen::Vector3f pointOnPixel = pointC + (x * delta + delta / 2) * rVector - (y * delta + delta / 2) * output.up;

            Ray ray(output.centre, (pointOnPixel - output.centre));
            Geometry* objectptr = nullptr;
            float t = -1;

            for(Geometry& object : scene.geometries) {
                float temp = -1;
                if(object.type == "sphere") {
                    temp = intersectSphere(ray, object);
                }

                if(object.type == "rectangle") {
                    temp = intersectRectangle(ray, object);
                }

                if(temp > 0 && (t < 0 || temp < t )) { 
                    t = temp; 
                    objectptr = &object;
                }
            }

            if(t < 0) { 
                buffer[3 * y * width + 3 * x + 0] = (double) output.bkc[0];
                buffer[3 * y * width + 3 * x + 1] = (double) output.bkc[1];
                buffer[3 * y * width + 3 * x + 2] = (double) output.bkc[2];

                continue;
            }

            Eigen::Vector3f pointOfIntersection = ray.at(t);
            Eigen::Vector3f normal = findNormal(pointOfIntersection, objectptr);

            // STEP 4 : Send a light to the light

            float red    = 0;
            float green  = 0;
            float blue   = 0;

            for(Light& light : scene.lights) {

                float distanceToLight;
                Eigen::Vector3f lightDir;

                if(light.type == "point" || (light.type == "area" && light.usecentre)) {
                    lightDir = light.centre - pointOfIntersection;
                    Ray rayToLight(pointOfIntersection, lightDir);
                    distanceToLight = lightDir.norm();

                    if(isInShadow(scene.geometries, objectptr, rayToLight, distanceToLight)) { continue; }

                    lightDir.normalize();

                    float diffuse = objectptr->kd * max(0.0f, normal.dot(lightDir));
                    red   += light.id[0] * objectptr->dc[0] * diffuse;
                    green += light.id[1] * objectptr->dc[1] * diffuse;
                    blue  += light.id[2] * objectptr->dc[2] * diffuse;

                    float specular = (2 * normal * (normal.dot(lightDir)) - lightDir).dot(-ray.direction());
                    specular = objectptr->ks * pow(max(0.0f, specular), objectptr->pc);
                    red   += light.is[0] * objectptr->sc[0] * specular;
                    green += light.is[1] * objectptr->sc[1] * specular;
                    blue  += light.is[2] * objectptr->sc[2] * specular;

                }
                else {

                    auto seed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
                    mt19937 gen(seed);
                    uniform_real_distribution<float> randGenerator(0.0f, 1.0f);

                    for(int row = 0; row < light.n; row++) {
                        for(int col = 0; col < light.n; col++) {

                            Eigen::Vector3f b_l_point = light.p2 + col * light.xVec + row * light.yVec;
                            Eigen::Vector3f pointOnCell = b_l_point + randGenerator(gen) * light.xVec + randGenerator(gen) *light.yVec;

                            lightDir = pointOnCell - pointOfIntersection;
                            Ray rayToLight(pointOfIntersection, lightDir);
                            distanceToLight = lightDir.norm();

                            if(isInShadow(scene.geometries, objectptr, rayToLight, distanceToLight)) { continue; }

                            lightDir.normalize();

                            float diffuse = objectptr->kd * max(0.0f, normal.dot(lightDir));
                            red   += light.id[0] * objectptr->dc[0] * diffuse;
                            green += light.id[1] * objectptr->dc[1] * diffuse;
                            blue  += light.id[2] * objectptr->dc[2] * diffuse;

                            float specular = (2 * normal * (normal.dot(lightDir)) - lightDir).dot(-ray.direction());
                            specular = objectptr->ks * pow(max(0.0f, specular), objectptr->pc);
                            red   += light.is[0] * objectptr->sc[0] * specular;
                            green += light.is[1] * objectptr->sc[1] * specular;
                            blue  += light.is[2] * objectptr->sc[2] * specular;
                        }
                    }

                    red   /= (light.n * light.n);
                    green /= (light.n * light.n);
                    blue  /= (light.n * light.n);
                }
            }

            red    += output.ai[0] * objectptr->ac[0] * objectptr->ka;
            green  += output.ai[1] * objectptr->ac[1] * objectptr->ka;
            blue   += output.ai[2] * objectptr->ac[2] * objectptr->ka;

            red   = max(0.0f, min(red, 1.0f));
            green = max(0.0f, min(green, 1.0f));
            blue  = max(0.0f, min(blue, 1.0f));

            if(objectptr != nullptr) {
                buffer[3 * y * width + 3 * x + 0] = (double) red;
                buffer[3 * y * width + 3 * x + 1] = (double) green;
                buffer[3 * y * width + 3 * x + 2] = (double) blue;
            } 
        }
    }
    return buffer;
}

vector<double> RayTracer::renderAntiAliasing(Output& output) {
    auto seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> randGenerator(0.0f, 1.0f);

    vector<double> buffer(3 * output.size[0] * output.size[1]);

    int height = output.size[1];
    int width = output.size[0];
    float delta = (2 * tan(output.fov / 2)) / height;
    Eigen::Vector3f rVector = (output.lookat).cross(output.up).normalized();

    int raysperpixel = output.rpp[0] * output.rpp[1] * output.rpp[2];

    Eigen::Vector3f pointC = findPointC(output, width, height, delta, rVector);
    pointC = pointC - delta * output.up; //Get bottom-left corner, easier with calculation afterwards

    Eigen::Vector3f gridCell;
    Eigen::Vector3f rayDir;

    float sizeRow = delta / output.rpp[0];
    float sizeCol = delta / output.rpp[1];

    Eigen::Vector3f sizeRowVector = sizeRow * output.up;
    Eigen::Vector3f sizeColVector = sizeCol * rVector;

    for(int y = 0; y < height; y++) {
        int prog = (y + 1) * 100 / height;
		std::cerr << "\rProgression: " << prog << " %" << std::flush;
        for(int x = 0; x < width; x++) {
            Eigen::Vector3f pointD = pointC + (x * delta) * rVector - (y * delta) * output.up; //Get bottom-left corner of all pixels

            float red = 0;
            float green = 0;
            float blue = 0;

            // We are inside a pixel, now we need to stratify it, using raysperpixel, we know
            // what grid to use [a, b] and how many rays per grid-cell (c). 
            // So we will go to the bottom left corner of each grid-cell and then apply a random sampling
            // to have a point randomly selected inside said grid-cell

            for(int row = 0; row < output.rpp[0]; row++) {
                for (int col = 0; col < output.rpp[1]; col++) {
                    gridCell = pointD + col * sizeColVector + row * sizeRowVector;

                    for(int nRayPerCell = 0; nRayPerCell < output.rpp[2]; nRayPerCell++){
                        rayDir = gridCell + randGenerator(gen) * sizeColVector + randGenerator(gen) * sizeRowVector;
                        rayDir = rayDir - output.centre;
                        Ray ray(output.centre, rayDir);

                        Geometry* objectptr = nullptr;
                        float t = -1;

                        for(Geometry& object : scene.geometries) {
                            float temp = -1;
                            if(object.type == "sphere") {
                                temp = intersectSphere(ray, object);
                            }

                            if(object.type == "rectangle") {
                                temp = intersectRectangle(ray, object);
                            }

                            if(temp > 0 && (t < 0 || temp < t )) { 
                                t = temp; 
                                objectptr = &object;
                            }
                        }

                        if (t < 0) {
                            red    += output.bkc[0];
                            green  += output.bkc[1];
                            blue   += output.bkc[2];

                            continue;
                        }

                        Eigen::Vector3f pointOfIntersection = ray.at(t);
                        Eigen::Vector3f normal = findNormal(pointOfIntersection, objectptr);

                        red   += output.ai[0] * objectptr->ac[0] * objectptr->ka;
                        green += output.ai[1] * objectptr->ac[1] * objectptr->ka;
                        blue  += output.ai[2] * objectptr->ac[2] * objectptr->ka;

                        for(auto light : scene.lights) {
                            Eigen::Vector3f lightDir = light.centre - pointOfIntersection;
                            Ray rayToLight(pointOfIntersection, lightDir);
                            float distanceToLight = lightDir.norm();

                            if(isInShadow(scene.geometries, objectptr, rayToLight, distanceToLight)) { continue; }

                            lightDir.normalize();

                            float diffuse = objectptr->kd * max(0.0f, normal.dot(lightDir));
                            red   += light.id[0] * objectptr->dc[0] * diffuse;
                            green += light.id[1] * objectptr->dc[1] * diffuse;
                            blue  += light.id[2] * objectptr->dc[2] * diffuse;

                            float specular = (2 * normal * (normal.dot(lightDir)) - lightDir).dot(-ray.direction());
                            specular = objectptr->ks * pow(max(0.0f, specular), objectptr->pc);
                            red   += light.is[0] * objectptr->sc[0] * specular;
                            green += light.is[1] * objectptr->sc[1] * specular;
                            blue  += light.is[2] * objectptr->sc[2] * specular;

                        }
                    }
                }
            }
        
            red   = min(red/raysperpixel, 1.0f);
            green = min(green/raysperpixel, 1.0f);
            blue  = min(blue/raysperpixel, 1.0f);

            buffer[3 * y * width + 3 * x + 0] = (double) red;
            buffer[3 * y * width + 3 * x + 1] = (double) green;
            buffer[3 * y * width + 3 * x + 2] = (double) blue;
             
        }
    }

    return buffer;
}

vector<double> RayTracer::renderGlobalIllumination(Output& output) {

    auto seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> randGenerator(0.0f, 1.0f);

    vector<double> buffer(3 * output.size[0] * output.size[1]);

    int height = output.size[1];
    int width = output.size[0];
    float delta = (2 * tan(output.fov / 2)) / (float) height;
    Eigen::Vector3f rVector = (output.lookat).cross(output.up).normalized();

    int raysperpixel = output.rpp[0] * output.rpp[1] * output.rpp[2];

    Eigen::Vector3f pointC = findPointC(output, width, height, delta, rVector);
    pointC = pointC - delta * output.up; //Get bottom-left corner, easier with calculation afterwards

    Eigen::Vector3f gridCell;
    Eigen::Vector3f rayDir;

    float sizeRow = delta / (float) output.rpp[0];
    float sizeCol = delta / (float) output.rpp[1];

    Eigen::Vector3f sizeRowVector = sizeRow * output.up;
    Eigen::Vector3f sizeColVector = sizeCol * rVector;

    for(int y = 0; y < height; y++){
        int prog = (y + 1) * 100 / height;
        std::cerr << "\rProgression: " << prog << " %" << std::flush;
        for(int x = 0; x < width; x++) {
            Eigen::Vector3f pointD = pointC + (x * delta) * rVector - (y * delta) * output.up; //Get bottom-left corner of all pixels

            int raysNotCounted = 0;

            float red   = 0;
            float green = 0;
            float blue  = 0;

            for(int row = 0; row < output.rpp[0]; row++) {
                for (int col = 0; col < output.rpp[1]; col++) {
                    gridCell = pointD + col * sizeColVector + row * sizeRowVector;

                    for(int nRayPerCell = 0; nRayPerCell < output.rpp[2]; nRayPerCell++){
                        rayDir = gridCell + randGenerator(gen) * sizeColVector + randGenerator(gen) * sizeRowVector;
                        rayDir = rayDir - output.centre;
                        Ray ray(output.centre, rayDir);

                        float redIntensity   = 1;
                        float greenIntensity = 1;
                        float blueIntensity  = 1;


                        int bounce = 0;
                        while(true) {

                            Geometry* objectptr = nullptr;
                            float t = -1;

                            for(Geometry& object : scene.geometries) {
                                float temp = -1;
                                if(object.type == "sphere") {
                                    temp = intersectSphere(ray, object);
                                }

                                if(object.type == "rectangle") {
                                    temp = intersectRectangle(ray, object);
                                }

                                if(temp > 0 && (t < 0 || temp < t )) {
                                    t = temp;
                                    objectptr = &object;
                                }
                            }

                            if (t < 0 && bounce == 0) {
                                red    += output.bkc[0];
                                green  += output.bkc[1];
                                blue   += output.bkc[2];

                                break;
                            }

                            if(t < 0) { raysNotCounted++; break; }

                            Eigen::Vector3f pointOfIntersection = ray.at(t);
                            Eigen::Vector3f normal = findNormal(pointOfIntersection, objectptr);

                            redIntensity   *= objectptr->kd * objectptr->dc[0];
                            greenIntensity *= objectptr->kd * objectptr->dc[1];
                            blueIntensity  *= objectptr->kd * objectptr->dc[2];

                            if(bounce >= output.maxbounces || randGenerator(gen) < output.probterminates) {
                                for(auto light : scene.lights) {
                                    Eigen::Vector3f lightDir = light.centre - pointOfIntersection;
                                    Ray rayToLight(pointOfIntersection, lightDir);
                                    float distanceToLight = lightDir.norm();

                                    if(!isInShadow(scene.geometries, objectptr, rayToLight, distanceToLight)) {
                                        red    += redIntensity   * light.id[0] * max(rayToLight.direction().dot(normal), 0.0f);
                                        green  += greenIntensity * light.id[1] * max(rayToLight.direction().dot(normal), 0.0f);
                                        blue   += blueIntensity  * light.id[2] * max(rayToLight.direction().dot(normal), 0.0f);
                                    }
                                }
                                break;
                            }


                            Eigen::Vector3f randVector(2 * randGenerator(gen) -1, 2 * randGenerator(gen) -1, 2 * randGenerator(gen) -1);
                            randVector.normalize();

                            Eigen::Vector3f xVector = randVector.cross(normal);
                            Eigen::Vector3f zVector = xVector.cross(normal);

                            Eigen::Matrix3f randMatrix;
                            randMatrix <<   xVector[0], normal[0], zVector[0],
                                            xVector[1], normal[1], zVector[1],
                                            xVector[2], normal[2], zVector[2];

                            float randX;
                            float randY;
                            float randZ;

                            while(true) {
                                randX = 2 * randGenerator(gen) -1;
                                randZ = 2 * randGenerator(gen) -1;

                                if((randX * randX) + (randZ * randZ) <= 1) {
                                    randY = sqrt(1 - (randX * randX) - (randZ * randZ));
                                    break;
                                }
                            }

                            Eigen::Vector3f pointOnHemisphere = Eigen::Vector3f(randX, randY, randZ);
                            pointOnHemisphere = randMatrix * pointOnHemisphere;

                            if(pointOnHemisphere.dot(normal) < 0) {
                                pointOnHemisphere *= -1;
                            }

                            Ray newRay(pointOfIntersection, pointOnHemisphere);
                            float damping = max(normal.dot(newRay.direction()), 0.0f);

                            redIntensity   *= damping;
                            greenIntensity *= damping;
                            blueIntensity  *= damping;

                            ray = newRay;
                            bounce++;
                        }
                    }
                }
            }

            int raysCounted = raysperpixel - raysNotCounted;
            if (raysCounted == 0) {
                buffer[3 * y * width + 3 * x + 0] = 0;
                buffer[3 * y * width + 3 * x + 1] = 0;
                buffer[3 * y * width + 3 * x + 2] = 0;
                break;
            }

            red   = min(red / (float) raysCounted, 1.0f);
            green = min(green / (float) raysCounted, 1.0f);
            blue  = min(blue / (float) raysCounted, 1.0f);

            buffer[3 * y * width + 3 * x + 0] = (double) sqrt(red);
            buffer[3 * y * width + 3 * x + 1] = (double) sqrt(green);
            buffer[3 * y * width + 3 * x + 2] = (double) sqrt(blue);

        }
    }

    return buffer;
}

Eigen::Vector3f findPointC(Output& output, int width, int height, float delta, Eigen::Vector3f& rVector) {

    Eigen::Vector3f pointA = output.centre + output.lookat;
    Eigen::Vector3f pointB = pointA + tan(output.fov / 2) * output.up;

    return (pointB - (width * delta / 2) * rVector);
}

float intersectSphere(Ray& r, Geometry& object) {
    float aParam = r.direction()[0] * r.direction()[0] +
                   r.direction()[1] * r.direction()[1] +
                   r.direction()[2] * r.direction()[2];

    float bParam = 2 * ((r.origin() - object.centre).dot(r.direction()));

    float cParam = (r.origin() - object.centre)[0] * (r.origin() - object.centre)[0] +
                   (r.origin() - object.centre)[1] * (r.origin() - object.centre)[1] +
                   (r.origin() - object.centre)[2] * (r.origin() - object.centre)[2] - 
                   object.radius * object.radius;

    if((bParam * bParam - (4 * aParam * cParam)) < 0) { return -1; }
    if((bParam * bParam - (4 * aParam * cParam)) == 0) {
        return (-bParam/(2 * aParam)) > 0 ? -bParam/(2 * aParam) : -1;
    }

    float squareRoot = sqrt(bParam * bParam - (4 * aParam * cParam));
    float t[2] = {(-bParam - squareRoot)/(2 * aParam), (-bParam + squareRoot)/(2 * aParam)};

    if(t[0] < 0 && t[1] < 0) { return -1; }
    if(t[0] < 0) { return t[1]; }

    return t[0];
}

float intersectRectangle(Ray& r, Geometry& object) {
    if(r.direction().dot(object.normal) == 0) {
        return -1;
    }

    float t = ((object.p1 - r.origin()).dot(object.normal)) / r.direction().dot(object.normal);
    Eigen::Vector3f point = r.at(t);

    int a  = signbit(((object.p4 - object.p1).cross(point - object.p1)).dot(object.normal));
    int b  = signbit(((object.p1 - object.p2).cross(point - object.p2)).dot(object.normal));
    int c  = signbit(((object.p2 - object.p3).cross(point - object.p3)).dot(object.normal));
    int d  = signbit(((object.p3 - object.p4).cross(point - object.p4)).dot(object.normal));

    if( a == b && b == c && c == d) { return t; }

    return -1;
}

Eigen::Vector3f findNormal(Eigen::Vector3f& pointOfIntersection, Geometry* objectptr) {
    if(objectptr->type == "sphere") {
        return (pointOfIntersection - objectptr->centre).normalized();
    }

    if(objectptr->type == "rectangle") {
        return objectptr->normal;
    }

    cout << "error in findNormal!" << endl;
    return Eigen::Vector3f(.0f, .0f, .0f);
}

bool isInShadow(vector<Geometry>& objects, Geometry* objectptr, Ray& rayToLight, float distanceToLight) {

    for(Geometry& otherObject : objects) {
        if(&otherObject == objectptr) { continue; }
    
        if(otherObject.type == "sphere") {
            float t_other = intersectSphere(rayToLight, otherObject);
            if(t_other > 0 && t_other < distanceToLight) { return true; }
        }

        if(otherObject.type == "rectangle") {
            float t_other = intersectRectangle(rayToLight, otherObject);
            if(t_other > 0 && t_other < distanceToLight) { return true; }
        }
    }

    return false;
}