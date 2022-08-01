#pragma once

#include "ofMain.h"
#include "ofxFft.h"

#define BUFFER_SIZE 1024

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void audioReceived(float* input, int bufferSize, int nChannels);
		void peak();

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

		glm::vec3 make_point(float R, float r, float u, float v);

		ofEasyCam cam;
		ofMesh face, line;

		ofxFft* fft;
		float* audioInput;
		float* fftOutput;

		float color;
};