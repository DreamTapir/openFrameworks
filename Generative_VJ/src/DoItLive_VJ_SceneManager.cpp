#include "DoItLive_VJ_SceneManager.h"

void DoItLive_VJ_SceneManager::setup() {
	this->elements.push_back(new CliffordAttractor());
	this->elements.push_back(new TransMesh());
	this->elements.push_back(new PointFlow());
	this->elements.push_back(new ReactionDiffusion());
	this->elements.push_back(new HexagonMesh());
	this->elements.push_back(new RotateSphere());
    init();
}

void DoItLive_VJ_SceneManager::keyPressed(int key) {
	unsigned idx = key - '0';
	if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
	if (key == 'q') Glitch.setFx(OFXPOSTGLITCH_CONVERGENCE, true);
	if (key == 'w') Glitch.setFx(OFXPOSTGLITCH_GLOW, true);
	if (key == 'e') Glitch.setFx(OFXPOSTGLITCH_SHAKER, true);
	if (key == 'r') Glitch.setFx(OFXPOSTGLITCH_CUTSLIDER, true);
	if (key == 't') Glitch.setFx(OFXPOSTGLITCH_TWIST, true);
	if (key == 'y') Glitch.setFx(OFXPOSTGLITCH_OUTLINE, true);
	if (key == 'u') Glitch.setFx(OFXPOSTGLITCH_NOISE, true);
	if (key == 'i') Glitch.setFx(OFXPOSTGLITCH_SLITSCAN, true);
	if (key == 'o') Glitch.setFx(OFXPOSTGLITCH_SWELL, true);
	if (key == 'p') Glitch.setFx(OFXPOSTGLITCH_INVERT, true);
	if (key == 'a') Glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	if (key == 's') Glitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE, true);
	if (key == 'd') Glitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, true);
	if (key == 'f') Glitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE, true);
	if (key == 'g') Glitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT, true);
	if (key == 'h') Glitch.setFx(OFXPOSTGLITCH_CR_REDINVERT, true);
	if (key == 'j') Glitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT, true);
}

void DoItLive_VJ_SceneManager::keyReleased(int key) {
	if (key == 'q') Glitch.setFx(OFXPOSTGLITCH_CONVERGENCE, false);
	if (key == 'w') Glitch.setFx(OFXPOSTGLITCH_GLOW, false);
	if (key == 'e') Glitch.setFx(OFXPOSTGLITCH_SHAKER, false);
	if (key == 'r') Glitch.setFx(OFXPOSTGLITCH_CUTSLIDER, false);
	if (key == 't') Glitch.setFx(OFXPOSTGLITCH_TWIST, false);
	if (key == 'y') Glitch.setFx(OFXPOSTGLITCH_OUTLINE, false);
	if (key == 'u') Glitch.setFx(OFXPOSTGLITCH_NOISE, false);
	if (key == 'i') Glitch.setFx(OFXPOSTGLITCH_SLITSCAN, false);
	if (key == 'o') Glitch.setFx(OFXPOSTGLITCH_SWELL, false);
	if (key == 'p') Glitch.setFx(OFXPOSTGLITCH_INVERT, false);
	if (key == 'a') Glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
	if (key == 's') Glitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE, false);
	if (key == 'd') Glitch.setFx(OFXPOSTGLITCH_CR_REDRAISE, false);
	if (key == 'f') Glitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE, false);
	if (key == 'g') Glitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT, false);
	if (key == 'h') Glitch.setFx(OFXPOSTGLITCH_CR_REDINVERT, false);
	if (key == 'j') Glitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT, false);

    switch (key) {
    case 'z':
        changeElement(0);
        break;
    case 'x':
        changeElement(1);
        break;
	case 'c':
		changeElement(2);
		break;
	case 'v':
		changeElement(3);
		break;
	case 'b':
		changeElement(4);
		break;
	case 'n':
		changeElement(5);
		break;
    default:
        break;
    }
}