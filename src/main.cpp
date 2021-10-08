// main source file for program
// meant to contain minimal functions, only main() and init()

#include "main.h"
#include "ui.h"
#include "obj.h"
#include "screen.h"
#include "time.h"
//#include "camera.h"
//#include "objects.h"
#include "simulation.h"
#include "gltf.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image_write.h"

// defined constants for main file

#define TITLE "Orbital Mechanics"
uint16_t width = 1000;
uint16_t height = 800;

// global variables for main file

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
SDL_GLContext gl_context;

// externally declared global object pointers

Screen* screen;
Timer* frameTimer;
std::vector<Simulation*> simulations;
extern INT_T activeSimulation;
//Camera* camera;
//Objects* objects;

// forward definitions of main functions

bool handleEvents();
bool init();
void shutdown();


int main() {
	// initialize main program parts
	// if initialization fails return from main with
	// non-zero value
	if (!init()) return -1;

	bool status;
	UINT_T size;
	//GLB_GenMesh("./assets/EarthClouds_1_12756.glb", size, status);

	// main loop will continue to run if true
	bool run = true;
	while (run) {
		// process all os events
		run = handleEvents();
		// clear screen for rendering
		screen->clear();

		// run user interface and render
		// UI will also handle all objects
		UI(window);

		// swap renderbuffer to screen
		screen->swap();
		// wait delay to fill out frame
		frameTimer->delay();
	}

	// shut down and deallocate all memory
	shutdown();
	return 0;
}


// This function will handle all inputs
// and OS requests. It will return true if the
// program should continue running
bool handleEvents() {
	bool run = true;

	static ImGuiIO& io = ImGui::GetIO();
	Simulation& simulation = *simulations[activeSimulation];

	// handle OS events
	while (SDL_PollEvent(&event)) {
		// let ImGui process events for UI
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type) {

			// case for program close
		case SDL_QUIT:
			run = false;
			break;

			// case for a window change
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
				// case for window resize
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				screen->setViewport(event.window.data1, event.window.data2);
				simulation.camera.setScreen(event.window.data1, event.window.data2);
				simulation.camera.calculateProjectionMatrix();
				width = event.window.data1;
				height = event.window.data2;
			}
			break;

			// case for a mouse motion event
		case SDL_MOUSEMOTION:
			// case for middle mouse button (revolve)
			// only revolve if mouse is not over GUI window
			if (event.motion.state & SDL_BUTTON_MMASK && !io.WantCaptureMouse) {
				simulation.camera.mouseRevolve(event.motion.xrel, event.motion.yrel);
				simulation.camera.calculateViewMatrix();
			}
			// case for right mouse button (translate on plane)
			if (event.motion.state & SDL_BUTTON_RMASK && !io.WantCaptureMouse) {
				simulation.camera.mouseTranslate(event.motion.xrel, event.motion.yrel);
				simulation.camera.calculateViewMatrix();
			}
			break;

			// case for mouse wheel event
		case SDL_MOUSEWHEEL:
			// check GUI window is not focused
			if (!io.WantCaptureMouse) {
				simulation.camera.mouseZoom(event.wheel.y);
				simulation.camera.calculateViewMatrix();
			}
			break;
		}
	}

	return run;
}


