#include "ofxSurfingSupabase.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../libs/cpp-httplib/include/httplib.h"

// Constants
const std::string ofxSurfingSupabase::CREDENTIALS_PATH = "credentials.txt";
const std::string ofxSurfingSupabase::TABLE_NAME = "presets";//TODO: to be used as kit name, alowing multiple kits
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
  selectedPresetIndex_ = -1;
  
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
  params_.add(bRemoteMode);
  params_.add(bAutoSync);
  params_.add(bShowDebug);
  params_.add(bShowPresetManager);
  params_.add(vSaveSceneDirect);
  params_.add(vLoadAndApply);
  params_.add(vSendToRemote);
  params_.add(vLoadFromRemote);
  
  paramsManager_.setName("Preset Manager");
  paramsManager_.add(selectedPresetIndex_);
  paramsManager_.add(vSelectPrevious);
  paramsManager_.add(vSelectNext);
  paramsManager_.add(vRefreshList);
  paramsManager_.add(vDeleteSelected);
  paramsManager_.add(vClearDatabase);
  
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
        refreshPresetList();
      }
    }
  });
  
  e_vSaveSceneDirect = vSaveSceneDirect.newListener([this]() {
    sendSceneDirect();
  });
  
  e_vLoadAndApply = vLoadAndApply.newListener([this]() {
    loadAndApplyRemote();
  });
  
  e_vSendToRemote = vSendToRemote.newListener([this]() {
    if (selectedPresetIndex_ >= 0 && selectedPresetIndex_ < presetNames_.size()) {
      savePreset(presetNames_[selectedPresetIndex_.get()]);
    }
  });
  
  e_vLoadFromRemote = vLoadFromRemote.newListener([this]() {
    if (selectedPresetIndex_ >= 0 && selectedPresetIndex_ < presetNames_.size()) {
      loadPreset(presetNames_[selectedPresetIndex_.get()]);
    }
  });
  
  e_vRefreshList = vRefreshList.newListener([this]() {
    refreshPresetList();
  });
  
  e_vDeleteSelected = vDeleteSelected.newListener([this]() {
    if (selectedPresetIndex_ >= 0 && selectedPresetIndex_ < presetNames_.size()) {
      deletePreset(presetNames_[selectedPresetIndex_.get()]);
    }
  });
  
  e_vClearDatabase = vClearDatabase.newListener([this]() {
    clearDatabase();
  });
  
  e_vSelectNext = vSelectNext.newListener([this]() {
    selectNext();
  });
  
  e_vSelectPrevious = vSelectPrevious.newListener([this]() {
    selectPrevious();
  });

  e_selectedPresetIndex_ = selectedPresetIndex_.newListener([this](int& i){
    selectedUpdate();
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
      refreshPresetList();
    }
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::exit() {
  ofLogNotice("ofxSurfingSupabase") << "exit()";
}

//--------------------------------------------------------------
void ofxSurfingSupabase::setupPresetParameters(ofParameterGroup& sceneParams) {
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
  for (auto& line : buffer.getLines()) {
    if (line.empty() || line[0] == '#') continue;
    
    std::size_t pos = line.find('=');
    if (pos == std::string::npos) continue;
    
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    
    // Trim whitespace
    value.erase(0, value.find_first_not_of(" \t\r\n"));
    value.erase(value.find_last_not_of(" \t\r\n") + 1);
    
    if (key == "AUTH_MODE") config_.authMode = value;
    else if (key == "SUPABASE_URL") {
      config_.supabaseUrl = value;
      // Remove trailing slashes
      while (!config_.supabaseUrl.empty() && config_.supabaseUrl.back() == '/') {
        config_.supabaseUrl.pop_back();
      }
    }
    else if (key == "SUPABASE_ANON_KEY") config_.supabaseAnonKey = value;
    else if (key == "EMAIL") config_.email = value;
    else if (key == "PASSWORD") config_.password = value;
  }
  
  if (bShowDebug) {
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
  }
  else if (config_.authMode == "EMAIL_PASSWORD") {
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
        {"Content-Type", "application/json"},
        {"apikey", config_.supabaseAnonKey}
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
          if (bShowDebug) {
            ofLogNotice("ofxSurfingSupabase") << "User ID: " << userId_;
          }
          
          return true;
        } catch (std::exception& e) {
          ofLogError("ofxSurfingSupabase") << "Failed to parse auth response: " << e.what();
        }
      } else {
        std::string errorMsg = res ? res->body : "Connection failed";
        ofLogError("ofxSurfingSupabase") << "✗ Authentication failed: HTTP " << (res ? res->status : 0);
        if (!res) {
          ofLogError("ofxSurfingSupabase") << "Error: " << httplib::to_string(res.error());
        }
        if (bShowDebug) {
          ofLogError("ofxSurfingSupabase") << errorMsg;
        }
      }
    } catch (std::exception& e) {
      ofLogError("ofxSurfingSupabase") << "Exception during authentication: " << e.what();
    }
  }
  else {
    ofLogError("ofxSurfingSupabase") << "Unknown AUTH_MODE: " << config_.authMode;
    ofLogError("ofxSurfingSupabase") << "Supported modes: ANON_KEY, EMAIL_PASSWORD";
  }
  
  bConnected = false;
  return false;
}

