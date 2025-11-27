# ofxSurfingSupabase

**Supabase integration for openFrameworks - Remote preset management**

## Features

✅ Email/Password & API Key authentication  
✅ Remote JSON preset storage (PostgreSQL + jsonb)  
✅ Real-time sync with local presets  
✅ Pure remote mode (cloud-first, no local files)  
✅ Browse/Load/Save/Delete from Supabase database  
✅ ofxGui integration  
✅ Two workflow modes: Hybrid & Pure Remote  

---

## Quick Start

### 1. Supabase Setup (Chrome)

**Execute this SQL** (see `SUPABASE-SQL-SETUP.md` for details):

```sql
-- Fix UPDATE policy (CRITICAL)
DROP POLICY IF EXISTS "Users update own presets" ON presets;
CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text)
  WITH CHECK (user_id = auth.uid()::text);
```

### 2. Config File

`bin/data/credentials.txt`:
```
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://your-project.supabase.co
SUPABASE_ANON_KEY=your-anon-key
EMAIL=user@example.com
PASSWORD=yourpassword
```

### 3. Code (Hybrid Mode)

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
        db.draw(); // UI panel
    }
};
```

### 4. Code (Pure Remote Mode)

```cpp
#include "ofxSurfingSupabase.h"

class ofApp : public ofBaseApp {
    SurfingSceneSimple scene;
    ofxSurfingSupabase db;
    
    void setup() {
        scene.setup();
        db.setup();
        db.setupSceneParams(scene.params); // Direct link
        db.bRemoteMode = true; // Enable pure remote
    }
    
    void draw() {
        db.draw();
    }
};
```

---

## Examples

### example/ - Hybrid Mode
- Local JSON files + Cloud sync
- presetsLite integration
- Traditional workflow

### example2/ - Pure Remote 🆕
- NO local JSON files
- Direct scene parameter save/load
- Cloud-first workflow
- See `example2/README.md`

---

## Documentation

- `SUPABASE-SQL-SETUP.md` - Database setup guide
- `docs/CHANGELOG.md` - Recent changes
- `docs/SESSION-SUMMARY.md` - Implementation details
- `example2/README.md` - Pure remote example
- `AI-AGENTS-GUIDE.md` - For AI assistants

---

## Dependencies

- ofxGui (core)
- ofxSurfingHelpersLite
- ofxSurfingPresetsLite (for hybrid mode)

---

## License

MIT

---

## Author

ofxSurfing - moebiusSurfing  
https://github.com/moebiusSurfing
