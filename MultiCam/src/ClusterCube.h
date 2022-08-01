#pragma once

#include "ofMain.h"

class ClusterCube {
public:
	void setup();
	void update();
	void draw();

	vector<glm::vec3> base_location_list;

	vector<tuple<ofColor, glm::vec3, float>> sphere_list; // BodyColor, Location, size
	int number_of_sphere;
};