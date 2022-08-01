#pragma once

#include "ofMain.h"
#include "pingPongBuffer.h"

class Particles {
	public:
		void setup();
		void update();
		void draw();

		//particles
		ofVboMesh particles;
		ofShader render, updatePos;
		pingPongBuffer pingPong;
		ofVec3f emitterPos, prevEmitterPos;
		int particleNum, texRes;
};