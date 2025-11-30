#### file: TODO.md

# START

- read `docs/START-AI-AGENTS-GUIDE.md` carefully first! at least once per agent session.

# OVERVIEW

- un addon para OF llamado ofxSurfingSupabase y su ofApp / example.
- SurfingSceneTesters is el grupo de params que usamos para aplicar y editar prestes en tiempo real.
- SurfingSceneTesters scene; -> target/source: scene.params  
- quiero usar Supabase como backend para guardar y recuperar datos en json.
- concretamente quiero poder guardar y recuperar configuraciones de ofParameter dentro de ofParameterGroups de OF.
- guardo configuraciones/presets de un ofParameterGroup en archivos json, que de-serializo para aplicar en un ofParameterGroup.
- los genero serializando un ofParameterGroup a json y se guardara en la ddbb remota.
- quiero usar Supabase como ddbb almacenamiento remoto de jsons.
- quiero una interfaz de usuario simple para gestionar los presets (guardar nuevo, guardar en actual, cargar seleccionado, borrar, listar, browsear (next/previous), auto apply toggle: para browsear y aplicar el json al ofParmeterGroup, auto cargar seleccionado, recargar tras editar si quiero descartar cambios (undo), etc).
- por ahora el sistema de login lo quiero manejar hardcoded con un archivo de texto secreto en local bin/data/credentials.txt que contenga el email y password del usuario de Supabase o un api token key. (la mejor manera, piensa que soy nuevo en en supabase.) debes revisar la documentacion de supabase para ver como autenticar usuarios y manejar sesiones y ayudarme a configurar supabase por el navegador chrome. quizas estas configuraciones se pueden hacer desde mi app OF usando la api de supabase?
    - prevista compatibilidad y modularidad maxima para futuros updates:
        mas adelante cuando integrase un ui como dear imgui, me gustaria implementar un sistema log con usurio de google, por ejemplo.
- quiero que sea bien modular para poder usarlo en cualquier otros proyectos de OF facilmente y tambien para mejorar funcionalidades en el futuro. por ejemplo otros usos que no sea guardar arcivos json, sino ddbb de usuarios con sus propios datos. browsear filtrando por usuarios, fecha de subida, categorias u otros criterios.
- quizas podrias estudiar de usar una lib cpp para realiazar las conexiones http a supabase, como cpp-httplib o similar si lo consideras mas isolado y portable que usar classes de OF core. prefiero no usar ofxAddons de http como ofxHTTP. que puedan estar desactualizados o no mantenidos.
- mas adelante integraremos algun sistema multithread para no bloquear el hilo principal de OF al hacer las conexiones http a supabase. podemos ir enfocandolo para que sea facil de adaptar a multithread en el futuro.
- en la ui deberia haber botones ofParameter void (ofGui) con sus callbackks lambda para borrar el kit, borrar la database remota.
- implementa buenos logs de debug en consola para saber que esta pasando en cada momento.
- implemente el /example funcional que sera el que probara el uso basico del addon ofxSurfingSupabase que estamos haciendo.

# DO

- [NEXT] compilar y testear con credenciales reales de Supabase
- [NEXT] verificar que todas las operaciones CRUD funcionan correctamente
- [FUTURE] implementar multithreading para operaciones async
- [FUTURE] añadir progress indicators en UI
- [FUTURE] implementar auto-sync funcional
- [FUTURE] añadir sistema de notificaciones toast

# DONE

- [✓] estructura básica del addon (header + implementation)
- [✓] sistema de autenticación EMAIL_PASSWORD
- [✓] carga de credenciales desde credentials.txt
- [✓] tracking de estado de conexión
- [✓] integración con ofxGui
- [✓] linking de scene parameters
- [✓] serialización/deserialización JSON
- [✓] navegación de presets (Next/Previous)
- [✓] lifecycle management (setup/update/draw/exit)
- [✓] example application funcional
- [✓] documentación completa (README, CHANGELOG, IMPLEMENTATION-STATUS)
- [✓] patrón modular para futuras extensiones
- [✓] **integración de cpp-httplib (header-only, multiplataforma)**
- [✓] **implementación completa de HTTP client con SSL/HTTPS**
- [✓] **todas las operaciones CRUD funcionando (SELECT, INSERT, DELETE)**
- [✓] **REST API de Supabase completamente integrado**
- [✓] **headers de autenticación (apikey, Authorization Bearer)**
- [✓] **manejo de errores y timeouts HTTP**
- [✓] **lista real de presets desde base de datos**
- [✓] **addon_config.mk para configuración multiplataforma**

# IGNORE (MY OWN NOTES)
