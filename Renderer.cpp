//#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_imGUI.h"
#include "InitShader.h"
#include "GLFW\glfw3.h"

//using namespace Renderer;

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :m_width(512), m_height(512)
{
	InitOpenGLRendering();
	CreateBuffers(512, 512);
}
Renderer::Renderer(int width, int height) : m_width(width), m_height(height)
{
	InitOpenGLRendering();
	CreateBuffers(width, height);
}

Renderer::~Renderer(void){}

void Renderer::Init(){}

void Renderer::DrawTriangles(const vector<Face>* faces, vector<Light*> lights, GLfloat ambient_scale, bool uniform_color)
{
	vec3 min_values;
	vec3 max_values;
	GLfloat min_x, min_y, min_z, max_x, max_y, max_z;
	int j = 0;
	int size = faces->size();
	for (size_t i = 0; i < size; i++)
	{
		Face cur_face = faces->at(i);
		//clipping method
		//default set to phong
		Color flat_color;
		Color c1, c2, c3;
		vec4 face_flat_normal;
		MATERIAL face_material = cur_face.v1->material;
		switch (shadingType)
		{
			case FLAT:
				// TODO:: checl what to do in case of different materials
				//face_flat_normal = normalize( cur_face.v1_normal->view_direction + cur_face.v2_normal->view_direction + cur_face.v3_normal->view_direction);
				if (!uniform_color)
				{
					face_material = interpulateMaterial(cur_face.v1->material, cur_face.v2->material, cur_face.v3->material);
				}
				flat_color = calcColor(face_material, truncateVec4(cur_face.face_normal.view_direction), truncateVec4(cur_face.face_center.view_position), lights, ambient_scale);
				fillFlatTriangle(cur_face, flat_color);
				break;
			case GOURAUD:
				 c1 = calcColor(cur_face.v1->material, truncateVec4(cur_face.v1_normal->view_direction), truncateVec4(cur_face.v1->view_position), lights, ambient_scale);
				 c2 = calcColor(cur_face.v2->material, truncateVec4(cur_face.v2_normal->view_direction), truncateVec4(cur_face.v2->view_position), lights, ambient_scale);
				 c3 = calcColor(cur_face.v3->material, truncateVec4(cur_face.v3_normal->view_direction), truncateVec4(cur_face.v3->view_position), lights, ambient_scale);
				fillGouraudTriangle(cur_face, c1, c2, c3);
				break;
					
			default:
				//TODO: check truncation correct
				fillPhongTriangle(cur_face, lights, ambient_scale, uniform_color);
				break;
		}

	
	}
	
}

void Renderer::fillPhongTriangle(Face face, vector<Light*> lights, GLfloat ambient_scale, bool uniform_color)
{
	// Sort vertices by y-coordinate
	vec3 v1 = face.v1->screen;
	vec3 v2 = face.v2->screen;
	vec3 v3 = face.v3->screen;
	vec3 temp_vec;
	if (v1.y < v2.y)
	{
		temp_vec = v2;
		v2 = v1;
		v1 = temp_vec;
	}
	if (v2.y < v3.y)
	{
		temp_vec = v3;
		v3 = v2;
		v2 = temp_vec;
	}
	if (v1.y < v2.y)
	{
		temp_vec = v2;
		v2 = v1;
		v1 = temp_vec;
	}

	GLfloat slope1 = 0;
	GLfloat slope2 = 0;
	if (v1.y - v2.y != 0)
		slope1 = (v1.x - v2.x) / (v1.y - v2.y);
	if (v1.y - v3.y != 0)
		slope2 = (v1.x - v3.x) / (v1.y - v3.y);
	GLfloat x1 = v1.x;
	GLfloat x2 = v1.x;
	int y = min(v1.y, GLfloat(m_height));
	for (; y >= v2.y; y--)
	{
		drawPhongScanline(x1, x2, y, face, lights, ambient_scale, uniform_color);
		x1 -= slope1;
		x2 -= slope2;
	}
	x1 = v2.x;
	if (v2.y - v3.y == 0)
		slope1 = 0;
	else
		slope1 = (v2.x - v3.x) / (v2.y - v3.y);
	for (; y >= v3.y; y--)
	{
		drawPhongScanline(x1, x2, y, face, lights, ambient_scale, uniform_color);
		x1 -= slope1;
		x2 -= slope2;
	}
}

