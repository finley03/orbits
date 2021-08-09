// simulation algorithm source file

#include "simulation.h"
extern "C" {
#include "mat.h"
}


Simulation::Simulation() : camera(), objects(this, objectsStatus) {
	// check objects class initialized
	if (!objectsStatus) return;

	// init camera to useful position
	float position[3] = { 5.0f, 5.0f, 10.0f };
	camera.setPosition(position);
	camera.calculateProjectionMatrix();
	camera.calculateViewMatrix();

	calcSimulationRate();
}

Simulation::~Simulation() {
	//joinRunThread();
}


void Simulation::run(float timeEnlapsed, float** velocities) {
	if (!paused) {
		INT_T index = 0;
		for (auto it = objects.begin(); it != objects.end(); ++it) {
			// allocate memory
			//velocities.push_back((float*)malloc(3 * sizeof(float)));

			//float velocity[3];
			objects.getVelocity(*it, velocities[index]);

			//float mass = objects.getMass(*it);
			float position[3];
			objects.getPosition(*it, position);

			// for each object, evaluate all other objects
			for (auto it2 = objects.begin(); it2 != objects.end(); ++it2) {
				// skip evaluating self
				if (it == it2) continue;

				float mass2 = objects.getMass(*it2);
				float position2[3];
				objects.getPosition(*it2, position2);

				// vector from position to position2
				float distanceVec[3];
				mat_subtract(position2, position, 3, distanceVec);

				float distance = vec_3_length(distanceVec);

				// get unit direction vector
				float directionVec[3];
				vec_3_normalize(distanceVec, directionVec);

				// change in velocity
				float deltaV[3];
				mat_scalar_product(directionVec, GRAVITATION_CONSTANT * mass2 / (distance * distance) * timeEnlapsed * timeAcceleration, 3, deltaV);

				// add velocity of iteration
				mat_add(velocities[index], deltaV, 3, velocities[index]);
			}
			++index;
		}

		index = 0;
		for (auto it = objects.begin(); it != objects.end(); ++it) {
			// change in position
			float deltaS[3];
			mat_scalar_product(velocities[index], timeEnlapsed * timeAcceleration, 3, deltaS);

			float position[3];
			objects.getPosition(*it, position);
			mat_add(position, deltaS, 3, position);

			objects.setPosition(*it, position);
			objects.setVelocity(*it, velocities[index]);

			++index;
		}
	}
}


void Simulation::runFrame() {
	INT_T nrIterations = ceilf(simulationRate / programFrameRate);

	// fast allocate memory
	// only two malloc() calls
	INT_T nrObjects = objects.size();
	float** velocities = (float**)malloc(nrObjects * sizeof(float*));
	float* velocitiesBlock = (float*)malloc(nrObjects * 3 * sizeof(float));
	for (INT_T i = 0; i < nrObjects; ++i) velocities[i] = velocitiesBlock + i * 3;
	for (INT_T i = 0; i < nrIterations; ++i) run(1 / simulationRate, velocities);
	free(velocities);
	free(velocitiesBlock);
}

void Simulation::calcSimulationRate() {
	simulationRate = MAX_2(timeAcceleration / maxIterationTime, programFrameRate);
}


void Simulation::start() {
	paused = false;
}

void Simulation::pause() {
	paused = true;
}

void Simulation::reset() {
	paused = true;
}


void Simulation::reportProgramFrameRate(float rate) {
	programFrameRate = rate;
}


void Simulation::setTimeAcceleration(float value) {
	timeAcceleration = value;
	calcSimulationRate();
}

float Simulation::getTimeAcceleration() {
	return timeAcceleration;
}

void Simulation::setMaxIterationTime(float seconds) {
	maxIterationTime = seconds;
	calcSimulationRate();
}

float Simulation::getMaxIterationTime() {
	return maxIterationTime;
}