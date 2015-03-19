#ifndef POSTPROCESSING_H_
#define POSTPROCESSING_H_
#include "Application.h"
#include "Camera.h"

class PostProcessing : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	unsigned int loadShader(unsigned int type, const char* path);
	void createDrawShader();

	FlyCamera camera;
	vec4 m_background_color;
	float timer = 0.0f;

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_program;
	unsigned int m_drawShader;
};

#endif //POSTPROCESSING_H_