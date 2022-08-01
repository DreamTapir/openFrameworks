#include "CliffordAttractor.h"

CliffordAttractor::CliffordAttractor() {
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

	ofSetLineWidth(4);

	centerFbo.allocate(896, 256);
	sideFbo.allocate(128, 384);

	// setup satellite
	this->base_radius = 500;
	this->number_of_satellite = 4;
	for (int i = 0; i < 4; i++) {

		this->base_noise_seed_list.push_back(glm::vec3(ofRandom(1000), ofRandom(1000), ofRandom(1000)));

		auto s_location_list = vector<glm::vec3>();
		auto s_noise_seed_list = vector<glm::vec2>();

		for (int k = 0; k < this->number_of_satellite; k++) {

			s_location_list.push_back(glm::normalize(glm::vec3(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1))) * this->base_radius * 1.2);
			s_noise_seed_list.push_back(glm::vec2(ofRandom(1000), ofRandom(1000)));

		}

		this->satellite_location_list.push_back(s_location_list);
		this->satellite_noise_seed_list.push_back(s_noise_seed_list);
	}

	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

	// Setup box positions
	for (unsigned i = 0; i < NUM_BOXES; ++i)
	{
		posns.push_back(ofVec3f(ofRandom(-500, 500), ofRandom(-150, 150), ofRandom(-300, 300)));
		cols.push_back(ofColor(255, 255, 255, 255));
	}

	// Setup light
	light.setPosition(1000, 1000, 2000);

	cam.setDistance(1300);

	a = 0;
	b = 0;
	c = 0;
	d = 0;
	e = 100;
	f = 80;
	x = 0;
	y = 0;
	pointSize = 1.0;
}

