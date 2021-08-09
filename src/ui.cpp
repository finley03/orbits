// source file for the user interface
// this file contains user interface code, and input validation

#include "ui.h"
#include "screen.h"
#include "camera.h"
#include "objects.h"
#include "time.h"
#include "simulation.h"

extern Screen* screen;
//extern Camera* camera;
//extern Objects* objects;
extern Timer* frameTimer;
extern std::vector<Simulation*> simulations;
INT_T activeSimulation = 0;

void UI_ConfigureStyle();

// forward declarations of functions
void UI_MenuBar(SDL_Window* window);
void UI_ControlWindow(SDL_Window* window, bool* p_open);
void UI_OutputWindow(SDL_Window* window, bool* p_open);
bool UI_FSReadDialog(SDL_Window* window, std::string& writeback, bool* p_open, const char* extension = "", bool hideOtherExtensions = false);

void UI_InputSimulationTab();
void UI_InputObjectsTab(SDL_Window* window);
void UI_InputRenderingTab();
void UI_OutputObjects();

void UI_TableInt(const char* text, INT_T a);
void UI_TableFloat(const char* text, float a);

// boolean for output window is global so other windows can modify it
static bool show_output_window = false;
static bool output_set_active_tab = false;
UINT_T output_active_tab;

// global vector needed for communication between input and output windows
std::vector<UINT_T> openObjects;


// main user interface function
// only this has to be called from main
void UI(SDL_Window* window) {
	// set up new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	UI_MenuBar(window);

	simulations[activeSimulation]->runFrame();

	//objects->renderAll();
	simulations[activeSimulation]->objects.renderAll();

	// render user interface
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// procedure to configure style of UI
void UI_ConfigureStyle() {
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.17f, 0.5f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.17f, 0.5f);
}


// procedure to generate menu bar at the top of the screen.
void UI_MenuBar(SDL_Window* window) {
	// static booleans to track UI state
	static bool show_imgui_demo = false;
	static bool show_control_window = true;
	//static bool show_output_window = false;
	//static bool showfs = false;

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
			ImGui::MenuItem("ImGui Demo Window##menubar", NULL, &show_imgui_demo);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();


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

	static ImGuiTabBarFlags tabflags =
		ImGuiTabBarFlags_Reorderable;

	if (ImGui::BeginTabBar("Tabs##input", tabflags)) {
		UI_InputSimulationTab();
		UI_InputObjectsTab(window);
		UI_InputRenderingTab();

		ImGui::EndTabBar();
	}



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

	UI_OutputObjects();

	ImGui::End();
}



