# Supabase SQL Setup Guide

## Step 1: Create Table (Already Done ✅)

Your current SQL is **PERFECT**:

```sql
-- Create presets table
CREATE TABLE presets (
  id uuid DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id text NOT NULL,
  preset_name text NOT NULL,
  preset_data jsonb NOT NULL,
  created_at timestamptz DEFAULT now(),
  updated_at timestamptz DEFAULT now(),
  CONSTRAINT unique_user_preset UNIQUE(user_id, preset_name)
);

-- Indexes for performance
CREATE INDEX idx_presets_user ON presets(user_id);
CREATE INDEX idx_presets_name ON presets(preset_name);

-- Enable Row Level Security
ALTER TABLE presets ENABLE ROW LEVEL SECURITY;

-- Security policies (for email/password auth)
CREATE POLICY "Users see own presets"
  ON presets FOR SELECT
  USING (user_id = auth.uid()::text);

CREATE POLICY "Users insert own presets"
  ON presets FOR INSERT
  WITH CHECK (user_id = auth.uid()::text);

CREATE POLICY "Users delete own presets"
  ON presets FOR DELETE
  USING (user_id = auth.uid()::text);
```

---

## Step 2: FIX UPDATE Policy

**IMPORTANT:** Your current UPDATE policy causes 403 errors.

### Current (Broken):
```sql
CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text);
```

### Fixed Version:
```sql
DROP POLICY IF EXISTS "Users update own presets" ON presets;

CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text)
  WITH CHECK (user_id = auth.uid()::text);  -- ← THIS LINE IS CRITICAL
```

**Execute this in Chrome → SQL Editor NOW**

---

## Why This Fix?

PostgreSQL RLS needs **both**:
- `USING`: Check if user can READ the row
- `WITH CHECK`: Check if user can WRITE the row

Without `WITH CHECK`, UPDATE fails with 403 forbidden.

---

## Test After Fix

```bash
cd example
make RunRelease
```

1. Click "Save Scene Direct"
2. Modify sliders  
3. Click "Save Scene Direct" again
4. ✅ Should UPDATE without error 403

---

## Alternative: Disable RLS (Testing Only)

**WARNING:** Only for testing! Not secure for production.

```sql
ALTER TABLE presets DISABLE ROW LEVEL SECURITY;
```

To re-enable:
```sql
ALTER TABLE presets ENABLE ROW LEVEL SECURITY;
```

---

## Verify Policies

Check current policies:
```sql
SELECT * FROM pg_policies WHERE tablename = 'presets';
```

Should show:
- `Users see own presets` (SELECT)
- `Users insert own presets` (INSERT)
- `Users update own presets` (UPDATE) ← **with BOTH USING and WITH CHECK**
- `Users delete own presets` (DELETE)

---

## Complete Setup Checklist

- [x] Table created with correct schema
- [x] Indexes created
- [x] RLS enabled
- [x] SELECT policy working
- [x] INSERT policy working
- [x] DELETE policy working
- [ ] **UPDATE policy fixed** ← DO THIS NOW
- [ ] Test from openFrameworks app

---

## Your Complete Working SQL

```sql
-- 1. Create table (DONE ✅)
CREATE TABLE presets (
  id uuid DEFAULT gen_random_uuid() PRIMARY KEY,
  user_id text NOT NULL,
  preset_name text NOT NULL,
  preset_data jsonb NOT NULL,
  created_at timestamptz DEFAULT now(),
  updated_at timestamptz DEFAULT now(),
  CONSTRAINT unique_user_preset UNIQUE(user_id, preset_name)
);

-- 2. Indexes (DONE ✅)
CREATE INDEX idx_presets_user ON presets(user_id);
CREATE INDEX idx_presets_name ON presets(preset_name);

-- 3. Enable RLS (DONE ✅)
ALTER TABLE presets ENABLE ROW LEVEL SECURITY;

-- 4. Policies (NEED TO FIX UPDATE)
CREATE POLICY "Users see own presets"
  ON presets FOR SELECT
  USING (user_id = auth.uid()::text);

CREATE POLICY "Users insert own presets"
  ON presets FOR INSERT
  WITH CHECK (user_id = auth.uid()::text);

-- FIX THIS ONE:
DROP POLICY IF EXISTS "Users update own presets" ON presets;
CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text)
  WITH CHECK (user_id = auth.uid()::text);

CREATE POLICY "Users delete own presets"
  ON presets FOR DELETE
  USING (user_id = auth.uid()::text);
```

---

## Done! 🎉

Execute the UPDATE policy fix, then test your app.