void CliffordAttractor::update() {
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	// Satelitte Update -----------------------------------------------------------
	for (int i = 0; i < this->base_noise_seed_list.size(); i++) {

		auto base_location = glm::vec3(
			ofMap(ofNoise(this->base_noise_seed_list[i].x, (ofGetFrameNum() + 0) * 0.005), 0, 1, -300, 300),
			ofMap(ofNoise(this->base_noise_seed_list[i].y, (ofGetFrameNum() + 0) * 0.005), 0, 1, -300, 300),
			ofMap(ofNoise(this->base_noise_seed_list[i].z, (ofGetFrameNum() + 0) * 0.005), 0, 1, -300, 300)
		);

		for (int k = 0; k < this->number_of_satellite; k++) {

			auto rotation_y = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].x, (ofGetFrameNum() + 0) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(0, 1, 0));
			auto rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].y, (ofGetFrameNum() + 0) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(1, 0, 0));
			auto satellite_location = glm::vec4(this->satellite_location_list[i][k], 0) * rotation_y * rotation_x;

			auto next_base_location = glm::vec3(
				ofMap(ofNoise(this->base_noise_seed_list[i].x, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300),
				ofMap(ofNoise(this->base_noise_seed_list[i].y, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300),
				ofMap(ofNoise(this->base_noise_seed_list[i].z, (ofGetFrameNum() + 1) * 0.005), 0, 1, -300, 300)
			);

			auto next_rotation_y = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].x, (ofGetFrameNum() + 1) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(0, 1, 0));
			auto next_rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(this->satellite_noise_seed_list[i][k].y, (ofGetFrameNum() + 1) * 0.005), 0, 1, -PI * 2, PI * 2), glm::vec3(1, 0, 0));
			auto next_satellite_location = glm::vec4(this->satellite_location_list[i][k], 0) * rotation_y * rotation_x;

			auto location = base_location + satellite_location;
			auto next_location = next_base_location + next_satellite_location;
			auto distance = location - next_location;

			auto future = glm::vec3(satellite_location) + distance * 80;
			auto random_deg_1 = ofRandom(360);
			auto random_deg_2 = ofRandom(360);
			future += glm::vec3(
				30 * cos(random_deg_1 * DEG_TO_RAD) * sin(random_deg_2 * DEG_TO_RAD),
				30 * sin(random_deg_1 * DEG_TO_RAD) * sin(random_deg_2 * DEG_TO_RAD),
				30 * cos(random_deg_2 * DEG_TO_RAD)
			);
			auto future_distance = future - satellite_location;

			this->frame.addVertex(location);
			this->face.addVertex(location);

			this->frame.addColor(ofColor(255));
			this->face.addColor(ofColor(255, 64));

			this->velocity_list.push_back(glm::normalize(future_distance) * glm::length(distance));
		}
	}

	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		this->frame.setVertex(i, this->frame.getVertex(i) + this->velocity_list[i]);
		this->face.setVertex(i, this->face.getVertex(i) + this->velocity_list[i]);
	}

	for (int i = this->velocity_list.size() - 1; i >= 0; i--) {

		if (glm::length(this->frame.getVertex(i)) > 720) {

			this->velocity_list.erase(this->velocity_list.begin() + i);
			this->frame.removeVertex(i);
			this->face.removeVertex(i);
		}
	}

	this->frame.clearIndices();
	this->face.clearIndices();
	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		vector<int> near_index_list;
		for (int k = i + 1; k < this->frame.getNumVertices(); k++) {

			auto distance = glm::distance(this->frame.getVertex(i), this->frame.getVertex(k));
			if (distance < 50) {

				this->frame.addIndex(i); this->frame.addIndex(k);
			}

			if (distance < 80) {

				near_index_list.push_back(k);
			}
		}

		if (near_index_list.size() > 3) {

			for (int k = 0; k < near_index_list.size() - 2; k++) {

				this->face.addIndex(i);
				this->face.addIndex(near_index_list[k]);
				this->face.addIndex(near_index_list[k + 1]);
			}
		}
	}
	//------------------------------------------------------------------------------

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();
	ofTranslate(448, 128);

	ofBackground(10);

	// setup gl state
	ofEnableDepthTest();
	light.enable();

	// draw cliffordattractor
	ofPushMatrix();
	vboMesh.clear();

	ofRotate(sound[10] * sound[10] * 100);

	float x = 0.0;
	float y = 0.0;
	float z = 1.0;

	for (int i = 0; i < numParticle; i++) {

		xn = sin(sound[20] * sound[20] * 1.8 * y) + sound[100] * sound[100] * 1.8 * cos(sound[22] * sound[2] * 1.8 * x);
		yn = sin(sound[62] * sound[62] * 1.8 * x) + sound[15] * sound[15] * 1.8 * cos(sound[90] * sound[90] * 1.8 * y);

		x = xn;
		y = yn;

		points[i].x = x * e;
		points[i].y = y * f;

		vboMesh.addVertex(ofVec3f(points[i].x, points[i].y, 0.0));
		vboMesh.addColor(color);
	}

	glPointSize(pointSize);
	vboMesh.setMode(OF_PRIMITIVE_POINTS);
	vboMesh.draw();

	ofPopMatrix();

	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		ofDrawSphere(this->frame.getVertex(i), 2);
	}

	this->frame.drawWireframe();
	this->face.draw();

	// draw spheres
	for (unsigned i = 0; i < posns.size(); ++i)
	{
		ofRotateX(ofGetFrameNum() * 0.25);
		ofRotateY(ofGetFrameNum() * 0.4);

		ofSetColor(cols[i]);
		ofPushMatrix();
		ofTranslate(posns[i]);
		boxMesh = ofMesh::sphere(sound[i] * sound[i] * 25, 10);
		boxMesh.drawWireframe();
		ofPopMatrix();
	}

	centerFbo.end();

	//------------------------------------------------------------------------------

	ofSeedRandom(39);
	sideFbo.begin();

	ofBackground(10);

	this->cam.begin();
	ofRotateY(ofGetFrameNum() * 0.45);

	ofNoFill();
	auto len = 150 + sound[10] * sound[10] * 150;
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

	this->cam.end();

	sideFbo.end();
}

void CliffordAttractor::draw() {
	ofClear(0);
	ofBackground(0);

	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);
}

void CliffordAttractor::audioIn(ofSoundBuffer& input) {

	float curVol = 0.0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {
		sound[i] = input[i] * 1;
	}

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

}