#ifndef PARTICLE_H_
#define PARTICLE_H_
#include "Application.h"
#include "Camera.h"
#include "ParticleEmitter.h"

class Particles : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	ParticleEmitter* m_emitter[4];

	unsigned int m_programID;

	FlyCamera camera;
};


#endif //PARTICLE_H_