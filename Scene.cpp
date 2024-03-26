//#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include "InitShader.h"
#include <GLFW/glfw3.h>

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

void Model::changeUniformShininess(GLfloat coefficient)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	for (auto it = vertices.begin(); it != vertices.end(); it++)
	{
		(*it).shininess_coefficient = coefficient;
	}
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::changeUniformEmissiveColor(vec4 color)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	for (auto it = vertices.begin(); it != vertices.end(); it++)
	{
		(*it).emissive_color = color;
	}
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::changeUniformSpecularColor(vec4 color)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	for (auto it = vertices.begin(); it != vertices.end(); it++)
	{
		(*it).specular_color = color;
	}
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::changeUniformDiffuseColor(vec4 color) 
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	for (auto it = vertices.begin(); it != vertices.end(); it++)
	{
		(*it).diffuse_color = color;
	}
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::colorByNormal()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	color_by_normal = true;
	color_by_pos = false;
	for (auto it = vertices.begin(); it != vertices.end(); it++)
	{
		
		vec4 color(abs((*it).normal.x), abs((*it).normal.y), abs((*it).normal.z), 1);
		(*it).diffuse_color = color;
		(*it).specular_color = color;
		(*it).emissive_color = 0.25 * color;
	}
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::colorByPosition()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	color_by_normal = false;
	color_by_pos = true;
	for (auto it = vertices.begin(); it != vertices.end(); it++)
	{
		vec4 color(abs((*it).position.x), abs((*it).position.y), abs((*it).position.z), 1);
		(*it).diffuse_color = color;
		(*it).specular_color = color;
		(*it).emissive_color = 0.25 * color;
	}
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Scene::loadOBJModel(string fileName)
{
	MeshModel* model = new MeshModel(fileName, ActiveProgramID);
	model->shading_type = shading_type;
	models.push_back(model);

	nModels++;
	activeModel++;
}

