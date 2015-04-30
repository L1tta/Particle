#include "C4.h"
#include "GLM_Header.h"
#include "GL_Header.h"
#include "Gizmos.h"
#include "stb_image.h"

bool C4::startup()
{
	m_background_color.r = 0.75f;
	m_background_color.g = 0.75f;
	m_background_color.b = 0.75f;
	m_background_color.a = 1.0f;

	if (Application::startup() == false)
	{
		return false;
	}

	glClearColor(m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
	glEnable(GL_DEPTH_TEST);
	Gizmos::create();

	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	camera.setLookAt(vec3(-10, 10, 0), vec3(0, 0, 0), vec3(0, 1, 0));
	camera.sensitivity = 3;

	// create a game of Connect Four
	m_game = new ConnectFour();
	//// create an A.I., just a random one for now
	//m_ai = new RandomAI();	// diffuicult opponent
	m_ai = new MCTS(1000);
	return true;
}

void C4::shutdown()
{
	delete m_ai;
	delete m_game;
	Gizmos::destroy();
	Application::shutdown();
}


bool C4::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	Gizmos::clear();
	timer += dt;

	//vec4 white(1);
	//vec4 black(0, 0, 0, 1);
	//for (int i = 0; i <= 20; ++i)
	//{
	//	Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
	//		i == 10 ? white : black);
	//	Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
	//		i == 10 ? white : black);
	//}
	camera.update(dt);

	// kepp track of if the mouse has been clicked
	static bool moveMade = false;
	// if the game isn't over...
	if (m_game->getCurrentGameState() == Game::UNKNOWN) {
		// if it is the user's turn (player 1)
		if (m_game->getCurrentPlayer() == Game::PLAYER_ONE) {
			// if the mouse button is down...
			if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS) {
				// if the mouse JUST went down then make a choice
				if (moveMade == false) {
					moveMade = true;
					// get the mouse position within the game grid
					double x = 0, y = 0;
					glfwGetCursorPos(m_window, &x, &y);
					vec3 m_pickPosition = camera.pickAgainstPlane(
						(float)x, (float)y, glm::vec4(0, 1, 0, 0));
					// determine which column was chosen
					int column = (int)((m_pickPosition.z +
						ConnectFour::COLUMNS) / 2);
					// if we clicked in the area for a
					// valid column then perform that action
					if (m_game->isActionValid(column))
						m_game->performAction(column);
				}
			}
			else
				moveMade = false;
		}
		else {
			// it is the opponent's turn (player 2)
			// use the A.I. to make a decision
			m_game->performAction(m_ai->makeDecision(*m_game));
		}
	}


	return true;
}

void C4::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw the game
	m_game->draw();
	// display the 3D gizmos
	Gizmos::draw(camera.view_proj);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}