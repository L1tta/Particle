#ifndef QUATERNIONS_H_
#define QUATERNIONS_H_
#include "Application.h"
#include "Camera.h"

struct Keyframe
{
	vec3 position;
	glm::quat rotation;
};

class Quaternions : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	Keyframe m_hip_frames[2];
	Keyframe m_knee_frames[2];
	Keyframe m_ankle_frames[2];

	FlyCamera camera;

	mat4 hip_bone;
	mat4 knee_bone;
	mat4 ankle_bone;

	vec3 m_positions[2];
	glm::quat rotations[2];

	float timer;

};

#endif //QUATERNIONS_H_