void Scene::loadPrimModel(string type)
{
	PrimMeshModel* model = new PrimMeshModel(type);
	model->shading_type = shading_type;
	model->programID = ActiveProgramID;
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

void Model::updateModel(Camera active_camera)
{


	/*for (auto vertex_it = vertices.begin(); vertex_it != vertices.end(); vertex_it++)
	{
		(*vertex_it).view_position = ModelViewMatrix * (*vertex_it).raw_position;
		(*vertex_it).projected = active_camera.projection * (*vertex_it).view_position;
	}

	for (auto normal_it = normals.begin(); normal_it != normals.end(); normal_it++)
	{
		(*normal_it).original_direction.w = 0;
		(*normal_it).view_direction = NormalViewMatrix * (*normal_it).original_direction;
		(*normal_it).view_direction.w = 0;
	}
	for (auto face_it = faces.begin(); face_it != faces.end(); face_it++)
	{
		(*face_it).face_normal.original_direction.w = 0;
		(*face_it).face_normal.view_direction = NormalViewMatrix * (*face_it).face_normal.original_direction;
		(*face_it).face_normal.view_direction.w = 0;
		(*face_it).face_center.view_position = ModelViewMatrix * (*face_it).face_center.raw_position;
		(*face_it).face_center.projected = active_camera.projection * (*face_it).face_center.view_position;
	}*/
}

bool Model::inViewVolume(Camera active_camera)
{
	//mat4 ModelViewMatrix = active_camera.cTransform * (_world_transform * _model_transform);
	//for (auto box_it = bounding_box.begin(); box_it != bounding_box.end(); box_it++)
	//{
	//	(*box_it).view_position = ModelViewMatrix * (*box_it).raw_position;
	//	(*box_it).projected = active_camera.projection * (*box_it).view_position;
	//	if (((*box_it).projected.w > -epsilon) && ((*box_it).projected.w < epsilon))
	//		(*box_it).projected.w = epsilon;
	//	(*box_it).canonical.x = (*box_it).projected.x / (*box_it).projected.w; //not really screen but just after devision by w
	//	(*box_it).canonical.y = (*box_it).projected.y / (*box_it).projected.w;
	//	(*box_it).canonical.z = (*box_it).projected.z / (*box_it).projected.w;
	//}
	//vec3 min = bounding_box[0].screen;
	//vec3 max = bounding_box[0].screen;
	//for (auto box_it = bounding_box.begin(); box_it != bounding_box.end(); box_it++)
	//{
	//	if ((*box_it).screen.x < min.x)
	//		min.x = (*box_it).screen.x;
	//	if ((*box_it).screen.x > max.x)
	//		max.x = (*box_it).screen.x;

	//	if ((*box_it).screen.y < min.y)
	//		min.y = (*box_it).screen.y;
	//	if ((*box_it).screen.y > max.y)
	//		max.y = (*box_it).screen.y;

	//	if ((*box_it).screen.z < min.z)
	//		min.z = (*box_it).screen.z;
	//	if ((*box_it).screen.z > max.z)
	//		max.z = (*box_it).screen.z;
	//}

	//if ((min.x > -1 && min.x < 1) || (max.x > -1 && max.x < 1)) // x in the view volume 
	//{
	//	if ((min.y > -1 && min.y < 1) || (max.y > -1 && max.y < 1)) // y in  view volume 
	//	{
	//		if ((min.z > -1 && min.z < 1) || (max.z > -1 && max.z < 1)) // z in view volume 
	//			return true;
	//		else if (min.z < -1 && max.z > 1) //z consists view volume 
	//			return true;
	//	}
	//	else if ((min.y < -1 && max.y > 1)) // y consists view volume
	//	{
	//		if ((min.z > -1 && min.z < 1) || (max.z > -1 && max.z < 1)) // z in view volume 
	//			return true;
	//		else if (min.z < -1 && max.z > 1) //z consists view volume 
	//			return true;

	//	}
	//}

	//else if ((min.x < -1 && max.x > 1)) // x consists view volume 
	//{
	//	if ((min.y > -1 && min.y < 1) || (max.y > -1 && max.y < 1)) // y in  view volume 
	//	{
	//		if ((min.z > -1 && min.z < 1) || (max.z > -1 && max.z < 1)) // z in view volume 
	//			return true;
	//		else if (min.z < -1 && max.z > 1) //z consists view volume 
	//			return true;
	//	}
	//	else if ((min.y < -1 && max.y > 1)) // y consists view volume
	//	{
	//		if ((min.z > -1 && min.z < 1) || (max.z > -1 && max.z < 1)) // z in view volume 
	//			return true;
	//		else if (min.z < -1 && max.z > 1) //z consists view volume 
	//			return true;

	//	}
	//}
	//return false;
	return true;
}

void Scene::ChangeAntiAliasingResolution(int resolution)
{
	//m_renderer->antialiasing_resolution = resolution;
	//m_renderer->Reshape(ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y - 20);
}

void Scene::draw()
{
	GLfloat projection[16];
	matToArray(projection, cameras.at(activeCamera)->projection);
	GLuint projectionMat_loc = glGetUniformLocation(ActiveProgramID, "projection");
	glUniformMatrix4fv(projectionMat_loc, 1, GL_FALSE, projection);

	GLfloat cameraMat[16];
	matToArray(cameraMat, cameras.at(activeCamera)->cTransform);
	GLuint cameraMat_loc = glGetUniformLocation(ActiveProgramID, "cameraMat");
	glUniformMatrix4fv(cameraMat_loc, 1, GL_FALSE, cameraMat);

	GLuint nLights_loc = glGetUniformLocation(ActiveProgramID, "nLights");
	glUniform1i(nLights_loc, nLights);

	vec4 res_ambient = ambient_color * ambient_scale;
	GLuint ambient_color_loc = glGetUniformLocation(ActiveProgramID, "ambient_color");
	glUniform4fv(ambient_color_loc,1, &(res_ambient[0]));

	for (int i = 0; i < nLights; ++i) {
		std::string lightName = "lights[" + std::to_string(i) + "]";
		glUniform4fv(glGetUniformLocation(ActiveProgramID, (lightName + ".color").c_str()), 1, &(lights.at(i)->color[0]));
		glUniform1f(glGetUniformLocation(ActiveProgramID, (lightName + ".intensity").c_str()), lights.at(i)->intensity);
		glUniform4fv(glGetUniformLocation(ActiveProgramID, (lightName + ".direction").c_str()), 1, &(lights.at(i)->direction[0]));
		glUniform1i(glGetUniformLocation(ActiveProgramID, (lightName + ".light_type").c_str()), lights.at(i)->light_type);
		glUniform4fv(glGetUniformLocation(ActiveProgramID, (lightName + ".position").c_str()), 1, &(lights.at(i)->position[0]));
	}

	/*for (size_t i = 0; i <lights.size(); i++)
	{
		Light* current_light = lights.at(i);
		mat4 ProjectionMatrix = cameras.at(activeCamera)->cTransform;
		current_light->view_position = ProjectionMatrix * current_light->position; 
		current_light->view_direction = ProjectionMatrix * current_light->direction;
	}*/
	
	for (auto model_it = models.begin(); model_it  != models.end(); model_it++)
	{
		drawModel(*model_it);
		if (displayFnormal) 
		{
			drawFaceNormals((*model_it));
		}
		if (displayVnormal)
		{
			drawVertexNormals((*model_it));
		}
		if (displayBoundingBox)
		{
			drawboundingBox((*model_it));
		}
	}

}

void Scene::changeShading(SHADING shading_type)
{
	if (shading_type == MESH)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (shading_type == FLAT)
	{
		shading_type = shading_type;
		ActiveProgramID = FlatProgramID;
		for (auto model_it = models.begin(); model_it != models.end(); model_it++)
		{
			(*model_it)->programID = FlatProgramID;
			(*model_it)->setVertexAttributes();
		}
		glUseProgram(ActiveProgramID);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	else if (shading_type == GOURAUD)
	{
		shading_type = shading_type;
		ActiveProgramID = GouraudProgramID;
		for (auto model_it = models.begin(); model_it != models.end(); model_it++)
		{
			(*model_it)->programID = GouraudProgramID;
			(*model_it)->setVertexAttributes();
		}
		glUseProgram(ActiveProgramID);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	else if (shading_type == PHONG)
	{
		shading_type = shading_type;
		ActiveProgramID = PhongProgramID;
		for (auto model_it = models.begin(); model_it != models.end(); model_it++)
		{
			(*model_it)->programID = PhongProgramID;
			(*model_it)->setVertexAttributes();
		}
		glUseProgram(ActiveProgramID);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
}

void Scene::drawboundingBox(Model* model)
{
	/*for (auto box_it = model->bounding_box.begin(); box_it != model->bounding_box.end(); box_it++)
	{
		(*box_it).screen = m_renderer->viewPortVec((*box_it).screen);
	}
	Color box_color(0.3,0.3,0.4);
	m_renderer->DrawBox(&(model->bounding_box), box_color);*/
}

void Scene::drawModel(Model* model)
{
	mat4 model_view_matrix = cameras.at(activeCamera)->cTransform * (model->_world_transform * model->_model_transform);
	mat4 normal_view_matrix = cameras.at(activeCamera)->cTransform * (model->_normal_world_transform * model->_normal_model_transform);
	GLfloat model_view[16];
	matToArray(model_view, model_view_matrix);
	GLfloat normal_view[16];
	matToArray(normal_view, normal_view_matrix);
	

	
	GLuint modelViewMatrix_loc = glGetUniformLocation(ActiveProgramID, "modelview");
	glUniformMatrix4fv(modelViewMatrix_loc, 1, GL_FALSE, model_view);
	GLuint normalViewMatrix_loc = glGetUniformLocation(ActiveProgramID, "normalMat");
	glUniformMatrix4fv(normalViewMatrix_loc, 1, GL_FALSE, normal_view);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error while drawing model in scene : " << error << std::endl;
	}
	model->draw();

}

void Scene::drawVertexNormals(Model* model)
{

	/*mat4 ProjectionMatrix = cameras.at(activeCamera)->projection;
	for (auto face_it = model->faces.begin(); face_it != model->faces.end(); face_it++)
	{
		vec4 v1_dest = (*face_it).v1->view_position + (normal_scale * (*face_it).v1_normal->view_direction);
		v1_dest = ProjectionMatrix * v1_dest;
		vec3 screen_v1_dest(v1_dest.x / v1_dest.w, v1_dest.y / v1_dest.w, v1_dest.z / v1_dest.w);
		screen_v1_dest = m_renderer->viewPortVec(screen_v1_dest);
		m_renderer->DrawLine((*face_it).v1->screen.x, screen_v1_dest.x, (*face_it).v1->screen.y, screen_v1_dest.y, c_blue);

		vec4 v2_dest = (*face_it).v2->view_position + (normal_scale * (*face_it).v2_normal->view_direction);
		v2_dest = ProjectionMatrix * v2_dest;
		vec3 screen_v2_dest(v2_dest.x / v2_dest.w, v2_dest.y / v2_dest.w, v2_dest.z / v2_dest.w);
		screen_v2_dest = m_renderer->viewPortVec(screen_v2_dest);
		m_renderer->DrawLine((*face_it).v2->screen.x, screen_v2_dest.x, (*face_it).v2->screen.y, screen_v2_dest.y, c_blue);

		vec4 v3_dest = (*face_it).v3->view_position + (normal_scale * (*face_it).v3_normal->view_direction);
		v3_dest = ProjectionMatrix * v3_dest;
		vec3 screen_v3_dest(v3_dest.x / v3_dest.w, v3_dest.y / v3_dest.w, v3_dest.z / v3_dest.w);
		screen_v3_dest = m_renderer->viewPortVec(screen_v3_dest);
		m_renderer->DrawLine((*face_it).v3->screen.x, screen_v3_dest.x, (*face_it).v3->screen.y, screen_v3_dest.y, c_blue);
	}*/
}

void Scene::drawFaceNormals(Model* model)
{
	/*mat4 ModelViewMatrix = cameras.at(activeCamera)->cTransform * (model->_world_transform * model->_model_transform);
	mat4 NormalViewMatrix = cameras.at(activeCamera)->cTransform * (model->_normal_world_transform * model->_normal_model_transform);
	mat4 ProjectionMatrix = cameras.at(activeCamera)->projection;
	for (auto face_it = model->faces.begin(); face_it != model->faces.end(); face_it++)
	{
		vec4 dest = (*face_it).face_center.view_position + (normal_scale * (*face_it).face_normal.view_direction);
		dest = ProjectionMatrix * dest;
		vec3 screen_dest((dest.x / dest.w), (dest.y / dest.w), (dest.z / dest.w));
		screen_dest = m_renderer->viewPortVec(screen_dest);
		m_renderer->DrawLine((*face_it).face_center.screen.x, screen_dest.x, (*face_it).face_center.screen.y, screen_dest.y, c_blue);
	}*/
}

void Scene::drawDemo()
{
	const int pnum = 3;
	static const GLfloat points[pnum][4] = {
		{-0.1, -0.1f, 0.0f,1.0f},
		{0.1f, -0.1f, 0.0f,1.0f},
		{0.0f,  0.1f, 0.0f,1.0f}
	};

	GLuint program = InitShader("minimal_vshader.glsl",
		"minimal_fshader.glsl");

	glUseProgram(program);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points),
		points, GL_STATIC_DRAW);


	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_LINE_LOOP, 0, pnum);
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
	//_world_transform = mat4(1.0);
	//m_renderer = new Renderer(512,512);
	//m_renderer = new Renderer(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height - 20);
	addCamera(vec4(0, 0, 0, 0), vec4(0, 0, -1, 0), vec4(0, 1, 0, 0));
	addLight(vec3(-1, 1, 0), vec3(0, -1, 0), POINT_LIGHT);

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
	activeLight = -1;
	nModels = 0;
	nCameras = 0;
	nLights = 0;
	addCamera(vec4(0, 0, 0, 0), vec4(0, 0, -1, 0), vec4(0, 1, 0, 0));
	changeShading(FLAT);
}

