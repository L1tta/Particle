#include "GPUParticle.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"

unsigned int GPUParticle::loadShader(unsigned int type, const char* path) {
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

bool GPUParticle::startup()
{

	if (Application::startup() == false)
	{
		return false;
	}

	time = 0;

	glClearColor(0.3f, 0.3f, 0.3f, 1);
	glEnable(GL_DEPTH_TEST);

	m_emitter = new GPUParticleEmitter();
	m_emitter->initalise(100000, 5.0f, 20.0f, 10, 40, 0.1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));

	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	return true;
}

void GPUParticle::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}


bool GPUParticle::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	time += dt;

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

void GPUParticle::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(camera.proj, camera.view);
	m_emitter->draw(time, camera.world, camera.view_proj);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}