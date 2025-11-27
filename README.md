# ofxSurfingSupabase

**Supabase integration for openFrameworks - Remote preset management**

---

## ✨ Features

✅ **Email/Password authentication**  
✅ **Remote JSON preset storage** (PostgreSQL + jsonb)  
✅ **Two workflow modes:**
   - **Hybrid:** Local files + Cloud sync  
   - **Pure Remote:** Cloud-first, no local files  
✅ **Browse/Load/Save/Delete** from database  
✅ **ofxGui integration**  
✅ **Direct scene parameter serialization**  

---

## 🚀 Quick Start

### 1. Setup Supabase (5 minutes)

See **[SUPABASE-SETUP.md](SUPABASE-SETUP.md)** for step-by-step guide:
- Create project
- Create `presets` table (SQL provided)
- Create user
- Get credentials

### 2. Configure Credentials

`bin/data/credentials.txt`:
```
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://your-project.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...your_anon_key
EMAIL=test@ofxsurfing.com
PASSWORD=yourpassword
```

### 3. Choose Your Mode

#### Option A: Pure Remote (Recommended - see example2/)

**Standalone cloud preset system:**

```cpp
#include "ofxSurfingSupabase.h"
#include "surfingSceneTesters.h"

class ofApp : public ofBaseApp {
    SurfingSceneTesters scene;
    ofxSurfingSupabase db;
    
    void setup() {
        scene.setup();
        db.setup();
        db.setupSceneParams(scene.params);
        db.bRemoteMode = true; // Pure remote
    }
    
    void draw() {
        db.draw(); // Shows connection status + UI
    }
};
```

**Workflow:**
- Adjust scene parameters
- Click **"Save Scene Direct"** → Uploads to Supabase
- Browse with **◀ ▶** buttons
- Click **"Load & Apply"** → Restore from cloud
- **NO local JSON files** created

#### Option B: Hybrid (Local + Cloud - see example/)

**With ofxSurfingPresetsLite integration:**

```cpp
#include "ofxSurfingSupabase.h"
#include "ofxSurfingPresetsLiteOfxGui.h"

class ofApp : public ofBaseApp {
    SurfingPresetsLite presetsManager;
    ofxSurfingSupabase db;
    
    void setup() {
        presetsManager.setup(scene.params);
        db.setup();
        db.syncWithPresetsManager(presetsManager);
    }
    
    void draw() {
        presetsManager.drawGui();
        db.draw();
    }
};
```

**Workflow:**
- Work with presetsLite normally (local JSON files)
- Click **"Send to Remote"** → Upload selected preset
- Click **"Load from Remote"** → Download and save locally

---

## 📂 Examples

### [example2/](example2/) - Pure Remote 🆕 **RECOMMENDED**
- Direct scene parameter save/load
- Cloud-only workflow
- No presetsLite dependency
- See `example2/README.md`

### [example/](example/) - Hybrid Mode
- Local JSON files + Cloud sync
- presetsLite integration
- Traditional workflow

---

## 📚 Documentation

- **[SUPABASE-SETUP.md](SUPABASE-SETUP.md)** - Database configuration guide
- **[docs/CHANGELOG.md](docs/CHANGELOG.md)** - Recent changes and fixes
- **[example2/README.md](example2/README.md)** - Pure remote mode guide
- **[AI-AGENTS-GUIDE.md](AI-AGENTS-GUIDE.md)** - For AI assistants

---

## 🎛️ UI Panel

```
┌─ Supabase ────────────────────┐
│ Status: 🟢 CONNECTED          │
│ Auth: EMAIL_PASSWORD          │
│                               │
│ [x] Auto Sync (OFF)           │
│ [x] Remote Mode (ON)          │
│ [x] Show Debug                │
│ [x] Show Preset Manager       │
│                               │
│ [Save Scene Direct]           │ ← Direct from params
│ [Load & Apply]                │ ← Apply without file
│                               │
│ Preset Manager:               │
│ ◀ Previous | Next ▶           │
│ scene_20251127_032145         │
│ [Delete Selected]             │
│ [Refresh List]                │
└───────────────────────────────┘
```

---

## 🔧 Dependencies

**Required:**
- `ofxGui` (OF core addon)
- `ofxSurfingHelpersLite`

**Optional (for hybrid mode):**
- `ofxSurfingPresetsLite`

---

## 🐛 Troubleshooting

**Status shows RED:**
- Check `credentials.txt` exists in `bin/data/`
- Verify SUPABASE_URL and SUPABASE_ANON_KEY
- Check network connection

**"Authentication failed":**
- Email/password incorrect
- User not created in Supabase Dashboard
- Email confirmations not disabled

**"DELETE failed: 400":**
- RLS policies not configured (see SUPABASE-SETUP.md)

---

## 📄 License

MIT

---

## 👤 Author

**ofxSurfing** - moebiusSurfing  
https://github.com/moebiusSurfing

---

**Version:** 1.0.0  
**Last Updated:** 2025-11-27
