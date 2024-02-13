#include  "imgui_main.h"
#include <L2DFileDialog.h>

// keyboard callback
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods, Scene* scene) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_EQUAL && mods == GLFW_MOD_CONTROL) {
			scene->cameras.at(scene->activeCamera)->Zoom(0.1);
		}
		else if (key == GLFW_KEY_MINUS && mods == GLFW_MOD_CONTROL) {
			scene->cameras.at(scene->activeCamera)->Zoom(-0.1);
		}
		else if (key == GLFW_KEY_RIGHT && mods == GLFW_MOD_CONTROL) {
			scene->moveModel(1, 0, 0);
		}
		else if (key == GLFW_KEY_LEFT && mods == GLFW_MOD_CONTROL) {
			scene->moveModel(-1, 0, 0);
			
		}
		else if (key == GLFW_KEY_UP && mods == GLFW_MOD_CONTROL) {
			scene->moveModel(0, 0, -1);

		}
		else if (key == GLFW_KEY_DOWN && mods == GLFW_MOD_CONTROL) {
			scene->moveModel(0, 0, 1);
		}
	}
}


bool file_uploaded = false;
static char* file_dialog_buffer = new char[500];
static char path[500] = "";
void FileMenu(Scene* scene)
{
	if (ImGui::Button("Upload file")) {
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
bool transform_camera = false;
bool add_camera = false;
bool show_matrices = false;

void MainMenuBar(Scene* scene)
{
	
	/*static char* file_dialog_buffer = new char[500];
	static char path[500] = "";*/
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Load model"))
		{
			FileMenu(scene);
			if (ImGui::MenuItem("Add uploaded model")) {
				if (file_uploaded) {
					scene->loadOBJModel(string(file_dialog_buffer));
				}
				else {
					std::cout << "please load file first" << file_dialog_buffer << std::endl;
				}
			}
			if (ImGui::MenuItem("Pawn")) {
				std::cout << "Paw" << file_dialog_buffer << std::endl;
				scene->loadOBJModel("obj_files/pawn.obj");
			}
			if (ImGui::MenuItem("Tetrahedron")) {
				scene->loadPrimModel("tetrahedron");
			}
			if (ImGui::MenuItem("Cow")) {
				scene->loadOBJModel("obj_files/cow.obj");
			}
			if (ImGui::MenuItem("Box")) {
				scene->loadOBJModel("obj_files/box.obj");
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scene")) {
			if (ImGui::MenuItem("Normal-per-vertex")) {
				if (scene->displayVnormal)
					scene->displayVnormal = false;
				else
					scene->displayVnormal = true;
			}
			if (ImGui::MenuItem("Normal-per-face")) {
				if (scene->displayFnormal)
					scene->displayFnormal = false;
				else
					scene->displayFnormal = true;
			}
			if (ImGui::MenuItem("Reset scene"))
			{
				scene->Reset(); 
			}

			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("Transform Active Camera")) {
				transform_camera = true;
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
				if(scene->displayCameras)
					scene->displayCameras = false;
				else
					scene->displayCameras = true;
				
			}
			if (ImGui::MenuItem("Add Camera")) {
				add_camera = true;
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
			if (ImGui::MenuItem("Look at active model")) {
				scene->LookAtModel();
			}
			ImGui::EndMenu();
			
		}
		if (ImGui::BeginMenu("Show Mat Values"))
		{
			show_matrices = true;
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

	if (transform_camera)
	{
		ImGui::OpenPopup("Transform Active Camera");
		transform_camera = false;
	}

	if (ImGui::BeginPopupModal("Transform Active Camera", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		transformCamera(scene);
		ImGui::EndPopup();
	}

	if (add_camera)
	{
		ImGui::OpenPopup("Add Camera");
		add_camera = false;
	}
	if (ImGui::BeginPopupModal("Add Camera", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		addCamera(scene);
		ImGui::EndPopup();
	}
	if (show_matrices)
	{
		ImGui::OpenPopup("Show Mat Values");
		show_matrices = false;
	}
	if (ImGui::BeginPopupModal("Show Mat Values", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		showMatriceValues(scene);
		ImGui::EndPopup();
	}
}

void transformCamera(Scene* scene)
{
	static GLfloat x_w = 0;
	static GLfloat y_w = 0;
	static GLfloat z_w = 0;
	ImGui::Text("Translate in world frame");
	ImGui::SliderFloat("X World", &x_w, -0.5, 0.5);
	ImGui::SliderFloat("Y World", &y_w, -0.5, 0.5);
	ImGui::SliderFloat("Z World", &z_w, -0.5, 0.5);

	static GLfloat x_v = 0;
	static GLfloat y_v = 0;
	static GLfloat z_v = 0;
	ImGui::Separator();
	ImGui::Text("Translate in view frame ");
	ImGui::SliderFloat("X view", &x_v, -0.5, 0.5);
	ImGui::SliderFloat("Y view", &y_v, -0.5, 0.5);
	ImGui::SliderFloat("Z view", &z_v, -0.5, 0.5);


	static GLfloat theta_w = 0;
	static int idx_w = 0;
	const char* hinge[] = { "X", "Y", "Z" };
	ImGui::Separator();
	ImGui::Text("Rotate in world frame ");
	ImGui::SliderFloat("Theta W", &theta_w, 0, 360);
	ImGui::Combo("World Axis", &idx_w, hinge, 3);
	static GLfloat theta_v = 0;
	static int idx_v = 0;
	ImGui::Separator();
	ImGui::Text("Rotate in view frame ");
	ImGui::SliderFloat("Theta V", &theta_v, -0, 360);
	ImGui::Combo("View Axis", &idx_v, hinge, 3);

	ImGui::Separator();
	ImGui::Text("Camera Position and Orientation");

	static bool useLookAt = false; // Checkbox state
	ImGui::Checkbox("Use LookAt", &useLookAt); // Checkbox for enabling LookAt

	static GLfloat eye[3] = { 0.0f, 0.0f, 0.0f };
	static GLfloat at[3] = { 0.0f, 0.0f, -1.0f }; // Adjusted default looking towards -Z
	static GLfloat up[3] = { 0.0f, 1.0f, 0.0f };  // Adjusted default up direction
	if (useLookAt) { // Only display these inputs if the checkbox is checked
		ImGui::InputFloat3("Eye", eye);
		ImGui::InputFloat3("At", at);
		ImGui::InputFloat3("Up", up);
	}
	static GLfloat left_right[2] = { -1.0f, 1.0f };
	static GLfloat bottom_top[2] = { -1.0f, 1.0f };
	static GLfloat near_far[2] = { 1.0f, -1.0f };
	static GLfloat fovy[2] = { 90.0f, 1.0f };
	static int perspective_type = 0;
	static int input_i = 0;
	static bool changePerspective = false; // Checkbox state
	ImGui::Checkbox("Change Perspective", &changePerspective); // Checkbox for enabling LookAt
	if (changePerspective)
	{
		ImGui::Text("Camera Perspective");

		
		const char* input_type[] = { "Coordinal", "FOV" };
		const char* perspective[] = { "Ortho", "Frustum" };
		ImGui::Combo("Perspective type", &perspective_type, perspective, 2);
		
		
		if (perspective_type == 0)
		{
			ImGui::InputFloat2("left/right", left_right);
			ImGui::InputFloat2("bottom/top", bottom_top);
			ImGui::InputFloat2("znear/zfar", near_far);
		}
		else
		{


			ImGui::Combo("Input Type", &input_i, input_type, 2);
			if (input_i == 0)
			{
				ImGui::InputFloat2("left/right", left_right);
				ImGui::InputFloat2("bottom/top", bottom_top);
				ImGui::InputFloat2("znear/zfar", near_far);
			}
			else
			{
				ImGui::InputFloat2("fovy/aspect", fovy);
				ImGui::InputFloat2("znear/zfar", near_far);
			}


		}
	}
	

	if (ImGui::Button("OK"))
	{
		
		if (useLookAt) { // Only call LookAtCurrentCamera if the checkbox was checked
			vec4 new_eye(eye[0], eye[1], eye[2], 0);
			vec4 new_at(at[0], at[1], at[2], 0);
			vec4 new_up(up[0], up[1], up[2], 0);
			scene->cameras.at(scene->activeCamera)->LookAt(new_eye,new_at,new_up);
		}
		scene->moveCamera(x_v, y_v, z_v, 0);
		scene->moveCamera(x_w, y_w, z_w, 1);
		scene->RotateCamera(idx_v, theta_v, 0);
		scene->RotateCamera(idx_w, theta_w, 1);
		x_v = 0; y_v = 0; z_v = 0; theta_v = 0; idx_v = 0; // Reset transformations
		x_w = 0; y_w = 0; z_w = 0; theta_w = 0; idx_w = 0; // Reset transformations

		if (changePerspective)
		{
			if (perspective_type == 0)
			{
				scene->cameras.at(scene->activeCamera)->Ortho(
					left_right[0], left_right[1], bottom_top[0], bottom_top[1], near_far[0], near_far[1]);
				//scene->setCameraOrtho(left_right[0], left_right[1], bottom_top[0], bottom_top[1], near_far[0], near_far[1]);
			}
			else
			{
				if (input_i == 0)
				{
					scene->cameras.at(scene->activeCamera)->Frustum(
						left_right[0], left_right[1], bottom_top[0], bottom_top[1], near_far[0], near_far[1]);
					//scene->setCameraFrustum(left_right[0], left_right[1], bottom_top[0], bottom_top[1], near_far[0], near_far[1]);
				}
				else
				{
					scene->cameras.at(scene->activeCamera)->Perspective(fovy[0], fovy[1], near_far[0], near_far[1]);
					//scene->setCameraPerspective(fovy[0], fovy[1], near_far[0], near_far[1]);
				}
			}
		}/*
		 left_right[0] =-1.0f;
		 left_right[1] =  1.0f;
		 bottom_top[0] = -1.0f ;
		 bottom_top[1] =  1.0f ;
		 near_far[0] =  -1.0f;
		 near_far[1] =  1.0f;
		 fovy[0] =  90.0f;
		 fovy[1] =  1.0f;*/
		ImGui::CloseCurrentPopup();
	}



}

void addCamera(Scene* scene)
{

	ImGui::Text("Camera Perspective");


	static int idx = 0;
	static int input_i = 0;
	const char* input_type[] = { "Coordinal", "FOV" };
	const char* perspective[] = { "Ortho", "Frustum"};
	ImGui::Combo("Perspective type", &idx, perspective, 2);
	static GLfloat left_right[2] = {-1.0f, 1.0f};
	static GLfloat bottom_top[2] = { -1.0f, 1.0f};
	static GLfloat near_far[2] = { -1.0f, 1.0f};
	static GLfloat fovy[2] = {90.0f, 1.0f };
	if (idx == 0)
	{
		ImGui::InputFloat2("left/right", left_right);
		ImGui::InputFloat2("bottom/top", bottom_top);
		ImGui::InputFloat2("znear/zfar", near_far);
	}
	else
	{
		
		
		ImGui::Combo("Input Type", &input_i, input_type, 2);
		if (input_i == 0)
		{
			ImGui::InputFloat2("left/right", left_right);
			ImGui::InputFloat2("bottom/top", bottom_top);
			ImGui::InputFloat2("znear/zfar", near_far);
		}
		else
		{
			ImGui::InputFloat2("fovy/aspect", fovy);
			ImGui::InputFloat2("znear/zfar", near_far);
		}


	}
		

	static GLfloat eye[3] = { 0.0f, 0.0f, 0.0f };
	static GLfloat at[3] = { 0.0f, 0.0f, -1.0f }; // Adjusted default looking towards -Z
	static GLfloat up[3] = { 0.0f, 1.0f, 0.0f };  // Adjusted default up direction
	
	ImGui::InputFloat3("Eye", eye);
	ImGui::InputFloat3("At", at);
	ImGui::InputFloat3("Up", up);


	if (ImGui::Button("OK"))
	{
		//scene->addCamera(eye, at, up);
		scene->addCamera(vec4(eye[0], eye[1], eye[2], 0),
		vec4(at[0], at[1], at[2], 0),
		vec4(up[0], up[1], up[2], 0));
		//ortho
		if (idx == 0)
		{
			scene->setCameraOrtho(left_right[0], left_right[1], bottom_top[0], bottom_top[1], near_far[0], near_far[1]);
		}
		else
		{
			if (input_i == 0)
			{
				scene->setCameraFrustum(left_right[0], left_right[1], bottom_top[0], bottom_top[1], near_far[0], near_far[1]);
			}
			else
			{
				scene->setCameraPerspective(fovy[0], fovy[1], near_far[0], near_far[1]);
			}
		}
		ImGui::CloseCurrentPopup();
	}
}


void transformModel(Scene* scene)
{
	static GLfloat x_w = 0;
	static GLfloat y_w = 0;
	static GLfloat z_w = 0;
	ImGui::Text("Translate in world frame");
	ImGui::SliderFloat("X World", &x_w, -0.5, 0.5);
	ImGui::SliderFloat("Y World", &y_w, -0.5, 0.5);
	ImGui::SliderFloat("Z World", &z_w, -0.5, 0.5);

	static GLfloat x_m = 0;
	static GLfloat y_m = 0;
	static GLfloat z_m = 0;
	ImGui::Separator();
	ImGui::Text("Translate in model frame ");
	ImGui::SliderFloat("X Model", &x_m, -0.5, 0.5);
	ImGui::SliderFloat("Y Model", &y_m, -0.5, 0.5);
	ImGui::SliderFloat("Z Model", &z_m, -0.5, 0.5);

	
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
	ImGui::SliderFloat("Scale z", &s_z, 0.2, 5);
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

void showMatriceValues(Scene* scene)
{
	ImGui::Text("Current Model Transform Mat");
	mat4 m = scene->getCurrentModelTrasform();
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[0][0], m[0][1], m[0][2], m[0][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[1][0], m[1][1], m[1][2], m[1][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[2][0], m[2][1], m[2][2], m[2][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[3][0], m[3][1], m[3][2], m[3][3]);

	ImGui::Text("Current World Transform Mat");
	 m = scene->getCurrentWorldTrasform();
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[0][0], m[0][1], m[0][2], m[0][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[1][0], m[1][1], m[1][2], m[1][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[2][0], m[2][1], m[2][2], m[2][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[3][0], m[3][1], m[3][2], m[3][3]);

	ImGui::Text("Current Camera Transform Mat");
	 m = scene->getCurrentCameraTrasform();
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[0][0], m[0][1], m[0][2], m[0][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[1][0], m[1][1], m[1][2], m[1][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[2][0], m[2][1], m[2][2], m[2][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[3][0], m[3][1], m[3][2], m[3][3]);

	ImGui::Text("Current Projection Mat");
	m = scene->getCurrentProjection();
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[0][0], m[0][1], m[0][2], m[0][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[1][0], m[1][1], m[1][2], m[1][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[2][0], m[2][1], m[2][2], m[2][3]);
	ImGui::Text("%4.2f,%4.2f,%4.2f,%4.2f", m[3][0], m[3][1], m[3][2], m[3][3]);

	ImGui::Text("Current eye, at , up");
	ImGui::Text("eye: %4.2f ,%4.2f,%4.2f,%4.2f", scene->cameras.at(scene->activeCamera)->eye.x, scene->cameras.at(scene->activeCamera)->eye.y, scene->cameras.at(scene->activeCamera)->eye.z);
	ImGui::Text("at: %4.2f,%4.2f,%4.2f,%4.2f", scene->cameras.at(scene->activeCamera)->at.x, scene->cameras.at(scene->activeCamera)->at.y, scene->cameras.at(scene->activeCamera)->at.z);
	ImGui::Text("up: %4.2f,%4.2f,%4.2f,%4.2f", scene->cameras.at(scene->activeCamera)->up.x, scene->cameras.at(scene->activeCamera)->up.y, scene->cameras.at(scene->activeCamera)->up.z);
	if (ImGui::Button("OK"))
	{
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

