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

	virtual ~Model() {}
	mat4 _model_transform;
	vec4 _center_of_mass; 
	mat4 _world_transform;
	mat4 _normal_world_transform;
	mat4 _normal_model_transform;
	bool calculate_vertex_textures;
	

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
