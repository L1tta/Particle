#include "GUI.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"


void On_mouse_button(GLFWwindow* window, int button, int pressed, int alt_keys)
{
	TwEventMouseButtonGLFW(button, pressed);
}
void On_mouse_position(GLFWwindow* window, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}
void On_mouse_scroll(GLFWwindow* window, double x, double y)
{
	TwEventMouseWheelGLFW((int)y);
}
void On_key(GLFWwindow* window, int key, int scancode, int pressed, int mod_keys)
{
	TwEventKeyGLFW(key, pressed);
}
void On_char(GLFWwindow* window, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}
void On_window_resize(GLFWwindow* window, int width, int height)
{
	TwWindowSize(width, height);
	glViewport(0, 0, width, height);
}


bool GUI::startup()
{
	m_draw_gizmos = true;
	m_background_color.r = 0.3f;
	m_background_color.g = 0.3f;
	m_background_color.b = 0.3f;
	m_background_color.a = 1.0f;

	if (Application::startup() == false)
	{
		return false;
	}
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	glfwSetMouseButtonCallback(m_window, On_mouse_button);
	glfwSetCursorPosCallback(m_window, On_mouse_position);
	glfwSetScrollCallback(m_window, On_mouse_scroll);
	glfwSetKeyCallback(m_window, On_key);
	glfwSetCharCallback(m_window, On_char);
	glfwSetWindowSizeCallback(m_window, On_window_resize);

	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	camera.sensitivity = 3;
	TwBar* light_bar;
	m_bar = TwNewBar("GUI");

	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_background_color, "");
	//you havent done the light tutorial
	//TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "group=light");
	//TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_color, "group=light");
	//TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_specular_power, "group=light min=0.01 max=100 step=0.05");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_draw_gizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");

	return true;
}

void GUI::shutdown()
{
	Gizmos::destroy();

	TwDeleteAllBars();
	TwTerminate();

	Application::shutdown();
}


bool GUI::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_fps = 1/dt;

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

void GUI::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);

	if (m_draw_gizmos == true)
	{
		Gizmos::draw(camera.proj, camera.view);
	}

	TwDraw();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}