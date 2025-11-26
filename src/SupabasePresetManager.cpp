// file: src/SupabasePresetManager.cpp

#include "SupabasePresetManager.h"
#include <exception>

//--------------------------------------------------------------
void SupabasePresetManager::setup(SupabaseClient* _client, const string& _userId) {
	client = _client;
	userId = _userId;
	
	if (!client || !client->isConfigured()) {
		ofLogError("SupabasePresetManager") << "Client not configured";
		return;
	}
	
	ofLogNotice("SupabasePresetManager") << "Setup complete for user: " << userId;
	
	// Load initial preset list
	refreshPresetList();
}

//--------------------------------------------------------------
void SupabasePresetManager::refreshPresetList() {
	if (!client || !client->isConfigured()) {
		string error = "Cannot refresh: client not configured";
		ofLogError("SupabasePresetManager") << error;
		ofNotifyEvent(onError, error);
		return;
	}
	
	bIsLoading = true;
	
	string query = "user_id=eq." + userId + "&order=created_at.desc";
	
	ofLogNotice("SupabasePresetManager") << "Refreshing preset list...";
	
	client->selectAsync("presets", query,
		[this](ofJson response) {
			bIsLoading = false;
			presets.clear();
			
			if (!response.is_array()) {
				ofLogError("SupabasePresetManager") << "Invalid response format";
				return;
			}
			
			for (auto& item : response) {
				try {
					PresetInfo info;
					info.id = item["id"].get<string>();
					info.name = item["preset_name"].get<string>();
					info.data = item["preset_data"];
					info.createdAt = item.contains("created_at") ? item["created_at"].get<string>() : "";
					info.updatedAt = item.contains("updated_at") ? item["updated_at"].get<string>() : "";
					
					presets.push_back(info);
				} catch (std::exception& e) {
					ofLogError("SupabasePresetManager") << "Error parsing preset: " << e.what();
				}
			}
			
			ofLogNotice("SupabasePresetManager") << "Loaded " << presets.size() << " presets";
		},
		[this](string error) {
			bIsLoading = false;
			ofLogError("SupabasePresetManager") << "Refresh failed: " << error;
			ofNotifyEvent(onError, error);
		}
	);
}

//--------------------------------------------------------------
void SupabasePresetManager::loadPreset(const string& name) {
	if (!client || !client->isConfigured()) return;
	
	// Find preset in list
	for (auto& preset : presets) {
		if (preset.name == name) {
			ofLogNotice("SupabasePresetManager") << "Loading preset: " << name;
			ofNotifyEvent(onPresetLoaded, preset);
			return;
		}
	}
	
	ofLogWarning("SupabasePresetManager") << "Preset not found: " << name;
}

//--------------------------------------------------------------
void SupabasePresetManager::savePreset(const string& name, const ofJson& data) {
	if (!client || !client->isConfigured()) return;
	
	ofJson payload = {
		{"user_id", userId},
		{"preset_name", name},
		{"preset_data", data}
	};
	
	ofLogNotice("SupabasePresetManager") << "Saving preset: " << name;
	
	// Try insert first
	client->insertAsync("presets", payload,
		[this, name](ofJson response) {
			ofLogNotice("SupabasePresetManager") << "Preset saved: " << name;
			string tempName = name;
			ofNotifyEvent(onPresetSaved, tempName);
			refreshPresetList();
		},
		[this, name, data](string error) {
			// If duplicate, update instead
			if (error.find("409") != string::npos || error.find("duplicate") != string::npos) {
				ofLogVerbose("SupabasePresetManager") << "Preset exists, updating: " << name;
				
				string filter = "user_id=eq." + userId + "&preset_name=eq." + name;
				ofJson updateData = {{"preset_data", data}};
				
				client->updateAsync("presets", filter, updateData,
					[this, name]() {
						ofLogNotice("SupabasePresetManager") << "Preset updated: " << name;
						string tempName = name;
						ofNotifyEvent(onPresetSaved, tempName);
						refreshPresetList();
					},
					[this](string updateError) {
						ofLogError("SupabasePresetManager") << "Update failed: " << updateError;
						ofNotifyEvent(onError, updateError);
					}
				);
			} else {
				ofLogError("SupabasePresetManager") << "Save failed: " << error;
				ofNotifyEvent(onError, error);
			}
		}
	);
}

