#pragma once

#include "ofMain.h"
#include "SceneManager.cpp"
#include "DoItLive_VJ_SceneManager.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);

		ofEasyCam cam;
		
		DoItLive_VJ_SceneManager manager;
};