//// procedure creates a file dialog and returns a file path
//bool UI_FSReadDialog(SDL_Window* window, std::string& writeback, bool* p_open, const char* extension, bool hideOtherExtensions) {
//	// get imgui io handler reference (static - only needs to be initialised once)
//	static ImGuiIO& io = ImGui::GetIO();
//	// get window size
//	INT_T windowWidth, windowHeight;
//	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
//	// set constraints on window size and position
//	ImGui::SetNextWindowPos(ImVec2((windowWidth - 500) / 2, (windowHeight - 350) / 2), ImGuiCond_Once);
//	ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Once);
//	// flags for window
//	ImGuiWindowFlags windowflags =
//		ImGuiWindowFlags_NoCollapse;
//
//	// create namespace declaration to reduce code
//	namespace fs = std::filesystem;
//
//	// wether the file has been opened
//	bool fileOpened = false;
//	// boolean that tracks whether opening unselected item has been attempted
//	static bool openAttempt = false;
//	// static bool that tracks wether extension is valid
//	static bool invalidExtension = false;
//
//	ImGui::Begin("Open##fs", p_open, windowflags);
//
//	// get current path
//	static fs::path path = fs::current_path();
//	static std::string pathstr = path.string();
//
//	// index of item selected
//	static INT_T selected = -1;
//
//	// button for moving to parent directory
//	if (ImGui::Button("<##fs", ImVec2(25, 0))) {
//		path = path.parent_path();
//		pathstr = path.string();
//		std::cout << pathstr << "\n";
//		// reset selected index
//		selected = -1;
//	}
//
//	ImGui::SameLine();
//	// show current directory
//	ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), pathstr.c_str());
//
//	// seperator before directory contents
//	ImGui::Spacing();
//	ImGui::Separator();
//	ImGui::Spacing();
//
//	static ImGuiSelectableFlags selectflags =
//		ImGuiSelectableFlags_AllowDoubleClick;
//
//	// list directory contents
//	INT_T index = 0;
//	static fs::path selectedpath;
//	// contain within list box
//	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
//	if (ImGui::BeginListBox("##directoryfs", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 98))) {
//		// iterate through items in directory
//		for (const auto& p : fs::directory_iterator(path)) {
//			// use one color if item is directory
//			if (fs::is_directory(p)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
//			// if not directory and not selected extension and hideOtherExtensions skip item
//			else if (hideOtherExtensions && strcmp(extension, p.path().extension().string().c_str()) != 0) continue;
//			// if file use different color
//			else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
//			// create selectable element for directory element
//			if (ImGui::Selectable(p.path().string().erase(0, pathstr.length()).c_str(), selected == index, selectflags)) {
//				// check if left mouse button clicked twice
//				if (ImGui::IsMouseDoubleClicked(0)) {
//					// check if path is directory
//					if (fs::is_directory(p)) {
//						path = p.path();
//						pathstr = path.string();
//					}
//					else {
//						// if file extension is required and extension does not match
//						if (*extension && strcmp(extension, p.path().extension().string().c_str()) != 0) {
//							invalidExtension = true;
//						}
//						else {
//							invalidExtension = false;
//							writeback = p.path().string();
//							// replace backslashes with forward slashes
//							// it appears std::ifstream doesn't like them
//							std::replace(writeback.begin(), writeback.end(), '\\', '/');
//							// close window
//							*p_open = false;
//							fileOpened = true;
//						}
//
//					}
//					// reset selected
//					selected = -1;
//				}
//				else {
//					// select item
//					selected = index;
//					openAttempt = false;
//					if (!fs::is_directory(p)) {
//						selectedpath = p.path();
//					}
//				}
//			}
//			++index;
//			ImGui::PopStyleColor();
//		}
//		ImGui::EndListBox();
//	}
//	ImGui::PopStyleColor();
//
//	// spacing before final buttons
//	ImGui::Spacing();
//
//	// display warning message to the user if applicable
//	// uses else if to only display one message at a time
//	if (openAttempt) {
//		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select a file");
//		ImGui::SameLine();
//	}
//	else if (invalidExtension) {
//		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Extension must be %s", extension);
//		ImGui::SameLine();
//	}
//
//	// position cursor to right for buttons
//	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);
//	
//	// button to open file
//	if (ImGui::Button("Open##fs", ImVec2(60, 0))) {
//		if (selectedpath.empty()) {
//			openAttempt = true;
//		}
//		else {
//			// if file extension is required
//			if (*extension && strcmp(extension, selectedpath.extension().string().c_str()) != 0) {
//				//if (strcmp(extension, selectedpath.extension().string().c_str()) != 0) {
//				invalidExtension = true;
//				}
//			else {
//				invalidExtension = false;
//				writeback = selectedpath.string();
//				std::replace(writeback.begin(), writeback.end(), '\\', '/');
//				*p_open = false;
//				fileOpened = true;
//			}
//		}
//	}
//	
//	// button to cancel opening file
//	ImGui::SameLine();
//	if (ImGui::Button("Cancel##fs", ImVec2(60, 0))) {
//		*p_open = false;
//		openAttempt = false;
//	}
//
//	ImGui::End();
//
//	// if window should close reset some parameters
//	if (!*p_open) {
//		selectedpath.clear();
//		selected = -1;
//	}
//
//	// return true if file found
//	return fileOpened;
//}


void UI_InputSimulationTab() {
	if (ImGui::BeginTabItem("Simulation##inpuc")) {
		Simulation& simulation = *simulations[activeSimulation];

		ImGui::Spacing();

		if (ImGui::Button("Start Simulation##simulation")) {
			simulation.start();
		}
		if (ImGui::Button("Pause Simulation##simulation")) {
			simulation.pause();
		}

		// input function with this flag returns true
		// when enter key is pressed
		static ImGuiInputTextFlags inputflags =
			ImGuiInputTextFlags_EnterReturnsTrue;

		static float timeAcceleration = simulation.getTimeAcceleration();
		if (ImGui::InputFloat("Time Acceleration##simulation", &timeAcceleration, NULL, NULL, "%.1f", inputflags)) {
			simulation.setTimeAcceleration(timeAcceleration);
		}
		// maximum world time for an iteration
		static float maxIterationTime = simulation.getMaxIterationTime();
		if (ImGui::InputFloat("Max Iteration Time (s)##simulation", &maxIterationTime, NULL, NULL, "%.0f", inputflags)) {
			simulation.setMaxIterationTime(maxIterationTime);
		}

		ImGui::EndTabItem();
	}
}


