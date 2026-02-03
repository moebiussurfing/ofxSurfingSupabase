#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {

	int w = 1500;
	ofSetupOpenGL(w, w * (9.f / 16.f), OF_WINDOW); // <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
