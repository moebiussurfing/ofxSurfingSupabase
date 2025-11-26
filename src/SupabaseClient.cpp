// file: src/SupabaseClient.cpp

#include "SupabaseClient.h"

//--------------------------------------------------------------
void SupabaseClient::setup(SupabaseConfig& cfg) {
	config = cfg;
	
	if (!config.isValid()) {
		ofLogError("SupabaseClient") << "Invalid configuration";
		return;
	}
	
	// If using email/password auth, authenticate now
	if (config.needsAuthentication()) {
		ofLogNotice("SupabaseClient") << "Email/password mode - authenticating...";
		authenticateAsync(
			[this](string userId) {
				ofLogNotice("SupabaseClient") << "Authenticated successfully, user: " << userId;
				bAuthenticated = true;
				ofNotifyEvent(onAuthSuccess, userId);
			},
			[this](string error) {
				ofLogError("SupabaseClient") << "Authentication failed: " << error;
				ofNotifyEvent(onAuthError, error);
			}
		);
	} else {
		ofLogNotice("SupabaseClient") << "Configured successfully (API key mode)";
		bAuthenticated = true;
		string userId = config.userId;
		ofNotifyEvent(onAuthSuccess, userId);
	}
}

//--------------------------------------------------------------
void SupabaseClient::authenticateAsync(function<void(string)> onSuccess,
                                        function<void(string)> onError) {
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
	
	ofLogVerbose("SupabaseClient") << "POST " << url;
	
	ofLoadURLAsync(request, "supabase_auth", [this, onSuccess, onError](ofHttpResponse& response) {
		if (response.status == 200) {
			try {
				ofJson json = ofJson::parse(response.data.getText());
				
				config.accessToken = json["access_token"].get<string>();
				config.userId = json["user"]["id"].get<string>();
				
				ofLogNotice("SupabaseClient") << "Authentication successful";
				onSuccess(config.userId);
				
			} catch (exception& e) {
				ofLogError("SupabaseClient") << "Auth response parse error: " << e.what();
				onError("Parse error: " + string(e.what()));
			}
		} else {
			ofLogError("SupabaseClient") << "Auth failed: " << response.status;
			ofLogError("SupabaseClient") << response.data.getText();
			onError("HTTP " + ofToString(response.status));
		}
	});
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
	
	// Workaround for PATCH method (Supabase accepts this)
	if (usePatch) {
		headers["X-HTTP-Method-Override"] = "PATCH";
	}
	
	return headers;
}

//--------------------------------------------------------------
void SupabaseClient::selectAsync(const string& table, const string& query,
                                  function<void(ofJson)> onSuccess,
                                  function<void(string)> onError) {
	if (!config.isValid()) {
		onError("Client not configured");
		return;
	}
	
	string url = config.getRestUrl() + "/" + table;
	if (!query.empty()) url += "?" + query;
	
	ofHttpRequest request(url, url);
	request.method = ofHttpRequest::GET;
	
	for (auto& [key, val] : getHeaders()) {
		request.headers[key] = val;
	}
	
	ofLogVerbose("SupabaseClient") << "GET " << url;
	
	ofLoadURLAsync(request, "supabase_select", [onSuccess, onError](ofHttpResponse& response) {
		if (response.status == 200) {
			try {
				ofJson json = ofJson::parse(response.data.getText());
				onSuccess(json);
			} catch (exception& e) {
				ofLogError("SupabaseClient") << "JSON parse error: " << e.what();
				onError("JSON parse error: " + string(e.what()));
			}
		} else {
			ofLogError("SupabaseClient") << "SELECT failed: " << response.status;
			onError("HTTP " + ofToString(response.status) + ": " + response.data.getText());
		}
	});
}

//--------------------------------------------------------------
void SupabaseClient::insertAsync(const string& table, const ofJson& data,
                                  function<void(ofJson)> onSuccess,
                                  function<void(string)> onError) {
	if (!config.isValid()) {
		onError("Client not configured");
		return;
	}
	
	string url = config.getRestUrl() + "/" + table;
	string body = data.dump();
	
	ofHttpRequest request(url, url);
	request.method = ofHttpRequest::POST;
	request.body = body;
	
	for (auto& [key, val] : getHeaders()) {
		request.headers[key] = val;
	}
	
	ofLogVerbose("SupabaseClient") << "POST " << url;
	ofLogVerbose("SupabaseClient") << "Body: " << body;
	
	ofLoadURLAsync(request, "supabase_insert", [onSuccess, onError](ofHttpResponse& response) {
		if (response.status >= 200 && response.status < 300) {
			try {
				ofJson json = ofJson::parse(response.data.getText());
				onSuccess(json);
			} catch (exception& e) {
				onSuccess(ofJson::object());
			}
		} else {
			ofLogError("SupabaseClient") << "INSERT failed: " << response.status;
			ofLogError("SupabaseClient") << response.data.getText();
			onError("HTTP " + ofToString(response.status) + ": " + response.data.getText());
		}
	});
}

//--------------------------------------------------------------
void SupabaseClient::updateAsync(const string& table, const string& filter,
                                  const ofJson& data,
                                  function<void()> onSuccess,
                                  function<void(string)> onError) {
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
	
	for (auto& [key, val] : getHeaders(true)) {
		request.headers[key] = val;
	}
	
	ofLogVerbose("SupabaseClient") << "PATCH (via POST) " << url;
	ofLogVerbose("SupabaseClient") << "Body: " << body;
	
	ofLoadURLAsync(request, "supabase_update", [onSuccess, onError](ofHttpResponse& response) {
		if (response.status >= 200 && response.status < 300) {
			onSuccess();
		} else {
			ofLogError("SupabaseClient") << "UPDATE failed: " << response.status;
			ofLogError("SupabaseClient") << response.data.getText();
			onError("HTTP " + ofToString(response.status) + ": " + response.data.getText());
		}
	});
}

//--------------------------------------------------------------
void SupabaseClient::deleteAsync(const string& table, const string& filter,
                                  function<void()> onSuccess,
                                  function<void(string)> onError) {
	if (!config.isValid()) {
		onError("Client not configured");
		return;
	}
	
	string url = config.getRestUrl() + "/" + table + "?" + filter;
	
	ofHttpRequest request(url, url);
	request.method = ofHttpRequest::DELETE;
	
	for (auto& [key, val] : getHeaders()) {
		request.headers[key] = val;
	}
	
	ofLogVerbose("SupabaseClient") << "DELETE " << url;
	
	ofLoadURLAsync(request, "supabase_delete", [onSuccess, onError](ofHttpResponse& response) {
		if (response.status >= 200 && response.status < 300) {
			onSuccess();
		} else {
			ofLogError("SupabaseClient") << "DELETE failed: " << response.status;
			onLogError("SupabaseClient") << response.data.getText();
			onError("HTTP " + ofToString(response.status) + ": " + response.data.getText());
		}
	});
}
