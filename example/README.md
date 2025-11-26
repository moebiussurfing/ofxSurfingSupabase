# ofxSurfingSupabase Example

## 🎯 Quick Start

### 1. Configure Supabase (server side)

Go to your Supabase project dashboard and run this SQL:

```sql
CREATE TABLE presets (
  id uuid DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id text NOT NULL,
  preset_name text NOT NULL,
  preset_data jsonb NOT NULL,
  created_at timestamptz DEFAULT now(),
  updated_at timestamptz DEFAULT now(),
  CONSTRAINT unique_user_preset UNIQUE(user_id, preset_name)
);

CREATE INDEX idx_presets_user ON presets(user_id);

ALTER TABLE presets ENABLE ROW LEVEL SECURITY;

-- For API key auth, allow all operations (simplified for testing)
CREATE POLICY "Allow all for authenticated users"
  ON presets FOR ALL
  USING (true)
  WITH CHECK (true);
```

### 2. Get your credentials

From Supabase Dashboard:

**Settings → API:**
- Project URL: `https://xxxxx.supabase.co`
- anon public key: Copy the long JWT token

**For user_id:**
- Authentication → Users → Add user → Create a test user
- Copy the UUID from the user list

### 3. Edit credentials.txt

Edit `bin/data/credentials.txt`:

```txt
project_url=https://your-project.supabase.co
api_key=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.your_actual_key_here
user_id=xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
```

### 4. Run the example

Compile and run. You should see:
- ✅ "Connected to Supabase" message
- ✅ Preset manager GUI (from ofxSurfingPresetsLite)
- ✅ Supabase status panel
- ✅ Auto-sync on startup (downloads existing presets)

---

## 📱 How it works

### Automatic sync:
1. **On startup**: Downloads all your presets from Supabase
2. **When you save a preset**: Automatically uploads to Supabase
3. **On multiple machines**: Presets stay in sync

### Manual controls:
- **G** - Toggle GUI
- **S** - Force sync (re-download from Supabase)
- **P** - Push current preset to Supabase

### The magic line:
```cpp
db.syncWithPresetsManager(presetsManager);
```
This connects both systems. After this, everything is automatic!

---

## 🧪 Testing the sync

1. Save a preset locally (using presetsManager GUI)
2. Check Supabase Dashboard → Table Editor → presets
3. You should see your preset there!
4. Edit the preset data in Supabase
5. Press **S** in the app to sync
6. Load the preset → see the changes!

---

## ⚠️ Troubleshooting

**"DISCONNECTED" status:**
- Check `credentials.txt` exists in `bin/data/`
- Verify all 3 fields are filled
- Check the API key is correct (copy from Supabase dashboard)

**No presets syncing:**
- Check Console logs for HTTP errors
- Verify the `presets` table exists in Supabase
- Check RLS policies allow operations

**Console shows 401 Unauthorized:**
- API key is wrong or expired
- Check you're using the `anon public` key, not `service_role`

---

## 📝 Code flow

```cpp
void ofApp::setup() {
    presetsManager.setup(scene.params);          // Setup local presets
    db.setup();                                   // Connect to Supabase
    db.syncWithPresetsManager(presetsManager);   // Enable auto-sync
}
```

That's it! Three lines for cloud sync.

---

## 🔧 Advanced usage

```cpp
// Check connection status
if (db.isConnected()) {
    // Connected to Supabase
}

// Check if sync is in progress
if (db.isSyncing()) {
    // Show loading indicator
}

// Get last sync time
string time = db.getLastSyncTime();

// Manual operations
db.syncNow();           // Force full sync
db.forcePull();         // Only download
db.pushCurrentPreset(); // Only upload current
```
