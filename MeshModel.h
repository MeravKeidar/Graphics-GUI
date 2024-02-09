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
	void boundingBox(vector<vec3>* vertices);
	//void Normals(vector<FaceIdcs> faces, vector<vec3> vertices, vector<vec3> verticesNorm);
public:
	MeshModel();
	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	
};

class PrimMeshModel : public MeshModel
{
public:
	PrimMeshModel(string type);

};
