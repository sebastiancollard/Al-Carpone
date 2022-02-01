#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

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

    // constructor with vectors
    BoundCamera() {
        mouseSensitivity = 200.0f;
        zoom = 60.0f;

        up = player.getUp();
        right = player.getRight();

        worldUp = glm::vec3(0, 1, 0);

        pos = player.getPos() - player.getDir() * 15.0f + glm::vec3(0, 2.5f, 0);
        dir = player.getDir();

        pitch = 0;
        yaw = 0;

        updateLocked();
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

            // Calculates upcoming vertical change in the Orientation
            up = glm::vec3(0, 1, 0);
            glm::vec3 newOrientation = glm::rotate(dir, glm::radians(-rotY), glm::normalize(glm::cross(dir, up)));

            // Decides whether or not the next vertical Orientation is legal or not
            if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                dir = newOrientation;
            }

            // Rotates the Orientation left and right
            dir = glm::rotate(dir, glm::radians(-rotX), up);

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
            pos = player.getPos() + -dir * 15.f;
            pos += glm::vec3(0, 3.5f, 0);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            // Unhides cursor since camera is not looking around anymore
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // Makes sure the next time the camera looks around it doesn't jump
            firstClick = true;

            //reset to cars view
            //pitch = 0;
            //yaw = 0;
            updateLocked();
        }
    }
private:
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


        glm::vec3 verticalOffset = player.getPos() + glm::vec3(0, 3.5f, 0);

        dir = glm::normalize(verticalOffset - pos);
        
        float x = float(state.timeStep * 1.2);

        pos = ((1-x)*(verticalOffset - dir * 15.f) + x*(verticalOffset - player.getDir() * 15.0f));


        //Update right and up vectors accordingly
        right = glm::normalize(glm::cross(dir, worldUp));
        up = glm::normalize(glm::cross(right, dir));
    }

};

#endif