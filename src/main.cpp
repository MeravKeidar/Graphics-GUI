#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
//#include "CG_skel_w_imGUI.h"

// Texture dimensions
const int TEXTURE_WIDTH = 512;
const int TEXTURE_HEIGHT = 512;
GLuint drawingTexture;
std::vector<unsigned char> pixels(TEXTURE_WIDTH* TEXTURE_HEIGHT * 4, 0); // RGBA format

void initializeTexture() {
    glGenTextures(1, &drawingTexture);
    glBindTexture(GL_TEXTURE_2D, drawingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}



int not_main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(512, 512, "CG", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize the drawing texture
    initializeTexture();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // If the ImGui canvas is hovered and the mouse is clicked, draw pixels
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 canvasPos = ImGui::GetWindowPos();

            // Correctly scale mouse coordinates to texture coordinates
            float scaleX = static_cast<float>(TEXTURE_WIDTH) / ImGui::GetWindowWidth();
            float scaleY = static_cast<float>(TEXTURE_HEIGHT) / ImGui::GetWindowHeight();
            int texX = static_cast<int>((mousePos.x - canvasPos.x) * scaleX);
            int texY = static_cast<int>((mousePos.y - canvasPos.y) * scaleY);

            // Check bounds and update pixels if within the canvas
            if (texX >= 0 && texY >= 0 && texX < TEXTURE_WIDTH && texY < TEXTURE_HEIGHT) {
                int index = (texY * TEXTURE_WIDTH + texX) * 4;
                pixels[index] = 255;     // R
                pixels[index + 1] = 255; // G
                pixels[index + 2] = 255; // B
                pixels[index + 3] = 255; // A

                // Update texture
                glBindTexture(GL_TEXTURE_2D, drawingTexture);
                glTexSubImage2D(GL_TEXTURE_2D, 0, texX, texY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[index]);
            }
        }

        // Display the texture in the ImGui window
        ImGui::Image((void*)(intptr_t)drawingTexture, ImVec2(TEXTURE_WIDTH, TEXTURE_HEIGHT));

        ImGui::End(); // End Canvas Window

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