// Initializes the entire program.
// Returns true if initialization succeeds,
// and false if it fails.
bool init() {
	// initialize SDL library with SDL_INIT_VIDEO flag
	// this will only load necessary SDL components
	// this function returns a non-zero value if it fails
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		// if init fails use SDL_GetError() to return const char* message
		std::cout << "SDL failed to initialize: " << SDL_GetError() << "\n";
		return false;
	}

	// create the window
	window = SDL_CreateWindow(
		// window title
		TITLE,
		// window x position
		SDL_WINDOWPOS_CENTERED,
		// window y position
		SDL_WINDOWPOS_CENTERED,
		// window width
		width,
		// window height
		height,
		// window creation flags
		// create window to support OpenGL
		SDL_WINDOW_OPENGL |
		// on startup show window (not minimised)
		SDL_WINDOW_SHOWN |
		// window is resizable after startup
		SDL_WINDOW_RESIZABLE
	);

	// check if window has been successfully created
	// i.e. check window variable is not a null pointer
	if (!window) {
		std::cout << "SDL failed to create window: " << SDL_GetError() << "\n";
		return false;
	}

	// create hardware renderer for window
	renderer = SDL_CreateRenderer(
		// pass pointer to window object
		window,
		// initialize z index to frontmost possible index
		-1,
		// pass flag for hardware acceleration
		SDL_RENDERER_ACCELERATED
	);

	// check if renderer has been created
	// i.e. check renderer is not nullptr
	if (!renderer) {
		std::cout << "SDL failed to initialize renderer: " << SDL_GetError() << "\n";
		return false;
	}

	// create OpenGL context
	// returns null if fail
	gl_context = SDL_GL_CreateContext(window);
	// check gl_context is not null
	if (!gl_context) {
		std::cout << "SDL failed to create OpenGl context" << "\n";
		return false;
	}

	// by default, Windows only loads OpenGL version 1 features
	// I will use GLAD to load all OpenGL extensions
	// up to and including OpenGL version 6
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to load OpenGL extensions" << "\n";
	}

	
	// initialize global objects (on heap)

	bool status;
	// initialize screen buffers
	screen = new Screen(window, status);
	// check successful init
	if (!status) return false;
	// create frame timer
	frameTimer = new Timer();
	//// create camera object
	//camera = new Camera();
	//// create objects handling instance
	//objects = new Objects(status);
	//if (!status) return false;
	simulations.push_back(new Simulation());
	simulations[activeSimulation]->reportProgramFrameRate(frameTimer->getRateCap());
	//simulations[activeSimulation]->startRunThread();

	//// init camera to useful position
	//float position[3] = { 5.0f, 5.0f, 10.0f };
	//camera->setPosition(position);
	//camera->calculateProjectionMatrix();
	//camera->calculateViewMatrix();

	// set details for the screen
	screen->setViewport(width, height);
	screen->setClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	// set OpenGL flags for rendering

	// enable depth buffer in rendering
	// this allows fragments behind others to be rejected
	glEnable(GL_DEPTH_TEST);
	// comparison used to determine fragment kept in depth comparison
	glDepthFunc(GL_LEQUAL);
	// enable writing to depth buffer
	glDepthMask(GL_TRUE);
	// set fragment blending mode for transparent fragments
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// enable blending
	glEnable(GL_BLEND);

	// initialize imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// set imgui to not write config to file
	io.IniFilename = NULL;
	ImGui::StyleColorsDark();
	// init imgui in opengl context
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init();

	UI_ConfigureStyle();

	Simulation& simulation = *simulations[activeSimulation];
	simulation.objects.newObject("./assets/planet.obj", status);
	simulation.objects.newObject("./assets/planet.obj", status);
	if (!status) {
		std::cout << "Initialization failed" << "\n";
	}

	auto it = simulation.objects.begin();

	float obj1pos[3] = { 1, 0, 0 };
	float obj2pos[3] = { -10, 0, 0 };
	float obj1vel[3] = { 0, 0, -2.7E-5 };
	float obj2vel[3] = { 0, 0, 2.7E-4 };
	simulation.objects.setPosition(*it, obj1pos);
	simulation.objects.setVelocity(*it, obj1vel);
	simulation.objects.setMass(*it, 10000);
	simulation.objects.setRadius(*it, 2);
	simulation.objects.setName(*it, "Planet 1");
	++it;
	simulation.objects.setPosition(*it, obj2pos);
	simulation.objects.setVelocity(*it, obj2vel);
	simulation.objects.setName(*it, "Planet 2");


	// if the function has reached this point, it's succeded
	return true;
}


// Deallocates all memory, deletes GPU resources,
// closes window, and closes libraries.
// Should be called once at the end.
void shutdown() {
	// delete heap allocated objects
	// delete will also call destructors
	// for these objects
	delete screen;
	delete frameTimer;
	for (auto it = simulations.begin(); it != simulations.end(); ++it) delete *it;
	//delete camera;
	//delete objects;

	// delete all contexts
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}