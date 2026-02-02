# Supabase Setup Guide

**Quick setup for ofxSurfingSupabase** - Browser-based configuration (5 minutes)

---

## Step 1: Create Project

1. Go to https://supabase.com → Sign in
2. Dashboard → **New project**
3. Fill:
   - Name: `ofxSurfingTest`
   - Database Password: (strong password)
   - Region: Closest to you
   - Plan: **Free**
4. Click **Create** → Wait 2 minutes

---

## Step 2: Create Table

1. Sidebar → **SQL Editor** → **New query**
2. Paste and **Run**:

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
CREATE INDEX idx_presets_name ON presets(preset_name);

ALTER TABLE presets ENABLE ROW LEVEL SECURITY;

CREATE POLICY "Users see own presets"
  ON presets FOR SELECT
  USING (user_id = auth.uid()::text);

CREATE POLICY "Users insert own presets"
  ON presets FOR INSERT
  WITH CHECK (user_id = auth.uid()::text);

CREATE POLICY "Users update own presets"
  ON presets FOR UPDATE
  USING (user_id = auth.uid()::text)
  WITH CHECK (user_id = auth.uid()::text);

CREATE POLICY "Users delete own presets"
  ON presets FOR DELETE
  USING (user_id = auth.uid()::text);
```

3. Verify: "Success. No rows returned"

---

## Step 3: Create User

1. Sidebar → **Authentication** → **Providers**
2. Find **Email** → Settings:
   - ✅ Enable Email provider
   - ❌ **Disable** "Confirm email" (for testing)
   - Save
3. Sidebar → **Authentication** → **Users** → **Add user**
4. Fill:
   - Email: `test@ofxsurfing.com`
   - Password: `testpass123`
   - ✅ **Auto Confirm User**
5. Click **Create user**

---

## Step 4: Get Credentials

1. Sidebar → **Settings** → **API**
2. Copy:
   - **Project URL**: `https://xxxxx.supabase.co`
   - **anon public** key: `eyJhbGc...` (long JWT)

---

## Step 5: Configure App

Edit `bin/data/credentials.txt`:

```txt
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://xxxxx.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...your_long_key_here
EMAIL=test@ofxsurfing.com
PASSWORD=testpass123
```

---

## Test

1. Compile and run app
2. Check console:
   ```
   [notice] SupabaseConfig: Auth mode: EMAIL_PASSWORD
   [notice] SupabaseClient: Authenticated successfully
   ```
3. GUI shows: **🟢 CONNECTED**

---

## Troubleshooting

### ❌ "Authentication failed: HTTP 400"
- Email/password incorrect
- User not created in Auth → Users
- Email confirmations still enabled

### ❌ "SELECT failed: 403"
- RLS policies not created (re-run SQL in Step 2)
- `user_id` mismatch

### ❌ Status shows RED
- `credentials.txt` missing or wrong path
- Wrong `SUPABASE_URL` or `SUPABASE_ANON_KEY`
- Network/firewall blocking Supabase

---

✅ **Done!** Your database is ready.
