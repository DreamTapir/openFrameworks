#pragma once
#include "ofxBox2d.h"

class CustomRect : public ofxBox2dRect {
public:
	CustomRect(int _num);
	void update();

	int num;
	float lfo, radius;
};