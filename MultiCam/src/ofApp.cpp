#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setupFrameworks();
    setupCameras();
    setupViewports();
    setupGui();
    particles.setup();
    rotatesphere.setup();
    clustercube.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    if (particle_bool) particles.update();
    if (rotatesphere_bool) rotatesphere.update();
    if (clustercube_bool) clustercube.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawCameras();
    drawLabels();
}

// Setup
void ofApp::setupFrameworks() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    ofDisableAlphaBlending();
}

void ofApp::setupCameras() {
    // default param
    deg = 270;
    fov = 53;
    nearclip = 10;
    farclip = 1000;
    aspect = 2./1.;

    axis.x = 0;
    axis.y = 1;
    axis.z = 0;

    // Setup cameras

    iMainCamera = 0;

    cameras[0] = &cam;
    cameras[1] = &camRight;
    cameras[2] = &camBack;
    cameras[3] = &camLeft;
    cameras[4] = &camFront;

    for (size_t i = 1; i != N_CAMERAS; ++i) {
        cameras[i]->setupPerspective(true, fov, nearclip, farclip, lensoffset);
        cameras[i]->setPosition(0, 0, 0);
        cameras[i]->rotateDeg(deg, axis);
        cameras[i]->setAspectRatio(aspect);

        deg -= 90;
    }
}

void ofApp::setupViewports() {
    //call here whenever we resize the window


    //--
    // Define viewports

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

    //
    //--
}

void ofApp::setupGui()
{
    // instantiate and position the gui //
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);

    // add some components //
    gui->addTextInput("message", "# MultiCam 3D #");

    gui->addFRM();
    gui->addBreak();

    // add a folder to group a few components together //
    ofxDatGuiFolder* RightCamfolder = gui->addFolder("RightCam", ofColor::white);
    RightCamfolder->addSlider("R Fov", 0, 180, fov);
    RightCamfolder->addSlider("R NearClip", 0, 5000, nearclip);
    RightCamfolder->addSlider("R FarClip", 0, 5000, farclip);
    RightCamfolder->addSlider("R LensOffsetX", -10, 10, Rlensoffset.x);
    RightCamfolder->addSlider("R LensOffsetY", -10, 10, Rlensoffset.y);
    RightCamfolder->addSlider("R Aspect", 0, 10, aspect);
    RightCamfolder->addToggle("R Ortho", false);
    RightCamfolder->expand();
    gui->addBreak();

    ofxDatGuiFolder* BackCamfolder = gui->addFolder("BackCam", ofColor::white);
    BackCamfolder->addSlider("B Fov", 0, 180, fov);
    BackCamfolder->addSlider("B NearClip", 0, 5000, nearclip);
    BackCamfolder->addSlider("B FarClip", 0, 5000, farclip);
    BackCamfolder->addSlider("B LensOffsetX", -10, 10, Blensoffset.x);
    BackCamfolder->addSlider("B LensOffsetY", -10, 10, Blensoffset.y);
    BackCamfolder->addSlider("B Aspect", 0, 10, aspect);
    BackCamfolder->addToggle("B Ortho", false);
    BackCamfolder->expand();
    gui->addBreak();

    ofxDatGuiFolder* LeftCamfolder = gui->addFolder("LeftCam", ofColor::white);
    LeftCamfolder->addSlider("L Fov", 0, 180, fov);
    LeftCamfolder->addSlider("L NearClip", 0, 5000, nearclip);
    LeftCamfolder->addSlider("L FarClip", 0, 5000, farclip);
    LeftCamfolder->addSlider("L LensOffsetX", -10, 10, Llensoffset.x);
    LeftCamfolder->addSlider("L LensOffsetY", -10, 10, Llensoffset.y);
    LeftCamfolder->addSlider("L Aspect", 0, 10, aspect);
    LeftCamfolder->addToggle("L Ortho", false);
    LeftCamfolder->expand();
    gui->addBreak();

    ofxDatGuiFolder* FrontCamfolder = gui->addFolder("FrontCam", ofColor::white);
    FrontCamfolder->addSlider("F Fov", 0, 180, fov);
    FrontCamfolder->addSlider("F NearClip", 0, 5000, nearclip);
    FrontCamfolder->addSlider("F FarClip", 0, 5000, farclip);
    FrontCamfolder->addSlider("F LensOffsetX", -10, 10, Flensoffset.x);
    FrontCamfolder->addSlider("F LensOffsetY", -10, 10, Flensoffset.y);
    FrontCamfolder->addSlider("F Aspect", 0, 10, aspect);
    FrontCamfolder->addToggle("F Ortho", false);
    FrontCamfolder->expand();
    gui->addBreak();

    ofxDatGuiFolder* Effectsfolder = gui->addFolder("Effects", ofColor::red);
    Effectsfolder->addToggle("Particle", false);
    Effectsfolder->addToggle("RotateSphere", false);
    Effectsfolder->addToggle("ClusterCube", false);

    gui->addToggle("draw scene", true);
    gui->addToggle("toggle fullscreen", true);

    // particles color
    /*gui->addColorPicker("Particles startColor", ofColor::fromHex(0xeeeeee));
    gui->addColorPicker("Particles endColor", ofColor::fromHex(0xeeeeee));*/

    gui->addHeader(":: drag me to reposition ::");
    gui->addFooter();

    // once the gui has been assembled, register callbacks to listen for component specific events //
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    gui->on2dPadEvent(this, &ofApp::on2dPadEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    gui->onColorPickerEvent(this, &ofApp::onColorPickerEvent);
    gui->onMatrixEvent(this, &ofApp::onMatrixEvent);

    // finally let's load up the stock themes, press spacebar to cycle through them //
    themes = { new ofxDatGuiTheme(true),
        new ofxDatGuiThemeSmoke(),
        new ofxDatGuiThemeWireframe(),
        new ofxDatGuiThemeMidnight(),
        new ofxDatGuiThemeAqua(),
        new ofxDatGuiThemeCharcoal(),
        new ofxDatGuiThemeAutumn(),
        new ofxDatGuiThemeCandy() };
    tIndex = 0;

    // launch the app //
    mFullscreen = true;
    refreshWindow();

    // ortho bool setup
    particle_bool = false;
    rotatesphere_bool = false;
    clustercube_bool = false;

    // effect bool setup
    RCObool = false;
    BCObool = false;
    LCObool = false;
    FCObool = false;
}

