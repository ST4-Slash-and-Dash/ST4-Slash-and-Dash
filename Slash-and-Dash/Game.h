#pragma once
#include"SFML/Graphics.hpp"
#include "Menu.h"
#include "Player.h"
#include "World.h"
#include "SFML/Audio.hpp"
#include "Endscreen.h"
#include "Debug_Menu.h"

class Player;

class Game {
private:
	sf::RenderWindow* window;
	bool fullscreen;
	sf::Event e;

	sf::Music mBg;

	sf::SoundBuffer die_buffer;
	sf::Sound die;

	enum State {
		inMainMenu,
		inGameMenu,
		Playing
	};
	State state;

	sf::View gameView;
	float viewWidth;
	float viewHeight;


	Menu* menu;
	World* world;
	Player* player;
	Player* player2;
	Endscreen* endscreen;
	Debug_Menu* debug_menu;
	bool enable_debug_menu = false;
	int points[2] = {};
	bool toMainMenu;
	bool player1_won;
	bool bool_start_winning_screen;

	void init();
	void initWinow();
	void initVars();
	void initPlayer();
	void initEndscreen();

	void update(sf::Time deltaTime);
	void updateMenu();
	void updateView();
	void updatePlayer(sf::Time deltaTime);
	void updatePollEvents();

	bool alive;
	bool countPoints;

public:
	void start_game();
	void start_Round();
	void end_game();

	Game();
	virtual ~Game();
	void run();
	void render();
};