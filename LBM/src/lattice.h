#pragma once
#include "ofMain.h"

class lattice {
public:
	ofPoint pos; //�ʒu
	ofPoint pos_center; //�Z�����S�ʒu
	ofColor lcolor; //�`��F
	float lwidth; //��
	bool inCircle;

public:
	lattice();
	void draw();
	void update();
};