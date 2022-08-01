#include "Particles.h"

void Particles::setup() {
    particleNum = 500000;
    texRes = ceil(sqrt(particleNum));

    render.load("shaders/render");
    updatePos.load("", "shaders/update.frag");

    particles.setMode(OF_PRIMITIVE_POINTS);
    for (int i = 0; i < texRes; i++) {
        for (int j = 0; j < texRes; j++) {
            int index = i * texRes + j;
            if (index < particleNum) {
                particles.addVertex(ofVec3f(0, 0, 0));
                particles.addTexCoord(ofVec2f(i, j));
                particles.addColor(ofFloatColor(0.8, 0.3, 0.1, 1));
            }
        }
    }

    pingPong.allocate(texRes, texRes, GL_RGBA32F, 2);

    float* posAndAge = new float[texRes * texRes * 4];
    for (int x = 0; x < texRes; x++) {
        for (int y = 0; y < texRes; y++) {
            int i = texRes * y + x;
            posAndAge[i * 4 + 0] = ofRandom(-1.0, 1.0);
            posAndAge[i * 4 + 1] = ofRandom(-1.0, 1.0);
            posAndAge[i * 4 + 2] = ofRandom(-1.0, 1.0);
            posAndAge[i * 4 + 3] = 0;
        }
    }

    pingPong.src->getTextureReference(0).loadData(posAndAge, texRes, texRes, GL_RGBA);
    delete[] posAndAge;

    float* velAndMaxAge = new float[texRes * texRes * 4];
    for (int x = 0; x < texRes; x++) {
        for (int y = 0; y < texRes; y++) {
            int i = texRes * y + x;
            velAndMaxAge[i * 4 + 0] = 0.0;
            velAndMaxAge[i * 4 + 1] = 0.0;
            velAndMaxAge[i * 4 + 2] = 0.0;
            velAndMaxAge[i * 4 + 3] = ofRandom(1, 150);
        }
    }

    pingPong.src->getTextureReference(1).loadData(velAndMaxAge, texRes, texRes, GL_RGBA);
    delete[] velAndMaxAge;
}

void Particles::update() {
    float time = ofGetElapsedTimef();

    prevEmitterPos = emitterPos;
    emitterPos = 300 * ofVec3f(ofSignedNoise(time, 0, 0), ofSignedNoise(0, time, 0), ofSignedNoise(0, 0, time));

    pingPong.dst->begin();

    pingPong.dst->activateAllDrawBuffers();
    ofClear(0);
    updatePos.begin();
    updatePos.setUniformTexture("u_posAndAgeTex", pingPong.src->getTextureReference(0), 0);
    updatePos.setUniformTexture("u_velAndMaxAgeTex", pingPong.src->getTextureReference(1), 1);
    updatePos.setUniform1f("u_time", time);
    updatePos.setUniform1f("u_timestep", 0.5);
    updatePos.setUniform1f("u_scale", 0.005);
    updatePos.setUniform3f("u_emitterPos", emitterPos.x, emitterPos.y, emitterPos.z);
    updatePos.setUniform3f("u_prevEmitterPos", prevEmitterPos.x, prevEmitterPos.y, prevEmitterPos.z);
    pingPong.src->draw(0, 0);
    updatePos.end();
    pingPong.dst->end();
    pingPong.swap();
}

void Particles::draw() {
    render.begin();
    render.setUniformTexture("u_posAndAgeTex", pingPong.src->getTextureReference(0), 0);
    particles.draw();
    render.end();
}