void Renderer::fillGouraudTriangle(Face face, Color c1, Color c2, Color c3)
{
	// Sort vertices by y-coordinate
	vec3 v1 = face.v1->screen;
	vec3 v2 = face.v2->screen;
	vec3 v3 = face.v3->screen;
	vec3 temp_vec;
	if (v1.y < v2.y)
	{
		temp_vec = v2;
		v2 = v1;
		v1 = temp_vec;
	}
	if (v2.y < v3.y)
	{
		temp_vec = v3;
		v3 = v2;
		v2 = temp_vec;
	}
	if (v1.y < v2.y)
	{
		temp_vec = v2;
		v2 = v1;
		v1 = temp_vec;
	}

	GLfloat slope1 = 0;
	GLfloat slope2 = 0;
	if (v1.y - v2.y != 0)
		slope1 = (v1.x - v2.x) / (v1.y - v2.y);
	if (v1.y - v3.y != 0)
		slope2 = (v1.x - v3.x) / (v1.y - v3.y);
	GLfloat x1 = v1.x;
	GLfloat x2 = v1.x;
	int y = min(v1.y, GLfloat(m_height));
	for (; y >= v2.y; y--)
	{
		drawGouraudScanline(x1, x2, y, face, c1, c2, c3);
		x1 -= slope1;
		x2 -= slope2;
	}
	x1 = v2.x;
	if (v2.y - v3.y == 0)
		slope1 = 0;
	else
		slope1 = (v2.x - v3.x) / (v2.y - v3.y);
	for (; y >= v3.y; y--)
	{
		drawGouraudScanline(x1, x2, y, face, c1, c2, c3);
		x1 -= slope1;
		x2 -= slope2;
	}

}

void Renderer::fillFlatTriangle(Face face, Color color)
{
	// Sort vertices by y-coordinate
	vec3 v1 = face.v1->screen;
	vec3 v2 = face.v2->screen;
	vec3 v3 = face.v3->screen;
	vec3 temp_vec;
	if (v1.y < v2.y)
	{
		temp_vec = v2;
		v2 = v1;
		v1 = temp_vec;
	}
	if (v2.y < v3.y)
	{
		temp_vec = v3;
		v3 = v2;
		v2 = temp_vec;
	}
	if (v1.y < v2.y)
	{
		temp_vec = v2;
		v2 = v1;
		v1 = temp_vec;
	}

	GLfloat slope1 = 0;
	GLfloat slope2 = 0;
	if (v1.y - v2.y != 0)
		slope1 = (v1.x - v2.x) / (v1.y - v2.y);
	if (v1.y - v3.y != 0)
		slope2 = (v1.x - v3.x) / (v1.y - v3.y);
	GLfloat x1 = v1.x;
	GLfloat x2 = v1.x;
	int y = min(v1.y, GLfloat(m_height));
	for (; y >= v2.y; y--)
	{
		drawFlatScanline(x1, x2, y, v1, v2 , v3, color); 
		x1 -= slope1;
		x2 -= slope2;
	}
	x1 = v2.x;
	if (v2.y - v3.y == 0)
		slope1 = 0;
	else
		slope1 = (v2.x - v3.x) / (v2.y - v3.y);
	for (; y >= v3.y; y--)
	{
		drawFlatScanline(x1, x2, y, v1, v2, v3, color);
		x1 -= slope1;
		x2 -= slope2;
	}
}

