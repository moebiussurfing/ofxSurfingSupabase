# 🎉 Changelog - Latest Version

## ✅ FIXED

### 1. **DELETE works** ✅
- Query first → Delete by ID
- NO more HTTP 400/403 errors
- "Delete Selected" and "Clear Database" working

### 2. **UPSERT instead of INSERT+UPDATE** ✅
- NO more 409 (duplicate) or 403 (forbidden) errors
- Logic: SELECT first → UPDATE if exists → INSERT if not
- Method: `upsertPreset(name, json)`

### 3. **Toggle "Remote Mode"** 🆕
```
[x] Remote Mode (OFF)
```
- **OFF:** "Load from Remote" saves local file
- **ON:** "Load & Apply" deserializes directly to scene.params

### 4. **Button "Load & Apply"** 🆕
- Applies remote preset DIRECTLY to scene
- NO local file saved
- Uses `ofDeserialize()` to `scene.params`

### 5. **Button "Save Scene Direct"** 🆕
- Reads `scene.params` directly
- Does NOT require presetsLite
- Generates name: `scene_20251127_004728`

---

## 🎛️ Final UI Panel

```
┌─ Supabase ────────────────────┐
│ [x] Auto Sync (OFF)           │
│ [x] Remote Mode (OFF)    🆕   │
│ [x] Show Debug                │
│ [x] Show Preset Manager       │
│ [Send to Remote]              │
│ [Save Scene Direct]      🆕   │
│ [Load from Remote]            │
│ [Load & Apply]           🆕   │
└───────────────────────────────┘
```

---

## 🔄 Working Modes

### Mode 1: **Hybrid (Remote Mode = OFF)**
1. Work with presetsLite normally
2. "Send to Remote" → Upload local JSON file
3. "Load from Remote" → Download and save file
4. Then load from presetsLite UI

### Mode 2: **Pure Remote (Remote Mode = ON)**
1. "Save Scene Direct" → Upload directly from scene.params
2. Browse remote presets with ◀ ▶
3. "Load & Apply" → Apply WITHOUT local file
4. presetsLite NOT used

---

## 📝 SQL Fix for Supabase

**In Chrome → SQL Editor:**

```sql
-- Fix RLS: Allow UPDATE for same user_id
DROP POLICY IF EXISTS "Users update own presets" ON presets;

CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text)
  WITH CHECK (user_id = auth.uid()::text);
```

---

## 🧪 Testing

### Test 1: UPSERT
```
1. Click "Save Scene Direct"
2. Modify sliders
3. Click "Save Scene Direct" again (same name)
4. ✅ Should update WITHOUT 409/403 error
```

### Test 2: Load & Apply
```
1. Enable "Remote Mode"
2. Browse with ◀ ▶
3. Click "Load & Apply"
4. ✅ Sliders change WITHOUT local file
```

### Test 3: DELETE
```
1. Select remote preset
2. Click "Delete Selected"
3. ✅ Disappears from Supabase table
```

---

## 📂 New: example2/

**Standalone** without presetsLite usage:
- Only scene params
- Only remote save/load
- No local files
- Minimalist UI

---

## 🔧 Modified Files

- `SupabaseClient.cpp` - DELETE fix
- `SupabasePresetManager.h/cpp` - upsertPreset()
- `ofxSurfingSupabase.h/cpp` - Remote Mode + Load & Apply
- `example2/` - Pure remote example
- `docs/` - Translated to English
