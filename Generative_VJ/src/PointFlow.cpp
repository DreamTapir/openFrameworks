#include "PointFlow.h"

PointFlow::PointFlow() {
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

	ofEnableDepthTest();

	this->base_radius = 120;
	this->number_of_satellite = 3;
	for (int i = 0; i < 3; i++) {

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
	this->ringframe.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

void PointFlow::update() {
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

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

			this->frame.addColor(ofColor::fromHsb(120, 255, 100 + sound[10] * sound[10] * 200));
			this->face.addColor(ofColor::fromHsb(140, 255, 100 + sound[20] * sound[20] * 200));

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

	ofSeedRandom(39);

	this->ringface.clear();
	this->ringframe.clear();

	for (int radius = 150; radius <= 300; radius += 150) {

		int theta_start = radius == 150 ? (int)(ofGetFrameNum() * 0.5) % 10 * -1 : (int)(ofGetFrameNum() * 0.5) % 10;
		for (int theta = theta_start; theta < theta_start + 180; theta += 10) {

			for (int phi = 0; phi < 360; phi += 3) {

				auto index = this->ringface.getNumVertices();
				vector<glm::vec3> vertices;

				vertices.push_back(glm::vec3(
					radius * cos(phi * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					radius * sin(phi * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					radius * cos((theta + 1) * DEG_TO_RAD)
				));
				vertices.push_back(glm::vec3(
					radius * cos((phi + 3) * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					radius * sin((phi + 3) * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					radius * cos((theta + 1) * DEG_TO_RAD)
				));
				vertices.push_back(glm::vec3(
					radius * cos((phi + 3) * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					radius * sin((phi + 3) * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					radius * cos((theta - 1) * DEG_TO_RAD)
				));
				vertices.push_back(glm::vec3(
					radius * cos(phi * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					radius * sin(phi * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					radius * cos((theta - 1) * DEG_TO_RAD)
				));

				vertices.push_back(glm::vec3(
					(radius - 30) * cos(phi * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					(radius - 30) * sin(phi * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					radius * cos((theta + 1) * DEG_TO_RAD)
				));
				vertices.push_back(glm::vec3(
					(radius - 30) * cos((phi + 3) * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					(radius - 30) * sin((phi + 3) * DEG_TO_RAD) * sin((theta + 1) * DEG_TO_RAD),
					radius * cos((theta + 1) * DEG_TO_RAD)
				));
				vertices.push_back(glm::vec3(
					(radius - 30) * cos((phi + 3) * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					(radius - 30) * sin((phi + 3) * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					radius * cos((theta - 1) * DEG_TO_RAD)
				));
				vertices.push_back(glm::vec3(
					(radius - 30) * cos(phi * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					(radius - 30) * sin(phi * DEG_TO_RAD) * sin((theta - 1) * DEG_TO_RAD),
					radius * cos((theta - 1) * DEG_TO_RAD)
				));

				this->ringface.addVertices(vertices);
				this->ringframe.addVertices(vertices);

				this->ringface.addIndex(index + 0); this->ringface.addIndex(index + 1); this->ringface.addIndex(index + 2);
				this->ringface.addIndex(index + 0); this->ringface.addIndex(index + 2); this->ringface.addIndex(index + 3);

				this->ringface.addIndex(index + 4); this->ringface.addIndex(index + 5); this->ringface.addIndex(index + 6);
				this->ringface.addIndex(index + 4); this->ringface.addIndex(index + 6); this->ringface.addIndex(index + 7);

				this->ringface.addIndex(index + 0); this->ringface.addIndex(index + 4); this->ringface.addIndex(index + 5);
				this->ringface.addIndex(index + 0); this->ringface.addIndex(index + 5); this->ringface.addIndex(index + 1);

				this->ringface.addIndex(index + 2); this->ringface.addIndex(index + 6); this->ringface.addIndex(index + 7);
				this->ringface.addIndex(index + 2); this->ringface.addIndex(index + 7); this->ringface.addIndex(index + 3);

				this->ringframe.addIndex(index + 0); this->ringframe.addIndex(index + 1);
				this->ringframe.addIndex(index + 2); this->ringframe.addIndex(index + 3);

				this->ringframe.addIndex(index + 4); this->ringframe.addIndex(index + 5);
				this->ringframe.addIndex(index + 6); this->ringframe.addIndex(index + 7);

				for (int i = 0; i < vertices.size(); i++) {

					this->ringface.addColor(radius == 150 ? ofColor(sound[10] * sound[10] * 100) : ofColor(sound[80] * sound[80] * 100));
					this->ringframe.addColor(radius == 150 ? ofColor(sound[60] * sound[60] * 100) : ofColor(sound[100] * sound[100] * 100));
				}

			}
		}
	}

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();

	ofBackground(sound[5]);

	this->cam.begin();

	for (int i = 0; i < this->frame.getNumVertices(); i++) {
		ofDrawSphere(this->frame.getVertex(i), 2);
	}

	this->frame.drawWireframe();
	this->face.draw();

	ofRotateX(270);

	this->ringface.draw();
	this->ringframe.drawWireframe();

	this->cam.end();

	centerFbo.end();

	//------------------------------------------------------------------------------

	sideFbo.begin();
	ofBackground(sound[5]);

	this->cam.begin();

	for (int i = 0; i < this->frame.getNumVertices(); i++) {

		ofDrawSphere(this->frame.getVertex(i), 2);
	}

	this->frame.drawWireframe();
	this->face.draw();

	this->cam.end();

	sideFbo.end();
}

void PointFlow::draw() {
	ofClear(0);
	ofBackground(0);

	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);


}

void PointFlow::audioIn(ofSoundBuffer& input) {

	float curVol = 0.0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {
		sound[i] = input[i] * 1;
	}

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

}