void Scene::addModel(Model* model)
{
	model->shading_type = shading_type;
	model->programID = ActiveProgramID;
	models.push_back(model);
	nModels++; 
	activeModel++;
}

void Scene::LookAtModel()
{
	if (nModels == 0)
		return;
	vec4 v = (models.at(activeModel)->_normal_world_transform * (models.at(activeModel)->_normal_model_transform * models.at(activeModel)->_center_of_mass)); //model-view 
	cameras.at(activeCamera)->LookAt(cameras.at(activeCamera)->eye, v, cameras.at(activeCamera)->up);
}

void Scene::drawCameras()
{
	//mat4 Tc = cameras.at(activeCamera)->cTransform;
	//mat4 P = cameras.at(activeCamera)->projection;
	//vec4 modified_eye;
	//vec3 modified_eye_3d;
	//for (size_t i = 0; i < nCameras; i++)
	//{
	//	if (i == activeCamera)
	//		continue;
	//	modified_eye = P * (Tc * cameras.at(i)->eye);  //Projection
	//	modified_eye_3d = (modified_eye.x, modified_eye.y);
	//	modified_eye_3d = m_renderer->viewPortVec(modified_eye_3d);//View-port
	//	m_renderer->DrawLine(modified_eye_3d[0] - 10, modified_eye_3d[0] + 10, modified_eye_3d[1], modified_eye_3d[1],c_yellow);
	//	m_renderer->DrawLine(modified_eye_3d[0], modified_eye_3d[0], modified_eye_3d[1] - 10, modified_eye_3d[1] + 10, c_yellow);
	//}
	
}

void Scene::setCameraOrtho(const float left, const float right,const float bottom, const float top,const float zNear, const float zFar)
{
	cameras.at(activeCamera)->Ortho(left, right, bottom, top, zNear, zFar);
}

void Scene::setCameraFrustum(const float left, const float right,const float bottom, const float top,const float zNear, const float zFar)
{
	cameras.at(activeCamera)->Frustum(left, right, bottom, top, zNear, zFar);
}

void Scene::setCameraPerspective(const float fovy, const float aspect,const float zNear, const float zFar)
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


void Scene::addLight(const vec4 location, const vec4 direction, LIGHT_TYPE light_type, vec4 color)
{
	Light * new_light = new Light();
	new_light->position = vec4(location.x, location.y, location.z, 1);
	new_light->direction = vec4(direction.x, direction.y, direction.z, 0);
	new_light->light_type = light_type;
	new_light->color = color;
	lights.push_back(new_light);
	activeLight++;
	nLights++;
}

