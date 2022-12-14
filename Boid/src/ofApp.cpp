#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    for (int i = 0; i < BOIDS; i++) {
        boid[i] = new boids(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), ofRandom(-10, 10), ofRandom(-10, 10), boid, BOIDS);
    }
    ofSetFrameRate(25);
}

//--------------------------------------------------------------
void ofApp::update(){
    for (int i = 0; i < BOIDS; i++) {
        boid[i]->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255, 255, 255);
    for (int i = 0; i < BOIDS; i++) {
        boid[i]->draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    for (int i = 0; i < BOIDS; i++) {
        boid[i]->boo();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