void Renderer::drawPhongScanline(int x1, int x2, int y, Face face, vector<Light*> lights, GLfloat ambient_scale, bool uniform_color)
{
	vec3 v1 = face.v1->screen;
	vec3 v2 = face.v2->screen;
	vec3 v3 = face.v3->screen;
	if (x1 > x2) std::swap(x1, x2);
	for (int x = x1; x <= x2; x++)
	{
		GLfloat z = getDepth(x, y, v1, v2, v3);

		if ((x < m_width) && (y < m_height) && (0 < x) && (0 < y))
		{
			if (m_zbuffer[x + m_width * y] > z)
			{
				m_zbuffer[x + m_width * y] = z;
				vec2 d1(x - v1.x, y - v1.y);
				vec2 d2(x - v2.x, y - v2.y);
				vec2 d3(x - v3.x, y - v3.y);
				vec3 normal = truncateVec4(face.v1_normal->view_direction * length(d1) + face.v2_normal->view_direction * length(d2) + face.v3_normal->view_direction * length(d3));
				vec3 p = truncateVec4(face.v1->view_position * length(d1) + face.v2->view_position * length(d2) + face.v3->view_position * length(d3));
				//TODO: chack what to os in case of different materials for vector 
				MATERIAL point_material = face.v1->material;
				if (!uniform_color)
				{
					point_material = interpulateMaterial(face.v1->material, face.v2->material, face.v3->material, length(d1), length(d2), length(d3));
				}
				Color color = calcColor(point_material, normal, p, lights, ambient_scale);
				DrawPixel(x, y, color);
			}
		}
	}
}

void Renderer::drawGouraudScanline(int x1, int x2, int y, Face face, Color c1, Color c2, Color c3)
{
	if (x1 > x2) std::swap(x1, x2);
	vec3 v1 = face.v1->screen;
	vec3 v2 = face.v2->screen;
	vec3 v3 = face.v3->screen;
	for (int x = x1; x <= x2; x++)
	{
		GLfloat z = getDepth(x, y, v1, v2, v3);
		if ((x < m_width) && (y < m_height) && (0 < x) && (0 < y))
		{
			if (m_zbuffer[x + m_width * y] > z)
			{
				m_zbuffer[x + m_width * y] = z;
				vec2 d1(x - v1.x, y - v1.y);
				vec2 d2(x - v2.x, y - v2.y);
				vec2 d3(x - v3.x, y - v3.y);
				GLfloat length1 = length(d1);
				GLfloat length2 = length(d2);
				GLfloat length3 = length(d3);

				GLfloat total_length = length1 + length2 + length3;
				//avoiding color bounding operation
				GLfloat	red = (c1.r * length1 + c2.r * length2 + c3.r * length3) / total_length;
				GLfloat	green = (c1.g * length1 + c2.g* length2 + c3.g * length3) / total_length;
				GLfloat	blue = (c1.b * length1 + c2.b * length2 + c3.b * length3) / total_length;
				Color color(red, green, blue);
				DrawPixel(x, y, color);
			}
		}
	}
}

void Renderer::drawFlatScanline(int x1, int x2, int y, vec3 v1, vec3 v2, vec3 v3, Color color)
{
	if (x1 > x2) std::swap(x1, x2);
	for (int x = x1; x <= x2; x++)
	{
		GLfloat z = getDepth(x, y, v1, v2, v3);

		if ((x < m_width) && (y < m_height) && (0 < x) && (0 < y))
		{
			if (m_zbuffer[x + m_width * y] > z)
			{
				m_zbuffer[x + m_width * y] = z;
				DrawPixel(x, y, color);
			}
		}
	}
}

