#include "Camera.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "constants.h"
#include "physx_globals.h" //TODO remove later

//Update these as we tune

#define CONTROLLER_RESET_MAX_TIME 2.f

// General Function
glm::vec3 getGLMvec3(PxVec3 v) {
    return glm::vec3(v.x, v.y, v.z);
}

////////////////////////////////////////////////////////////////////////////////
// FREE CAMERA
////////////////////////////////////////////////////////////////////////////////


FreeCamera::FreeCamera(Player& p) : player(p) {
    movementSpeed = 50.0f;
    mouseSensitivity = 5.0f;
    zoom = 60.0f;

    pos = player.getPos() - player.getDir() * 15.0f + glm::vec3(0, 2.5f, 0);
    dir = player.getDir();

    worldUp = glm::vec3(0, 1, 0);

    update();
}


void FreeCamera::update() {
    //Update right and up vectors according to dir
    right = glm::normalize(glm::cross(dir, worldUp));
    up = glm::normalize(glm::cross(right, dir));
}


void FreeCamera::handleInput(GLFWwindow* window, State& state) {
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
            movementSpeed = 500.f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        {
            movementSpeed = 50.f;
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



////////////////////////////////////////////////////////////////////////////////
// BOUND CAMERA
////////////////////////////////////////////////////////////////////////////////



 // constructor with vectors
BoundCamera::BoundCamera(Player& p, State& state) : player(p) {
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

    updateLocked(state);
}



//update pitch and yaw (in the future update zoom)
void BoundCamera::handleInput(GLFWwindow* window, State& state) {
    // Handles mouse inputs
    // check for state of C key before left mouse button to avoid camera stuttering
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        usingKeyboard = true;
        dir = -player.getDir();
        stoppedLookingBehind = true;
    }
    else if (usingKeyboard && stoppedLookingBehind) {
        dir = player.getDir();
        stoppedLookingBehind = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        usingKeyboard = true;
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
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && usingKeyboard)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;

        //reset to cars view
        //pitch = 0;
        //yaw = 0;
        updateLocked(state);
    }
    
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        //get controller name
        //const char* controller_name = glfwGetGamepadName(GLFW_JOYSTICK_1);
        //std::cout << controller_name << std::endl;

        GLFWgamepadstate controller;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &controller))
        {
            controllerIdle = true;
            if (abs(controller.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB]) == 1) {
                controllerIdle = false;
                controllerTimer = 0.f;
                if (usingKeyboard) usingKeyboard = false;
                dir = -player.getDir();
                stoppedLookingBehind = true;
            }
            else if (!usingKeyboard && stoppedLookingBehind) {
                controllerTimer = CONTROLLER_RESET_MAX_TIME;
                dir = player.getDir();
                stoppedLookingBehind = false;
            }

            float rotX = 0;
            float rotY = 0;
            if (abs(controller.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]) > 0.15)
            {
                controllerIdle = false;
                controllerTimer = 0.f;
                if (usingKeyboard) usingKeyboard = false;
                rotY = controller.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * 0.33f;
            }
            if (abs(controller.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) > 0.15)
            {
                controllerIdle = false;
                controllerTimer = 0.f;
                if (usingKeyboard) usingKeyboard = false;
                rotX = controller.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * 0.5f;
            }
            if (abs(controller.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB]) == 1) {
                controllerTimer = CONTROLLER_RESET_MAX_TIME;
            }
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
            
            //dir = glm::rotate(dir, atan2(glm::dot(glm::cross(oldVehDir, player.getDir()), up), glm::dot(player.getDir(), oldVehDir)), up);


            // update position
            pos = player.getPos() + glm::vec3(0, verticalOffset, 0) + -dir * radius;

            oldVehDir = player.getDir();

            if (controllerIdle && !usingKeyboard && controllerTimer  < CONTROLLER_RESET_MAX_TIME) {
                controllerTimer += state.timeStep;
            }
        }

        if (!usingKeyboard && controllerTimer >= CONTROLLER_RESET_MAX_TIME) {
            std::cout << "update " << state.timeStep << std::endl;
            updateLocked(state);
        }
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
    radius = 0.95f * radius + 0.05f * (default_radius + radius_offset);

}



// adjust camera position if objects are currently obscurring the car
void BoundCamera::checkClipping(GLFWwindow* window) {
    // get camera's target and direction
    glm::vec3 target = player.getPos() + glm::vec3(0, verticalOffset, 0);
    glm::vec3 direction = -glm::normalize(target - pos);

    PxVec3 origin = PxVec3(target.x, target.y, target.z);
    PxVec3 unitDir = PxVec3(direction.x, direction.y, direction.z);
    PxRaycastBuffer hit;

    // send raycast starting at the camera target and towards the camera's position
    bool status = gScene->raycast(origin, unitDir, PxReal(radius), hit, PxHitFlag::eMESH_BOTH_SIDES);
    // if the raycast hit something closer than the camera, put the camera in front of it 
    // to be able to see the car again
    if (status)
        pos -= (radius - float(hit.getAnyHit(0).distance) + 0.05f) * direction;

    //OVERLAP TRIGGER TEST
    //PxOverlapBuffer hit2;            // [out] Overlap results
    //
    //PxTransform shapePose = PxTransform(PxVec3(0, 2, 50));    // [in] initial shape pose (at distance=0)
    ////shapePose = shapePose.transform(PxTransform(PxVec3(0, 10, 0)));
    //float halfExtent = 5.f;
    //physx::PxBoxGeometry box(halfExtent, halfExtent, halfExtent);
    //physx::PxVec3 origin(0, 20, 0);
    //physx::PxQuat orientation(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
    //
    //
    //std::cout << "--------------------------- " << std::endl;
    //if (gScene->overlap(box, shapePose, hit2, physx::PxQueryFilterData(physx::PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC)))
    //{
    //    std::cout << "getNbAnyHits: " << hit2.getNbAnyHits() << std::endl;
    //    std::cout << "getNbTouches: " << hit2.getNbTouches() << std::endl;
    //    std::cout << "hasBlock: " << hit2.hasBlock << std::endl;
    //}
}


void BoundCamera::updateLook() {
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
    //apply rotation to origin
    glm::vec4 posOffset((-playerDirHorizontalProjection * radius + glm::vec3(0, verticalOffset, 0)), 1.0f);
    posOffset = posOffset * rot;

    dir = glm::normalize(glm::vec3(transformedDir.x, transformedDir.y, transformedDir.z));

    //Add the offset vector to the playerpos to get the updated origin
    glm::vec3 playerPos = player.getPos();
    pos = glm::vec3(playerPos.x + posOffset.x, playerPos.y + posOffset.y, playerPos.z + posOffset.z);

    //Update right and up vectors accordingly
    right = glm::normalize(glm::cross(dir, worldUp));
    up = glm::normalize(glm::cross(right, dir));
}



void BoundCamera::updateLocked(State& state) {

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