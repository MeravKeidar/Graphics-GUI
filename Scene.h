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
	
	void virtual draw() = 0;

public:
	GLfloat diffuse_fraction;
	GLfloat specular_fraction;
	Color color;
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

class Light {
};

class Camera {
	GLfloat _left = -1;
	GLfloat _right= 1;
	GLfloat _bottom = -1;
	GLfloat _top = 1;
	GLfloat _zNear = 1;
	GLfloat _zFar = -1;
	int projection_type = 0; //ortho by deafult 
public:
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
	void Zoom(GLfloat scale);
	vec4 eye;
	vec4 at;
	vec4 up;


};

class Scene {
	vector<Model*> models;
	vector<Light*> lights;
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