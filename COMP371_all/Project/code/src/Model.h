#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

using namespace std;

struct Vertex {
    float x, y, z;
};

struct TextureCoord {
    float u, v;
};

struct Normal {
    float x, y, z;
};

struct Face {
    int vIndex[3];
    int tIndex[3];
    int nIndex[3];
};

struct VertexInfo {
    Vertex v;
    Normal n;
    TextureCoord t;
};

struct Orbit {
    unsigned int nSegments = 1000;
    vector<Vertex> vertices;
    glm::vec3 colour = glm::vec3(1.0f);
    GLuint VAO; // vertex array
    GLuint VBO; // vertex buffer
};

struct Rings {
    float radius;

    vector<VertexInfo> vertexInfo;
    vector<Vertex> vertices;
    vector<TextureCoord> TextureCoords;
    vector<Normal> normals;
    vector<Face> faces;
    vector<unsigned int> index;
    GLuint texture;

    glm::vec3 currentPosition;

    GLuint VAO; // vertex array
    GLuint VBO; // vertex buffer
};

struct Planet {
    string name;
    int id;

    vector<VertexInfo> vertexInfo;

    vector<Vertex> vertices;
    vector<TextureCoord> TextureCoords;
    vector<Normal> normals;
    vector<Face> faces;
    vector<unsigned int> index;
    GLuint texture;

    //Planet Infos
    glm::vec3 currentPosition;
    float currentAxisAngle = 0;

    //Description
    string description;
    string radius_str;
    string mass;
    string volume;
    string gravity;
    string temperature;
    string satelites;
    string dayPeriod_str;
    string orbitPeriod_str;
    string averageDistanceToSun;
    string equatorialSpeed;
    string orbitalSpeed;

    float radius; // scale to earth i.e earth is 1 and sun is 109
    float inclination;
    float dayPeriod; // a day in hours

    // Orbit
    Orbit orbit;
    float majorAxis;
    float eccentricity;
    float orbitalPeriod;
    float anomalyJ2000;
    float anomalyM0;
    float meanAnomaly;
    float orbitInclination;
    float longitude;
    float argument;

    // Constants
    float k;
    float a;

    // Orbit plane
    glm::vec3 orbitP;
    glm::vec3 orbitQ;

    GLuint VAO; // vertex array
    GLuint VBO; // vertex buffer

    glm::vec3 axis;

};

struct Light {
    glm::vec3 position;
    glm::vec3 l_colour;
};

struct Skybox {
    GLuint VAO; // vertex array
    GLuint VBO; // vertex buffer
    GLuint texture;
};