void UI_InputObjectsTab(SDL_Window* window) {
	if (ImGui::BeginTabItem("Objects##input")) {
		Simulation& simulation = *simulations[activeSimulation];

		ImGui::Spacing();
		// general status variable
		bool status;

		static bool show_fs_dialog = false;

		if (ImGui::Button("Open Object##input")) {
			show_fs_dialog = true;
		}

		// timer to track object loading
		static Timer objectLoadingTimer;

		bool objectOpened = false;
		static std::string objectPath;
		if (show_fs_dialog) {
			objectOpened = UI_FSReadDialog(window, objectPath, &show_fs_dialog, ".obj", true);
			if (objectOpened) {
				//objects->newObjectThread(objectPath.c_str());
				simulation.objects.newObjectThread(objectPath.c_str());
				objectLoadingTimer.start();
			}
		}

		// display loading animation if object is loading
		if (simulation.objects.getThreadOpen()) {
			static char buffer[32];
			sprintf(buffer, "Loading%.*s", (int)objectLoadingTimer.getEnlapsedMS() / 300 % 4, "...");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.7f, 1.0f, 0.7f, 1.0f), buffer);
		}

		if (simulations[activeSimulation]->objects.getThreadDone()) {
			UINT_T objectHandle = simulation.objects.joinThread(status);
			if (status) {
				std::filesystem::path path = objectPath;
				std::string objectName = path.stem().string();
				simulation.objects.setName(objectHandle, objectName);
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static ImGuiSelectableFlags selectflags =
			ImGuiSelectableFlags_AllowDoubleClick;

		// keep track of selected item
		static INT_T selected = -1;
		for (auto it = simulation.objects.begin(); it != simulation.objects.end(); ++it) {
			ImVec4 selectableColor;
			// check whether object is visible
			if (simulation.objects.getVisible(*it)) selectableColor = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
			else selectableColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

			static char buffer[32];
			sprintf(buffer, "%s##%d", simulation.objects.getName(*it).c_str(), *it);

			ImGui::PushStyleColor(ImGuiCol_Text, selectableColor);
			if (ImGui::Selectable(buffer, selected == *it, selectflags)) {
				// set selected object
				selected = *it;

				// check mouse is double clicked and that vector does not contain value already
				if (ImGui::IsMouseDoubleClicked(0)) {
					// if objects is not in current tab list show it
					if (!contains(openObjects, *it)) {
						openObjects.push_back(*it);
					}
					// regardless set active tab to object
					output_active_tab = *it;
					output_set_active_tab = true;
					show_output_window = true;
				}
			}
			ImGui::PopStyleColor();
		}

		ImGui::EndTabItem();
	}
}


