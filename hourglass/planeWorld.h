#pragma once

#include <iostream>
#include <vector>

#include <SFML\Graphics.hpp>

class Pixel;

typedef std::vector<std::vector<Pixel>> vec2Di;

class Pixel : public sf::RectangleShape
{
public:
	int value;
};

class planeWorld
{
public:
	planeWorld();
	~planeWorld();

	void run();
	void setRenderWindow(sf::RenderWindow* wndw);
	bool setWorldDimensions(int size); //true = ERROR; false = successful
	bool setWorldDimensions(int size_x, int size_y);

	float tick,
		gap,
		pixelSize;
	bool stepMode;

private:
	vec2Di m_grid;
	sf::RenderWindow* m_window;
	sf::View m_view;
	bool m_step;
	sf::RectangleShape m_pixel;
};

