#pragma once
#pragma once
#include <vector>
#include <string>
#include "mat.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
using namespace std;





class Model {
protected:
	
	void virtual draw() = 0;

public:
	MATERIAL material;
	virtual ~Model() {}
	mat4 _model_transform;
	vector<vec3> vertex_positions;
	vector<vec3> face_normals;
	vector<vec3> face_normals_origin;
	vector<vec3> bounding_box;
	vector<vec3> vertices;
	vector<vec3> vertex_normals;
	vec4 _center_of_mass; 
	mat4 _world_transform;
	mat4 _normal_world_transform;
	mat4 _normal_model_transform;


	void Translate(const GLfloat x, const GLfloat y, const GLfloat z);
	void Scale(const GLfloat x, const GLfloat y, const GLfloat z);
	void Rotate(const int hinge, const GLfloat theta);

};




class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	GLfloat ambient_scale = 0.2;
	Renderer* m_renderer;
	mat4 _world_transform;
	void drawModel(Model* model);
	void drawFaceNormals(Model* model);
	void drawboundingBox(Model* model);
	
public:
	Scene();
	Scene(Renderer* renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void loadAxisModels();
	void loadPrimModel(string type);
	void addCamera(const vec4& eye, const vec4& at, const vec4& up);
	void addLight(const vec3 location, const vec3 direction, LIGHT_TYPE light_type);
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
	
	mat4 getCurrentModelTrasform();
	mat4 getCurrentWorldTrasform();
	mat4 getCurrentCameraTrasform();
	mat4 getCurrentProjection();
	vec3 getCurrentViewPort();
	vector<Camera*> cameras;
	void setCameraOrtho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void setCameraFrustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void setCameraPerspective(const float fovy, const float aspect,
		const float zNear, const float zFar);
	int activeModel=-1;
	int activeLight=-1;
	int activeCamera=-1;
	int nModels = 0;
	int nCameras = 0;
	bool displayVnormal = false;
	bool displayFnormal = false;
	bool displayCameras = false;
	bool displayBoundingBox = false;
};