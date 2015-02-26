#include "Quaternions.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"
#include "Vertex.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


bool Quaternions::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(0.3f,0.3f,0.3f,1.0f);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	glm::quat boring_quaternion(1, 0, 0, 0);
	glm::quat euler_quat(vec3(3, 5, 7));
	glm::quat mixed_quat = glm::slerp(boring_quaternion, euler_quat, 0.8f);

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	//m_positions[0] = vec3(10, 5, 10);
	//m_positions[1] = vec3(-10, 0, -10);

	//m_rotations[0] = glm::quat(vec3(0, -1, 0));
	//m_rotations[1] = glm::quat(vec3(0, 1, 0));

	m_hip_frames[0].position = vec3(0, 5, 0);
	m_hip_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_knee_frames[0].position = vec3(0, -2.5, 0);
	m_knee_frames[0].rotation = glm::quat(vec3(-1, 0, 0));

	m_ankle_frames[0].position = vec3(0, -2.5, 0);
	m_ankle_frames[0].rotation = glm::quat(vec3(0, 0, 0));

	m_hip_frames[1].position = vec3(0, 5, 0);
	m_hip_frames[1].rotation = glm::quat(vec3(1, 0, 0));

	m_knee_frames[1].position = vec3(0, -2.5, 0);
	m_knee_frames[1].rotation = glm::quat(vec3(0, 0, 0));

	m_ankle_frames[1].position = vec3(0, -2.5, 0);
	m_ankle_frames[1].rotation = glm::quat(vec3(0, 0, 0));

	return true;
}

mat4 EvaluateKeyframes(Keyframe start, Keyframe end, float t)
{
	vec3 pos = glm::mix(start.position, end.position, t);
	glm::quat rot = glm::slerp(start.rotation, end.rotation, t);
	mat4 result = glm::translate(pos) * glm::toMat4(rot);
	return result;
}

void Quaternions::shutdown()
{
	Gizmos::destroy();

	Application::shutdown();
}


bool Quaternions::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();
	timer += dt;

	float sin_wave = sinf(timer) * 0.5f + 0.5f;

	//vec3 final_pos = glm::mix(m_positions[0], m_positions[1], sin_wave);
	//glm::quat final_rot = glm::slerp(m_rotations[0], m_rotations[1], sin_wave);

	//glm::mat4 transform = glm::translate(final_pos) * glm::toMat4(final_rot);

	//Gizmos::addTransform(transform);
	//Gizmos::addAABBFilled(final_pos, vec3(1), vec4(0, 0, 1, 1), &transform);
	hip_bone = EvaluateKeyframes(m_hip_frames[0], m_hip_frames[1], sin_wave);
	knee_bone = EvaluateKeyframes(m_knee_frames[0], m_knee_frames[1], sin_wave);
	ankle_bone = EvaluateKeyframes(m_ankle_frames[0], m_ankle_frames[1], sin_wave);

	mat4 global_hip = hip_bone;
	mat4 global_knee = hip_bone * knee_bone;
	mat4 global_ankle = global_knee * ankle_bone;

	vec3 hip_pos = global_hip[3].xyz;
	vec3 knee_pos = global_knee[3].xyz;
	vec3 ankle_pos = global_ankle[3].xyz;

	Gizmos::addAABBFilled(hip_pos, vec3(0.5f), vec4(1, 1, 0, 1), &global_hip);
	Gizmos::addAABBFilled(knee_pos, vec3(0.5f), vec4(1, 0, 1, 1), &global_knee);
	Gizmos::addAABBFilled(ankle_pos, vec3(0.5f), vec4(0, 1, 1, 1), &global_ankle);

	Gizmos::addLine(hip_pos, knee_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(knee_pos, ankle_pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));

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

void Quaternions::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	Gizmos::draw(camera.proj, camera.view);


	glfwSwapBuffers(m_window);
	glfwPollEvents();
}