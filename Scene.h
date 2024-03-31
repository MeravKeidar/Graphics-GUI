#pragma once
#pragma once
#include <vector>
#include <string>
#include "mat.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include <thread>


using namespace std;

enum SHADING {
	FLAT,
	GOURAUD,
	PHONG,
	MESH
};

class Model {
private:
	
protected:
	GLfloat min_x;
	GLfloat min_y;
	GLfloat min_z;
	GLfloat max_x;
	GLfloat max_y;
	GLfloat max_z;
	
public:
	vector<Vertex> vertices;
	vector<minimalVertex> faces;
	vector<minimalVertex> vertices_and_normals;
	vector<minimalVertex> bounding_box;
	float size_scale;
	unsigned int vao; 
	unsigned int vbo;
	GLuint programID = 0;
	GLuint normal_programID = 0;
	unsigned int vertex_normal_vao;
	unsigned int vertex_normal_vbo;
	unsigned int face_normal_vao;
	unsigned int face_normal_vbo;
	unsigned int bounding_box_vao;
	unsigned int bounding_box_vbo;
	std::string texture_path;
	std::string normal_map_path;
	bool use_texture = false;
	bool marble_texture = false;
	bool use_normal_mapping = false;
	bool enviromental_mapping = false;
	bool animation_vibrate = false;
	bool animation_pulse = false;
	bool animation_flash = false;
	std::thread myThread;
	virtual ~Model() {}
	mat4 _model_transform;
	vec4 _center_of_mass; 
	mat4 _world_transform;
	mat4 _normal_world_transform;
	mat4 _normal_model_transform;
	bool calculate_vertex_textures;
	bool repeat_animation_pulse = false;
	bool repeat_animation_vibrate = false;
	bool repeat_animation_color_flash = false;
	SHADING shading_type;
	void virtual draw() = 0; 
	void virtual setVertexAttributes() = 0;
	void virtual setNormalsVertexAttributes() = 0;
	void virtual setBoundingBoxAttributes() = 0;
	void virtual calculateTextureCoordinates(int mod) = 0;
	void virtual calculateTangent() = 0;
	void Translate(const GLfloat x, const GLfloat y, const GLfloat z);
	void Scale(const GLfloat x, const GLfloat y, const GLfloat z);
	void Rotate(const int hinge, const GLfloat theta);
	void changeUniformShininess(GLfloat coefficient);
	void changeUniformEmissiveColor(vec4 color);
	void changeUniformSpecularColor(vec4 color);
	void changeUniformDiffuseColor(vec4 color);
	void uploadTexture(const std::string& path);
	void uploadNormalMap(const std::string& path);
	void colorByNormal();
	void colorByPosition();
	void updateModel(Camera active_camera);
	bool inViewVolume(Camera active_camera);

