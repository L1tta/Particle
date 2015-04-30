#ifndef C4_H_
#define C4_H_
#include "Application.h"
#include "Camera.h"
#include <vector>
#include "tiny_obj_loader.h"
#include "FBXFile.h"
#include "ConnectFour.h"
#include "AIBase.h"
class C4 : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	FlyCamera camera;
	vec4 m_background_color;
	float timer = 0.0f;

	// the game and A.I. components
	Game* m_game;
	MCTS* m_ai;
};

#endif //C4_H_