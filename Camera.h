
#pragma once
#include <vector>
#include <string>
#include "mat.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Camera {
	
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
	GLfloat _left = -1;
	GLfloat _right = 1;
	GLfloat _bottom = -1;
	GLfloat _top = 1;
	GLfloat _zNear = 1;
	GLfloat _zFar = -1;


};