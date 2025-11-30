#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofLogNotice("ofApp") << "setup()";
  
  ofSetFrameRate(60);
  ofSetWindowTitle("ofxSurfingSupabase Example");
  
  // Setup scene
  scene.params.setName("Scene");
  
  // Setup database
  db.setup();
  db.setupSceneParams(scene.params);
  db.bRemoteMode = true; // Pure remote mode
  
  ofLogNotice("ofApp") << "Setup complete";
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
  
  // Draw instructions
  int y = ofGetHeight() - 80;
  ofSetColor(255);
  ofDrawBitmapString("CONTROLS:", 20, y);
  ofDrawBitmapString("Space: Save Scene Direct", 20, y + 20);
  ofDrawBitmapString("L: Load & Apply Remote", 20, y + 40);
  ofDrawBitmapString("Left/Right: Browse Presets", 20, y + 60);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key == ' ') {
    db.sendSceneDirect();
  } else if (key == 'l' || key == 'L') {
    db.loadAndApplyRemote();
  } else if (key == OF_KEY_LEFT) {
    db.selectPrevious();
  } else if (key == OF_KEY_RIGHT) {
    db.selectNext();
  } else if (key == 'r' || key == 'R') {
    db.refreshPresetList();
  } else if (key == 'd' || key == 'D') {
    if (ofGetKeyPressed(OF_KEY_SHIFT)) {
      db.clearDatabase();
    }
  }
}

//--------------------------------------------------------------
void ofApp::exit() {
  ofLogNotice("ofApp") << "exit()";
  db.exit();
}
