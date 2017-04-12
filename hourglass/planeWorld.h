#pragma once

#define NOMINMAX

#include <iostream>
#include <vector>
#include <sstream>
#include <omp.h>
#include <fstream>

#include <CL/cl.hpp>

#include <SFML\Graphics.hpp>

#include "RNGesus.h"

struct OpenCLData {
	cl::Context context;
	cl::Program program;
	cl::Device device;
	cl::Kernel kernel;
};

class planeWorld
{
public:
	enum Rotation {
		ROTATE_LEFT = 0,
		ROTATE_RIGHT
	};

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
	void rotate(Rotation r);
	void setNumberOfThreads(unsigned int t);

	std::string cl_errorstring(cl_int err);
	void handle_clerror(cl_int err);

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
	unsigned int m_numberOfThreads;
	bool m_debug;

	OpenCLData m_OpenCLData;

	int i_manhattanDistance(sf::Vector2i a, sf::Vector2i b);
	void i_createHourglass();
	void i_physicRules(sf::Color * fields);
	sf::Image* i_getOtherPointer();
	void i_updateGridCPU(int init);
	void i_initOpenCL(unsigned int platformId, unsigned int deviceId);
	void i_updateGridGPU(int init);
};
