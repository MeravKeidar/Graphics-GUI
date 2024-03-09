
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

struct FaceIdcs
{
	// vertices positions
	int v[4] = { 0,0,0,0 };
	// vertices normals
	int vn[4]= { 0,0,0,0 };
	// vertices textures
	int vt[4]= { 0,0,0,0 };

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
	//Note: faces is always at the end of file. can directly construct the face sturcture
	vector<FaceIdcs> tempfaces;
	/**
		vector<vec3> vertex_normals;
	vector<vec3> vertex_textures;
	vector<vec3> verticesText;
	vector<vec3> verticesNormal;
	*/
	
	GLfloat min_x = numeric_limits<float>::max();
	GLfloat min_y = min_x;
	GLfloat min_z = min_x;
	GLfloat max_x = numeric_limits<float>::min();
	GLfloat max_y = max_x;
	GLfloat max_z = max_x;
	bool calculate_vertex_normals = false;

	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);
		bool calculate_vertex_normals = true;
		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		if (lineType == "v")
		{
			vec3 current_v3 = vec3fFromStream(issLine);
			vertices.push_back(current_v3);
			max_x = max(max_x, current_v3.x);
			max_y = max(max_y, current_v3.y);
			max_z = max(max_z, current_v3.z);

			min_x = min(min_x, current_v3.x);
			min_y = min(min_y, current_v3.y);
			min_z = min(min_x, current_v3.z);

		}
		else if (lineType == "vn")
		{
			normals.push_back(vec3fFromStream(issLine));
		}
		else if (lineType == "vt")
		{
			//TODO: avoid for now from textures
			//vertex_textures.push_back(vec3fFromStream(issLine));
		}

		else if (lineType == "f")
		{
			//Note: down the line the face normal for drawing and for calculation(acquired via averaging vertex normals) are not the same -- they should be...
			//Note: keep textures for later
			FaceIdcs face_indices(issLine);
			tempfaces.push_back(face_indices);
			
		
		}
	
		else if (lineType != "#" && lineType != "")
		{
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	
	}

	if (normals.empty())
	{
		calculate_vertex_normals = true;
		for (size_t i = 0; i < vertices.size(); i++)
		{
			normals.push_back(Normal(vec3(0, 0, 0)));
		}
	}

	for (vector<FaceIdcs>::iterator it = tempfaces.begin(); it != tempfaces.end(); ++it)
	{
		Face current_face;
		current_face.v1 = &vertices.at((*it).v[0] - 1);
		current_face.v2 = &vertices.at((*it).v[1] - 1);
		current_face.v3 = &vertices.at((*it).v[2] - 1);

		vec4 temp_vec = current_face.v1->raw_position + current_face.v2->raw_position + current_face.v3->raw_position;
		vec3 current_face_center_position = (normalize(truncateVec4(temp_vec)));
		//vertices.push_back(current_face_center_position);  BUG
		current_face.face_center = Vertex(current_face_center_position);

		vec3 xi, xj, xk;
		xi = truncateVec4(current_face.v1->raw_position);
		xj = truncateVec4(current_face.v2->raw_position);
		xk = truncateVec4(current_face.v3->raw_position);
		vec4 normal_direction = (cross((xj - xi), (xj - xk)), 0);
		normal_direction = normalize(normal_direction);
		current_face.face_normal.original_direction = normal_direction;

		if (calculate_vertex_normals)
		{
			current_face.v1_normal = &normals.at((*it).v[0] - 1);
			current_face.v1_normal->original_direction += normal_direction;

			current_face.v2_normal = &normals.at((*it).v[1] - 1);
			current_face.v2_normal->original_direction += normal_direction;

			current_face.v3_normal = &normals.at((*it).v[2] - 1);
			current_face.v3_normal->original_direction += normal_direction;
		}
		else
		{
			current_face.v1_normal = &normals.at((*it).vn[0] - 1);
			current_face.v2_normal = &normals.at((*it).vn[1] - 1);
			current_face.v3_normal = &normals.at((*it).vn[2] - 1);
		}

		faces.push_back(current_face);
	}

	boundingBox(min_x, min_y, min_z, max_x, max_y, max_z);

}

