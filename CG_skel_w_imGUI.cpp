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

#define FILE_OPEN 1
#define MAIN_DEMO 1
#define MAIN_ABOUT 2

Scene* scene;
Renderer* renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;

//----------------------------------------------------------------------------
// Callbacks
void display(void)
{
	scene->draw(); //TODO: implement
}

void reshape(GLFWwindow* window, int width, int height)
{
	renderer->Reshape(width, height); //TODO: implement
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	}
}

void mouse(GLFWwindow* window, int button, int state, int mods)
{
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		lb_down = (state == GLFW_RELEASE) ? false : true;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		rb_down = (state == GLFW_RELEASE) ? false : true;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mb_down = (state == GLFW_RELEASE) ? false : true;
		break;
	}

}

void motion(GLFWwindow* window, double xpos, double ypos) {
	// mouse motion handling logic
	int x = static_cast<int>(xpos);
	int y = static_cast<int>(ypos);
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = y - last_y;
	// update last x,y
	last_x = x;
	last_y = y;
}

void filemenu() {
	static std::string selectedFilePath;
	static char buffer[500];

	ImGui::TextUnformatted("Selected File: ");
	ImGui::InputText("##file", buffer, sizeof(buffer));
	ImGui::SameLine();

	if (ImGui::Button("Load .obj File")) {
		// Limit the assignment to the size of buffer
		selectedFilePath = std::string(buffer, strnlen_s(buffer, sizeof(buffer)));

		FileDialog::file_dialog_open = true;
		FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
	}

	if (FileDialog::file_dialog_open) {
		FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, buffer, sizeof(buffer), FileDialog::file_dialog_open_type);

		// Update the selected file path after choosing a file
		selectedFilePath = std::string(buffer, strnlen_s(buffer, sizeof(buffer)));
		scene->loadOBJModel(selectedFilePath);
	}
}








static char* file_dialog_buffer = nullptr;
static char path[500] = "";
static bool open_file_dialog = false;


void MainMenu() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Model")) {
			if (ImGui::MenuItem("Load .obj model")) {
				FileDialog::file_dialog_open = true;
				FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
	
			}

			if (FileDialog::file_dialog_open) {
				file_dialog_buffer = path;
				FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
				FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
			
				//selectedFilePath = std::string(buffer, strnlen_s(buffer, sizeof(buffer)));
				//scene->loadOBJModel(selectedFilePath);
				file_dialog_buffer = nullptr;
				FileDialog::file_dialog_open = false; // Close the file dialog
			}

			if (ImGui::MenuItem("Load Primative")) {
				
			}
			
		ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

}

//void renderimguimenus() {
//	renderMainMenu();
//
//	 Your other menu rendering functions go here
//}

//
//void fileMenu(int id)
//{
//	switch (id)
//	{
//	case FILE_OPEN:
//		CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
//		if (dlg.DoModal() == IDOK)
//		{
//			std::string s((LPCTSTR)dlg.GetPathName());
//			scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
//		}
//		break;
//	}
//}
//
//void mainMenu(int id)
//{
//	switch (id)
//	{
//	case MAIN_DEMO:
//		scene->drawDemo();
//		break;
//	case MAIN_ABOUT:
//		AfxMessageBox(_T("Computer Graphics"));
//		break;
//	}
//}




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
	glfwSwapInterval(1); // Enable vsync

	//glewInit();
	//GLenum err = glewInit();
	//if (GLEW_OK != err)
	//{
	//	/* Problem: glewInit failed, something is seriously wrong. */
	//	fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	//	/*		...*/
	//}
	//fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	// 
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


	//renderer = new Renderer(512, 512);
	//scene = new Scene(renderer);

	// Set GLFW callbacks
	/*glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, motion);*/

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << error << std::endl;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		
		drawLine(100,100,200,200);

		// OR, specify a custom color
		GLubyte customColor[] = { 255, 0, 0, 255 };  // Red color
		
		drawPixel(255, 255);


		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		

		//display();
		//MainMenu();
		// Rendering ImGUI 
		// TODO: add an IMGUI Menu renderer function (file menu using ImGuiFileMenu
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
	//delete scene;
	//delete renderer;

	return 0;
}

int main() {
	return my_main();
}





//
//void initMenu()
//{
//
//	int menuFile = glutCreateMenu(fileMenu);
//	glutAddMenuEntry("Open..",FILE_OPEN);
//	glutCreateMenu(mainMenu);
//	glutAddSubMenu("File",menuFile);
//	glutAddMenuEntry("Demo",MAIN_DEMO);
//	glutAddMenuEntry("About",MAIN_ABOUT);
//	glutAttachMenu(GLUT_RIGHT_BUTTON);
//}
////----------------------------------------------------------------------------
//
//
//
//int my_main( int argc, char **argv )
//{
//	//----------------------------------------------------------------------------
//	// Initialize window
//	glutInit( &argc, argv );
//	glutInitDisplayMode( GLUT_RGBA| GLUT_DOUBLE);
//	glutInitWindowSize( 512, 512 );
//	glutInitContextVersion( 3, 2 );
//	glutInitContextProfile( GLUT_CORE_PROFILE );
//	glutCreateWindow( "CG" );
//	glewExperimental = GL_TRUE;
//	glewInit();
//	GLenum err = glewInit();
//	if (GLEW_OK != err)
//	{
//		/* Problem: glewInit failed, something is seriously wrong. */
//		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
//		/*		...*/
//	}
//	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
//
//	
//	
//	renderer = new Renderer(512,512);
//	scene = new Scene(renderer);
//	//----------------------------------------------------------------------------
//	// Initialize Callbacks
//
//	glutDisplayFunc( display );
//	glutKeyboardFunc( keyboard );
//	glutMouseFunc( mouse );
//	glutMotionFunc ( motion );
//	glutReshapeFunc( reshape );
//	initMenu();
//	
//
//	glutMainLoop();
//	delete scene;
//	delete renderer;
//	return 0;
//}
//
//CWinApp theApp;
//
//using namespace std;
//
//int main( int argc, char **argv )
//{
//	int nRetCode = 0;
//	
//	// initialize MFC and print and error on failure
//	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
//	{
//		// TODO: change error code to suit your needs
//		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
//		nRetCode = 1;
//	}
//	else
//	{
//		my_main(argc, argv );
//	}
//	
//	return nRetCode;
//}
