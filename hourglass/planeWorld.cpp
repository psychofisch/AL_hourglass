#include "planeWorld.h"



planeWorld::planeWorld()
	:tick(0.1f),
	stepMode(false),
	gap(1.f),
	pixelSize(1.f)
{
	m_pixel.setFillColor(sf::Color::Green);
	m_pixel.setSize(sf::Vector2f(pixelSize, pixelSize));
}

planeWorld::~planeWorld()
{
	delete[] m_grid;
}

void planeWorld::run()
{
	int mouseMoveRectSize = 200;
	sf::IntRect mouseMoveRect = sf::IntRect((m_window->getSize().x - mouseMoveRectSize) / 2, (m_window->getSize().x - mouseMoveRectSize) / 2, mouseMoveRectSize, mouseMoveRectSize);
	sf::Vector2f windowCenter(m_window->getSize());
	windowCenter /= 2.0f;

	sf::Clock time;
	float dt = 0.16f;
	float tickRun = tick;
	int currentTile = -1;

	//DEBUG
	sf::Font debug_font;
	debug_font.loadFromFile("./NotoSans-Regular.ttf");
	sf::Text debug_text;
	debug_text.setFont(debug_font);
	debug_text.setCharacterSize(32);
	//debug_text.setScale(sf::Vector2f(1.f, 1.f));
	debug_text.setOutlineColor(sf::Color::Black);
	debug_text.setOutlineThickness(1.f);
	debug_text.setPosition(50.f, 50.f);
	bool measure = true,
		pause = false;

	std::stringstream debugString;
	//***d

	sf::Texture gridTexture;
	sf::Sprite gridSprite;

	gridTexture.create(m_dimension.x, m_dimension.y);

	gridSprite.setTexture(gridTexture);
	gridSprite.setPosition(0.f, 0.f);

	bool quit = false;
	while (!quit)
	{
		time.restart();

		sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
		sf::Vector2f mousePos_mapped = m_window->mapPixelToCoords(mousePos, m_view);

		//controls
		sf::Event eve;
		while (m_window->pollEvent(eve))
		{
			if (eve.type == sf::Event::Closed)
			{
				quit = true;
				break;
			}
			else if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Left)
			{
				break;
			}
			else if (eve.type == sf::Event::MouseButtonPressed && eve.mouseButton.button == sf::Mouse::Right)
			{
				break;
			}
			else if (eve.type == sf::Event::MouseWheelScrolled)
			{
				if (eve.mouseWheelScroll.delta < 0)
				{
					m_view.zoom(1.1f);
				}
				else if (eve.mouseWheelScroll.delta > 0)
				{
					m_view.zoom(0.9f);
				}
				break;
			}
			else if (eve.type == sf::Event::KeyPressed)
			{
				switch (eve.key.code)
				{
				case sf::Keyboard::H: std::cout << "no one can help you :)" << std::endl;
					break;
				case sf::Keyboard::P:
					pause = !pause;
					break;
				case sf::Keyboard::Escape:
					quit = true;
					break;
				}
			}
			else if (eve.type == sf::Event::Resized)
			{
				m_view = sf::View(sf::FloatRect(0, 0, eve.size.width, eve.size.height));
				//sf::Vector2f view_center((m_grid[0].getPosition() + m_grid[m_grid.size() - 1].getPosition()) / 2.0f);
				m_view.setCenter(sf::Vector2f(0, 0));
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) && !mouseMoveRect.contains(mousePos))
		{
			m_view.move((sf::Vector2f(mousePos) - windowCenter)*dt);
		}

		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
			m_gridImage.setPixel(static_cast<int>(mousePos_mapped.x), static_cast<int>(mousePos_mapped.y), sf::Color::White);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			m_view.move(sf::Vector2f(0.f, -1.f)*50.0f*dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			m_view.move(sf::Vector2f(-1.f, 0.f)*50.0f*dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			m_view.move(sf::Vector2f(0.f, 1.f)*50.0f*dt);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			m_view.move(sf::Vector2f(1.f, 0.f)*50.0f*dt);

		/*currentTile = m_grid.getGridNumber(mousePos_mapped);
		if (currentTile != -1)
			m_currentTile.setPosition(m_grid[currentTile].getPosition());*/
		//*** controls

		//updates
		tickRun -= dt;
		if (tickRun <= 0.f)
		{
			if (m_step == true)
			{
				tickRun = tick;
				//m_grid.evaporate(0.85f);
				//m_player.update(dt);
				if (measure)
				{
					std::cout << "FPS: " << 1 / dt << std::endl;
					measure = false;
				}
			}

			if (stepMode)
				m_step = false;
		}

		//render
		m_window->clear(sf::Color(69, 69, 69));

		m_window->setView(m_view);

		gridTexture.update(m_gridImage);
		m_window->draw(gridSprite);
		/*for (int x = 0; x < m_grid.size(); ++x)
		{
			for (int y = 0; y < m_grid[x].size(); ++y)
			{
				m_window->draw(m_grid[x][y]);

				//debug_text.setPosition(m_grid[i].getPosition() - sf::Vector2f(1.f, 1.f));
				//debug_text.setString(std::to_string(static_cast<int>(m_grid.getPheromoneMap()[i])));
				////debug_text.setString(std::to_string(i));
				//m_window->draw(debug_text);
			}
		}*/

		// debug text
		m_window->setView(m_window->getDefaultView());

		debugString.str(std::string());
		int fps = 1.f / dt;
		debugString << fps;
		debugString << "\n" << static_cast<int>(mousePos_mapped.x) << ":" << static_cast<int>(mousePos_mapped.y);
		debug_text.setString(debugString.str());
		//debug_text.setString(std::to_string(fps));

		m_window->draw(debug_text);
		//*** dt
		//m_window->draw(m_currentTile);

		m_window->display();
		//*** render

		dt = time.getElapsedTime().asSeconds();
	}
}

void planeWorld::setRenderWindow(sf::RenderWindow * wndw)
{
	m_window = wndw;
}

bool planeWorld::setWorldDimensions(int size)
{
	return setWorldDimensions(size, size);
}

bool planeWorld::setWorldDimensions(int size_x, int size_y)
{
	if(size_x < 0 || size_y < 0)
		return true;

	Pixel tmpPixel;
	tmpPixel.setSize(sf::Vector2f(pixelSize, pixelSize));
	tmpPixel.setFillColor(sf::Color::Green);
	tmpPixel.value = 0.f;

	m_dimension = sf::Vector2i(size_x, size_y);

	//m_grid = new int[m_dimension.x * m_dimension.y]{0};

	m_gridImage.create(m_dimension.x, m_dimension.y, sf::Color::Black);

	return false;
}
