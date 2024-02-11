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
			scene->RotateModel(1, 10);
			std::cout << "rotate by y" << std::endl;
		}
		else if (key == GLFW_KEY_DOWN && mods == GLFW_MOD_CONTROL) {
			scene->RotateModel(2, 10);
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


bool transform_model = false;

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
				if (scene->activeCamera < (scene->nCameras - 1))
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
			if (ImGui::MenuItem("Transforn Active Model")) {
				transform_model = true;
			}
			if (ImGui::MenuItem("Change Active Model")) {
				if (scene->activeModel < (scene->nModels - 1))
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

void ImguiPopUps(Scene* scene) 
{

	if (transform_model)
	{
		ImGui::OpenPopup("Transform Active Model");
		transform_model = false;
	}

	if (ImGui::BeginPopupModal("Transform Active Model", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		transformModel(scene);
		ImGui::EndPopup();
	}
}

void transformModel(Scene* scene)
{
	static GLfloat x_w = 0;
	static GLfloat y_w = 0;
	static GLfloat z_w = 0;
	ImGui::Text("Translate in world frame");
	ImGui::SliderFloat("X World", &x_w, -0.2, 0.2);
	ImGui::SliderFloat("Y World", &y_w, -0.2, 0.2);
	ImGui::SliderFloat("Z World", &z_w, -0.2, 0.2);

	static GLfloat x_m = 0;
	static GLfloat y_m = 0;
	static GLfloat z_m = 0;
	ImGui::Separator();
	ImGui::Text("Translate in model frame ");
	ImGui::SliderFloat("X Model", &x_m, -0.2, 0.2);
	ImGui::SliderFloat("Y Model", &y_m, -0.2, 0.2);
	ImGui::SliderFloat("Z Model", &z_m, -0.2, 0.2);

	
	static GLfloat theta_w = 0;
	static int idx_w = 0;
	const char* hinge[] = { "X", "Y", "Z" };
	ImGui::Separator();
	ImGui::Text("Rotate in world frame ");
	ImGui::SliderFloat("Theta W", &theta_w, 0, 360);
	ImGui::Combo("World Axis", &idx_w, hinge, 3);
	static GLfloat theta_m = 0;
	static int idx_m = 0;
	ImGui::Separator();
	ImGui::Text("Rotate in model frame ");
	ImGui::SliderFloat("Theta M", &theta_m, -0, 360);
	ImGui::Combo("Model Axis", &idx_m, hinge, 3);

	static GLfloat s_x = 1;
	static GLfloat s_y = 1;
	static GLfloat s_z = 1;
	static GLfloat s = 1;
	ImGui::Text("Scale");
	ImGui::SliderFloat("Scale x", &s_x, 0.2, 5);
	ImGui::SliderFloat("Scale y", &s_y, 0.2, 5);
	ImGui::SliderFloat("Scale z", &s_y, 0.2, 5);
	ImGui::SliderFloat("Scale All", &s,0.2 ,5);
	
	if (ImGui::Button("OK"))
	{
		scene->moveModel(x_w, y_w, z_w, 1);
		scene->moveModel(x_m, y_m, z_m, 0);
		scene->RotateModel(idx_w, theta_w, 1);
		scene->RotateModel(idx_m, theta_m,0);
		scene->scaleModel(s_x, s_y, s_z);
		scene->scaleModel(s,s,s);
		x_w = 0;
		y_w = 0;
		z_w = 0;
		x_m = 0;
		y_m = 0;
		z_m = 0;
		theta_m = 0;
		theta_w = 0;
		idx_m = 0;
		idx_w = 0;
		s_x = 1;
		s_y = 1;
		s_z = 1;
		s = 1;
		ImGui::CloseCurrentPopup();
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

