#include "HexagonMesh.h"

HexagonMesh::HexagonMesh() {
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

	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

void HexagonMesh::update() {
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	// HexagonToMesh
	ofSeedRandom(39);

	this->face.clear();
	this->frame.clear();

	auto radius = 15;
	auto x_span = radius * sqrt(3);
	auto flg = true;
	for (float y = -500; y < 500; y += radius * 1.5) {

		for (float x = -500; x < 500; x += x_span) {

			glm::vec2 location;
			if (flg) {

				location = glm::vec2(x, y);
			}
			else {

				location = glm::vec2(x + (x_span / 2), y);
			}

			auto noise_value = ofNoise(glm::vec2(location.x * 0.0036, location.y * 0.001 + ofGetFrameNum() * 0.005));
			auto height = 0.f;
			if (noise_value > 0.75) {

				height = ofMap(noise_value, 0.75, 1, 0, 200);
			}

			this->setHexagonToMesh(this->face, this->frame, glm::vec3(location, 0), radius, height * sound[10] * sound[10]);
		}
		flg = !flg;
	}

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();
	ofBackground(0);
	ofTranslate(512, 128);

	this->cam.begin();
	ofRotateX(295);

	this->face.draw();
	this->frame.draw();

	this->cam.end();

	centerFbo.end();

	//------------------------------------------------------------------------------
	sideFbo.begin();
	ofBackground(0);

	this->cam.begin();
	ofRotateX(295);

	this->face.draw();
	this->frame.draw();

	this->cam.end();

	sideFbo.end();
}

void HexagonMesh::draw() {
	ofClear(0);
	ofBackground(0);

	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);
}

void HexagonMesh::audioIn(ofSoundBuffer& input) {

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
void HexagonMesh::setHexagonToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float radius, float height) {

	ofColor face_color = ofColor(0);
	ofColor frame_color = ofColor::fromHsb(150, 255, 255);
	if (height < 100) {

		//face_color = ofColor(ofMap(height, 0, 100, 255, 0));
		//frame_color = ofColor(ofMap(height, 0, 100, 0, 255));
		face_color = ofColor::fromHsb(120, 255, ofMap(height, 0, 100, 0, 255));
		//frame_color = ofColor::fromHsb(ofMap(height, 150, 100, 0, 170), 255, 255);
	}

	for (int deg = 90; deg < 450; deg += 60) {

		auto face_radius = radius - 0.5;
		auto face_index = face_target.getNumVertices();

		vector<glm::vec3> vertices;
		vertices.push_back(glm::vec3(0, 0, 0));
		vertices.push_back(glm::vec3(face_radius * cos(deg * DEG_TO_RAD), face_radius * sin(deg * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(face_radius * cos((deg + 60) * DEG_TO_RAD), face_radius * sin((deg + 60) * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(0, 0, height));
		vertices.push_back(glm::vec3(face_radius * cos((deg + 60) * DEG_TO_RAD), face_radius * sin((deg + 60) * DEG_TO_RAD), height));
		vertices.push_back(glm::vec3(face_radius * cos(deg * DEG_TO_RAD), face_radius * sin(deg * DEG_TO_RAD), height));

		for (auto& vertex : vertices) {

			vertex = location + glm::vec4(vertex, 0);
		}

		face_target.addVertices(vertices);

		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2);
		face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);
		face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 4);
		face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);

		auto frame_index = frame_target.getNumVertices();

		vertices.clear();
		vertices.push_back(glm::vec3(0, 0, 0));
		vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(radius * cos((deg + 60) * DEG_TO_RAD), radius * sin((deg + 60) * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(0, 0, height));
		vertices.push_back(glm::vec3(radius * cos((deg + 60) * DEG_TO_RAD), radius * sin((deg + 60) * DEG_TO_RAD), height));
		vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), height));

		for (auto& vertex : vertices) {

			vertex = location + glm::vec4(vertex, 0);
		}

		frame_target.addVertices(vertices);

		frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 2);
		frame_target.addIndex(frame_index + 4); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 2); frame_target.addIndex(frame_index + 4);

		for (int i = 0; i < vertices.size(); i++) {

			face_target.addColor(face_color);
			frame_target.addColor(frame_color);
		}
	}
}