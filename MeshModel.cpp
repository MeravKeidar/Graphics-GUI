
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
	vector<vec3> verticesText;
	vector<vec3> verticesNormal;
	bool calculate_vnorm = false; 
		// while not end of file
	int current_normal_index = 0;
	int current_texture_index = 0;

	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		Vertex current_vertex;
		PolygonTri current_face;

		// based on the type parse data
		if (lineType == "v")
		{
			current_vertex.homogenous = vec3fFromStream(issLine);
			vertices.push_back(current_vertex);
		}	
		else if (lineType == "vn")
		{
			vertices.at(current_normal_index).normal = vec3fFromStream(issLine);
			current_normal_index++;
		}
		else if (lineType == "vt")
		{
			vec2 temp = vec2fFromStream(issLine);
			vertices.at(current_normal_index).texture_x = temp.x;
			vertices.at(current_normal_index).texture_y = temp.y;
			current_texture_index++;
		}
			
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
	if (current_normal_index == 0)
	{
		calculate_vnorm = true; 
	}

	for (vector<PolygonTri>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		//calculate face normals
		vec3 xi, xj, xk, normal;
		//GLfloat norm;
		
		xi = truncateVec4(vertices.at((*it).vertices[0] - 1).homogenous);
		xj = truncateVec4(vertices.at((*it).vertices[1] - 1).homogenous);
		xk = truncateVec4(vertices.at((*it).vertices[2] - 1).homogenous);

		normal = cross((xj-xi),(xj-xk));
		(*it).normal = normalize(normal);
		(*it).origin = (xi + xj + xk) / 3;
		for (int i = 0; i < 3; i++)
		{
			//vertex_positions.push_back(vertices.at((*it).v[i] - 1));
			//calculate vertices normals
			if (calculate_vnorm)
			{
				vertices.at((*it).vertices[i] - 1).normal += normal;
				//vertex_normals.at((*it).v[i] - 1) += normal; 
			}
		}
	}

	//normalize all normals 
	//TODO: make sure normalizing is correct
	if (calculate_vnorm)
	{
		for (auto& v : vertices) {
			v.normal = normalize(v.normal);
		}
	}

	//TODO: make sure positions are alligned
	/*for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++)
		{
			vertex_normal_positions.push_back(vertex_normals.at((*it).v[i] - 1));
		}
	}*/

	boundingBox();

}


//send the renderer the geometry and transformations of the model, and any other information the renderer might require to draw the model.//
void MeshModel::draw()
{
}


MeshModel::MeshModel()
{
	 
	material.color.r = 0;
	material.color.g = 0;
	material.color.b = 0;
	_world_transform = mat4(1.0);
	_model_transform = mat4(1.0);
	_normal_world_transform = mat4(1.0);
	_normal_model_transform = mat4(1.0);
	
}

