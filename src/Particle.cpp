#include "Particle.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"

bool Particles::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	const char* vsSource = "#version 410\n \
						   in vec4 Position; \
						   in vec4 Colour; \
						   out vec4 colour; \
						   uniform mat4 projectionView; \
						   void main() { \
						   colour = Colour; \
						   gl_Position = projectionView * Position;}";
	const char* fsSource = "#version 410\n \
						   in vec4 colour; \
						   void main() { \
						   gl_FragColor = colour;}";
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs, 1, &vsSource, nullptr);
	glShaderSource(fs, 1, &fsSource, nullptr);
	glCompileShader(fs);
	glCompileShader(vs);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vs);
	glAttachShader(m_programID, fs);
	glLinkProgram(m_programID);
	glDeleteShader(fs);
	glDeleteShader(vs);


	glClearColor(0.1f,0.1f,0.1f,1.0f);
	glEnable(GL_DEPTH_TEST);


	m_emitter[0] = new ParticleEmitter(10.0f,12.0f,-10.0f);
	m_emitter[0]->initalise(1000, 500, 0.1f, 5.0f, 1, 6, 0.2f, 0.1f, glm::vec4(1.0f, 1.0f, 1.0f, 1), glm::vec4(1, 1, 1, 1));
	m_emitter[1] = new ParticleEmitter(-10.0f, 12.0f, 10.0f);
	m_emitter[1]->initalise(1000, 500, 0.1f, 5.0f, 1, 6, 0.2f, 0.1f, glm::vec4(1.0f, 1.0f, 1.0f, 1), glm::vec4(1, 1, 1, 1));
	m_emitter[2] = new ParticleEmitter(10.0f, 12.0f, 10.0f);
	m_emitter[2]->initalise(1000, 500, 0.1f, 5.0f, 1, 6, 0.2f, 0.1f, glm::vec4(1.0f, 1.0f, 1.0f, 1), glm::vec4(1, 1, 1, 1));
	m_emitter[3] = new ParticleEmitter(-10.0f, 12.0f, -10.0f);
	m_emitter[3]->initalise(1000, 500, 0.1f, 5.0f, 1, 6, 0.2f, 0.1f, glm::vec4(1.0f, 1.0f, 1.0f, 1), glm::vec4(1, 1, 1, 1));
	//m_emitter[1] = new ParticleEmitter(1.0f,0.0f,-1.0f);
	//m_emitter[1]->initalise(1000, 500, 0.1f, 5.0f, 1, 5, 1, 0.1f, glm::vec4(0.0f, 1.0f, 0.0f, 1), glm::vec4(1, 1, 0, 1));
	//m_emitter[2] = new ParticleEmitter(-1.0f, 0.0f, 1.0f);
	//m_emitter[2]->initalise(1000, 500, 0.1f, 5.0f, 1, 5, 1, 0.1f, glm::vec4(0.0f, 0.0f, 1.0f, 1), glm::vec4(1, 0, 1, 1));
	//m_emitter[3] = new ParticleEmitter(-1.0f, 0.0f, -1.0f);
	//m_emitter[3]->initalise(1000, 500, 0.1f, 5.0f, 1, 5, 1, 0.1f, glm::vec4(1.0f, 0.0f, 0.0f, 1), glm::vec4(1, 0, 1, 1));

	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	return true;
}

void Particles::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}


bool Particles::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	glm::vec3 move(1,0,0);
	for (int i = 0; i < 4; i++)
	{
		m_emitter[i]->update(dt, camera.world);
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

void Particles::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_programID);
	int loc = glGetUniformLocation(m_programID, "projectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&camera.view_proj);
	for (int i = 0; i < 4; i++)
	{
		m_emitter[i]->draw();
	}

	Gizmos::draw(camera.proj, camera.view);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}