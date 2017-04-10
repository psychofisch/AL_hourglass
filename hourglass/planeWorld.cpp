#include "planeWorld.h"



planeWorld::planeWorld()
	:tick(0.1f),
	stepMode(false),
	gap(1.f),
	pixelSize(1.f)
{
	m_pixel.setFillColor(sf::Color::Green);
	m_pixel.setSize(sf::Vector2f(pixelSize, pixelSize));

	m_rng = new RNGesus(1, 2, 3);

	m_brushCircle.setFillColor(sf::Color::Transparent);
	m_brushCircle.setOutlineColor(sf::Color(69, 69, 69, 255));
	m_brushCircle.setOutlineThickness(1.f);

	setBrushSize(10);
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
				case sf::Keyboard::R:
					m_gridImage1.create(m_dimension.x, m_dimension.y, sf::Color::Black);
					m_gridImage2.create(m_dimension.x, m_dimension.y, sf::Color::Black);
					break;
				case sf::Keyboard::N:
					m_step = true;
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
				m_view = sf::View(sf::FloatRect(0, 0, static_cast<float>(eve.size.width), static_cast<float>(eve.size.height)));
				//sf::Vector2f view_center((m_grid[0].getPosition() + m_grid[m_grid.size() - 1].getPosition()) / 2.0f);
				m_view.setCenter(sf::Vector2f(0, 0));
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) && !mouseMoveRect.contains(mousePos))
		{
			m_view.move((sf::Vector2f(mousePos) - windowCenter)*dt);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && gridSprite.getGlobalBounds().contains(mousePos_mapped))
		{
			draw(static_cast<sf::Vector2u>(mousePos_mapped), sf::Color::White);
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && gridSprite.getGlobalBounds().contains(mousePos_mapped))
		{
			draw(static_cast<sf::Vector2u>(mousePos_mapped), sf::Color::Black);
		}

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
				updateGrid();
				if (measure)
				{
					std::cout << "FPS: " << 1 / dt << std::endl;
					measure = false;
				}
				m_step = false;
			}

			//if (stepMode)
				//m_step = false;
		}
		updateGrid();

		//render
		m_window->clear(sf::Color(69, 69, 69));

		m_window->setView(m_view);

		gridTexture.update(*m_gridImagePtr);
		m_window->draw(gridSprite);

		m_brushCircle.setPosition(mousePos_mapped);
		m_window->draw(m_brushCircle);

		// debug text
		m_window->setView(m_window->getDefaultView());

		debugString.str(std::string());
		int fps = int(1.f / dt);
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

	m_dimension = sf::Vector2u(size_x, size_y);

	m_gridImage1.create(m_dimension.x, m_dimension.y, sf::Color::Black);
	m_gridImage2.create(m_dimension.x, m_dimension.y, sf::Color::Black);

	m_gridImagePtr = &m_gridImage1;

	return false;
}

