#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:
        Camera();
        Camera(glm::vec3 &position, glm::vec3 &target);

        void setUp(glm::vec3 &position, glm::vec3 &target);

        void setPosition(glm::vec3 position) { camPos = position; }
        void setTarget(glm::vec3 target) { camTarget = target; }
        void setDirection(glm::vec3 direction) { camDir = direction; }
        void setRight(glm::vec3 right) { camRight = right; }

        void translateFront(float speed);
        void translateSide(float speed);
        void translateUp(float speed);

        void updateView();
        void updateCam();

        glm::mat4 view() { return camView; }
        glm::vec3 position() { return camPos; }
        glm::vec3 target() { return camTarget; }
        glm::vec3 up() { return camUp; }

    private :
        glm::vec3 camPos;
        glm::vec3 camTarget;
        glm::vec3 camDir;
        glm::vec3 camRight;
        glm::vec3 camUp;

        // Rotation with mouse
        glm::vec3 direction;

        glm::mat4 camView;
};