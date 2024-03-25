// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//
//#include <L2DFileDialog.h>
//#include "CG_skel_w_imGUI.h"
#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "MeshModel.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include  "imgui_main.h"
#include <vector>
#include <iostream>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

Scene* scene;


// keyboard callback
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {keyboard(window, key, scancode, action, mods, scene);}

int my_main() {
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
	glfwSwapInterval(1); 

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		return -1;
	}
	
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	scene = new Scene();
	scene->programID = program;
	ImguiInit(window);
	glfwSetKeyCallback(window, keyboardCallback);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << error << std::endl;
		}
		int framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		glViewport(0, 0, min(framebufferWidth,framebufferHeight), min(framebufferWidth, framebufferHeight));
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		ImguiFrame();
		scene->draw();
		MainMenuBar(scene);
		ImguiPopUps(scene);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImguiCleanUP();
	glfwDestroyWindow(window);
	glfwTerminate();
	delete scene;

	return 0;
}



int main(int argc, char *argv[]) {
		return my_main();
}


