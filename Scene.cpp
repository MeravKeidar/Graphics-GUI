//#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;

void Model::Translate(const GLfloat x, const GLfloat y, const GLfloat z)
{
	mat4 t = TranslationMat(x, y, z);
	_model_transform = t * _model_transform;
	_normal_model_transform = t * _normal_model_transform;
}


void Model::Scale(const GLfloat x, const GLfloat y, const GLfloat z)
{
	mat4 s = ScalingMat(x, y, z);
	_model_transform = s * _model_transform;
	mat4 n = ScalingMat(1 / x, 1 / y, 1 / z);
	_normal_model_transform = n * _normal_model_transform;
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
	_normal_model_transform = r * _normal_model_transform;
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
	PrimMeshModel* model = new PrimMeshModel(type);
	models.push_back(model);
	nModels++;
	activeModel++;
}

void Scene::addCamera(const vec4& eye, const vec4& at, const vec4& up)
{
	Camera* c = new Camera();
	c->LookAt(eye, at, up);
	c->eye = eye;
	c-> at = at;
	c->up = up; 
	cameras.push_back(c);
	nCameras++;
	activeCamera++;
}


void Scene::draw()
{
	m_renderer->ClearColorBuffer();
	m_renderer->Reshape(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - 20 );

	if (displayCameras)
		drawCameras();
	int size = models.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i != activeModel) 
		{
			drawModel(models.at(i),0.6,0.6,0.6);
		}
		else
		{
			drawModel(models.at(i));
		}
		
		if (displayBoundingBox) {
			drawboundingBox(models.at(i));
		}
		if (displayFnormal) 
		{
			drawFaceNormals(models.at(i));
		}
		if (displayVnormal)
		{
			drawVertexNormals(models.at(i));
		}

	}
	
	m_renderer->SwapBuffers();
}




void Scene::drawModel(Model* model, float r, float g ,float b)
{
	int size = model->vertex_positions.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;\
	mat4 P = cameras.at(activeCamera)->projection;
	vector<vec2> modified_vertex;
	for (size_t i = 0; i < size; i++)
	{
		vec4 v(model->vertex_positions.at(i));
		v = Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v))); //model view transform
		v = P * v; // projection
		if ((v.w < 0.00001) && (v.w > -0.00001)) continue;
		vec2 v_2d(v.x / v.w, v.y / v.w);
		v_2d = m_renderer->viewPortVec(v_2d); // view-port
		modified_vertex.push_back(v_2d);

	}
	m_renderer->DrawTriangles(&(modified_vertex),r,g,b);

}



void Scene::drawboundingBox(Model* model)
{
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;
	vector<vec2> modified_box;
	for (size_t i = 0; i < 8; i++)
	{
		vec4 v(model->bounding_box.at(i));
		v = Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v))); //model view transform
		v = P * v; //Projection
		vec2 v_2d(v.x / v.w, v.y / v.w);
		v_2d = m_renderer->viewPortVec(v_2d); // view-port
		modified_box.push_back(v_2d);
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
		vec4 normal = model->face_normals.at(i);
		normal[3] = 0;
		vec4 v_origin = model->face_normals_origin.at(i);
		normal = Tc * (_world_transform * (model->_normal_world_transform * (model->_normal_model_transform * normal))); //model-view
		v_origin = Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v_origin))); //model-view
		vec4 v_dest = v_origin + normal;
		v_dest = (P * v_dest); //View-port
		v_origin = (P * v_origin); //View-port
		vec2 v_2d_dest(v_dest.x / v_dest.w, v_dest.y / v_dest.w);
		v_2d_dest = m_renderer->viewPortVec(v_2d_dest); //View-port
		vec2 v_2d_origin(v_origin.x / v_origin.w, v_origin.y / v_origin.w);
		v_2d_origin = m_renderer->viewPortVec(v_2d_origin); //View-port
		m_renderer->DrawLine(v_2d_origin.x, v_2d_dest.x, v_2d_origin.y, v_2d_dest.y, 0.0f, 0.8, 0.9f);

	}
}

