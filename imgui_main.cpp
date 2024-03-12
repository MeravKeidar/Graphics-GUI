#include  "imgui_main.h"
#include <L2DFileDialog.h>


void showInstructions()
{
	ImGui::Text("Shortcuts");
	ImGui::Text("Zoom in: Cntrl + '+' ");
	ImGui::Text("Zoom out: Cntrl + '-' ");
	ImGui::Text("");
	ImGui::Text("Move Active Camera Left(in view space): C + Left Arrow ");
	ImGui::Text("Move Active Camera Right(in view space): C + Right Arrow ");
	ImGui::Text("Move Active Camera Down(in view space): C + Down Arrow ");
	ImGui::Text("Move Active Camera Up( in view space): C + Up Arrow ");
	ImGui::Text("Look At Current Model(center camera): Cntrl + 'L' ");
	ImGui::Text("");
	ImGui::Text("Move Active Model Left( in -x): X + Left Arrow ");
	ImGui::Text("Move Active Model Right( in +x): X + Right Arrow ");
	ImGui::Text("Move Active Model Down( in -y): Y + Down Arrow ");
	ImGui::Text("Move Active Model Up( in +y): Y + Up Arrow ");
	ImGui::Text("Move Active Model far( in -z): Z + Down Arrow ");
	ImGui::Text("Move Active Model near( in +z): Z + Up Arrow ");
	ImGui::Text("");
	ImGui::Text("Scale Active Model : M + Plus/Minus Key ");

	if (ImGui::Button("OK"))
	{
		ImGui::CloseCurrentPopup();
	}
}

