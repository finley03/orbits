// source file for the user interface
// this file contains user interface code, and input validatoin

#include "ui.h"
#include "main.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"


// main user interface function
// only this has to be called from main
void UI(SDL_Window* window) {
	// set up new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	ImGui::Begin("Test Window");

	ImGuiInputTextFlags inputflags =
		ImGuiInputTextFlags_EnterReturnsTrue;

	static float frameCap = frameTimer->getRateCap();
	if (ImGui::InputFloat("Frame Cap", &frameCap, NULL, NULL, "%.1f", inputflags)) {
		frameTimer->setRateCap(frameCap);
	}

	ImGui::End();

	// render user interface
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}