#pragma once

#include <ctime>
#include <cstdlib>

void seedRNG() {
	srand(time(nullptr));
}

int randomFromRange(int min, int max) {
	int delta = max - min;
	return min + (rand() % delta);
}

float randomFloat() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float randomFloat(float min, float max) {
	float delta = max - min;
	return min + (randomFloat() * delta);
}
