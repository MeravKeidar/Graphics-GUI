#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;



class MeshModel : public Model
{
protected:
	
public:
	void boundingBox(GLfloat min_x, GLfloat min_y, GLfloat min_z, GLfloat max_x, GLfloat max_y, GLfloat max_z);
	MeshModel();
	MeshModel(string fileName, GLuint program);
	~MeshModel(void);
	void loadFile(string fileName,bool default_projection_tex = true);
	void genBuffers();
	void setBoundingBoxAttributes();
	void draw();
	void setVertexAttributes();
	void setNormalsVertexAttributes();
	void calculateTextureCoordinates(int mod = 0);
	void calculateTangent();
	
};

class PrimMeshModel : public MeshModel
{
	void Cube();
	void Tetrahedron();
public:
	PrimMeshModel(string type);

};

