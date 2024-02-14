#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
//#include "MeshModel.h"
//#include "vec.h"
//#include "mat.h"
#include "Scene.h"
//#include <string>
//#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
//#include <vector>
//#include <L2DFileDialog.h>

void FileMenu(Scene* scene);
void ImguiInit(GLFWwindow* window);
void ImguiFrame();
void ImguiCleanUP();
void MainMenuBar(Scene* scene);
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods, Scene* scene);
void ImguiPopUps(Scene* scene);
void transformModel(Scene* scene);
void transformCamera(Scene* scene);
void addCamera(Scene* scene);
void showMatriceValues(Scene* scene);
void showInstructions();
void showManual();