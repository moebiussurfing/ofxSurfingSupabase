# Authentication Modes

**ofxSurfingSupabase** soporta dos modos de autenticación con Supabase.

---

## 🔑 Modo 1: ANON_KEY (Recomendado para empezar)

### ¿Qué es?

Modo de autenticación **simple** que usa solo la **API Key pública** (anon key) de Supabase.

**No requiere:**
- ❌ Email/password
- ❌ Crear usuarios en Supabase Auth
- ❌ Login/logout
- ❌ Gestión de sesiones

**Perfecto para:**
- ✅ Probar rápidamente
- ✅ Apps sin usuarios específicos
- ✅ Datos públicos o compartidos
- ✅ Prototipos y desarrollo

### Configuración

**credentials.txt:**
```ini
AUTH_MODE=ANON_KEY
SUPABASE_URL=https://xxxxx.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...tu_anon_key_aqui
```

Solo necesitas 3 líneas!

### ¿Cómo funciona?

```cpp
// Internamente:
authToken_ = config_.supabaseAnonKey;  // Usa anon key como token
userId_ = "anonymous";                 // Usuario genérico
bConnected = true;
```

Todas las operaciones usan el `anon key` directamente como token de autorización.

### Permisos

Con ANON_KEY, los datos se guardan con `user_id = "anonymous"`.

**RLS Policy necesaria:**

```sql
-- Permitir lectura/escritura anónima
CREATE POLICY "Allow anonymous access"
  ON presets
  FOR ALL
  USING (user_id = 'anonymous')
  WITH CHECK (user_id = 'anonymous');
```

**Todos los usuarios** comparten los mismos presets (identificados como "anonymous").

### Ventajas

✅ **Configuración ultra simple**  
✅ **No necesitas crear usuarios**  
✅ **Funciona inmediatamente**  
✅ **Ideal para testing**  
✅ **Menos puntos de fallo**  

### Desventajas

⚠️ **No hay usuarios individuales** (todos comparten datos)  
⚠️ **Menos seguro** (cualquiera con el anon key puede acceder)  
⚠️ **No hay separación de datos por usuario**  

---

## 🔐 Modo 2: EMAIL_PASSWORD (Autenticación completa)

### ¿Qué es?

Modo de autenticación **completo** con login de usuario vía email/password.

**Requiere:**
- ✅ Email/password válidos
- ✅ Usuario creado en Supabase Auth
- ✅ Login exitoso
- ✅ RLS policies configuradas

**Perfecto para:**
- ✅ Apps multiusuario
- ✅ Datos privados por usuario
- ✅ Producción
- ✅ Separación de datos

### Configuración

**credentials.txt:**
```ini
AUTH_MODE=EMAIL_PASSWORD
SUPABASE_URL=https://xxxxx.supabase.co
SUPABASE_ANON_KEY=eyJhbGc...tu_anon_key_aqui
EMAIL=test@ofxsurfing.com
PASSWORD=testpass123
```

Necesitas 5 líneas + usuario creado en Supabase.

### ¿Cómo funciona?

```cpp
// Internamente hace POST a:
POST /auth/v1/token?grant_type=password
{
  "email": "test@ofxsurfing.com",
  "password": "testpass123"
}

// Respuesta:
{
  "access_token": "eyJhbGc...",  // Token JWT del usuario
  "user": {
    "id": "abc123-..."           // ID único del usuario
  }
}

// Se guarda:
authToken_ = access_token;
userId_ = user.id;
bConnected = true;
```

### Crear Usuario en Supabase

**Dashboard → Authentication → Users → Add user:**

1. Email: `test@ofxsurfing.com`
2. Password: `testpass123`
3. ✅ **Auto Confirm User** (importante!)
4. Create user

### Permisos (RLS Policies)

Con EMAIL_PASSWORD, cada usuario guarda sus propios datos.

**RLS Policy necesaria:**

