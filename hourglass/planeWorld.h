#pragma once

#include <iostream>
#include <vector>
#include <sstream>

#include <SFML\Graphics.hpp>

#include "RNGesus.h"

class Pixel;

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
	void draw(sf::Vector2u pos, sf::Color color);
	void setBrushSize(int size);

	float tick,
		gap,
		pixelSize;
	bool stepMode;

private:
	int* m_grid;
	sf::Image* m_gridImagePtr;
	sf::Image m_gridImage1, m_gridImage2;
	sf::Vector2u m_dimension;
	sf::RenderWindow* m_window;
	sf::View m_view;
	bool m_step, m_margo;
	sf::RectangleShape m_pixel;
	RNGesus* m_rng;
	unsigned int m_brushSize;
	sf::CircleShape m_brushCircle;

	int i_manhattanDistance(sf::Vector2i a, sf::Vector2i b);
};