void Scene::drawVertexNormals(Model* model)
{
	int size = model->vertices.size();
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;

	for (size_t i = 0; i < size; i++)
	{
		vec4 normal = model->vertex_normals.at(i);
		normal[3] = 0;
		vec4 v_origin = model->vertices.at(i);
		normal =  Tc * (_world_transform * (model->_normal_world_transform * (model->_normal_model_transform * normal))); //model-view
		v_origin = Tc *  ( _world_transform * (model->_world_transform * (model->_model_transform * v_origin))); //model-view
		vec4 v_dest = v_origin + normal; 
		v_dest = (P * v_dest); //View-port
		v_origin = (P * v_origin); //View-port
		vec2 v_2d_dest(v_dest.x / v_dest.w, v_dest.y / v_dest.w);
		v_2d_dest = m_renderer->viewPortVec(v_2d_dest); //View-port
		vec2 v_2d_origin(v_origin.x / v_origin.w, v_origin.y / v_origin.w);
		v_2d_origin = m_renderer->viewPortVec(v_2d_origin); //View-port
		m_renderer->DrawLine(v_2d_origin.x, v_2d_dest.x, v_2d_origin.y, v_2d_dest.y, 0.0f, 0.8, 0.9f);
	}

}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}


void Camera::Zoom(GLfloat scale) {
	GLfloat width = _right - _left;
	GLfloat height = _top - _bottom;
	GLfloat newWidth = width * (1.0f - scale);
	GLfloat newHeight = height * (1.0f - scale);
	GLfloat dx = (width - newWidth) / 2.0f;
	GLfloat dy = (height - newHeight) / 2.0f;

	_left += dx;
	_right -= dx;
	_bottom += dy;
	_top -= dy;
	if (projection_type == 0) {
		Ortho(_left, _right, _bottom, _top, _zNear, _zFar);
	}
	else {
		Frustum(_left, _right, _bottom, _top, _zNear, _zFar);
	}
}


void Scene::moveModel(const GLfloat x, const GLfloat y, const GLfloat z,int mod)
{
	if (nModels == 0) { return; };
	mat4 t = TranslationMat(x, y, z);
	if (mod == 0) {
		models.at(activeModel)->_model_transform = t * models.at(activeModel)->_model_transform;
		models.at(activeModel)->_normal_model_transform = t * models.at(activeModel)->_normal_model_transform;
		
	}
	else if(mod == 1)
	{
		models.at(activeModel)->_world_transform = t * models.at(activeModel)->_world_transform;
		models.at(activeModel)->_normal_world_transform = t * models.at(activeModel)->_normal_world_transform;
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
		models.at(activeModel)->_normal_model_transform = r * models.at(activeModel)->_normal_model_transform;
	}
	else if (mod == 1)
	{
		models.at(activeModel)->_world_transform = r * models.at(activeModel)->_world_transform;
		models.at(activeModel)->_normal_world_transform = r * models.at(activeModel)->_normal_world_transform;
	}
	
}

void Scene::scaleModel(const GLfloat x, const GLfloat y, const GLfloat z) {
	models.at(activeModel)->Scale(x, y, z);
}


void Scene::moveCamera(const GLfloat x, const GLfloat y, const GLfloat z, int mod)
{
	if (nCameras == 0) { return; };	
	Camera* camera = cameras.at(activeCamera);
	if (mod == 0) // view frame 
	{
		mat4 t = TranslationMat(x, y, z);
		camera->cTransform = t * camera->cTransform;
		camera->eye[0] -= x;
		camera->eye[1] -= y;
		camera->eye[2] -= z;
	}
	else if (mod == 1) // world frame 
	{
		mat4 t = TranslationMat(-x, -y, -z);
		camera->setTransformation(t);
		camera->eye[0] += x;
		camera->eye[1] += y;

	}
}

