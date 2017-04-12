#include <iostream>
#include <omp.h>

#include <SFML\Graphics.hpp>

#include "planeWorld.h"

#define DEBUG(x) std::cout << #x << ": " << x << std::endl;

void main(int argc, char* argv[])
{
	unsigned int threads = 1, platformId = 0, deviceId = 0;
	planeWorld::MT_MODE mode = planeWorld::MT_CPU;
	bool debug = false;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--mode") == 0)
		{
			++i;
			if (strcmp(argv[i], "cpu") == 0)
				mode = planeWorld::MT_CPU;
			else if (strcmp(argv[i], "gpu") == 0)
				mode = planeWorld::MT_GPU;
		}
		else if (strcmp(argv[i], "--threads") == 0)
		{
			threads = atoi(argv[++i]);
			if (debug)
				DEBUG(threads);
		}
		else if (strcmp(argv[i], "--platformId") == 0)
		{
			platformId = atoi(argv[++i]);
			if (debug)
				DEBUG(platformId);
		}
		else if (strcmp(argv[i], "--deviceId") == 0)
		{
			deviceId = atoi(argv[++i]);
			if (debug)
				DEBUG(deviceId);
		}
		else if (strcmp(argv[i], "--debug") == 0)
		{
			debug = true;
			std::cout << "DEBUG MODE ACTIVATED" << std::endl;
		}
	}

	sf::VideoMode vm;
	vm.width = 720;
	vm.height = 720;
	vm.bitsPerPixel = 32;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;

	sf::RenderWindow window(vm, "Hourglass Simulation", sf::Style::Titlebar | sf::Style::Close | sf::Style::Default, settings);
	window.setFramerateLimit(60);

	planeWorld pw;
	pw.tick = .016f;
	pw.stepMode = false;

	pw.setRenderWindow(&window);
	pw.setWorldDimensions(300, 1000);
	pw.setNumberOfThreads(threads);
	pw.setMutlithreadingMode(mode);
	pw.setOpenCLPlatformAndDevice(platformId, deviceId);
	pw.setDebugMode(debug);
	pw.run();
}
