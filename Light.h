
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

};
struct MATERIAL {
	Color color = { 1.0,0,0 };
	GLfloat transparency = 1.0;
	GLfloat diffuse_fraction = 1.0;
	GLfloat specular_fraction = 1.0;
	GLfloat ambient_fraction = 1.0;
	GLfloat shininess_coefficient = 1.0; 
};
class Light
{
public:
	Color color = { 1.0, 1.0, 1.0 }; //white by default 
	GLfloat intensity;
	LIGHT_TYPE light_type;
	vec4 direction;
	vec3 modified_direction;
	vec4 location;
	vec3 modified_location;
	Light() : location(0, 1, 0, 1), direction(0, -1, 0, 0), intensity(0.5) { light_type = POINT_LIGHT; };


};