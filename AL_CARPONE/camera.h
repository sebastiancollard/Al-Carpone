#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "State.h"
#include "Player.h"


////////////////////////////////////////////////////////////////////////////////
// CAMERA
////////////////////////////////////////////////////////////////////////////////

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

    bool usingKeyboard = true;

    // prevent first click snap
    bool firstClick = true;

    virtual void handleInput(GLFWwindow* window, State& state) {}

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(pos, pos + dir, up);
    }
};


////////////////////////////////////////////////////////////////////////////////
// FREE CAMERA
////////////////////////////////////////////////////////////////////////////////

class FreeCamera : public Camera {

public:

    float movementSpeed;

    FreeCamera(Player& p);
    void handleInput(GLFWwindow* window, State& state);

private:

    Player& player;

    void update();
};


////////////////////////////////////////////////////////////////////////////////
// BOUND CAMERA
////////////////////////////////////////////////////////////////////////////////

//spherical camera
class BoundCamera : public Camera {

public:

    float pitch; //vertical angle offset
    float yaw; //horizontal angle offset

    float radius; //radius of sphere for camera to orbit around
    float default_radius;
    float radius_goal;
    float radius_min_offset = -3.0f;
    float radius_max_offset = 7.0f;
    float verticalOffset; //how much higher the camera is relative to car position

    glm::vec3 prev_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 oldVehDir = glm::vec3(0, 0, 1);

    bool stoppedLookingBehind = false; // used for switching between looking behind and forward
    bool controllerIdle = false;
    float controllerTimer = 0.f;
    
    BoundCamera(Player& p, State& state);
    bool checkController(GLFWgamepadstate state);
    void handleInput(GLFWwindow* window, State& state); //update pitch and yaw (in the future update zoom)
    void checkClipping(GLFWwindow* window);             // adjust camera position if objects are currently obscurring the car

private:
    Player& player;

    void updateLook();
    void updateLocked(State& state);
    void updateLockedController(State& state);
};

#endif