#include "DebugPanel.h"
#include <iostream>


DebugPanel::DebugPanel(GLFWwindow* window) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}


void DebugPanel::cleanUp() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    std::cout << "CLEAN UP IMGUI" << std::endl;
}


void DebugPanel::newFrame() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

}


void DebugPanel::draw() {

    enabled = true;

    // Create panel
    ImGui::Begin("Debug Panel");
    ImGui::Text("Hello, World %d", 123);
    ImGui::End();

    // Render Panel
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    std::cout << "DEBUG MODE" << std::endl;
}

void DebugPanel::disable() {
    enabled = false;
}

void DebugPanel::update() {

}