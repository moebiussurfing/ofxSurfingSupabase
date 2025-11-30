# Testing Guide - ofxSurfingSupabase

**Version:** 0.9.0-beta  
**Status:** Ready for Testing

---

## 🎯 Pre-Test Checklist

### 1. Supabase Configuration
- [ ] Supabase project created
- [ ] `presets` table created (see SUPABASE-SETUP.md)
- [ ] Test user created and auto-confirmed
- [ ] RLS policies applied
- [ ] Credentials obtained (URL + anon key)

### 2. App Configuration
- [ ] `bin/data/credentials.txt` exists
- [ ] All fields filled correctly:
  - `AUTH_MODE=EMAIL_PASSWORD`
  - `SUPABASE_URL=https://xxxxx.supabase.co`
  - `SUPABASE_ANON_KEY=eyJhbGc...`
  - `EMAIL=your@email.com`
  - `PASSWORD=yourpassword`

### 3. Compilation
```bash
cd example
make clean
make
```
- [ ] Compiles without errors
- [ ] No SSL/OpenSSL linking errors
- [ ] Binary created successfully

---

## 🧪 Test Cases

### Test 1: Authentication ✅

**Steps:**
1. Run the app: `./bin/example`
2. Check console output

**Expected:**
```
[notice] ofxSurfingSupabase: setup()
[notice] ofxSurfingSupabase: loadCredentials()
[notice] ofxSurfingSupabase: Auth mode: EMAIL_PASSWORD
[notice] ofxSurfingSupabase: URL: https://xxxxx.supabase.co
[notice] ofxSurfingSupabase: authenticate()
[notice] ofxSurfingSupabase: Authenticated successfully
[notice] ofxSurfingSupabase: User ID: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
```

**GUI Status:**
```
Status: 🟢 CONNECTED
```

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

### Test 2: Save Scene Direct ✅

**Steps:**
1. Adjust scene parameters (colors, amount, scale)
2. Press **Space** or click **"Save Scene Direct"**
3. Check console output

**Expected:**
```
[notice] ofxSurfingSupabase: sendSceneDirect()
[notice] ofxSurfingSupabase: savePreset(): scene_20251130_HHMMSS
[notice] ofxSurfingSupabase: Saving to: /rest/v1/presets
[notice] ofxSurfingSupabase: Preset name: scene_20251130_HHMMSS
[notice] ofxSurfingSupabase: ✓ Preset saved successfully
[notice] ofxSurfingSupabase: refreshPresetList()
[notice] ofxSurfingSupabase: ✓ Found 1 presets
```

**Verify in Supabase Dashboard:**
1. Go to Table Editor → `presets`
2. Should see new row with:
   - `user_id`: your user ID
   - `preset_name`: scene_YYYYMMDD_HHMMSS
   - `preset_data`: JSON object
   - `created_at`: current timestamp

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

### Test 3: Refresh Preset List ✅

**Steps:**
1. Click **"Refresh List"** or press **R**
2. Check console output
3. Check GUI status

**Expected Console:**
```
[notice] ofxSurfingSupabase: refreshPresetList()
[notice] ofxSurfingSupabase: ✓ Found N presets
```

**Expected GUI:**
```
Selected: scene_20251130_HHMMSS (1/N)
```

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

### Test 4: Browse Presets ✅

**Steps:**
1. Press **Right Arrow** or click **"Next >"**
2. Press **Left Arrow** or click **"< Previous"**
3. Check console and GUI

**Expected:**
```
[notice] ofxSurfingSupabase: Selected: scene_20251130_120000
[notice] ofxSurfingSupabase: Selected: scene_20251130_130000
```

**GUI Updates:**
```
Selected: scene_20251130_120000 (1/3)
Selected: scene_20251130_130000 (2/3)
```

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

### Test 5: Load & Apply Preset ✅

**Steps:**
1. Save current scene (Space)
2. Change scene parameters (different colors, amount, scale)
3. Select first preset (Left arrow)
4. Press **L** or click **"Load & Apply"**
5. Verify scene parameters revert to saved state

**Expected Console:**
```
[notice] ofxSurfingSupabase: loadAndApplyRemote()
[notice] ofxSurfingSupabase: loadPreset(): scene_20251130_HHMMSS
[notice] ofxSurfingSupabase: ✓ Preset loaded and applied
[notice] ofxSurfingSupabase: Deserialized scene parameters
```

