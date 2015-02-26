#ifndef TEXTURE_H_
#define TEXTURE_H_
#include "Application.h"
#include "Camera.h"

class Texture : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	unsigned int m_texture;
	unsigned int m_programID;
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;

	FlyCamera camera;
};

#endif //TEXTURE_H_