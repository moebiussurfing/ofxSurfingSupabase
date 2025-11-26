// file: src/ofxSurfingSupabase.cpp

#include "ofxSurfingSupabase.h"

//--------------------------------------------------------------
void ofxSurfingSupabase::setup() {
	ofLogNotice("ofxSurfingSupabase") << "Setup";
	
	// Load credentials
	if (!config.loadFromFile("credentials.txt")) {
		ofLogError("ofxSurfingSupabase") << "Failed to load credentials.txt";
		ofLogError("ofxSurfingSupabase") << "Create bin/data/credentials.txt with:";
		ofLogError("ofxSurfingSupabase") << "  Option 1 (API Key):";
		ofLogError("ofxSurfingSupabase") << "    project_url=https://xxxxx.supabase.co";
		ofLogError("ofxSurfingSupabase") << "    api_key=your_api_key";
		ofLogError("ofxSurfingSupabase") << "    user_id=your_user_id";
		ofLogError("ofxSurfingSupabase") << "  Option 2 (Email/Password):";
		ofLogError("ofxSurfingSupabase") << "    project_url=https://xxxxx.supabase.co";
		ofLogError("ofxSurfingSupabase") << "    api_key=your_anon_key";
		ofLogError("ofxSurfingSupabase") << "    email=your@email.com";
		ofLogError("ofxSurfingSupabase") << "    password=your_password";
		return;
	}
	
	// Listen to auth events before setup
	ofAddListener(client.onAuthSuccess, this, &ofxSurfingSupabase::setupAfterAuth);
	ofAddListener(client.onAuthError, this, &ofxSurfingSupabase::onSyncError);
	
	// Setup client (will authenticate if needed)
	client.setup(config);
	
	// Setup GUI
	gui.setup(params);
	gui.setPosition(ofGetWidth() - 220, 10);
	
	ofLogNotice("ofxSurfingSupabase") << "Setup complete";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupAfterAuth(string& userId) {
	// This is called after authentication is successful
	ofLogNotice("ofxSurfingSupabase") << "Setting up sync for user: " << userId;
	
	// Setup sync
	sync.setup(&client, userId);
	
	// Setup preset manager
	presetManager.setup(&client, userId);
	
	// Listen to sync events
	ofAddListener(sync.onSyncComplete, this, &ofxSurfingSupabase::onSyncComplete);
	ofAddListener(sync.onSyncError, this, &ofxSurfingSupabase::onSyncError);
	
	ofLogNotice("ofxSurfingSupabase") << "Sync and PresetManager initialized";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::update() {
	// Nothing to update in main thread (async operations)
}

//--------------------------------------------------------------
void ofxSurfingSupabase::draw() {
	if (!bShowDebug) return;
	
	gui.draw();
	
	// Draw status info with color coding
	int x = gui.getPosition().x;
	int y = gui.getPosition().y + gui.getHeight() + 10;
	
	ofPushStyle();
	
	// Auth mode
	ofSetColor(200, 200, 255);
	ofDrawBitmapString("Auth: " + getAuthMode(), x, y);
	y += 20;
	
	// Connection status with color
	if (isConnected()) {
		ofSetColor(0, 255, 0);  // GREEN - Connected
		ofDrawBitmapString("Status: CONNECTED", x, y);
	} else if (client.isConfigured()) {
		ofSetColor(255, 200, 0);  // YELLOW - Authenticating
		ofDrawBitmapString("Status: AUTHENTICATING...", x, y);
	} else {
		ofSetColor(255, 0, 0);  // RED - Error
		ofDrawBitmapString("Status: ERROR - Check credentials.txt", x, y);
	}
	y += 20;
	
	// Sync info
	if (isConnected()) {
		if (isSyncing()) {
			ofSetColor(255, 200, 0);
			ofDrawBitmapString("Syncing...", x, y);
		} else if (!getLastSyncTime().empty()) {
			ofSetColor(150, 255, 150);
			ofDrawBitmapString("Last sync: " + getLastSyncTime(), x, y);
		}
		y += 20;
		
		if (getPendingOperations() > 0) {
			ofSetColor(255, 200, 0);
			ofDrawBitmapString("Pending: " + ofToString(getPendingOperations()), x, y);
		}
	}
	
	ofPopStyle();
	
	// Draw preset manager
	if (bShowPresetManager && isConnected()) {
		presetManager.drawGui(10, 10);
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::exit() {
	ofLogNotice("ofxSurfingSupabase") << "Exit";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::syncWithPresetsManager(SurfingPresetsLiteOfxGui& pm) {
	if (!isConnected()) {
		ofLogError("ofxSurfingSupabase") << "Cannot sync: not connected";
		return;
	}
	
	sync.syncWithPresetsManager(pm);
	ofLogNotice("ofxSurfingSupabase") << "Synced with presetsManager";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::syncNow() {
	sync.pullAllPresets();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::forcePull() {
	sync.pullAllPresets();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::pushCurrentPreset() {
	sync.pushCurrentPreset();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onSyncComplete() {
	ofLogNotice("ofxSurfingSupabase") << "Sync completed successfully";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onSyncError(string& error) {
	ofLogError("ofxSurfingSupabase") << "Sync error: " << error;
}
