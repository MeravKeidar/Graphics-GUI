//#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>


using namespace std;
Color c_white{ 1, 1, 1 };
Color c_black{ 0, 0, 0 };
Color c_red{ 1, 0, 0 };
Color c_green{ 0, 1, 0 };
Color c_blue{ 0, 0, 1 };
Color c_yellow{ 1, 1, 0 };
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


void Model::changeUniformColor(Color color)
{
	for (size_t i = 0; i < faces.size(); i++)
	{
		faces.at(i).v1.material.color = color;
	}
}

void Model::changeUniformMaterial(MATERIAL material)
{
	for (size_t i = 0; i < faces.size(); i++)
	{
		faces.at(i).v1.material = material;
	}
}

void Scene::loadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName);
	model->material.color = { 1,0,0 };
	models.push_back(model);
	

	nModels++;
	activeModel++;
}

void Scene::loadPrimModel(string type)
{
	PrimMeshModel* model = new PrimMeshModel(type);
	model->material.color = { 0,1,1 };
	models.push_back(model);
	nModels++;
	activeModel++;
	std::cout << "r:" << model->material.color.r << std::endl;
	std::cout << "g:" << model->material.color.g << std::endl;
	std::cout << "b:" << model->material.color.b << std::endl;

	std::cout << "ambient_fraction :" << model->material.ambient_fraction << std::endl;
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
	m_renderer->ClearDepthBuffer();
	m_renderer->Reshape(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - 20 );

	if (displayCameras)
		drawCameras();
	int size = models.size();

	for (size_t i = 0; i <lights.size(); i++)
	{
		Light* current_light = lights.at(i);
		mat4 ProjectionMatrix = cameras.at(activeCamera)->cTransform;
		current_light->view_position = ProjectionMatrix * current_light->position; 
		current_light->view_direction = ProjectionMatrix * current_light->direction;
	}

	for (size_t i = 0; i < size; i++)
	{
		drawModel(models.at(i));
		//drawVertexNormals(models.at(i));
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

void Scene::changeShading(SHADING shading_type)
{
	m_renderer->shadingType = shading_type; 
}

bool Scene::drawboundingBox(Model* model)
{
	mat4 ModelViewMatrix = cameras.at(activeCamera)->cTransform * (_world_transform * (model->_world_transform * model->_model_transform));
	mat4 ProjectionMatrix = cameras.at(activeCamera)->projection;
	//taking bounding edges at 0(min) and 7(max) of bounding box
	vec4 min_corner(1, 1, 1, 1);
	vec4 max_corner(-1, -1, -1, 1);
	vector<vec3> modified_box;

	for (size_t i = 0; i < 8; i++)
	{
		vec4 v(model->bounding_box.at(i));
		v = ModelViewMatrix * v;
		v = ProjectionMatrix * v; 

		if ((v.w < 0.00001) && (v.w > -0.00001)) continue;
		vec3 v_2d(v.x / v.w, v.y / v.w, v.z / v.w);

		if (v_2d.x < min_corner.x)
			min_corner.x = v_2d.x;

		if (v_2d.y < min_corner.y)
			min_corner.y = v_2d.y;

		if (v_2d.z < min_corner.z)
			min_corner.z = v_2d.z;

		if (v_2d.x > max_corner.x)
			max_corner.x = v_2d.x;

		if (v_2d.y > max_corner.y)
			max_corner.y = v_2d.y;

		if (v_2d.z > max_corner.z)
			max_corner.z = v_2d.z;

		v_2d = m_renderer->viewPortVec(v_2d); // view-port
		modified_box.push_back(v_2d);
	}

	if ((min_corner.x > min_corner.w || min_corner.y > min_corner.w || min_corner.z > min_corner.w)
		|| (max_corner.x < -max_corner.w || max_corner.y < -max_corner.w || max_corner.z < -max_corner.w))
		return false;

	if (displayBoundingBox)
	{
		Color box_color;
		box_color.r = 0.3;
		box_color.g = 0.3;
		box_color.b = 0.4;
		m_renderer->DrawBox(&(modified_box), box_color);
	}
	return true;

}

void Scene::drawModel(Model* model)
{
	
	mat4 ModelViewMatrix = cameras.at(activeCamera)->cTransform * (model->_world_transform * model->_model_transform);
	mat4 NormalViewMatrix = cameras.at(activeCamera)->cTransform * (model->_normal_world_transform * model->_normal_model_transform);
	mat4 ProjectionMatrix = cameras.at(activeCamera)->projection;
	/*vector<vec4> modified_vertex;
	vector<vec3> modified_vertex_normals;
	vector<vec3> modified_face_normals;*/
	 
	if (drawboundingBox(model) == false) //model is out of the clip volume completly 
		return;
	// Iterate the faces: transform to view frame (position and normals), and project positions (only projected normals if there is a need to draw them)
	int size = model->faces.size();
	for (size_t i = 0; i < size; i++)
	{
		Face cur_face = model->faces.at(i);
		model->faces.at(i).v1.view = ModelViewMatrix * cur_face.v1.position;
		model->faces.at(i).v2.view = ModelViewMatrix * cur_face.v2.position;
		model->faces.at(i).v3.view = ModelViewMatrix * cur_face.v3.position;
		
		model->faces.at(i).v1.projected = ProjectionMatrix * cur_face.v1.view;
		model->faces.at(i).v2.projected = ProjectionMatrix * cur_face.v2.view;
		model->faces.at(i).v3.projected = ProjectionMatrix * cur_face.v3.view;

		vec3 v1_screen(cur_face.v1.projected.x / cur_face.v1.projected.w, cur_face.v1.projected.y / cur_face.v1.projected.w, cur_face.v1.projected.z / cur_face.v1.projected.w);
		model->faces.at(i).v1.screen = m_renderer->viewPortVec(v1_screen);
		vec3 v2_screen(cur_face.v2.projected.x / cur_face.v2.projected.w, cur_face.v2.projected.y / cur_face.v2.projected.w, cur_face.v2.projected.z / cur_face.v2.projected.w);
		model->faces.at(i).v2.screen = m_renderer->viewPortVec(v2_screen);
		vec3 v3_screen(cur_face.v3.projected.x / cur_face.v3.projected.w, cur_face.v3.projected.y / cur_face.v3.projected.w, cur_face.v3.projected.z / cur_face.v3.projected.w);
		model->faces.at(i).v3.screen = m_renderer->viewPortVec(v3_screen);

		cur_face.v1.normal.w = 0;
		cur_face.v2.normal.w = 0;
		cur_face.v3.normal.w = 0;
		model->faces.at(i).v1.view_normal = NormalViewMatrix * cur_face.v1.normal;
		model->faces.at(i).v2.view_normal = NormalViewMatrix * cur_face.v2.normal;
		model->faces.at(i).v3.view_normal = NormalViewMatrix * cur_face.v3.normal;
		model->faces.at(i).v1.view_normal.w = 0;
		model->faces.at(i).v2.view_normal.w = 0;
		model->faces.at(i).v3.view_normal.w = 0;


		model->faces.at(i).view = ModelViewMatrix * cur_face.position;
		model->faces.at(i).projected = ProjectionMatrix * cur_face.projected;
		cur_face.normal.w = 0;
		model->faces.at(i).view_normal = NormalViewMatrix * cur_face.normal;
		model->faces.at(i).view_normal.w = 0;
	}
	
	m_renderer->DrawTriangles(&(model->faces),lights, ambient_scale);

	//for (size_t i = 0; i < size; i++)
	//{
	//	vec4 v(model->vertices.at(i).homogenous);
	//	
	//	v = P * (Tc * (_world_transform * (model->_world_transform * (model->_model_transform * v))));
	//	vec4 vertex_normal = model->vertices.at(i).normal;
	//	vertex_normal[3] = 0;
	//	vertex_normal = P * (Tc * (_world_transform * (model->_normal_world_transform * (model->_normal_model_transform * vertex_normal))));
	//	int j = 0;
	//	if (i%3 == 0)
	//	{
	//		vec4 face_normal = model->faces.at(j++).normal;
	//		face_normal[0] = 0;
	//		face_normal = P * (Tc * (_world_transform * (model->_normal_world_transform * (model->_normal_model_transform * face_normal))));
	//		modified_face_normals.push_back(vec3(face_normal.x, face_normal.y, face_normal.z));
	//	}

	//	modified_vertex.push_back(v);
	//	modified_vertex_normals.push_back(vec3(vertex_normal.x, vertex_normal.y, vertex_normal.z));

	//}

	/*m_renderer->DrawTriangles(&(modified_vertex),model->material, &(modified_vertex_normals), &(modified_face_normals), lights, cameras.at(activeCamera)->eye, ambient_scale);*/

}

void Scene::drawFaceNormals(Model* model)
{
	mat4 ProjectionMatrix = cameras.at(activeCamera)->projection;
	int size = model->faces.size();
	for (size_t i = 0; i < size; i++)
	{
		Face cur_face = model->faces.at(i);
		vec4 dest = cur_face.view + cur_face.view_normal;
		dest = ProjectionMatrix * dest;

		if ((cur_face.projected.w < 0.00001) && (cur_face.projected.w > -0.00001)) continue;
		vec3 v_2d_dest(dest.x / dest.w, dest.y / dest.w);
		v_2d_dest = m_renderer->viewPortVec(v_2d_dest); //View-port
		vec3 v_2d_origin(cur_face.projected.x / cur_face.projected.w, cur_face.projected.y / cur_face.projected.w);
		v_2d_origin = m_renderer->viewPortVec(v_2d_origin); //View-port
		
		m_renderer->DrawLine(v_2d_origin.x, v_2d_dest.x, v_2d_origin.y, v_2d_dest.y,c_blue);

	}
}

void Scene::drawVertexNormals(Model* model)
{
	mat4 ProjectionMatrix = cameras.at(activeCamera)->projection;
	int size = model->faces.size();
	for (size_t i = 0; i < size; i++)
	{
		Face cur_face = model->faces.at(i);
		vec4 v1_dest = cur_face.v1.view + cur_face.v1.view_normal; //dest = origin + normal 
		v1_dest = ProjectionMatrix * v1_dest;
		if ((cur_face.v1.projected.w > 0.00001) || (cur_face.v1.projected.w < -0.00001))
		{
			vec3 v1_2d_dest(v1_dest.x / v1_dest.w, v1_dest.y / v1_dest.w);
			v1_2d_dest = m_renderer->viewPortVec(v1_2d_dest); //View-port
			m_renderer->DrawLine(cur_face.v1.screen.x, v1_2d_dest.x, cur_face.v1.screen.y, v1_2d_dest.y, c_blue);
		}

		vec4 v2_dest = cur_face.v2.view + cur_face.v2.view_normal;
		v2_dest = ProjectionMatrix * v2_dest;
		if ((cur_face.v2.projected.w > 0.00001) || (cur_face.v2.projected.w < -0.00001))
		{
			vec3 v2_2d_dest(v2_dest.x / v2_dest.w, v2_dest.y / v2_dest.w);
			v2_2d_dest = m_renderer->viewPortVec(v2_2d_dest); //View-port
			m_renderer->DrawLine(cur_face.v2.screen.x, v2_2d_dest.x, cur_face.v2.screen.y, v2_2d_dest.y, c_blue);
		}

		vec4 v3_dest = cur_face.v3.view + cur_face.v3.view_normal;
		v3_dest = ProjectionMatrix * v3_dest;
		if ((cur_face.v3.projected.w > 0.00001) || (cur_face.v3.projected.w < -0.00001))
		{
			vec3 v3_2d_dest(v3_dest.x / v3_dest.w, v3_dest.y / v3_dest.w);
			v3_2d_dest = m_renderer->viewPortVec(v3_2d_dest); //View-port
			m_renderer->DrawLine(cur_face.v3.screen.x, v3_2d_dest.x, cur_face.v3.screen.y, v3_2d_dest.y, c_blue);
		}

		//vec4 normal = model->vertices.at(i).normal;
		//normal[3] = 0;
		//vec4 v_origin = model->vertices.at(i).homogenous;
		//normal =  Tc * (_world_transform * (model->_normal_world_transform * (model->_normal_model_transform * normal))); //model-view
		//v_origin = Tc *  ( _world_transform * (model->_world_transform * (model->_model_transform * v_origin))); //model-view
		//vec4 v_dest = v_origin + normal; 
		//v_dest = (P * v_dest); //View-port
		//v_origin = (P * v_origin); //View-port
		//vec3 v_2d_dest(v_dest.x / v_dest.w, v_dest.y / v_dest.w);
		//v_2d_dest = m_renderer->viewPortVec(v_2d_dest); //View-port
		//vec3 v_2d_origin(v_origin.x / v_origin.w, v_origin.y / v_origin.w);
		//v_2d_origin = m_renderer->viewPortVec(v_2d_origin); //View-port
		//m_renderer->DrawLine(v_2d_origin.x, v_2d_dest.x, v_2d_origin.y, v_2d_dest.y,c_blue);
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
	addCamera(vec4(0, 0, 0, 0), vec4(0, 0, -1, 0), vec4(0, 1, 0, 0));
	addLight(vec3(0, 1, 0), vec3(0, -1, 0), POINT_LIGHT);

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
	size = lights.size();
	for (size_t i = 0; i < size; ++i) {
		Light* curr_l = lights.at(i);
		delete curr_l;
	}
	lights.clear();
	activeModel = -1;
	activeCamera = -1;
	int nModels = 0;
	int nCameras = 0;
	addCamera(vec4(0, 0, 2, 0), vec4(0, 0, -1, 0), vec4(0, 1, 0, 0));
	changeShading(FLAT);
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
	vec3 modified_eye_3d;
	for (size_t i = 0; i < nCameras; i++)
	{
		if (i == activeCamera)
			continue;
		modified_eye = P * (Tc * cameras.at(i)->eye);  //Projection
		modified_eye_3d = (modified_eye.x, modified_eye.y);
		modified_eye_3d = m_renderer->viewPortVec(modified_eye_3d);//View-port
		m_renderer->DrawLine(modified_eye_3d[0] - 10, modified_eye_3d[0] + 10, modified_eye_3d[1], modified_eye_3d[1],c_yellow);
		m_renderer->DrawLine(modified_eye_3d[0], modified_eye_3d[0], modified_eye_3d[1] - 10, modified_eye_3d[1] + 10, c_yellow);
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

vec3 Scene::getCurrentViewPort()
{
	return m_renderer->viewPortVec(vec3(1));
}


void Scene::addLight(const vec4 location, const vec4 direction, LIGHT_TYPE light_type, Color color)
{
	Light * new_light = new Light();
	new_light->position = vec4(location.x, location.y, location.z, 1);
	new_light->direction = vec4(direction.x, direction.y, direction.z, 0);
	new_light->light_type = light_type;
	new_light->color = color;
	lights.push_back(new_light);
}

