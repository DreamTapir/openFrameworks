#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Particles.h"
#include "RotateSphere.h"
#include "ClusterCube.h"

#define N_CAMERAS 5
#define WIDTH 1920
#define HEIGHT 1080
#define VIEWWINDOW_HEIGHT 192

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void setupFrameworks();
		void setupCameras();
		void setupViewports();
		void setupGui();

		void drawCameras();
		void drawLabels();
		void drawMonitor(int iCameraDraw);
		void drawScene(int iCameraDraw);

		// gui
		ofxDatGui* gui;
		bool mFullscreen;
		void refreshWindow();
		void toggleFullscreen();
		void keyPressed(int key);
		void onButtonEvent(ofxDatGuiButtonEvent e);
		void onToggleEvent(ofxDatGuiToggleEvent e);
		void onSliderEvent(ofxDatGuiSliderEvent e);
		void onTextInputEvent(ofxDatGuiTextInputEvent e);
		void on2dPadEvent(ofxDatGui2dPadEvent e);
		void onDropdownEvent(ofxDatGuiDropdownEvent e);
		void onColorPickerEvent(ofxDatGuiColorPickerEvent e);
		void onMatrixEvent(ofxDatGuiMatrixEvent e);

		int tIndex;
		vector<ofxDatGuiTheme*> themes;

		int deg;
		int fov;
		float nearclip, farclip;
		float aspect;
		ofVec2f lensoffset{0, 0}, Rlensoffset{0, 0}, Blensoffset{0, 0}, Llensoffset{0, 0}, Flensoffset{0, 0};

		// ortho bool
		void RightCamOrtho();
		void BackCamOrtho();
		void LeftCamOrtho();
		void FrontCamOrtho();
		bool RCObool, BCObool, LCObool, FCObool;

		// draw bool
		bool drawScene_bool, particle_bool, rotatesphere_bool, clustercube_bool;

		// camera
		ofEasyCam cam;
		ofCamera camFront, camBack, camRight, camLeft;
		float camPos;
		ofVec3f axis;

		// camera pointers
		ofCamera* cameras[N_CAMERAS];
		int iMainCamera;

		// viewports
		ofRectangle viewMain;
		ofRectangle viewGrid[N_CAMERAS];

		// particles
		Particles particles;

		// rotate sphere
		RotateSphere rotatesphere;

		// cluster cube
		ClusterCube clustercube;
};
