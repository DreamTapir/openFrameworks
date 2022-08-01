#include "boids.h"
#include "ofMain.h"
float sq(float a) {
    return a * a;
}

boids::boids(float x, float y, float vx, float vy, boids* others[], int num_others) {
    this->r1 = 40;
    this->r2 = 80;
    this->r3 = 160;

    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
    this->others = others;
    this->num_others = num_others;
}

void boids::boo() {
    vx = ofRandom(-25, 25);
    vy = ofRandom(-25, 25);
}

void boids::update() {
    sep();
    align();
    coh();

    float l = sqrt(sq(vx) + sq(vy));
    if (l > 4) {
        vx = 0.9 * vx;
        vy = 0.9 * vy;
    }

    x += vx;
    y += vy;

    if (x > ofGetWidth() - 30) { vx -= 2; }
    if (x < 30) { vx += 2; }
    if (y > ofGetHeight() - 30) { vy -= 2; }
    if (y < 30) { vy += 2; }
}

void boids::sep() {
    int count = 0;
    float sx = 0;
    float sy = 0;

    for (int i = 0; i < num_others; i++) {

        float d = sqrt(sq(x - others[i]->x) + sq(y - others[i]->y));
        if (d < r1 && this != others[i]) {
            count++;
            sx += others[i]->x;
            sy += others[i]->y;
        }
    }

    if (count > 0) {
        sx = x - sx / count;
        sy = y - sy / count;

        float l = sqrt(sq(sx) + sq(sy));
        sx = 1.2 * sx / l;
        sy = 1.2 * sy / l;

    }

    vx += sx;
    vy += sy;
}

void boids::align() {
    int count = 0;
    float dx = 0;
    float dy = 0;

    for (int i = 0; i < num_others; i++) {

        float d = sqrt(sq(x - others[i]->x) + sq(y - others[i]->y));
        if (d < r2 && this != others[i]) {
            count++;
            dx += others[i]->vx;
            dy += others[i]->vy;
        }
    }

    if (count > 0) {
        dx = dx / count;
        dy = dy / count;
        float l = sqrt(sq(dx) + sq(dy));
        dx = dx / l;
        dy = dy / l;
    }

    vx += dx;
    vy += dy;
}

void boids::coh() {
    int count = 0;
    float sx = 0;
    float sy = 0;

    for (int i = 0; i < num_others; i++) {

        float d = sqrt(sq(x - others[i]->x) + sq(y - others[i]->y));
        if (d < r3 && d > r1 && this != others[i]) {
            count++;
            sx += others[i]->x;
            sy += others[i]->y;
        }
    }

    if (count > 0) {
        sx = x - sx / count;
        sy = y - sy / count;

        float l = sqrt(sq(sx) + sq(sy));
        sx = sx / l;
        sy = sy / l;

    }

    vx -= sx;
    vy -= sy;
}


void boids::draw() {
    ofSetLineWidth(2);
    glPushMatrix();
    glTranslatef(x, y, 0);
    float alpha = atan(vy / vx) + (vx < 0 ? PI : 0);

    glRotatef(alpha * 180 / PI, 0, 0, 1);
    ofSetColor(228, 228, 228);
    //ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
    ofFill();

    ofBeginShape();
    ofVertex(-15, -5);
    ofVertex(-15, 5);
    ofVertex(15, 5);
    ofVertex(15, -5);
    ofEndShape(true);
    ofSetColor(128, 128, 128);
    ofNoFill();

    ofBeginShape();
    ofVertex(-15, -5);
    ofVertex(-15, 5);
    ofVertex(15, 5);
    ofVertex(15, -5);
    ofEndShape(true);


    glPopMatrix();

}