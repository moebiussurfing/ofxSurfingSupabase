#include "ofxSurfingSupabase.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../libs/cpp-httplib/include/httplib.h"

// Constants
const std::string ofxSurfingSupabase::CREDENTIALS_PATH = "credentials.txt";
const std::string ofxSurfingSupabase::TABLE_NAME = "presets"; //TODO: to be used as kit name, alowing multiple kits
//TODO: add tag to be used as kit name filtering for multiple kits

//--------------------------------------------------------------
void ofxSurfingSupabase::setup(ofParameterGroup & sceneParams) {
	ofLogNotice("ofxSurfingSupabase") << "setup(" << sceneParams.getName() << ")";

	setup();
	setupPresetParameters(sceneParams);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setup() {
	ofLogNotice("ofxSurfingSupabase") << "setup()";

	bConnected = false;
	sceneParams_ = nullptr;
	selectedPresetIndexRemote = -1;

	setupParameters();
	setupCallbacks();
	setupGui();
	startup();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupParameters() {
	ofLogNotice("ofxSurfingSupabase") << "setupParameters()";

	params_.setName("Supabase");
	params_.add(bConnected);
	params_.add(vReconnect);
	params_.add(vRefreshListRemote);
	params_.add(vClearDatabase);
	params_.add(bKeys);
	params_.add(bDebug);
	params_.add(bGui);

	paramsManager_.setName("Preset Manager");
	paramsManager_.add(selectedPresetIndexRemote);
	paramsManager_.add(vSelectNextRemote);
	paramsManager_.add(vSelectPreviousRemote);
	paramsManager_.add(bAutoLoad);
	paramsManager_.add(vSaveToRemote);
	paramsManager_.add(vSaveNewRemote);
	paramsManager_.add(vLoadFromRemote);
	paramsManager_.add(vDeleteSelectedRemote);

	params_.add(paramsManager_);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupCallbacks() {
	ofLogNotice("ofxSurfingSupabase") << "setupCallbacks()";

	e_vReconnect = vReconnect.newListener([this]() {
		ofLogNotice("ofxSurfingSupabase") << "Reconnect triggered";
		if (loadCredentials()) {
			authenticate();
			if (bConnected) {
				refreshPresetListRemote();
			}
		}
	});

	e_vSaveToRemote = vSaveToRemote.newListener([this]() {
		if (selectedPresetIndexRemote >= 0 && selectedPresetIndexRemote < presetsNamesRemote.size()) {
			savePreset(presetsNamesRemote[selectedPresetIndexRemote.get()]);
		}
	});

	e_vSaveNewRemote = vSaveNewRemote.newListener([this]() {
		savePresetNew(generateTimestampName());
	});

	e_vLoadFromRemote = vLoadFromRemote.newListener([this]() {
		if (selectedPresetIndexRemote >= 0 && selectedPresetIndexRemote < presetsNamesRemote.size()) {
			loadPreset(presetsNamesRemote[selectedPresetIndexRemote.get()]);
		}
	});

	e_vRefreshListRemnote = vRefreshListRemote.newListener([this]() {
		refreshPresetListRemote();
	});

	e_vDeleteSelected = vDeleteSelectedRemote.newListener([this]() {
		if (selectedPresetIndexRemote >= 0 && selectedPresetIndexRemote < presetsNamesRemote.size()) {
			deletePresetRemote(presetsNamesRemote[selectedPresetIndexRemote.get()]);
		}
	});

	e_vClearDatabase = vClearDatabase.newListener([this]() {
		clearDatabase();
	});

	e_vSelectNextRemote = vSelectNextRemote.newListener([this]() {
		selectNextRemote();
	});

	e_vSelectPreviousRemote = vSelectPreviousRemote.newListener([this]() {
		selectPreviousRemote();
	});

	e_selectedPresetIndexRemote = selectedPresetIndexRemote.newListener([this](int & i) {
		selectedIndexRemoteUpdate();
	});
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupGui() {
	ofLogNotice("ofxSurfingSupabase") << "setupGui()";

	gui_.setup(params_.getName());
	gui_.add(params_);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::startup() {
	ofLogNotice("ofxSurfingSupabase") << "startup()";

	if (loadCredentials()) {
		authenticate();
		if (bConnected) {
			refreshPresetListRemote();
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::exit() {
	ofLogNotice("ofxSurfingSupabase") << "exit()";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupPresetParameters(ofParameterGroup & sceneParams) {
	ofLogNotice("ofxSurfingSupabase") << "setupPresetParameters()";
	sceneParams_ = &sceneParams;
}

//--------------------------------------------------------------
bool ofxSurfingSupabase::loadCredentials() {
	ofLogNotice("ofxSurfingSupabase") << "loadCredentials()";

	std::string path = ofToDataPath(CREDENTIALS_PATH);
	ofFile file(path);

	if (!file.exists()) {
		ofLogError("ofxSurfingSupabase") << "Credentials file not found: " << path;
		return false;
	}

	ofBuffer buffer = file.readToBuffer();
	for (auto & line : buffer.getLines()) {
		if (line.empty() || line[0] == '#') continue;

		std::size_t pos = line.find('=');
		if (pos == std::string::npos) continue;

		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		// Trim whitespace
		value.erase(0, value.find_first_not_of(" \t\r\n"));
		value.erase(value.find_last_not_of(" \t\r\n") + 1);

		if (key == "AUTH_MODE")
			config_.authMode = value;
		else if (key == "SUPABASE_URL") {
			config_.supabaseUrl = value;
			// Remove trailing slashes
			while (!config_.supabaseUrl.empty() && config_.supabaseUrl.back() == '/') {
				config_.supabaseUrl.pop_back();
			}
		} else if (key == "SUPABASE_ANON_KEY")
			config_.supabaseAnonKey = value;
		else if (key == "EMAIL")
			config_.email = value;
		else if (key == "PASSWORD")
			config_.password = value;
	}

	if (bDebug) {
		ofLogNotice("ofxSurfingSupabase") << "Auth mode: " << config_.authMode;
		ofLogNotice("ofxSurfingSupabase") << "URL: " << config_.supabaseUrl;
	}

	return config_.isValid();
}

//--------------------------------------------------------------
bool ofxSurfingSupabase::authenticate() {
	ofLogNotice("ofxSurfingSupabase") << "authenticate()";

	// Check auth mode
	if (config_.authMode == "ANON_KEY") {
		// Simple mode: just use anon key, no email/password
		ofLogNotice("ofxSurfingSupabase") << "Using ANON_KEY authentication (no user login)";

		// Use anon key as auth token
		authToken_ = config_.supabaseAnonKey;
		userId_ = "anonymous"; // No real user ID
		bConnected = true;

		ofLogNotice("ofxSurfingSupabase") << "✓ Connected with ANON_KEY";
		return true;
	} else if (config_.authMode == "EMAIL_PASSWORD") {
		// Full auth mode: login with email/password
		ofLogNotice("ofxSurfingSupabase") << "Using EMAIL_PASSWORD authentication";

		try {
			// Extract host from URL
			std::string host = config_.supabaseUrl;
			if (host.find("https://") == 0) {
				host = host.substr(8);
			} else if (host.find("http://") == 0) {
				host = host.substr(7);
			}

			// Create HTTPS client
			httplib::SSLClient client(host);
			client.set_connection_timeout(10, 0);
			client.set_read_timeout(10, 0);
			client.set_write_timeout(10, 0);
			client.enable_server_certificate_verification(false);

			// Build auth request
			ofJson authData;
			authData["email"] = config_.email;
			authData["password"] = config_.password;
			std::string body = authData.dump();

			// Set headers
			httplib::Headers headers = {
				{ "Content-Type", "application/json" },
				{ "apikey", config_.supabaseAnonKey }
			};

			// Perform authentication
			auto res = client.Post("/auth/v1/token?grant_type=password", headers, body, "application/json");

			if (res && res->status == 200) {
				try {
					ofJson responseJson = ofJson::parse(res->body);
					authToken_ = responseJson["access_token"].get<std::string>();
					userId_ = responseJson["user"]["id"].get<std::string>();
					bConnected = true;

					ofLogNotice("ofxSurfingSupabase") << "✓ Authenticated successfully";
					if (bDebug) {
						ofLogNotice("ofxSurfingSupabase") << "User ID: " << userId_;
					}

					return true;
				} catch (std::exception & e) {
					ofLogError("ofxSurfingSupabase") << "Failed to parse auth response: " << e.what();
				}
			} else {
				std::string errorMsg = res ? res->body : "Connection failed";
				ofLogError("ofxSurfingSupabase") << "✗ Authentication failed: HTTP " << (res ? res->status : 0);
				if (!res) {
					ofLogError("ofxSurfingSupabase") << "Error: " << httplib::to_string(res.error());
				}
				if (bDebug) {
					ofLogError("ofxSurfingSupabase") << errorMsg;
				}
			}
		} catch (std::exception & e) {
			ofLogError("ofxSurfingSupabase") << "Exception during authentication: " << e.what();
		}
	} else {
		ofLogError("ofxSurfingSupabase") << "Unknown AUTH_MODE: " << config_.authMode;
		ofLogError("ofxSurfingSupabase") << "Supported modes: ANON_KEY, EMAIL_PASSWORD";
	}

	bConnected = false;
	return false;
}

//--------------------------------------------------------------
ofxSurfingSupabase::HttpResponse ofxSurfingSupabase::httpGet(const std::string & endpoint) {
	HttpResponse result;
	result.success = false;

	if (!bConnected) {
		result.statusCode = 0;
		result.body = "Not connected";
		return result;
	}

	try {
		std::string host = config_.supabaseUrl;
		if (host.find("https://") == 0) {
			host = host.substr(8);
		} else if (host.find("http://") == 0) {
			host = host.substr(7);
		}

		if (bDebug) {
			ofLogNotice("ofxSurfingSupabase") << "HTTP GET: " << endpoint;
			ofLogNotice("ofxSurfingSupabase") << "Host: " << host;
		}

		httplib::SSLClient client(host);
		client.set_connection_timeout(10, 0);
		client.set_read_timeout(10, 0);
		client.enable_server_certificate_verification(false); // Disable cert verification for testing

		httplib::Headers headers = {
			{ "apikey", config_.supabaseAnonKey },
			{ "Authorization", "Bearer " + authToken_ }
		};

		auto res = client.Get(endpoint.c_str(), headers);

		if (res) {
			result.statusCode = res->status;
			result.body = res->body;
			result.success = (res->status >= 200 && res->status < 300);

			if (bDebug) {
				ofLogNotice("ofxSurfingSupabase") << "Response: HTTP " << result.statusCode;
			}
		} else {
			auto err = res.error();
			ofLogError("ofxSurfingSupabase") << "HTTP GET failed - Error: " << httplib::to_string(err);
			result.statusCode = 0;
			result.body = "Connection error: " + std::string(httplib::to_string(err));
		}
	} catch (std::exception & e) {
		ofLogError("ofxSurfingSupabase") << "HTTP GET exception: " << e.what();
	}

	return result;
}

//--------------------------------------------------------------
ofxSurfingSupabase::HttpResponse ofxSurfingSupabase::httpPost(const std::string & endpoint, const std::string & jsonBody) {
	HttpResponse result;
	result.success = false;

	if (!bConnected) {
		result.statusCode = 0;
		result.body = "Not connected";
		return result;
	}

	try {
		std::string host = config_.supabaseUrl;
		if (host.find("https://") == 0) {
			host = host.substr(8);
		} else if (host.find("http://") == 0) {
			host = host.substr(7);
		}

		httplib::SSLClient client(host);
		client.set_connection_timeout(10, 0);
		client.set_read_timeout(10, 0);
		client.enable_server_certificate_verification(false);

		httplib::Headers headers = {
			{ "apikey", config_.supabaseAnonKey },
			{ "Authorization", "Bearer " + authToken_ },
			{ "Content-Type", "application/json" },
			{ "Prefer", "resolution=merge-duplicates" }
		};

		auto res = client.Post(endpoint.c_str(), headers, jsonBody, "application/json");

		if (res) {
			result.statusCode = res->status;
			result.body = res->body;
			result.success = (res->status >= 200 && res->status < 300);
		} else {
			auto err = res.error();
			ofLogError("ofxSurfingSupabase") << "HTTP POST failed - Error: " << httplib::to_string(err);
			result.statusCode = 0;
			result.body = "Connection error: " + std::string(httplib::to_string(err));
		}
	} catch (std::exception & e) {
		ofLogError("ofxSurfingSupabase") << "HTTP POST exception: " << e.what();
	}

	return result;
}

//--------------------------------------------------------------
ofxSurfingSupabase::HttpResponse ofxSurfingSupabase::httpDelete(const std::string & endpoint) {
	HttpResponse result;
	result.success = false;

	if (!bConnected) {
		result.statusCode = 0;
		result.body = "Not connected";
		return result;
	}

	try {
		std::string host = config_.supabaseUrl;
		if (host.find("https://") == 0) {
			host = host.substr(8);
		} else if (host.find("http://") == 0) {
			host = host.substr(7);
		}

		httplib::SSLClient client(host);
		client.set_connection_timeout(10, 0);
		client.set_read_timeout(10, 0);
		client.enable_server_certificate_verification(false);

		httplib::Headers headers = {
			{ "apikey", config_.supabaseAnonKey },
			{ "Authorization", "Bearer " + authToken_ }
		};

		auto res = client.Delete(endpoint.c_str(), headers);

		if (res) {
			result.statusCode = res->status;
			result.body = res->body;
			result.success = (res->status >= 200 && res->status < 300);
		} else {
			auto err = res.error();
			ofLogError("ofxSurfingSupabase") << "HTTP DELETE failed - Error: " << httplib::to_string(err);
			result.statusCode = 0;
			result.body = "Connection error: " + std::string(httplib::to_string(err));
		}
	} catch (std::exception & e) {
		ofLogError("ofxSurfingSupabase") << "HTTP DELETE exception: " << e.what();
	}

	return result;
}

//--------------------------------------------------------------
void ofxSurfingSupabase::update() {
	if (hasPendingPreset_.load()) {
		std::string presetData;
		std::string presetName;
		{
			std::lock_guard<std::mutex> lock(pendingPresetMutex_);
			presetData = pendingPresetJson_;
			presetName = pendingPresetName_;
			pendingPresetJson_.clear();
			pendingPresetName_.clear();
			hasPendingPreset_ = false;
		}

		if (!presetData.empty()) {
			deserializeJsonToScene(presetData);
			ofLogNotice("ofxSurfingSupabase") << "loadPreset(): ✓ Preset loaded and applied (" << presetName << ")";
		}
	}

	if (hasPendingPresetList_.load()) {
		std::vector<std::string> presetList;
		{
			std::lock_guard<std::mutex> lock(pendingPresetListMutex_);
			presetList = pendingPresetList_;
			pendingPresetList_.clear();
			hasPendingPresetList_ = false;
		}

		presetsNamesRemote = presetList;
		ofLogNotice("ofxSurfingSupabase") << "✓ Found " << presetsNamesRemote.size() << " presets";

		int newMin = presetsNamesRemote.empty() ? -1 : 0;
		int newMax = presetsNamesRemote.empty() ? -1 : static_cast<int>(presetsNamesRemote.size()) - 1;
		int value = presetsNamesRemote.empty() ? -1 : ofClamp(selectedPresetIndexRemote.get(), newMin, newMax);

		if (selectedPresetIndexRemote.getMin() != newMin || selectedPresetIndexRemote.getMax() != newMax) {
			selectedPresetIndexRemote.set(selectedPresetIndexRemote.getName(), value, newMin, newMax);
		} else {
			selectedPresetIndexRemote = value;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::keyPressed(int key) {
	if (!bKeys) return;

	if (key == 'g' || key == 'G') {
		bGui = !bGui;
	} else if (key == 'd' || key == 'D') {
		bDebug = !bDebug;
	} else if (key == OF_KEY_RIGHT) {
		selectNextRemote();
	} else if (key == OF_KEY_LEFT) {
		selectPreviousRemote();
	} else if (key == 's' || key == 'S') {
		vSaveToRemote.trigger();
	} else if (key == 'n' || key == 'N') {
		vSaveNewRemote.trigger();
	} else if (key == 'l' || key == 'L') {
		loadAndApplyRemote();
	} else if (key == 'r' || key == 'R') {
		refreshPresetListRemote();
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::draw() {
	if (!bGui && !bDebug) return;

	gui_.draw();

	// Draw status
	int x = gui_.getPosition().x + 5;
	int y = gui_.getPosition().y + gui_.getHeight() + 20;
	int p = 20;

	if (bDebug) {

		// Status
		std::string status = "Status: ";
		ofColor statusColor;
		if (bConnected) {
			status += "CONNECTED";
			statusColor = ofColor::green;
		} else {
			status += "DISCONNECTED";
			statusColor = ofColor::red;
		}
		ofDrawBitmapStringHighlight(status, x, y, ofColor::black, statusColor);

		y = y + p;
		if (isLoadingRemote_.load()) {
			static const std::string spinner = "|/-\\";
			char tick = spinner[ofGetFrameNum() % spinner.size()];
			std::string wait = "Loading preset  " + std::string(1, tick);
			ofDrawBitmapStringHighlight(wait, x, y, ofColor::black, ofColor::yellow);
		} else if (isSavingRemote_.load()) {
			static const std::string spinner = "|/-\\";
			char tick = spinner[ofGetFrameNum() % spinner.size()];
			std::string wait = "Saving preset   " + std::string(1, tick);
			ofDrawBitmapStringHighlight(wait, x, y, ofColor::black, ofColor::yellow);
		} else {
			std::string wait = "                 ";
			ofDrawBitmapStringHighlight(wait, x, y, ofColor::black, ofColor::yellow);
		}

		// Selected
		if (!presetsNamesRemote.empty() && selectedPresetIndexRemote >= 0 && selectedPresetIndexRemote < presetsNamesRemote.size()) {
			y = y + p;
			std::string presetInfo = "Selected: " + presetsNamesRemote[selectedPresetIndexRemote.get()];
			presetInfo += " (" + ofToString(selectedPresetIndexRemote.get()) + "/" + ofToString(presetsNamesRemote.size() - 1) + ")";
			ofSetColor(255);
			ofDrawBitmapStringHighlight(presetInfo, x, y);
		}

		// Print presets names
		y = y + 5;
		for (int i = 0; i < presetsNamesRemote.size(); i++) {
			p = 15;
			y = y + p;
			string s = "";
			if (i < 10) s += " ";
			s += ofToString(i) + " ";
			s += presetsNamesRemote[i];
			if (i == selectedPresetIndexRemote) s += " *";
			ofDrawBitmapStringHighlight(s, x, y);
		}
	}

	//--

	// Keys info
	if (bKeys) {
		p = 20;
		x = ofGetWidth() - 200;
		y = ofGetHeight() - 9 * p;
		ofDrawBitmapStringHighlight("KEYS", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("G: Toggle Gui", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("D: Toggle Debug", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight(">: Next Preset", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("<: Previous Preset", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("L: Load", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("S: Save (Overwrite)", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("N: Save New", x, y);
		y = y + p;
		ofDrawBitmapStringHighlight("R: Refresh", x, y);
	}
}

//--------------------------------------------------------------
std::string ofxSurfingSupabase::generateTimestampName() {
	std::time_t now = std::time(nullptr);
	std::tm * tm = std::localtime(&now);

	char buffer[64];
	std::strftime(buffer, sizeof(buffer), "scene_%Y%m%d_%H%M%S", tm);

	return std::string(buffer);
}

//--------------------------------------------------------------
std::string ofxSurfingSupabase::serializeSceneToJson() {
	if (!sceneParams_) {
		ofLogError("ofxSurfingSupabase") << "Scene params not set";
		return "{}";
	}

	ofJson json;
	ofSerialize(json, *sceneParams_);

	return json.dump();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::deserializeJsonToScene(const std::string & jsonStr) {
	if (!sceneParams_) {
		ofLogError("ofxSurfingSupabase") << "Scene params not set";
		return;
	}

	try {
		ofJson json = ofJson::parse(jsonStr);
		ofDeserialize(json, *sceneParams_);
		ofLogNotice("ofxSurfingSupabase") << "Deserialized scene parameters";
	} catch (std::exception & e) {
		ofLogError("ofxSurfingSupabase") << "Failed to deserialize: " << e.what();
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::loadAndApplyRemote() {
	ofLogNotice("ofxSurfingSupabase") << "loadAndApplyRemote()";

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "Not connected";
		return;
	}

	if (selectedPresetIndexRemote < 0 || selectedPresetIndexRemote >= presetsNamesRemote.size()) {
		ofLogWarning("ofxSurfingSupabase") << "No preset selected";
		return;
	}

	loadPreset(presetsNamesRemote[selectedPresetIndexRemote.get()]);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::savePreset(const std::string & presetName) {
	ofLogNotice("ofxSurfingSupabase") << "savePreset(): " << presetName;

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "Not connected";
		return;
	}

	if (isSavingRemote_.exchange(true)) {
		ofLogWarning("ofxSurfingSupabase") << "savePreset(): Save already in progress";
		return;
	}

	std::string presetNameCopy = presetName;
	std::string jsonData = serializeSceneToJson();

	std::thread([this, presetNameCopy, jsonData]() {
		ofJson presetJson;

		try {
			presetJson = ofJson::parse(jsonData);
		} catch (std::exception & e) {
			ofLogError("ofxSurfingSupabase") << "savePreset(): Invalid JSON: " << e.what();
			isSavingRemote_ = false;
			return;
		}

		ofJson insertData;
		insertData["user_id"] = userId_;
		insertData["preset_name"] = presetNameCopy;
		insertData["preset_data"] = presetJson;

		std::string endpoint = "/rest/v1/" + TABLE_NAME + "?on_conflict=user_id,preset_name";
		std::string body = insertData.dump();

		if (bDebug) {
			ofLogNotice("ofxSurfingSupabase") << "savePreset(): Saving to: " << endpoint;
			ofLogNotice("ofxSurfingSupabase") << "savePreset(): Preset name: " << presetNameCopy;
		}

		HttpResponse res = httpPost(endpoint, body);

		if (res.success) {
			ofLogNotice("ofxSurfingSupabase") << "savePreset(): ✓ Preset saved successfully";

			std::string listEndpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + "&select=preset_name&order=created_at.asc";
			HttpResponse listRes = httpGet(listEndpoint);
			if (listRes.success) {
				try {
					ofJson responseJson = ofJson::parse(listRes.body);
					std::vector<std::string> list;
					if (responseJson.is_array()) {
						for (auto & item : responseJson) {
							if (item.contains("preset_name")) {
								list.push_back(item["preset_name"].get<std::string>());
							}
						}
					}
					{
						std::lock_guard<std::mutex> lock(pendingPresetListMutex_);
						pendingPresetList_ = std::move(list);
						hasPendingPresetList_ = true;
					}
				} catch (std::exception & e) {
					ofLogError("ofxSurfingSupabase") << "savePreset(): Failed to parse preset list: " << e.what();
				}
			} else {
				ofLogError("ofxSurfingSupabase") << "savePreset(): Failed to refresh preset list: HTTP " << listRes.statusCode;
				if (bDebug) {
					ofLogError("ofxSurfingSupabase") << listRes.body;
				}
			}
		} else {
			ofLogError("ofxSurfingSupabase") << "savePreset(): ✗ Failed to save preset: HTTP " << res.statusCode;
			if (bDebug) {
				ofLogError("ofxSurfingSupabase") << res.body;
			}
		}

		isSavingRemote_ = false;
	}).detach();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::savePresetNew(const std::string & presetName) {
	ofLogNotice("ofxSurfingSupabase") << "savePresetNew(): " << presetName;

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "Not connected";
		return;
	}

	if (isSavingRemote_.exchange(true)) {
		ofLogWarning("ofxSurfingSupabase") << "savePresetNew(): Save already in progress";
		return;
	}

	std::string baseName = presetName.empty() ? generateTimestampName() : presetName;
	std::string jsonData = serializeSceneToJson();

	std::thread([this, baseName, jsonData]() {
		ofJson presetJson;

		try {
			presetJson = ofJson::parse(jsonData);
		} catch (std::exception & e) {
			ofLogError("ofxSurfingSupabase") << "savePresetNew(): Invalid JSON: " << e.what();
			isSavingRemote_ = false;
			return;
		}

		std::string endpoint = "/rest/v1/" + TABLE_NAME;

		for (int attempt = 0; attempt < 100; ++attempt) {
			std::string name = (attempt == 0) ? baseName : baseName + "_" + ofToString(attempt);

			ofJson insertData;
			insertData["user_id"] = userId_;
			insertData["preset_name"] = name;
			insertData["preset_data"] = presetJson;

			HttpResponse res = httpPost(endpoint, insertData.dump());

			if (res.success) {
				ofLogNotice("ofxSurfingSupabase") << "savePresetNew(): ✓ Preset saved as: " << name;

				std::string listEndpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + "&select=preset_name&order=created_at.asc";
				HttpResponse listRes = httpGet(listEndpoint);
				if (listRes.success) {
					try {
						ofJson responseJson = ofJson::parse(listRes.body);
						std::vector<std::string> list;
						if (responseJson.is_array()) {
							for (auto & item : responseJson) {
								if (item.contains("preset_name")) {
									list.push_back(item["preset_name"].get<std::string>());
								}
							}
						}
						{
							std::lock_guard<std::mutex> lock(pendingPresetListMutex_);
							pendingPresetList_ = std::move(list);
							hasPendingPresetList_ = true;
						}
					} catch (std::exception & e) {
						ofLogError("ofxSurfingSupabase") << "savePresetNew(): Failed to parse preset list: " << e.what();
					}
				} else {
					ofLogError("ofxSurfingSupabase") << "savePresetNew(): Failed to refresh preset list: HTTP " << listRes.statusCode;
					if (bDebug) {
						ofLogError("ofxSurfingSupabase") << listRes.body;
					}
				}

				isSavingRemote_ = false;
				return;
			}

			if (res.statusCode != 409) {
				ofLogError("ofxSurfingSupabase") << "savePresetNew(): ✗ Failed to save preset: HTTP " << res.statusCode;
				if (bDebug) {
					ofLogError("ofxSurfingSupabase") << res.body;
				}
				isSavingRemote_ = false;
				return;
			}
		}

		ofLogError("ofxSurfingSupabase") << "savePresetNew(): ✗ Failed to find unique name";
		isSavingRemote_ = false;
	}).detach();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::loadPreset(const std::string & presetName) {
	ofLogNotice("ofxSurfingSupabase") << "loadPreset(): " << presetName;

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "loadPreset(): Not connected";
		return;
	}

	if (isLoadingRemote_.exchange(true)) {
		ofLogWarning("ofxSurfingSupabase") << "loadPreset(): Load already in progress";
		return;
	}

	std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + "&preset_name=eq." + presetName + "&select=preset_data";
	std::string presetNameCopy = presetName;

	std::thread([this, endpoint, presetNameCopy]() {
		HttpResponse res = httpGet(endpoint);

		if (res.success) {
			try {
				ofJson responseJson = ofJson::parse(res.body);

				if (responseJson.is_array() && !responseJson.empty() && responseJson[0].contains("preset_data")) {
					std::string presetData = responseJson[0]["preset_data"].dump();
					{
						std::lock_guard<std::mutex> lock(pendingPresetMutex_);
						pendingPresetJson_ = presetData;
						pendingPresetName_ = presetNameCopy;
						hasPendingPreset_ = true;
					}
				} else {
					ofLogWarning("ofxSurfingSupabase") << "loadPreset(): Preset not found";
				}
			} catch (std::exception & e) {
				ofLogError("ofxSurfingSupabase") << "loadPreset(): Failed to parse response: " << e.what();
			}
		} else {
			ofLogError("ofxSurfingSupabase") << "loadPreset(): ✗ Failed to load preset: HTTP " << res.statusCode;
			if (bDebug) {
				ofLogError("ofxSurfingSupabase") << res.body;
			}
		}

		isLoadingRemote_ = false;
	}).detach();
}

//--------------------------------------------------------------
void ofxSurfingSupabase::deletePresetRemote(const std::string & presetName) {
	ofLogNotice("ofxSurfingSupabase") << "deletePresetRemote(): " << presetName;

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "deletePresetRemote(): Not connected";
		return;
	}

	std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + "&preset_name=eq." + presetName;

	HttpResponse res = httpDelete(endpoint);

	if (res.success) {
		ofLogNotice("ofxSurfingSupabase") << "deletePresetRemote(): ✓ Preset deleted successfully";
		refreshPresetListRemote();

		// Adjust selected index if needed
		if (selectedPresetIndexRemote >= presetsNamesRemote.size() && !presetsNamesRemote.empty()) {
			selectedPresetIndexRemote = presetsNamesRemote.size() - 1;
		}
		//clamp selected index
		selectedPresetIndexRemote.setMin(0);
		selectedPresetIndexRemote.setMax(presetsNamesRemote.size() - 1);
	} else {
		ofLogError("ofxSurfingSupabase") << "deletePresetRemote(): ✗ Failed to delete preset: HTTP " << res.statusCode;
		if (bDebug) {
			ofLogError("ofxSurfingSupabase") << res.body;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::refreshPresetListRemote() {
	ofLogNotice("ofxSurfingSupabase") << "refreshPresetListRemote()";

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "refreshPresetListRemote(): Not connected";
		return;
	}

	//// Sort descendent
	//std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + "&select=preset_name&order=created_at.desc";
	// Sort ascendent
	std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + "&select=preset_name&order=created_at.asc";

	HttpResponse res = httpGet(endpoint);

	if (res.success) {
		try {
			ofJson responseJson = ofJson::parse(res.body);

			presetsNamesRemote.clear();

			if (responseJson.is_array()) {
				for (auto & item : responseJson) {
					if (item.contains("preset_name")) {
						presetsNamesRemote.push_back(item["preset_name"].get<std::string>());
					}
				}
			}

			ofLogNotice("ofxSurfingSupabase") << "✓ Found " << presetsNamesRemote.size() << " presets";

			// Clamp selected index
			selectedPresetIndexRemote.setMin(0);
			selectedPresetIndexRemote.setMax(presetsNamesRemote.size() - 1);

			if (!presetsNamesRemote.empty() && selectedPresetIndexRemote < 0) {
				selectedPresetIndexRemote = 0;
			}

			// Clamp index
			if (selectedPresetIndexRemote >= presetsNamesRemote.size()) {
				selectedPresetIndexRemote = presetsNamesRemote.empty() ? -1 : presetsNamesRemote.size() - 1;
			}

		} catch (std::exception & e) {
			ofLogError("ofxSurfingSupabase") << "refreshPresetListRemote(): Failed to parse preset list: " << e.what();
		}
	} else {
		ofLogError("ofxSurfingSupabase") << "refreshPresetListRemote(): ✗ Failed to refresh preset list: HTTP " << res.statusCode;
		if (bDebug) {
			ofLogError("ofxSurfingSupabase") << res.body;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::clearDatabase() {
	ofLogNotice("ofxSurfingSupabase") << "clearDatabase()";

	if (!bConnected) {
		ofLogWarning("ofxSurfingSupabase") << "clearDatabase(): Not connected";
		return;
	}

	ofLogWarning("ofxSurfingSupabase") << "⚠️  Deleting ALL presets for user: " << userId_;

	std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_;

	HttpResponse res = httpDelete(endpoint);

	if (res.success) {
		ofLogNotice("ofxSurfingSupabase") << "clearDatabase(): ✓ Database cleared successfully";
		presetsNamesRemote.clear();
		selectedPresetIndexRemote = -1;
		selectedPresetIndexRemote.setMin(-1);
		selectedPresetIndexRemote.setMax(-1);
	} else {
		ofLogError("ofxSurfingSupabase") << "clearDatabase(): ✗ Failed to clear database: HTTP " << res.statusCode;
		if (bDebug) {
			ofLogError("ofxSurfingSupabase") << res.body;
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectedIndexRemoteUpdate() {
	if (presetsNamesRemote.empty()) return;

	// clamp index
	if (selectedPresetIndexRemote < 0) { //cycled
		selectedPresetIndexRemote = presetsNamesRemote.size() - 1;
	}
	if (selectedPresetIndexRemote > presetsNamesRemote.size() - 1) { //cycled
		selectedPresetIndexRemote = selectedPresetIndexRemote.get() % presetsNamesRemote.size(); //cycled
	}

	// auto load current preset index
	if (bAutoLoad) {
		loadAndApplyRemote();
	}
	ofLogNotice("ofxSurfingSupabase") << "selectedIndexRemoteUpdate(): Preset index: " << selectedPresetIndexRemote.get() << " name:  " << presetsNamesRemote[selectedPresetIndexRemote.get()];
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectNextRemote() {
	if (presetsNamesRemote.empty()) return;

	selectedPresetIndexRemote = (selectedPresetIndexRemote.get() + 1) % presetsNamesRemote.size(); //cycled

	ofLogNotice("ofxSurfingSupabase") << "selectNextRemote(): Selected: " << presetsNamesRemote[selectedPresetIndexRemote.get()];
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectPreviousRemote() {
	if (presetsNamesRemote.empty()) return;

	// selectedPresetIndexRemote--;
	selectedPresetIndexRemote = selectedPresetIndexRemote.get() - 1;
	if (selectedPresetIndexRemote < 0) { //cycled
		selectedPresetIndexRemote = presetsNamesRemote.size() - 1;
	}
	ofLogNotice("ofxSurfingSupabase") << "selectPreviousRemote(): Selected: " << presetsNamesRemote[selectedPresetIndexRemote.get()];
}

//--------------------------------------------------------------
std::string ofxSurfingSupabase::getConnectionStatus() const {
	if (bConnected) return "CONNECTED";
	return "DISCONNECTED";
}

//--------------------------------------------------------------
bool ofxSurfingSupabase::SupabaseConfig::isValid() const {
	if (authMode == "ANON_KEY") {
		return !supabaseUrl.empty() && !supabaseAnonKey.empty();
	}

	if (authMode == "EMAIL_PASSWORD") {
		return !supabaseUrl.empty() && !supabaseAnonKey.empty() && !email.empty() && !password.empty();
	}

	return false;
}