void UI_InputRenderingTab() {
	if (ImGui::BeginTabItem("Rendering##input")) {
		Simulation& simulation = *simulations[activeSimulation];

		ImGui::Spacing();

		// input function with this flag returns true
		// when enter key is pressed
		static ImGuiInputTextFlags inputflags =
			ImGuiInputTextFlags_EnterReturnsTrue;

		static ImGuiSliderFlags sliderflags =
			ImGuiSliderFlags_AlwaysClamp;

		ImGui::Text("Camera");
		ImGui::Spacing();

		float cameraPosition[3], cameraTarget[3];
		static float cameraFov = simulation.camera.getFov();
		simulation.camera.getPosition(cameraPosition);
		simulation.camera.getTarget(cameraTarget);
		
		// user can input any value for position and target
		if (ImGui::InputFloat3("Position##camera", cameraPosition, "%.2f", inputflags)) {
			simulation.camera.setPosition(cameraPosition);
			simulation.camera.calculateViewMatrix();
		}
		if (ImGui::InputFloat3("Target##camera", cameraTarget, "%.2f", inputflags)) {
			simulation.camera.setTarget(cameraTarget);
			simulation.camera.calculateViewMatrix();
		}
		// value for fov is clamped to between 5 and 135 degrees
		if (ImGui::DragFloat("FOV##camera", &cameraFov, 0.5f, 5.0f, 135.0f, "%.1f", sliderflags)) {
			simulation.camera.setFov(cameraFov);
			simulation.camera.calculateProjectionMatrix();
		}


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static float frameCap = frameTimer->getRateCap();
		// to keep any error messages persistant, checking values must be handled
		// externally
		bool applyFrameCap = false;;
		if (ImGui::InputFloat("Frame Cap##renering", &frameCap, NULL, NULL, "%.1f", inputflags)) {
			applyFrameCap = true;
		}

		// check validity of input
		if (applyFrameCap && frameCap >= 5) {
			frameTimer->setRateCap(frameCap);
			simulation.reportProgramFrameRate(frameCap);
		}
		// display warning messages
		if (frameCap < 0) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Value cannot be negative");
		}
		else if (frameCap < 5) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Frame Cap too low");
		}

		ImGui::EndTabItem();
	}
}


void UI_OutputObjects() {
	static ImGuiTabBarFlags tabflags =
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_TabListPopupButton |
		ImGuiTabBarFlags_FittingPolicyScroll;


	// create tab bar
	if (ImGui::BeginTabBar("Tabs##output", tabflags)) {
		Simulation& simulation = *simulations[activeSimulation];
		// iterate through open objects
		//for (auto it = openObjects.begin(); it != openObjects.end(); ++it) {
		for (INT_T i = 0; i < openObjects.size(); ++i) {
			// create object handle reference
			UINT_T& object = openObjects[i];

			// create tab item
			static char buffer[32];
			sprintf(buffer, "%s##%d", simulation.objects.getName(object).c_str(), object);

			bool p_open = true;

			static ImGuiTabItemFlags itemflags;
			if (output_set_active_tab && openObjects[i] == output_active_tab) {
				itemflags = ImGuiTabItemFlags_SetSelected;
				output_set_active_tab = false;
			}
			else itemflags = 0;

			if (ImGui::BeginTabItem(buffer, &p_open, itemflags)) {
				ImGui::Spacing();
				ImGui::Text("Data Output");
				ImGui::Spacing();

				// flags for numeric inputs
				static ImGuiInputTextFlags inputflags =
					ImGuiInputTextFlags_EnterReturnsTrue;
				static ImGuiSliderFlags sliderflags =
					ImGuiSliderFlags_Logarithmic;

				float objectPosition[3], objectVelocity[3], objectOrientation[3];
				simulation.objects.getPosition(object, objectPosition);
				simulation.objects.getVelocity(object, objectVelocity);
				simulation.objects.getOrientation(object, objectOrientation);

				// position input / output
				if (ImGui::InputFloat3("Position##output", objectPosition, "%.2f", inputflags)) {
					simulation.objects.setPosition(object, objectPosition);
				}

				// velocity input / output
				if (ImGui::InputFloat3("Velocity##output", objectVelocity, "%.2f", inputflags)) {
					simulation.objects.setVelocity(object, objectVelocity);
				}

				// orientation input / output
				if (ImGui::InputFloat3("Orientation##output", objectOrientation, "%.2f", inputflags)) {
					simulation.objects.setOrientation(object, objectOrientation);
				}

				float mass = simulation.objects.getMass(object);
				if (ImGui::InputFloat("Mass / Kg##output", &mass, NULL, NULL, "%.0f", inputflags)) {
					simulation.objects.setMass(object, mass);
				}

				float scale = simulation.objects.getScale(object);
				// change scale of model
				if (ImGui::DragFloat("Scale##output", &scale, 0.6f, 0.001f, 1000.0f, "%.2f", sliderflags)) {
					simulation.objects.setScale(object, scale);
				}

				ImGui::Spacing();
				ImGui::Text("Object Info");
				ImGui::Spacing();

				static ImGuiTableFlags tableflags =
					ImGuiTableFlags_RowBg |
					ImGuiTableFlags_BordersOuter |
					ImGuiTableFlags_BordersV;

				UINT_T nrVertices = simulation.objects.getSize(object);

				if (ImGui::BeginTable("objectinfo##output", 2, tableflags)) {
					UI_TableInt("Vertices", nrVertices);
					//UI_TableFloat("Radius", simulation.objects.getRadius(object));

					ImGui::EndTable();
				}

				ImGui::Spacing();
				ImGui::Text("Object Actions");
				ImGui::Spacing();

				// set wether object is visible
				bool visible = simulation.objects.getVisible(object);
				bool staticvisible = visible;
				ImGui::Checkbox("Visible##output", &visible);
				// just removes one unnecessary function call
				if (visible != staticvisible) simulation.objects.setVisible(object, visible);

				ImGui::Spacing();

				// button to let user completely remove object
				if (ImGui::Button("Delete Object##output")) {
					simulation.objects.deleteObject(openObjects[i]);
					openObjects.erase(openObjects.begin() + i);
					openObjects.shrink_to_fit();
				}

				ImGui::Spacing();
				ImGui::Text("Debug");
				ImGui::Spacing();

				if (ImGui::BeginTable("objectinfo##output", 2, tableflags)) {
					UI_TableInt("Identifier", object);

					ImGui::EndTable();
				}


				ImGui::EndTabItem();
			}

			if (!p_open) {
				openObjects.erase(openObjects.begin() + i);
				openObjects.shrink_to_fit();
			}
		}
		ImGui::EndTabBar();
	}
}


