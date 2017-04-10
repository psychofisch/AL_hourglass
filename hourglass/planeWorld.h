#pragma once

#include <iostream>
#include <vector>
#include <sstream>

#include <SFML\Graphics.hpp>

#include "RNGesus.h"

class Pixel;

typedef std::vector<std::vector<Pixel>> vec2Di;

class planeWorld
{
public:
	planeWorld();
	~planeWorld();

	void run();
	void setRenderWindow(sf::RenderWindow* wndw);
	bool setWorldDimensions(int size); //true = ERROR; false = successful
	bool setWorldDimensions(int size_x, int size_y);
	void updateGrid();
	void toggleGridBuffer();

	float tick,
		gap,
		pixelSize;
	bool stepMode;

private:
	int* m_grid;
	sf::Image* m_gridImagePtr;
	sf::Image m_gridImage1, m_gridImage2;
	sf::Vector2i m_dimension;
	sf::RenderWindow* m_window;
	sf::View m_view;
	bool m_step, m_margo;
	sf::RectangleShape m_pixel;
	RNGesus* m_rng;
};

