//#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;

void Model::Translate(const GLfloat x, const GLfloat y, const GLfloat z)
{
	mat4 t = TranslationMat(x, y, z);
	_model_transform = t * _model_transform;
	_normal_transform = t * _normal_transform;
}


void Model::Scale(const GLfloat x, const GLfloat y, const GLfloat z)
{
	mat4 s = ScalingMat(x, y, z);
	_model_transform = s * _model_transform;
	mat4 n = ScalingMat(1 / x, 1 / y, 1 / z);
	_normal_transform = n * _normal_transform;
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
	_normal_transform = r * _model_transform;
}


void Scene::loadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName);
	models.push_back(model);
	nModels++;
	activeModel++;
}

void Scene::loadAxisModels()
{
	MeshModel* model = new MeshModel("obj_files/x_axis_arrow.obj");
	axis_models.push_back(model);
	model = new MeshModel("obj_files/y_axis_arrow.obj");
	axis_models.push_back(model);
	model = new MeshModel("obj_files/z_axis_arrow.obj");
	axis_models.push_back(model);
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
	//c->Frustum(-1, 1, -1, 1, -1, 1);
	//c->LookAt(eye, at, up);
	cameras.push_back(c);
	nCameras++;
	activeCamera++;
}


void Scene::draw(bool draw_axis)
{
	m_renderer->ClearColorBuffer();
	m_renderer->Reshape(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - 20 );
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves

	//m_renderer->SetCameraTransform(cameras[activeCamera]->setTransformation)
	//m_renderer->SetProjection(cameras[activeCamera]->)
	if (displayCameras)
		drawCameras();
	if (draw_axis)
	{
		
			drawModel(axis_models.at(0),1,0,0);
			drawModel(axis_models.at(1), 0, 1, 0);
			drawModel(axis_models.at(2), 0, 0, 1);
		
	}

	for (vector<Model*>::iterator i = models.begin(); i != models.end(); i++)
	{
		drawModel(*i);
		if (displayBoundingBox) {
			drawboundingBox(*i);
		}
		if (displayFnormal) 
		{
			drawFaceNormals(*i);
		}
		if (displayVnormal)
		{
			drawVertexNormals(*i);
		}

	}
	
	m_renderer->SwapBuffers();
}




void Scene::drawModel(Model* model, float r, float g ,float b)
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
		//v = Tc * (_world_transform * (model->_world_transform*(model->_model_transform * v)));
		// v = (ProjectionM()*(P*v));
		// v = m_renderer->viewPortVec(v);
		// splitting for debugging
		vec4 post_model_transform = model->_model_transform * v;
		vec4 post_model_world = (model->_world_transform * post_model_transform);
		vec4 post_world_transform = _world_transform * post_model_world;
		vec4 post_camera = Tc * post_world_transform;
		vec4 post_projection = P * post_camera;
		vec4 post_proj_M = ProjectionM() * post_projection;
		vec4 post_view_port = m_renderer->viewPortVec(post_proj_M);
		//Projection
		
		//View-port
		
		modified_vertex.push_back(post_view_port); 
	}
	m_renderer->DrawTriangles(&(modified_vertex),r,g,b);

}

void Scene::drawAxis()
{
	Model* model = new MeshModel();
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
		v = Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v)));
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
	
	for (size_t i = 0; i < size; i++)
	{
		//model view transform
		//TODO: check if we need to make a world normal transform
		vec4 v1 = model->face_normals.at(i);
		v1 = Tc * (_world_transform * (model->_normal_transform * v1));
		//Projection
		v1 = (ProjectionM() * (P * v1));
		//View-port
		v1 = m_renderer->viewPortVec(v1);

		vec4 v2 = model->face_normals_origin.at(i);
		v2 = Tc * (_world_transform * (model->_world_transform*(model->_model_transform * v2)));
		//Projection
		v2 = (ProjectionM() * (P * v2));
		//View-port
		v2 = m_renderer->viewPortVec(v2);
		

		vec4 v_dest = v1 + v2;
		m_renderer->DrawLine(v2.x, v_dest.x, v2.y, v_dest.y, 0.0f, 1, 0.2f);
	}


	//m_renderer->DrawTriangles(&(modified_face_normals));

}

void Scene::drawVertexNormals(Model* model)
{
	int size = model->vertices.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;

	for (size_t i = 0; i < size; i++)
	{
		//model view transform
		//TODO: check if we need to make a world normal transform
		vec4 v1 = model->vertex_normals.at(i);
		v1 = Tc * (_world_transform * (model->_normal_transform * v1));
		//Projection
		v1 = (ProjectionM() * (P * v1));
		//View-port
		v1 = m_renderer->viewPortVec(v1);

		vec4 v2 = model->vertices.at(i);
		v2 = Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v2)));
		//Projection
		v2 = (ProjectionM() * (P * v2));
		//View-port
		v2 = m_renderer->viewPortVec(v2);


		vec4 v_dest = v1 + v2;
		m_renderer->DrawLine(v2.x, v_dest.x, v2.y, v_dest.y, 1, 0, 0.2f);
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
	//mat4 s = ScalingMat(scale);
	//_world_transform = s * _world_transform;
	if (nCameras == 0) { return; };
	mat4 s = ScalingMat(scale);
	Camera* camera = cameras.at(activeCamera);
	camera->setTransformation(s);
	
}


