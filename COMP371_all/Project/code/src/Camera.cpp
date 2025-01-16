#include "Camera.h"

Camera::Camera() {}
Camera::Camera(glm::vec3 &position, glm::vec3 &target) {
    setUp(position, target);
}

void Camera::setUp(glm::vec3 &position, glm::vec3 &target) {
    camPos = position;
    camTarget = target;

    camDir = glm::normalize(camPos - camTarget);
    updateCam();
}

void Camera::translateFront(float speed) {
    camPos += speed * camDir;
    updateView();
}

void Camera::translateSide(float speed) { 
    camPos += speed * camRight; 
    updateView();
}

void Camera::translateUp(float speed) {
    camPos += speed * glm::vec3(0.0f, 1.0f, 0.0f);
    updateView();
}

void Camera::updateView() {
    camView = glm::lookAt(camPos, camPos - camDir, camUp);
}

void Camera::updateCam() {
    camRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camDir));
    camUp = glm::normalize(glm::cross(camDir, camRight));

    updateView();
}