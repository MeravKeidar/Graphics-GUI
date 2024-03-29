#pragma once

#include <vector>
#include <string>
#include "mat.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Camera.h"
#include "Light.h"

class Texture
{
private: 
	std::string m_FilePath;
	unsigned char* m_localBuffer;
public:
	GLuint ProgramID;
	int m_width, m_height, m_BPP;
	Texture() {};
	Texture(const std::string& path, GLuint programID);
	~Texture();

	void bind(unsigned int slot = 0) const;
	void unbind() const;

};