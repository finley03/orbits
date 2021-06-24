// source file for time class

#include "time.h"
#include <SDL.h>

void Timer::start() {
	startTime = clock::now();
}

void Timer::reset() {
	startTime = {};
}

void Timer::setRateCap(float cap) {
	rateCap = cap;
}

float Timer::getRateCap() {
	return rateCap;
}

Timer::clock::duration Timer::getEnlapsedDuration() {
	// calculates difference between current time and previous recorded time
	clock::duration enlapsedTime = clock::now() - startTime;
	return enlapsedTime;
}

float Timer::getEnlapsed() {
	// basically just a fancy cast
	return std::chrono::duration<float>(getEnlapsedDuration()).count();
}

float Timer::getEnlapsedMS() {
	return std::chrono::duration<float>(getEnlapsedDuration()).count() * 1000;
}

void Timer::delay() {
	// get current enlapsed time
	// delay function works in ms, therefore
	// I will work in ms
	float enlapsedTime = getEnlapsedMS();
	// if enlapsed time is less than target time
	// call delay function
	if (enlapsedTime < 1000.0f / rateCap) {
		SDL_Delay(static_cast<uint32_t>((1000.0f / rateCap) - enlapsedTime));
	}
	reset();
	start();
}