//#include "Shadows.h"
//
//#include "GLM_Header.h"
//#include "GL_Header.h"
//#include "Gizmos.h"
//#include "stb_image.h"
//
//unsigned int Shadows::loadShader(unsigned int type, const char* path) {
//	FILE* file = fopen(path, "rb");
//	if (file == nullptr)
//		return 0;
//	// read the shader source
//	fseek(file, 0, SEEK_END);
//	unsigned int length = ftell(file);
//	fseek(file, 0, SEEK_SET);
//	char* source = new char[length + 1];
//	memset(source, 0, length + 1);
//	fread(source, sizeof(char), length, file);
//	fclose(file);
//	unsigned int shader = glCreateShader(type);
//	glShaderSource(shader, 1, &source, 0);
//	glCompileShader(shader);
//	delete[] source;
//	return shader;
//}
//
//void Shadows::createDrawShader()
//{
//	unsigned int vs = loadShader(GL_VERTEX_SHADER,
//		"Shadow.vert");
//	unsigned int fs = loadShader(GL_FRAGMENT_SHADER,
//		"Shadow.frag");
//	m_useShadowProgram = glCreateProgram();
//	glAttachShader(m_useShadowProgram, vs);
//	glAttachShader(m_useShadowProgram, fs);
//	glLinkProgram(m_useShadowProgram);
//	// remove unneeded handles
//	glDeleteShader(vs);
//	glDeleteShader(fs);
//}
//
//void Shadows::createShadowShader()
//{
//	unsigned int vs = loadShader(GL_VERTEX_SHADER,
//		"ShadowMap.vert");
//	unsigned int fs = loadShader(GL_FRAGMENT_SHADER,
//		"ShadowMap.frag");
//	m_shadowGenProgram = glCreateProgram();
//	glAttachShader(m_shadowGenProgram, vs);
//	glAttachShader(m_shadowGenProgram, fs);
//	glLinkProgram(m_shadowGenProgram);
//	// remove unneeded handles
//	glDeleteShader(vs);
//	glDeleteShader(fs);
//}
//
//void Shadows::createOpenGLBuffers(FBXFile* fbx)
//{
//	// create the GL VAO/VBO/IBO data for each mesh
//	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
//	{
//		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
//		// storage for the opengl data in 3 unsigned int
//		unsigned int* glData = new unsigned int[3];
//		// continued next page…
//		glGenVertexArrays(1, &glData[0]);
//		glBindVertexArray(glData[0]);
//		glGenBuffers(1, &glData[1]);
//		glGenBuffers(1, &glData[2]);
//		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
//		glBufferData(GL_ARRAY_BUFFER,
//			mesh->m_vertices.size() * sizeof(FBXVertex),
//			mesh->m_vertices.data(), GL_STATIC_DRAW);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//			mesh->m_indices.size() * sizeof(unsigned int),
//			mesh->m_indices.data(), GL_STATIC_DRAW);
//		glEnableVertexAttribArray(0); // position
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
//			sizeof(FBXVertex), 0);
//		glEnableVertexAttribArray(1); // normal
//		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
//			sizeof(FBXVertex),
//			((char*)0) + FBXVertex::NormalOffset);
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//		mesh->m_userData = glData;
//	}
//}////void Shadows::cleanupOpenGLBuffers(FBXFile* fbx) {
//	// clean up the vertex data attached to each mesh
//	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i) {
//		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
//		unsigned int* glData = (unsigned int*)mesh->m_userData;
//		glDeleteVertexArrays(1, &glData[0]);
//		glDeleteBuffers(1, &glData[1]);
//		glDeleteBuffers(1, &glData[2]);
//		delete[] glData;
//	}
//}//
////void Lighting::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
////{
////	m_gl_info.resize(shapes.size());
////	for (unsigned int mesh_index = 0;
////		mesh_index < shapes.size();
////		++mesh_index)
////	{
////		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
////		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
////		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
////		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
////		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
////		float_count += shapes[mesh_index].mesh.normals.size();
////		float_count += shapes[mesh_index].mesh.texcoords.size();
////		std::vector<float> vertex_data;
////		vertex_data.reserve(float_count);
////		vertex_data.insert(vertex_data.end(),
////			shapes[mesh_index].mesh.positions.begin(),
////			shapes[mesh_index].mesh.positions.end());
////		vertex_data.insert(vertex_data.end(),
////			shapes[mesh_index].mesh.normals.begin(),
////			shapes[mesh_index].mesh.normals.end());
////		m_gl_info[mesh_index].m_index_count =
////			shapes[mesh_index].mesh.indices.size();
////		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
////		glBufferData(GL_ARRAY_BUFFER,
////			vertex_data.size() * sizeof(float),
////			vertex_data.data(), GL_STATIC_DRAW);
////		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
////		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
////			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
////			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
////		glEnableVertexAttribArray(0); //position
////		glEnableVertexAttribArray(1); //normal data
////		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
////		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
////			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
////		glBindVertexArray(0);
////		glBindBuffer(GL_ARRAY_BUFFER, 0);
////		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
////	}
////}
//
//bool Shadows::startup()
//{
//	m_background_color.r = 0.75f;
//	m_background_color.g = 0.75f;
//	m_background_color.b = 0.75f;
//	m_background_color.a = 1.0f;
//
//	if (Application::startup() == false)
//	{
//		return false;
//	}
//
//
//	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
//	glEnable(GL_DEPTH_TEST);
//	Gizmos::create();
//	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
//	camera.setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
//	camera.sensitivity = 3;
//	m_fbx = new FBXFile();
//	m_fbx->load("./data/models/stanford/Bunny.fbx");
//	createOpenGLBuffers(m_fbx);
//	createShadowShader();
//	createDrawShader();
//	glGenFramebuffers(1, &m_fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//	glGenTextures(1, &m_fboDepth);
//	glBindTexture(GL_TEXTURE_2D, m_fboDepth);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024,
//		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
//		GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//		GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
//		GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
//		GL_CLAMP_TO_EDGE);
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//		m_fboDepth, 0);
//	glDrawBuffer(GL_NONE);
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if (status != GL_FRAMEBUFFER_COMPLETE)
//		printf("Framebuffer Error!\n");
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	m_lightDirection = glm::normalize(glm::vec3(1, 2.5f, 1));
//	glm::mat4 lightProjection = glm::ortho<float>(-10, 10,
//		-10, 10, -10, 10);
//	glm::mat4 lightView = glm::lookAt(m_lightDirection,
//		glm::vec3(0), glm::vec3(0, 1, 0));
//	glm::mat4 textureSpaceOffset(
//		0.5f, 0.0f, 0.0f, 0.0f,
//		0.0f, 0.5f, 0.0f, 0.0f,
//		0.0f, 0.0f, 0.5f, 0.0f,
//		0.5f, 0.5f, 0.5f, 1.0f
//		);
//	m_lightMatrix = lightProjection * lightView;
//	return true;
//}
//
//void Shadows::shutdown()
//{
//	Gizmos::destroy();
//	cleanupOpenGLBuffers(m_fbx);
//	glDeleteProgram(m_useShadowProgram);
//	Application::shutdown();
//}
//
//
//bool Shadows::update()
//{
//	if (Application::update() == false)
//	{
//		return false;
//	}
//
//	float dt = (float)glfwGetTime();
//	glfwSetTime(0.0);
//
//	Gizmos::clear();
//	timer += dt;
//
//	vec4 white(1);
//	vec4 black(0, 0, 0, 1);
//	for (int i = 0; i <= 20; ++i)
//	{
//		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
//			i == 10 ? white : black);
//		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
//			i == 10 ? white : black);
//	}
//	camera.update(dt);
//
//
//	return true;
//}
//
//void Shadows::draw()
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//	glViewport(0, 0, 1024, 1024);
//	glClear(GL_DEPTH_BUFFER_BIT);
//	glUseProgram(m_shadowGenProgram);
//	int loc = glGetUniformLocation(m_shadowGenProgram, "LightMatrix");
//	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_lightMatrix[0][0]));
//	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
//	{
//		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
//		unsigned int* glData = (unsigned int*)mesh->m_userData;
//		glBindVertexArray(glData[0]);
//		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
//	}
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, 1280, 720);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glUseProgram(m_useShadowProgram);
//	loc = glGetUniformLocation(m_useShadowProgram, "ProjectionView");
//	glUniformMatrix4fv(loc, 1, GL_FALSE,
//		&(camera.view_proj[0][0]));
//	glm::mat4 textureSpaceOffset(
//		0.5f, 0.0f, 0.0f, 0.0f,
//		0.0f, 0.5f, 0.0f, 0.0f,
//		0.0f, 0.0f, 0.5f, 0.0f,
//		0.5f, 0.5f, 0.5f, 1.0f
//		);
//	glm::mat4 lightMatrix = textureSpaceOffset * m_lightMatrix;
//	loc = glGetUniformLocation(m_useShadowProgram, "LightMatrix");
//	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);
//	loc = glGetUniformLocation(m_useShadowProgram, "lightDir");
//	glUniform3fv(loc, 1, &m_lightDirection[0]);
//	loc = glGetUniformLocation(m_useShadowProgram, "shadowMap");
//	glUniform1i(loc, 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_fboDepth);
//	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
//	{
//		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
//		unsigned int* glData = (unsigned int*)mesh->m_userData;
//		glBindVertexArray(glData[0]);
//		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
//	}
//	glBindVertexArray(m_vao);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//	Gizmos::draw(camera.view_proj);
//	glfwSwapBuffers(m_window);
//	glfwPollEvents();
//}