void UI_TableInt(const char* text, INT_T a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%d", a);
}

void UI_TableFloat(const char* text, float a) {
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(text);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("%f", a);
}


// procedure creates a file dialog and returns a file path
bool UI_FSReadDialog(SDL_Window* window, std::string& writeback, bool* p_open, const char* extension, bool hideOtherExtensions) {
	// get imgui io handler reference (static - only needs to be initialised once)
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	ImGui::SetNextWindowPos(ImVec2((windowWidth - 500) / 2, (windowHeight - 350) / 2), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Once);
	// flags for window
	static ImGuiWindowFlags windowflags =
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

	// index of item selected
	static INT_T selected = -1;

	// button for moving to parent directory
	if (ImGui::Button("<##fs", ImVec2(25, 0))) {
		path = path.parent_path();
		pathstr = path.string();
		//std::cout << pathstr << "\n";
		// reset selected index
		selected = -1;
	}

	ImGui::SameLine();
	// show current directory
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), pathstr.c_str());

	float textHeight = ImGui::GetTextLineHeight();

	// seperator before directory contents
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static ImGuiSelectableFlags selectflags =
		ImGuiSelectableFlags_AllowDoubleClick;

	// list directory contents
	INT_T index = 0;
	static fs::path selectedpath;
	// contain within list box
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (ImGui::BeginListBox("##directoryfs", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 72 - 2 * textHeight))) {
		// iterate through items in directory
		for (const auto& p : fs::directory_iterator(path)) {
			// use one color if item is directory
			if (fs::is_directory(p)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
			// if not directory and not selected extension and hideOtherExtensions skip item
			else if (hideOtherExtensions && strcmp(extension, p.path().extension().string().c_str()) != 0) continue;
			// if file use different color
			else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			// create selectable element for directory element
			if (ImGui::Selectable(p.path().string().erase(0, pathstr.length()).c_str(), selected == index, selectflags)) {
				// check if left mouse button clicked twice
				if (ImGui::IsMouseDoubleClicked(0)) {
					// check if path is directory
					if (fs::is_directory(p)) {
						path = p.path();
						pathstr = path.string();
					}
					else {
						// if file extension is required and extension does not match
						if (*extension && strcmp(extension, p.path().extension().string().c_str()) != 0) {
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

	// spacing before final buttons
	ImGui::Spacing();

	// display warning message to the user if applicable
	// uses else if to only display one message at a time
	if (openAttempt) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select a file");
		ImGui::SameLine();
	}
	else if (invalidExtension) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Extension must be %s", extension);
		ImGui::SameLine();
	}

	// position cursor to right for buttons
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);

	// button to open file
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

	// button to cancel opening file
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


// procedure creates a file dialog and returns a file path
bool UI_FSWriteDialog(SDL_Window* window, std::string& writeback, bool* p_open) {
	// get imgui io handler reference (static - only needs to be initialised once)
	static ImGuiIO& io = ImGui::GetIO();
	// get window size
	INT_T windowWidth, windowHeight;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
	// set constraints on window size and position
	ImGui::SetNextWindowPos(ImVec2((windowWidth - 500) / 2, (windowHeight - 350) / 2), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Once);
	// flags for window
	static ImGuiWindowFlags windowflags =
		ImGuiWindowFlags_NoCollapse;

	// create namespace declaration to reduce code
	namespace fs = std::filesystem;

	// whether the file has been opened
	bool fileSaved = false;
	// boolean that tracks whether opening unselected item has been attempted
	static bool saveAttempt = false;

	ImGui::Begin("Save As##fs", p_open, windowflags);

	// get current path
	static fs::path path = fs::current_path();
	static std::string pathstr = path.string();

	// index of item selected
	static INT_T selected = -1;

	// button for moving to parent directory
	if (ImGui::Button("<##fs", ImVec2(25, 0))) {
		path = path.parent_path();
		pathstr = path.string();
		//std::cout << pathstr << "\n";
		// reset selected index
		selected = -1;
	}

	ImGui::SameLine();
	// show current directory
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), pathstr.c_str());

	float textHeight = ImGui::GetTextLineHeight();

	// seperator before directory contents
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static ImGuiSelectableFlags selectflags =
		ImGuiSelectableFlags_AllowDoubleClick;

	// list directory contents
	INT_T index = 0;
	//static fs::path selectedpath;
	// contain within list box
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (ImGui::BeginListBox("##directoryfs", ImVec2(-FLT_MIN, ImGui::GetWindowHeight() - 72 - 2 * textHeight))) {
		// iterate through items in directory
		for (const auto& p : fs::directory_iterator(path)) {
			// use one color if item is directory
			if (fs::is_directory(p)) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
			// if file use different color
			else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
			// create selectable element for directory element
			if (ImGui::Selectable(p.path().string().erase(0, pathstr.length()).c_str(), selected == index, selectflags)) {
				// check if left mouse button clicked twice
				if (ImGui::IsMouseDoubleClicked(0)) {
					// check if path is directory
					if (fs::is_directory(p)) {
						path = p.path();
						pathstr = path.string();
					}
					// reset selected
					selected = -1;
				}
				else {
					// select item
					selected = index;
					saveAttempt = false;
				}
			}
			++index;
			ImGui::PopStyleColor();
		}
		ImGui::EndListBox();
	}
	ImGui::PopStyleColor();

	// spacing before final buttons
	ImGui::Spacing();

	// display warning message to the user if applicable
	// uses else if to only display one message at a time
	if (saveAttempt) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Enter File Name");
		ImGui::SameLine();
	}

	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 145 - ImGui::GetCursorPosX());

	static ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	static char buffer[260];

	//ImGui::InputTextWithHint("##inputfs", "File Name", buffer);

	if (ImGui::InputTextWithHint("##inputfs", "File Name", buffer, 260, inputflags)) {
		if (strlen(buffer) != 0) {
			//selectedpath = path;
			//selectedpath.replace_filename(buffer);
			writeback = path.string();
			writeback.append("/");
			writeback.append(buffer);
			std::replace(writeback.begin(), writeback.end(), '\\', '/');
			fileSaved = true;
			*p_open = false;
		}
		else {
			saveAttempt = true;
		}
	}

	ImGui::SameLine();

	//// position cursor to right for buttons
	//ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 140);

	// button to save file
	if (ImGui::Button("Save##fs", ImVec2(60, 0))) {
		if (strlen(buffer) != 0) {
			//selectedpath = path;
			//selectedpath.replace_filename(buffer);
			writeback = path.string();
			writeback.append("/");
			writeback.append(buffer);
			std::replace(writeback.begin(), writeback.end(), '\\', '/');
			fileSaved = true;
			*p_open = false;
		}
		else {
			saveAttempt = true;
		}
	}

	// button to cancel saving file
	ImGui::SameLine();
	if (ImGui::Button("Cancel##fs", ImVec2(60, 0))) {
		*p_open = false;
		saveAttempt = false;
	}

	ImGui::End();

	// if window should close reset some parameters
	if (!*p_open) {
		selected = -1;
	}

	// return true if file found
	return fileSaved;
}