void MeshModel::boundingBox() 
{
	if (vertices.empty()){return;}
	
	vec3 min = (vertices[0].homogenous.x, vertices[0].homogenous.y, vertices[0].homogenous.y);
	vec3 max = (vertices[0].homogenous.x, vertices[0].homogenous.y, vertices[0].homogenous.y);
	for (auto& vertex : vertices)
	{
		min.x = (vertex.homogenous.x < min.x) ? vertex.homogenous.x : min.x;
		max.x = (vertex.homogenous.x > max.x) ? vertex.homogenous.x : max.x;

		min.y = (vertex.homogenous.y < min.y) ? vertex.homogenous.y : min.y;
		max.y = (vertex.homogenous.y > max.y) ? vertex.homogenous.y : max.y;

		min.z = (vertex.homogenous.z < min.z) ? vertex.homogenous.z : min.z;
		max.z = (vertex.homogenous.z > max.z) ? vertex.homogenous.z : max.z;

		_center_of_mass += vertex.homogenous;
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
	Vertex temp;
	//x
	temp.homogenous = vec4(1, 0, 0, 1);
	
	vertices.push_back(temp);
	//y
	temp.homogenous = vec4(0, 1, 0, 1);

	vertices.push_back(temp);
	//z
	temp.homogenous = vec4(0, 0, 1, 1);
	vertices.push_back(temp);

	//0
	temp.homogenous = vec4(0, 0, 0, 1);
	vertices.push_back(temp);

	PolygonTri temp_face;
	temp_face.normal = vec3(-1, 0, 0);
	temp_face.origin = vec3(0, 0.33333333, 0.33333333);
	faces.push_back(temp_face);

	temp_face.normal = vec3(0, 0, -1);
	temp_face.origin = vec3(0.33333333, 0.33333333, 0);
	faces.push_back(temp_face);

	temp_face.normal = vec3(0, -1, 0);
	temp_face.origin = vec3(0.33333333, 0, 0.33333333);
	faces.push_back(temp_face);

	temp_face.normal = normalize(vec3(1, 1, 1));
	temp_face.origin = vec3(0.33333333, 0.33333333, 0.33333333);
	faces.push_back(temp_face);




	vertices.at(0).normal = normalize(faces.at(1).normal + faces.at(2).normal + faces.at(3).normal);
	vertices.at(1).normal = normalize(faces.at(0).normal + faces.at(1).normal + faces.at(3).normal);
	vertices.at(2).normal = normalize(faces.at(0).normal + faces.at(2).normal + faces.at(3).normal);
	vertices.at(3).normal = normalize(faces.at(1).normal + faces.at(2).normal + faces.at(0).normal);


	// 0->z->y
	faces.at(0).vertices[0] = 3;//0
	faces.at(0).vertices[1] = 2;//0
	faces.at(0).vertices[2] = 1;//0


	/**
	vertex_positions.push_back(vertices.at(3));
	vertex_normal_positions.push_back(vertex_normals.at(3));
	vertex_positions.push_back(vertices.at(2));
	vertex_normal_positions.push_back(vertex_normals.at(2));
	vertex_positions.push_back(vertices.at(1));
	vertex_normal_positions.push_back(vertex_normals.at(1));
	*/

	// 0->y->x

	faces.at(1).vertices[0] = 3;//0
	faces.at(1).vertices[1] = 1;//y
	faces.at(1).vertices[2] = 0;//x

	/**
	* 
	* vertex_positions.push_back(vertices.at(3));
	vertex_normal_positions.push_back(vertex_normals.at(3));
	vertex_positions.push_back(vertices.at(1));
	vertex_normal_positions.push_back(vertex_normals.at(1));
	vertex_positions.push_back(vertices.at(0));
	vertex_normal_positions.push_back(vertex_normals.at(0));
	*/
	
	// 0->x->z
	faces.at(2).vertices[0] = 3;//0
	faces.at(2).vertices[1] = 0;//x
	faces.at(2).vertices[2] = 2;//z

	/**
		vertex_positions.push_back(vertices.at(3));
	vertex_normal_positions.push_back(vertex_normals.at(3));
	vertex_positions.push_back(vertices.at(0));
	vertex_normal_positions.push_back(vertex_normals.at(0));
	vertex_positions.push_back(vertices.at(2));
	vertex_normal_positions.push_back(vertex_normals.at(2));
	*/
	
	// z->x->y

	faces.at(3).vertices[0] = 2;//z
	faces.at(3).vertices[1] = 0;//x
	faces.at(3).vertices[2] = 1;//y

	/**
	* vertex_positions.push_back(vertices.at(2));
	vertex_normal_positions.push_back(vertex_normals.at(2));
	vertex_positions.push_back(vertices.at(0));
	vertex_normal_positions.push_back(vertex_normals.at(0));
	vertex_positions.push_back(vertices.at(1));
	vertex_normal_positions.push_back(vertex_normals.at(1));
	*/
	
	
	boundingBox();

}

void PrimMeshModel::Cube()
{
	/**
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
	*/
	

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