//All the old code from loadfile here
	//vec3 normal3 = cross((xj - xi), (xj - xk));
			//Normal normal(normal3);
			////add face normal to pool of normals
			//normals.push_back(normal3);
			////current_face.face_normal = &(*(normals.end() - 1));
			////TODO: reconstruct calculating vertex normal logic
			//Normal custom_vertex_normal(vec3(0));
			//
			//if (face_indices.vn[0] != 0)
			//{
			//	current_face.v1_normal = &normals.at(face_indices.vn[0] - 1);
			//	current_face.v2_normal = &normals.at(face_indices.vn[1] - 1);
			//	current_face.v3_normal = &normals.at(face_indices.vn[2] - 1);
			//}
			//else
			//{
				/*if (current_face.v1_normal == NULL)
				{
					normals.push_back(custom_vertex_normal);
					current_face.v1_normal = &(*(normals.end() - 1));
				}
				current_face.v1_normal->original_direction += current_face.face_normal->original_direction;

				if (current_face.v2_normal == NULL)
				{
					normals.push_back(custom_vertex_normal);
					current_face.v2_normal = &(*(normals.end() - 1));
				}
				current_face.v2_normal->original_direction += current_face.face_normal->original_direction;

				if (current_face.v3_normal == NULL)
				{
					normals.push_back(custom_vertex_normal);
					current_face.v3_normal = &(*(normals.end() - 1));
				}
				current_face.v3_normal->original_direction += current_face.face_normal->original_direction;*/
				//}


	// the face calculation thingy has to be at the end... 
	/**
	for (vector<FaceIdcs>::iterator it = tempfaces.begin(); it != tempfaces.end(); ++it)
	{
		Face current_face;
		//calculate face normals
		vec3 xi, xj, xk, normal;

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
		}
		else
		{
			current_face.v1.normal = vertex_normals.at((*it).vn[0] - 1);
			current_face.v2.normal = vertex_normals.at((*it).vn[1] - 1);
			current_face.v3.normal = vertex_normals.at((*it).vn[2] - 1);
		}
		faces.push_back(current_face);
	}
	*/
	

	//normalize all normals 
	/**
			for (size_t i = 0; i < faces.size(); i++)
	{
		faces.at(i).v1.normal = normalize(faces.at(i).v1.normal);
		faces.at(i).v2.normal = normalize(faces.at(i).v2.normal);
		faces.at(i).v3.normal = normalize(faces.at(i).v3.normal);

		faces.at(i).v1.normal.w = 0;
		faces.at(i).v2.normal.w = 0;
		faces.at(i).v3.normal.w = 0;
	}
	*/

	//Note: collect min max before bounding box and handing over the values
	//Assume all vertices of model are in use(otherwise bounding box would give illusion of bigger than needed)
	


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

void MeshModel::boundingBox(GLfloat min_x, GLfloat min_y, GLfloat min_z, GLfloat max_x, GLfloat max_y, GLfloat max_z)
{
	if (vertices.empty()){return;}

	//Note: settle for this center of mass for now
	_center_of_mass = vec4((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2, 1);


	bounding_box.push_back(vec3(min_x, min_y, min_z));
	bounding_box.push_back(vec3(max_x, min_y, min_z));
	bounding_box.push_back(vec3(max_x, min_y, max_z));
	bounding_box.push_back(vec3(min_x, min_y, max_z));
	bounding_box.push_back(vec3(min_x, max_y, min_z));
	bounding_box.push_back(vec3(max_x, max_y, min_z));
	bounding_box.push_back(vec3(max_x, max_y, max_z));
	bounding_box.push_back(vec3(min_x, max_y, max_z));
	vec3 model_center((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + min_z) / 2);

	_world_transform = TranslationMat(-model_center) * _world_transform;
	_normal_world_transform = TranslationMat(-model_center) * _normal_world_transform;
}




void PrimMeshModel::Tetrahedron()
{
	loadFile("obj_files/tetrahedron.obj");
	
	/*Face f1, f2, f3, f4;
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
	*/
	

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
	
	
	//boundingBox();

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

//vec3 xi, xj, xk;
//xi = truncateVec4(current_face.v1->raw_position);
//xj = truncateVec4(current_face.v2->raw_position);
//xk = truncateVec4(current_face.v3->raw_position);
//vec4 normal_direction = (cross((xj - xi), (xj - xk)), 0);
//normal_direction = normalize(normal_direction);
//current_face.face_normal.original_direction = normal_direction;
//
//if (calculate_vertex_normals)
//{
//	current_face.v1_normal->original_direction += normal_direction;
//	current_face.v2_normal->original_direction += normal_direction;
//	current_face.v3_normal->original_direction += normal_direction;
//}
//else
//{
//	current_face.v1_normal = &normals.at(face_indices.vn[0] - 1);
//	current_face.v2_normal = &normals.at(face_indices.vn[1] - 1);
//	current_face.v3_normal = &normals.at(face_indices.vn[2] - 1);
//}
//faces.push_back(current_face);
