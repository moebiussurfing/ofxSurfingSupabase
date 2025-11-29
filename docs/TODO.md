#### file: TODO.md

# OVERVIEW

# DO NO

# DONE

# IGNORE (MY OWN NOTES)

- OAuth Google → Pendiente para futuro con Dear ImGui
- Conflict resolution → No prioritario ahora
- Offline queue → Nice to have
- Preset versioning → Futuro
- cpp-httplib → Por ahora usando ofURLFileLoader (funciona bien)
- Multithread → Enfocado para futuro (async callbacks ya implementados)

- addon para oF llamado ofxSurfingSupabase y su ofApp /example. 
- quiero usar Supabase como backend para guardar y recuperar datos en json.
- concretamente quiero poder guardar y recuperar configuraciones de ofParameter dentro de ofParameterGroups de oF.
- guardo configuraciones/presets de un ofParameterGroup en archivos json en OF, que de-serializo para aplicar en un ofParameterGroup.
- los genero serializando un ofParameterGroup a json.
- quiero hacer lo mismo pero usando Supabase como almacenamiento remoto. quizas manteniendo un mirror de todos los presets del usuario loggeado en supabase en local (archivos json) y remoto (Supabase). podria aver un toggle para activar cada modo: asi a veces usa local y a veces remoto. o los dos. boton void con trigger para sincronizar local -> remoto o remoto -> local.
- quiero usar la base de datos de Supabase para guardar los jsons.
- quiero una interfaz de usuario simple para gestionar los presets (guardar, cargar, borrar, listar)
- por ahora el sistema de login lo quiero manejar hardcoded con un archivo de texto secreto en local bin/data/credentials.txt que contenga el email y password del usuario de Supabase o un api token key. (la mejor manera, piensa que soy nuevo en en supabase.) debes revisar la doc de supabase para ver como autenticar usuarios y manejar sesiones y ayudarme a configurar supabase por el navegador chrome. quizas estas configuraciones se pueden hacer desde mi app OF usando la api de supabase?
    mas adelante cuando integrase un ui como dear imgui, me gustaria implementar un sistema log con usurio de google, por ejemplo.
- quiero que sea bien modular para poder usarlo en otros proyectos de oF facilmente y tambien para mejorar funcionalidades en el futuro. por ejemplo otros usos que no sea guardar arcivos json, sino ddbb de usuarios con sus propios datos. browsear filtrando por usuarios, fechas u otros criterios.
- quizas podrias estudiar de usar una lib cpp para realiazar las conexiones http a supabase, como cpp-httplib o similar si lo consideras mas isolado y portable que usar classes de OF core. cprefiero no usar ofxAddons de http que puedan estar desactualizados o no mantenidos.
- mas adelante integraremos algun sistema multithread para no bloquear el hilo principal de oF al hacer las conexiones http a supabase. podemos ir enfocandolo para que sea facil de adaptar a multithread en el futuro.
- en la ui deberia haber botones ofParameter void (ofGui) con sus callbackks lambda para borrar el kit, borrar la databadse remota, sincronizar local -> remoto, remoto -> local, etc.
- implementa buenos logs de debug en consola para saber que esta pasando en cada momento.
- implemente el /example funcional que sera el que probara el uso bassico del addon ofxSurfingSupabase que estamos haciendo.