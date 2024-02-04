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
	m_renderer->DrawLine(0, 200, 0, 200);
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
	/*vec4 n = normalize(eye-at);
	vec4 u = normalize(cross(up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	cTransform = c * Translate(-eye);*/
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
