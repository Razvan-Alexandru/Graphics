#include "Model.h"
#include "Tools.h"
#include "Gui.h"

#include <iostream>

using namespace std;

double referenceTime = 0;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
double timeElapsed = 0;

int width = 1920;
int height = 1080;

Camera camera;
int current_cam_focus = -1;
float camAngle = 0;
float fov = 45.0f;
bool focus = false;

bool firstMouse = true;
bool lb_pressed = false;
float yaw   = 180.0f;
float pitch =  0.0f;
float lastX =  width / 2.0;
float lastY =  height / 2.0;

bool isKeyPressed = false;

float simSpeed = 1.0f;

int main() {

    getReferenceTime();
    GLFWwindow* window = init();

    string vertexShaderPath = "../src/shaders/vertex_shader.glsl";
    string fragmentShaderPath = "../src/shaders/fragment_shader.glsl";
    GLuint shaderProgram = loadShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    if(!shaderProgram) {
        cerr << "Error in loading Shaders";
        exit(0);
    }

    string lvertexShaderPath = "../src/shaders/light_vertex_shader.glsl";
    string lfragmentShaderPath = "../src/shaders/light_fragment_shader.glsl";
    GLuint lightShaderProgram = loadShaders(lvertexShaderPath.c_str(), lfragmentShaderPath.c_str());
    if(!lightShaderProgram) {
        cerr << "Error in loading Shaders";
        exit(0);
    }

    string overtexShaderPath = "../src/shaders/orbit_vertex_shader.glsl";
    string ofragmentShaderPath = "../src/shaders/orbit_fragment_shader.glsl";
    GLuint oShaderProgram = loadShaders(overtexShaderPath.c_str(), ofragmentShaderPath.c_str());
    if(!oShaderProgram) {
        cerr << "Error in loading Shaders";
        exit(0);
    }

    string skyboxVertexShaderPath = "../src/shaders/skybox_vertex_shader.glsl";
    string skyboxFragmentShaderPath = "../src/shaders/skybox_fragment_shader.glsl";
    GLuint skyboxShaderProgram = loadShaders(skyboxVertexShaderPath.c_str(), skyboxFragmentShaderPath.c_str());
    if(!skyboxShaderProgram) {
        cerr << "Error in loading Shaders";
        exit(0);
    }

    vector<Planet> planets;
    Rings saturnRings;
    setPlanets(planets, saturnRings);

    Light light;
    light.position = glm::vec3(0.0f);
    light.l_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    setShaderVector(shaderProgram, "lightPos", light.position);

    Skybox skybox;
    setSkybox(skybox);

    glm::vec3 position(414.0f, 0.0f, 0.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    camera.setUp(position, target);

    // Init ImGui
    initImGui(window);

    cout << endl << "Starting main loop" << endl; 
    while(!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime(); // Current machine time
        deltaTime = currentFrame - lastFrame;
        timeElapsed += deltaTime * simSpeed;
        lastFrame = currentFrame; 

        processInput(window, deltaTime, planets);
        setShaderVector(shaderProgram, "cameraPos", camera.position());

        //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float) width / height, 0.1f, 10000000.0f);

        renderPlanets(shaderProgram, planets, saturnRings, projection);
        renderSun(lightShaderProgram, planets[0], projection);
        renderOrbits(oShaderProgram, planets, projection);
        renderSkybox(skyboxShaderProgram, skybox, projection);

        if(current_cam_focus >= 0) {
            // Render ImGui
            renderImGui(planets); 
        }
        renderLegend();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui
    cleanup();

    glfwDestroyWindow(window);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(lightShaderProgram);
    glDeleteProgram(oShaderProgram);
    glDeleteProgram(skyboxShaderProgram);

    glfwTerminate();
    return 0;
}

