#include "DebugPanel.h"
#include <iostream>


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

    enabled = true;

    // New Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create panel
    ImGui::Begin("Debug Panel");
    //ImGui::Text("Hello, World %d", 123);
    ImGui::Checkbox("checked", &updateValues);
    ImGui::End();

    // Render Panel
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());    
}