// Draw
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
    ofDrawGrid(100, 100, false, true, true, true);
    if (particle_bool) particles.draw();
    if (rotatesphere_bool) rotatesphere.draw();
    if (clustercube_bool) clustercube.draw();
    cameras[iMainCamera]->end();

    // draw side viewports
    for (int i = 1; i < N_CAMERAS; i++) {
        cameras[i]->begin(viewGrid[i]);
        if (drawScene_bool) drawScene(i);
        if (particle_bool) particles.draw();
        if (rotatesphere_bool) rotatesphere.draw();
        if (clustercube_bool) clustercube.draw();
        cameras[i]->end();
    }

    ofDisablePointSprites();

    ofPopStyle();
}

void ofApp::drawLabels() {
    ofPushStyle();
    ofDisableDepthTest();

    // draw some labels
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Right", viewGrid[1].x + 20, viewGrid[1].y + 30);
    ofDrawBitmapString("Back", viewGrid[2].x + 20, viewGrid[2].y + 30);
    ofDrawBitmapString("Left", viewGrid[3].x + 20, viewGrid[3].y + 30);
    ofDrawBitmapString("Front", viewGrid[4].x + 20, viewGrid[4].y + 30);

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
            ofSetColor(255, 50);
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

// Event
void ofApp::keyPressed(int key){
    if (key >= '1' && key <= '5') {
        iMainCamera = key - '1';
    }
    if (key == 'f') {
        ofToggleFullscreen();
    }
}

void ofApp::toggleFullscreen()
{
    mFullscreen = !mFullscreen;
    gui->getToggle("toggle fullscreen")->setChecked(mFullscreen);
    refreshWindow();
}

void ofApp::refreshWindow()
{
    ofSetFullscreen(mFullscreen);
    if (!mFullscreen) {
        int width = ofGetScreenWidth() * .8;
        int height = ofGetScreenHeight() * .8;
        ofSetWindowShape(width, height);
        ofSetWindowPosition((ofGetScreenWidth() / 2) - (width / 2), 0);
    }
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
    cout << "onButtonEvent: " << e.target->getLabel() << endl;
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e)
{
    if (e.target->is("toggle fullscreen")) toggleFullscreen();
    cout << "onToggleEvent: " << e.target->getLabel() << " " << e.checked << endl;

    // ortho
    if (e.target->is("R Ortho")) RightCamOrtho();
    if (e.target->is("B Ortho"))  BackCamOrtho();
    if (e.target->is("L Ortho")) LeftCamOrtho();
    if (e.target->is("F Ortho")) FrontCamOrtho();

    // draw scene bool
    if (e.target->is("draw scene")) {
        drawScene_bool = !drawScene_bool;
        gui->getToggle("draw scene")->setChecked(drawScene_bool);
    }

    // draw effects bool
    if (e.target->is("Particle")) {
        particle_bool = !particle_bool;
        gui->getToggle("Particle")->setChecked(particle_bool);
    }
    if (e.target->is("RotateSphere")) {
        rotatesphere_bool = !rotatesphere_bool;
        gui->getToggle("RotateSphere")->setChecked(rotatesphere_bool);
    }
    if (e.target->is("ClusterCube")) {
        clustercube_bool = !clustercube_bool;
        gui->getToggle("ClusterCube")->setChecked(clustercube_bool);
    }
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
    cout << "onSliderEvent: " << e.target->getLabel() << " " << e.target->getValue() << endl;

    // fov
    if (e.target->is("R Fov")) cameras[1]->setFov(e.value);
    if (e.target->is("B Fov")) cameras[2]->setFov(e.value);
    if (e.target->is("L Fov")) cameras[3]->setFov(e.value);
    if (e.target->is("F Fov")) cameras[4]->setFov(e.value);

    // nearclip
    if (e.target->is("R NearClip")) cameras[1]->setNearClip(e.value);
    if (e.target->is("B NearClip")) cameras[2]->setNearClip(e.value);
    if (e.target->is("L NearClip")) cameras[3]->setNearClip(e.value);
    if (e.target->is("F NearClip")) cameras[4]->setNearClip(e.value);

    // farclip
    if (e.target->is("R FarClip")) cameras[1]->setFarClip(e.value);
    if (e.target->is("B FarClip")) cameras[2]->setFarClip(e.value);
    if (e.target->is("L FarClip")) cameras[3]->setFarClip(e.value);
    if (e.target->is("F FarClip")) cameras[4]->setFarClip(e.value);

    // aspect
    if (e.target->is("R Aspect")) cameras[1]->setAspectRatio(e.value);
    if (e.target->is("B Aspect")) cameras[2]->setAspectRatio(e.value);
    if (e.target->is("L Aspect")) cameras[3]->setAspectRatio(e.value);
    if (e.target->is("F Aspect")) cameras[4]->setAspectRatio(e.value);

    // lensoffset X
    if (e.target->is("R LensOffsetX")){
        Rlensoffset.x = e.value;
        cameras[1]->setLensOffset(Rlensoffset);
    }
    if (e.target->is("B LensOffsetX")) {
        Blensoffset.x = e.value;
        cameras[2]->setLensOffset(Blensoffset);
    }
    if (e.target->is("L LensOffsetX")) {
        Llensoffset.x = e.value;
        cameras[3]->setLensOffset(Llensoffset);
    }
    if (e.target->is("F LensOffsetX")) {
        Flensoffset.x = e.value;
        cameras[4]->setLensOffset(Flensoffset);
    }

    // lensoffset Y
    if (e.target->is("R LensOffsetY")) {
        Rlensoffset.y = e.value;
        cameras[1]->setLensOffset(Rlensoffset);
    }
    if (e.target->is("B LensOffsetY")) {
        Blensoffset.y = e.value;
        cameras[2]->setLensOffset(Blensoffset);
    }
    if (e.target->is("L LensOffsetY")) {
        Llensoffset.y = e.value;
        cameras[3]->setLensOffset(Llensoffset);
    }
    if (e.target->is("F LensOffsetY")) {
        Flensoffset.y = e.value;
        cameras[4]->setLensOffset(Flensoffset);
    }
}

void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    cout << "onTextInputEvent: " << e.target->getLabel() << " " << e.target->getText() << endl;
}

