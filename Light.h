
#pragma once
#include <vector>
#include <string>
#include "mat.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


enum LIGHT_TYPE {
	PARALLEL_LIGHT,
	POINT_LIGHT
};
struct Color {
	GLfloat r;
	GLfloat g;
	GLfloat b;

	Color operator*(const GLfloat scale) const {
		Color result{ r * scale, g * scale, b * scale };
		return result;
	}

	void operator*=(GLfloat scale) {
		Color result{ r * scale, g * scale, b * scale };
		*this = result;
	}

	Color operator+(const Color other) const {
		Color result{ std::min(r + other.r,1.0f),std::min(g + other.g,1.0f),std::min(b + other.b,1.0f) };
		return result;
	}

	Color(GLfloat new_r, GLfloat new_g, GLfloat new_b) :r(std::min(new_r, 1.0f)), g(std::min(new_g, 1.0f)), b(std::min(new_b, 1.0f)) {};
	Color() {};

};

struct MATERIAL {
	Color color = { 1.0,0,0 };
	GLfloat transparency = 1.0;
	GLfloat diffuse_fraction = 1.0;
	GLfloat specular_fraction = 1.0;
	GLfloat ambient_fraction = 1.0;
	GLfloat shininess_coefficient = 100; 
};

struct Vertex
{
	vec4 raw_position; //original position in world frame
	vec4 view_position;  // position in view frame (after transformation amd camera matrix multiplication)
	vec4 projected; 
	vec3 screen; // after devision by w in screen coordinates
	vec3 texture = vec3(0);
	int texture_x = 0;
	int texture_y = 0;
	MATERIAL material;

	Vertex(vec3 new_vec) { raw_position.x = new_vec.x;
	raw_position.y = new_vec.y;
	raw_position.z = new_vec.z;
	raw_position.w = 1;
	};

	Vertex() {};
};

struct Normal
{
	vec4 original_direction; //original position in world frame
	vec4 view_direction;  // position in view frame (after transformation amd camera matrix multiplication)
	vec4 projected;
	vec3 screen; // after devision by w in screen coordinates


	vec3 texture = vec3(0);
	int texture_x = 0;
	int texture_y = 0;
	MATERIAL material;

	Normal(vec3 new_vec) {
		original_direction.x = new_vec.x;
		original_direction.y = new_vec.y;
		original_direction.z = new_vec.z;
		original_direction.w = 0;
	};
};

struct Face
{
	Vertex* v1 = NULL;
	Vertex* v2 = NULL;
	Vertex* v3 = NULL;
	//vec4 position;
	//vec4 view;  // position in view frame (after transformation amd camera matrix multiplication)
	//vec4 projected;
	Normal* face_normal = NULL;
	Normal* v1_normal = NULL;
	Normal* v2_normal = NULL;
	Normal* v3_normal = NULL;
	MATERIAL material;
};

//
//struct PolygonTri
//{
//	int vertices[3];
//	vec3 origin;
//	vec3 normal;
//	vec3 texture;
//	vec4 modified_origin;
//	vec4 modified_normal;
//	PolygonTri()
//	{
//
//	}
//
//	PolygonTri(std::istream& aStream)
//	{
//		for (int i = 0; i < 4; i++)
//			vertices[i] = normal[i] = texture[i] = 0;
//
//		char c;
//		for (int i = 0; i < 3; i++)
//		{
//			aStream >> std::ws >> vertices[i] >> std::ws;
//			if (aStream.peek() != '/')
//				continue;
//			aStream >> c >> std::ws;
//			if (aStream.peek() == '/')
//			{
//				aStream >> c >> std::ws >> normal[i];
//				continue;
//			}
//			else
//				aStream >> texture[i];
//			if (aStream.peek() != '/')
//				continue;
//			aStream >> c >> normal[i];
//		}
//	}
//
//};

class Light
{
public:
	Color color = { 1.0, 1.0, 1.0 }; //white by default 
	GLfloat intensity;
	LIGHT_TYPE light_type;
	vec4 direction;
	vec4 view_direction;
	//location is the loaded location after transformations
	vec4 position;
	//location after projection + w division
	vec4 view_position;
	Light() : position(0, 1, 0, 1), direction(0, -1, 0, 0), intensity(0.5) {light_type = POINT_LIGHT;};


};