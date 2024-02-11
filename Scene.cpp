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

void Model::Rotate(const int hinge, const GLfloat theta)
{
	mat4 r;
	if (hinge == '0') {
		r = RotationByX(theta);
	}
	if (hinge == '1') {
		r = RotationByY(theta);
	}
	if (hinge == '2') {
		r = RotationByZ(theta);
	}
	_model_transform = r * _model_transform;
}


void Scene::loadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName);
	models.push_back(model);
	nModels++;
	activeModel++;
}

void Scene::loadPrimModel(string type)
{
	MeshModel* model = new MeshModel(type+"obj");
	models.push_back(model);
	nModels++;
	activeModel++;
}

void Scene::addCamera(const vec4& eye, const vec4& at, const vec4& up)
{
	Camera* c = new Camera();
	c->LookAt(eye, at, up);
	cameras.push_back(c);
	nCameras++;
	activeCamera++;
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
		if (displayBoundingBox) {
			drawboundingBox(*i);
		}
		//if (DISPLAY_FACE_NORMAL) { drawFaceNormals(*i); };
	}

	m_renderer->SwapBuffers();
}


void Scene::drawModel(Model* model)
{
	int size = model->vertex_positions.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;
	vector<vec4> modified_vertex;
	for (size_t i = 0; i < size; i++)
	{
		vec4 v(model->vertex_positions.at(i));
		//model view transform
		//v = (Tc * (_world_transform * (model->_model_transform * v)));
		v = (_world_transform * (model->_world_transform*(model->_model_transform * v)));
		//Projection
		v = (ProjectionM()*(P*v));
		//View-port
		v = m_renderer->viewPortVec(v);
		modified_vertex.push_back(v); 
	}
	m_renderer->DrawTriangles(&(modified_vertex));

}


void Scene::drawboundingBox(Model* model)
{
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;
	vector<vec4> modified_box;
	for (size_t i = 0; i < 8; i++)
	{
		vec4 v(model->bounding_box.at(i));
		//model view transform
		//v = (Tc * (_world_transform * (model->_model_transform * v)));
		v = (_world_transform * (model->_world_transform * (model->_model_transform * v)));
		//Projection
		v = (ProjectionM() * (P * v));
		//View-port
		v = m_renderer->viewPortVec(v);
		modified_box.push_back(v);
	}
	m_renderer->DrawBox(&(modified_box),0.3, 0.3, 0.4);

}


void Scene::drawFaceNormals(Model* model)
{
	int size = model->face_normals.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;
	vector<vec4> modified_face_normals;
	for (size_t i = 0; i < size; i++)
	{
		//model view transform
		//TODO: check if we need to make a world normal transform
		vec4 v = model->face_normals.at(i);
		v = Tc * (_world_transform * (model->_normal_transform * model->face_normals.at(i)));
		//Projection
		v = (ProjectionM() * (P * v));
		//View-port
		v = m_renderer->viewPortVec(v);
		modified_face_normals.push_back(v);
		//TODO: how do we draw normals? line starting ehre ending where ? 
		///m_renderer -> DrawLine(v.x,x.y,0.0f, 0.0f, 0.2f)
	}


	//m_renderer->DrawTriangles(&(modified_face_normals));

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


void Scene::moveModel(const GLfloat x, const GLfloat y, const GLfloat z,int mod)
{
	if (nModels == 0) { return; };
	mat4 t = TranslationMat(x, y, z);
	if (mod == 0) {
		models.at(activeModel)->_model_transform = t * models.at(activeModel)->_model_transform;
	}
	else if(mod == 1)
	{
		models.at(activeModel)->_world_transform = t * models.at(activeModel)->_world_transform;
	}
}

void Scene::RotateModel(const int hinge, const GLfloat theta, int mod)
{
	if (nModels == 0) { return; };
	mat4 r;
	if (hinge == 0) {
		r = RotationByX(theta);
	}
	if (hinge == 1) {
		r = RotationByY(theta);
	}
	if (hinge == 2) {
		r = RotationByZ(theta);
	}
	if (mod == 0) {
		models.at(activeModel)->_model_transform = r * models.at(activeModel)->_model_transform;
	}
	else if (mod == 1)
	{
		models.at(activeModel)->_world_transform = r * models.at(activeModel)->_world_transform;
	}
}

void Scene::scaleModel(const GLfloat x, const GLfloat y, const GLfloat z) {
	models.at(activeModel)->Scale(x, y, z);
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
	_world_transform = mat4(1.0);
	//m_renderer = new Renderer(512,512);
	m_renderer = new Renderer(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height - 20);
	Camera* init_camera = new Camera();
	addCamera(vec4(0, 0, -1, 0), vec4(0, 0, 10, 0), vec4(0, 1, 0, 0));
}

void Scene::addModel(Model* model)
{
	models.push_back(model);
	nModels++; 
	activeModel++;
}

