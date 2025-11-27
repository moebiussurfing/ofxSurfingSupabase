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
	
	// Add button listeners
	btnSendToRemote.addListener(this, &ofxSurfingSupabase::onBtnSendToRemote);
	btnSaveSceneDirect.addListener(this, &ofxSurfingSupabase::onBtnSaveSceneDirect);
	btnLoadFromRemote.addListener(this, &ofxSurfingSupabase::onBtnLoadFromRemote);
	
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
	
	// Listen to preset manager events
	ofAddListener(presetManager.onPresetLoaded, this, &ofxSurfingSupabase::onRemotePresetLoaded);
	
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
		presetManager.drawGui();
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::exit() {
	ofLogNotice("ofxSurfingSupabase") << "Exit";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupSceneParams(ofParameterGroup& sceneParams) {
	sceneParamsPtr = &sceneParams;
	ofLogNotice("ofxSurfingSupabase") << "Scene params linked: " << sceneParams.getName();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::syncWithPresetsManager(SurfingPresetsLiteOfxGui& pm) {
	if (!isConnected()) {
		ofLogError("ofxSurfingSupabase") << "Cannot sync: not connected";
		return;
	}
	
	presetsManagerPtr = &pm; // Store pointer for later use
	
	if (bAutoSync) {
		sync.syncWithPresetsManager(pm);
		ofLogNotice("ofxSurfingSupabase") << "Auto-sync enabled with presetsManager";
	} else {
		ofLogNotice("ofxSurfingSupabase") << "Manual mode - use 'Send to Remote' button";
	}
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

//--------------------------------------------------------------
void ofxSurfingSupabase::sendCurrentToRemote() {
	if (!presetsManagerPtr) {
		ofLogError("ofxSurfingSupabase") << "PresetsManager not connected";
		return;
	}
	
	// Get current preset index
	int idx = presetsManagerPtr->index.get();
	
	// Build preset file path (standard presetsLite format)
	string filename = ofToString(idx, 2, '0') + ".json";
	string path = "Kit-00/" + filename; // Default kit folder
	
	ofFile presetFile(path);
	if (!presetFile.exists()) {
		ofLogError("ofxSurfingSupabase") << "Preset file not found: " << path;
		ofLogError("ofxSurfingSupabase") << "Save preset locally first!";
		return;
	}
	
	// Load JSON from file
	ofJson settings = ofLoadJson(path);
	
	// Generate remote preset name
	string name = "preset_" + ofToString(idx, 3, '0');
	
	ofLogNotice("ofxSurfingSupabase") << "Sending to remote: " << name << " from " << path;
	
	// Save to Supabase
	presetManager.savePreset(name, settings);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::loadFromRemote() {
	string name = presetManager.getSelectedPresetName();
	if (name == "none") {
		ofLogWarning("ofxSurfingSupabase") << "No remote preset selected";
		return;
	}
	
	ofLogNotice("ofxSurfingSupabase") << "Loading from remote: " << name;
	presetManager.loadPreset(name);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onBtnSendToRemote() {
	sendCurrentToRemote();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onBtnSaveSceneDirect() {
	sendSceneDirect();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onBtnLoadFromRemote() {
	loadFromRemote();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::sendSceneDirect() {
	if (!sceneParamsPtr) {
		ofLogError("ofxSurfingSupabase") << "Scene params not set! Call setupSceneParams() first";
		return;
	}
	
	// Serialize scene params directly to JSON
	ofJson settings;
	ofSerialize(settings, *sceneParamsPtr);
	
	// Generate unique name with timestamp
	string timestamp = ofGetTimestampString("%Y%m%d_%H%M%S");
	string name = "scene_" + timestamp;
	
	ofLogNotice("ofxSurfingSupabase") << "Saving scene direct: " << name;
	
	// Save to Supabase
	presetManager.savePreset(name, settings);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onRemotePresetLoaded(PresetInfo& info) {
	if (!presetsManagerPtr) return;
	
	ofLogNotice("ofxSurfingSupabase") << "Remote preset loaded: " << info.name;
	ofLogNotice("ofxSurfingSupabase") << "Save to local file and use presetsManager to load it";
	
	// Save to a temp file in the kit folder
	string tempFilename = "remote_" + info.name + ".json";
	string path = "Kit-00/" + tempFilename;
	
	bool saved = ofSavePrettyJson(path, info.data);
	
	if (saved) {
		ofLogNotice("ofxSurfingSupabase") << "Saved remote preset to: " << path;
		ofLogNotice("ofxSurfingSupabase") << "Load it manually from presetsManager UI";
	} else {
		ofLogError("ofxSurfingSupabase") << "Failed to save remote preset";
	}
}
