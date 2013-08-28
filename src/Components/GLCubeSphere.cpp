#include "GLCubeSphere.h"
#include "GLIcoSphere.h"
#include <vector>
#include <algorithm>

// For std::find
bool operator ==(const vertex &lhs, const vertex &rhs) { return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z); }

GLCubeSphere::GLCubeSphere( const int entityID /*= 0*/ ) : IGLComponent(entityID) {
	this->drawMode = GL_TRIANGLES;
	this->ElemBufIndex = 2;
	this->ColorBufIndex = 1;
	this->VertBufIndex = 0;
	this->NormalBufIndex = 3;
}

void GLCubeSphere::Initialize() {
	srand(this->GetEntityID());

	int subdivisions = 5;

	// Create the verts to begin refining at.
	float t = 0.5f;
	glm::vec3 coordPair(t, t, t);

	this->verts.push_back(vertex(-coordPair.x, -coordPair.y, coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(coordPair.x, -coordPair.y, coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(coordPair.x, coordPair.y, coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(-coordPair.x, coordPair.y, coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(-coordPair.x, -coordPair.y, -coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(coordPair.x, -coordPair.y, -coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(coordPair.x, coordPair.y, -coordPair.z));
	this->colors.push_back(color(0,1,0));
	this->verts.push_back(vertex(-coordPair.x, coordPair.y, -coordPair.z));
	this->colors.push_back(color(0,1,0));

	// front
	this->faces.push_back(face(0,1,2));
	this->faces.push_back(face(2,3,0));
	// top
    this->faces.push_back(face(3,2,6));
    this->faces.push_back(face(6, 7, 3));
    // back
    this->faces.push_back(face(7, 6, 5));
    this->faces.push_back(face(5, 4, 7));
    // bottom
    this->faces.push_back(face(4, 5, 1));
    this->faces.push_back(face(1, 0, 4));
    // left
    this->faces.push_back(face(4, 0, 3));
    this->faces.push_back(face(3, 7, 4));
    // right
    this->faces.push_back(face(1, 5, 6));
    this->faces.push_back(face(6, 2, 1));

	this->SubDivide(subdivisions);

	std::vector<vertex> surfaceNorms;

	// compute surface normals
	for(size_t i = 0; i < faces.size(); i++) {
		glm::vec3 vector1, vector2, cross, normal;
		vertex vert1(verts[faces[i].v1]), vert2(verts[faces[i].v2]), vert3(verts[faces[i].v3]);

		vector1 = glm::vec3(vert2.x-vert1.x, vert2.y-vert1.y, vert2.z-vert1.z);
		vector2 = glm::vec3(vert3.x-vert1.x, vert3.y-vert1.y, vert3.z-vert1.z);
		cross = glm::cross(vector1, vector2);
		normal = glm::normalize(cross);

		surfaceNorms.push_back(vertex(normal.x, normal.y, normal.z));
	}

	// compute vertex normals
	// should probably compute adjacency first, this could be slow
	for(size_t i = 0; i < verts.size(); i++) {
		vertex total_normals(0.0f, 0.0f, 0.0f);

		for(size_t j = 0; j < faces.size(); j++) {
			if ((faces[j].v1 == i || faces[j].v2 == i || faces[j].v3 == i)) {
				total_normals.x += surfaceNorms[j].x;
				total_normals.y += surfaceNorms[j].y;
				total_normals.z += surfaceNorms[j].z;
			}
		}

		glm::vec3 final_normal(total_normals.x, total_normals.y, total_normals.z);
		final_normal = glm::normalize(final_normal);
		vertNorms.push_back(vertex(final_normal.x, final_normal.y, final_normal.z));
	}

	surfaceNorms.clear();

	// We must create a vao and then store it in our GLIcoSphere.
	glGenVertexArrays(1, &this->vao); // Generate the VAO
	glBindVertexArray(this->vao); // Bind the VAO

	glGenBuffers(1, &this->buffers[this->VertBufIndex]); 	// Generate the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, this->buffers[this->VertBufIndex]); // Bind the vertex buffer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * this->verts.size(), &this->verts.front(), GL_STATIC_DRAW); // Stores the verts in the vertex buffer.
	GLint posLocation = glGetAttribLocation(GLIcoSphere::shader.GetProgram(), "in_Position"); // Find the location in the shader where the vertex buffer data will be placed.
	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell the VAO the vertex data will be stored at the location we just found.
	glEnableVertexAttribArray(posLocation); // Enable the VAO line for vertex data.

	glGenBuffers(1, &this->buffers[this->ColorBufIndex]);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffers[this->ColorBufIndex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color) * this->colors.size(), &this->colors.front(), GL_STATIC_DRAW);
	GLint colLocation = glGetAttribLocation(GLIcoSphere::shader.GetProgram(), "in_Color");
	glVertexAttribPointer(colLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colLocation);

	glGenBuffers(1, &this->buffers[this->ElemBufIndex]); // Generate the element buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers[this->ElemBufIndex]); // Bind the element buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face) * this->faces.size(), &this->faces.front(), GL_STATIC_DRAW); // Store the faces in the element buffer.

	if (this->vertNorms.size() > 0) {
		glGenBuffers(1, &this->buffers[this->NormalBufIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, this->buffers[this->NormalBufIndex]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*this->vertNorms.size(), &this->vertNorms[0], GL_STATIC_DRAW);
		GLint normalLocation = glGetAttribLocation(GLIcoSphere::shader.GetProgram(), "in_Normal");
		glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(normalLocation);
	}

	glBindVertexArray(0); // Reset the buffer binding because we are good programmers.

	// albedo map
	glGenTextures(1, &this->_cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->_cubeMap);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

	// There is always six filenames
	for(int i=1; i < 7; i++) {
		char filename[100];
		sprintf_s(filename, "mars%d.jpg", i);
		//BMPData* bmpData = LoadTextureBMPData_custom(filename);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB,bmpData->width,bmpData->height,0,GL_BGR,GL_UNSIGNED_BYTE,&(bmpData->data[0]));
	}
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void GLCubeSphere::SubDivide(int levels) {
	std::vector<face> newFaces;

	// Iterate over each face and subdivide it
	// Note: currently produces redundant vertices
	for(std::vector<face>::iterator i = this->faces.begin(); i != this->faces.end(); ++i) {
		vertex v1(0, 0, 0), v2(0, 0, 0), newVert(0, 0, 0);
		face newFace(0, 0, 0);

		short i1, i2, i3;

		// Split each edge
		v1 = this->verts[(*i).v1];
		v2 = this->verts[(*i).v2];

		newVert.x = (v1.x + v2.x)/2.0f;
		newVert.y = (v1.y + v2.y)/2.0f;
		newVert.z = (v1.z + v2.z)/2.0f;

		// See if vertex already exists
		std::vector<vertex>::iterator existingVert = std::find(this->verts.begin(), this->verts.end(), newVert);
		if(existingVert==this->verts.end()) {
			i1 = verts.size();
			this->verts.push_back(newVert);
			this->colors.push_back(color(0,1,0));
		}
		else {
			i1 = std::distance(this->verts.begin(), existingVert);
		}

		v1 = this->verts[(*i).v1];
		v2 = this->verts[(*i).v3];

		newVert.x = (v1.x + v2.x)/2.0f;
		newVert.y = (v1.y + v2.y)/2.0f;
		newVert.z = (v1.z + v2.z)/2.0f;

		existingVert = std::find(this->verts.begin(), this->verts.end(), newVert);
		if(existingVert==this->verts.end()) {
			i2 = this->verts.size();
			this->verts.push_back(newVert);
			this->colors.push_back(color(0,1,0));
		}
		else {
			i2 = std::distance(this->verts.begin(), existingVert);
		}

		v1 = this->verts[(*i).v2];
		v2 = this->verts[(*i).v3];

		newVert.x = (v1.x + v2.x)/2.0f;
		newVert.y = (v1.y + v2.y)/2.0f;
		newVert.z = (v1.z + v2.z)/2.0f;

		existingVert = std::find(this->verts.begin(), this->verts.end(), newVert);
		if(existingVert==verts.end()) {
			i3 = this->verts.size();
			this->verts.push_back(newVert);
			this->colors.push_back(color(0,1,0));
		}
		else {
			i3 = std::distance(this->verts.begin(), existingVert);
		}

		newFace.v1 = (*i).v1;
		newFace.v2 = i1;
		newFace.v3 = i2;
		newFaces.push_back(newFace);

		newFace.v1 = i1;
		newFace.v2 = (*i).v2;
		newFace.v3 = i3;
		newFaces.push_back(newFace);

		newFace.v1 = i3;
		newFace.v2 = i2;
		newFace.v3 = i1;
		newFaces.push_back(newFace);

		newFace.v1 = i2;
		newFace.v2 = i3;
		newFace.v3 = (*i).v3;
		newFaces.push_back(newFace);
	}

	this->faces = newFaces;

	if(levels > 1) {
		this->SubDivide(levels-1);
	}
}