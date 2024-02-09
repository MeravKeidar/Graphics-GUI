
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
	loadFile(fileName);
	//calculateNormals();
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
	vector<vec3> vertices;
	vector<vec3> verticesNorm;
	vector<vec3> verticesText;
	//vertices.push_back(vec3(0, 0, 0));
	verticesNorm.push_back(vec3(0, 0, 0)); // if there is no vertex normal in the file, add 0
	verticesText.push_back(vec3(0, 0, 0)); // if there is no vertex texture in the file, add 0
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
			verticesNorm.push_back(vec3fFromStream(issLine));
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
	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertex_positions should contain:
	//vertex_positions={v1,v2,v3,v1,v3,v4}

	//vertex_positions = new vec4[faces.size() * 4]; //TODO: check correctness

	min_cordinates = max_cordinates = (vertices[0][0], vertices[0][1], vertices[0][2]);
	// iterate through all stored faces and create triangles
	//int k = 0;
	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
		{
			vertex_positions.push_back(vertices.at((*it).v[i] - 1));
			vertex_textures.push_back(verticesText.at((*it).vt[i]));
			vertex_normals.push_back(verticesNorm.at((*it).vn[i]));

			// create the modifed vertices
			modified_vertex.push_back(vec4(vertex_positions.at(i)));
			
			//uptade min/max coordinates
			// x coordinates
			min_cordinates[0] = (vertices[i][0] < min_cordinates[0]) ? vertices[i][0] : min_cordinates[0];
			max_cordinates[0] = (vertices[i][0] > max_cordinates[0]) ? vertices[i][0] : max_cordinates[0];
			// y coordinates
			min_cordinates[1] = (vertices[i][1] < min_cordinates[1]) ? vertices[i][1] : min_cordinates[1];
			max_cordinates[1] = (vertices[i][1] > max_cordinates[1]) ? vertices[i][1] : max_cordinates[1];
			// z coordinates
			min_cordinates[2] = (vertices[i][2] < min_cordinates[2]) ? vertices[i][2] : min_cordinates[2];
			max_cordinates[2] = (vertices[i][2] > max_cordinates[2]) ? vertices[i][2] : max_cordinates[2];
		}

		//calculate face normals
		vec3 xi, xj, xk, normal;
		GLfloat norm;
		xi = vertices.at((*it).v[0] - 1);
		xj = vertices.at((*it).v[1] - 1);
		xk = vertices.at((*it).v[2] - 1);
		normal = cross((xj-xi),(xj-xk));
		norm = length(normal);
		face_normals.push_back(normal/norm);
		modified_face_normals.push_back(vec4(normal/norm));

		//calculate vertices normals - would be a nightmare 
	}

}

PrimMeshModel::PrimMeshModel(string type)
{
	loadFile(type + ".obj");
}

//send the renderer the geometry and transformations of the model, and any other information the renderer might require to draw the model.//
void MeshModel::draw()
{
	
	}


MeshModel::MeshModel()
{
	modified_vertex.push_back(vec4(0, 0, 0, 0));
	modified_vertex.push_back(vec4(1, 0, 0, 0));
	modified_vertex.push_back(vec4(0, 1, 0, 0));
	
}