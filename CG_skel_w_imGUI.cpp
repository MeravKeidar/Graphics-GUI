// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "CG_skel_w_imGUI.h"

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
#include "Renderer.h"
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <iostream>
#include <L2DFileDialog.h>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))


bool DISPLAY_VERTEX_NORMAL = false;
bool DISPLAY_FACE_NORMAL = false;
bool DISPLAY_CAMERAS = false;


Scene* scene;
Renderer* renderer;


/// ImGUI Menus 
static char* file_dialog_buffer = nullptr;
static char path[500] = "";
static bool open_file_dialog = false;
void pushVec(vector<GLfloat>* triangle, vec4 vec)
{
	for (size_t i = 0; i < 4; i++)
	{
		triangle->push_back(vec[i]);
	}

}

void FileMenu() {
	FileDialog::file_dialog_open = true;
	FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
	if (FileDialog::file_dialog_open) {
		file_dialog_buffer = path;
		FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
		FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
	
		file_dialog_buffer = nullptr;
		FileDialog::file_dialog_open = false; // Close the file dialog
	}
}


static void MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{

		if (ImGui::BeginMenu("Load File"))
		{
			//FileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load Primative"))
		{
			if (ImGui::MenuItem("Pyramid")) {
				//scene->loadOBJModel("C:/Users/user/source/repo/GraphicsGui/obj_files/pyramid.obj");
				scene->loadPrimModel("pyramid");
			}
			//TODO:  add more primatives 
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Normal")) {
			if (ImGui::MenuItem("Display Normal-per-vertex")) {
				DISPLAY_VERTEX_NORMAL = true;
			}
			if (ImGui::MenuItem("Hide Normal-per-vertex")) {
				DISPLAY_VERTEX_NORMAL = false;
			}
			if (ImGui::MenuItem("Display Normal-per-face")) {
				DISPLAY_FACE_NORMAL = true;
			}
			if (ImGui::MenuItem("Hide Normal-per-face")) {
				DISPLAY_FACE_NORMAL = false;
			}
			ImGui::EndMenu();
			
		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("Add Camera")) {
				// pop up 
				//scene->addCamera(
			}
			if (ImGui::MenuItem("Look AT")) {

			}
			if (ImGui::MenuItem("Change Active Camera")) {

			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
		
	}
}



int my_main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	// Create a GLFWwindow of 512 by 512 pixels, named "CG"
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


	// Setup scene
	//TODO danger danger danger
	
	scene = new Scene();
	MeshModel temp_model;
	scene->addModel(&temp_model);

	
	//Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << error << std::endl;
		}

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//TODO: not sure this is the right approach to refresh buffers
		//renderer->ClearColorBuffer();
		
		//std::cout << "window width " << ImGui::GetMainViewport()->Size.x << std::endl;
		scene->draw();



		//renderer->DrawTriangles(&post_viewport_mat);
		
		//renderer->SwapBuffers();
		MainMenuBar();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
		
		glfwSwapBuffers(window);
		
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	delete scene;
	delete renderer;

	return 0;
}



int main(int argc, char *argv[]) {
		return my_main();
}


