#include "Gui.h"

void initImGui(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void renderImGui(vector<Planet> &p) {
    constexpr ImGuiWindowFlags imgui_default_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    const ImVec2 entity_box_size = ImVec2(0.25 * width, 0.5 * height);
    const ImVec2 entity_box_pos = ImVec2(width - 0.25 * width - 10, 10);
    
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const char* title = p[current_cam_focus].name.c_str();
    // ImGui content goes here
    ImGui::Begin(title, nullptr, imgui_default_flags);
    ImGui::SetWindowPos(entity_box_pos);
    ImGui::SetWindowSize(entity_box_size);

    ImGui::TextWrapped("%s", p[current_cam_focus].description.c_str());

    string n;
    string rounded;

    ImGui::Indent();
    ImGui::Text("\n");
    ImGui::BulletText("%s: %s", "Radius", p[current_cam_focus].radius_str.c_str());
    ImGui::BulletText("%s: %s", "Mass", p[current_cam_focus].mass.c_str());
    ImGui::BulletText("%s: %s", "Volume", p[current_cam_focus].volume.c_str());
    ImGui::BulletText("%s: %s", "Gravity", p[current_cam_focus].gravity.c_str());
    if(current_cam_focus ==0) 
        ImGui::BulletText("%s: %s", "Temperature", p[current_cam_focus].temperature.c_str());
    if(current_cam_focus != 0) {
        ImGui::BulletText("%s: %s", "Mean Temperature", p[current_cam_focus].temperature.c_str());
        ImGui::BulletText("%s: %s", "Known Satelites", p[current_cam_focus].satelites.c_str());
        ImGui::BulletText("%s: %s", "Day Period", p[current_cam_focus].dayPeriod_str.c_str());
        ImGui::BulletText("%s: %s", "Rotation Speed at Equator", p[current_cam_focus].equatorialSpeed.c_str());
        n = std::to_string(glm::degrees(p[current_cam_focus].inclination));
        rounded = n.substr(0, n.find(".")+3);
        ImGui::BulletText("%s: %s°", "Tilt On Axis", rounded.c_str());
        ImGui::BulletText("%s: %s", "Orbit Period", p[current_cam_focus].orbitPeriod_str.c_str());
        ImGui::BulletText("%s: %s", "Mean Orbital Speed", p[current_cam_focus].orbitalSpeed.c_str());
        ImGui::BulletText("%s: %s", "Mean Distance to the Sun", p[current_cam_focus].averageDistanceToSun.c_str());
        n = std::to_string(glm::degrees(p[current_cam_focus].orbitInclination));
        rounded = n.substr(0, n.find(".")+3);
        ImGui::BulletText("%s: %s°", "Orbital Inclination Over Ecliptic", rounded.c_str());
        ImGui::BulletText("%s: %s", "Eccentricity of Orbit", std::to_string(p[current_cam_focus].eccentricity).c_str());
    }
    
    ImGui::Unindent();

    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void renderLegend() {
    constexpr ImGuiWindowFlags imgui_default_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

    const ImVec2 entity_box_size = ImVec2(0.25 * width, 0.25 * height);
    const ImVec2 entity_box_pos = ImVec2(width - 0.25 * width - 10, height - 0.25 * height - 10);
    
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const char* title = "Legend";
    // ImGui content goes here
    ImGui::Begin(title, nullptr, imgui_default_flags);
    ImGui::SetWindowPos(entity_box_pos);
    ImGui::SetWindowSize(entity_box_size);

    ImGui::Text("KeyBinds:");
    ImGui::Indent();
    ImGui::BulletText("%s: %s", "Quit", "ESC");
    ImGui::BulletText("%s: %s", "Next Planet", "TAB");
    ImGui::BulletText("%s: %s", "Back to Centre", "C");
    ImGui::BulletText("%s: %s", "Zoom in or out", "Scroll Wheel");
    if(focus) {
        ImGui::BulletText("%s: %s", "Rotate", "Q or E");
        ImGui::BulletText("%s: %s", "Free Camera", "F");
    }
    else {
        ImGui::BulletText("%s: %s", "Move", "WASD");
        ImGui::BulletText("%s: %s", "Up", "SPACE");
        ImGui::BulletText("%s: %s", "Down", "CTRL");
        ImGui::BulletText("%s: %s", "Look Around", "Left Mouse Button");
    }
    ImGui::Unindent();

    ImGui::Text("\n\n");
    ImGui::Text("Speed up : UP     Slow down : DOWN");

    if(simSpeed <= 24 && simSpeed >= -24) {
        string n = std::to_string(simSpeed);
        string rounded = n.substr(0, n.find(".")+0);

        ImGui::Text("%s: %s hours", "Simulation speed: 1 second = ", rounded.c_str());
    }
    else {
        string n = std::to_string(simSpeed / 24);
        string rounded = n.substr(0, n.find(".")+0);

        ImGui::Text("%s %s days", "Simulation speed: 1 second = ", rounded.c_str());
    }
    
    //ImGui::SliderFloat("Simulation speed", &simSpeed, 0.0f, 10.0f, "%.1f");


    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}