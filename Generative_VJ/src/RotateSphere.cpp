#include "RotateSphere.h"

RotateSphere::RotateSphere() {
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

	ofSetLineWidth(2);
	ofEnableDepthTest();

	centerFbo.allocate(896, 256);
	sideFbo.allocate(128, 384);
	
	cam.setDistance(100);

	// WarpSphere
	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

	// TriangleSphere
	auto ico_sphere = ofIcoSpherePrimitive(250, 3);
	this->triangle_list.insert(this->triangle_list.end(), ico_sphere.getMesh().getUniqueFaces().begin(), ico_sphere.getMesh().getUniqueFaces().end());
}

void RotateSphere::update() {
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	// WarpSphere
	ofSeedRandom(39);

	this->face.clear();
	this->frame.clear();

	for (int radius = 300; radius <= 300; radius += 150) {

		auto noise_seed = ofRandom(1500);
		for (int theta = 0; theta <= 180; theta += 15) {

			for (int phi = 0; phi < 360; phi += 3) {

				auto index = this->face.getNumVertices();
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

				for (auto& vertex : vertices) {

					auto rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(glm::vec2(noise_seed, vertex.x * 0.00008 + ofGetFrameNum() * 0.0005)), 0, 1, -180, 180) * (float)DEG_TO_RAD, glm::vec3(1, 0, 0));
					vertex = glm::vec4(vertex, 0) * rotation_x;

				}

				this->face.addVertices(vertices);
				this->frame.addVertices(vertices);

				this->face.addIndex(index + 0); this->face.addIndex(index + 1); this->face.addIndex(index + 2);
				this->face.addIndex(index + 0); this->face.addIndex(index + 2); this->face.addIndex(index + 3);

				this->face.addIndex(index + 4); this->face.addIndex(index + 5); this->face.addIndex(index + 6);
				this->face.addIndex(index + 4); this->face.addIndex(index + 6); this->face.addIndex(index + 7);

				this->face.addIndex(index + 0); this->face.addIndex(index + 4); this->face.addIndex(index + 5);
				this->face.addIndex(index + 0); this->face.addIndex(index + 5); this->face.addIndex(index + 1);

				this->face.addIndex(index + 2); this->face.addIndex(index + 6); this->face.addIndex(index + 7);
				this->face.addIndex(index + 2); this->face.addIndex(index + 7); this->face.addIndex(index + 3);

				this->frame.addIndex(index + 0); this->frame.addIndex(index + 1);
				this->frame.addIndex(index + 2); this->frame.addIndex(index + 3);

				this->frame.addIndex(index + 4); this->frame.addIndex(index + 5);
				this->frame.addIndex(index + 6); this->frame.addIndex(index + 7);

				for (int i = 0; i < vertices.size(); i++) {

					if (radius == 300) {

						this->face.addColor(ofColor(sound[10] * sound[10] * 100));
						this->frame.addColor(ofColor(255 - (sound[10] * sound[10] * 100)));
					}
					else {

						this->face.addColor(ofColor(255 - (sound[10] * sound[10] * 100)));
						this->frame.addColor(ofColor(sound[10] * sound[10] * 100));
					}
				}

			}
		}
	}

	// TriangleSphere
	ofSeedRandom(39);

	this->mesh.clear();
	this->meshframe.clear();

	int radius = 50 + sound[60] * 10;

	for (int i = 0; i < this->triangle_list.size(); i++) {

		glm::vec3 avg = (this->triangle_list[i].getVertex(0) + this->triangle_list[i].getVertex(1) + this->triangle_list[i].getVertex(2)) / 3;
		auto noise_value = ofNoise(x + avg.x * 0.0025 + ofGetFrameNum() * 0.003 + sound[5] * sound[5] * 0.1, (y + avg.y) * 0.0025 + sound[25] * sound[25] * 0.1, avg.z * 0.0025);

		if (noise_value < 0.45) { noise_value = 0; }
		else if (noise_value > 0.55) { noise_value = 1; }
		else { noise_value = ofMap(noise_value, 0.45, 0.55, 0, 1); }

		vector<glm::vec3> vertices;

		vertices.push_back(glm::normalize(this->triangle_list[i].getVertex(0)) * (radius + 5) - avg);
		vertices.push_back(glm::normalize(this->triangle_list[i].getVertex(1)) * (radius + 5) - avg);
		vertices.push_back(glm::normalize(this->triangle_list[i].getVertex(2)) * (radius + 5) - avg);

		vertices.push_back(glm::normalize(this->triangle_list[i].getVertex(0)) * (radius - 5) - avg);
		vertices.push_back(glm::normalize(this->triangle_list[i].getVertex(1)) * (radius - 5) - avg);
		vertices.push_back(glm::normalize(this->triangle_list[i].getVertex(2)) * (radius - 5) - avg);

		for (auto& vertex : vertices) {

			vertex *= noise_value;
			vertex += avg;

			auto rotation_x = glm::rotate(glm::mat4(), ofMap(ofNoise(glm::vec2(y * 0.007 + sound[5] * sound[5] * 0.1, (x + vertex.x) * 0.0007 + ofGetFrameNum() * 0.0035 + sound[5] * sound[5] * 0.1)), 0, 1, -180, 180) * (float)DEG_TO_RAD, glm::vec3(1, 0, 0));
			vertex = glm::vec4(vertex, 0) * rotation_x + glm::vec4(x, y, 0, 0);
		}

		this->mesh.addVertices(vertices);
		this->meshframe.addVertices(vertices);

		this->mesh.addColor(ofColor::fromHsb(120 + sound[10] * sound[10] * 10, 255, sound[10] * sound[10] * 100));
		this->mesh.addColor(ofColor::fromHsb(130 + sound[15] * sound[15] * 10, 255, sound[15] * sound[15] * 100));
		this->mesh.addColor(ofColor::fromHsb(140 + sound[20] * sound[20] * 10, 255, sound[20] * sound[20] * 100));

		this->mesh.addColor(ofColor(sound[80] * sound[80] * 50));
		this->mesh.addColor(ofColor(sound[80] * sound[80] * 50));
		this->mesh.addColor(ofColor(sound[80] * sound[80] * 50));

		this->meshframe.addColor(ofColor::fromHsb(170, 255, 255));
		this->meshframe.addColor(ofColor::fromHsb(180, 255, 255));
		this->meshframe.addColor(ofColor::fromHsb(190, 255, 255));

		this->meshframe.addColor(ofColor::fromHsb(160, 255, 255));
		this->meshframe.addColor(ofColor::fromHsb(150, 255, 255));
		this->meshframe.addColor(ofColor::fromHsb(140, 255, 255));

		this->mesh.addTriangle(this->mesh.getNumVertices() - 1, this->mesh.getNumVertices() - 2, this->mesh.getNumVertices() - 3);
		this->mesh.addTriangle(this->mesh.getNumVertices() - 4, this->mesh.getNumVertices() - 5, this->mesh.getNumVertices() - 6);

		this->mesh.addTriangle(this->mesh.getNumVertices() - 1, this->mesh.getNumVertices() - 2, this->mesh.getNumVertices() - 5);
		this->mesh.addTriangle(this->mesh.getNumVertices() - 1, this->mesh.getNumVertices() - 5, this->mesh.getNumVertices() - 4);

		this->mesh.addTriangle(this->mesh.getNumVertices() - 1, this->mesh.getNumVertices() - 3, this->mesh.getNumVertices() - 6);
		this->mesh.addTriangle(this->mesh.getNumVertices() - 1, this->mesh.getNumVertices() - 6, this->mesh.getNumVertices() - 4);

		this->mesh.addTriangle(this->mesh.getNumVertices() - 2, this->mesh.getNumVertices() - 3, this->mesh.getNumVertices() - 6);
		this->mesh.addTriangle(this->mesh.getNumVertices() - 2, this->mesh.getNumVertices() - 6, this->mesh.getNumVertices() - 5);

		this->meshframe.addIndex(this->meshframe.getNumVertices() - 1); this->meshframe.addIndex(this->meshframe.getNumVertices() - 2);
		this->meshframe.addIndex(this->meshframe.getNumVertices() - 2); this->meshframe.addIndex(this->meshframe.getNumVertices() - 3);
		this->meshframe.addIndex(this->meshframe.getNumVertices() - 1); this->meshframe.addIndex(this->meshframe.getNumVertices() - 3);

		this->meshframe.addIndex(this->meshframe.getNumVertices() - 4); this->meshframe.addIndex(this->meshframe.getNumVertices() - 5);
		this->meshframe.addIndex(this->meshframe.getNumVertices() - 5); this->meshframe.addIndex(this->meshframe.getNumVertices() - 6);
		this->meshframe.addIndex(this->meshframe.getNumVertices() - 4); this->meshframe.addIndex(this->meshframe.getNumVertices() - 6);

		this->meshframe.addIndex(this->meshframe.getNumVertices() - 1); this->meshframe.addIndex(this->meshframe.getNumVertices() - 4);
		this->meshframe.addIndex(this->meshframe.getNumVertices() - 2); this->meshframe.addIndex(this->meshframe.getNumVertices() - 5);
		this->meshframe.addIndex(this->meshframe.getNumVertices() - 3); this->meshframe.addIndex(this->meshframe.getNumVertices() - 6);
	}

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();
	ofBackground(0);
	ofTranslate(512, 128);

	this->cam.begin();
	ofRotateX(270);

	// WarpSphere draw
	this->face.draw();
	this->frame.drawWireframe();

	// TriangleSphere draw
	this->mesh.drawFaces();
	this->meshframe.drawWireframe();

	this->cam.end();

	centerFbo.end();

	//------------------------------------------------------------------------------
	sideFbo.begin();
	ofBackground(0);

	this->cam.begin();

	ofNoFill();
	for (int k = 0; k < 150; k++) {

		ofPushMatrix();
		ofRotateZ(ofRandom(360));
		ofRotateY(ofRandom(360));
		ofRotateX(ofRandom(360));
		
		auto location = glm::vec3(0, 0, (sound[10] * sound[10]) * 50 + 10);
		auto radius = (int)(ofRandom(1000) + ofGetFrameNum() * 2) % 180 - 100;

		if (radius < 0) {

			auto z = ofMap(radius, -100, 0, 1500, 0);

			ofSetColor(ofColor::fromHsb(140, 255, 255));
			ofDrawLine(location + glm::vec3(0, 0, z), location + glm::vec3(0, 0, z + 40));
		}
		else {

			ofSetColor(255, radius > 25 ? ofMap(radius, 25, 80, 255, 0) : 255);

			ofBeginShape();
			for (int deg = 0; deg < 360; deg++) {

				auto vertex = location + glm::vec2(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD));
				vertex = glm::normalize(vertex) * 250;

				ofVertex(vertex);
			}
			ofEndShape(true);
		}

		ofPopMatrix();
	}

	ofFill();
	ofSetColor(0);
	ofDrawSphere(sound[50] * sound[50]);

	this->cam.end();

	sideFbo.end();
}

void RotateSphere::draw() {
	ofClear(0);
	ofBackground(0);

	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);
}

void RotateSphere::audioIn(ofSoundBuffer& input) {

	float curVol = 0.0;

	for (size_t i = 0; i < input.getNumFrames(); i++) {
		sound[i] = input[i] * 1;
	}

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

}