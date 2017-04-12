#include "planeWorld.h"



planeWorld::planeWorld()
	:tick(0.1f),
	stepMode(false),
	gap(1.f),
	pixelSize(1.f),
	m_debug(true)
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

	i_createHourglass();

	i_initOpenCL(1, 0);

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
					i_createHourglass();
					break;
				case sf::Keyboard::N:
					m_step = true;
					break;
				case sf::Keyboard::P:
					pause = !pause;
					break;
				case sf::Keyboard::Equal:
					setBrushSize(m_brushSize + 10);
					break;
				case sf::Keyboard::Dash:
					if(m_brushSize > 10)
						setBrushSize(m_brushSize - 10);
					break;
				case sf::Keyboard::Q:
					rotate(ROTATE_LEFT);
					break;
				case sf::Keyboard::E:
					rotate(ROTATE_RIGHT);
					break;
				case sf::Keyboard::U:
					updateGrid();
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
			//draw(static_cast<sf::Vector2u>(mousePos_mapped), sf::Color(1, 255, 0));
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
			tickRun = tick;
			if (m_step == true)
			{
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
			updateGrid();
		}

		//render
		m_window->clear(sf::Color(69, 69, 69));

		m_window->setView(m_view);

		gridTexture.update(*m_gridImagePtr);
		m_window->draw(gridSprite);

		m_brushCircle.setPosition(mousePos_mapped);
		m_window->draw(m_brushCircle);

		// debug text
		m_window->setView(m_window->getDefaultView());

		debugString.str(std::string());//to clean string
		int fps = int(1.f / dt);
		debugString << fps;
		debugString << "\n" << static_cast<int>(mousePos_mapped.x) << ":" << static_cast<int>(mousePos_mapped.y);
		debug_text.setString(debugString.str());

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

	m_OpenCL_imageData = new sf::Uint32[size_x * size_y];

	return false;
}

void planeWorld::updateGrid()
{
	const sf::Uint8* gridPtr = m_gridImagePtr->getPixelsPtr();

	sf::Image* otherPtr = i_getOtherPointer();

	int init;
	if (m_margo)
		init = 0;
	else
		init = 1;
	m_margo = !m_margo;

	if (init == 1)
	{
		for (unsigned int x = 0; x < m_dimension.x; ++x)
			otherPtr->setPixel(x, 0, m_gridImagePtr->getPixel(x, 0));
		for (unsigned int x = 0; x < m_dimension.x; ++x)
			otherPtr->setPixel(x, m_dimension.y - 1, m_gridImagePtr->getPixel(x, m_dimension.y - 1));
		for (unsigned int y = 1; y < m_dimension.y - 1; ++y)
			otherPtr->setPixel(0, y, m_gridImagePtr->getPixel(0, y));
		for (unsigned int y = 1; y < m_dimension.y - 1; ++y)
			otherPtr->setPixel(m_dimension.x - 1, y, m_gridImagePtr->getPixel(m_dimension.x - 1, y));
	}

	//i_updateGridCPU(init);
	i_updateGridGPU(init);

	m_gridImagePtr = otherPtr;
}

