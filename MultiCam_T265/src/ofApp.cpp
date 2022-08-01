#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setupFrameworks();
    setupCameras();
    setupViewports();
    particles.setup();
    rotatesphere.setup();

    vol = 100;

    if (!initializeT265()) {
        return;
    }

    fisheye_texture_.allocate(intrinsics.width, intrinsics.height, GL_RGB, GL_LUMINANCE, GL_UNSIGNED_BYTE);

    is_start_ = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    particles.update();
    rotatesphere.update();

    if (!updateT265()) {
        return;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawCameras();
    drawLabels();
}

//--------------------------------------------------------------
void ofApp::setupFrameworks() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::setupCameras() {
    int deg = 0;

    axis.x = 0;
    axis.y = 1;
    axis.z = 0;

    // Setup cameras

    iMainCamera = 0;

    cameras[0] = &cam;
    cameras[1] = &camFront;
    cameras[2] = &camRight;
    cameras[3] = &camBack;
    cameras[4] = &camLeft;

    for (size_t i = 1; i != N_CAMERAS; ++i) {
        cameras[i]->setPosition(0, 0, 0);
        cameras[i]->setFov(53);
        cameras[i]->rotateDeg(deg, axis);
        cameras[i]->setLensOffset(lensoffset);

        deg -= 90;
    }
}

//--------------------------------------------------------------
void ofApp::setupViewports() {
    float xOffset = WIDTH / N_CAMERAS;
    float yOffset = VIEWWINDOW_HEIGHT;

    viewMain.x = 0;
    viewMain.y = yOffset;
    viewMain.width = WIDTH;
    viewMain.height = HEIGHT - yOffset;

    for (int i = 1; i < N_CAMERAS; i++) {

        viewGrid[i].x = xOffset * i;
        viewGrid[i].y = 0;
        viewGrid[i].width = xOffset;
        viewGrid[i].height = yOffset;
    }
}

//--------------------------------------------------------------
void ofApp::drawCameras() {
    ofPushStyle();

    ofDisableDepthTest();
    ofSetColor(50, 50, 50);
    ofDrawRectangle(viewGrid[iMainCamera]);

    ofEnableDepthTest();

    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofEnablePointSprites();

    // draw main viewport
    cameras[0]->begin(viewMain);
    drawMonitor(iMainCamera);
    ofDrawGrid(50, 50, false, true, true, true);
    particles.draw();
    //rotatesphere.draw();
    cameras[iMainCamera]->end();

    // draw side viewports
    for (int i = 1; i < N_CAMERAS; i++) {
        arBegin(i);
        ofDrawGrid(50, 50, false, true, true, true);
        particles.draw();
        //rotatesphere.draw();
        arEnd(i);
    }

    ofDisablePointSprites();


    ofPopStyle();
}

void ofApp::drawLabels() {
    ofPushStyle();
    ofDisableDepthTest();

    // draw some labels
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Front", viewGrid[1].x + 20, viewGrid[1].y + 30);
    ofDrawBitmapString("Right", viewGrid[2].x + 20, viewGrid[2].y + 30);
    ofDrawBitmapString("Back", viewGrid[3].x + 20, viewGrid[3].y + 30);
    ofDrawBitmapString("Left", viewGrid[4].x + 20, viewGrid[4].y + 30);

    // draw outlines on views
    ofSetLineWidth(3);
    ofNoFill();
    ofSetColor(50, 50, 50);
    for (int i = 0; i < N_CAMERAS; i++) {
        ofDrawRectangle(viewGrid[i]);
    }
    ofDrawRectangle(viewMain);

    // restore the GL depth function

    ofPopStyle();
}

void ofApp::drawMonitor(int cameraIndex) {
    for (int i = 1; i < 5; i++) {

		ofPushStyle();
		ofPushMatrix();

		ofRectangle boundsToUse;
		boundsToUse = viewGrid[i];

		glm::mat4 inverseCameraMatrix;
        inverseCameraMatrix = glm::inverse(cameras[i]->getModelViewProjectionMatrix(boundsToUse));

		ofMultMatrix(inverseCameraMatrix);
        if (iMainCamera == i) {
            ofSetColor(255,  10);
            ofFill();
            // i.e. a box -1, -1, -1 to +1, +1, +1
            ofDrawBox(0, 0, 0, 2.0f);
        }
        else {
            ofSetColor(255);
            ofNoFill();
            // i.e. a box -1, -1, -1 to +1, +1, +1
            ofDrawBox(0, 0, 0, 2.0f);
        }

		ofPopMatrix();
		ofPopStyle();
	}
}