//--------------------------------------------------------------
void SupabasePresetManager::deletePreset(const string& name) {
	if (!client || !client->isConfigured()) return;
	
	string filter = "user_id=eq." + userId + "&preset_name=eq." + name;
	
	ofLogNotice("SupabasePresetManager") << "Deleting preset: " << name;
	
	client->deleteAsync("presets", filter,
		[this, name]() {
			ofLogNotice("SupabasePresetManager") << "Preset deleted: " << name;
			string tempName = name;
			ofNotifyEvent(onPresetDeleted, tempName);
			refreshPresetList();
		},
		[this](string error) {
			ofLogError("SupabasePresetManager") << "Delete failed: " << error;
			ofNotifyEvent(onError, error);
		}
	);
}

//--------------------------------------------------------------
void SupabasePresetManager::renamePreset(const string& oldName, const string& newName) {
	if (!client || !client->isConfigured()) return;
	
	// Find the preset data
	for (auto& preset : presets) {
		if (preset.name == oldName) {
			// Save with new name
			savePreset(newName, preset.data);
			// Delete old one
			deletePreset(oldName);
			return;
		}
	}
}

//--------------------------------------------------------------
void SupabasePresetManager::drawGui(int x, int y) {
	int width = 300;
	
	ofPushStyle();
	ofSetColor(40);
	ofDrawRectangle(x, y, width, 400);
	
	ofSetColor(255);
	ofDrawBitmapString("SUPABASE PRESET MANAGER", x + 10, y + 20);
	
	drawPresetList(x, y + 30, width);
	drawControls(x, y + 280, width);
	drawDialogs();
	
	ofPopStyle();
}

//--------------------------------------------------------------
void SupabasePresetManager::drawPresetList(int x, int y, int width) {
	// List background
	ofPushStyle();
	ofSetColor(30);
	ofDrawRectangle(x + 10, y, width - 20, 240);
	
	// Loading indicator
	if (bIsLoading) {
		ofSetColor(255, 200, 0);
		ofDrawBitmapString("Loading...", x + 20, y + 120);
		ofPopStyle();
		return;
	}
	
	// Presets list
	int py = y + 15;
	for (int i = 0; i < presets.size(); i++) {
		bool isSelected = (i == selectedIndex);
		
		if (isSelected) {
			ofSetColor(60, 120, 200);
			ofDrawRectangle(x + 15, py - 12, width - 30, 15);
		}
		
		ofSetColor(isSelected ? 255 : 200);
		ofDrawBitmapString(presets[i].name, x + 20, py);
		
		py += 18;
		
		if (py > y + 230) break; // Don't overflow
	}
	
	ofPopStyle();
}

//--------------------------------------------------------------
void SupabasePresetManager::drawControls(int x, int y, int width) {
	ofPushStyle();
	
	int buttonWidth = (width - 40) / 3;
	int buttonHeight = 30;
	int spacing = 10;
	
	// Load button
	bool canLoad = selectedIndex >= 0 && selectedIndex < presets.size();
	ofSetColor(canLoad ? ofColor(60, 180, 60) : ofColor(60));
	ofDrawRectangle(x + 10, y, buttonWidth, buttonHeight);
	ofSetColor(canLoad ? 255 : 100);
	ofDrawBitmapString("LOAD", x + 15, y + 20);
	
	// Save button
	ofSetColor(80, 120, 200);
	ofDrawRectangle(x + 10 + buttonWidth + spacing, y, buttonWidth, buttonHeight);
	ofSetColor(255);
	ofDrawBitmapString("SAVE", x + 15 + buttonWidth + spacing, y + 20);
	
	// Delete button
	bool canDelete = selectedIndex >= 0 && selectedIndex < presets.size();
	ofSetColor(canDelete ? ofColor(200, 60, 60) : ofColor(60));
	ofDrawRectangle(x + 10 + 2 * (buttonWidth + spacing), y, buttonWidth, buttonHeight);
	ofSetColor(canDelete ? 255 : 100);
	ofDrawBitmapString("DELETE", x + 15 + 2 * (buttonWidth + spacing), y + 20);
	
	// Refresh button
	ofSetColor(100, 100, 100);
	ofDrawRectangle(x + 10, y + buttonHeight + spacing, width - 20, buttonHeight);
	ofSetColor(255);
	ofDrawBitmapString("REFRESH LIST", x + 15, y + buttonHeight + spacing + 20);
	
	// Help text
	ofSetColor(150);
	int ty = y + 2 * (buttonHeight + spacing) + 10;
	ofDrawBitmapString("Click preset to select", x + 15, ty);
	
	ofPopStyle();
}

//--------------------------------------------------------------
void SupabasePresetManager::drawDialogs() {
	// TODO: Implement modal dialogs for save/rename/delete
	// For now, operations work via code directly
}