void planeWorld::i_physicRules(sf::Color * fields)
{
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
	else if (fields[0].r > 0 && fields[1].r == 0 && fields[2].r > 0 && fields[3].r == 0 ||
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
}

sf::Image* planeWorld::i_getOtherPointer()
{
	if (m_gridImagePtr == &m_gridImage1)
		return &m_gridImage2;
	else
		return &m_gridImage1;
}

void planeWorld::i_updateGridCPU(int init)
{
	sf::Image* otherPtr = i_getOtherPointer();
	//omp_set_dynamic(0);
	#pragma omp parallel for num_threads(m_numberOfThreads)
	for (int y = init; y < m_dimension.y - 1; y += 2)//no need to calculate last line, because it is the floor
	{
		for (int x = init; x < m_dimension.x - 1; x += 2)
		{
			sf::Color fields[4];
			fields[0] = m_gridImagePtr->getPixel(x, y);
			fields[1] = m_gridImagePtr->getPixel(x + 1, y);
			fields[2] = m_gridImagePtr->getPixel(x, y + 1);
			fields[3] = m_gridImagePtr->getPixel(x + 1, y + 1);

			bool isWall = false;
			for (int i = 0; i < 4; ++i)
			{
				if (fields[i] == sf::Color::Blue)
				{
					isWall = true;
					break;
				}
			}

			if (!isWall)
				i_physicRules(fields);

			otherPtr->setPixel(x, y, fields[0]);
			otherPtr->setPixel(x + 1, y, fields[1]);
			otherPtr->setPixel(x, y + 1, fields[2]);
			otherPtr->setPixel(x + 1, y + 1, fields[3]);
		}
	}
}

void planeWorld::i_initOpenCL(unsigned int platformId, unsigned int deviceId)
{
	const std::string KERNEL_FILE = "cell.cl";
	cl_int err = CL_SUCCESS;
	std::vector<cl::Device> devices;

	platformId = ((platformId < 0) ? 0 : platformId);
	deviceId = ((deviceId < 0) ? 0 : deviceId);

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() == 0) {
		std::cout << "No OpenCL platforms available!\n";
		return;
	}

	// create a context and get available devices
	cl::Platform platform = platforms[platformId]; // on a different machine, you may have to select a different platform
	std::cout << "Platform Name: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
	cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0 };

	m_OpenCLData.context = cl::Context(CL_DEVICE_TYPE_ALL, properties);
	devices = m_OpenCLData.context.getInfo<CL_CONTEXT_DEVICES>();

	if (devices.size() == 0 && platforms.size() != 0)
	{
		platform = platforms[platformId + 1];
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0 };

		m_OpenCLData.context = cl::Context(CL_DEVICE_TYPE_ALL, properties);

		devices = m_OpenCLData.context.getInfo<CL_CONTEXT_DEVICES>();

		if (devices.size() == 0)
		{
			std::cout << "no available devices found\n";
			exit(666);
		}
	}

	char deviceName[255];
	err = devices[deviceId].getInfo(CL_DEVICE_NAME, &deviceName);
	handle_clerror(err);
	if (m_debug)
		std::cout << deviceName << std::endl;
	m_OpenCLData.device = devices[deviceId];

	// load and build the kernel
	std::ifstream sourceFile(KERNEL_FILE);
	if (!sourceFile)
	{
		std::cout << "kernel source file " << KERNEL_FILE << " not found!" << std::endl;
		return;
	}
	std::string sourceCode(
		std::istreambuf_iterator<char>(sourceFile),
		(std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	m_OpenCLData.program = cl::Program(m_OpenCLData.context, source);

	//try {
	err = m_OpenCLData.program.build(devices);
	if (err != ERROR_SUCCESS)
	{
		std::string s;
		m_OpenCLData.program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &s);
		std::cout << s << std::endl;
		m_OpenCLData.program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_OPTIONS, &s);
		std::cout << s << std::endl;
	}
	handle_clerror(err);

	//create kernels
	m_OpenCLData.kernel = cl::Kernel(m_OpenCLData.program, "cell", &err);
	handle_clerror(err);
}

