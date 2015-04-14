#ifndef _ASSESMENT_H_
#define _ASSESMENT_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex2.h"

class Assesment : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	void buildGrid(vec2 realDim, glm::ivec2 dims);
	void buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance);


	OpenGLData m_planeMesh;
	unsigned int m_programID;
	unsigned int m_texture;
	unsigned int m_perlinTexture;
	float* m_perlinData;
	FlyCamera m_Camera;
	float dims;
	float size;
	int octaves;
	float persistance;
};



#endif