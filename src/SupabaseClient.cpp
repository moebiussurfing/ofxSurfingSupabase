// file: src/SupabaseClient.cpp

#include "SupabaseClient.h"
#include <exception>

//--------------------------------------------------------------
void SupabaseClient::setup(SupabaseConfig& cfg) {
config = cfg;

if (!config.isValid()) {
ofLogError("SupabaseClient") << "Invalid configuration";
return;
}

// If using email/password auth, authenticate now (SYNC)
if (config.needsAuthentication()) {
ofLogNotice("SupabaseClient") << "Email/password mode - authenticating...";

string url = config.getAuthUrl() + "/token?grant_type=password";

ofJson payload = {
{"email", config.email},
{"password", config.password}
};

ofHttpRequest request(url, url);
request.method = ofHttpRequest::POST;
request.body = payload.dump();
request.headers["Content-Type"] = "application/json";
request.headers["apikey"] = config.apiKey;

ofURLFileLoader loader;
ofHttpResponse response = loader.handleRequest(request);

if (response.status == 200) {
try {
ofJson json = ofJson::parse(response.data.getText());
config.accessToken = json["access_token"].get<string>();
config.userId = json["user"]["id"].get<string>();

ofLogNotice("SupabaseClient") << "Authenticated successfully, user: " << config.userId;
bAuthenticated = true;

string userId = config.userId;
ofNotifyEvent(onAuthSuccess, userId);
} catch (std::exception& e) {
ofLogError("SupabaseClient") << "Auth parse error: " << e.what();
string error = "Parse error: " + string(e.what());
ofNotifyEvent(onAuthError, error);
}
} else {
ofLogError("SupabaseClient") << "Auth failed: " << response.status;
string error = "HTTP " + ofToString(response.status);
ofNotifyEvent(onAuthError, error);
}
} else {
ofLogNotice("SupabaseClient") << "Configured successfully (API key mode)";
bAuthenticated = true;
string userId = config.userId;
ofNotifyEvent(onAuthSuccess, userId);
}
}

//--------------------------------------------------------------
void SupabaseClient::authenticateAsync(std::function<void(string)> onSuccess,
                                        std::function<void(string)> onError) {
// Sync version - name kept for API compatibility
if (config.authMode != AuthMode::EMAIL_PASSWORD) {
onError("Not in email/password auth mode");
return;
}

string url = config.getAuthUrl() + "/token?grant_type=password";

ofJson payload = {
{"email", config.email},
{"password", config.password}
};

ofHttpRequest request(url, url);
request.method = ofHttpRequest::POST;
request.body = payload.dump();
request.headers["Content-Type"] = "application/json";
request.headers["apikey"] = config.apiKey;

ofURLFileLoader loader;
ofHttpResponse response = loader.handleRequest(request);

if (response.status == 200) {
try {
ofJson json = ofJson::parse(response.data.getText());
config.accessToken = json["access_token"].get<string>();
config.userId = json["user"]["id"].get<string>();
onSuccess(config.userId);
} catch (std::exception& e) {
onError("Parse error: " + string(e.what()));
}
} else {
onError("HTTP " + ofToString(response.status));
}
}

//--------------------------------------------------------------
bool SupabaseClient::isAuthenticated() const {
return bAuthenticated;
}

//--------------------------------------------------------------
string SupabaseClient::getUserId() const {
return config.userId;
}

//--------------------------------------------------------------
map<string, string> SupabaseClient::getHeaders(bool usePatch) const {
map<string, string> headers;

if (config.authMode == AuthMode::EMAIL_PASSWORD && !config.accessToken.empty()) {
headers["Authorization"] = "Bearer " + config.accessToken;
headers["apikey"] = config.apiKey;
} else {
headers["apikey"] = config.apiKey;
headers["Authorization"] = "Bearer " + config.apiKey;
}

headers["Content-Type"] = "application/json";
headers["Prefer"] = "return=representation";

if (usePatch) {
headers["X-HTTP-Method-Override"] = "PATCH";
}

return headers;
}

//--------------------------------------------------------------
void SupabaseClient::selectAsync(const string& table, const string& query,
                                  std::function<void(ofJson)> onSuccess,
                                  std::function<void(string)> onError) {
if (!config.isValid()) {
onError("Client not configured");
return;
}

string url = config.getRestUrl() + "/" + table;
if (!query.empty()) url += "?" + query;

ofHttpRequest request(url, url);
request.method = ofHttpRequest::GET;

for (auto& pair : getHeaders()) {
request.headers[pair.first] = pair.second;
}

ofLogVerbose("SupabaseClient") << "GET " << url;

ofURLFileLoader loader;
ofHttpResponse response = loader.handleRequest(request);

if (response.status == 200) {
try {
ofJson json = ofJson::parse(response.data.getText());
onSuccess(json);
} catch (std::exception& e) {
onError("JSON parse error: " + string(e.what()));
}
} else {
onError("HTTP " + ofToString(response.status));
}
}

