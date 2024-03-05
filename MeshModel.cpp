
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
	vector<FaceIdcs> tempfaces;
	vector<vec3> vertex_normals;
	vector<vec3> vertex_textures;
	vector<vec3> verticesText;
	vector<vec3> verticesNormal;
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

		/*Vertex current_vertex;
		PolygonTri current_face;*/


		// based on the type parse data
	if (lineType == "v")
	{
		vertices.push_back(vec3fFromStream(issLine));
	}
	else if (lineType == "vn")
	{
		//vertices.at(current_normal_index).normal = vec3fFromStream(issLine);
		//current_normal_index++;
		vertex_normals.push_back(vec3fFromStream(issLine));
	}
	else if (lineType == "vt")
	{
		vertex_textures.push_back(vec3fFromStream(issLine));
		/*vec2 temp = vec2fFromStream(issLine);
		vertices.at(current_normal_index).texture_x = temp.x;
		vertices.at(current_normal_index).texture_y = temp.y;
		current_texture_index++;*/
	}

	else if (lineType == "f")
	tempfaces.push_back(issLine);
	else if (lineType == "#" || lineType == "")
	{

	}
	else
	{
		cout << "Found unknown line Type \"" << lineType << "\"";
	}
		}

	//for (vector<PolygonTri>::iterator it = faces.begin(); it != faces.end(); ++it)
	for (vector<FaceIdcs>::iterator it = tempfaces.begin(); it != tempfaces.end(); ++it)
	{
		Face current_face;
		//calculate face normals
		vec3 xi, xj, xk, normal;
		GLfloat norm;

		/*
		xi = truncateVec4(vertices.at((*it).vertices[0] - 1).homogenous);
		xj = truncateVec4(vertices.at((*it).vertices[1] - 1).homogenous);
		xk = truncateVec4(vertices.at((*it).vertices[2] - 1).homogenous);*/

		xi = vertices.at((*it).v[0] - 1);
		xj = vertices.at((*it).v[1] - 1);
		xk = vertices.at((*it).v[2] - 1);
		normal = cross((xj - xi), (xj - xk));
		current_face.normal = normalize(normal);
		current_face.normal.w = 0;
		current_face.position = ((xi + xj + xk) / 3);

		current_face.v1.position = xi;
		current_face.v2.position = xj;
		current_face.v3.position = xk;

		//only add if a texture was given
		if ((*it).vt[0] != 0)
		{
			current_face.v1.texture = vertex_textures.at((*it).vt[0] - 1);
		}
		if ((*it).vt[1] != 0)
		{
			current_face.v2.texture = vertex_textures.at((*it).vt[1] - 1);
		}
		if ((*it).vt[2] != 0)
		{
			current_face.v3.texture = vertex_textures.at((*it).vt[2] - 1);
		}

		if (vertex_normals.empty())
		{
			current_face.v1.normal += normal;
			current_face.v2.normal += normal;
			current_face.v3.normal += normal;
			//vertex_normals.at((*it).v[i] - 1) += normal; 
		}
		else
		{
			current_face.v1.normal = vertex_normals.at((*it).vn[0] - 1);
			current_face.v2.normal = vertex_normals.at((*it).vn[1] - 1);
			current_face.v3.normal = vertex_normals.at((*it).vn[2] - 1);
		}
		faces.push_back(current_face);
	}

	//normalize all normals 
	//TODO: make sure normalizing is correct
	/*if (calculate_vnorm)
	{
		for (auto& v : vertices) {
			v.normal = normalize(v.normal);
		}
	}*/
	for (size_t i = 0; i < faces.size(); i++)
	{
		faces.at(i).v1.normal = normalize(faces.at(i).v1.normal);
		faces.at(i).v2.normal = normalize(faces.at(i).v2.normal);
		faces.at(i).v3.normal = normalize(faces.at(i).v3.normal);

		faces.at(i).v1.normal.w = 0;
		faces.at(i).v2.normal.w = 0;
		faces.at(i).v3.normal.w = 0;
	}

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
	Face f1, f2, f3, f4;
	Vertex _x, _y, _z, _o;
	vertices.push_back(vec3(1, 0, 0));
	vertices.push_back(vec3(0, 1, 0));
	vertices.push_back(vec3(0, 0, 1));
	vertices.push_back(vec3(0, 0, 0));

	_x.position = vec3(1, 0, 0);
	_y.position = vec3(0, 1, 0);
	_z.position = vec3(0, 0, 1);
	_o.position = vec3(0, 0, 0);

	f1.normal = vec4(-1, 0, 0,0);
	f2.normal = vec4(0, 0, -1,0);
	f3.normal = vec4(0, -1, 0,0);
	f4.normal= vec4(1, 1, 1,0);


	f1.position = vec3(0, 0.33333333, 0.33333333);
	f2.position = vec3(0.33333333, 0.33333333, 0);
	f3.position = vec3(0.33333333, 0, 0.33333333);
	f4.position = vec3(0.33333333, 0.33333333, 0.33333333);

	_x.normal = normalize(f2.normal + f3.normal + f4.normal);
	_y.normal = normalize(f1.normal + f2.normal + f4.normal);
	_z.normal = normalize(f1.normal + f3.normal + f4.normal);
	_o.normal = normalize(f2.normal + f3.normal + f1.normal);

	_x.normal.w = 0;
	_y.normal.w = 0; 
	_z.normal.w = 0;
	_o.normal.w = 0;

	f1.v1 = _o;
	f1.v2 = _z;
	f1.v3 = _y;

	f2.v1 = _o;
	f2.v2 = _y;
	f2.v3 = _x;

	f3.v1 = _o;
	f3.v2 = _x;
	f3.v3 = _z;
	
	f4.v1 = _z;
	f4.v2 = _x;
	f4.v3 = _y;
	
	faces.push_back(f1);
	faces.push_back(f2);
	faces.push_back(f3);
	faces.push_back(f4);

	//Vertex temp;
	////x
	//temp.homogenous = vec4(1, 0, 0, 1);
	//
	//vertices.push_back(temp);
	////y
	//temp.homogenous = vec4(0, 1, 0, 1);

	//vertices.push_back(temp);
	////z
	//temp.homogenous = vec4(0, 0, 1, 1);
	//vertices.push_back(temp);

	////0
	//temp.homogenous = vec4(0, 0, 0, 1);
	//vertices.push_back(temp);

	//PolygonTri temp_face;
	//temp_face.normal = vec3(-1, 0, 0);
	//temp_face.origin = vec3(0, 0.33333333, 0.33333333);
	//faces.push_back(temp_face);

	//temp_face.normal = vec3(0, 0, -1);
	//temp_face.origin = vec3(0.33333333, 0.33333333, 0);
	//faces.push_back(temp_face);

	//temp_face.normal = vec3(0, -1, 0);
	//temp_face.origin = vec3(0.33333333, 0, 0.33333333);
	//faces.push_back(temp_face);

	//temp_face.normal = normalize(vec3(1, 1, 1));
	//temp_face.origin = vec3(0.33333333, 0.33333333, 0.33333333);
	//faces.push_back(temp_face);

	//vertices.at(0).normal = normalize(faces.at(1).normal + faces.at(2).normal + faces.at(3).normal);
	//vertices.at(1).normal = normalize(faces.at(0).normal + faces.at(1).normal + faces.at(3).normal);
	//vertices.at(2).normal = normalize(faces.at(0).normal + faces.at(2).normal + faces.at(3).normal);
	//vertices.at(3).normal = normalize(faces.at(1).normal + faces.at(2).normal + faces.at(0).normal);


	//// 0->z->y
	//faces.at(0).vertices[0] = 3;//0
	//faces.at(0).vertices[1] = 2;//0
	//faces.at(0).vertices[2] = 1;//0


	/**
	vertex_positions.push_back(vertices.at(3));
	vertex_normal_positions.push_back(vertex_normals.at(3));
	vertex_positions.push_back(vertices.at(2));
	vertex_normal_positions.push_back(vertex_normals.at(2));
	vertex_positions.push_back(vertices.at(1));
	vertex_normal_positions.push_back(vertex_normals.at(1));
	*/

	// 0->y->x

	//faces.at(1).vertices[0] = 3;//0
	//faces.at(1).vertices[1] = 1;//y
	//faces.at(1).vertices[2] = 0;//x

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
	//faces.at(2).vertices[0] = 3;//0
	//faces.at(2).vertices[1] = 0;//x
	//faces.at(2).vertices[2] = 2;//z

	/**
		vertex_positions.push_back(vertices.at(3));
	vertex_normal_positions.push_back(vertex_normals.at(3));
	vertex_positions.push_back(vertices.at(0));
	vertex_normal_positions.push_back(vertex_normals.at(0));
	vertex_positions.push_back(vertices.at(2));
	vertex_normal_positions.push_back(vertex_normals.at(2));
	*/
	
	// z->x->y

	//faces.at(3).vertices[0] = 2;//z
	//faces.at(3).vertices[1] = 0;//x
	//faces.at(3).vertices[2] = 1;//y

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
