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



DebugPanel::DebugPanel(GLFWwindow* window, State* state) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    this->state = state;

    // Initialize Driving Model initial values
    speed0 = &gSteerVsForwardSpeedData[1];
    speed5 = &gSteerVsForwardSpeedData[3];
    speed30 = &gSteerVsForwardSpeedData[5];
    speed120 = &gSteerVsForwardSpeedData[7];

    for (int i= 0; i < 5; i++)
        key_riseRates[i] = &gKeySmoothingData.mRiseRates[i];
    for (int i = 0; i < 5; i++)
        key_fallRates[i] = &gKeySmoothingData.mFallRates[i];

    for (int i = 0; i < 5; i++)
        pad_riseRates[i] = &gPadSmoothingData.mRiseRates[i];
    for (int i = 0; i < 5; i++)
        pad_fallRates[i] = &gPadSmoothingData.mFallRates[i];
}


// Clean up ImGui
void DebugPanel::cleanUp() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void DebugPanel::draw(Player& player) {

    using namespace ImGui;

    // New Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    // Create panel
    if (Begin("Car Model Debug Panel")) {
        
        // Colour edit
        if (CollapsingHeader("Colour")) {
            ColorEdit3("Color", (float*)&color);
            if (Button("Apply")) {
                updateValues = true;
            }
        }

        if (Button("TOGGLE DEBUG MODE")) {
            enableDebug = !enableDebug;
        }
        
        if (Button("ADD CASH")) {
            player.addCash(10000);
        }

   

        Spacing();
        if (CollapsingHeader("Steer Vs Forward Speed")) {
            SliderFloat("Speed 0", speed0, speed_range[0], speed_range[1]);
            SliderFloat("Speed 5", speed5, speed_range[0], speed_range[1]);
            SliderFloat("Speed 30", speed30, speed_range[0], speed_range[1]);
            SliderFloat("Speed 120", speed120, speed_range[0], speed_range[1]);
        }

        Spacing();
        if (CollapsingHeader("Key Smoothing Data")) {
            SliderFloat("Acceleration Rise Rate", key_riseRates[0], rates_range[0], rates_range[1]);
            SliderFloat("Brake Rise Rate", key_riseRates[1], rates_range[0], rates_range[1]);
            SliderFloat("HandBrake Rise Rate", key_riseRates[2], rates_range[0], rates_range[1]);
            SliderFloat("Steer Left Rise Rate", key_riseRates[3], rates_range[0], rates_range[1]);
            SliderFloat("Steer Right Rise Rate", key_riseRates[4], rates_range[0], rates_range[1]);
            Spacing();
            SliderFloat("Acceleration Fall Rate", key_fallRates[0], rates_range[0], rates_range[1]);
            SliderFloat("Brake Fall Rate", key_fallRates[1], rates_range[0], rates_range[1]);
            SliderFloat("HandBrake Fall Rate", key_fallRates[2], rates_range[0], rates_range[1]);
            SliderFloat("Steer Left Fall Rate", key_fallRates[3], rates_range[0], rates_range[1]);
            SliderFloat("Steer Right Fall Rate", key_fallRates[4], rates_range[0], rates_range[1]);
        }

        Spacing();
        if (CollapsingHeader("Pad Smoothing Data")) {
            SliderFloat("Acceleration Rise Rate", pad_riseRates[0], rates_range[0], rates_range[1]);
            SliderFloat("Brake Rise Rate", pad_riseRates[1], rates_range[0], rates_range[1]);
            SliderFloat("HandBrake Rise Rate", pad_riseRates[2], rates_range[0], rates_range[1]);
            SliderFloat("Steer Left Rise Rate", pad_riseRates[3], rates_range[0], rates_range[1]);
            SliderFloat("Steer Right Rise Rate", pad_riseRates[4], rates_range[0], rates_range[1]);
            Spacing();
            SliderFloat("Acceleration Fall Rate", pad_fallRates[0], rates_range[0], rates_range[1]);
            SliderFloat("Brake Fall Rate", pad_fallRates[1], rates_range[0], rates_range[1]);
            SliderFloat("HandBrake Fall Rate", pad_fallRates[2], rates_range[0], rates_range[1]);
            SliderFloat("Steer Left Fall Rate", pad_fallRates[3], rates_range[0], rates_range[1]);
            SliderFloat("Steer Right Fall Rate", pad_fallRates[4], rates_range[0], rates_range[1]);
        }

        // Display Player Position
        Spacing();
        if (CollapsingHeader("Player Position")) {
            glm::vec3 pos = player.getPos();
            Text("X: %.3f", pos.x);
            Text("Y: %.3f", pos.y);
            Text("Z: %.3f", pos.z);
        }

        // Display FPS
        Spacing();
        Separator();
        Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.f / GetIO().Framerate, GetIO().Framerate);

        End();
    }


    // Render Panel
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());    
}
