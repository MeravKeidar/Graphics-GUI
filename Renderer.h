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

	
	void CreateLocalBuffer();

	
	//unnecessary way to force 2 formulas into a matrix
	mat3 to_screen;
	//////////////////////////////
	// openGL stuff. Don't touch.

	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////
public:
	//TODO: sneak back to private
	//watchout for buffer screen sizes
	mat4 to_cannonical_projection;
	mat4 viewPortMat;
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	//void DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals = NULL);
	void CreateBuffers(int width, int height);
	void DrawTriangles(const vector<vec4>* vertices, float r = 1, float g = 1, float b = 1);
	void DrawBox(const vector<vec4>* vertices, float r = 1, float g = 1, float b = 1);
	//void SetCameraTransform(const mat4& cTransform);
	//void SetProjection(const mat4& projection);
	//void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void Reshape(int width, int height);
	void DrawPixel(int x, int y, float r = 1, float g = 1, float b = 1);
	void DrawLine(int x1, int x2, int y1, int y2, float r = 1, float g = 1, float b = 1);
	void UpdateViewPort(GLfloat width, GLfloat height);
	vec4 viewPortVec(vec4 cannonial);
	void UpdateToScreenMat(int width, int height);
	void multVertex(const vector<GLfloat>* vertices, mat4 mat , vector<GLfloat>* modified);
	void pipeLine(const vector<GLfloat>* vertices, vector<GLfloat>* modified, mat4 _world_transform, mat4 camera_mat);
};
