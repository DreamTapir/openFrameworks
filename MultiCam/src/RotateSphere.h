#pragma once

#include "ofMain.h"

class RotateSphere {
public:
	void setup();
	void update();
	void draw();

	ofMesh face, frame;
};