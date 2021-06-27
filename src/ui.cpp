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
bool UI_FSDialog(SDL_Window* window, std::string& writeback, bool* p_open, const char* extension = "");


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

	objects->renderAll();

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
	static bool showfs = false;

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
			ImGui::MenuItem("FS##menubar", NULL, &showfs);
			ImGui::MenuItem("ImGui Demo Window##menubar", NULL, &show_imgui_demo);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();


	if (show_imgui_demo) ImGui::ShowDemoWindow(&show_imgui_demo);
	if (show_control_window) UI_ControlWindow(window, &show_control_window);
	if (show_output_window) UI_OutputWindow(window, &show_output_window);

	std::string dummystring;
	static bool testbool = false;
	if (showfs) testbool = UI_FSDialog(window, dummystring, &showfs, ".obj");
	if (testbool) {
		std::cout << dummystring << "\n";
		testbool = false;

		bool status;
		objects->newObject(dummystring.c_str(), status);
	}
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



// procedure creates a file dialog and returns a file path
bool UI_FSDialog(SDL_Window* window, std::string& writeback, bool* p_open, const char* extension) {
	// get imgui io handler reference
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	//static INT_T width, height
	ImGui::SetNextWindowPos(ImVec2((windowWidth - 500) / 2, (windowHeight - 350) / 2), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Once);
	// flags for window
	ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoCollapse;

	// create namespace declaration to reduce code
	namespace fs = std::filesystem;

	// wether the file has been opened
	bool fileOpened = false;
	// boolean that tracks whether opening unselected item has been attempted
	static bool openAttempt = false;
	// static bool that tracks wether extension is valid
	static bool invalidExtension = false;

	ImGui::Begin("Open##fs", p_open, windowflags);

	// get current path
	static fs::path path = fs::current_path();
	static std::string pathstr = path.string();

	// button for moving to parent directory
	//ImGui::PushItemWidth(60);
	if (ImGui::Button("<##fs", ImVec2(25, 0))) {
		path = path.parent_path();
		pathstr = path.string();
	}
	//ImGui::PopItemWidth();

	ImGui::SameLine();
	// show current directory
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), pathstr.c_str());

	// seperator before directory contents
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGuiSelectableFlags selectflags =
		ImGuiSelectableFlags_AllowDoubleClick;

	// list directory contents
	// index of item selected
	static INT_T selected = -1;
	INT_T index = 0;
	static fs::path selectedpath;
	// contain within list box
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (ImGui::BeginListBox("directory##fs", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 98))) {
		for (const auto& p : fs::directory_iterator(path)) {
			if (fs::is_directory(p)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
			else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			if (ImGui::Selectable(p.path().string().erase(0, pathstr.length()).c_str(), selected == index, selectflags)) {
				//selected = index;
				// check if left mouse button clicked twice
				if (ImGui::IsMouseDoubleClicked(0)) {
					// check if path is directory
					if (fs::is_directory(p)) {
						path = p.path();
						pathstr = path.string();
					}
					else {
						// if file extension is required
						if (*extension && strcmp(extension, p.path().extension().string().c_str()) != 0) {
							//if (strcmp(extension, p.path().extension().string().c_str()) != 0) {
							invalidExtension = true;
						}
						else {
							invalidExtension = false;
							writeback = p.path().string();
							// replace backslashes with forward slashes
							// it appears std::ifstream doesn't like them
							std::replace(writeback.begin(), writeback.end(), '\\', '/');
							// close window
							*p_open = false;
							fileOpened = true;
						}

					}
					// reset selected
					selected = -1;
				}
				else {
					// select item
					selected = index;
					openAttempt = false;
					if (!fs::is_directory(p)) {
						selectedpath = p.path();
					}
				}
			}
			++index;
			ImGui::PopStyleColor();
		}
		ImGui::EndListBox();
	}
	ImGui::PopStyleColor();

	// seperator before final buttons
	//ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 35);
	//ImGui::Separator();
	ImGui::Spacing();

	// display warning message to the user
	if (openAttempt) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select a file");
		ImGui::SameLine();
	}
	else if (invalidExtension) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Extension must be %s", extension);
		ImGui::SameLine();
	}

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);
	
	if (ImGui::Button("Open##fs", ImVec2(60, 0))) {
		if (selectedpath.empty()) {
			openAttempt = true;
		}
		else {
			// if file extension is required
			if (*extension && strcmp(extension, selectedpath.extension().string().c_str()) != 0) {
				//if (strcmp(extension, selectedpath.extension().string().c_str()) != 0) {
				invalidExtension = true;
				}
			else {
				invalidExtension = false;
				writeback = selectedpath.string();
				std::replace(writeback.begin(), writeback.end(), '\\', '/');
				*p_open = false;
				fileOpened = true;
			}
		}
	}
	
	ImGui::SameLine();
	if (ImGui::Button("Cancel##fs", ImVec2(60, 0))) {
		*p_open = false;
		openAttempt = false;
	}

	ImGui::End();

	// if window should close reset some parameters
	if (!*p_open) {
		selectedpath.clear();
		selected = -1;
	}

	// return true if file found
	return fileOpened;
}