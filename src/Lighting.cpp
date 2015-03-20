#include "Lighting.h"

#include "GLM_Header.h"
#include "GL_Header.h"
#include "Gizmos.h"
#include "stb_image.h"

unsigned int Lighting::loadShader(unsigned int type, const char* path) {
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
		return 0;
	// read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;
	return shader;
}

void Lighting::createDrawShader()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER,
		"lighting_vertex.glsl");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER,
		"lighting_fragment.glsl");
	m_program = glCreateProgram();
	glAttachShader(m_program, vs);
	glAttachShader(m_program, fs);
	glLinkProgram(m_program);
	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Lighting::createOpenGLBuffers(FBXFile* fbx)
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		// storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];
		// continued next page…
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::NormalOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
	}
}void Lighting::cleanupOpenGLBuffers(FBXFile* fbx) {
	// clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);
		delete[] glData;
	}
}
//void Lighting::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
//{
//	m_gl_info.resize(shapes.size());
//	for (unsigned int mesh_index = 0;
//		mesh_index < shapes.size();
//		++mesh_index)
//	{
//		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
//		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
//		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
//		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
//		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
//		float_count += shapes[mesh_index].mesh.normals.size();
//		float_count += shapes[mesh_index].mesh.texcoords.size();
//		std::vector<float> vertex_data;
//		vertex_data.reserve(float_count);
//		vertex_data.insert(vertex_data.end(),
//			shapes[mesh_index].mesh.positions.begin(),
//			shapes[mesh_index].mesh.positions.end());
//		vertex_data.insert(vertex_data.end(),
//			shapes[mesh_index].mesh.normals.begin(),
//			shapes[mesh_index].mesh.normals.end());
//		m_gl_info[mesh_index].m_index_count =
//			shapes[mesh_index].mesh.indices.size();
//		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
//		glBufferData(GL_ARRAY_BUFFER,
//			vertex_data.size() * sizeof(float),
//			vertex_data.data(), GL_STATIC_DRAW);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
//			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
//		glEnableVertexAttribArray(0); //position
//		glEnableVertexAttribArray(1); //normal data
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
//			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	}
//}

bool Lighting::startup()
{
	m_background_color.r = 0.75f;
	m_background_color.g = 0.75f;
	m_background_color.b = 0.75f;
	m_background_color.a = 1.0f;

	if (Application::startup() == false)
	{
		return false;
	}


	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	m_fbx = new FBXFile();
	m_fbx->load("./data/models/stanford/Bunny.fbx");
	createOpenGLBuffers(m_fbx);

	// specular lighting
	//const char* vsSource = "#version 410\n \
	//					   layout(location=0) in vec4 Position; \
	//					   layout(location=1) in vec4 Normal; \
	//					   out vec4 vNormal; \
	//					   out vec4 vPosition; \
	//					   uniform mat4 ProjectionView; \
	//					   void main() { vNormal = Normal; \
	//					   vPosition = Position; \
	//					   gl_Position = ProjectionView*Position; }";
	//
	//const char* fsSource = "#version 410\n \
	//					   in vec4 vNormal; \
	//					   in vec4 vPosition; \
	//					   out vec4 FragColor; \
	//					   uniform vec3 LightDir; \
	//					   uniform vec3 LightColour; \
	//					   uniform vec3 CameraPos; \
	//					   uniform float SpecPow; \
	//					   void main() { \
	//					   float d = max(0, \
	//					   dot(normalize(vNormal.xyz),LightDir ) ); \
	//					   vec3 E = normalize( CameraPos - \
	//					    vPosition.xyz );\
	//						vec3 R = reflect( -LightDir, \
	//						vNormal.xyz ); \
	//						float s = max( 0, dot( E, R ) ); \
	//						s = pow( s, SpecPow ); \
	//						FragColor = vec4( LightColour * d + \
	//						LightColour * s, 1); }";

	// directional lighting
	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Normal; \
						   out vec4 vNormal; \
						   uniform mat4 ProjectionView; \
						   void main() { vNormal = Normal; \
						   gl_Position = ProjectionView*Position; }";
	const char* fsSource = "#version 410\n \
						   in vec4 vNormal; \
						   out vec4 FragColor; \
						   void main() { \
						   float d = max(0, \
						   dot( normalize(vNormal.xyz), \
						   vec3(0,1,0) ) ); \
						   FragColor = vec4(d,d,d,1); }";
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;
	//
	//std::string err = tinyobj::LoadObj(shapes, materials, "./data/models/stanford/bunny.obj");

	//createDrawShader();
	//createOpenGLBuffers(shapes);
	return true;
}

void Lighting::shutdown()
{
	Gizmos::destroy();
	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_program);
	Application::shutdown();
}


bool Lighting::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();
	timer += dt;

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
			i == 10 ? white : black);
	}
	camera.update(dt);


	return true;
}

void Lighting::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glUseProgram(m_program);
	//int view_proj_uniform = glGetUniformLocation(m_program, "projView");
	//glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&camera.view_proj);
	//for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	//{
	//	glBindVertexArray(m_gl_info[i].m_VAO);
	//	glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	//}
	glUseProgram(m_program);
	// bind the camera
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(camera.view_proj[0][0]));
	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	Gizmos::draw(camera.view_proj);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}