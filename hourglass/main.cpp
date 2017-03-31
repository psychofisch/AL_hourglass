#include <iostream>
#include <omp.h>

#include <SFML\Graphics.hpp>

void main(int argc, char* argv[])
{
	sf::VideoMode vm;
	vm.width = 720;
	vm.height = 720;
	vm.bitsPerPixel = 32;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 2;

	sf::RenderWindow window(vm, "Hourglass Simulation", sf::Style::Titlebar | sf::Style::Close | sf::Style::Default, settings);
	window.setFramerateLimit(60);
}