void Scene::RotateCamera(const int hinge, const GLfloat theta, int mod)
{

	if (nCameras == 0) { return; };
	mat4 r;
	Camera* camera = cameras.at(activeCamera);
	if (mod == 0) // view frame 
	{
		if (hinge == 0) {
			r = RotationByX(theta);
		}
		if (hinge == 1) {
			r = RotationByY(theta);
		}
		if (hinge == 2) {
			r = RotationByZ(theta);
		}
		camera->cTransform = r * camera->cTransform;
	}
	else if (mod == 1) // world frame 
	{
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
}
	


void Camera::setTransformation(const mat4& transform)
{
	//notice transform is the T^-1
	cTransform = cTransform * transform;
}

void Camera::LookAt(const vec4& new_eye, const vec4& new_at, const vec4& new_up)
{
	eye = new_eye;
	at = new_at; 
	up = new_up; 
	eye[3] = 1;
	at[3] = 1;
	up[3] = 1;
	//notice its the Tc^-1
	vec4 n = normalize(new_eye -new_at);
	vec4 u = normalize(cross(new_up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	u[3] = 0;
	v[3] = 0;
	n[3] = 0;
	mat4 c = mat4(u, v, n, t); // (rotation)^-1
	c[0][3] = 0;
	c[1][3] = 0;
	c[2][3] = 0;
	cTransform = c * TranslationMat(-new_eye);
}

void Camera::Ortho(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	projection_type = 0; 
	_left = _left;
	_right = _right;
	_bottom = bottom;
	_zNear = zNear;
	_zFar = zFar;
	projection = mat4(1);
	projection[0][0] = 2 / (right - left);
	projection[1][1] = 2 / (top - bottom);
	projection[2][2] = -2 / (zFar - zNear);
	projection[0][3] = -((left + right) / (right - left));
	projection[1][3] = -((top + bottom) / (top - bottom));
	projection[2][3] = -((zFar + zNear) / (zFar - zNear));
	/*mat4 _projection_m(1.0);
	_projection_m[2][2] = 0;
	projection = _projection_m * projection;*/
}

void Camera::Frustum(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar)
{
	projection_type = 1; 
	_left = _left;
	_right = _right;
	_bottom = bottom;
	_zNear = zNear;
	_zFar = zFar;
	projection = mat4(0);
	projection[0][0] = (2 * zNear) / (right - left);
	projection[0][2] = (right + left) / (right - left);
	projection[1][1] = (2 * zNear) / (top - bottom);
	projection[1][2] = (top + bottom) / (top - bottom);
	projection[2][2] = -(zFar + zNear) / (zFar - zNear);
	projection[2][3] = -(2 * zNear * zFar) / (zFar - zNear);
	projection[3][2] = -1;
}

void Camera::Perspective(const float fovy, const float aspect, const float zNear, const float zFar)
{
	const float height = 2 * zNear *tan (fovy/2);
	const float width = height * aspect;
	const float left = - width / 2;
	const float right =  width / 2;
	const float bottom = - height / 2;
	const float top = height / 2;
	_left = _left;
	_right = _right;
	_bottom = bottom;
	_zNear = zNear;
	_zFar = zFar;
	Frustum(left,right,bottom,top,zNear,zFar);
}

Scene::Scene()
{
	_world_transform = mat4(1.0);
	
	//m_renderer = new Renderer(512,512);
	m_renderer = new Renderer(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height - 20);
	addCamera(vec4(0, 0, 2, 0), vec4(0, 0, -1, 0), vec4(0, 1, 0, 0));


}

void Scene::Reset()
{
	int size = models.size();
	for (size_t i = 0; i < size; ++i) {
		Model* curr_m = models.at(i);
		delete curr_m;
	}
	models.clear();
	size = cameras.size();
	for (size_t i = 0; i < size; ++i) {
		Camera* curr_c = cameras.at(i);
		delete curr_c;
	}
	cameras.clear();
	activeModel = -1;
	activeCamera = -1;
	int nModels = 0;
	int nCameras = 0;
	addCamera(vec4(0, 0, 2, 0), vec4(0, 0, -1, 0), vec4(0, 1, 0, 0));
}


void Scene::addModel(Model* model)
{
	models.push_back(model);
	nModels++; 
	activeModel++;
}


void Scene::LookAtModel()
{
	if (nModels == 0)
		return;
	vec4 v = _world_transform * (models.at(activeModel)->_normal_world_transform * (models.at(activeModel)->_normal_model_transform * models.at(activeModel)->_center_of_mass)); //model-view 
	cameras.at(activeCamera)->LookAt(cameras.at(activeCamera)->eye, v, cameras.at(activeCamera)->up);
}


void Scene::drawCameras()
{
	mat4 Tc = cameras.at(activeCamera)->cTransform;
	mat4 P = cameras.at(activeCamera)->projection;
	vec4 modified_eye;
	vec2 modified_eye_2d;
	for (size_t i = 0; i < nCameras; i++)
	{
		if (i == activeCamera)
			continue;
		modified_eye = P * (Tc * cameras.at(i)->eye);  //Projection
		modified_eye_2d = (modified_eye.x, modified_eye.y);
		modified_eye_2d = m_renderer->viewPortVec(modified_eye_2d);//View-port
		m_renderer->DrawLine(modified_eye_2d[0] - 10, modified_eye_2d[0] + 10, modified_eye_2d[1], modified_eye_2d[1], 0.6, 0.8, 0.9);
		m_renderer->DrawLine(modified_eye_2d[0], modified_eye_2d[0], modified_eye_2d[1] - 10, modified_eye_2d[1] + 10, 0.6, 0.8, 0.9);
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

vec2 Scene::getCurrentViewPort()
{
	return m_renderer->viewPortVec(vec2(1));
}

