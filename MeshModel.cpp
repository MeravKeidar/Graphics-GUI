
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void drawPixel(int x, int y, GLubyte color[4]) {
	if (color == nullptr) {
		GLubyte defaultColor[] = {0,0,0,0};  // white by default
		color = defaultColor;
	}
	glRasterPos2i(x, y);
	glDrawPixels(1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
}


void drawLine(int x1, int x2, int y1, int y2, GLubyte color[4]) {
	if (color == nullptr) {
		GLubyte defaultColor[] = {0,0,0,0};  // white by default
		color = defaultColor;
	}

	int x = x1;
	int y =y1;
	int dx = x2 -x1;
	int dy = y2 - y1;
	int d = 2*dy - dx;
	int de = 2*dy;
	int dne = 2*dy - 2*dx;

	drawPixel(x, y);

	while (x < x2) {
		if (d <= 0) {
			d += de;
		}
		else {
			d += dne;
			y++;
		}
		x++;
		drawPixel(x, y);
	}


}

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

	//TODO: check if 4'th cell needs to be 1 (Homo purposes)
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
	vertices.push_back(vec3(0, 0, 0));
	verticesNorm.push_back(vec3(0, 0, 0));
	verticesText.push_back(vec3(0, 0, 0));
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

	vertex_positions = new vec3[faces.size() * 3]; //TODO: check correctness

	// iterate through all stored faces and create triangles
	int k = 0;
	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
		{
			//TODO: check sanity for reading faces with no normals or\and textures 
			vertex_normals[k] = (verticesNorm.at((*it).vn[i]));
			vertex_textures[k] = (verticesText.at((*it).vt[i]));
			vertex_positions[k++] = (vertices.at((*it).v[i]));

		}
	}
}

PrimMeshModel::PrimMeshModel(string type)
{
	loadFile(type + ".obj");
}

bool isPrimType(string type)
{
	//TODO: add primative type for || 
	return type == "pyramid";
}


void MeshModel::draw()
{

}