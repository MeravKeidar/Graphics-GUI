#pragma once
#pragma once
#include <vector>
#include <string>
#include "mat.h"
#include "Renderer.h"
using namespace std;

class Model {
protected:
	virtual ~Model() {}
	void virtual draw() = 0;
	mat4 _model_transform;
public:
	void Translate(const GLfloat x, const GLfloat y, const GLfloat z);
	void Scale(const GLfloat x, const GLfloat y, const GLfloat z);
	void Rotate(const char hinge, const GLfloat theta);

};


class Light {
};

class Camera {
	mat4 cTransform;
	mat4 projection;

public:
	Camera() : cTransform(mat4(1.0)), projection(mat4(1.0)) {}
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up);
	void Ortho(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	void Frustum(const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar);
	mat4 Perspective(const float fovy, const float aspect,
		const float zNear, const float zFar);

};

class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer* m_renderer;
	mat4 _world_transform;

public:
	Scene() : _world_transform(mat4(1.0)) {};
	Scene(Renderer* renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void loadPrimModel(string type);
	void addCamera(const vec4& eye, const vec4& at, const vec4& up);
	void draw();
	void drawDemo();
	void zoom(GLfloat scale);

	int activeModel=0;
	int activeLight=0;
	int activeCamer=0;
};