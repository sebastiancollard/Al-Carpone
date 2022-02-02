#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera {
public:

    // camera Attributes
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // camera options
   
    float mouseSensitivity;
    float zoom;

    // prevent first click snap
    bool firstClick = true;

    virtual void handleInput(GLFWwindow* window) {}
    virtual float getPitch() { return 0; }
    virtual float getPitchGoal() { return 0; }
    virtual float getYaw() { return 0; }
    virtual float getYawGoal() { return 0; }

    glm::mat4 GetViewMatrix(){
        return glm::lookAt(pos, pos + dir, up);
    }
};

class FreeCamera : public Camera {
public:
    float movementSpeed;
    FreeCamera() {
        movementSpeed = 10.0f;
        mouseSensitivity = 5.0f;
        zoom = 60.0f;

        pos = player.getPos() -player.getDir() * 15.0f + glm::vec3(0, 2.5f, 0);
        dir = player.getDir();

        worldUp = glm::vec3(0, 1, 0);

        update();
    }
    void handleInput(GLFWwindow* window) {
        float velocity = movementSpeed * state.timeStep;
        if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // FORWARD
            {
                pos += dir * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // LEFT
            {
                pos -= right * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // BACKWARD
            {
                pos -= dir * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // RIGHT
            {
                pos += right * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // UP
            {
                pos += up * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // DOWN
            {
                pos += -up * velocity;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                movementSpeed = 50.f;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            {
                movementSpeed = 10.f;
            }
        }
        // Handles mouse inputs
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // Hides mouse cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // Prevents camera from jumping on the first click
            if (firstClick) {
                glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
                firstClick = false;
            }

            // Stores the coordinates of the cursor
            double mouseX;
            double mouseY;

            // Fetches the coordinates of the cursor
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
            // and then "transforms" them into degrees 
            float rotY = mouseSensitivity * (float)(mouseY - (SCREEN_HEIGHT / 2)) / SCREEN_HEIGHT;
            float rotX = mouseSensitivity * (float)(mouseX - (SCREEN_WIDTH / 2)) / SCREEN_WIDTH;

            glm::mat4 rot(1.0f);
            rot = glm::rotate(rot, rotY, right);
            rot = glm::rotate(rot, rotX, up);
            
            glm::vec4 dirUpdate(dir, 1.0f);
            dirUpdate = dirUpdate * rot;

            dir = glm::vec3(dirUpdate.x, dirUpdate.y, dirUpdate.z);

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            // Unhides cursor since camera is not looking around anymore
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // Makes sure the next time the camera looks around it doesn't jump
            firstClick = true;
        }
        update();
    }

    float getPitch() { return 0; }
    float getPitchGoal() { return 0; }
    float getYaw() { return 0; }
    float getYawGoal() { return 0; }

private:
    void update() {
        //Update right and up vectors according to dir
        right = glm::normalize(glm::cross(dir, worldUp));
        up = glm::normalize(glm::cross(right, dir));
    }
};

//spherical camera
class BoundCamera : public Camera {
public:

    float pitch; //vertical angle offset
    float yaw; //horizontal angle offset

    float pitch_goal;
    float yaw_goal;

    float dPitch_dT = 0;
    float dYaw_dT = 0;

    float radius = 15.0f;
    float radius_goal = radius;
    // constructor with vectors
    BoundCamera() {
        mouseSensitivity = 5.0f;
        zoom = 60.0f;

        up = player.getUp();
        right = player.getRight();

        worldUp = glm::vec3(0, 1, 0);

        pos = player.getPos() - player.getDir() * 15.0f + glm::vec3(0, 2.5f, 0);
        dir = player.getDir();

        pitch = 0;
        yaw = 0;

        update();
    }

    //update pitch and yaw (in the future update zoom)
    void handleInput(GLFWwindow* window) {
        // Handles mouse inputs
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            // Hides mouse cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // Prevents camera from jumping on the first click
            if (firstClick){
                glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
                firstClick = false;
                pitch_goal = pitch;
                yaw_goal = yaw;
            }

            // Stores the coordinates of the cursor
            double mouseX;
            double mouseY;

            // Fetches the coordinates of the cursor
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
            // and then "transforms" them into degrees 
            float rotY = mouseSensitivity * (float)(mouseY - (SCREEN_HEIGHT / 2)) / SCREEN_HEIGHT;
            float rotX = mouseSensitivity * (float)(mouseX - (SCREEN_WIDTH / 2)) / SCREEN_WIDTH;

            //update pitch/yaw accordingly
            pitch_goal += rotY;
            yaw_goal += rotX;

            //bind yaw between [-pi,pi] so that it will drift in about the more efficient angle
            if (yaw_goal > M_PI) yaw_goal *= -1;
            else if (yaw_goal < - M_PI) yaw_goal *= -1;

            //bind pitch
            if (pitch_goal > M_PI  /2.25f) pitch_goal = M_PI / 2.25f;
            if (pitch_goal < -M_PI / 2.25f) pitch_goal = -M_PI / 2.25f;

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));

            pitch = pitch_goal;
            yaw = yaw_goal;

            update();
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
            // Unhides cursor since camera is not looking around anymore
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // Makes sure the next time the camera looks around it doesn't jump
            firstClick = true;

            //reset to cars view
            pitch_goal = 0;
            yaw_goal = 0;
            update();
        }
    }
private:
    void update() {
       // calculate the new direction vector
        glm::mat4 rot(1.0f);
        //rotate by pitch and yaw
        rot = glm::rotate(rot, pitch, player.getRight());
        rot = glm::rotate(rot, yaw, worldUp);

        //Use projection of player direction for smoother behaviour entering/exiting hills
        glm::vec3 playerDirHorizontalProjection = glm::normalize(glm::vec3(player.getDir()[0], 0, player.getDir()[2]));

        //apply rotation to pos
        glm::vec4 posOffset((-playerDirHorizontalProjection * 15.0f + glm::vec3(0, 3.5f, 0)), 1.0f);
        posOffset = posOffset * rot;

        //apply rotation to dir about players direction
        glm::vec4 transformedDir(playerDirHorizontalProjection, 1.0f);
        transformedDir = transformedDir * rot;

        dir = glm::normalize(glm::vec3(transformedDir.x, transformedDir.y, transformedDir.z));

        //Add the offset vector to the playerpos to get the updated pos
        glm::vec3 playerPos = player.getPos();
        pos = glm::vec3(playerPos.x + posOffset.x, playerPos.y + posOffset.y, playerPos.z + posOffset.z);

        if (pitch != pitch_goal || yaw != yaw_goal) {
            //shift pitch/yaw toward the goal
            pitch += (pitch_goal - pitch) * state.timeStep * 3.0f;
            yaw += (yaw_goal - yaw) * state.timeStep * 3.0f;
            //if pitch/yaw are close enough to the goal, just set them to the goal
            if (abs(pitch - pitch_goal) < 0.001) pitch = pitch_goal;
            if (abs(yaw - yaw_goal) < 0.001) yaw = yaw_goal;
        }

        PxVec3 angVelPx = player.vehiclePtr->getRigidDynamicActor()->getAngularVelocity();

        glm::vec3 angVel(angVelPx.x, angVelPx.y, angVelPx.z);
        printVec3("angVel",angVel);
        yaw += angVelPx.y * state.timeStep;

        //Update right and up vectors accordingly
        right = glm::normalize(glm::cross(dir, worldUp));
        up = glm::normalize(glm::cross(right, dir));
    }

    float getPitch() { return pitch; }
    float getPitchGoal() { return pitch_goal; }
    float getYaw() { return yaw; }
    float getYawGoal() { return yaw_goal; }
};

/*
 void updateLook() {
        //std::cout << glm::dot(player.getLinearVelocity(), player.getDir()) << std::endl;
        // calculate the new direction vector
        glm::mat4 rot(1.0f);
        //rotate by pitch and yaw
        rot = glm::rotate(rot, pitch, player.getRight());
        rot = glm::rotate(rot, yaw, worldUp);

        //Use projection of player direction for smoother behaviour entering/exiting hills
        glm::vec3 playerDirHorizontalProjection = glm::normalize(glm::vec3(player.getDir()[0], 0, player.getDir()[2]));

        //apply rotation to dir about players direction
        glm::vec4 transformedDir(playerDirHorizontalProjection, 1.0f);
        transformedDir = transformedDir * rot;
        //apply rotation to pos
        glm::vec4 posOffset((-playerDirHorizontalProjection * 15.0f + glm::vec3(0, 3.5f, 0)), 1.0f);
        posOffset = posOffset * rot;

        dir = glm::normalize(glm::vec3(transformedDir.x, transformedDir.y, transformedDir.z));

        //Add the offset vector to the playerpos to get the updated pos
        glm::vec3 playerPos = player.getPos();
        pos = glm::vec3(playerPos.x + posOffset.x, playerPos.y + posOffset.y, playerPos.z + posOffset.z);

        //Update right and up vectors accordingly
        right = glm::normalize(glm::cross(dir, worldUp));
        up = glm::normalize(glm::cross(right, dir));
    }

    void updateLocked() {
        // calculate the new direction vector
        glm::mat4 rot(1.0f);

        //std::cout << "before: " << this->pos.x << ", " << this->pos.y << ", " << this->pos.z << std::endl;

        glm::vec3 verticalOffset = player.getPos() + glm::vec3(0, 3.5f, 0);

        dir = glm::normalize(verticalOffset - pos);

        float x = float(state.timeStep * 1.2);

        pos = ((1-x)*(verticalOffset - dir * 15.f) + x*(verticalOffset - player.getDir() * 15.0f));

        //std::cout << "after: " << this->pos.x << ", " << this->pos.y << ", " << this->pos.z << std::endl;


        //Update right and up vectors accordingly
        right = glm::normalize(glm::cross(dir, worldUp));
        up = glm::normalize(glm::cross(right, dir));
    }

};
*/

#endif