
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
	GLfloat r = 0.0f;
	GLfloat g = 0.0f;
	GLfloat b = 0.0f;

	Color operator*(const GLfloat scale) const {
		Color result{ r * scale, g * scale, b * scale };
		return result;
	}

	void operator*=(GLfloat scale) {
		Color result{ r * scale, g * scale, b * scale };
		*this = result;
	}

	Color operator*(const Color other) const {
		Color result{ std::min(r * other.r,1.0f),  std::min(g * other.b,1.0f),  std::min(b * other.b,1.0f) };
		return result;
	}

	Color operator+(const Color other) const {
		Color result{ std::min(r + other.r,1.0f),std::min(g + other.g,1.0f),std::min(b + other.b,1.0f) };
		return result;
	}

	Color operator-(const Color other) const {
		Color result{ std::max(r - other.r,0.0f),std::max(g - other.g,0.0f),std::max(b - other.b,0.0f) };
		return result;
	}

	Color(GLfloat new_r, GLfloat new_g, GLfloat new_b) :r(std::min(new_r, 1.0f)), g(std::min(new_g, 1.0f)), b(std::min(new_b, 1.0f)) {};
	Color() {};

};

struct MATERIAL {
	Color emissive_color = {0,0,0};
	Color diffuse_color = {0.2,0.2,0.2};
	Color specular_color = {0.2,0.2,0.2};
	GLfloat shininess_coefficient = 100; 
};

MATERIAL interpulateMaterial(MATERIAL v1_material, MATERIAL v2_material, MATERIAL v3_material, GLfloat d1 = 1, GLfloat d2 = 1, GLfloat d3 = 1);

Color interpulateColor(Color v1_color, Color v2_color, Color v3_color, GLfloat d1 = 1, GLfloat d2 = 1, GLfloat d3 = 1);

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 texture;
	vec4 emissive_color;
	vec4 diffuse_color;
	vec4 specular_color;
	GLfloat shininess_coefficient; 


	//vec4 raw_position; //original position in world frame
	//vec4 view_position;  // position in view frame (after transformation amd camera matrix multiplication)
	//vec4 projected; 
	//vec3 canonical; // after devision by w
	//vec3 screen; // in screen coordinates
	//vec3 texture = vec3(0);
	//int texture_x = 0;
	//int texture_y = 0;
	Vertex(vec3 new_position) :position(new_position) ,emissive_color(0.0f, 0.0f, 0.0f, 0.0f),
		diffuse_color(0.2f, 0.2f, 0.2f, 1.0f),
		specular_color(0.2f, 0.2f, 0.2f, 1.0f),
		shininess_coefficient(100.0f), normal(0, 0, 0) {
	};
	Vertex(vec3 new_position, vec3 new_normal) :position(new_position), normal(new_normal){};
	Vertex() {};
};

//struct Normal
//{
//	vec4 original_direction; //original position in world frame
//	vec4 view_direction;  // position in view frame (after transformation amd camera matrix multiplication)
//	//vec4 projected; // No meaning to projection a normal 
//	//vec3 screen; // after devision by w in screen coordinates
//	
//	Normal() {
//		original_direction = (0, 0, 0, 0);
//	};
//	Normal(vec3 new_vec) {
//		vec3 normal = normalize(new_vec);
//		original_direction.x = normal.x;
//		original_direction.y = normal.y;
//		original_direction.z = normal.z;
//		original_direction.w = 0;
//	};
//};
//
//struct Face
//{
//	Vertex* v1 = NULL;
//	Vertex* v2 = NULL;
//	Vertex* v3 = NULL;
//	Vertex face_center;
//
//	Normal* v1_normal = NULL;
//	Normal* v2_normal = NULL;
//	Normal* v3_normal = NULL;
//	Normal face_normal;
//
//
//	Face() {};
//};
//


class Light
{
public:
	Color color = { 1.0, 1.0, 1.0 }; //white by default 
	GLfloat intensity = 1;
	LIGHT_TYPE light_type;
	vec4 direction;
	vec4 view_direction;
	//location is the loaded location after transformations
	vec4 position;
	//location after projection + w division
	vec4 view_position;
	Light() : position(0, 1, 0, 1), direction(0, -1, 0, 0), intensity(0.5) {light_type = POINT_LIGHT;};

};
