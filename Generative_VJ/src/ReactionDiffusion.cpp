#include "ReactionDiffusion.h"

ReactionDiffusion::ReactionDiffusion() {
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

	/*
 ofEasyCam
 */
	cam.setupPerspective(true, 60, 10, 0, ofVec2f(0.0, 0.0));
	cam.setDistance(150);

	/*
	 ofxReactionDiffusion
	 */
	scale = 3.0;

	rd.allocate(ofGetWidth(), ofGetHeight(), scale);
	rd.setPasses(0.998265);
	rd.setGrayScottParams(0.0239133, 0.0501276, 0.179571, 0.212041);
	//rd.setGrayScottParams(0.02, 0.0501276, 0.1785, 0.212041);

	/// add Initial Sources
	rd.getSourceFbo().begin();
	ofSetColor(0, 0, 200);
	ofDrawRectangle(100, 100, 100, 100);
	rd.getSourceFbo().end();

	/*
	 Mesh
	 */
	mesh.setMode(OF_PRIMITIVE_POINTS);

	for (int y = 0; y < ofGetHeight() / scale; y++) {
		for (int x = 0; x < ofGetWidth() / scale; x++) {
			mesh.addVertex(ofVec3f(0, 0, 0));
			mesh.addTexCoord(ofVec2f(y, x));
			mesh.addColor(ofFloatColor(1.0));
		}
	}

	/// Shader
	string updatePosVert = STRINGIFY(
		uniform sampler2DRect sourceTex;
	uniform sampler2DRect colorTex;

	void main() {
		vec2 st = gl_MultiTexCoord0.st;

		vec3 pos = vec3(st.x, st.y, texture2DRect(sourceTex, st).b * 50.0);
		vec3 color = texture2DRect(colorTex, st).rgb;

		gl_FrontColor = vec4(color, 1.0);
		gl_PointSize = 3.0;
		gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
	}
	);
	updatePosShader.setupShaderFromSource(GL_VERTEX_SHADER, updatePosVert);
	updatePosShader.linkProgram();

	// NoiseBox
	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

	int span = 4;
	for (int x = -22; x <= 22; x += span) {

		for (int y = -22; y <= 22; y += span) {

			for (int z = -22; z <= 22; z += 44) {

				this->location_list.push_back(glm::vec3(x, y, z));
			}
		}
	}

	for (int x = -22; x <= 22; x += 44) {

		for (int y = -22; y <= 22; y += span) {

			for (int z = -22 + span; z <= 22 - span; z += span) {

				this->location_list.push_back(glm::vec3(x, y, z));
			}
		}
	}

	for (int x = -22 + span; x <= 22 - span; x += span) {

		for (int y = -22; y <= 22; y += 44) {

			for (int z = -22 + span; z <= 22 - span; z += span) {

				this->location_list.push_back(glm::vec3(x, y, z));
			}
		}
	}
}

void ReactionDiffusion::update() {
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	rd.update();

	this->face.clear();
	this->frame.clear();

	float size;
	for (int i = 0; i < this->location_list.size(); i++) {

		auto noise_value = ofNoise(glm::vec4(this->location_list[i] * 0.04, ofGetFrameNum() * 0.030 + sound[100]  * sound[100] * 10));
		if (noise_value < sound[50] * sound[50]) { size = 0; }
		else if (noise_value > 0.45) { size = 4; }
		else { size = ofMap(noise_value, sound[50] * sound[50], 0.45, 0, 4); }

		ofColor color;
		color.setHsb(ofMap(noise_value, 0.5, 1, 180, 120), 255, 255);

		this->setBoxToMesh(this->face, this->frame, this->location_list[i], size, color);
	}

	// for Glitch Buffer -----------------------------------------------------------
	centerFbo.begin();
	ofBackground(sound[100] * 0.1);

	cam.begin();

	ofPushStyle();

	ofRotateXDeg(60);
	ofRotateZDeg(225);
	ofTranslate(-200, -200, 30);

	updatePosShader.begin();
	updatePosShader.setUniformTexture("sourceTex", rd.getSourceTexture(), 0);
	updatePosShader.setUniformTexture("colorTex", rd.getColorTexture(), 1);
	mesh.draw();
	updatePosShader.end();

	ofPopStyle();

	light.enable();

	ofTranslate(150, 155, 10);

	ofRotateX(ofGetFrameNum() * 0.22);
	ofRotateY(ofGetFrameNum() * 0.44);

	this->face.draw();

	ofSetColor(239);
	this->frame.drawWireframe();

	cam.end();

	centerFbo.end();

	//------------------------------------------------------------------------------

	sideFbo.begin();
	ofBackground(sound[5] * 0.7);

	cam.begin();
	ofPushStyle();

	ofRotateXDeg(60);
	ofRotateZDeg(225);
	ofTranslate(-200, -200, 30);

	updatePosShader.begin();
	updatePosShader.setUniformTexture("sourceTex", rd.getSourceTexture(), 0);
	updatePosShader.setUniformTexture("colorTex", rd.getColorTexture(), 1);
	mesh.draw();
	updatePosShader.end();

	ofPopStyle();
	cam.end();

	sideFbo.end();
}