void Scene::moveModel(const GLfloat x, const GLfloat y, const GLfloat z,int mod)
{
	if (nModels == 0) { return; };
	mat4 t = TranslationMat(x, y, z);
	models.at(activeModel)->_normal_transform = t * models.at(activeModel)->_normal_transform;
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
	models.at(activeModel)->_normal_transform = r * models.at(activeModel)->_normal_transform;
}

void Scene::scaleModel(const GLfloat x, const GLfloat y, const GLfloat z) {
	models.at(activeModel)->Scale(x, y, z);
}

void Scene::moveCamera(const GLfloat x, const GLfloat y, const GLfloat z)
{
	if (nCameras == 0) { return; };
	mat4 t = TranslationMat(-x, -y, -z);
	Camera* camera = cameras.at(activeCamera);
	camera->setTransformation(t);
	camera->eye[0] += x;
	camera->eye[1] += y;
	camera->eye[2] += z;
}

void Scene::RotateCamera(const int hinge, const GLfloat theta)
{

	if (nCameras == 0) { return; };
	mat4 r;
	if (hinge == 0) {
		r = RotationByX(-theta);
	}
	if (hinge == 1) {
		r = RotationByY(-theta);
	}
	if (hinge == 2) {
		r = RotationByZ(-theta);
	}
	cameras.at(activeCamera)->setTransformation(r);
}


void Camera::setTransformation(const mat4& transform)
{
	//notice transform is the T^-1
	cTransform = cTransform * transform;
}

void Camera::LookAt(const vec4& new_eye, const vec4& at, const vec4& up)
{
	eye = new_eye;

	//notice its the Tc^-1
	vec4 n = normalize(new_eye -at);
	vec4 u = normalize(cross(up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = mat4(u, v, n, t);
	cTransform = c * TranslationMat(-new_eye);
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
	addCamera(vec4(0, 0, -1, 0), vec4(0, 0, 0, 0), vec4(0, 1, 0, 0));
	loadAxisModels();

}

void Scene::addModel(Model* model)
{
	models.push_back(model);
	nModels++; 
	activeModel++;
}

void Scene::LookAtCurrentCamera(const vec4& eye, const vec4& at, const vec4& up)
{
	if (nCameras == 0)
		return;
	cameras.at(activeCamera)->LookAt(eye, at, up);

}

void Scene::drawCameras()
{
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;
	vec4 modified_eye;
	for (size_t i = 0; i < nCameras; i++)
	{
		if (i == activeCamera)
			continue;
		//vec4 v(model->vertex_positions.at(i));
		//model view transform
		//v = (Tc * (_world_transform * (model->_model_transform * v)));
		//v = Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v)));
		//Projection
		modified_eye =(ProjectionM() * (P * (Tc * cameras.at(i)->eye)));
		//View-port
		
		modified_eye = m_renderer->viewPortVec(modified_eye);
		m_renderer->DrawLine(modified_eye[0] - 10, modified_eye[0] + 10, modified_eye[1], modified_eye[1], 0.6, 0.8, 0.9);
		m_renderer->DrawLine(modified_eye[0], modified_eye[0], modified_eye[1] - 10, modified_eye[1] + 10, 0.6, 0.8, 0.9);
	}
	
}

void Scene::setCameraOrtho(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar)
{
	cameras.at(activeCamera)->Ortho(left, right, bottom, top, zNear, zFar);
}

void Scene::setCameraFrustum(const float left, const float right,
	const float bottom, const float top,
	const float zNear, const float zFar)
{
	cameras.at(activeCamera)->Frustum(left, right, bottom, top, zNear, zFar);
}
void Scene::setCameraPerspective(const float fovy, const float aspect,
	const float zNear, const float zFar)
{
	cameras.at(activeCamera)->Perspective(fovy, aspect, zNear, zFar);
}

mat4 Scene::getCurrentModelTrasform()
{
	if (models.size() == 0)
		return mat4(0);
	return models.at(activeModel)->_model_transform;
}
mat4 Scene::getCurrentWorldTrasform()
{
	if (models.size() == 0)
		return mat4(0);
	return models.at(activeModel)->_world_transform;
}

mat4 Scene::getCurrentCameraTrasform()
{
	if (cameras.size() == 0)
		return mat4(0);
	return cameras.at(activeCamera)->cTransform;
}
mat4 Scene::getCurrentProjection()
{
	if (cameras.size() == 0)
		return mat4(0);
	return cameras.at(activeCamera)->projection;
}
vec4 Scene::getCurrentViewPort()
{
	return m_renderer->viewPortVec(vec4(1));
}

