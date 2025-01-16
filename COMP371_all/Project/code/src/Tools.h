#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

#include "Camera.h"
#include "Model.h"
#include "Text.h"

#define GLEW_STATIC 1
#define PI 3.14159265

using namespace std;

extern double referenceTime;

extern int width;
extern int height;

extern float deltaTime;
extern double timeElapsed;

extern Camera camera;
extern int current_cam_focus;
extern float camAngle;
extern float fov;
extern bool focus;

extern bool firstMouse;
extern bool lb_pressed;
extern float yaw;
extern float pitch;
extern float lastX;
extern float lastY;

extern bool isKeyPressed;

extern float simSpeed;

void getReferenceTime();

GLFWwindow* init();
GLuint setUpShaders();
GLuint loadShaders(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
bool parseOBJ(const string &filename, vector<VertexInfo> &vertexInfo, vector<Vertex> &vertices, vector<TextureCoord> &TextureCoords, vector<Normal> &normals, vector<Face> &faces);
GLuint loadTexture(const char* path);

void processInput(GLFWwindow *window, float &delta, vector<Planet> &p);
void followPlanet(Planet &p);
double newtowns(double M, float e);

void setPlanet(Planet &p, string &modelfilename);
void setPlanets(vector<Planet> &planets, Rings &saturnRings);
void setDescription(Planet &p, string mass, string volume, string gravity, string temperature, string satelites, string dayPeriod_str, string orbitPeriod_str, string averageDistanceToSun, string equatorialSpeed, string orbitalSpeed);
void setSkybox(Skybox &s);

void setRotation(Planet &p, float inclination, float period);
void setOrbit(Planet &p, float majorAxis, float eccentricity, float orbitalPeriod, float meanAnomaly, float orbitInclination, float longitude, float argument);
void findPlane(Planet &p);
void createOrbit(Planet &p);

void renderPlanets(GLuint shaderProgram, vector<Planet> &planets, Rings rings, glm::mat4 &projection);
void renderSun(GLuint lightShaderProgram, Planet &sun, glm::mat4 &projection);
void renderOrbits(GLuint shaderProgram, vector<Planet> &planets, glm::mat4 &projection);
void renderSkybox(GLuint skyboxShaderProgram, Skybox &skybox, glm::mat4 &projection);

void setShaderMatrix(const GLuint program, const string &name, const glm::mat4 &mat);
void setShaderVector(const GLuint program, const string &name, const glm::vec3 &vec);
void setShaderFloat(const GLuint program, const string &name, GLfloat value);
void setShaderInt(const GLuint program, const string &name, GLfloat value);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cam_rotate_callback(GLFWwindow* window, double xposIn, double yposIn);
void lb_pressed_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);