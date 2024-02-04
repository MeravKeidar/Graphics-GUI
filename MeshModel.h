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
	MeshModel() {}
	vec3* vertex_positions;
	vec3* vertex_normals;
	vec3* vertex_textures;
	mat4 _world_transform;
	mat3 _normal_transform;

	/// for calculating the bounding box /// 
	vec3 min_cordinates;
	vec3 max_cordinates;

public:
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
