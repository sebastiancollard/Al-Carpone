#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 10.f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float YawOffset;
    float PitchOffset;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool firstClick = true;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;

        Front = glm::normalize(player.getDir());

        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);

        Front = glm::normalize(player.getDir());

        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
   
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)  
            Position -= Right * velocity; 
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position += -Up * velocity;
    }
 
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void Inputs(GLFWwindow* window) {
        // Handles key inputs
        if (state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                ProcessKeyboard(FORWARD, state.timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                ProcessKeyboard(LEFT, state.timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                ProcessKeyboard(BACKWARD, state.timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                ProcessKeyboard(RIGHT, state.timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                ProcessKeyboard(UP, state.timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            {
                ProcessKeyboard(DOWN, state.timeStep);
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                //speed = 0.2f;
            }
            else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            {
                //speed = 0.1f;
            }
        }

        // Handles mouse inputs
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
         
            // Hides mouse cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // Prevents camera from jumping on the first click
            if (firstClick)
            {
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
            float rotY = MouseSensitivity * (float)(mouseY - (SCREEN_HEIGHT / 2)) / SCREEN_HEIGHT;
            float rotX = MouseSensitivity * (float)(mouseX - (SCREEN_WIDTH / 2)) / SCREEN_WIDTH;

            PitchOffset -= rotY;
            YawOffset += rotX;

            if (PitchOffset > 89.0f)
                PitchOffset = 89.0f;
            if (PitchOffset < -89.0f)
                PitchOffset = -89.0f;

            if (YawOffset > 360.0f) YawOffset -= 360.0f;
            if (YawOffset < -360.0f) YawOffset += 360.0f;

            if (state.cameraMode == CAMERA_MODE_BOUND_FREELOOK) {
                glm::vec4 boundVec((-player.getDir() * 15.0f + glm::vec3(0, 2.5f + PitchOffset, 0)),1.0f);
                glm::mat4 rot(1.0f);
                rot = glm::rotate(rot, YawOffset, Up);

                boundVec = boundVec * rot;

                Position = player.getPos() + glm::vec3(boundVec.x, boundVec.y, boundVec.z);

                glm::vec4 dir(player.getDir(), 1.0f);
                dir = dir * rot;
                
                Front = glm::vec3(dir.x, 0, dir.z);

                updateCameraVectors();
            }

            else if(state.cameraMode == CAMERA_MODE_UNBOUND_FREELOOK) {
                glm::mat4 rot(1.0f);
                rot = glm::rotate(rot, rotX * 0.1f, glm::vec3(0, 1, 0));
                //rot = glm::rotate(rot, -rotY * 0.1f, glm::vec3(1, 0, 0)); Unbound vertical movement is bugged :(
                glm::vec4 front(Front, 1);
                front = front * rot;
                Front = glm::vec3(front.x, front.y, front.z);
                updateCameraVectors();
            }

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            // Unhides cursor since camera is not looking around anymore
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // Makes sure the next time the camera looks around it doesn't jump
            
            firstClick = true;

            if (state.cameraMode == CAMERA_MODE_BOUND) {
                PitchOffset = 0;
                YawOffset = 0;

                Front = player.getDir();
                updateCameraVectors();
            }

            
        }

    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        //glm::mat4 rot(1.0f);
        //rot = glm::rotate(rot, Yaw, Up);
        //rot = glm::rotate(rot, Pitch, Right);
        //glm::vec4 front(Front, 1.0f);
        //front = front * rot;
        //Front = glm::vec3(front.x, front.y, front.z);
        // also re-calculate the Right and Up vector
        Front = glm::normalize(Front);
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
        //printf("RIGHT[<%.2f,%.2f,%.2f>] | WORLDUP[<%.2f,%.2f,%.2f>]\n", Right.x, Right.y, Right.z, WorldUp.x,WorldUp.y, WorldUp.z);
    }
};

#endif