**Visual Verification:**
- Scene should revert to saved state
- Colors should match original
- Amount/scale should restore

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

### Test 6: Delete Single Preset ✅

**Steps:**
1. Select a preset (arrow keys)
2. Click **"Delete Selected"**
3. Check console
4. Verify in Supabase Dashboard

**Expected Console:**
```
[notice] ofxSurfingSupabase: deletePreset(): scene_20251130_HHMMSS
[notice] ofxSurfingSupabase: ✓ Preset deleted successfully
[notice] ofxSurfingSupabase: refreshPresetList()
[notice] ofxSurfingSupabase: ✓ Found N-1 presets
```

**Dashboard Verification:**
- Row should be removed from `presets` table

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

### Test 7: Clear Database ⚠️

**WARNING:** This deletes ALL presets!

**Steps:**
1. Press **Shift+D** or click **"Clear Database"**
2. Check console
3. Verify in Dashboard

**Expected Console:**
```
[notice] ofxSurfingSupabase: clearDatabase()
[warning] ofxSurfingSupabase: ⚠️  Deleting ALL presets for user: xxxxx
[notice] ofxSurfingSupabase: ✓ Database cleared successfully
```

**GUI Updates:**
```
Selected: (empty - no presets)
```

**Dashboard Verification:**
- All rows for your user_id should be deleted

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```


```

---

## 🔧 Error Handling Tests

### Test 8: Invalid Credentials

**Steps:**
1. Edit `credentials.txt` with wrong password
2. Restart app

**Expected:**
```
[error] ofxSurfingSupabase: Authentication failed: HTTP 400
[error] ofxSurfingSupabase: {"error":"Invalid login credentials","error_description":"..."}
```

**GUI:**
```
Status: 🔴 DISCONNECTED
```

**Result:** [ ] PASS / [ ] FAIL

---

### Test 9: Network Timeout

**Steps:**
1. Disconnect network
2. Try to save preset

**Expected:**
```
[warning] ofxSurfingSupabase: Not connected
```
OR
```
[error] ofxSurfingSupabase: HTTP POST exception: ...
```

**Result:** [ ] PASS / [ ] FAIL

---

### Test 10: Empty Database

**Steps:**
1. Clear database (Shift+D)
2. Try to load preset (L)

**Expected:**
```
[warning] ofxSurfingSupabase: No preset selected
```

**Result:** [ ] PASS / [ ] FAIL

---

## 📊 Performance Tests

### Test 11: Multiple Rapid Saves

**Steps:**
1. Press Space 10 times quickly
2. Wait for all requests to complete
3. Check Supabase Dashboard

**Expected:**
- All 10 presets created
- No crashes
- No duplicate timestamps (within same second)

**Result:** [ ] PASS / [ ] FAIL

**Time taken:** ___ seconds

---

### Test 12: Large Scene Data

**Steps:**
1. Add more parameters to scene (simulate large preset)
2. Save and load

**Expected:**
- Saves successfully
- Loads successfully
- No JSON truncation

**Result:** [ ] PASS / [ ] FAIL

---

## 🎓 Integration Tests

### Test 13: Full Workflow

**Complete scenario:**
1. Start fresh (clear database)
2. Create 3 different presets
3. Browse all presets
4. Load each preset and verify
5. Delete middle preset
6. Verify list updates correctly

**Result:** [ ] PASS / [ ] FAIL

**Notes:**
```




```

---

## 📝 Test Summary

**Date:** _______________  
**Tester:** _______________  
**Platform:** [ ] macOS [ ] Linux [ ] Windows

**Results:**
- Tests Passed: ___ / 13
- Tests Failed: ___ / 13
- Tests Skipped: ___ / 13

**Critical Issues Found:**
```




```

**Minor Issues Found:**
```




```

**Recommendations:**
```




```

**Ready for Production:** [ ] Yes [ ] No [ ] Needs Work

---

## 🚀 Next Steps After Testing

### If All Tests Pass ✅
- [ ] Update README with "Production Ready" badge
- [ ] Bump version to 1.0.0
- [ ] Create release notes
- [ ] Tag repository

### If Tests Fail ❌
- [ ] Document failures in GitHub Issues
- [ ] Fix critical bugs
- [ ] Re-test failed cases
- [ ] Update CHANGELOG with fixes

---

**Testing Complete!**

See `CHANGELOG.md` for version history and `README.md` for usage guide.
