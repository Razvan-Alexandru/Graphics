#pragma once

#include <string>
#include <sstream>

#include "Model.h"
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>

using namespace std;

extern int width;
extern int height;
extern int current_cam_focus;
extern Camera camera;

extern float simSpeed;

extern bool focus;

void initImGui(GLFWwindow *window);
void renderImGui(vector<Planet> &p);
void renderLegend();
void cleanup();