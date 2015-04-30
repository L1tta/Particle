#pragma once

#include <vector>
#include <stdlib.h>
#include <ctime>
// base class to define a zero-sum game with two opponents
// actions are defined as int's
class Game {
public:

	enum State {
		UNKNOWN,
		PLAYER_ONE,
		PLAYER_TWO,
		DRAW,
	};

	Game() :
		m_currentPlayer(PLAYER_ONE),
		m_currentOpponent(PLAYER_TWO) {

	}
	virtual ~Game() {}

	// returns wether someone is winning or not
	virtual State	getCurrentGameState() const = 0;

	// these switch whenever an action is made
	State			getCurrentPlayer() const	{ return m_currentPlayer;	}
	State			getCurrentOpponent() const	{ return m_currentOpponent; }

	// get a list of valid actions for current player
	virtual void	getValidActions(std::vector<int>& actions) const = 0;
	virtual bool	isActionValid(int action) const = 0;

	// performs an action for the current player, and switches current player
	virtual void	performAction(int action) = 0;

	// draw the game
	virtual void	draw() const = 0;

	// clones the current game state
	virtual Game*	clone() const = 0;

protected:

	State	m_currentPlayer;
	State	m_currentOpponent;
};

// base class for an A.I. opponent
class AI {
public:

	AI() {}
	virtual ~AI() {}

	// makes a decision for the current game
	virtual int	makeDecision(const Game& game) = 0;
};

// an A.I. opponent that chooses purely random actions to perform
class RandomAI : public AI {
public:

	RandomAI() {}
	virtual ~RandomAI() {}

	// randomly choose an action to perform
	virtual int	makeDecision(const Game& game) {
		std::vector<int> actions;
		game.getValidActions(actions);
		if (actions.size() > 0)
			return actions[rand() % actions.size()];
		else
			return -1;
	}
};

class MCTS : public AI {
public:
	MCTS(int playouts) :
		m_playouts(playouts) {
	}
	int StartValue;
	virtual ~MCTS() {}
	virtual int makeDecision(const Game& game)
	{
		std::vector<int> actions;
		std::vector<int> actionScore;
		game.getValidActions(actions);


		for (unsigned int i = 0; i < actions.size(); i++)
		{
			StartValue = 0;
			for (int k = 0; k < m_playouts; k++)
			{
				Game* Tgame = game.clone();
				Tgame->performAction(i);
				while (Tgame->getCurrentGameState() == 0)
				{
					std::vector<int> Cactions;
					Tgame->getValidActions(Cactions);
					Tgame->performAction(Cactions[rand() % Cactions.size()]);
				}

				if (Tgame->getCurrentGameState() == 2)
				{
					StartValue += 1;
				}

				delete Tgame;
			}
			actionScore.push_back(StartValue);
		}

		int ScoreT = 0;
		int index = 0;

		for (unsigned int j = 0; j < actionScore.size(); j++)
		{
			if (ScoreT < actionScore[j])
			{
				ScoreT = actionScore[j];
				index = j;
			}
		}
		
		return actions[index];
	}
private:
	// the number of times it will simulate a game
	int m_playouts;
};