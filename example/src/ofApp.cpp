#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofLogNotice("ofApp") << "setup()";
  
  ofSetFrameRate(60);
  ofSetWindowTitle("ofxSurfingSupabase Example");
  //ofSetWindowPosition(-ofGetWidth()-100, 100);//left monitor
  ofSetWindowPosition(3840+2160+100, 100);//right 4th monitor

  // Setup scene
  scene.params.setName("Scene");
  
  // Setup database
  db.setup(scene.params);
  db.bRemoteMode = true; // Pure remote mode

  guiScene.setup(scene.params);
  guiScene.setPosition(ofGetWidth() - guiScene.getWidth() - 10, 10);

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
  guiScene.draw();
  
  // Draw database UI
  db.draw();
  
  // Draw instructions
  int y = ofGetHeight() - 80;
  ofSetColor(255);
  ofDrawBitmapStringHighlight("CONTROLS:", 20, y);
  ofDrawBitmapStringHighlight("Space: Save Scene Direct", 20, y + 20);
  ofDrawBitmapStringHighlight("L: Load & Apply Remote", 20, y + 40);
  ofDrawBitmapStringHighlight("Left/Right: Browse Presets", 20, y + 60);
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
