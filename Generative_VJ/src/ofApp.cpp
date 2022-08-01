#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	manager.setup();

	manager.Fbo.allocate(1920, 1080);
	manager.Glitch.setup(&manager.Fbo);

	// Setup post-processing chain
	//manager.post.init(ofGetWidth(), ofGetHeight());
	manager.post.createPass<FxaaPass>()->setEnabled(false);
	manager.post.createPass<BloomPass>()->setEnabled(false);
	manager.post.createPass<DofPass>()->setEnabled(false);
	manager.post.createPass<KaleidoscopePass>()->setEnabled(false);
	manager.post.createPass<NoiseWarpPass>()->setEnabled(false);
	manager.post.createPass<PixelatePass>()->setEnabled(false);
	manager.post.createPass<EdgePass>()->setEnabled(false);
	manager.post.createPass<VerticalTiltShifPass>()->setEnabled(false);
	manager.post.createPass<GodRaysPass>()->setEnabled(false);

	// Setup light
	manager.light.setPosition(1000, 1000, 2000);
	manager.cam.setDistance(1000);
}

//--------------------------------------------------------------
void ofApp::update(){
	manager.update();

	manager.Fbo.begin();
	ofEnableDepthTest();
	manager.light.enable();
	//manager.post.begin(manager.cam);
	manager.draw();
	//manager.post.end();
	manager.Fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	manager.Glitch.generateFx();
	manager.Fbo.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	manager.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	manager.keyReleased(key);
}