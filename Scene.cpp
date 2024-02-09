//#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;

void Model::Translate(const GLfloat x, const GLfloat y, const GLfloat z)
{
	mat4 t = TranslationMat(x, y, z);
	_model_transform = t * _model_transform;
}


void Model::Scale(const GLfloat x, const GLfloat y, const GLfloat z)
{
	mat4 s = ScalingMat(x, y, z);
	_model_transform = s * _model_transform;
}

void Model::Rotate(const char hinge, const GLfloat theta)
{
	mat4 r;
	if (hinge == 'x') {
		r = RotationByX(theta);
	}
	if (hinge == 'y') {
		r = RotationByY(theta);
	}
	if (hinge == 'z') {
		r = RotationByZ(theta);
	}
	_model_transform = r * _model_transform;
}

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

void Scene::addCamera(const vec4& eye, const vec4& at, const vec4& up)
{
	Camera* c = new Camera();
	c->LookAt(eye, at, up);
	cameras.push_back(c);
}


void Scene::draw()
{
	m_renderer->ClearColorBuffer();
	m_renderer->Reshape(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - 20 );
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves

	//m_renderer->SetCameraTransform(cameras[activeCamera]->setTransformation)
	//m_renderer->SetProjection(cameras[activeCamera]->)
	
	for (vector<Model*>::iterator i = models.begin(); i != models.end(); i++)
	{
		drawModel(*i);
		//if (DISPLAY_FACE_NORMAL) { drawFaceNormals(*i); };
	}

	m_renderer->SwapBuffers();
}


void Scene::drawModel(Model* model)
{
	int size = model->vertex_positions.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;

	for (size_t i = 0; i < size; i++)
	{
		//model view transform
		model->modified_vertex.at(i) = (Tc*(_world_transform*(model->_model_transform * model->modified_vertex.at(i))));
		//Projection
		model->modified_vertex.at(i) = (ProjectionM() *(P* (model->modified_vertex.at(i))));
		//View-port
		model->modified_vertex.at(i) = m_renderer->viewPortVec(model->modified_vertex.at(i));
	}

	m_renderer->DrawTriangles(&(model->modified_vertex));

}


void Scene::drawFaceNormals(Model* model)
{
	int size = model->face_normals.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;

	for (size_t i = 0; i < size; i++)
	{
		//model view transform
		//TODO: check if we need to make a world normal transform
		model->modified_face_normals.at(i) = (Tc * (_world_transform * (model->_normal_transform * model->face_normals.at(i))));
		//Projection
		model->modified_face_normals.at(i) = (ProjectionM() * (P * (model->modified_face_normals.at(i))));
		//View-port
		model->modified_face_normals.at(i) = m_renderer->viewPortVec(model->modified_face_normals.at(i));
	}
		m_renderer->DrawTriangles(&(model->modified_face_normals));

}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}

void Scene::zoom(GLfloat scale) 
{
	mat4 s = ScalingMat(scale);
	_world_transform = s * _world_transform;
}

void Camera::setTransformation(const mat4& transform)
{
	cTransform = transform;
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up)
{
	//notice its the Tc^-1
	vec4 n = normalize(eye-at);
	vec4 u = normalize(cross(up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	cTransform = c * TranslationMat(-eye);
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	projection = mat4(1);
	projection[0][0] = 2 / (right - left);
	projection[1][1] = 2 / (top - bottom);
	projection[2][2] = -2 / (zFar - zNear);
	projection[0][3] = -(left + right) / (right - left);
	projection[1][3] = -(top + bottom) / (top - bottom);
	projection[2][3] = -(zFar + zNear) / (zFar - zNear);
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	projection = mat4(0);
	projection[0][0] = 2 * zNear / (right - left);
	projection[0][2] = (right + left) / (right - left);
	projection[1][1] = 2 * zNear / (top - bottom);
	projection[1][2] = (top + bottom) / (top - bottom);
	projection[2][2] = -(zFar + zNear) / (zFar - zNear);
	projection[2][3] = -2 * zNear * zFar / (zFar - zNear);
	projection[3][2] = -1;
}

void Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
	
	const float height = 2*zNear*tan(fovy/2);
	const float width = height*aspect;
	const float left = -width/2;
	const float right = width / 2;
	const float bottom = -height/2;
	const float top = height / 2;
	Frustum(left,right,bottom,top,zNear,zFar);
}

Scene::Scene()
{
	_world_transform = mat4(1);
	//m_renderer = new Renderer(512,512);
	m_renderer = new Renderer(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height - 20);
	Camera* init_camera = new Camera();
	addCamera(vec4(0, 0, -1, 0), vec4(0, 0, 0, 0), vec4(0, 1, 0, 0));
}

void Scene::addModel(Model* model)
{
	models.push_back(model);
}