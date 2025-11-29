// file: src/ofxSurfingSupabase.cpp
// PURE REMOTE MODE - No presetsLite dependencies

#include "ofxSurfingSupabase.h"

//--------------------------------------------------------------
void ofxSurfingSupabase::setup() {
	ofLogNotice("ofxSurfingSupabase") << "Setup - PURE REMOTE MODE";
	
	// Load credentials
	if (!config.loadFromFile("credentials.txt")) {
		ofLogError("ofxSurfingSupabase") << "Failed to load credentials.txt";
		ofLogError("ofxSurfingSupabase") << "Create bin/data/credentials.txt with:";
		ofLogError("ofxSurfingSupabase") << "  project_url=https://xxxxx.supabase.co";
		ofLogError("ofxSurfingSupabase") << "  api_key=your_anon_key";
		ofLogError("ofxSurfingSupabase") << "  email=your@email.com";
		ofLogError("ofxSurfingSupabase") << "  password=your_password";
		return;
	}
	
	// Setup GUI FIRST (before client, so it's ready)
	gui.setup(params);
	
	// Button listeners
	btnSave.addListener(this, &ofxSurfingSupabase::onSaveScene);
	btnLoad.addListener(this, &ofxSurfingSupabase::onLoadAndApply);
	btnRefresh.addListener(this, &ofxSurfingSupabase::onRefreshList);
	btnPrev.addListener(this, &ofxSurfingSupabase::onPrevPreset);
	btnNext.addListener(this, &ofxSurfingSupabase::onNextPreset);
	btnDelete.addListener(this, &ofxSurfingSupabase::onDeleteSelected);
	btnClear.addListener(this, &ofxSurfingSupabase::onClearDatabase);
	bAutoApply.addListener(this, &ofxSurfingSupabase::onAutoApplyChanged);
	
	// Listen to auth events
	ofAddListener(client.onAuthSuccess, this, &ofxSurfingSupabase::setupAfterAuth);
	
	// Setup client (this will trigger authentication)
	client.setup(config);
	
	ofLogNotice("ofxSurfingSupabase") << "Setup complete - GUI ready";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupAfterAuth(string& userId) {
	ofLogNotice("ofxSurfingSupabase") << "Authenticated as: " << userId;
	
	// Setup preset manager
	presetManager.setup(&client, userId);
	
	// Initial refresh
	refreshList();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::update() {
	// Nothing needed
}

//--------------------------------------------------------------
void ofxSurfingSupabase::draw() {
	// if (!bShowGui) return;
	
	gui.setPosition(ofGetWidth() - gui.getWidth() - 10, 10);
	gui.draw();
	
	// Help text using ofxSurfingHelpersLite
	std::string helpText = "";
	helpText += "PURE REMOTE MODE\n";
	helpText += "No presetsLite - Direct DB sync\n";
	helpText += "\n";
	helpText += "1. Adjust sliders (Scene panel)\n";
	helpText += "2. Click 'Save Scene Direct'\n";
	helpText += "3. Browse with < > arrows\n";
	helpText += "4. Enable 'Auto Apply' to preview\n";
	helpText += "5. Click 'Load & Apply' to restore\n";
	helpText += "\n";
	helpText += "Status: " + string(isConnected() ? "CONNECTED" : "ERROR") + "\n";
	helpText += "Auth: " + getAuthMode() + "\n";
	helpText += "Presets: " + ofToString(presetManager.getPresets().size()) + "\n";
	helpText += "\n";
	helpText += "[G] Toggle GUI";
	ofxSurfing::ofDrawBitmapStringBox(helpText, &gui);
	
	// Preset browser
	if (isConnected()) {
		drawPresetBrowser();
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::drawPresetBrowser() {
	auto& presets = presetManager.getPresets();
	string selected = presetManager.getSelectedPresetName();
	
	// Build preset list string
	string listText = "";
	listText += "REMOTE PRESET BROWSER\n";
	listText += "====================\n";
	
	if (presets.empty()) {
		listText += "(no presets yet)\n";
	} else {
		int maxShow = 10;
		int count = 0;
		for (auto& preset : presets) {
			if (count >= maxShow) {
				listText += "... (" + ofToString(presets.size() - maxShow) + " more)\n";
				break;
			}
			
			bool isSelected = (preset.name == selected);
			
			// Mark selected with *
			if (isSelected) {
				listText += "* " + preset.name + "\n";
			} else {
				listText += "  " + preset.name + "\n";
			}
			
			count++;
		}
	}
	
	ofxSurfing::ofDrawBitmapStringBox(listText, &gui);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::exit() {
	ofLogNotice("ofxSurfingSupabase") << "Exit";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupSceneParams(ofParameterGroup& sceneParams) {
	sceneParamsPtr = &sceneParams;
	ofLogNotice("ofxSurfingSupabase") << "Scene linked: " << sceneParams.getName();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::saveSceneDirect() {
	if (!sceneParamsPtr) {
		ofLogError("ofxSurfingSupabase") << "Scene not linked!";
		return;
	}
	
	// Serialize scene → JSON
	ofJson json;
	ofSerialize(json, *sceneParamsPtr);
	
	// Generate name with timestamp
	string timestamp = ofGetTimestampString("%Y%m%d_%H%M%S");
	string name = "scene_" + timestamp;
	
	ofLogNotice("ofxSurfingSupabase") << "Saving: " << name;
	
	// Save to DB
	presetManager.savePreset(name, json);
	
	// Refresh list after a delay
	ofSleepMillis(500);
	refreshList();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::loadAndApply() {
	if (!sceneParamsPtr) {
		ofLogError("ofxSurfingSupabase") << "Scene not linked!";
		return;
	}
	
	string selected = presetManager.getSelectedPresetName();
	if (selected == "none") {
		ofLogWarning("ofxSurfingSupabase") << "No preset selected";
		return;
	}
	
	// Find preset in cached list
	auto& presets = presetManager.getPresets();
	for (auto& preset : presets) {
		if (preset.name == selected) {
			ofLogNotice("ofxSurfingSupabase") << "Loading: " << selected;
			
			try {
				ofDeserialize(preset.data, *sceneParamsPtr);
				ofLogNotice("ofxSurfingSupabase") << "✅ Applied!";
			} catch (std::exception& e) {
				ofLogError("ofxSurfingSupabase") << "Failed: " << e.what();
			}
			return;
		}
	}
	
	ofLogError("ofxSurfingSupabase") << "Not found: " << selected;
}

//--------------------------------------------------------------
void ofxSurfingSupabase::deleteSelected() {
	string selected = presetManager.getSelectedPresetName();
	if (selected == "none") {
		ofLogWarning("ofxSurfingSupabase") << "No preset selected";
		return;
	}
	
	ofLogNotice("ofxSurfingSupabase") << "Deleting: " << selected;
	presetManager.deletePreset(selected);
	
	// Refresh after delete
	ofSleepMillis(500);
	refreshList();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::clearDatabase() {
	ofLogWarning("ofxSurfingSupabase") << "CLEARING ALL PRESETS!";
	presetManager.clearDatabase();
	
	// Refresh after clear
	ofSleepMillis(500);
	refreshList();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::refreshList() {
	ofLogNotice("ofxSurfingSupabase") << "Refreshing preset list...";
	presetManager.refreshPresetList();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectNext() {
	presetManager.selectNext();
	
	if (bAutoApply) {
		loadAndApply();
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectPrevious() {
	presetManager.selectPrev();
	
	if (bAutoApply) {
		loadAndApply();
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onSaveScene() {
	saveSceneDirect();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onLoadAndApply() {
	loadAndApply();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onRefreshList() {
	refreshList();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onPrevPreset() {
	selectPrevious();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onNextPreset() {
	selectNext();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onDeleteSelected() {
	deleteSelected();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onClearDatabase() {
	clearDatabase();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::onAutoApplyChanged(bool& value) {
	if (value) {
		ofLogNotice("ofxSurfingSupabase") << "Auto Apply: ON - will apply on browse";
	} else {
		ofLogNotice("ofxSurfingSupabase") << "Auto Apply: OFF - manual load only";
	}
}
