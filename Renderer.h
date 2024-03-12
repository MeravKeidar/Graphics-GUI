#pragma once
#include <vector>
#include "CG_skel_w_imGUI.h"
#include "vec.h"
#include "mat.h"
#include "GLFW/glfw3.h"
#include "Camera.h"
#include "Light.h"

using namespace std;

static const GLfloat epsilon = 0.001;

enum SHADING {
	FLAT,
	GOURAUD, 
	PHONG,
	MESH
};


enum FOG {
	LINEAR,
	EXP,
}; 

class Renderer
{
	float* _outBuffer; // 3*width*height
	
	float* m_outBuffer; //antialiasing_resolution * 3*width*height
	float* m_zbuffer; // antialiasing_resolution *width*height
	int m_width, m_height;
	int _width, _height;

	

	void CreateLocalBuffer();
	
	//////////////////////////////
	// openGL stuff. Don't touch.

	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	
	//////////////////////////////
public:
	float  bloom_threshold = 1.5;
	
	bool fog = true; 
	int antialiasing_resolution = 1;
	FOG fog_type = EXP; 
	Color fog_color = Color(0.2f, 0.2f, 0.2f);
	GLfloat fog_start = 0.2;
	GLfloat fog_end = 0.8; 
	GLfloat fog_density = 0.5;
	SHADING shadingType = FLAT;
	mat4 to_cannonical_projection;
	mat4 viewPortMat;
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	//void DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals = NULL);
	void CreateBuffers(int width, int height);
	//void DrawTriangles(const vector<vec4>* vertices, MATERIAL material, const vector<vec3>* vertex_normals, const vector<vec3>* face_normals, vector<Light*> lights, vec4 camera_location, GLfloat ambient_scale);
	void DrawTriangles(const vector<Face>* faces , vector<Light*> lights, GLfloat ambient_scale, bool color_by_pos, bool color_by_normal);
	void DrawBox(const vector<Vertex>* vertices, Color color);
	//void SetCameraTransform(const mat4& cTransform);
	//void SetProjection(const mat4& projection);
	//void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void Reshape(int width, int height);
	void DrawPixel(int x, int y, Color color);
	void DrawLine(int x1, int x2, int y1, int y2, Color color);
	void UpdateViewPort(GLfloat width, GLfloat height);
	vec3 viewPortVec(vec3 cannonial);
	void AntiAlias();
	void UpdateToScreenMat(int width, int height);
	void multVertex(const vector<GLfloat>* vertices, mat4 mat , vector<GLfloat>* modified);
	void pipeLine(const vector<GLfloat>* vertices, vector<GLfloat>* modified, mat4 _world_transform, mat4 camera_mat);
	GLfloat getDepth(int x, int y, vec4 v1, vec4 v2, vec4 v3);
	void fillFlatTriangle(Face face, Color color);
	void fillGouraudTriangle(Face face, Color c1, Color c2, Color c3);
	void fillPhongTriangle(Face face, vector<Light*> lights, GLfloat ambient_scale, bool color_by_pos, bool color_by_normal);
	void drawFlatScanline(int x1, int x2, int y, vec3 v1, vec3 v2, vec3 v3, Color color);
	void drawGouraudScanline(int x1, int x2, int y, Face face, Color c1, Color c2, Color c3);
	void drawPhongScanline(int x1, int x2, int y, Face face , vector<Light*> lights , GLfloat ambient_scale, bool color_by_pos, bool color_by_normal);
	bool liangBarsky(vec3 v1, vec3 v2);
	Color calcColor(MATERIAL material, vec3 normal, vec3 p, vector<Light*> lights, GLfloat ambient_scale);
	Color calcFogColor(GLfloat z, Color color);
	void Blur();
	void Bloom();
};


