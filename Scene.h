#pragma once
#pragma once
#include <vector>
#include <string>
#include "mat.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Renderer.h"
using namespace std;

class Model {
protected:

	virtual ~Model() {}
	void virtual draw() = 0;

public:
	mat4 _model_transform;
	vector<vec4> vertex_positions;
	vector<vec4> vertex_normals;
	vector<vec4> face_normals;
	vector<vec4> modified_vertex;
	vector<vec4> modified_vertex_normals;
	vector<vec4> modified_faces_normals;
	vector<vec4> vertex_textures;

	mat4 _world_transform;
	mat4 _normal_transform;
	/// for calculating the bounding box /// 
	vec3 min_cordinates;
	vec3 max_cordinates;
	void Translate(const GLfloat x, const GLfloat y, const GLfloat z);
	void Scale(const GLfloat x, const GLfloat y, const GLfloat z);
	void Rotate(const char hinge, const GLfloat theta);

};


class Light {
};

class Camera {
	
	

public:
	//TODO: fix privacy
	mat4 projection;
	mat4 cTransform;
	Camera() : cTransform(mat4(1.0)), projection(mat4(1.0)) {}
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Frustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);

};

class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer* m_renderer;
	mat4 _world_transform;
	void draw_model(Model* model);

public:
	Scene();
	Scene(Renderer* renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void loadPrimModel(string type);
	void addCamera(const vec4& eye, const vec4& at, const vec4& up);
	void draw();
	void drawDemo();
	void zoom(GLfloat scale);
	void addModel(Model* model);
	int activeModel=0;
	int activeLight=0;
	int activeCamera=0;
};