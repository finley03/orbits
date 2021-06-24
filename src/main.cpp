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

// forward definitions of main functions

bool handleEvents();
bool init();
void shutdown();


int main() {
	// initialize main program parts
	// if initialization fails return from main with
	// non-zero value
	if (!init()) return -1;

	uint32_t size;
	bool success;
	std::vector<float> vertices = OBJ_GenMesh("../glTests/assets/backpack/backpack.obj", size, success);
	//for (uint16_t i = 0; i < vertices.size(); i += 4) {
	//	printf("%f, %f, %f, %f\n", vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3]);
	//}
	std::cout << "size: " << size << "\n";

	std::string mtlfile = OBJ_GetMTLpath("../glTests/assets/backpack/backpack.obj", success);
	std::cout << mtlfile << "\n";

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
				width = event.window.data1;
				height = event.window.data2;
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
	screen = new Screen(window);
	frameTimer = new Timer();

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

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}