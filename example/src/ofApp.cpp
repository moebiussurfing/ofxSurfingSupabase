#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofLogNotice("ofApp") << "setup()";

	ofSetWindowTitle("ofxSurfingSupabase Example");
	ofSetFrameRate(60);

// Move window
#if 1
	//ofSetWindowPosition(-ofGetWidth()-100, 100);//left monitor
	ofSetWindowPosition(3840 + 2160 + 100, 100); //right 4th FHD monitor
#endif

	// Setup database
	db.setup(scene.params);

	// Gui scene
	guiScene.setup(scene.params);
	guiScene.setPosition(ofGetWidth() - guiScene.getWidth() - 10, 10);
	guiScene.add(vRandomize);

	// Scene randomizer
	e_vRandomize = vRandomize.newListener([this]() {
		ofxSurfing::doRandomize(scene.params);
	});

	ofLogNotice("ofApp") << "setup(): Setup complete";
}

//--------------------------------------------------------------
void ofApp::update() {
	db.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(0);

	// Draw scene
	scene.draw();

	// Draw database UI
	db.draw();

	// UI Debug
	if (!db.bGui) return;

	// Scene UI
	guiScene.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	db.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::exit() {
	ofLogNotice("ofApp") << "exit()";
	db.exit();
}
