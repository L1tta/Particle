#ifndef GPUPARTICLE_H_
#define GPUPARTICLE_H_
#include "Application.h"
#include "Camera.h"
#include "GPUParticleEmitter.h"

class GPUParticle : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	float time;

	unsigned int loadShader(unsigned int type, const char* path);
	unsigned int m_programID;

	GPUParticleEmitter* m_emitter;

	FlyCamera camera;
};


#endif //GPUPARTICLE_H_