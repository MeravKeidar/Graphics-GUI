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
using namespace std;

enum SHADING {
	FLAT,
	GOURAUD,
	PHONG,
	MESH
};

class Model {
protected:
	
	
public:
	vector<Vertex> bounding_box;
	vector<Vertex> vertices;
	vector<Vertex> faces;
	unsigned int vao; 
	unsigned int vbo;
	GLuint programID = 0;

	virtual ~Model() {}


	mat4 _model_transform;
	vec4 _center_of_mass; 
	mat4 _world_transform;
	mat4 _normal_world_transform;
	mat4 _normal_model_transform;

	bool color_by_pos = false;
	bool color_by_normal = false;

	void virtual draw() = 0; 
	void virtual setVertexAttributes() = 0;
	void Translate(const GLfloat x, const GLfloat y, const GLfloat z);
	void Scale(const GLfloat x, const GLfloat y, const GLfloat z);
	void Rotate(const int hinge, const GLfloat theta);
	void changeUniformShininess(GLfloat coefficient);
	void changeUniformEmissiveColor(vec4 color);
	void changeUniformSpecularColor(vec4 color);
	void changeUniformDiffuseColor(vec4 color);
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
	GLuint programID = 0;
	void ChangeAntiAliasingResolution(int resolution);
	Scene();
	void loadOBJModel(string fileName);
	void loadAxisModels();
	void loadPrimModel(string type);
	void addCamera(const vec4& eye, const vec4& at, const vec4& up);
	void addLight(const vec4 location, const vec4 direction, LIGHT_TYPE light_type,Color color = Color(1,1,1));
	void draw();
	void drawDemo();
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
	bool blur = false;
	bool bloom = false;
	GLfloat ambient_scale = 0.2;
	GLfloat normal_scale = 0.5;
	vec4 ambient_color = { 0.2,1,0.5 , 1.0};
};
