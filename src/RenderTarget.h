#ifndef RENDERTARGET_H_
#define RENDERTARGET_H_
#include "Application.h"
#include "Camera.h"

class RenderTarget : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	FlyCamera camera;
	FlyCamera reflection_camera;
	vec4 m_background_color;

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_program;

	vec3 m_positions[2];
	glm::quat m_rotations[2];
	float timer = 0.0f;
};

#endif //RENDERTARGET_H_