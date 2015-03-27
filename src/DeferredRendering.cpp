#include "DeferredRendering.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"

unsigned int DeferredRendering::loadShader(unsigned int type, const char* path) {
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

void DeferredRendering::createDrawShader()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER,
		"GBuffer.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER,
		"GBuffer.frag");
	m_gpassShader = glCreateProgram();
	glAttachShader(m_gpassShader, vs);
	glAttachShader(m_gpassShader, fs);
	glLinkProgram(m_gpassShader);
	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);

	unsigned int dfs = loadShader(GL_FRAGMENT_SHADER,
		"DirLight.frag");
	m_directionLightShader = glCreateProgram();
	glAttachShader(m_directionLightShader, fs);
	glLinkProgram(m_directionLightShader);
	// remove unneeded handles
	glDeleteShader(dfs);

	unsigned int cvs = loadShader(GL_VERTEX_SHADER,
		"Composite.vert");
	unsigned int cfs = loadShader(GL_FRAGMENT_SHADER,
		"Composite.frag");
	m_compositeShader = glCreateProgram();
	glAttachShader(m_compositeShader, cvs);
	glAttachShader(m_compositeShader, cfs);
	glLinkProgram(m_compositeShader);
	// remove unneeded handles
	glDeleteShader(cvs);
	glDeleteShader(cfs);
}

void DeferredRendering::drawDirectionalLight(const glm::vec3& direction,
	const glm::vec3& diffuse) {
	glm::vec4 viewSpaceLight = camera.view *
		glm::vec4(glm::normalize(direction), 0);
	int loc = glGetUniformLocation(m_directionLightShader,
		"lightDirection");
	glUniform3fv(loc, 1, &viewSpaceLight[0]);
	loc = glGetUniformLocation(m_directionLightShader,
		"lightDiffuse");
	glUniform3fv(loc, 1, &diffuse[0]);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void DeferredRendering::createOpenGLBuffers(FBXFile* fbx)
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
}

bool DeferredRendering::startup()
{
	m_background_color.r = 0.3f;
	m_background_color.g = 0.3f;
	m_background_color.b = 0.3f;
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

	// setup gpass framebuffer
	glGenFramebuffers(1, &m_gpassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glGenTextures(1, &m_albedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glGenTextures(1, &m_positionTexture);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glGenTextures(1, &m_normalTexture);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glGenRenderbuffers(1, &m_gpassDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gpassDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
		1280, 720);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		m_albedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		m_positionTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
		m_normalTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, m_gpassDepth);
	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, gpassTargets);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// setup light framebuffer
	glGenFramebuffers(1, &m_lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glGenTextures(1, &m_lightTexture);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, m_lightTexture, 0);
	GLenum lightTargets[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, lightTargets);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void DeferredRendering::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}


bool DeferredRendering::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();
	timer += dt;
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		glDeleteProgram(m_gpassShader);
		glDeleteProgram(m_compositeShader);
		glDeleteProgram(m_directionLightShader);
		createDrawShader();
	}

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

void DeferredRendering::draw()
{
	// G-Pass: render out the albedo, position and normal
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_gpassShader);
	// bind camera transforms
	int loc = glGetUniformLocation(m_gpassShader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(camera.view_proj[0][0]));
	loc = glGetUniformLocation(m_gpassShader, "View");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(camera.view[0][0]));
	// draw our scene, in this example just the Stanford Bunny
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(),
			GL_UNSIGNED_INT, 0);
	}

	// Light Pass: render lights as geometry, sampling position and
	// normals disable depth testing and enable additive blending
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glUseProgram(m_directionLightShader);
	loc = glGetUniformLocation(m_directionLightShader,
		"positionTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	loc = glGetUniformLocation(m_directionLightShader,
		"normalTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	// draw lights as fullscreen quads
	drawDirectionalLight(glm::vec3(-1), glm::vec3(1));
	glDisable(GL_BLEND);

	// Composite Pass: render a quad and combine albedo and light
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_compositeShader);
	loc = glGetUniformLocation(m_compositeShader, "albedoTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	loc = glGetUniformLocation(m_compositeShader, "lightTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	Gizmos::draw(camera.view_proj);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}