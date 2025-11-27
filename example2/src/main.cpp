// example2/src/main.cpp

#include "ofMain.h"
#include "ofApp.h"

int main() {
	ofGLFWWindowSettings settings;
	settings.setSize(1280, 720);
	settings.windowMode = OF_WINDOW;
	
	auto window = ofCreateWindow(settings);
	auto app = make_shared<ofApp>();
	
	ofRunApp(window, app);
	return ofRunMainLoop();
}
