#pragma once
#define numParticle 1000000

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		//ofxPanel gui;
		//ofxFloatSlider a, b, c, d, e, f, pointSize;

		float a, b, c, d, e, f, g, x, y, z,pointSize;
		float xn, yn, zn;
		float rotate;
		ofVboMesh vboMesh;
		ofColor color;
		ofVec3f points[numParticle];

		ofEasyCam cam;
		ofSoundPlayer sound;
		vector<float> fft_smoothed;
		int n_bands_to_get;
};
