#pragma once
#define BUFFER_SIZE 256

#include "ofMain.h"
#include "ofxProcessFFT.h"
#include "ofxSandLine.h"
#include "ofxBox2d.h"
#include "CustomRect.h"

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
		
		void audioIn(ofSoundBuffer& input);

		ofEasyCam cam;

		ofFbo			centerFbo, sideFbo;

		int base_radius;
		int number_of_satellite;

		vector<glm::vec3> base_noise_seed_list;
		vector<vector<glm::vec3>> satellite_location_list;
		vector<vector<glm::vec2>> satellite_noise_seed_list;
		vector<glm::vec3> location_list;
		vector<glm::vec3> velocity_list;

		ofMesh face, frame;

		// scene stuff
		ofLight light;

		// AudioInput
		float smoothedVol;
		float scaledVol;
		vector <float> sound;

		ofSoundStream soundStream;

		ofxSandLine spline;

		ofxBox2d box2d;
		vector <shared_ptr<CustomRect> > particles;
		ofxBox2dRect dac;
		ofImage particleImage;
		ofImage dacImage;

		float dist;
};
