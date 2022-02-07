#include <iostream>
#include "DebugPanel.h"
#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleUtil.h"
#include "physXVehicleSettings.h"

using namespace physx;


/*
* Vehicles: Tuning PhysX 4.1 SDK Guide:
* https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/Vehicles.html#tuning-guide
*/



DebugPanel::DebugPanel(GLFWwindow* window) {

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}


// Clean up ImGui
void DebugPanel::cleanUp() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void DebugPanel::draw() {

    using namespace ImGui;

    // New Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    // Create panel
    if (Begin("Debug Panel")) {
        
        // Colour edit
        if (CollapsingHeader("Colour")) {
            ColorEdit3("Clear color", (float*)&clear_color);
        }

        Spacing();
        if (CollapsingHeader("Steer Vs Forward Speed")) {

        }

        Spacing();
        if (CollapsingHeader("Key Smoothing Data")) {

        }

        Spacing();
        if (CollapsingHeader("Pad Smoothing Data")) {

        }

        Spacing();
        Separator();
        if (Button("Reload Data")) {
            updateValues = true;
        }

        // Display FPS
        Spacing();
        Separator();
        //Text("Application average %.3f ms/frame (%.1f FPS)",
          //  1000.f / GETIO().Framerate, GetIO().Framerate);

        End();
    }


    // 


    // Maximum steer as a function of speed
    // To easier control at higher speeds
    /*gSteerVsForwardSpeedData =
    {
        0.f,        0.75f,
        5.f,        0.75f,
        30.f,       0.125f,
        120.f,      0.1f,
        PX_MAX_F32, PX_MAX_F32,
        PX_MAX_F32, PX_MAX_F32,
        PX_MAX_F32, PX_MAX_F32,
        PX_MAX_F32, PX_MAX_F32
    };*/

    // Controls Smoothing Data
    /*gPadSmoothingData =
    {
        {
            6.f,    // rise rate eANALOG_INPUT_ACCEL
            6.f,    // rise rate eANALOG_INPUT_BRAKE
            12.f,   // rise rate eANALOG_INPUT_HANDBRAKE
            2.5f,   // rise rate eANALOG_INPUT_STEER_LEFT
            2.5f    // rise rate eANALOG_INPUT_STEER_RIGHT
        },
        {
            10.f,   // fall rate eANALOG_INPUT_ACCEL
            10.f,   // fall rate eANALOG_INPUT_BRAKE
            12.f,   // fall rate eANALOG_INPUT_HANDBRAKE
            5.f,    // fall rate eANALOG_INPUT_STEER_LEFT
            5.f     // fall rate eANALOG_INPUT_StEER_RIGHT
        }
    };*/
    gPadSmoothingData;


    // Render Panel
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());    
}
