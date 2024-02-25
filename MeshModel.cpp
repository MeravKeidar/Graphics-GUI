
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct FaceIdcs
{
	// vertices positions
	int v[4];
	// vertices normals
	int vn[4];
	// vertices textures
	int vt[4];

	FaceIdcs()
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream& aStream)
	{
		for (int i = 0; i < 4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for (int i = 0; i < 3; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}
};

vec3 vec3fFromStream(std::istream& aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream& aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel(string fileName)
{
	_world_transform = mat4(1.0);
	_model_transform = mat4(1.0);
	_normal_world_transform = mat4(1.0);
	_normal_model_transform = mat4(1.0);
	loadFile(fileName);
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
	
	vector<vec3> verticesText;
	bool calculate_vnorm = false; 
		// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
			vertices.push_back(vec3fFromStream(issLine));
		else if (lineType == "vn")
			vertex_normals.push_back(vec3fFromStream(issLine));
		else if (lineType == "vt")
			verticesText.push_back(vec3fFromStream(issLine));
		else if (lineType == "f")
			faces.push_back(issLine);
		else if (lineType == "#" || lineType == "")
		{
			//TODO: check if need to manually skip line
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}

	//initilize all vertex vectors to be zero if none were given
	if (vertex_normals.empty())
	{
		calculate_vnorm = true; 
		vertex_normals = vector<vec3>(vertices.size(),vec3(0.0f));
	}

	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		//calculate face normals
		vec3 xi, xj, xk, normal;
		GLfloat norm;
		xi = vertices.at((*it).v[0] - 1);
		xj = vertices.at((*it).v[1] - 1);
		xk = vertices.at((*it).v[2] - 1);
		normal = cross((xj-xi),(xj-xk));
		face_normals.push_back(normalize(normal));
		face_normals_origin.push_back((xi + xj + xk) / 3);
		for (int i = 0; i < 3; i++)
		{
			vertex_positions.push_back(vertices.at((*it).v[i] - 1));
			//calculate vertices normals
			if (calculate_vnorm)
			{
				vertex_normals.at((*it).v[i] - 1) += normal; 
			}
		}
	}

	//normalize all normals 
	if (calculate_vnorm)
	{
		for (auto& v_normal : vertex_normals) {
			v_normal = normalize(v_normal);
		}
	}

	boundingBox();

}


//send the renderer the geometry and transformations of the model, and any other information the renderer might require to draw the model.//
void MeshModel::draw()
{
}


MeshModel::MeshModel()
{
	color.r = 0;
	color.g = 0;
	color.b = 0;
	_world_transform = mat4(1.0);
	_model_transform = mat4(1.0);
	_normal_world_transform = mat4(1.0);
	_normal_model_transform = mat4(1.0);
	
}

void MeshModel::boundingBox() 
{
	if (vertices.empty()){return;}
	vec3 min = (vertices[0].x, vertices[0].y, vertices[0].y);
	vec3 max = (vertices[0].x, vertices[0].y, vertices[0].y);
	for (auto& vertex : vertices)
	{
		min.x = (vertex.x < min.x) ? vertex.x : min.x;
		max.x = (vertex.x > max.x) ? vertex.x : max.x;

		min.y = (vertex.y < min.y) ? vertex.y : min.y;
		max.y = (vertex.y > max.y) ? vertex.y : max.y;

		min.z = (vertex.z < min.z) ? vertex.z : min.z;
		max.z = (vertex.z > max.z) ? vertex.z : max.z;

		_center_of_mass += vertex;
	}

	_center_of_mass = _center_of_mass / vertices.size(); 

	bounding_box.push_back(vec3(min.x, min.y, min.z));
	bounding_box.push_back(vec3(max.x, min.y, min.z));
	bounding_box.push_back(vec3(max.x, min.y, max.z));
	bounding_box.push_back(vec3(min.x, min.y, max.z));
	bounding_box.push_back(vec3(min.x, max.y, min.z));
	bounding_box.push_back(vec3(max.x, max.y, min.z));
	bounding_box.push_back(vec3(max.x, max.y, max.z));
	bounding_box.push_back(vec3(min.x, max.y, max.z));
	vec3 model_center((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + min.z) / 2);

	_world_transform = TranslationMat(-model_center) * _world_transform;
	_normal_world_transform = TranslationMat(-model_center) * _normal_world_transform;
}




void PrimMeshModel::Tetrahedron()
{

	vertices.push_back(vec3(1, 0, 0));
	vertices.push_back(vec3(0, 1, 0));
	vertices.push_back(vec3(0, 0, 1));
	vertices.push_back(vec3(0, 0, 0)); 

	// 0->z->y
	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(2));
	vertex_positions.push_back(vertices.at(1));
	// 0->y->x
	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(1));
	vertex_positions.push_back(vertices.at(0));
	// 0->x->z
	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(2));
	// z->x->y
	vertex_positions.push_back(vertices.at(2));
	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(1));
	

	face_normals.push_back(vec3(-1, 0, 0));
	face_normals.push_back(vec3(0, 0, -1));
	face_normals.push_back(vec3(0, -1, 0));
	face_normals.push_back(normalize(vec3(1, 1, 1)));

	face_normals_origin.push_back(vec3(0, 0.33333333, 0.33333333));
	face_normals_origin.push_back(vec3(0.33333333, 0.33333333,0));
	face_normals_origin.push_back(vec3(0.33333333, 0, 0.33333333));
	face_normals_origin.push_back(vec3(0.33333333, 0.33333333, 0.33333333));

	vertex_normals.push_back(normalize(face_normals.at(1)+ face_normals.at(2)+ face_normals.at(3)));
	vertex_normals.push_back(normalize(face_normals.at(0) + face_normals.at(1) + face_normals.at(3)));
	vertex_normals.push_back(normalize(face_normals.at(0) + face_normals.at(2) + face_normals.at(3)));
	vertex_normals.push_back(normalize(face_normals.at(1) + face_normals.at(2) + face_normals.at(0)));

	boundingBox();

}