//--------------------------------------------------------------
ofxSurfingSupabase::HttpResponse ofxSurfingSupabase::httpGet(const std::string& endpoint) {
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
    
    if (bShowDebug) {
      ofLogNotice("ofxSurfingSupabase") << "HTTP GET: " << endpoint;
      ofLogNotice("ofxSurfingSupabase") << "Host: " << host;
    }
    
    httplib::SSLClient client(host);
    client.set_connection_timeout(10, 0);
    client.set_read_timeout(10, 0);
    client.enable_server_certificate_verification(false); // Disable cert verification for testing
    
    httplib::Headers headers = {
      {"apikey", config_.supabaseAnonKey},
      {"Authorization", "Bearer " + authToken_}
    };
    
    auto res = client.Get(endpoint.c_str(), headers);
    
    if (res) {
      result.statusCode = res->status;
      result.body = res->body;
      result.success = (res->status >= 200 && res->status < 300);
      
      if (bShowDebug) {
        ofLogNotice("ofxSurfingSupabase") << "Response: HTTP " << result.statusCode;
      }
    } else {
      auto err = res.error();
      ofLogError("ofxSurfingSupabase") << "HTTP GET failed - Error: " << httplib::to_string(err);
      result.statusCode = 0;
      result.body = "Connection error: " + std::string(httplib::to_string(err));
    }
  } catch (std::exception& e) {
    ofLogError("ofxSurfingSupabase") << "HTTP GET exception: " << e.what();
  }
  
  return result;
}

//--------------------------------------------------------------
ofxSurfingSupabase::HttpResponse ofxSurfingSupabase::httpPost(const std::string& endpoint, const std::string& jsonBody) {
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
      {"apikey", config_.supabaseAnonKey},
      {"Authorization", "Bearer " + authToken_},
      {"Content-Type", "application/json"},
      {"Prefer", "resolution=merge-duplicates"}
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
  } catch (std::exception& e) {
    ofLogError("ofxSurfingSupabase") << "HTTP POST exception: " << e.what();
  }
  
  return result;
}

//--------------------------------------------------------------
ofxSurfingSupabase::HttpResponse ofxSurfingSupabase::httpDelete(const std::string& endpoint) {
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
      {"apikey", config_.supabaseAnonKey},
      {"Authorization", "Bearer " + authToken_}
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
  } catch (std::exception& e) {
    ofLogError("ofxSurfingSupabase") << "HTTP DELETE exception: " << e.what();
  }
  
  return result;
}

//--------------------------------------------------------------
void ofxSurfingSupabase::update() {
  // Reserved for future async operations
}

//--------------------------------------------------------------
void ofxSurfingSupabase::draw() {
  if (!bShowDebug) return;
  
  gui_.draw();
  
  // Draw status
  int x = gui_.getPosition().x+5;
  int y = gui_.getPosition().y + gui_.getHeight() + 20;
  
  std::string status = "Status: ";
  ofColor statusColor;
  
  if (bConnected) {
    status += "🟢 CONNECTED";
    statusColor = ofColor::green;
  } else {
    status += "🔴 DISCONNECTED";
    statusColor = ofColor::red;
  }
  
  ofPushStyle();
  ofSetColor(statusColor);
  ofDrawBitmapStringHighlight(status, x, y);
  
  if (!presetNames_.empty() && selectedPresetIndex_ >= 0 && selectedPresetIndex_ < presetNames_.size()) {
    std::string presetInfo = "Selected: " + presetNames_[selectedPresetIndex_.get()];
    presetInfo += " (" + ofToString(selectedPresetIndex_.get() + 1) + "/" + ofToString(presetNames_.size()) + ")";
    ofSetColor(255);
	ofDrawBitmapStringHighlight(presetInfo, x, y + 20);
  }
  
  ofPopStyle();
}

