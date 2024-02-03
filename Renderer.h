#pragma once
#include <vector>
#include "CG_skel_w_imGUI.h"
#include "vec.h"
#include "mat.h"
#include "GLFW/glfw3.h"

using namespace std;
class Renderer
{
	float* m_outBuffer; // 3*width*height
	float* m_zbuffer; // width*height
	int m_width, m_height;

	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();

	//////////////////////////////
	// openGL stuff. Don't touch.

	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals = NULL);
	void SetCameraTransform(const mat4& cTransform);
	void SetProjection(const mat4& projection);
	void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void Reshape(int width, int height);
	void DrawPixel(int x, int y, float r = 1, float g = 1, float b = 1);
	void DrawLine(int x1, int x2, int y1, int y2, float r = 1, float g = 1, float b = 1);
};