// keyboard callback
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods, Scene* scene) {

	//move model
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		scene->moveModel(0.3, 0, 0);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		scene->moveModel(-0.3, 0, 0);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		scene->moveModel(0,0.3, 0);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		scene->moveModel(0,-0.3, 0);
	}

	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		scene->moveModel(0,0, 0.3);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		scene->moveModel(0,0, -0.3);
	}
	//move camera
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		scene->moveCamera(-0.3, 0, 0, 0);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		scene->moveCamera(0.3, 0, 0, 0);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		scene->moveCamera(0, -0.3, 0, 0);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		scene->moveCamera(0, 0.3, 0, 0);
	}
	//scale model
	else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		scene->scaleModel(1.1, 1.1, 1.1);
	}
	else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		scene->scaleModel(0.9, 0.9, 0.9);
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_EQUAL && mods == GLFW_MOD_CONTROL) {
			scene->cameras.at(scene->activeCamera)->Zoom(0.1);
		}
		else if (key == GLFW_KEY_MINUS && mods == GLFW_MOD_CONTROL) {
			scene->cameras.at(scene->activeCamera)->Zoom(-0.1);
		}
		else if (key == GLFW_KEY_L && mods == GLFW_MOD_CONTROL) {
			scene->LookAtModel();
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
bool show_instructions = false;
bool display_manual = false;
bool add_light = false;
bool transform_lights = false;
bool change_material = false;
bool change_scene_parameters = false;

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
			if (ImGui::MenuItem("Custom Debug Model")) {
				scene->loadOBJModel("obj_files/custom.obj");
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scene")) {

			if (ImGui::MenuItem("Change Scene Parameters"))
			{
				change_scene_parameters = true;
			}
			if (ImGui::BeginMenu("Display Normals"))
			{
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
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Reset scene"))
			{
				scene->Reset(); 
			}

			if (ImGui::BeginMenu("Shading"))
			{
				if (ImGui::MenuItem("Flat"))
				{
					scene->changeShading(FLAT);
				}
				if (ImGui::MenuItem("Guoroad"))
				{
					scene->changeShading(GOURAUD);
				}
				if (ImGui::MenuItem("Phong"))
				{
					scene->changeShading(PHONG);
				}
				if (ImGui::MenuItem("Mesh"))
				{
					scene->changeShading(MESH);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Blur"))
			{
				if (scene->blur)
					scene->blur = false;
				else
					scene->blur = true;
			}

			
			ImGui::EndMenu();

		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("Transform Active Camera")) {
				transform_camera = true;
			}
			if (ImGui::MenuItem("Change Active Camera")) {
				if (scene->activeCamera < (scene->nCameras -1))
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

		if (ImGui::BeginMenu("Lights"))
		{
			if (ImGui::MenuItem("Add light")) {
				add_light = true;
			}
			
			if (ImGui::MenuItem("Transform Lights")) {
				if (scene->nLights != 0) 
				{
					transform_lights = true;
				}
			}
			if (ImGui::MenuItem("Change Active light")) {
				if (scene->activeLight < (scene->nLights - 1))
				{
					scene->activeLight++;
				}
				else
				{
					scene->activeLight = 0;
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Transforn Active Model")) {
				transform_model = true;
			}
			if (ImGui::MenuItem("Change Active Model")) {
				if (scene->activeModel < (scene->nModels-1))
				{
					scene->activeModel++;
				}
				else
				{
					scene->activeModel = 0;
				}
			}
			if (ImGui::MenuItem("Display Bounding box")) {
				if (scene->displayBoundingBox)
					scene->displayBoundingBox = false;
				else
					scene->displayBoundingBox = true;
				
			}
			if (ImGui::MenuItem("Look at active model")) {
				scene->LookAtModel();
			}

			if (ImGui::MenuItem("Change Model Material")) {
				change_material = true;
			}

			ImGui::EndMenu();
			
		}
		if (ImGui::BeginMenu("Show Mat Values"))
		{
			show_matrices = true;
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Keyboard Shortcuts"))
		{
			show_instructions = true;
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void scenePararmeters(Scene* scene)
{
	GLfloat a = scene->ambient_scale;
	ImGui::SliderFloat("Ambient Scale", &a, 0.0, 1.0);
	scene->ambient_scale = a;
	GLfloat s = scene->normal_scale;
	ImGui::SliderFloat("Normal Scale", &s, 0.0, 1.0);
	scene->normal_scale = s;

	if (ImGui::Button("Allow Fog"))
	{
		if (scene->m_renderer->fog)
			scene->m_renderer->fog = false;
		else
			scene->m_renderer->fog = true;
	}
	static int fog_type_idx = (scene->m_renderer->fog_type == EXP ? 0 : 1);
	const char* fog_type[] = { "exp", "linear" };
	ImGui::Combo("Light type", &fog_type_idx, fog_type, 2);
	if (fog_type_idx == 0) //exp
	{
		scene->m_renderer->fog_type = EXP;
		GLfloat fd = scene->m_renderer->fog_density;
		ImGui::SliderFloat("Fog density", &fd, 0.0, 1.0);
		scene->m_renderer->fog_density = fd;
	}
	if (fog_type_idx == 1) //linear
	{
		scene->m_renderer->fog_type = LINEAR;
		GLfloat fs = scene->m_renderer->fog_start;
		ImGui::SliderFloat("Fog starting distance", &fs, 0.0, 1.0);
		scene->m_renderer->fog_start = fs;
		GLfloat fe = scene->m_renderer->fog_end;
		ImGui::SliderFloat("Fog ending distance", &fe, 0.0, 1.0);
		scene->m_renderer->fog_end = fe;
	}
	
	

	static ImVec4 fc = ImVec4(0.2f, 0.2f, 0.2f, 0.2f);
	ImGui::ColorPicker4("Emissive Color Picker", (float*)&fc, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);
	scene->m_renderer->fog_color = Color(fc.x, fc.y, fc.z);
	
	int r = scene->m_renderer->antialiasing_resolution;
	ImGui::SliderInt("Anti-Aliasing Resolution", &r, 1, 4);
	scene->ChangeAntiAliasingResolution(r);
	if (ImGui::Button("OK"))
	{
		ImGui::CloseCurrentPopup();
	}
}

void ImguiPopUps(Scene* scene) 
{
	if (change_scene_parameters)
	{
		ImGui::OpenPopup("Change Scene parameters");
		change_scene_parameters = false;
	}
	if (ImGui::BeginPopupModal("Change Scene parameters", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		scenePararmeters(scene);
		ImGui::EndPopup();
	}

	if (display_manual)
	{
		ImGui::OpenPopup("Welcome Manual");
		display_manual = false;
	}
	if (ImGui::BeginPopupModal("Welcome Manual", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		showManual();
		ImGui::EndPopup();
	}

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
	if (show_instructions)
	{
		ImGui::OpenPopup("Keyboard Shortcuts");
		show_instructions = false;
	}
	if (ImGui::BeginPopupModal("Keyboard Shortcuts", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		showInstructions();
		ImGui::EndPopup();
	}
	if (transform_lights)
	{
		ImGui::OpenPopup("Transform Lights");
		transform_lights = false;
	}
	if (ImGui::BeginPopupModal("Transform Lights", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		transformLights(scene);
		ImGui::EndPopup();
	}
	if (add_light)
	{
		ImGui::OpenPopup("Add Light");
		add_light = false;
	}
	if (ImGui::BeginPopupModal("Add Light", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		addLight(scene);
		ImGui::EndPopup();
	}
	if (change_material)
	{
		ImGui::OpenPopup("Change Material");
		change_material = false;
	}
	if (ImGui::BeginPopupModal("Change Material", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		changeMaterial(scene);
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
		scene->addCamera(vec4(eye[0], eye[1], eye[2], 1),
		vec4(at[0], at[1], at[2], 1),
		vec4(up[0], up[1], up[2], 1));
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

void changeMaterial(Scene* scene)
{
	if (scene->nModels == 0)
		ImGui::CloseCurrentPopup();
	if (ImGui::Button("Set Color By Normal"))
	{
		scene->models.at(scene->activeModel)->colorByNormal();
	}
	if (ImGui::Button("Set Color By Position"))
	{
		scene->models.at(scene->activeModel)->colorByPosition();
	}
	static bool change_ambiant = false;
	ImGui::Checkbox("Change Ambiant Color", &change_ambiant);
	if (change_ambiant)
	{
		static ImVec4 ambient_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImGui::ColorPicker4("Ambiant Color Picker", (float*)&ambient_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);
		Color ambientColor(ambient_color.x, ambient_color.y, ambient_color.z);
		scene->models.at(scene->activeModel)->changeUniformColor(ambientColor);
	}
	
	static bool change_Diffuse = false;
	ImGui::Checkbox("Change Diffuse Color", &change_Diffuse);
	if (change_Diffuse)
	{
		static ImVec4 diffuse_color = ImVec4(0.2f, 0.2f, 0.2f, 0.2f);
		ImGui::ColorPicker4("Diffuse Color Picker", (float*)&diffuse_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);
		Color diffuseColor(diffuse_color.x, diffuse_color.y, diffuse_color.z);
		scene->models.at(scene->activeModel)->changeUniformDiffuseColor(diffuseColor);
	}

	static bool change_specular = false;
	ImGui::Checkbox("Change Specular Color", &change_specular);
	if (change_specular)
	{
		static ImVec4 specular_color = ImVec4(0.2f, 0.2f, 0.2f, 0.2f);
		ImGui::ColorPicker4("Specular Color Picker", (float*)&specular_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);
		Color specularColor(specular_color.x, specular_color.y, specular_color.z);
		scene->models.at(scene->activeModel)->changeUniformSpecularColor(specularColor);
	}

	static bool change_emissive = false;
	ImGui::Checkbox("Change Emissive Color", &change_emissive);
	if (change_emissive)
	{
		static ImVec4 emissive_color = ImVec4(0.2f, 0.2f, 0.2f, 0.2f);
		ImGui::ColorPicker4("Emissive Color Picker", (float*)&emissive_color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);
		Color emissiveColor(emissive_color.x, emissive_color.y, emissive_color.z);
		scene->models.at(scene->activeModel)->changeUniformEmissiveColor(emissiveColor);
	}

	 GLfloat shininess_slide = scene->models.at(scene->activeModel)->material.shininess_coefficient;
	ImGui::SliderFloat("Shininess coefficient", &shininess_slide, 0, 100);
	scene->models.at(scene->activeModel)->material.shininess_coefficient = shininess_slide;
	GLfloat shininess = scene->models.at(scene->activeModel)->material.shininess_coefficient;
	ImGui::InputFloat("Shininess", &shininess);
	scene->models.at(scene->activeModel)->material.shininess_coefficient = shininess;


	if (ImGui::Button("OK"))
	{
		change_Diffuse = false;
		change_emissive = false;
		change_specular = false;
		change_ambiant = false;
		ImGui::CloseCurrentPopup();
	}
}

void transformModel(Scene* scene)
{
	if (scene->nModels == 0)
		ImGui::CloseCurrentPopup();
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

void transformLights(Scene* scene)
{
	if (scene->nLights == 0)
		ImGui::CloseCurrentPopup();
	static int type_idx = (scene->lights.at(scene->activeLight)->light_type == POINT_LIGHT ? 0 : 1);
	const char* light_type[] = { "Point", "Parallel" };
	ImGui::Combo("Light type", &type_idx, light_type, 2);
	scene->lights.at(scene->activeLight)->light_type = (type_idx == 0 ? POINT_LIGHT: PARALLEL_LIGHT);

	 GLfloat location[3] = { scene->lights.at(scene->activeLight)->position.x,
							scene->lights.at(scene->activeLight)->position.y,
							scene->lights.at(scene->activeLight)->position.z};
	ImGui::InputFloat3("Light location", location);
	scene->lights.at(scene->activeLight)->position.x = location[0];
	scene->lights.at(scene->activeLight)->position.y = location[1];
	scene->lights.at(scene->activeLight)->position.z = location[2];

	 GLfloat direction[3] = { scene->lights.at(scene->activeLight)->direction.x, 
									scene->lights.at(scene->activeLight)->direction.y, 
									scene->lights.at(scene->activeLight)->direction.z };
	ImGui::Text("Light Direction");
	ImGui::InputFloat3("Light Direction", direction);
	scene->lights.at(scene->activeLight)->direction.x =direction[0];
	scene->lights.at(scene->activeLight)->direction.y = direction[1];
	scene->lights.at(scene->activeLight)->direction.z = direction[2];
	ImGui::Text("Change Light Color");
	 ImVec4 c = ImVec4(scene->lights.at(scene->activeLight)->color.r, scene->lights.at(scene->activeLight)->color.g, scene->lights.at(scene->activeLight)->color.b, 1.0f);
	ImGui::ColorPicker4("Color Picker", (float*)&c, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);
	scene->lights.at(scene->activeLight)->color = Color(c.x, c.y, c.z);
	 GLfloat light_intensity = scene->lights.at(scene->activeLight)->intensity;
	ImGui::Text("Change Light Intensity");
	ImGui::InputFloat("light intensity", &light_intensity);
	scene->lights.at(scene->activeLight)->intensity = light_intensity;
	
	if (ImGui::Button("OK"))
	{
		ImGui::CloseCurrentPopup();
	}
}

void addLight(Scene* scene) 
{
	const char* new_light_type[] = { "Point", "Parallel" };
	static int selected_light_type = 0;
	ImGui::Combo("Light type", &selected_light_type, new_light_type, 2);

	static GLfloat new_location[3] = { 0.0f,1.0f,0.0f};
	ImGui::InputFloat3("Light location", new_location);

	static GLfloat new_direction[3] = { 0.0f,1.0f,0.0f };
	ImGui::Text("Light Direction");
	ImGui::InputFloat3("Light Direction", new_direction);

	ImGui::Text("Light Color");
	static ImVec4 new_c = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImGui::ColorPicker4("Color Picker", (float*)&new_c, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview);

	static GLfloat new_intensity = 0.5;
	ImGui::InputFloat("light intensity", &new_intensity);

	if (ImGui::Button("OK"))
	{
		LIGHT_TYPE new_type = POINT_LIGHT;
		if (selected_light_type == 1)
		{
			new_type = PARALLEL_LIGHT;
		}
		Color color(new_c.x,new_c.y,new_c.z);
		scene->addLight(vec4(new_location[0], new_location[1], new_location[2],1), vec4(new_direction[0], new_direction[1], new_direction[2],0), new_type, color);
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


void showManual()
{
	ImGui::Text("Welcome to our model rasterization app.");
	ImGui::Text("");
	ImGui::Text("--Models--");
	ImGui::Text("You can start using this program by loading some models to the scene using two methods");
	ImGui::Text("1. Loading from the preloaded models from the menu bar (! make sure the obj files are in the \"obj Files\" dir)");
	ImGui::Text("2. Loading a file from the current machine (using browse option in the menu bar)");
	ImGui::Text("After choosing a file via browsing, load the model using the \"Add uploaded model\" option under \"Model\" tab");
	ImGui::Text("");
	ImGui::Text("--Cameras--");
	ImGui::Text("The program loads a default camera");
	ImGui::Text("You can add additional cameras via the menu bar as well as transforming the existing ones");
	ImGui::Text("");
	ImGui::Text("--Normals--");
	ImGui::Text("You can display vertex normal and faces normals via the \"Scene\" tab");
	ImGui::Text("");
	ImGui::Text("Have fun and enjoy :)");


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