void ofApp::on2dPadEvent(ofxDatGui2dPadEvent e)
{
    cout << "on2dPadEvent: " << e.target->getLabel() << " " << e.x << ":" << e.y << endl;
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
}

void ofApp::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
{
    cout << "onColorPickerEvent: " << e.target->getLabel() << " " << e.target->getColor() << endl;
}

void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e)
{
    cout << "onMatrixEvent " << e.child << " : " << e.enabled << endl;
    cout << "onMatrixEvent " << e.target->getLabel() << " : " << e.target->getSelected().size() << endl;
}

// Ortho bool
void ofApp::RightCamOrtho()
{
    RCObool = !RCObool;
    gui->getToggle("R Ortho")->setChecked(RCObool);
    if (RCObool) cameras[1]->enableOrtho();
    if (!RCObool) cameras[1]->disableOrtho();
}

void ofApp::BackCamOrtho()
{
    BCObool = !BCObool;
    gui->getToggle("B Ortho")->setChecked(BCObool);
    if (BCObool) cameras[2]->enableOrtho();
    if (!BCObool) cameras[2]->disableOrtho();
}

void ofApp::LeftCamOrtho()
{
    LCObool = !LCObool;
    gui->getToggle("L Ortho")->setChecked(LCObool);
    if (LCObool) cameras[3]->enableOrtho();
    if (!LCObool) cameras[3]->disableOrtho();
}

void ofApp::FrontCamOrtho()
{
    FCObool = !FCObool;
    gui->getToggle("F Ortho")->setChecked(FCObool);
    if (FCObool) cameras[4]->enableOrtho();
    if (!FCObool) cameras[4]->disableOrtho();
}