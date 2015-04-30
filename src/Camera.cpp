#include "Camera.h"
#include <iostream>

#include "GLFW/glfw3.h"

vec3 Camera::pickAgainstPlane(float x, float y, vec4 plane)
{
	float nxPos = x / 1280;
	float nyPos = x / 720;

	float sxPos = nxPos - 0.5;
	float syPos = nyPos - 0.5;

	float fxPos = sxPos * 2;
	float fyPos = syPos * -2;

	mat4 inv_viewProj = glm::inverse(view_proj);

	vec4 mouse_pos(fxPos, fyPos, 1, 1);
	vec4 world_pos = inv_viewProj * mouse_pos;

	world_pos /= world_pos.w;

	vec3 cam_pos = world[3].xyz();
	vec3 dir = world_pos.xyz() - cam_pos;

	float t = -(glm::dot(cam_pos, plane.xyz()) + plane.w) / (glm::dot(dir, plane.xyz()));
	
	vec3 result = cam_pos + dir * t;

	return result;
}

Camera::Camera(float aspect)
{
	view = glm::lookAt(vec3(0,0,0), vec3(0,0,1), vec3(0, 1, 0));
	world = glm::inverse(view);
	proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
	view_proj = proj * view;
}

void Camera::updateViewProj()
{
	view_proj = proj * view;
}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
	proj = glm::perspective(fov, aspect, near, far);
	updateViewProj();
}

void Camera::setLookAt(vec3 pos, vec3 center, vec3 up)
{
	view = glm::lookAt(pos, center, up);
	world = glm::inverse(view);
	updateViewProj();
}

void Camera::setPosition(vec3 pos)
{
	world[3] = vec4(pos, 1);
	view = glm::inverse(world);
	updateViewProj();
}


FlyCamera::FlyCamera(float aspect, float new_speed) : Camera(aspect)
{
	this->speed = new_speed;
}

void FlyCamera::update(float dt)
{
	GLFWwindow* curr_window = glfwGetCurrentContext();

	if (glfwGetKey(curr_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		world[3] -= world[2] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		world[3] += world[2] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		world[3] -= world[0] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		world[3] += world[0] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		world[3] -= world[1] * speed * dt;
	}
	if (glfwGetKey(curr_window, GLFW_KEY_E) == GLFW_PRESS)
	{
		world[3] += world[1] * speed * dt;
	}
	

	if (glfwGetMouseButton(curr_window, 1))
	{
		double x_delta, y_delta;
		glfwGetCursorPos(curr_window, &x_delta, &y_delta);
		//sets to centre
		glfwSetCursorPos(curr_window, 1280.f / 2.f, 720.f / 2.f);

		x_delta -= (1280.f / 2.f);
		y_delta -= (720.f / 2.f);

		x_delta /= (1280.f / 2.f);
		y_delta /= (720.f / 2.f);

		x_delta *= -sensitivity;
		y_delta *= -sensitivity;

		vec3 camera_right = (vec3)world[0];

		mat4 yaw = glm::rotate((float)x_delta, vec3(0, 1, 0));
		mat4 pitch = glm::rotate((float)y_delta, camera_right);
		mat4 rot = yaw * pitch;

		world[0] = rot * world[0];
		world[1] = rot * world[1];
		world[2] = rot * world[2];
	}
	view = glm::inverse(world);
	updateViewProj();
}
