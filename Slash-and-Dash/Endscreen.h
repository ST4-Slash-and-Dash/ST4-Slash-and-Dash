#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>


using namespace std;

class Endscreen {
private:
	sf::RectangleShape shape;
	sf::Text text;
	sf::Font font;
	float wait;
	float duration;
	bool render_screen;

public:
	sf::Clock endscreenTime;

	Endscreen();
	void update();
	void endscreen_start(float wait, float duration);
	~Endscreen() {};
	void render(sf::RenderWindow* target);

};
