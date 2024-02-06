#include <L2DFileDialog.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "CG_skel_w_imGUI.cpp"

static char* file_dialog_buffer = nullptr;
static char path[500] = "";
static bool open_file_dialog = false;

void FileMenu() {
	FileDialog::file_dialog_open = true;
	FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
	if (FileDialog::file_dialog_open) {
		file_dialog_buffer = path;
		FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
		FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
		//selectedFilePath = std::string(buffer, strnlen_s(buffer, sizeof(buffer)));
		//scene->loadOBJModel(selectedFilePath);
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
