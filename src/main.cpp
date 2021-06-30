// main source file for program
// meant to contain minimal functions, only main() and init()

// SDL is used for window management and all
// that nasty platform specific stuff
#include <SDL.h>
#undef main // must undefine main defined in SDL.h
// to avoid linker errors
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "main.h"
#include "ui.h"
#include "obj.h"

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
Camera* camera;
Objects* objects;

// forward definitions of main functions

bool handleEvents();
bool init();
void shutdown();


int main() {
	// initialize main program parts
	// if initialization fails return from main with
	// non-zero value
	if (!init()) return -1;

	//bool success;
	//UINT_T object1 = objects->newObject("C:/Users/Finley/code/autopilotinterface/assets/cube2.obj", success);
	////UINT_T object1 = objects->newObject("..\\autopilotinterface\\assets\\cube2.obj", success);
	//float object1position[3] = { -2.0f, 0.0f, 0.0f };
	//objects->setPosition(object1, object1position);
	//objects->setName(object1, "test");
	//UINT_T object2 = objects->newObject("C:/Users/Finley/code/autopilotinterface/assets/cube2.obj", success);
	////UINT_T object1 = objects->newObject("..\\autopilotinterface\\assets\\cube2.obj", success);
	//objects->setPosition(object2, object1position);
	//objects->setName(object2, "test2");
	////objects->deleteObject(object1);
	//objects->deleteObject(object2);

	float position[3] = { -5.0f, 5.0f, 10.0f };
	camera->setPosition(position);
	camera->calculateProjectionMatrix();
	camera->calculateViewMatrix();

	// main loop will continue to run if true
	bool run = true;
	while (run) {
		run = handleEvents();
		screen->clear();

		UI(window);

		screen->swap();
		frameTimer->delay();
	}

	shutdown();
	return 0;
}


// This function will handle all inputs
// and OS requests. It will return true if the
// program should continue running
bool handleEvents() {
	bool run = true;

	static ImGuiIO& io = ImGui::GetIO();

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
				camera->setScreen(event.window.data1, event.window.data2);
				camera->calculateProjectionMatrix();
				width = event.window.data1;
				height = event.window.data2;
			}
			break;

			// case for a mouse motion event
		case SDL_MOUSEMOTION:
			// case for middle mouse button (revolve)
			// only revolve if mouse is not over GUI window
			if (event.motion.state & SDL_BUTTON_MMASK && !io.WantCaptureMouse) {
				camera->mouseRevolve(event.motion.xrel, event.motion.yrel);
				camera->calculateViewMatrix();
			}
			// case for right mouse button (translate on plane)
			if (event.motion.state & SDL_BUTTON_RMASK && !io.WantCaptureMouse) {
				camera->mouseTranslate(event.motion.xrel, event.motion.yrel);
				camera->calculateViewMatrix();
			}
			break;

			// case for mouse wheel event
		case SDL_MOUSEWHEEL:
			// check GUI window is not focused
			if (!io.WantCaptureMouse) {
				camera->mouseZoom(event.wheel.y);
				camera->calculateViewMatrix();
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

	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

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
	bool screenStatus;
	screen = new Screen(window, screenStatus);
	if (!screenStatus) return false;
	frameTimer = new Timer();
	camera = new Camera();
	bool objectStatus;
	objects = new Objects(objectStatus);
	if (!objectStatus) return false;

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

	screen->setViewport(width, height);
	screen->setClearColor(0.05f, 0.05f, 0.05f, 1.0f);

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


	// if the function has reached this point, it's succeded
	return true;
}


// Deallocates all memory, deletes GPU resources,
// closes window, and closes libraries.
// Should be called once at the end.
void shutdown() {
	// delete heap allocated objects
	delete screen;
	delete frameTimer;
	delete camera;
	delete objects;

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}