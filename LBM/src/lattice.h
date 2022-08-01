#pragma once
#include "ofMain.h"

class lattice {
public:
	ofPoint pos; //位置
	ofPoint pos_center; //セル中心位置
	ofColor lcolor; //描画色
	float lwidth; //幅
	bool inCircle;

public:
	lattice();
	void draw();
	void update();
};