//#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;
void Scene::loadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName);
	models.push_back(model);
}

void Scene::loadPrimModel(string type)
{
	PrimMeshModel* model = new PrimMeshModel(type);
	models.push_back(model);
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves

	//m_renderer->SetCameraTransform(cameras[activeCamera]->setTransformation)
	//m_renderer->SetProjection(cameras[activeCamera]->)
	m_renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}

void Camera::setTransformation(const mat4& transform)
{
	cTransform = transform;
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	vec4 n = eye - at;
	vec4 u = cross(up, n);
	vec4 v = cross(n, u);
	vec4 i = vec4(0, 0, 0, 1);
	//TODO:: check the whole lineqrow thing when creating a new mat4
	mat4 *r = new mat4(u, v, v,i);
	mat4* t = new mat4(i, i, i, eye);
	cTransform = (*t)*(*r);
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
}

mat4 Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
	return mat4();
}