```sql
-- Solo ver/editar propios presets
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

**Cada usuario** solo ve/edita sus propios presets.

### Ventajas

✅ **Multiusuario real**  
✅ **Datos privados por usuario**  
✅ **Más seguro**  
✅ **RLS automático**  
✅ **Escalable**  

### Desventajas

⚠️ **Más complejo de configurar**  
⚠️ **Requiere crear usuarios**  
⚠️ **Puede fallar si email/password incorrectos**  
⚠️ **Token expira** (necesita refresh en el futuro)  

---

## 📊 Comparación

| Característica | ANON_KEY | EMAIL_PASSWORD |
|----------------|----------|----------------|
| **Complejidad** | Muy simple | Media |
| **Líneas config** | 3 | 5 |
| **Crear usuario** | No | Sí |
| **Multiusuario** | No | Sí |
| **Datos privados** | No | Sí |
| **Seguridad** | Baja | Alta |
| **Para testing** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Para producción** | ⭐⭐ | ⭐⭐⭐⭐⭐ |

---

## 🎯 ¿Cuál usar?

### Usa ANON_KEY si:

- 🚀 Quieres probar rápido
- 🧪 Estás desarrollando/prototipando
- 👥 No necesitas usuarios separados
- 📦 Datos compartidos están bien
- ⚡ Simplicidad es prioridad

### Usa EMAIL_PASSWORD si:

- 👤 Necesitas usuarios individuales
- 🔒 Cada usuario tiene sus datos privados
- �� Es para producción
- 📊 Necesitas estadísticas por usuario
- 🔐 Seguridad es importante

---

## 🔄 Cambiar entre Modos

Solo edita `credentials.txt`:

**De ANON_KEY → EMAIL_PASSWORD:**
```ini
# Cambiar esta línea:
AUTH_MODE=EMAIL_PASSWORD

# Añadir estas líneas:
EMAIL=tu@email.com
PASSWORD=tupassword
```

**De EMAIL_PASSWORD → ANON_KEY:**
```ini
# Cambiar esta línea:
AUTH_MODE=ANON_KEY

# Comentar o borrar:
#EMAIL=tu@email.com
#PASSWORD=tupassword
```

Luego **reiniciar la app** o click en **"Reconnect"**.

---

## 💡 Recomendación

**Para empezar:** Usa `ANON_KEY`  
- Más fácil de configurar
- Menos cosas que pueden fallar
- Funciona inmediatamente

**Cuando funcione:** Cambia a `EMAIL_PASSWORD`  
- Añade usuarios en Supabase
- Actualiza credentials.txt
- Prueba que funciona
- Listo para producción

---

## 🐛 Troubleshooting

### ANON_KEY no conecta

**Check:**
- ✅ `SUPABASE_URL` correcto (con https://)
- ✅ `SUPABASE_ANON_KEY` correcto (muy largo)
- ✅ RLS policy permite `user_id = 'anonymous'`
- ✅ Internet funciona

### EMAIL_PASSWORD no conecta

**Check:**
- ✅ Usuario creado en Supabase Auth
- ✅ Email/password correctos
- ✅ Usuario **auto-confirmado**
- ✅ Email confirmations **deshabilitadas** (para testing)

**Errores comunes:**
```
HTTP 400 → Email/password incorrectos
HTTP 401 → Usuario no confirmado
HTTP 403 → RLS policy bloqueando
```

---

## 📝 Logs de Debug

**ANON_KEY:**
```
[notice] ofxSurfingSupabase: authenticate()
[notice] ofxSurfingSupabase: Using ANON_KEY authentication (no user login)
[notice] ofxSurfingSupabase: ✓ Connected with ANON_KEY
```

**EMAIL_PASSWORD:**
```
[notice] ofxSurfingSupabase: authenticate()
[notice] ofxSurfingSupabase: Using EMAIL_PASSWORD authentication
[notice] ofxSurfingSupabase: ✓ Authenticated successfully
[notice] ofxSurfingSupabase: User ID: abc123-xxxx-...
```

---

**Para más info:** Ver `SUPABASE-SETUP.md`

**Version:** 0.9.0-beta  
**Last Updated:** 2025-11-30
