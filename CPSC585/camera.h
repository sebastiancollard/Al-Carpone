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

//Update these as we tune
#define CAR_MAX_VELOCITY_FORWARD 40.0f
#define CAR_MAX_VELOCITY_BACKWARD -11.35f


glm::vec3 getGLMvec3(PxVec3 v) {
    return glm::vec3(v.x, v.y, v.z);
}

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

    glm::mat4 GetViewMatrix() {
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

        pos = player.getPos() - player.getDir() * 15.0f + glm::vec3(0, 2.5f, 0);
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

    float radius; //radius of sphere for camera to orbit around

    float default_radius;
    float radius_goal;

    float radius_min_offset = -3.0f;
    float radius_max_offset = 7.0f;

    float verticalOffset; //how much higher the camera is relative to car position

    glm::vec3 prev_velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 oldVehDir = glm::vec3(0, 0, 1);

    bool stoppedLookingBehind = false; // used for switching between looking behind and forward

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

        verticalOffset = 3.5f;

        default_radius = 10.0f;
        radius = default_radius;

        radius_goal = default_radius;

        updateLocked();
    }

    //update pitch and yaw (in the future update zoom)
    void handleInput(GLFWwindow* window) {
        // Handles mouse inputs
        
        // check for state of C key before left mouse button to avoid camera stuttering
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            dir = -player.getDir();
            stoppedLookingBehind = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && stoppedLookingBehind) {
            dir = player.getDir();
            stoppedLookingBehind = false;
        }
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

            // Calculates upcoming vertical change in the Orientation
            up = worldUp;
            glm::vec3 newOrientation = glm::rotate(dir, glm::radians(-rotY), glm::normalize(glm::cross(dir, up)));

            // Decides whether or not the next vertical Orientation is legal or not
            if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                dir = newOrientation;
            }

            // Rotates the Orientation left and right
            dir = glm::rotate(dir, glm::radians(-rotX), up);

            //v-----------------------------------DISABLE IF USING KEYBOARD--------------------------------------------------------------------v
            // we also want to calculate the angle between the old vehicle direction and the new one and add to the existing rotation
            // 
            //dir = glm::rotate(dir, atan2(glm::dot(glm::cross(oldVehDir, player.getDir()), up), glm::dot(player.getDir(), oldVehDir)), up);
            // 
            //--------below 2 lines give seemingly same result, but apparently can be less accurate at times..
            //--------float angle = glm::angle(player.getDir(), oldVehDir);
            //--------dir = glm::dot(glm::cross(player.getDir(), oldVehDir), up) < 0 ? glm::rotate(dir, angle, up) : glm::rotate(dir, -angle, up);
            //^-----------------------------------DISABLE IF USING KEYBOARD--------------------------------------------------------------------^

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));

            // update position
            pos = player.getPos() + glm::vec3(0, verticalOffset, 0) + -dir * radius;

            oldVehDir = player.getDir();
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
        

        glm::vec3 velocity = getGLMvec3(player.vehiclePtr->getRigidDynamicActor()->getLinearVelocity());
        //printVec3("velocity", velocity);
        float forwardVel = glm::dot(velocity, player.getDir());
        //printf("forward velocity[%.2f]\n", forwardVel);

        //if (forwardVel < 0) radius_goal = radius_max;
        //else if (forwardVel > 0) radius_goal = radius_min;

        float radius_offset = 0;

        float dot = glm::dot(player.getDir(), dir);
        
        if (forwardVel > 0) {
            radius_offset = radius_max_offset * fmin(forwardVel / CAR_MAX_VELOCITY_FORWARD, 1.0f);
        }
        else if (forwardVel < 0) {
            // when looking behind, the camera behaves better if the min offset uses CAR_MAX_VELOCITY_FORWARD instead of CAR_MAX_VELOCITY_BACKWARD in the clamp
            // so we use the dot product to determine which one to use for the clamped value
            radius_offset = radius_min_offset * (dot > -0.999f ? fmin(forwardVel / CAR_MAX_VELOCITY_BACKWARD, 1.0f) : fmin(forwardVel / CAR_MAX_VELOCITY_FORWARD, 1.0f));
        }

        // add smoothing to radius change (mostly to address cases like hitting a wall head on)
        radius = 0.9f * radius + 0.1f * (default_radius + radius_offset);

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
        glm::vec4 posOffset((-playerDirHorizontalProjection * radius + glm::vec3(0, verticalOffset, 0)), 1.0f);
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

        // get position we want the camera to look at
        glm::vec3 targetPos = player.getPos() + glm::vec3(0, verticalOffset, 0);

        // variable used for interpolation
        float x = float(state.timeStep * 1.2 * state.simulationSpeed);

        // before performing the position updated via interpolation, 
        // make sure the starting point is still on the sphere
        pos = targetPos - dir * radius;

        // first, get HORIZONTAL position as proportional interpolation between 2 locations on the sphere:
        // 1: closest point on sphere between the camera's current position and sphere center
        // 2: point on the sphere behind the car
        pos = glm::vec3((1 - x) * (pos).x + x * (targetPos - player.getDir() * radius).x, pos.y, pos.z); // x-coord
        pos = glm::vec3(pos.x, pos.y, (1 - x) * (targetPos - dir * radius).z + x * (targetPos - player.getDir() * radius).z); //z-coord

        // if we are farther/closer to the target than the orbit radius, then we need to place it on the sphere.
        // instead of finding the general closest point (using the line between the camera and target) we actually 
        // need actually need to project the camera position onto the sphere with respect to the world horizontal plane
        float dist = glm::distance(pos, targetPos);
        if (dist != radius) {
            float heightDiff = targetPos.y - pos.y;
            float horizontalDist = glm::distance(pos, glm::vec3(targetPos.x, pos.y, targetPos.z));
            float crossSectionalRadius = sqrt(radius * radius - (heightDiff) * (heightDiff));
            glm::vec3 tempDir = glm::normalize(glm::vec3(targetPos.x, targetPos.y - heightDiff, targetPos.z) - pos);

            pos.x -= (crossSectionalRadius - horizontalDist) * tempDir.x;
            pos.z -= (crossSectionalRadius - horizontalDist) * tempDir.z;
        }

        // now, update vertical position using the same interpolation
        pos.y = (1 - x) * (targetPos - dir * radius).y + x * (targetPos - player.getDir() * radius).y; //y-coord

       // update the camera position such that it is placed on the 
       // point of the sphere between the current camera position and the sphere center 
        dist = glm::distance(pos, targetPos);
        if (dist != radius)
            pos -= (radius - dist) * dir;

        // update direction of camera
        dir = glm::normalize(targetPos - pos);

        //Update right and up vectors accordingly
        right = glm::normalize(glm::cross(dir, worldUp));
        up = glm::normalize(glm::cross(right, dir));

        oldVehDir = player.getDir();
    }

};

#endif
/*























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

#endif
*/