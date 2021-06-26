// source file for the user interface
// this file contains user interface code, and input validation

#include "ui.h"
#include "main.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// forward declarations of functions
void UI_MenuBar(SDL_Window* window);
void UI_ControlWindow(SDL_Window* window, bool* p_open);
void UI_OutputWindow(SDL_Window* window, bool* p_open);


// main user interface function
// only this has to be called from main
void UI(SDL_Window* window) {
	// set up new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	//ImGui::Begin("Test Window");

	//ImGuiInputTextFlags inputflags =
	//	ImGuiInputTextFlags_EnterReturnsTrue;

	//static float frameCap = frameTimer->getRateCap();
	//if (ImGui::InputFloat("Frame Cap", &frameCap, NULL, NULL, "%.1f", inputflags)) {
	//	frameTimer->setRateCap(frameCap);
	//}

	//ImGui::End();

	UI_MenuBar(window);

	// render user interface
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// procedure to generate menu bar at the top of the screen.
void UI_MenuBar(SDL_Window* window) {
	// static booleans to track UI state
	static bool show_imgui_demo = false;
	static bool show_control_window = true;
	static bool show_output_window = false;

	// get imgui io handler reference
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	// set constraints on window size and position
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	// min window height is smaller than text height therefore minimum height applies
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 10));

	// unused variable, simply here to I can pass window flags to the window
	static bool p_open;

	// flags for window creation
	static ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_MenuBar;

	// bool for passing to unused options
	static bool dummyBool;

	// remove minimum window size constraint
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	// create menu bar
	ImGui::Begin("Menu Bar##menubar", &p_open, windowflags);
	ImGui::PopStyleVar();

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("View##menubar")) {
			ImGui::MenuItem("Control Window##menubar", NULL, &show_control_window);
			ImGui::MenuItem("Output Window##menubar", NULL, &show_output_window);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug##menubar")) {
			ImGui::MenuItem("ImGui Demo Window", NULL, &show_imgui_demo);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);
	if (show_control_window) UI_ControlWindow(window, &show_control_window);
	if (show_output_window) UI_OutputWindow(window, &show_output_window);
}

// procedure for control window
void UI_ControlWindow(SDL_Window* window, bool* p_open) {
	// get imgui io handler reference
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	// top menu bar is 18 px high
	ImGui::SetNextWindowPos(ImVec2(0, 18));
	ImGui::SetNextWindowSizeConstraints(ImVec2(250, windowHeight - 18), ImVec2(windowWidth, windowHeight - 18));
	// flags for window
	ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoMove;

	// create window
	ImGui::Begin("Control Menu##control", p_open, windowflags);






	ImGui::End();
}

// procedure for output window
void UI_OutputWindow(SDL_Window* window, bool* p_open) {
	// get imgui io handler reference
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	ImGui::SetNextWindowSizeConstraints(ImVec2(250, windowHeight - 18), ImVec2(windowWidth, windowHeight - 18));
	// flags for window
	ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoMove;

	// create window
	ImGui::Begin("Output Menu##output", p_open, windowflags);
	// set window position
	ImVec2 window_size = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(windowWidth - window_size.x, 18));






	ImGui::End();
}