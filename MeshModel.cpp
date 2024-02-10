
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
	_normal_transform = mat4(1.0);
	loadFile(fileName);
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
	vector<vec3> vertices;
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

	boundingBox(&vertices);

}


//send the renderer the geometry and transformations of the model, and any other information the renderer might require to draw the model.//
void MeshModel::draw()
{
}

//
//MeshModel::MeshModel()
//{
//	//_world_transform = mat4(1.0);
//	_model_transform = mat4(1.0);
//	_normal_transform = mat4(1.0);
//	vertex_positions.push_back(vec3(0, 0, 0));
//	vertex_positions.push_back(vec3(1, 0, 0));
//	vertex_positions.push_back(vec3(0, 1, 0));
//	
//}

void MeshModel::boundingBox(vector<vec3>* vertices) 
{
	if (vertices->empty()){return;}
	vec3 min = ((*vertices)[0].x, (*vertices)[0].y, (*vertices)[0].y);
	vec3 max = ((*vertices)[0].x, (*vertices)[0].y, (*vertices)[0].y);
	for (auto& vertex : *vertices)
	{
		min.x = (vertex.x < min.x) ? vertex.x : min.x;
		max.x = (vertex.x > max.x) ? vertex.x : max.x;

		min.y = (vertex.y < min.y) ? vertex.y : min.y;
		max.y = (vertex.y > max.y) ? vertex.y : max.y;

		min.z = (vertex.z < min.z) ? vertex.z : min.z;
		max.z = (vertex.z > max.z) ? vertex.z : max.z;
	}

	bounding_box.push_back(vec3(min.x, min.y, min.z));
	bounding_box.push_back(vec3(max.x, min.y, min.z));
	bounding_box.push_back(vec3(max.x, min.y, max.z));
	bounding_box.push_back(vec3(min.x, min.y, max.z));
	bounding_box.push_back(vec3(min.x, max.y, min.z));
	bounding_box.push_back(vec3(max.x, max.y, min.z));
	bounding_box.push_back(vec3(max.x, max.y, max.z));
	bounding_box.push_back(vec3(min.x, max.y, max.z));

	_model_transform[0][3] = (max.x - min.x) / 2;
	_model_transform[1][3] = (max.y - min.y) / 2;
	_model_transform[2][3] = (max.z - min.z) / 2;
}




MeshModel::MeshModel()
{
	_world_transform = mat4(1.0);
	_model_transform = mat4(1.0);
	_normal_transform = mat4(1.0);

	vector<vec3> vertices;
	vertices.push_back(vec3(1, 0, 0));
	vertices.push_back(vec3(0, 1, 0));
	vertices.push_back(vec3(0, 0, 1));
	vertices.push_back(vec3(0, 0, 0)); 

	vertex_positions.push_back(vertices.at(1));
	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(2));

	vertex_positions.push_back(vertices.at(3));
	vertex_positions.push_back(vertices.at(1));
	vertex_positions.push_back(vertices.at(0));

	vertex_positions.push_back(vertices.at(2));
	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(1));

	vertex_positions.push_back(vertices.at(0));
	vertex_positions.push_back(vertices.at(2));
	vertex_positions.push_back(vertices.at(3));

	boundingBox(&vertices);


}