void ReactionDiffusion::draw() {
	ofClear(0);
	ofBackground(0);

	centerFbo.draw(512, 704);
	sideFbo.draw(320, 576);
	sideFbo.draw(1472, 576);


}

void ReactionDiffusion::audioIn(ofSoundBuffer& input) {

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
void ReactionDiffusion::setBoxToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float size, ofColor face_color) {

	this->setBoxToMesh(face_target, frame_target, location, size, size, size, face_color);
}

//--------------------------------------------------------------
void ReactionDiffusion::setBoxToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float height, float width, float depth, ofColor face_color) {

	int face_index = face_target.getNumVertices();
	int frame_index = frame_target.getNumVertices();

	face_target.addVertex(location + glm::vec3(width * -0.5 * 0.99, height * 0.5 * 0.99, depth * -0.5 * 0.99));
	face_target.addVertex(location + glm::vec3(width * 0.5 * 0.99, height * 0.5 * 0.99, depth * -0.5 * 0.99));
	face_target.addVertex(location + glm::vec3(width * 0.5 * 0.99, height * 0.5 * 0.99, depth * 0.5 * 0.99));
	face_target.addVertex(location + glm::vec3(width * -0.5 * 0.99, height * 0.5 * 0.99, depth * 0.5 * 0.99));

	face_target.addVertex(location + glm::vec3(width * -0.5 * 0.99, height * -0.5 * 0.99, depth * -0.5 * 0.99));
	face_target.addVertex(location + glm::vec3(width * 0.5 * 0.99, height * -0.5 * 0.99, depth * -0.5 * 0.99));
	face_target.addVertex(location + glm::vec3(width * 0.5 * 0.99, height * -0.5 * 0.99, depth * 0.5 * 0.99));
	face_target.addVertex(location + glm::vec3(width * -0.5 * 0.99, height * -0.5 * 0.99, depth * 0.5 * 0.99));

	face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2);
	face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 3);

	face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5); face_target.addIndex(face_index + 6);
	face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 6); face_target.addIndex(face_index + 7);

	face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 1);
	face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5); face_target.addIndex(face_index + 1);

	face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 5); face_target.addIndex(face_index + 6);
	face_target.addIndex(face_index + 6); face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 1);

	face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 6); face_target.addIndex(face_index + 7);
	face_target.addIndex(face_index + 7); face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 2);

	face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 7); face_target.addIndex(face_index + 4);
	face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 3);

	frame_target.addVertex(location + glm::vec3(width * -0.5, height * 0.5, depth * -0.5));
	frame_target.addVertex(location + glm::vec3(width * 0.5, height * 0.5, depth * -0.5));
	frame_target.addVertex(location + glm::vec3(width * 0.5, height * 0.5, depth * 0.5));
	frame_target.addVertex(location + glm::vec3(width * -0.5, height * 0.5, depth * 0.5));

	frame_target.addVertex(location + glm::vec3(width * -0.5, height * -0.5, depth * -0.5));
	frame_target.addVertex(location + glm::vec3(width * 0.5, height * -0.5, depth * -0.5));
	frame_target.addVertex(location + glm::vec3(width * 0.5, height * -0.5, depth * 0.5));
	frame_target.addVertex(location + glm::vec3(width * -0.5, height * -0.5, depth * 0.5));

	frame_target.addIndex(frame_index + 0); frame_target.addIndex(frame_index + 1);
	frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 2);
	frame_target.addIndex(frame_index + 2); frame_target.addIndex(frame_index + 3);
	frame_target.addIndex(frame_index + 3); frame_target.addIndex(frame_index + 0);

	frame_target.addIndex(frame_index + 4); frame_target.addIndex(frame_index + 5);
	frame_target.addIndex(frame_index + 5); frame_target.addIndex(frame_index + 6);
	frame_target.addIndex(frame_index + 6); frame_target.addIndex(frame_index + 7);
	frame_target.addIndex(frame_index + 7); frame_target.addIndex(frame_index + 4);

	frame_target.addIndex(frame_index + 0); frame_target.addIndex(frame_index + 4);
	frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 5);
	frame_target.addIndex(frame_index + 2); frame_target.addIndex(frame_index + 6);
	frame_target.addIndex(frame_index + 3); frame_target.addIndex(frame_index + 7);

	for (int i = 0; i < 8; i++) {

		face_target.addColor(face_color);
	}
}