void ofApp::drawScene(int cameraIndex) {
    if (cameraIndex != 0) {

        ofPushStyle();
        ofPushMatrix();

        ofRectangle boundsToUse;
        if (iMainCamera == 0) {
            boundsToUse = viewMain;
        }
        else {
            boundsToUse = viewGrid[0];
        }

        glm::mat4 inverseCameraMatrix;
        inverseCameraMatrix = glm::inverse(cam.getModelViewProjectionMatrix(boundsToUse));

        ofMultMatrix(inverseCameraMatrix);
        ofSetColor(255);
        ofNoFill();
        // i.e. a box -1, -1, -1 to +1, +1, +1
        ofDrawBox(0, 0, 0, 2.0f);

        ofPopMatrix();
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key >= '1' && key <= '5') {
        iMainCamera = key - '1';
    }
    if (key == 'f') {
        ofToggleFullscreen();
    }
}

bool ofApp::initializeT265() {
    try {
        cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
        cfg.enable_stream(RS2_STREAM_FISHEYE, 1);
        cfg.enable_stream(RS2_STREAM_FISHEYE, 2);

        pipe_profile = pipe.start(cfg);

        fisheye_stream = pipe_profile.get_stream(RS2_STREAM_FISHEYE, FISH_EYE_SENSOR_USE_INDEX);
        intrinsics = fisheye_stream.as<rs2::video_stream_profile>().get_intrinsics();
        extrinsics = fisheye_stream.get_extrinsics_to(pipe_profile.get_stream(RS2_STREAM_POSE));
    }
    catch (const rs2::error& e) {
        std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool ofApp::updateT265() {
    if (!is_start_) {
        return false;
    }

    auto frames = pipe.wait_for_frames();
    rs2::video_frame fisheye_frame = frames.get_fisheye_frame(FISH_EYE_SENSOR_USE_INDEX);
    rs2::pose_frame pose_frame = frames.get_pose_frame();
    device_pose_in_world = pose_frame.get_pose_data();

    fisheye_texture_.loadData(static_cast<const uint8_t*>(fisheye_frame.get_data()),
        intrinsics.width,
        intrinsics.height,
        GL_LUMINANCE);

    return true;
}

void ofApp::arBegin(int i) {
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    translateCamByFishEyeCenter(i);
    translateCamByExtrinsics(i);
    //rotateCam(i);
    cameras[i]->begin(viewGrid[i]);
}

void ofApp::arEnd(int i) {
    cameras[i]->end();
    ofPopMatrix();
}

void ofApp::translateCamByFishEyeCenter(int i) {
    float x = device_pose_in_world.translation.x * vol;
    float y = device_pose_in_world.translation.y * vol;
    float z = device_pose_in_world.translation.z * vol;

    cameras[i]->setPosition(x, y, z);
}

void ofApp::translateCamByExtrinsics(int i) {
    float x = -device_pose_in_world.translation.x * vol;
    float y = -device_pose_in_world.translation.y * vol;
    float z = -device_pose_in_world.translation.z * vol;

    float x2 = extrinsics.rotation[0] * x + extrinsics.rotation[3] * y + extrinsics.rotation[6] * z + extrinsics.translation[0] * vol;
    float y2 = extrinsics.rotation[1] * x + extrinsics.rotation[4] * y + extrinsics.rotation[7] * z + extrinsics.translation[1] * vol;
    float z2 = extrinsics.rotation[2] * x + extrinsics.rotation[5] * y + extrinsics.rotation[8] * z + extrinsics.translation[2] * vol;

    // https://github.com/IntelRealSense/librealsense/blob/master/examples/ar-basic/coordinates.jpg
    cameras[i]->setPosition(-x2, y2, z2);
}

void ofApp::rotateCam(int i) {
    ofQuaternion q(device_pose_in_world.rotation.x,
        device_pose_in_world.rotation.y,
        device_pose_in_world.rotation.z,
        device_pose_in_world.rotation.w);

    cameras[i]->setOrientation(q);
}