	void pulse( )
	{
		while (repeat_animation_pulse)
		{
			for (size_t i = 0; i < 10; i++)
			{
				Scale(1.02, 1.02, 1.02);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			for (size_t i = 0; i < 10; i++)
			{
				Scale(1 / 1.02, 1 / 1.02, 1 / 1.02);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}

	void vibrate()
	{
		float scale = size_scale / 400;
		while (repeat_animation_vibrate)
		{
			for (size_t i = 0; i < 10; i++)
			{
				int random_num = rand() % 27;
				int x_dir, y_dir, z_dir;
				x_dir = random_num % 3 - 1;
				y_dir = (random_num / 3) % 3 - 1;
				z_dir = (random_num / 9) % 3 - 1;
				Translate(x_dir * scale, y_dir * scale, z_dir * scale);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				Translate(-x_dir * scale, -y_dir * scale, -z_dir * scale);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));

			}
		}
		

	}

	void color_flash()
	{
		//vec4 current_color(127, 127, 127, 1);
		//std::vector <vec4> vertices_original_colors;
		//for (auto it = vertices.begin(); it != vertices.end(); it++)
		//{
		//	vertices_original_colors.push_back((*it).emissive_color);
		//}
		//while (repeat_animation_color_flash)
		//{
		//	for (size_t i = 0; i < 10; i++)
		//	{
		//		int random_num = rand() % 27;
		//		int x_dir, y_dir, z_dir;
		//		//x_dir = random_num % 3 - 1;
		//		//y_dir = (random_num / 3) % 3 - 1;
		//		//z_dir = (random_num / 9) % 3 - 1;
		//		x_dir = 1;
		//		y_dir = 1;
		//		z_dir = 1;
		//		current_color += vec4(x_dir, y_dir, z_dir, 0);
		//		current_color = vec4(std::max(std::min(current_color.x, 255.0f), 0.0f),
		//			std::max(std::min(current_color.x, 255.0f), 0.0f),
		//			std::max(std::min(current_color.x, 255.0f), 0.0f),
		//			51);
		//		if (current_color.x > 250)
		//			std::cout << "reached bright" << std::endl;
		//		//changeUniformEmissiveColor(vec4(1,1,1,1));
		//		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		//	}
		//	
		//}

		//for (int i = 0; i <  vertices.size(); i++)
		//{
		//	vertices.at(i).emissive_color = vertices_original_colors.at(i);
		//}
			
	}
};

class Scene {
	void drawModel(Model* model);
	void drawFaceNormals(Model* model);
	void drawboundingBox(Model* model);
	
	
public:
	SHADING shading_type = FLAT;
	GLuint FlatProgramID = 0;
	GLuint GouraudProgramID = 0;
	GLuint PhongProgramID = 0;
	GLuint skyBoxProgramID = 0;
	GLuint NormalProgramID = 0;
	GLuint ActiveProgramID = 0;
	GLuint skyboxVAO, skyboxVBO;
	CubeTexture skyTexture;
	bool draw_sky_box = false;
	void ChangeAntiAliasingResolution(int resolution);
	Scene();
	void loadOBJModel(string fileName);
	void loadAxisModels();
	void loadPrimModel(string type);
	void addCamera(const vec4& eye, const vec4& at, const vec4& up);
	void addLight(const vec4 location, const vec4 direction, LIGHT_TYPE light_type, vec4 color, GLfloat lightIntensity);
	void draw();
	void drawDemo();
	void drawSkyBox();
	void genSkyBox();
	void drawCameras();
	void drawVertexNormals(Model* model);
	void zoomOnModel(GLfloat scale);
	void moveModel(const GLfloat x, const GLfloat y, const GLfloat z, int mod=0);
	void RotateModel(const int hinge, const GLfloat theta, int mod = 0);
	void scaleModel(const GLfloat x, const GLfloat y, const GLfloat z);
	void moveCamera(const GLfloat x, const GLfloat y, const GLfloat z, int mod);
	void RotateCamera(const int hinge, const GLfloat theta, int mod);
	void LookAtCurrentCamera(const vec4& eye, const vec4& at, const vec4& up);
	void LookAtModel();
	void addModel(Model* model);
	void Reset();
	void changeShading(SHADING shading_type);
	void toggleCurrentModelVibrate();
	void toggleCurrentModelPulse();
	void toggleCurrentModelFlash();
	mat4 getCurrentModelTrasform();
	mat4 getCurrentWorldTrasform();
	mat4 getCurrentCameraTrasform();
	mat4 getCurrentProjection();
	void setCameraOrtho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void setCameraFrustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void setCameraPerspective(const float fovy, const float aspect,
		const float zNear, const float zFar);
	vector<Camera*> cameras;
	vector<Light*> lights;
	vector<Model*> models;
	int activeModel=-1;
	int activeLight=-1;
	int activeCamera=-1;
	int nModels = 0;
	int nCameras = 0;
	int nLights = 0;
	bool displayVnormal = false;
	bool displayFnormal = false;
	bool displayCameras = false;
	bool displayBoundingBox = false;
	GLfloat ambient_scale = 0.2;
	GLfloat normal_scale = 0.5;
	vec4 ambient_color = { 0.1,0.1,0.1,1.0};
};