void PrimMeshModel::Cube()
{

	vertices.push_back(vec3(1.0f, 1.0f, 1.0f)); // Vertex 0
	vertices.push_back(vec3(-1.0f, 1.0f, 1.0f)); // Vertex 1
	vertices.push_back(vec3(-1.0f, -1.0f, 1.0f)); // Vertex 2
	vertices.push_back(vec3(1.0f, -1.0f, 1.0f)); // Vertex 3
	vertices.push_back(vec3(1.0f, -1.0f, -1.0f)); // Vertex 4
	vertices.push_back(vec3(1.0f, 1.0f, -1.0f)); // Vertex 5
	vertices.push_back(vec3(-1.0f, 1.0f, -1.0f)); // Vertex 6
	vertices.push_back(vec3(-1.0f, -1.0f, -1.0f)); // Vertex 7
	vertices.push_back(vec3(0.0f, 0.0f, 0.0f)); // Vertex 8

	vector<vec3> vertex_positions;
	// Front face
	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(1));
	vertex_positions.push_back(vertices.at(2));

	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(2));
	vertex_positions.push_back(vertices.at(3));

	// Right face
	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(4));

	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(4));
	vertex_positions.push_back(vertices.at(5));

	// Back face
	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(5));
	vertex_positions.push_back(vertices.at(6));

	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(6));
	vertex_positions.push_back(vertices.at(1));

	// Left face
	vertex_positions.push_back(vertices.at(1));
	vertex_positions.push_back(vertices.at(6));
	vertex_positions.push_back(vertices.at(7));

	vertex_positions.push_back(vertices.at(1));
	vertex_positions.push_back(vertices.at(7));
	vertex_positions.push_back(vertices.at(2));

	// Bottom face
	vertex_positions.push_back(vertices.at(7));
	vertex_positions.push_back(vertices.at(4));
	vertex_positions.push_back(vertices.at(3));

	vertex_positions.push_back(vertices.at(7));
	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(2));

	// Top face
	vertex_positions.push_back(vertices.at(4));
	vertex_positions.push_back(vertices.at(7));
	vertex_positions.push_back(vertices.at(6));

	vertex_positions.push_back(vertices.at(4));
	vertex_positions.push_back(vertices.at(6));
	vertex_positions.push_back(vertices.at(5));


	

	boundingBox();

}


PrimMeshModel::PrimMeshModel(string type)
{
	if (type == "tetrahedron")
	{
		Tetrahedron();
	}
	else if (type == "cube")
	{
		Cube();
	}
}
