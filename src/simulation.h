// file that declares class structure for a simulation

#ifndef SIMULATION_H
#define SIMULATION_H

#include "util.h"
#include "objects.h"
#include "camera.h"
#include "time.h"
#include "csys.h"
#include "grid.h"

// simulation class controls objects, rendering, and movement
class Simulation {
private:
	// used to check init of Objects class
	bool objectsStatus;

	// handle to grid object
	UINT_T gridHandle = 0;

	// rate of time in simulation relative to real time.
	float timeAcceleration = 10000;
	// maximum world time of iteration in seconds
	float maxIterationTime = 100;

	float programFrameRate = 30;

	float simulationRate = 30;

	//// thread assets
	//std::thread simulationThread;
	//bool simulationRun = false;

	// is simulation paused
	bool paused = true;

	void calcSimulationRate();
public:
	Camera camera;
	Objects objects;
	//Grid grid;
	//Csys csys;

	// constructor
	Simulation();
	// destructor
	~Simulation();

	// run method
	void run(float timeEnlapsed, float** velocities);
	void runFrame();

	// simulation run methods
	void start();
	void pause();
	void reset();

	void reportProgramFrameRate(float value);

	// time acceleration methods
	void setTimeAcceleration(float value);
	float getTimeAcceleration();
	void setMaxIterationTime(float seconds);
	float getMaxIterationTime();

	void setGridHandle(UINT_T handle);
	UINT_T getGridHandle();
};

#endif