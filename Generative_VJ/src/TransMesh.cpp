#include "TransMesh.h"

TransMesh::TransMesh() {
	ofSetFrameRate(60);
	ofBackground(0);

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

	cam.setDistance(100);

	ofPlanePrimitive plane;
	plane.set(1000, 1000, 50, 50);

	mesh = plane.getMesh();

	// Setup box positions
	for (unsigned i = 0; i < NUM_BOXES; ++i)
	{
		posns.push_back(ofVec3f(ofRandom(-500, 500), ofRandom(-150, 150), ofRandom(-300, 300)));
		cols.push_back(ofColor(255, 255, 255, 255));
	}
}

void TransMesh::update() {
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	float div = 400.0;
	float scale = 300.0;
	float speed = 0.25;

	for (int i = 0; i < mesh.getVertices().size(); i++) {
		glm::vec3 pos = mesh.getVertices()[i];
		pos.z = ofNoise(pos.x / div, pos.y / div, ofGetElapsedTimef()* speed + sound[10] * sound[10] * speed) * scale;
		//pos.z = sound[i] * 50;
		mesh.setVertex(i, pos);
	}

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();

	ofBackground(sound[5]);

	cam.begin();
	ofTranslate(0, -256);
	ofRotateXDeg(-60);

	mesh.drawWireframe();
	cam.end();

	centerFbo.end();

	//------------------------------------------------------------------------------

	ofSeedRandom(39);
	sideFbo.begin();
	ofBackground(sound[5]);

	this->cam.begin();
	ofRotateY(ofGetFrameNum() * 0.45);

	ofNoFill();
	auto len = 150 + sound[5] * sound[5] * 70;
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

				ofSetColor(255, radius > 25 ? ofMap(radius, 25, 50, 255, 0) : 255);

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

	ofFill();
	ofSetColor(0);

	ofDrawBox(len * 0.75);

	cam.end();

	sideFbo.end();
}

void TransMesh::draw() {
	ofClear(0);
	ofBackground(0);

	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);

	
}

void TransMesh::audioIn(ofSoundBuffer& input) {

	float curVol = 0.0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {
		sound[i] = input[i] * 1;
	}

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

}