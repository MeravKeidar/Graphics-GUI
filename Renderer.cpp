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

void Renderer::DrawTriangles(const vector<vec2>* vertices, float r, float g , float b)
{
	int size = vertices->size();
	for (size_t i = 0; i < size-3; i+= 3)
	{
		vec2 v1 = vertices->at(i);
		vec2 v2 = vertices->at(i+1);
		vec2 v3 = vertices->at(i+2);
		DrawLine(v1.x, v2.x, v1.y, v2.y,r,g,b);
		DrawLine(v1.x, v3.x, v1.y, v3.y,r,g,b);
		DrawLine(v2.x, v3.x, v2.y, v3.y,r,g,b);
	}
	
}

void Renderer::DrawBox(const vector<vec2>* vertices, float r, float g, float b)
{
	//bottom
	DrawLine(vertices->at(0).x, vertices->at(1).x, vertices->at(0).y, vertices->at(1).y, r, g, b);
	DrawLine(vertices->at(1).x, vertices->at(2).x, vertices->at(1).y, vertices->at(2).y, r, g, b);
	DrawLine(vertices->at(2).x, vertices->at(3).x, vertices->at(2).y, vertices->at(3).y, r, g, b);
	DrawLine(vertices->at(2).x, vertices->at(0).x, vertices->at(2).y, vertices->at(0).y, r, g, b);
	//top
	DrawLine(vertices->at(4).x, vertices->at(5).x, vertices->at(4).y, vertices->at(5).y, r, g, b);
	DrawLine(vertices->at(5).x, vertices->at(6).x, vertices->at(5).y, vertices->at(6).y, r, g, b);
	DrawLine(vertices->at(6).x, vertices->at(7).x, vertices->at(6).y, vertices->at(7).y, r, g, b);
	DrawLine(vertices->at(7).x, vertices->at(4).x, vertices->at(7).y, vertices->at(4).y, r, g, b);
	//front
	DrawLine(vertices->at(0).x, vertices->at(4).x, vertices->at(0).y, vertices->at(4).y, r, g, b);
	DrawLine(vertices->at(1).x, vertices->at(5).x, vertices->at(1).y, vertices->at(5).y, r, g, b);
	//back
	DrawLine(vertices->at(3).x, vertices->at(7).x, vertices->at(3).y, vertices->at(7).y, r, g, b);
	DrawLine(vertices->at(2).x, vertices->at(6).x, vertices->at(2).y, vertices->at(6).y, r, g, b);
}
//
//void Renderer::SetCameraTransform(const mat4& cTransform)
//{
//
//}
//
//void Renderer::SetProjection(const mat4& projection)
//{
//}




void Renderer::CreateBuffers(int width, int height)
{
	if (m_outBuffer != NULL)
		delete m_outBuffer;
	m_width = width;
	m_height = height;
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_width * m_height];
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


void Renderer::DrawPixel(int x, int y, float r, float g, float b) {
	if ((x < m_width) && (y < m_height) && (0 < x) && ( 0 < y)) {
		m_outBuffer[INDEX(m_width, x, y, 0)] = r;
		m_outBuffer[INDEX(m_width, x, y, 1)] = g;
		m_outBuffer[INDEX(m_width, x, y, 2)] = b;
	}
	

}
/**
	void  Renderer::DrawLine(int x1, int x2, int y1, int y2, float r, float g, float b) {

	int x = x1;
	int y = y1;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = 2 * dy - dx;
	int de = 2 * dy;
	int dne = 2 * dy - 2 * dx;

	DrawPixel(x, y,r,g,b);

	while (x < x2) {
		if (d <= 0) {
			d += de;
		}
		else {
			d += dne;
			y++;
		}
		x++;
		DrawPixel(x, y,r,g,b);
	}
}
*/


void Renderer::DrawLine(int x1, int x2, int y1, int y2, float r, float g, float b) {

	int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
	int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
	int err = dx + dy, e2;

	while (true) {
		DrawPixel(x1, y1, r, g, b);
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
	// will be used on late stages, when we add depth 
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

vec2 Renderer::viewPortVec(vec2 cannonial)
{
	GLfloat factor = min(m_width, m_height);
	//to fix center
	GLfloat max_fix = max(m_width, m_height);
	return vec2((factor / 2) * (cannonial.x + 1) + (max_fix - m_height)/2, (factor / 2) * (cannonial.y + 1) + (max_fix - m_width)/2);
}


void Renderer::UpdateToScreenMat(int width, int height)
{
	to_screen[0][0] = width / 2.0;
	to_screen[1][1] = height / 2.0;
	//[1][1] just felt right to make it so |A| = 1
	to_screen[1][1] = 4.0 / width*height;
	to_screen[0][2] = width / 2.0;
	to_screen[1][2] = height / 2.0;

}

//assume vertices at vec4
void Renderer::multVertex(const vector<GLfloat>* vertices, mat4 mat, vector<GLfloat>* modified)
{
	//std::cout << "mult vertex func" << std::endl;
	int size = vertices->size();
	//std::cout << "size : " << size << std::endl;
	for (size_t i = 0; i < size; i+= 4)
	{

		vec4 vec(vertices->at(i) , vertices->at(i+1), vertices->at(i+2), vertices->at(i+3));
		
		vec4 res_vec = mat * vec;
		//std::cout << "x: " << vec[i] << " x': " << res_vec[i]<< std::endl;
		//std::cout << "y: " << vec[i+1] << " y': " << res_vec[i+1] << std::endl;
		(*(modified)).at(i) = res_vec[0];
		(*(modified)).at(i+1) = res_vec[1];
		(*(modified)).at(i+2) = res_vec[2];
		(*(modified)).at(i+3) = res_vec[3];
	}

}

void Renderer::pipeLine(const vector<GLfloat>* vertices, vector<GLfloat>* modified, mat4 _world_transform, mat4 camera_mat)
{
	//Assume for now modelT = I
	//either the vertices given are already transformed
	//set for normal projection rather than ortho
	mat4 total_mat = viewPortMat*(to_cannonical_projection* (camera_mat * _world_transform));
	multVertex(vertices, total_mat, modified);
}

