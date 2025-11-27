# ofxSurfingSupabase - Development Notes

**Session:** 2025-11-27  
**Status:** Production Ready

---

## Implementation Summary

### Architecture

```
ofxSurfingSupabase (main API)
    ├─ SupabaseConfig (credentials loader)
    ├─ SupabaseClient (HTTP + Auth)
    │   ├─ Methods: authenticate, CRUD operations
    │   └─ Synchronous HTTP (ofLoadURL/ofHttpRequest)
    ├─ SupabasePresetSync (auto-sync with presetsLite)
    │   └─ Auto push/pull on preset save/load
    └─ SupabasePresetManager (UI + manual operations)
        └─ Methods: load, save, delete, refresh, upsert
```

### Key Design Decisions

1. **Synchronous HTTP** - OF 0.12.1 compatibility
2. **Email/Password Auth** - User-friendly, no manual user_id
3. **UPSERT Pattern** - SELECT → UPDATE or INSERT (avoids conflicts)
4. **DELETE by ID** - Query by name first, then delete by UUID
5. **ofxGui Integration** - ofParameter-based UI
6. **Two Modes** - Hybrid (with presetsLite) vs Pure Remote (standalone)

---

## Technical Details

### Authentication Flow
```
1. Load credentials.txt
2. POST /auth/v1/token with email/password
3. Extract access_token and user UUID
4. Use Bearer token in all subsequent requests
5. Setup sync/manager after auth success
```

### CRUD Operations
- **SELECT:** GET /rest/v1/presets?user_id=eq.{uuid}
- **INSERT:** POST /rest/v1/presets with JSON body
- **UPDATE:** PATCH /rest/v1/presets?id=eq.{uuid}
- **DELETE:** DELETE /rest/v1/presets?id=eq.{uuid}

### UPSERT Implementation
```cpp
1. GET preset by name
2. If exists: PATCH with id
3. If not: POST new preset
4. Return success/error
```

### Row Level Security (RLS)
```sql
-- Ensures users only access their own presets
-- Uses auth.uid()::text from JWT token
-- Policies for SELECT, INSERT, UPDATE, DELETE
```

---

## Solved Problems

### Problem 1: DELETE HTTP 400
**Cause:** Trying to delete by preset_name (not a column filter)  
**Solution:** Query by name → Extract ID → Delete by ID

### Problem 2: INSERT 409 / UPDATE 403
**Cause:** Duplicate key constraint / missing WITH CHECK  
**Solution:** UPSERT pattern + RLS policy fix

### Problem 3: presetsLite Dependency
**Cause:** example2 should be standalone  
**Solution:** Keep dependency (SupabasePresetSync header) but don't use at runtime

### Problem 4: Missing ofxGui Include
**Cause:** ofxPanel used without #include  
**Solution:** Added to ofxSurfingSupabase.h

---

## Code Patterns

### ofParameter void Buttons
```cpp
ofParameter<void> buttonSave{"Save Scene Direct"};
ofAddListener(buttonSave.newListener([this]() {
    sendSceneDirect();
}), this);
```

### Scene Parameter Serialization
```cpp
// Save
ofJson settings;
ofSerialize(settings, scene.params);
db.savePreset("preset_name", settings);

// Load
ofJson settings = db.loadPreset("preset_name");
ofDeserialize(settings, scene.params);
```

### Error Handling
```cpp
if (response.status == 200) {
    ofLogNotice() << "Success";
} else {
    ofLogError() << "Failed: " << response.status;
    ofLogError() << response.data;
}
```

---

## Testing Notes

### Successful Tests
- ✅ Authentication with email/password
- ✅ Save preset to Supabase
- ✅ Load preset from Supabase
- ✅ Delete preset
- ✅ UPSERT (update existing)
- ✅ Browse remote presets
- ✅ Direct scene save/load

### Known Limitations
- Synchronous HTTP (blocking)
- No offline queue
- No conflict resolution (multi-user)
- Auto-generated preset names only
- No custom text input (yet)

---

## Future Enhancements

### High Priority
- [ ] ofParameter<string> for custom preset names
- [ ] Async HTTP (using ofEvents + threads)
- [ ] Offline queue for pending operations

### Medium Priority
- [ ] Preset versioning
- [ ] Preset descriptions (metadata)
- [ ] Conflict resolution UI
- [ ] Mouse interaction in preset list

### Low Priority
- [ ] OAuth with Google
- [ ] Realtime subscriptions
- [ ] Preset sharing between users
- [ ] Preset tags/categories

---

## Performance Notes

### Database
- JSONB column indexable and searchable
- UUID primary key (fast lookups)
- Composite unique constraint prevents duplicates
- Indexes on user_id and preset_name

### Network
- Typical latency: 100-300ms (depends on region)
- Payload size: ~1-5KB per preset
- RLS policies add minimal overhead

---

## Security Notes

### What's Protected
✅ Credentials in local file (not committed)  
✅ RLS policies (users only see own data)  
✅ Anon key (low privilege)  
✅ JWT auth tokens (short-lived)

### What's NOT Protected
❌ credentials.txt in bin/data/ (user responsibility)  
❌ Supabase project URL (public)  
❌ Anon key (public, but restricted by RLS)

### Best Practices
- Never commit credentials.txt
- Use .gitignore for bin/data/credentials.txt
- Rotate passwords regularly
- Use strong passwords (12+ chars)

---

## Debugging Tips

### Enable Debug Logs
```cpp
db.bShowDebug = true;
```
Shows all HTTP requests/responses

### Check Supabase Dashboard
- Table Editor → See raw data
- Authentication → Verify user exists
- SQL Editor → Test queries manually
- Logs → See API requests

### Common Console Messages
```
[notice] Authenticated successfully, user: xxx
[notice] Loaded 5 presets
[error] DELETE failed: 400  ← Check RLS policies
[error] Preset not found     ← Refresh list first
```

---

## Workflow for AI Agents

See **AI-AGENTS-GUIDE.md** for:
- Project structure
- Code style rules
- Implementation guidelines
- Testing procedures

---

**Last Updated:** 2025-11-27  
**Maintainer:** ofxSurfing

