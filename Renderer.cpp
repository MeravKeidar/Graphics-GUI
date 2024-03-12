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

void Renderer::DrawTriangles(const vector<Face>* faces, vector<Light*> lights, GLfloat ambient_scale, bool color_by_pos, bool color_by_normal)
{
	vec3 min_values;
	vec3 max_values;
	GLfloat min_x, min_y, min_z, max_x, max_y, max_z;
	int j = 0;
	int size = faces->size();
	for (size_t i = 0; i < size; i++)
	{
		Face cur_face = faces->at(i);
		Color flat_color;
		Color c1, c2, c3;
		vec4 face_flat_normal;
		MATERIAL face_material = cur_face.v1->material;
		switch (shadingType)
		{
		case FLAT:
				if (color_by_pos)
				{
					Color face_color_by_pos(abs(cur_face.face_center.view_position.x), abs(cur_face.face_center.view_position.y), abs(cur_face.face_center.view_position.z));
					face_material.ambient_color = face_color_by_pos;
				}
				if (color_by_normal)
				{
					Color face_color_by_normal(abs(cur_face.face_normal.view_direction.x), abs(cur_face.face_normal.view_direction.y), abs(cur_face.face_normal.view_direction.z));
					face_material.ambient_color = face_color_by_normal;
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
					
		case PHONG:
				//TODO: check truncation correct
				fillPhongTriangle(cur_face, lights, ambient_scale,  color_by_pos,  color_by_normal);
				break;
		case MESH:
			DrawLine(cur_face.v1->screen.x, cur_face.v2->screen.x, cur_face.v1->screen.y, cur_face.v2->screen.y,Color(1,1,1));
			DrawLine(cur_face.v2->screen.x, cur_face.v3->screen.x, cur_face.v2->screen.y, cur_face.v3->screen.y, Color(1, 1, 1));
			DrawLine(cur_face.v1->screen.x, cur_face.v3->screen.x, cur_face.v1->screen.y, cur_face.v3->screen.y, Color(1, 1, 1));
		}

	
	}
	
}

void Renderer::fillPhongTriangle(Face face, vector<Light*> lights, GLfloat ambient_scale, bool color_by_pos, bool color_by_normal)
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
	if (((v1.y - v2.y) > epsilon) || ((v1.y - v2.y) < -epsilon))
		slope1 = (v1.x - v2.x) / (v1.y - v2.y);
	if (((v1.y - v3.y) > epsilon) || ((v1.y - v3.y) < -epsilon))
		slope2 = (v1.x - v3.x) / (v1.y - v3.y);
	GLfloat x1 = v1.x;
	GLfloat x2 = v1.x;
	int y = min(v1.y, GLfloat(m_height));
	for (; y >= v2.y; y--)
	{
		drawPhongScanline(x1, x2, y, face, lights, ambient_scale, color_by_pos, color_by_normal);
		x1 -= slope1;
		x2 -= slope2;
	}
	x1 = v2.x;
	if (((v2.y - v3.y) > -epsilon) && ((v2.y - v3.y) < epsilon))
		slope1 = 0;
	else
		slope1 = (v2.x - v3.x) / (v2.y - v3.y);
	for (; y >= v3.y; y--)
	{
		drawPhongScanline(x1, x2, y, face, lights, ambient_scale, color_by_pos, color_by_normal);
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
	if (((v1.y - v2.y) > epsilon) || ((v1.y - v2.y) < -epsilon))
		slope1 = (v1.x - v2.x) / (v1.y - v2.y);
	if (((v1.y - v3.y) > epsilon) || ((v1.y - v3.y) < -epsilon))
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
	if (((v2.y - v3.y) > -epsilon) && ((v2.y - v3.y) < epsilon))
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
	if (((v1.y - v2.y) > epsilon) || ((v1.y - v2.y) < -epsilon))
		slope1 = (v1.x - v2.x) / (v1.y - v2.y);
	if (((v1.y - v3.y) > epsilon) || ((v1.y - v3.y) < -epsilon))
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
	if (((v2.y - v3.y) > -epsilon) && ((v2.y - v3.y) < epsilon))
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

void Renderer::drawPhongScanline(int x1, int x2, int y, Face face, vector<Light*> lights, GLfloat ambient_scale, bool color_by_pos, bool color_by_normal)
{
	vec3 v1 = face.v1->screen;
	vec3 v2 = face.v2->screen;
	vec3 v3 = face.v3->screen;
	if (x1 > x2) std::swap(x1, x2);
	for (int x = x1; x <= x2; x++)
	{
		if ((x < m_width) && (y < m_height) && (0 < x) && (0 < y))
		{
			vec2 p1(x - v1.x, y - v1.y);
			vec2 p2(x - v2.x, y - v2.y);
			vec2 p3(x - v3.x, y - v3.y);
			GLfloat a1 = abs(cross(p2, p3));
			GLfloat a2 = abs(cross(p1, p3));
			GLfloat a3 = abs(cross(p2, p1));
			GLfloat total_area = a1 + a2 + a3;
			if ((total_area < epsilon) && (total_area > -epsilon))
				continue;
			GLfloat z = (a1 * v1.z + a2 * v2.z + a3 * v3.z) / (total_area);

			if (m_zbuffer[x + m_width * y] > z)
			{
				m_zbuffer[x + m_width * y] = z;
				/*GLfloat d1 = length(vec2(x - v1.x, y - v1.y));
				GLfloat d2 = length(vec2(x - v2.x, y - v2.y));
				GLfloat d3 = length(vec2(x - v3.x, y - v3.y));
				GLfloat total_d = d1 + d2 + d3; 
				if ((total_d < epsilon) && (total_d > -epsilon))
					continue;*/
				//vec4 normal = (face.v1_normal->view_direction * d1 + face.v2_normal->view_direction * d2 + face.v3_normal->view_direction *d3) / (total_d) ;
				//vec4 p = (face.v1->view_position * d1 + face.v2->view_position * d2 + face.v3->view_position * d3) / (d1+d2+d3);
				vec4 normal = (face.v1_normal->view_direction * a1 + face.v2_normal->view_direction * a2 + face.v3_normal->view_direction * a3) / (total_area);
				vec4 p = (face.v1->view_position * a1 + face.v2->view_position * a2 + face.v3->view_position * a3) / (total_area);
				MATERIAL point_material = face.v1->material;
				if (color_by_pos)
				{
					Color face_color_by_pos(abs(p.x), abs(p.y), abs(p.z));
					point_material.ambient_color = face_color_by_pos;
				}
				else if (color_by_normal)
				{
					Color face_color_by_normal(abs(normal.x), abs(normal.y), abs(normal.z));
					point_material.ambient_color = face_color_by_normal;
				}

				Color color = calcColor(point_material, truncateVec4(normal), truncateVec4(p), lights, ambient_scale);
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

Color Renderer::calcFogColor(GLfloat z, Color color)
{

	GLfloat fog_factor = 0;
	z = abs(z); 
	if (fog_type == LINEAR)
	{
		fog_factor = 1 - ((fog_end - z) / (fog_end - fog_start));
	}
	else 
	{
		fog_factor = 1 - exp(-fog_density * z);
	}
	fog_factor = max(0.0f, fog_factor);
	fog_factor = min(1.0f, fog_factor);
	return (color*(1 - fog_factor) + fog_color * fog_factor);
}

Color Renderer::calcColor(MATERIAL material, vec3 normal, vec3 p, vector<Light*> lights, GLfloat ambient_scale)
{
	Color color = material.emissive_color;
	vec3 l; //direction from point to light(per light)
	vec3 v = normalize(-p); //direction from point to camera - point is in view frame;
	//ambient
	Color ambiant = (material.ambient_color * ambient_scale);
	color = color + ambiant;
	
	//itarate per light source
	int size = lights.size();
	for (size_t i = 0; i < size; i++)
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
		GLfloat LN = max(0.0f, dot(l, normal));
		vec3 r = normalize((2 * normal * LN) - l);

		//diffuse
		Color diffuse = current_light->color * (material.diffuse_color * (LN * current_light->intensity));
		// specular
		GLfloat Shininess = pow(max(dot(r, v), 0.0f), material.shininess_coefficient);
		Color specular = current_light->color * (material.specular_color * Shininess * current_light->intensity);
		color = color + diffuse + specular;
	}
	if (fog)
	{
		color = calcFogColor(p.z, color);
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
	GLfloat total_d = d1 + d2 + d3;
	if ((total_d < epsilon) && (total_d > -epsilon))
		return Color(0, 0, 0);
	GLfloat r = (v1_color.r * d1 + v2_color.r * d2 + v3_color.r * d3) / (d1 + d2 + d3);
	GLfloat g = (v1_color.g * d1 + v2_color.g * d2 + v3_color.g * d3) / (d1 + d2 + d3);
	GLfloat b = (v1_color.b * d1 + v2_color.b * d2 + v3_color.b * d3) / (d1 + d2 + d3);
	return Color(r, g, b);
}

GLfloat Renderer::getDepth(int x, int y, vec4 v1, vec4 v2, vec4 v3)
{
	vec2 p1(x - v1.x, y - v1.y);
	vec2 p2(x - v2.x, y - v2.y);
	vec2 p3(x - v3.x, y - v3.y);
	GLfloat a1 = abs(cross(p2, p3));
	GLfloat a2 = abs(cross(p1, p3));
	GLfloat a3 = abs(cross(p2, p1));
	GLfloat total_area = a1 + a2 + a3;
	if ((total_area < epsilon) && (total_area > -epsilon))
		return 0;
	return (a1 * v1.z + a2 * v2.z + a3 * v3.z) / (total_area);
}

void Renderer::DrawBox(const vector<Vertex>* vertices, Color color)
{

	//bottom
	DrawLine(vertices->at(0).screen.x, vertices->at(1).screen.x, vertices->at(0).screen.y, vertices->at(1).screen.y, color);
	DrawLine(vertices->at(1).screen.x, vertices->at(2).screen.x, vertices->at(1).screen.y, vertices->at(2).screen.y, color);
	DrawLine(vertices->at(2).screen.x, vertices->at(3).screen.x, vertices->at(2).screen.y, vertices->at(3).screen.y, color);
	DrawLine(vertices->at(2).screen.x, vertices->at(0).screen.x, vertices->at(2).screen.y, vertices->at(0).screen.y, color);
	//top
	DrawLine(vertices->at(4).screen.x, vertices->at(5).screen.x, vertices->at(4).screen.y, vertices->at(5).screen.y, color);
	DrawLine(vertices->at(5).screen.x, vertices->at(6).screen.x, vertices->at(5).screen.y, vertices->at(6).screen.y, color);
	DrawLine(vertices->at(6).screen.x, vertices->at(7).screen.x, vertices->at(6).screen.y, vertices->at(7).screen.y, color);
	DrawLine(vertices->at(7).screen.x, vertices->at(4).screen.x, vertices->at(7).screen.y, vertices->at(4).screen.y, color);
	//front
	DrawLine(vertices->at(0).screen.x, vertices->at(4).screen.x, vertices->at(0).screen.y, vertices->at(4).screen.y, color);
	DrawLine(vertices->at(1).screen.x, vertices->at(5).screen.x, vertices->at(1).screen.y, vertices->at(5).screen.y, color);
	//back
	DrawLine(vertices->at(3).screen.x, vertices->at(7).screen.x, vertices->at(3).screen.y, vertices->at(7).screen.y, color);
	DrawLine(vertices->at(2).screen.x, vertices->at(6).screen.x, vertices->at(2).screen.y, vertices->at(6).screen.y, color);
}

vec3 Renderer::viewPortVec(vec3 cannonial)
{
	GLfloat factor = min(m_width, m_height);
	//to fix center
	GLfloat max_fix = max(m_width, m_height);
	return vec3((factor / 2) * (cannonial.x + 1) + (max_fix - m_height) / 2, (factor / 2) * (cannonial.y + 1) + (max_fix - m_width) / 2, (cannonial.z + 1) / 2);
}

void Renderer::AntiAlias()
{
	if (antialiasing_resolution == 1)
	{
		for (size_t i = 0; i < 3 * m_width * m_height; i++)
		{
			_outBuffer[i] = m_outBuffer[i];
		}
	}
	else 
	{
		size_t m_y_idx = 0;
		for (int y = 0; y < _height; y++)
		{
			size_t m_x_idx = 0;
			for (int x = 0; x < _width ; x++)
			{
				float sumR = 0.0f;
				float sumG = 0.0f;
				float sumB = 0.0f;
				for (size_t j = 0; j < 3 * antialiasing_resolution; j += 3)
				{
					sumR += m_outBuffer[(m_y_idx * m_width + m_x_idx + j) * 3] + m_outBuffer[((m_y_idx + j) * m_width + m_x_idx) * 3];
					sumG += m_outBuffer[(m_y_idx * m_width + m_x_idx + j) * 3 + 1] + m_outBuffer[((m_y_idx + j) * m_width + m_x_idx) * 3 + 1];
					sumB += m_outBuffer[(m_y_idx * m_width + m_x_idx + j) * 3 + 2] + m_outBuffer[((m_y_idx + j) * m_width + m_x_idx) * 3 + 2];
		

				}
				_outBuffer[INDEX(_width, x, y, 0)] = sumR / (antialiasing_resolution*2);
				_outBuffer[INDEX(_width, x, y, 1)] = sumG / (antialiasing_resolution*2);
				_outBuffer[INDEX(_width, x, y, 2)] = sumB / (antialiasing_resolution*2);
				m_x_idx += antialiasing_resolution;
			}
			m_y_idx += antialiasing_resolution;
		}
	}
}

void Renderer::CreateBuffers(int width, int height)
{
	if (m_outBuffer != NULL)
		delete m_outBuffer;
	if (m_zbuffer != NULL)
		delete m_zbuffer;
	if (_outBuffer != NULL)
		delete _outBuffer;
	m_width = antialiasing_resolution * width;
	m_height = antialiasing_resolution * height;
	_width = width;
	_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width * m_height];
	m_zbuffer = new float[m_width * m_height];
	_outBuffer = new float[3 * _width * _height];
	for (size_t i = 0; i < m_width * m_height; i++)
	{
		m_zbuffer[i] = 10;
	}
	
}

void Renderer::CreateLocalBuffer()
{
	
}

void Renderer::Bloom()
{
	const int offsets[11] = { -5,-4,-3,-2,-1,0,1,2,3,4,5 };
	const float weights[11] = {
		0.011797803481426415,
		0.0320697548192923,
		0.06980398824247301,
		0.1216619992483103,
		0.16979299781175633,
		0.18974691279348324,
		0.16979299781175633,
		0.1216619992483103,
		0.06980398824247301,
		0.0320697548192923,
		0.011797803481426415
	};

	float* horizontalBuffer = new float[3 * _width * _height];
	float* verticalBuffer = new float[3 * _width * _height];

	for (size_t i = 0; i < _width * _height * 3; i++)
	{
		horizontalBuffer[i] = 0;
		verticalBuffer[i] = _outBuffer[i] * weights[0];
	}

	for (int k = 0; k < 5; k++)
	{
		for (int h = 0; h < _height; h++)
		{
			for (int w = 0; w < _width; w++)
			{

				for (int i = 0; i < 11; i++)
		
				{
					if (verticalBuffer[INDEX(_width, w + offsets[i], h, 0)] + verticalBuffer[INDEX(_width, w + offsets[i], h, 1)] + verticalBuffer[INDEX(_width, w + offsets[i], h, 2)] > bloom_threshold)
					{
						if ((w + offsets[i]) < _width && (w + offsets[i]) >= 0)
						{
							horizontalBuffer[INDEX(_width, w, h, 0)] += verticalBuffer[INDEX(_width, w + offsets[i], h, 0)] * weights[i];
							horizontalBuffer[INDEX(_width, w, h, 1)] += verticalBuffer[INDEX(_width, w + offsets[i], h, 1)] * weights[i];
							horizontalBuffer[INDEX(_width, w, h, 2)] += verticalBuffer[INDEX(_width, w + offsets[i], h, 2)] * weights[i];
						}

					}

				}
			}
		}
		for (int w = 0; w < _width; w++) {
			for (int h = 0; h < _height; h++) {
				for (int i = 0; i < 11; i++) {
					if ((h + offsets[i]) < _height && (h + offsets[i]) >= 0)
					{
						if (horizontalBuffer[(_width * (h + offsets[i]) + w) * 3] + horizontalBuffer[(_width * (h + offsets[i]) + w) * 3 + 1] + horizontalBuffer[(_width * (h + offsets[i]) + w) * 3 + 2] > bloom_threshold) 
						{
							verticalBuffer[INDEX(_width, w, h, 0)] += horizontalBuffer[(_width * (h + offsets[i]) + w) * 3] * weights[i];
							verticalBuffer[INDEX(_width, w, h, 1)] += horizontalBuffer[(_width * (h + offsets[i]) + w) * 3 + 1] * weights[i];
							verticalBuffer[INDEX(_width, w, h, 2)] += horizontalBuffer[(_width * (h + offsets[i]) + w) * 3 + 2] * weights[i];
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < _width * _height * 3; i++) {
		_outBuffer[i] += verticalBuffer[i];
	}
	delete[] horizontalBuffer;
	delete[] verticalBuffer;
}


void Renderer::Blur()
{
	const int offsets[11] = {-5,-4,-3,-2,-1,0,1,2,3,4,5};
	const float weights[11] = {
		0.011797803481426415,
		0.0320697548192923,
		0.06980398824247301,
		0.1216619992483103,
		0.16979299781175633,
		0.18974691279348324,
		0.16979299781175633,
		0.1216619992483103,
		0.06980398824247301,
		0.0320697548192923,
		0.011797803481426415
	};

	float* horizontalBuffer = new float[3 * _width * _height];
	float* verticalBuffer = new float[3 * _width * _height];

	for (size_t i = 0; i < _width * _height * 3; i++)
	{
		horizontalBuffer[i] = 0;
		verticalBuffer[i] = _outBuffer[i] * weights[0];
	}
	for (int k = 0; k < 5 ; k++)
	{
		// Horizontal blur
		for (int h = 0; h < _height; h++)
		{
			for (int w = 0; w < _width; w++)
			{

				for (int i = 0; i < 11; i++)
				{
					if ((w + offsets[i]) < _width && (w + offsets[i]) >= 0)
					{
						horizontalBuffer[INDEX(_width, w, h, 0)] += verticalBuffer[INDEX(_width, w + offsets[i], h, 0)] * weights[i];
						horizontalBuffer[INDEX(_width, w, h, 1)] += verticalBuffer[INDEX(_width, w + offsets[i], h, 1)] * weights[i];
						horizontalBuffer[INDEX(_width, w, h, 2)] += verticalBuffer[INDEX(_width, w + offsets[i], h, 2)] * weights[i];
					}
				}
			}
		}

		// Vertical blur
		
		for (int w = 0; w < _width; w++) {
			for (int h = 0; h < _height; h++) {
				for (int i = 0; i < 11; i++) {
					if ((h + offsets[i]) < _height && (h + offsets[i]) >= 0) {
						verticalBuffer[INDEX(_width, w, h, 0)] += horizontalBuffer[(_width * (h + offsets[i]) + w) * 3] * weights[i];
						verticalBuffer[INDEX(_width, w, h, 1)] += horizontalBuffer[(_width * (h + offsets[i]) + w) * 3 + 1] * weights[i];
						verticalBuffer[INDEX(_width, w, h, 2)] += horizontalBuffer[(_width * (h + offsets[i]) + w) * 3 + 2] * weights[i];
					}
				}
			}
		}
	}
	

	for (int i = 0; i < _width * _height * 3; i++) {
		_outBuffer[i] = verticalBuffer[i];
	}

	delete[] horizontalBuffer;
	delete[] verticalBuffer;

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
	m_width = antialiasing_resolution * width;
	m_height = antialiasing_resolution * height;
	_width =  width;
	_height =  height;
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
	for (size_t i = 0; i < _height; i++)
	{
		for (size_t j = 0; j < _width; j++)
		{
			_outBuffer[(i * _width + j) * 3] = 0;
			_outBuffer[(i * _width + j) * 3 + 1] = 0;
			_outBuffer[(i * _width + j) * 3 + 2] = 0;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, _width, _height);
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGB, GL_FLOAT, _outBuffer);
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



