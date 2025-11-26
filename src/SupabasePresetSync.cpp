// file: src/SupabasePresetSync.cpp

#include "SupabasePresetSync.h"
#include <exception>

//--------------------------------------------------------------
void SupabasePresetSync::setup(SupabaseClient* _client, const string& _userId) {
	client = _client;
	userId = _userId;
	
	if (!client || !client->isConfigured()) {
		ofLogError("SupabasePresetSync") << "Client not configured";
		return;
	}
	
	ofLogNotice("SupabasePresetSync") << "Setup complete for user: " << userId;
}

//--------------------------------------------------------------
void SupabasePresetSync::syncWithPresetsManager(SurfingPresetsLiteOfxGui& pm) {
	presetsManager = &pm;
	
	// Listen to preset changes
	presetsManager->index.addListener(this, &SupabasePresetSync::onPresetSaved);
	
	ofLogNotice("SupabasePresetSync") << "Connected to presetsManager";
	
	// Initial pull from Supabase
	pullAllPresets();
}

//--------------------------------------------------------------
void SupabasePresetSync::pullAllPresets() {
	if (!client || !client->isConfigured()) {
		ofLogError("SupabasePresetSync") << "Cannot pull: client not configured";
		return;
	}
	
	bIsSyncing = true;
	pendingOps++;
	
	string query = "user_id=eq." + userId + "&order=created_at.desc";
	
	ofLogNotice("SupabasePresetSync") << "Pulling all presets...";
	
	client->selectAsync("presets", query,
		[this](ofJson response) {
			pendingOps--;
			
			if (!response.is_array()) {
				ofLogError("SupabasePresetSync") << "Invalid response format";
				bIsSyncing = false;
				return;
			}
			
			ofLogNotice("SupabasePresetSync") << "Received " << response.size() << " presets";
			
			// Save each preset to local JSON files
			for (auto& preset : response) {
				try {
					string name = preset["preset_name"].get<string>();
					ofJson data = preset["preset_data"];
					savePresetToLocal(name, data);
				} catch (std::exception& e) {
					ofLogError("SupabasePresetSync") << "Error processing preset: " << e.what();
				}
			}
			
			lastSyncTime = ofGetTimestampString();
			bIsSyncing = false;
			
			ofNotifyEvent(onSyncComplete);
			ofLogNotice("SupabasePresetSync") << "Sync complete at " << lastSyncTime;
		},
		[this](string error) {
			pendingOps--;
			bIsSyncing = false;
			ofLogError("SupabasePresetSync") << "Pull failed: " << error;
			ofNotifyEvent(onSyncError, error);
		}
	);
}

//--------------------------------------------------------------
void SupabasePresetSync::pushPreset(const string& name, const ofJson& data) {
	if (!client || !client->isConfigured()) {
		ofLogError("SupabasePresetSync") << "Cannot push: client not configured";
		return;
	}
	
	pendingOps++;
	
	ofJson payload = {
		{"user_id", userId},
		{"preset_name", name},
		{"preset_data", data}
	};
	
	ofLogNotice("SupabasePresetSync") << "Pushing preset: " << name;
	
	// Try insert first, if fails (duplicate), update
	client->insertAsync("presets", payload,
		[this, name](ofJson response) {
			pendingOps--;
			ofLogNotice("SupabasePresetSync") << "Preset inserted: " << name;
		},
		[this, name, data](string error) {
			// If insert fails (likely duplicate), try update
			if (error.find("409") != string::npos || error.find("duplicate") != string::npos) {
				ofLogVerbose("SupabasePresetSync") << "Preset exists, updating: " << name;
				
				string filter = "user_id=eq." + userId + "&preset_name=eq." + name;
				ofJson updateData = {{"preset_data", data}};
				
				client->updateAsync("presets", filter, updateData,
					[this, name]() {
						pendingOps--;
						ofLogNotice("SupabasePresetSync") << "Preset updated: " << name;
					},
					[this](string updateError) {
						pendingOps--;
						ofLogError("SupabasePresetSync") << "Update failed: " << updateError;
					}
				);
			} else {
				pendingOps--;
				ofLogError("SupabasePresetSync") << "Insert failed: " << error;
			}
		}
	);
}

//--------------------------------------------------------------
void SupabasePresetSync::pushCurrentPreset() {
	if (!presetsManager) return;
	
	string name = getCurrentPresetName();
	ofJson data = getCurrentPresetData();
	
	pushPreset(name, data);
}

//--------------------------------------------------------------
void SupabasePresetSync::deleteRemotePreset(const string& name) {
	if (!client || !client->isConfigured()) return;
	
	pendingOps++;
	
	string filter = "user_id=eq." + userId + "&preset_name=eq." + name;
	
	ofLogNotice("SupabasePresetSync") << "Deleting preset: " << name;
	
	client->deleteAsync("presets", filter,
		[this, name]() {
			pendingOps--;
			ofLogNotice("SupabasePresetSync") << "Preset deleted: " << name;
		},
		[this](string error) {
			pendingOps--;
			ofLogError("SupabasePresetSync") << "Delete failed: " << error;
		}
	);
}

//--------------------------------------------------------------
void SupabasePresetSync::onPresetSaved(int& index) {
	if (!presetsManager) return;
	
	// When user saves a preset, auto-push to Supabase
	ofLogVerbose("SupabasePresetSync") << "Preset saved locally, index: " << index;
	
	// Wait a moment to ensure local file is written
	ofSleepMillis(100);
	
	pushCurrentPreset();
}

//--------------------------------------------------------------
void SupabasePresetSync::onPresetDeleted(int& index) {
	// TODO: Implement if presetsManager exposes delete events
}

//--------------------------------------------------------------
string SupabasePresetSync::getCurrentPresetName() {
	if (!presetsManager) return "";
	
	int idx = presetsManager->index.get();
	return "preset_" + ofToString(idx, 3, '0');
}

//--------------------------------------------------------------
ofJson SupabasePresetSync::getCurrentPresetData() {
	if (!presetsManager) return ofJson::object();
	
	// For now, return empty - TODO: integrate with presetsManager properly
	return ofJson::object();
}

//--------------------------------------------------------------
void SupabasePresetSync::savePresetToLocal(const string& name, const ofJson& data) {
	if (!presetsManager) return;
	
	// TODO: Save to local - needs proper integration with presetsManager
	ofLogVerbose("SupabasePresetSync") << "Saving to local: " << name << " (stub)";
}