//--------------------------------------------------------------
std::string ofxSurfingSupabase::generateTimestampName() {
  std::time_t now = std::time(nullptr);
  std::tm* tm = std::localtime(&now);
  
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
void ofxSurfingSupabase::deserializeJsonToScene(const std::string& jsonStr) {
  if (!sceneParams_) {
    ofLogError("ofxSurfingSupabase") << "Scene params not set";
    return;
  }
  
  try {
    ofJson json = ofJson::parse(jsonStr);
    ofDeserialize(json, *sceneParams_);
    ofLogNotice("ofxSurfingSupabase") << "Deserialized scene parameters";
  } catch (std::exception& e) {
    ofLogError("ofxSurfingSupabase") << "Failed to deserialize: " << e.what();
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::sendSceneDirect() {
  ofLogNotice("ofxSurfingSupabase") << "sendSceneDirect()";
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  std::string presetName = generateTimestampName();
  std::string jsonData = serializeSceneToJson();
  
  savePreset(presetName);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::loadAndApplyRemote() {
  ofLogNotice("ofxSurfingSupabase") << "loadAndApplyRemote()";
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  if (selectedPresetIndex_ < 0 || selectedPresetIndex_ >= presetNames_.size()) {
    ofLogWarning("ofxSurfingSupabase") << "No preset selected";
    return;
  }
  
  loadPreset(presetNames_[selectedPresetIndex_.get()]);
}

//--------------------------------------------------------------
void ofxSurfingSupabase::savePreset(const std::string& presetName) {
  ofLogNotice("ofxSurfingSupabase") << "savePreset(): " << presetName;
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  std::string jsonData = serializeSceneToJson();
  
  // Build insert request
  ofJson insertData;
  insertData["user_id"] = userId_;
  insertData["preset_name"] = presetName;
  insertData["preset_data"] = ofJson::parse(jsonData);
  
  std::string endpoint = "/rest/v1/" + TABLE_NAME;
  std::string body = insertData.dump();
  
  if (bShowDebug) {
    ofLogNotice("ofxSurfingSupabase") << "Saving to: " << endpoint;
    ofLogNotice("ofxSurfingSupabase") << "Preset name: " << presetName;
  }
  
  HttpResponse res = httpPost(endpoint, body);
  
  if (res.success) {
    ofLogNotice("ofxSurfingSupabase") << "✓ Preset saved successfully";
    refreshPresetList();
  } else {
    ofLogError("ofxSurfingSupabase") << "✗ Failed to save preset: HTTP " << res.statusCode;
    if (bShowDebug) {
      ofLogError("ofxSurfingSupabase") << res.body;
    }
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::loadPreset(const std::string& presetName) {
  ofLogNotice("ofxSurfingSupabase") << "loadPreset(): " << presetName;
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + 
                         "&preset_name=eq." + presetName + "&select=preset_data";
  
  HttpResponse res = httpGet(endpoint);
  
  if (res.success) {
    try {
      ofJson responseJson = ofJson::parse(res.body);
      
      if (responseJson.is_array() && !responseJson.empty()) {
        std::string presetData = responseJson[0]["preset_data"].dump();
        deserializeJsonToScene(presetData);
        ofLogNotice("ofxSurfingSupabase") << "✓ Preset loaded and applied";
      } else {
        ofLogWarning("ofxSurfingSupabase") << "Preset not found";
      }
    } catch (std::exception& e) {
      ofLogError("ofxSurfingSupabase") << "Failed to parse response: " << e.what();
    }
  } else {
    ofLogError("ofxSurfingSupabase") << "✗ Failed to load preset: HTTP " << res.statusCode;
    if (bShowDebug) {
      ofLogError("ofxSurfingSupabase") << res.body;
    }
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::deletePreset(const std::string& presetName) {
  ofLogNotice("ofxSurfingSupabase") << "deletePreset(): " << presetName;
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + 
                         "&preset_name=eq." + presetName;
  
  HttpResponse res = httpDelete(endpoint);
  
  if (res.success) {
    ofLogNotice("ofxSurfingSupabase") << "✓ Preset deleted successfully";
    refreshPresetList();
    
    // Adjust selected index if needed
    if (selectedPresetIndex_ >= presetNames_.size() && !presetNames_.empty()) {
      selectedPresetIndex_ = presetNames_.size() - 1;
    }
    //clamp selected index
    selectedPresetIndex_.setMin(0);
    selectedPresetIndex_.setMax(presetNames_.size() - 1);
  } else {
    ofLogError("ofxSurfingSupabase") << "✗ Failed to delete preset: HTTP " << res.statusCode;
    if (bShowDebug) {
      ofLogError("ofxSurfingSupabase") << res.body;
    }
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::refreshPresetList() {
  ofLogNotice("ofxSurfingSupabase") << "refreshPresetList()";
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_ + 
                         "&select=preset_name&order=created_at.desc";
  
  HttpResponse res = httpGet(endpoint);
  
  if (res.success) {
    try {
      ofJson responseJson = ofJson::parse(res.body);
      
      presetNames_.clear();
      
      if (responseJson.is_array()) {
        for (auto& item : responseJson) {
          if (item.contains("preset_name")) {
            presetNames_.push_back(item["preset_name"].get<std::string>());
          }
        }
      }
      
      ofLogNotice("ofxSurfingSupabase") << "✓ Found " << presetNames_.size() << " presets";
      
      //clamp selected index
      selectedPresetIndex_.setMin(0);
      selectedPresetIndex_.setMax(presetNames_.size() - 1);

      if (!presetNames_.empty() && selectedPresetIndex_ < 0) {
        selectedPresetIndex_ = 0;
      }
      
      //clamp index
      if (selectedPresetIndex_ >= presetNames_.size()) {
        selectedPresetIndex_ = presetNames_.empty() ? -1 : presetNames_.size() - 1;
      }
      
    } catch (std::exception& e) {
      ofLogError("ofxSurfingSupabase") << "Failed to parse preset list: " << e.what();
    }
  } else {
    ofLogError("ofxSurfingSupabase") << "✗ Failed to refresh preset list: HTTP " << res.statusCode;
    if (bShowDebug) {
      ofLogError("ofxSurfingSupabase") << res.body;
    }
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::clearDatabase() {
  ofLogNotice("ofxSurfingSupabase") << "clearDatabase()";
  
  if (!bConnected) {
    ofLogWarning("ofxSurfingSupabase") << "Not connected";
    return;
  }
  
  ofLogWarning("ofxSurfingSupabase") << "⚠️  Deleting ALL presets for user: " << userId_;
  
  std::string endpoint = "/rest/v1/" + TABLE_NAME + "?user_id=eq." + userId_;
  
  HttpResponse res = httpDelete(endpoint);
  
  if (res.success) {
    ofLogNotice("ofxSurfingSupabase") << "✓ Database cleared successfully";
    presetNames_.clear();
    selectedPresetIndex_ = -1;
    selectedPresetIndex_.setMin(-1);
    selectedPresetIndex_.setMax(-1);
  } else {
    ofLogError("ofxSurfingSupabase") << "✗ Failed to clear database: HTTP " << res.statusCode;
    if (bShowDebug) {
      ofLogError("ofxSurfingSupabase") << res.body;
    }
  }
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectedUpdate() {
  if (presetNames_.empty()) return;
  
  // clamp index
  if (selectedPresetIndex_ < 0) {//cycled
    selectedPresetIndex_ = presetNames_.size() - 1;
  }
  if (selectedPresetIndex_ > presetNames_.size()-1) {//cycled
    selectedPresetIndex_ = selectedPresetIndex_.get() % presetNames_.size();//cycled
  }

  ofLogNotice("ofxSurfingSupabase") << "selectedUpdate() Preset index: " << selectedPresetIndex_.get()<< " name:  " << presetNames_[selectedPresetIndex_.get()];
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectNext() {
  if (presetNames_.empty()) return;
  
  selectedPresetIndex_ = (selectedPresetIndex_.get() + 1) % presetNames_.size();//cycled

  ofLogNotice("ofxSurfingSupabase") << "Selected: " << presetNames_[selectedPresetIndex_.get()];
}

//--------------------------------------------------------------
void ofxSurfingSupabase::selectPrevious() {
  if (presetNames_.empty()) return;
  
  // selectedPresetIndex_--;
  selectedPresetIndex_ = selectedPresetIndex_.get() - 1;
  if (selectedPresetIndex_ < 0) {//cycled
    selectedPresetIndex_ = presetNames_.size() - 1;
  }
  ofLogNotice("ofxSurfingSupabase") << "Selected: " << presetNames_[selectedPresetIndex_.get()];
}

//--------------------------------------------------------------
std::string ofxSurfingSupabase::getConnectionStatus() const {
  if (bConnected) return "🟢 CONNECTED";
  return "🔴 DISCONNECTED";
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