bool Renderer::liangBarsky(vec3 v1, vec3 v2)
{
	return true;
}
//Critical assumption: calculating color is done using cannonial coordinates
//Second assumption: to avoid repeating division by w, assume values are divided (3d cannonial)
//The following are vectors(no need for w): normal, per-light direction
//The following are locations(w is needed, assumption is division already done): p , per-light location(for point_light), camera location
Color Renderer::calcColor(MATERIAL material, vec3 normal, vec3 p, vector<Light*> lights, GLfloat ambient_scale)
{
	Color color = material.emissive_color;
	vec3 l; //direction from point to light(per light)
	vec3 v = normalize(-p); //direction from point to camera - point is in view frame;
	//ambient
	color = color + (material.ambient_color * ambient_scale);
	//itarate per light source
	for (size_t i = 0; i < lights.size(); i++)
	{
		Light* current_light = lights.at(i);
		if (current_light->light_type == POINT_LIGHT)
		{
			l = normalize(truncateVec4(current_light->view_position) - p);
		}
		else
		{
			l = normalize(truncateVec4(current_light->view_direction));
		}

		GLfloat LN = max(0.0f,dot(l, normal));
		vec3 r = normalize((2 * normal * LN) - l);

		//diffuse
		color = color + (current_light->color) * (material.diffuse_color * LN * current_light->intensity);
		// specular
		GLfloat Shininess = pow(abs(dot(r, v)), material.shininess_coefficient);
		color = color + (current_light->color) * (material.specular_color * Shininess * current_light->intensity);
	}

	return color;
}

MATERIAL interpulateMaterial(MATERIAL v1_material, MATERIAL v2_material, MATERIAL v3_material, GLfloat d1, GLfloat d2, GLfloat d3)
{
	MATERIAL m;
	m.shininess_coefficient = (v1_material.shininess_coefficient * d1 + v2_material.shininess_coefficient * d2 + v3_material.shininess_coefficient * d3) / (d1 + d2 + d3);
	m.emissive_color = interpulateColor(v1_material.emissive_color, v2_material.emissive_color, v3_material.emissive_color, d1, d2, d3);
	m.diffuse_color = interpulateColor(v1_material.diffuse_color, v2_material.diffuse_color, v3_material.diffuse_color, d1, d2, d3);
	m.specular_color = interpulateColor(v1_material.specular_color, v2_material.specular_color, v3_material.specular_color, d1, d2, d3);
	m.ambient_color = interpulateColor(v1_material.ambient_color, v2_material.ambient_color, v3_material.ambient_color, d1, d2, d3);
	return m;
}

Color interpulateColor(Color v1_color, Color v2_color, Color v3_color, GLfloat d1, GLfloat d2, GLfloat d3)
{
	GLfloat r = (v1_color.r * d1 + v2_color.r * d2 + v3_color.r * d3) / (d1 + d2 + d3);
	GLfloat g = (v1_color.g * d1 + v2_color.g * d2 + v3_color.g * d3) / (d1 + d2 + d3);
	GLfloat b = (v1_color.b * d1 + v2_color.b * d2 + v3_color.b * d3) / (d1 + d2 + d3);
	return Color(r, g, b);
}

void Renderer::DrawBox(const vector<vec3>* vertices, Color color)
{

	//bottom
	DrawLine(vertices->at(0).x, vertices->at(1).x, vertices->at(0).y, vertices->at(1).y, color);
	DrawLine(vertices->at(1).x, vertices->at(2).x, vertices->at(1).y, vertices->at(2).y, color);
	DrawLine(vertices->at(2).x, vertices->at(3).x, vertices->at(2).y, vertices->at(3).y, color);
	DrawLine(vertices->at(2).x, vertices->at(0).x, vertices->at(2).y, vertices->at(0).y, color);
	//top
	DrawLine(vertices->at(4).x, vertices->at(5).x, vertices->at(4).y, vertices->at(5).y, color);
	DrawLine(vertices->at(5).x, vertices->at(6).x, vertices->at(5).y, vertices->at(6).y, color);
	DrawLine(vertices->at(6).x, vertices->at(7).x, vertices->at(6).y, vertices->at(7).y, color);
	DrawLine(vertices->at(7).x, vertices->at(4).x, vertices->at(7).y, vertices->at(4).y, color);
	//front
	DrawLine(vertices->at(0).x, vertices->at(4).x, vertices->at(0).y, vertices->at(4).y, color);
	DrawLine(vertices->at(1).x, vertices->at(5).x, vertices->at(1).y, vertices->at(5).y, color);
	//back
	DrawLine(vertices->at(3).x, vertices->at(7).x, vertices->at(3).y, vertices->at(7).y, color);
	DrawLine(vertices->at(2).x, vertices->at(6).x, vertices->at(2).y, vertices->at(6).y, color);
}

