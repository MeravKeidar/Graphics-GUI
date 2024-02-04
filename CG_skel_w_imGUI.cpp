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
				//scene->loadPrimModel("pyramid");
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
			ImGui::EndMenu();
			if (ImGui::MenuItem("Look AT")) {

			}
			ImGui::EndMenu();
			if (ImGui::MenuItem("Change Active Camera")) {

			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
		
	}
}







/*
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
		if (ImGui::BeginMenu("Model")) {
			if (ImGui::MenuItem("Load .obj model")) {

		ImGui::EndMenu();
		}
	ImGui::EndMainMenuBar();
	}

}


////////////////////Original skeleton funcs/////////////////////////////
//Original skeleton funcs
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


/*

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, PixelBuffer);
	glFlush();
}
*/
///////////////////////////////////////////////////////////////////////////

int my_main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 2.1 to support fixed-function pipeline
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	
	// Create a GLFWwindow object of 512 by 512 pixels, naming it "CG"
	
	

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


	renderer = new Renderer(512, 512);
	scene = new Scene(renderer);
	vec3 eye(0.0,0.0,0.0); 
	vec3 at(0.0,0.0,-1.0);
	vec3 up(0.0,1.0,0.0);
	scene->addCamera(eye,at,up);//TODO: check about default perspective;

	// Set GLFW callbacks
	/*glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, motion);*/

	glViewport(0, 0, 512, 512); // Set the viewport size to match the window size

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

		//renderer->SetDemoBuffer();
		//renderer->DrawLine(1,400, 1, 400,1,0,1);
		//renderer->SwapBuffers();

		//scene->draw();
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




/*OLD CODE - Might be relevant (?)*/
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
//
//#define FILE_OPEN 1
//#define MAIN_DEMO 1
//#define MAIN_ABOUT 2
//int last_x, last_y;
//bool lb_down, rb_down, mb_down;
//
////----------------------------------------------------------------------------
//// Callbacks
//void display(void)
//{
//	scene->draw(); //TODO: implement
//}
//
//void reshape(GLFWwindow* window, int width, int height)
//{
//	renderer->Reshape(width, height); //TODO: implement
//}
//
//void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	switch (key) {
//	case GLFW_KEY_ESCAPE:
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//		break;
//	}
//}
//
//void mouse(GLFWwindow* window, int button, int state, int mods)
//{
//	switch (button) {
//	case GLFW_MOUSE_BUTTON_LEFT:
//		lb_down = (state == GLFW_RELEASE) ? false : true;
//		break;
//	case GLFW_MOUSE_BUTTON_RIGHT:
//		rb_down = (state == GLFW_RELEASE) ? false : true;
//		break;
//	case GLFW_MOUSE_BUTTON_MIDDLE:
//		mb_down = (state == GLFW_RELEASE) ? false : true;
//		break;
//	}
//
//}
//
//void motion(GLFWwindow* window, double xpos, double ypos) {
//	// mouse motion handling logic
//	int x = static_cast<int>(xpos);
//	int y = static_cast<int>(ypos);
//	// calc difference in mouse movement
//	int dx = x - last_x;
//	int dy = y - last_y;
//	// update last x,y
//	last_x = x;
//	last_y = y;
//}
