#ifndef CAM_PROJ_H_
#define CAM_PROJ_H_

#include "Application.h"
#include "Camera.h"


class CameraAndProjections : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	unsigned int rows;
	unsigned int cols;

	void generateGrid(unsigned int rows, unsigned int cols);

	// our vertex and index buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_programID;
	unsigned int m_indexCount;

	FlyCamera camera;
};

#endif //CAM_PROJ_H_