//--------------------------------------------------------------
void SupabaseClient::insertAsync(const string& table, const ofJson& data,
                                  std::function<void(ofJson)> onSuccess,
                                  std::function<void(string)> onError) {
if (!config.isValid()) {
onError("Client not configured");
return;
}

string url = config.getRestUrl() + "/" + table;
string body = data.dump();

ofHttpRequest request(url, url);
request.method = ofHttpRequest::POST;
request.body = body;

for (auto& pair : getHeaders()) {
request.headers[pair.first] = pair.second;
}

ofLogVerbose("SupabaseClient") << "POST " << url;

ofURLFileLoader loader;
ofHttpResponse response = loader.handleRequest(request);

if (response.status >= 200 && response.status < 300) {
try {
ofJson json = ofJson::parse(response.data.getText());
onSuccess(json);
} catch (std::exception& e) {
onSuccess(ofJson::object());
}
} else {
ofLogError("SupabaseClient") << "INSERT failed: " << response.status;
onError("HTTP " + ofToString(response.status) + ": " + response.data.getText());
}
}

//--------------------------------------------------------------
void SupabaseClient::updateAsync(const string& table, const string& filter,
                                  const ofJson& data,
                                  std::function<void()> onSuccess,
                                  std::function<void(string)> onError) {
if (!config.isValid()) {
onError("Client not configured");
return;
}

string url = config.getRestUrl() + "/" + table + "?" + filter;
string body = data.dump();

// Use POST with X-HTTP-Method-Override: PATCH
ofHttpRequest request(url, url);
request.method = ofHttpRequest::POST;
request.body = body;

for (auto& pair : getHeaders(true)) {
request.headers[pair.first] = pair.second;
}

ofLogVerbose("SupabaseClient") << "PATCH (via POST) " << url;

ofURLFileLoader loader;
ofHttpResponse response = loader.handleRequest(request);

if (response.status >= 200 && response.status < 300) {
onSuccess();
} else {
ofLogError("SupabaseClient") << "UPDATE failed: " << response.status;
onError("HTTP " + ofToString(response.status));
}
}

//--------------------------------------------------------------
void SupabaseClient::deleteAsync(const string& table, const string& filter,
                                  std::function<void()> onSuccess,
                                  std::function<void(string)> onError) {
if (!config.isValid()) {
onError("Client not configured");
return;
}

string url = config.getRestUrl() + "/" + table + "?" + filter;

// Workaround: Query first, then delete by ID
ofHttpRequest selectReq(url, url);
selectReq.method = ofHttpRequest::GET;

for (auto& pair : getHeaders()) {
selectReq.headers[pair.first] = pair.second;
}

ofURLFileLoader loader;
ofHttpResponse selectResp = loader.handleRequest(selectReq);

if (selectResp.status != 200) {
ofLogError("SupabaseClient") << "DELETE: Failed to query: " << selectResp.status;
onError("HTTP " + ofToString(selectResp.status));
return;
}

try {
ofJson records = ofJson::parse(selectResp.data.getText());

if (!records.is_array() || records.empty()) {
ofLogNotice("SupabaseClient") << "DELETE: No records found";
onSuccess();
return;
}

// Delete each by ID
int deletedCount = 0;
for (auto& record : records) {
if (!record.contains("id")) continue;

string id = record["id"].get<string>();
string deleteUrl = config.getRestUrl() + "/" + table + "?id=eq." + id;

ofHttpRequest delReq(deleteUrl, deleteUrl);
delReq.method = ofHttpRequest::POST;
delReq.headers["X-HTTP-Method-Override"] = "DELETE";
delReq.headers["Prefer"] = "return=minimal";

for (auto& pair : getHeaders()) {
delReq.headers[pair.first] = pair.second;
}

ofHttpResponse delResp = loader.handleRequest(delReq);

if (delResp.status >= 200 && delResp.status < 300) {
deletedCount++;
}
}

ofLogNotice("SupabaseClient") << "Deleted " << deletedCount << " records";
onSuccess();

} catch (std::exception& e) {
ofLogError("SupabaseClient") << "DELETE error: " << e.what();
onError("Error: " + string(e.what()));
}
}