void Renderer::CreateBuffers(int width, int height)
{
	if (m_outBuffer != NULL)
		delete m_outBuffer;
	if (m_zbuffer != NULL)
		delete m_zbuffer;
	m_width = width;
	m_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width * m_height];
	m_zbuffer = new float[m_width * m_height];
	for (size_t i = 0; i < m_width * m_height; i++)
	{
		m_zbuffer[i] = 1.1;
	}
	
}

void Renderer::CreateLocalBuffer()
{
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, 256, i, 0)] = 1;	m_outBuffer[INDEX(m_width, 256, i, 1)] = 0;	m_outBuffer[INDEX(m_width, 256, i, 2)] = 0;

	}
	//horizontal line
	for (int i = 0; i < m_width; i++)
	{
		m_outBuffer[INDEX(m_width, i, 256, 0)] = 1;	m_outBuffer[INDEX(m_width, i, 256, 1)] = 0;	m_outBuffer[INDEX(m_width, i, 256, 2)] = 1;

	}
}

void Renderer::DrawPixel(int x, int y, Color color) {
	if ((x < m_width) && (y < m_height) && (0 < x) && ( 0 < y)) {
		m_outBuffer[INDEX(m_width, x, y, 0)] = color.r;
		m_outBuffer[INDEX(m_width, x, y, 1)] = color.g;
		m_outBuffer[INDEX(m_width, x, y, 2)] = color.b;
	}
	

}

void Renderer::DrawLine(int x1, int x2, int y1, int y2, Color color) {

	int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
	int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
	int err = dx + dy, e2;

	while (true) {
		DrawPixel(x1, y1, color);
		if (x1 == x2 && y1 == y2) break;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x1 += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y1 += sy;
		}

	}
}

void Renderer::Reshape(int width, int height)
{
	m_width = width;
	m_height = height;
}

void Renderer::ClearColorBuffer()
{
	for (size_t i = 0; i < m_height; i++)
	{
		for (size_t j = 0; j < m_width; j++)
		{
			m_outBuffer[(i * m_width + j) * 3] = 0;
			m_outBuffer[(i * m_width + j) * 3 + 1 ] = 0;
			m_outBuffer[(i * m_width + j) * 3 + 2] = 0;
		}

	}

	
}

void Renderer::ClearDepthBuffer()
{
	for (size_t i = 0; i < m_height * m_width; i++)
	{
		m_zbuffer[i] = 1.1;
	}
}

/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[] = {
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1 };
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	GLint  vPosition = glGetAttribLocation(program, "vPosition");

	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0);

	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid*)sizeof(vtc));
	glProgramUniform1i(program, glGetUniformLocation(program, "texture"), 0);
	a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();

	glBindVertexArray(gScreenVtc);
	a = glGetError();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	a = glGetError();
	//glutSwapBuffers();
	//glfwSwapBuffers(glfwGetCurrentContext());
	a = glGetError();
}

vec3 Renderer::viewPortVec(vec3 cannonial)
{
	GLfloat factor = min(m_width, m_height);
	//to fix center
	GLfloat max_fix = max(m_width, m_height);
	return vec3((factor / 2) * (cannonial.x + 1) + (max_fix - m_height)/2, (factor / 2) * (cannonial.y + 1) + (max_fix - m_width)/2, (cannonial.z + 1 ) / 2 );
}

GLfloat Renderer::getDepth(int x, int y, vec4 v1, vec4 v2, vec4 v3 )
{
	vec2 p1 (x - v1.x, y - v1.y);
	vec2 p2 (x - v2.x, y - v2.y);
	vec2 p3 (x - v3.x, y - v3.y);
	GLfloat area1 = abs(cross(p2, p3));
	GLfloat area2 = abs(cross(p1, p3));
	GLfloat area3 = abs(cross(p2, p1));
	return (area1 * v1.z + area2 * v2.z + area3 * v3.z) / (area1 + area2 + area3);
}

