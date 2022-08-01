#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(false);
	ofSetFrameRate(60);
	ofBackground(0);

	box2d.init();
	box2d.setGravity(0, 0);
	box2d.setFPS(30.0);

	particleImage.load("emitter.png");
	dacImage.load("particle.png");

	dac.fixture.filter.groupIndex = -1;
	dac.setup(box2d.getWorld(), 448, 128, 500, 1);

	for (int i = 0; i < 80; i++) {
		auto rect = make_shared<CustomRect>(particles.size());
		rect->setPhysics(150.0, 0.1, 0.95);
		rect->fixture.filter.groupIndex = -1;
		rect->setup(box2d.getWorld(), ofRandom(20, 876), ofRandom(10, 178), ofRandom(10, 490), 10);
		particles.push_back(rect);

		float dacRadius = sin(ofGetElapsedTimef() * 0.01) * 8.0 + 4.0;
		ofVec2f offset(dacRadius, dacRadius);
		dacImage.draw(ofRandom(20, 876), ofRandom(20, 876));
	}

	// AudioInput
	sound.assign(BUFFER_SIZE, 0.0);
	smoothedVol = 0.0;
	scaledVol = 0.0;

	ofSoundStreamSettings settings;

	// or by name
	auto devices = soundStream.getMatchingDevices("default");
	if (!devices.empty()) {
		settings.setInDevice(devices[0]);
	}

	settings.setInListener(this);
	settings.sampleRate = 44100;
	settings.numOutputChannels = 0;
	settings.numInputChannels = 2;
	settings.bufferSize = BUFFER_SIZE;
	soundStream.setup(settings);

	centerFbo.allocate(896, 256);
	sideFbo.allocate(128, 384);

	ofPoint start(0, 128);
	ofPoint end(896, 128);
	ofPoint control1(896 / 3, 256 / 3);
	ofPoint control2(896 - 896 / 3, 256 - 256 / 3);

	spline = ofxSandLine(start, control1, control2, end);
	spline.setMaxSize(5);
	spline.setOffset(0);
	spline.setColor(ofColor(255, 255, 255));
}

//--------------------------------------------------------------
void ofApp::update(){
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	ofPoint start(-50, 128);
	ofPoint end(946, 128);
	ofPoint control1(448 * sin(ofGetFrameNum() * 0.005) + (3 + sound[10] * 50) / 3, (3 + sound[20] * 50) / 3);
	ofPoint control2(896 - 448 * sin(ofGetFrameNum() * 0.005) + (3 + sound[30] * 50) / 3, 256 - (3 + sound[40] * 50) / 3);

	spline.setColor(ofColor::fromHsb(170 + sound[50] * 50, 255, 255));
	spline = ofxSandLine(start, control1, control2, end);

	box2d.update();

	for (int i = 0; i < particles.size(); i++) {
		particles[i]->update();
		particles[i]->addAttractionPoint(particles[i]->getPosition(), 0.1);

		for (int j = i + 1; j < particles.size(); j++) {
			particles[j]->addAttractionPoint(particles[i]->getPosition(), 0.001);
		}
	}

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();

	ofBackgroundGradient(ofColor(70, 0, 60), ofColor(20, 0, 30), OF_GRADIENT_LINEAR);

	for (int i = 0; i < particles.size(); i++) {
		ofVec2f p1(particles[i]->getPosition());

		for (int j = i + 1; j < particles.size(); j++) {
			ofVec2f p2(particles[j]->getPosition());
			dist = p2.distance(p1);

			if (dist < 50) {
				int col = (50 - dist);
				ofSetColor(255, 255, 255, col);
				ofDrawLine(p1.x, p1.y, p2.x, p2.y);
			}
		}
	}

	ofSetColor(255, 255, 255);
	for (int i = 0; i < particles.size(); i++) {
		float radius = particles[i]->radius;
		particleImage.draw(particles[i]->getPosition() - radius / 2.0, radius, radius);
	}

	spline.draw(10000);

	centerFbo.end();

	//------------------------------------------------------------------------------

	ofSeedRandom(39);
	sideFbo.begin();

	ofBackgroundGradient(ofColor(70, 0, 60), ofColor(20, 0, 30), OF_GRADIENT_LINEAR);

	this->cam.begin();
	ofRotateY(ofGetFrameNum() * 0.45);

	ofNoFill();
	auto len =1500 + sound[10] * sound[10] * 200;
	for (int i = 0; i < 8; i++) {

		if (i < 4) {

			ofRotateX(90);
		}
		else if (i < 5) {

			ofRotateY(90);
		}
		else {

			ofRotateY(180);
		}

		for (int k = 0; k < 30; k++) {

			auto location = glm::vec3(ofRandom(-len * 0.5, len * 0.5), ofRandom(-len * 0.5, len * 0.5), len * 0.5);
			auto radius = (int)(ofRandom(1000) + ofGetFrameNum()) % 150 - 100;

			if (radius < 0) {

				auto z = ofMap(radius, -100, 0, len * 10, 0);
				ofDrawLine(location + glm::vec3(0, 0, z), location + glm::vec3(0, 0, z + 50));
			}
			else {

				//ofSetColor(255, radius > 25 ? ofMap(radius, 25, 50, 255, 0) : 255);
				ofSetColor(ofColor::fromHsb(30 - sound[20] * sound[20] * 50, 255, radius > 25 ? ofMap(radius, 25, 50, 255, 0) : 255, 200));

				ofBeginShape();
				for (int deg = 0; deg < 360; deg++) {

					auto vertex = location + glm::vec2(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD));

					if (vertex.y > len * 0.5) { vertex.y = len * 0.5; }
					if (vertex.y < -len * 0.5) { vertex.y = -len * 0.5; }
					if (vertex.x > len * 0.5) { vertex.x = len * 0.5; }
					if (vertex.x < -len * 0.5) { vertex.x = -len * 0.5; }

					ofVertex(vertex);
				}
				ofEndShape(true);
			}
		}
	}

	//ofNoFill();
	//ofSetColor(20, 0, 30);

	//ofDrawBox(len * 0.75);

	this->cam.end();

	sideFbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);
}

void ofApp::audioIn(ofSoundBuffer& input) {

	float curVol = 0.0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {
		sound[i] = input[i] * 1;
	}

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
