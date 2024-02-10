#include  "imgui_main.h"
#include <L2DFileDialog.h>

// keyboard callback
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods, Scene* scene) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_EQUAL && mods == GLFW_MOD_CONTROL) {
			scene->zoom(1.5);
			std::cout << "Zoom In" << std::endl;
		}
		else if (key == GLFW_KEY_MINUS && mods == GLFW_MOD_CONTROL) {
			scene->zoom(1 / 1.5);
			std::cout << "Zoom Out" << std::endl;
		}

		else if (key == GLFW_KEY_MINUS && mods == GLFW_MOD_CONTROL) {
			scene->zoom(1 / 1.5);
			std::cout << "Zoom Out" << std::endl;
		}
		else if (key == GLFW_KEY_RIGHT && mods == GLFW_MOD_CONTROL) {
			scene->moveModel(1, 0, 0);
			std::cout << "Move Right" << std::endl;
		}
		else if (key == GLFW_KEY_LEFT && mods == GLFW_MOD_CONTROL) {
			scene->moveModel(-1, 0, 0);
			std::cout << "Move left" << std::endl;
		}
		else if (key == GLFW_KEY_UP && mods == GLFW_MOD_CONTROL) {
			scene->RotateModel('y', 10);
			std::cout << "rotate by y" << std::endl;
		}
		else if (key == GLFW_KEY_DOWN && mods == GLFW_MOD_CONTROL) {
			scene->RotateModel('z', 10);
			std::cout << "rotate by z" << std::endl;
		}
	}
}


bool file_uploaded = false;
static char* file_dialog_buffer = new char[500];
static char path[500] = "";
void FileMenu(Scene* scene)
{
	if (ImGui::Button("browse file")) {
		file_dialog_buffer = path;
		FileDialog::file_dialog_open = true;
		FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
		std::cout << "file" << file_dialog_buffer << std::endl;
	}
	if (FileDialog::file_dialog_open) {
			FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
			file_uploaded = true; 
	}
	
}

void MainMenuBar(Scene* scene)
{
	
	/*static char* file_dialog_buffer = new char[500];
	static char path[500] = "";*/
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Load model"))
		{
			FileMenu(scene);
			if (ImGui::MenuItem("file")) {
				if (file_uploaded) {
					scene->loadOBJModel(string(file_dialog_buffer));
				}
				else {
					std::cout << "please browse file first" << file_dialog_buffer << std::endl;
				}
			}
			if (ImGui::MenuItem("Pawn")) {
				std::cout << "Paw" << file_dialog_buffer << std::endl;
				scene->loadOBJModel("obj_files/pawn.obj");
			}
			if (ImGui::MenuItem("Tetrahedron")) {
				scene->loadOBJModel("obj_files/tetrahedron.obj");
			}
			if (ImGui::MenuItem("Cow")) {
				scene->loadOBJModel("obj_files/cow.obj");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Normal")) {
			if (ImGui::MenuItem("Display Normal-per-vertex")) {
				scene->displayVnormal = true;
			}
			if (ImGui::MenuItem("Hide Normal-per-vertex")) {
				scene->displayVnormal = false;
			}
			if (ImGui::MenuItem("Display Normal-per-face")) {
				scene->displayFnormal = true;
			}
			if (ImGui::MenuItem("Hide Normal-per-face")) {
				scene->displayFnormal = false;
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
				if (scene->activeCamera < (scene->nCameras() - 1))
				{
					scene->activeCamera++;
				}
				else
				{
					scene->activeCamera = 0;
				}
			}
			if (ImGui::MenuItem("Display cameras")) {
				scene->displayCameras;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Transform Model")) {
				// pop up 
		
			}
			
			if (ImGui::MenuItem("Change Active Model")) {
				if (scene->activeModel < (scene->nModels() - 1))
				{
					scene->activeModel++;
				}
				else
				{
					scene->activeModel = 0;
				}
			}
			if (ImGui::MenuItem("Display Bounding box")) {
				scene->displayBoundingBox = true;
			}
			ImGui::EndMenu();
			

		}

		ImGui::EndMainMenuBar();

	}
}

void ImguiInit(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	
}


void ImguiFrame() 
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void ImguiCleanUP() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}