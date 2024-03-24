#pragma once
#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;



class MeshModel : public Model
{
protected:
	//void Normals(vector<FaceIdcs> faces, vector<vec3> vertices, vector<vec3> verticesNorm);
public:
	void boundingBox(GLfloat min_x, GLfloat min_y, GLfloat min_z, GLfloat max_x, GLfloat max_y, GLfloat max_z);
	MeshModel();
	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	
};

class PrimMeshModel : public MeshModel
{
	void Cube();
	void Tetrahedron();
public:
	PrimMeshModel(string type);

};

