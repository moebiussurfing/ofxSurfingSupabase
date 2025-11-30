# Developer Quick Reference

**ofxSurfingSupabase v0.9.0-beta**

---

## 🚀 Quick Start (3 steps)

```bash
# 1. Compile
cd example && make clean && make

# 2. Run
./bin/example

# 3. Test
# Press Space → Save
# Press L → Load
```

---

## 📁 Project Structure

```
ofxSurfingSupabase/
├── src/
│   ├── ofxSurfingSupabase.h        [120 lines] Main class
│   └── ofxSurfingSupabase.cpp      [686 lines] Implementation
├── libs/
│   └── cpp-httplib/
│       └── include/
│           └── httplib.h           [417 KB]   HTTP client
├── example/
│   ├── src/
│   │   ├── ofApp.h                 [18 lines]  Example app
│   │   └── ofApp.cpp               [68 lines]  Implementation
│   └── bin/data/
│       └── credentials.txt         [YOUR FILE] Supabase config
├── docs/
│   ├── TESTING-GUIDE.md            Test cases
│   ├── CHANGELOG.md                Version history
│   └── TODO.md                     Progress tracking
├── addon_config.mk                 OF configuration
├── COMPILE-AND-TEST.md             Testing instructions
└── PHASE2-SUMMARY.md               What changed
```

---

## 🎯 Core API

### Setup

```cpp
#include "ofxSurfingSupabase.h"

ofxSurfingSupabase db;
ofParameterGroup sceneParams;

void ofApp::setup() {
  db.setup();
  db.setupSceneParams(sceneParams);
  db.bRemoteMode = true;
}
```

### Operations

```cpp
// Save
db.sendSceneDirect();              // Auto timestamp name
db.savePreset("myPreset");         // Custom name

// Load
db.loadAndApplyRemote();           // Selected preset
db.loadPreset("myPreset");         // Specific preset

// Browse
db.selectNext();                   // Navigate forward
db.selectPrevious();               // Navigate backward
db.refreshPresetList();            // Update list from DB

// Delete
db.deletePreset("myPreset");       // Single preset
db.clearDatabase();                // All presets

// Status
bool connected = db.isConnected();
std::string status = db.getConnectionStatus();
```

### UI

```cpp
void ofApp::draw() {
  db.draw();  // Shows GUI + status
}
```

---

## ⌨️ Keyboard Controls (Example)

| Key | Action |
|-----|--------|
| Space | Save scene direct |
| L | Load & apply remote |
| Left Arrow | Previous preset |
| Right Arrow | Next preset |
| R | Refresh list |
| Shift+D | Clear database |

---

## 🔧 Configuration

**File:** `bin/data/credentials.txt`

```ini
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://xxxxx.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...
EMAIL=test@example.com
PASSWORD=yourpassword
```

---

## 📊 Supabase Table Structure

```sql
CREATE TABLE presets (
  id uuid PRIMARY KEY,
  user_id text NOT NULL,
  preset_name text NOT NULL,
  preset_data jsonb NOT NULL,
  created_at timestamptz DEFAULT now(),
  updated_at timestamptz DEFAULT now(),
  UNIQUE(user_id, preset_name)
);
```

---

## 🎓 Expected Console Output

```
[notice] ofxSurfingSupabase: Authenticated successfully
[notice] ofxSurfingSupabase: User ID: xxxxx
[notice] ofxSurfingSupabase: ✓ Found 0 presets

// After pressing Space:
[notice] ofxSurfingSupabase: ✓ Preset saved successfully
[notice] ofxSurfingSupabase: ✓ Found 1 presets

// After pressing L:
[notice] ofxSurfingSupabase: ✓ Preset loaded and applied
```

---

## 🐛 Common Issues

### "Authentication failed"
→ Check credentials.txt (email, password, URL, key)

### "httplib.h not found"
→ Run: `ls libs/cpp-httplib/include/httplib.h` (should exist)

### "🔴 DISCONNECTED"
→ Check internet, Supabase project active, credentials valid

### "No preset selected"
→ Save first (Space), then refresh (R)

---

## 📚 Documentation Quick Links

| File | Purpose |
|------|---------|
| `COMPILE-AND-TEST.md` | Compilation & testing guide |
| `PHASE2-SUMMARY.md` | What changed in beta |
| `docs/TESTING-GUIDE.md` | 13 test cases |
| `docs/CHANGELOG.md` | Version history |
| `README.md` | Full documentation |
| `SUPABASE-SETUP.md` | Database setup |

---

## ✅ Status: v0.9.0-beta

**What Works:**
- ✅ Authentication
- ✅ Save/Load presets
- ✅ Browse presets
- ✅ Delete presets
- ✅ Real database integration
- ✅ Error handling
- ✅ ofxGui integration

**What's Optional (v1.0.0):**
- ⏳ Multithreading
- ⏳ Auto-sync
- ⏳ Toast notifications
- ⏳ Progress indicators

---

## 🎯 Integration Example

```cpp
// MyCustomApp.h
#include "ofxSurfingSupabase.h"

class MyCustomApp : public ofBaseApp {
  ofxSurfingSupabase db;
  
  ofParameter<float> volume{"Volume", 0.5, 0, 1};
  ofParameter<int> tempo{"Tempo", 120, 60, 180};
  ofParameterGroup audioParams{"Audio", volume, tempo};
  
  void setup() {
    db.setup();
    db.setupSceneParams(audioParams);
  }
  
  void keyPressed(int key) {
    if (key == 's') db.sendSceneDirect();
    if (key == 'l') db.loadAndApplyRemote();
  }
};
```

**That's it!** Your parameters are now cloud-synced.

---

## 💡 Tips

1. **Always check `bConnected_` before operations**
2. **Use `bShowDebug = true` for detailed logs**
3. **Preset names are unique per user** (auto-UPSERT)
4. **Timestamps prevent duplicates** (scene_YYYYMMDD_HHMMSS)
5. **GUI shows selected preset** (1/N indicator)

---

## 🆘 Support

**If stuck:**
1. Check console for error messages
2. Review `credentials.txt`
3. Verify Supabase Dashboard → Table Editor
4. See `docs/TESTING-GUIDE.md` for specific cases
5. Check `PHASE2-SUMMARY.md` for implementation details

---

**Version:** 0.9.0-beta  
**Status:** Production Ready (single-threaded)  
**Last Updated:** 2025-11-30

---

**Ready to test!** 🚀

See `COMPILE-AND-TEST.md` for detailed instructions.
