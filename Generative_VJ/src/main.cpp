#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	//ofGLWindowSettings settings;
	////settings.setGLVersion(3,3);
	//ofCreateWindow(settings);

	ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