void planeWorld::updateGrid()
{
	const sf::Uint8* gridPtr = m_gridImagePtr->getPixelsPtr();

	sf::Image* otherPtr;
	if (m_gridImagePtr == &m_gridImage1)
		otherPtr = &m_gridImage2;
	else
		otherPtr = &m_gridImage1;

	sf::Color fields[4];

	int init;
	if (m_margo)
		init = 0;
	else
		init = 1;
	m_margo = !m_margo;

	if (init == 1)
	{
		for (int x = 0; x < m_dimension.x; ++x)
			otherPtr->setPixel(x, 0, m_gridImagePtr->getPixel(x, 0));
		for (int x = 0; x < m_dimension.x; ++x)
			otherPtr->setPixel(x, m_dimension.y - 1, m_gridImagePtr->getPixel(x, m_dimension.y - 1));
		for (int y = 0; y < m_dimension.y; ++y)
			otherPtr->setPixel(0, y, m_gridImagePtr->getPixel(0, y));
		for (int y = 0; y < m_dimension.y; ++y)
			otherPtr->setPixel(m_dimension.x - 1, y, m_gridImagePtr->getPixel(m_dimension.x - 1, y));
	}

//#pragma omp parallel for
	for (int y = init; y < m_dimension.y - 1; y += 2)//no need to calculate last line, because it is the floor
	{
		for (int x = init; x < m_dimension.x - 1; x += 2)
		{
			fields[0] = m_gridImagePtr->getPixel(x,     y);
			fields[1] = m_gridImagePtr->getPixel(x + 1, y);
			fields[2] = m_gridImagePtr->getPixel(x,     y + 1);
			fields[3] = m_gridImagePtr->getPixel(x + 1, y + 1);

			//	x | o		o | o
			//	- - -	->	- - -
			//	o | o		x | o
			if (fields[0].r > 0 && fields[1].r == 0 && fields[2].r == 0 && fields[3].r == 0)
			{
				fields[0] = sf::Color::Black;
				fields[2] = sf::Color::White;
			}
			//	o | x		o | o
			//	- - -	->	- - -
			//	o | o		o | x
			else if (fields[0].r == 0 && fields[1].r > 0 && fields[2].r == 0 && fields[3].r == 0)
			{
				fields[1] = sf::Color::Black;
				fields[3] = sf::Color::White;
			}
			//	x | o		o | o	&	o | x		o | o	&	o | x		o | o	&	x | o		o | o
			//	- - -	->	- - -	&	- - -	->	- - -	& 	- - -	->	- - -	&	- - -	->	- - -
			//	x | o		x | x	&	o | x		x | x	&	x | o		x | x	&	o | x		x | x
			else if (	fields[0].r > 0 && fields[1].r == 0 && fields[2].r > 0 && fields[3].r == 0 ||
						fields[0].r == 0 && fields[1].r > 0 && fields[2].r == 0 && fields[3].r > 0 ||
						fields[0].r == 0 && fields[1].r > 0 && fields[2].r > 0 && fields[3].r == 0 ||
						fields[0].r > 0 && fields[1].r == 0 && fields[2].r == 0 && fields[3].r > 0)
			{
				fields[0] = fields[1] = sf::Color::Black;
				fields[2] = fields[3] = sf::Color::White;
			}
			//	x | x		o | x
			//	- - -	->	- - -
			//	o | x		x | x
			else if (fields[0].r > 0 && fields[1].r > 0 && fields[2].r == 0 && fields[3].r > 0)
			{
				fields[0] = sf::Color::Black;
				fields[2] = sf::Color::White;
			}
			//	x | x		x | o
			//	- - -	->	- - -
			//	x | o		x | x
			else if (fields[0].r > 0 && fields[1].r > 0 && fields[2].r > 0 && fields[3].r == 0)
			{
				fields[1] = sf::Color::Black;
				fields[3] = sf::Color::White;
			}
			//	x | x		o | o		x | x
			//	- - -	->	- - -	OR	- - -
			//	o | o		x | x		o | o
			else if (fields[0].r > 0 && fields[1].r > 0 && fields[2].r == 0 && fields[3].r == 0)
			{
				if (m_rng->GetNumber() / (ULONG_MAX + 1.0f) > 0.2)
				{
					fields[0] = fields[1] = sf::Color::Black;
					fields[2] = fields[3] = sf::Color::White;
				}
			}

			otherPtr->setPixel(x,     y,     fields[0]);
			otherPtr->setPixel(x + 1, y,     fields[1]);
			otherPtr->setPixel(x,     y + 1, fields[2]);
			otherPtr->setPixel(x + 1, y + 1, fields[3]);
		}
	}

	m_gridImagePtr = otherPtr;
}

void planeWorld::toggleGridBuffer()
{
	if (m_gridImagePtr == &m_gridImage1)
		m_gridImagePtr = &m_gridImage2;
	else
		m_gridImagePtr = &m_gridImage1;
}

void planeWorld::draw(sf::Vector2u pos, sf::Color color)
{
	int half = m_brushSize * 0.5;
	for (int x = -half; x < half; ++x)
	{
		for (int y = -half; y < half; ++y)
		{
			int dist = i_manhattanDistance(sf::Vector2i(x, y), sf::Vector2i(0, 0));
			//std::cout << x << "|" << y << ": " << dist << std::endl;
			if (dist < m_brushSize * 0.8)
			{
				int newX = pos.x + x;
				int newY = pos.y + y;
				if(newX > half && newY > half && newX < m_dimension.x - half && newY < m_dimension.y - half)
					m_gridImagePtr->setPixel(newX, newY, color);
			}
		}
	}
}

void planeWorld::setBrushSize(int size)
{
	m_brushSize = size;
	float half = size * 0.5f;
	m_brushCircle.setRadius(half);
	m_brushCircle.setOrigin(sf::Vector2f(half, half));
}

int planeWorld::i_manhattanDistance(sf::Vector2i a, sf::Vector2i b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