void planeWorld::i_updateGridGPU(int init)
{
	cl_int err = CL_SUCCESS;
	cl::CommandQueue queue(m_OpenCLData.context, m_OpenCLData.device, 0, &err);
	handle_clerror(err);
	// create input and output data

	// buffers
	sf::Uint32 imageSize = m_dimension.x * m_dimension.y/* * sizeof(sf::Uint32)*/;
	cl::Buffer elements = cl::Buffer(m_OpenCLData.context, CL_MEM_READ_WRITE, imageSize * sizeof(sf::Uint32));
	// fill buffers
	queue.enqueueWriteBuffer(
		elements, // which buffer to write to
		CL_TRUE, // block until command is complete
		0, // offset
		imageSize * sizeof(sf::Uint32), // size of write 
		m_gridImagePtr->getPixelsPtr()// pointer to input
	);

	m_OpenCLData.kernel.setArg(0, elements);
	m_OpenCLData.kernel.setArg(1, m_dimension.x);
	m_OpenCLData.kernel.setArg(2, m_dimension.y);
	m_OpenCLData.kernel.setArg(3, init);

	if (m_rng->GetNumber() / (ULONG_MAX + 1.0f) > 0.2)
	{
		m_OpenCLData.kernel.setArg(4, 1);
	}
	else
		m_OpenCLData.kernel.setArg(4, 0);

	// launch add kernel
	// Run the kernel on specific ND range
	sf::Vector2i globalSize;
	for (int i = 1; i <= 20; ++i)
	{
		if (pow(2, i) > m_dimension.x/2)
		{
			globalSize.x = pow(2, i);
			break;
		}
	}

	for (int i = 1; i <= 20; ++i)
	{
		if (pow(2, i) > m_dimension.y/2)
		{
			globalSize.y = pow(2, i);
			break;
		}
	}

	/*if (m_debug)
		std::cout << "threads|real size -> " << globalSize.x << ":" << m_dimension.x << "|" << globalSize.y << ":" << m_dimension.y << std::endl;*/

	cl::NDRange global(globalSize.x, globalSize.y);
	cl::NDRange local(16, 16); //make sure local range is divisible by global range
	cl::NDRange offset(0, 0);

	//std::cout << "call 'cell' kernel; cycle " << i << std::endl;
	queue.enqueueNDRangeKernel(m_OpenCLData.kernel, offset, global, local);

	//queue.enqueueCopyBuffer(tmp, elements, 0, 0, m_elements.size() * sizeof(char), 0, 0);

	// read back result
	queue.enqueueReadBuffer(elements, CL_TRUE, 0, imageSize * sizeof(sf::Uint32), m_OpenCL_imageData);

	sf::Image* otherPtr = i_getOtherPointer();
	sf::Uint32 pos;
	for (unsigned int y = 0; y < m_dimension.y; ++y)
	{
		for (unsigned int x = 0; x < m_dimension.x; ++x)
		{
			pos = x + (y * m_dimension.x);
			otherPtr->setPixel(x, y, sf::Color(_byteswap_ulong(m_OpenCL_imageData[pos])));
			//otherPtr->setPixel(x, y, sf::Color(m_OpenCL_imageData[pos]));
		}
	}
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
	int half = static_cast<int>(m_brushSize * 0.5);
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
				if(newX > 0 && newY > 0 && newX < m_dimension.x && newY < m_dimension.y)//assuming the brushSize is always smaller than the image
				{
					sf::Color cellColor = m_gridImagePtr->getPixel(newX, newY);
					if(cellColor != sf::Color::Blue)
						m_gridImagePtr->setPixel(newX, newY, color);
				}
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

void planeWorld::rotate(Rotation r)
{
	sf::Image* otherPtr = i_getOtherPointer();
	otherPtr->create(m_dimension.x, m_dimension.y, sf::Color::Black);

	for (int y = 0; y < m_dimension.y; ++y)
	{
		for (int x = 0; x < m_dimension.x; ++x)
		{
			int newX = x + y + 1;
			int newY = - x + y + m_dimension.y;

			if (newX > 0 && newX < m_dimension.x && newY > 0 && newY < m_dimension.y)
			{
				sf::Color tmpCol = m_gridImagePtr->getPixel(x, y);
				//if (tmpCol != sf::Color::Black)
				//	__debugbreak();
				otherPtr->setPixel(newX, newY, tmpCol);
			}
		}
	}

	m_gridImagePtr->copy(*otherPtr, 0, 0, sf::IntRect(0, 0, m_dimension.x, m_dimension.y));
}

void planeWorld::setNumberOfThreads(unsigned int t)
{
	m_numberOfThreads = t;
}

int planeWorld::i_manhattanDistance(sf::Vector2i a, sf::Vector2i b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

void planeWorld::i_createHourglass()
{
	double aspectRatio = static_cast<double>(m_dimension.x) / m_dimension.y;
	int halfX = static_cast<int>(m_dimension.x / 2);
	int halfY = static_cast<int>(m_dimension.y / 2);
	for (int x = 0; x < halfX - halfX * 0.03; ++x)
	{
		for (int y = 0; y < halfY; ++y)
		{
			double tmp = static_cast<double>(x + 1) / (y + 1);
			if(tmp < aspectRatio)
				m_gridImagePtr->setPixel(x, y, sf::Color::Blue);
		}
	}

	sf::Image* otherPtr = i_getOtherPointer();

	//copy top left corner to the second image
	otherPtr->copy(*m_gridImagePtr, 0, 0, sf::IntRect(0, 0, halfX, halfY));

	//flip and copy to top right corner
	m_gridImagePtr->flipHorizontally();
	otherPtr->copy(*m_gridImagePtr, halfX, 0, sf::IntRect(halfX, 0, halfX, halfY));

	//flip and copy to bottom right corner
	m_gridImagePtr->flipVertically();
	otherPtr->copy(*m_gridImagePtr, halfX, halfY, sf::IntRect(halfX, halfY, halfX, halfY));

	//flip and copy to bottom right corner
	m_gridImagePtr->flipHorizontally();
	otherPtr->copy(*m_gridImagePtr, 0, halfY, sf::IntRect(0, halfY, halfX, halfY));

	//copy whole image back to the original
	m_gridImagePtr->copy(*otherPtr, 0, 0, sf::IntRect(0, 0, m_dimension.x, m_dimension.y));
}

std::string planeWorld::cl_errorstring(cl_int err) {
	switch (err) {
	case CL_SUCCESS:                          return std::string("Success");
	case CL_DEVICE_NOT_FOUND:                 return std::string("Device not found");
	case CL_DEVICE_NOT_AVAILABLE:             return std::string("Device not available");
	case CL_COMPILER_NOT_AVAILABLE:           return std::string("Compiler not available");
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:    return std::string("Memory object allocation failure");
	case CL_OUT_OF_RESOURCES:                 return std::string("Out of resources");
	case CL_OUT_OF_HOST_MEMORY:               return std::string("Out of host memory");
	case CL_PROFILING_INFO_NOT_AVAILABLE:     return std::string("Profiling information not available");
	case CL_MEM_COPY_OVERLAP:                 return std::string("Memory copy overlap");
	case CL_IMAGE_FORMAT_MISMATCH:            return std::string("Image format mismatch");
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:       return std::string("Image format not supported");
	case CL_BUILD_PROGRAM_FAILURE:            return std::string("Program build failure");
	case CL_MAP_FAILURE:                      return std::string("Map failure");
		// case CL_MISALIGNED_SUB_BUFFER_OFFSET:     return std::string("Misaligned sub buffer offset");
		// case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return std::string("Exec status error for events in wait list");
	case CL_INVALID_VALUE:                    return std::string("Invalid value");
	case CL_INVALID_DEVICE_TYPE:              return std::string("Invalid device type");
	case CL_INVALID_PLATFORM:                 return std::string("Invalid platform");
	case CL_INVALID_DEVICE:                   return std::string("Invalid device");
	case CL_INVALID_CONTEXT:                  return std::string("Invalid context");
	case CL_INVALID_QUEUE_PROPERTIES:         return std::string("Invalid queue properties");
	case CL_INVALID_COMMAND_QUEUE:            return std::string("Invalid command queue");
	case CL_INVALID_HOST_PTR:                 return std::string("Invalid host pointer");
	case CL_INVALID_MEM_OBJECT:               return std::string("Invalid memory object");
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  return std::string("Invalid image format descriptor");
	case CL_INVALID_IMAGE_SIZE:               return std::string("Invalid image size");
	case CL_INVALID_SAMPLER:                  return std::string("Invalid sampler");
	case CL_INVALID_BINARY:                   return std::string("Invalid binary");
	case CL_INVALID_BUILD_OPTIONS:            return std::string("Invalid build options");
	case CL_INVALID_PROGRAM:                  return std::string("Invalid program");
	case CL_INVALID_PROGRAM_EXECUTABLE:       return std::string("Invalid program executable");
	case CL_INVALID_KERNEL_NAME:              return std::string("Invalid kernel name");
	case CL_INVALID_KERNEL_DEFINITION:        return std::string("Invalid kernel definition");
	case CL_INVALID_KERNEL:                   return std::string("Invalid kernel");
	case CL_INVALID_ARG_INDEX:                return std::string("Invalid argument index");
	case CL_INVALID_ARG_VALUE:                return std::string("Invalid argument value");
	case CL_INVALID_ARG_SIZE:                 return std::string("Invalid argument size");
	case CL_INVALID_KERNEL_ARGS:              return std::string("Invalid kernel arguments");
	case CL_INVALID_WORK_DIMENSION:           return std::string("Invalid work dimension");
	case CL_INVALID_WORK_GROUP_SIZE:          return std::string("Invalid work group size");
	case CL_INVALID_WORK_ITEM_SIZE:           return std::string("Invalid work item size");
	case CL_INVALID_GLOBAL_OFFSET:            return std::string("Invalid global offset");
	case CL_INVALID_EVENT_WAIT_LIST:          return std::string("Invalid event wait list");
	case CL_INVALID_EVENT:                    return std::string("Invalid event");
	case CL_INVALID_OPERATION:                return std::string("Invalid operation");
	case CL_INVALID_GL_OBJECT:                return std::string("Invalid OpenGL object");
	case CL_INVALID_BUFFER_SIZE:              return std::string("Invalid buffer size");
	case CL_INVALID_MIP_LEVEL:                return std::string("Invalid mip-map level");
	case CL_INVALID_GLOBAL_WORK_SIZE:         return std::string("Invalid gloal work size");
		// case CL_INVALID_PROPERTY:                 return std::string("Invalid property");
	default:                                  return std::string("Unknown error code");
	}
}

void planeWorld::handle_clerror(cl_int err) {
	if (err != CL_SUCCESS) {
		std::cerr << "OpenCL Error: " << cl_errorstring(err) << std::string(".") << std::endl;
		std::cin.ignore();
		exit(EXIT_FAILURE);
	}
}
