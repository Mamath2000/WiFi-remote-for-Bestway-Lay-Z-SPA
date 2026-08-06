# Handoff — portage ESP32-S3 + refactor boot séquencé — RÉSOLU

> **Statut (2026-08-06) : tous les blocages listés ci-dessous sont résolus et validés sur
> matériel réel.** Voir [## Résolution](#résolution-session-du-2026-08-06) pour le détail des
> causes racines et des fixes. Ce fichier est conservé comme trace de l'investigation (utile
> si un symptôme similaire réapparaît sur un autre modèle/carte), pas comme TODO actif.

Contexte pour reprendre le fil sur une autre machine (accès USB au board). Coller ce fichier
(ou son chemin) dans le premier message d'une nouvelle session Claude Code.

## Ce qui a déjà été fait et validé

1. **Bug de portage ESP32 corrigé** — `lib/cio/CIO_TYPE1.cpp`, `eopHandler()` (~ligne 168) :
   écrivait dans un registre GPIO direct ESP8266 (`WRITE_PERI_REG`) sans `#ifdef`, empêchant
   `_DATA_PIN` de repasser en `INPUT` sur ESP32 → collision de bus après le 1er échange.
   Remplacé par `pinMode(_DATA_PIN, INPUT)` sous `#else`. **Confirmé fonctionnel sur le
   matériel réel** : le spa démarre et affiche la température correctement depuis ce fix.

2. **Refactor du boot en 6 étapes séquentielles** (plan complet dans
   `/root/.claude/plans/je-trouve-le-fonctionnement-mellow-fairy.md`) :
   - `src/main.h`/`main.cpp` : FSM `BootStage` (BOOT_WIFI → BOOT_MQTT → BOOT_HA_DISCOVERY →
     BOOT_SPA_CHECK → BOOT_SPA_LINK → BOOT_RUNNING), `advanceBootSequence()`,
     `maybeSendHADiscovery()` (extrait du hack de statics qui vivait dans `loop()`).
   - `lib/BWC_unified/bwc.h`/`.cpp` : `spaLinkEverOk()`/`spaLinkHealthy()` basés sur
     `good_packets_count` (seul compteur fiable sur toutes les variantes CIO/DSP).
   - `lib/smarthomes/ha.hpp` : nouveau `binary_sensor spa_link`.
   - `webInterface/` + `data_base/Langue_*.txt` (6 langues) : statut "Spa link" dans le
     footer web (nécessite un reflash **filesystem**, pas juste firmware).
   - 3 correctifs annexes : suppression de `pause_all()` dans `handleFileRead()` (coupait
     les interruptions CIO à chaque téléchargement de fichier HTTP), `ArduinoOTA.onError()`
     qui reboote maintenant au lieu de laisser l'ESP mort après un OTA raté, boucles bornées
     dans `handleHWtest()`/`handleInputs()` (avant : `while(true)` infini).
   - **Compile proprement sur les deux cibles** (`pio run -e esp32s3-devkitc1u-n16r8` et
     `-e nodemcuv2`, + `pio run -e esp32s3-devkitc1u-n16r8 -t buildfs` pour le filesystem).
   - **Pas encore testé sur le matériel réel** — c'est l'objet du blocage actuel.

## Résolution (session du 2026-08-06)

### 1. Boot loop initial : brownout

Ajout d'un log de `esp_reset_reason()` juste après `Serial.begin()` (donc en ASCII fiable
même quand le reste du dump de crash est illisible - voir §4 ci-dessous). Résultat sans appel
: **`BROWNOUT`**. Le reset survient systématiquement pile au démarrage du SoftAP WiFi (pic de
courant TX radio, en plus du bit-banging CIO/DSP et du level shifter TXS0108E). Fix matériel
(pas logiciel) : câble/port USB de meilleure qualité ou alimentation externe 5V capable
d'encaisser le pic. Confirmé : `Reset reason: 1 (POWERON)` propre après changement d'alim.

### 2. Flood LEDC (`ledc: ledc_set_duty is not initialized`)

Confirmé fatal, contrairement à l'hypothèse initiale de "juste un warning". `noTone()` sur
ESP32 (arduino-esp32 core 3.x) poste dans une queue FreeRTOS (capacité 128) traitée par une
tâche qui logue une erreur LEDC à chaque appel sur un pin jamais attaché. `dsp->handleStates()`
rappelait `noTone()` à chaque `loop()` principal (donc en continu), la queue se remplissait
plus vite qu'elle ne se vidait (chaque log prenant du temps à sortir sur l'UART à 76800 bauds)
jusqu'à bloquer `loopTask` sur l'envoi -> Task Watchdog -> reboot. Fix : `tone()`/`noTone()`
seulement sur changement réel de `audiofrequency` (`lib/dsp/DSP_TYPE1.cpp`/`DSP_TYPE2.cpp`).
C'était bien lié au bug de "bip continu" mentionné plus bas dans l'ancienne section.

### 3. Le "bloc de données binaires illisibles"

Confirmé cosmétique : bannière de reset ROM à un baud fixe différent de celui de l'app,
garbled par le moniteur PlatformIO. Pas le contenu du crash. `monitor_filters =
esp32_exception_decoder, default` ajouté à l'env pour les prochains crashs ASCII.

### 4. Nouveau blocage découvert : ESP32 ne discutait pas avec le spa (bruit continu sur boutons)

Une fois le boot stabilisé, un second problème est apparu (absent du blocage initial) : une
fois branché sur le vrai spa, le CIO/DSP ne communiquait pas et le panneau du spa émettait un
bruit continu inquiétant. Root cause : les ISR bit-bang CIO (`lib/cio/CIO_TYPE1.cpp` :
`isr_packetHandler`/`isr_clkHandler`/`eopHandler`) utilisaient `digitalWrite`/`digitalRead`/
`pinMode` côté ESP32 (`#else` du `#ifdef ESP8266`), alors que le code ESP8266 d'origine
accède aux registres GPIO en direct via `ports.h` pour tenir le timing serré du protocole.
Le surcoût Arduino HAL sur ESP32 mangeait la marge de timing. Fix : accès direct à la struct
`GPIO` d'ESP32 (`GPIO.out_w1ts`/`out_w1tc`, `GPIO.enable_w1ts`/`enable_w1tc`, `GPIO.in`),
équivalent structurel des registres ESP8266.

En creusant, bug distinct trouvé dans `lib/cio/CIO_TYPE2.cpp` (modèles 54149E/54173/54154/
54144/54138/54123, pas le modèle testé) : les mêmes appels `READ_PERI_REG`/`WRITE_PERI_REG`
y tournaient **sans aucun `#ifdef ESP8266`**. `ports.h` code en dur des adresses physiques
ESP8266 (`0x60000300`-`0x60000318`) sans garde de plateforme ; sur ESP32-S3 ces adresses
tombent dans les registres d'**UART0** (vérifié dans le SDK : `DR_REG_UART_BASE = 0x60000000`,
alors que le vrai GPIO est à `DR_REG_GPIO_BASE = 0x60004000`). Chaque front d'horloge CIO
lisait/corrompait donc UART0 (le port de debug série !) au lieu du vrai pin. Corrigé avec le
même pattern `#ifdef`/registres GPIO rapides que CIO_TYPE1.

**Validé sur matériel réel** (ESP32-S3 + spa MALDIVES2021, via MQTT structuré `layzspa/log`
pendant le test live) : CIO+DSP link établis, communication stable dans la durée, bruit
disparu, panneau plus réactif qu'avec l'ESP8266 d'origine.

### 5. Bonus découverts pendant l'investigation

- WebSocket ne complétait jamais son handshake sur ESP32 : `webSocket->loop()` manquant dans
  `loop()` (nécessaire pour le backend synchrone `NETWORK_ESP32` de la lib WebSockets,
  contrairement au backend async utilisé côté ESP8266).
- Lecture non bornée du payload MQTT (`(const char *)&payload[0]` sans `length`) dans
  `mqttCallback()` pour `/command`, `/command_batch`, `/set_config`, `/log_level`.
- Log MQTT structuré ajouté (`bwcLog()` dans `lib/BWC_unified/bwc_debug.h`/`.cpp`) : publie
  sur `<base>/log` en JSON, niveau info/error par défaut, debug togglable à chaud via
  `<base>/log_level` — c'est ce qui a servi à diagnostiquer le point 4 sans accès série.

Historique complet des commits (branche `port/esp32-s3`) : `e7914f8` → `6f74be3`.

---

*Ce qui suit est le contenu original de l'investigation, conservé tel quel pour référence
historique (hypothèses de l'époque, certaines confirmées ci-dessus, d'autres écartées).*

## Blocage en cours : crash-loop après reflash

Après avoir reflashé **firmware + filesystem** (donc LittleFS entièrement vidé — plus de
`hwcfg.json`/`wifi.json`/`mqtt.json`/`settings.json`/`cmdq.json`/`webconfig.json`), l'ESP32
redémarre en boucle toutes les ~350-450ms. Log OTA/monitor (baud probablement pas fiable,
voir plus bas) :

```
Setup > Start @ millis: 252
[ 262][E] open(): /littlefs/hwcfg.json does not exist, no permits for creation
[ 280][E] open(): /littlefs/cmdq.json does not exist, no permits for creation
[ 297][E] open(): /littlefs/settings.json does not exist, no permits for creation
E (312) ledc: ledc_set_duty(725): LEDC is not initialized
E (313) ledc: ledc_update_duty(653): LEDC is not initialized
[ 323][E] open(): /littlefs/webconfig.json does not exist, no permits for creation
startWiFi() @ millis: 341
[ 416][E] open(): /littlefs/wifi.json does not exist, no permits for creation
WiFi > AP info not found. Using last known AP ...
Station > disconnected. Starting soft AP
<bloc de données binaires/illisibles>
Setup > Start @ millis: 252   <- reboot
```

### Ce qu'on sait / hypothèses de travail (non vérifiées, investigation interrompue)

- Les erreurs `open(): .../xxx.json does not exist` sont **attendues** (fs vide) et ne
  devraient pas être fatales en soi — `_loadHardware()` a un fallback documenté sur des pins
  par défaut. **Pas encore lu en détail le contenu exact de ce fallback** (fichier
  `lib/BWC_unified/bwc.cpp`, fonction `_loadHardware()`) — c'est la première chose à vérifier.
- **`ledc: ledc_set_duty is not initialized`** apparaît pendant `bwc->setup()`/`bwc->loop()`
  (avant `startWiFi()`). Piste principale : `lib/dsp/DSP_TYPE1.cpp:22` appelle
  `noTone(_AUDIO_PIN)` — sur ESP32, `tone()`/`noTone()` passent par le périphérique LEDC ; si
  `noTone()` est appelé sur un pin qui n'a **jamais** eu de `tone()` avant (canal LEDC jamais
  attaché), certaines versions du core ESP32-Arduino loggent exactement cette erreur. À
  vérifier : est-ce fatal (crash) ou juste un warning logué sans conséquence ? Le crash
  effectif pourrait être ailleurs.
  - Point de départ possible pour une piste précédente non résolue dans la même session :
    un bug de "bip continu" avait été observé sur bouton pressé, jamais élucidé — pourrait
    être lié au même sujet LEDC/tone().
- Le **bloc de données binaires illisibles** après "Starting soft AP" ressemble à la
  bannière de reset du bootloader ROM ESP32 (imprimée à un baud différent — 74880 en général
  — de celui configuré par l'app et écouté par le moniteur, d'où le charabia). Si c'est bien
  ça, ce n'est **pas** le contenu du crash lui-même, juste un artefact cosmétique confirmant
  qu'un **reset matériel a eu lieu** juste après le début de `startSoftAp()`. La vraie raison
  du reset (panic/watchdog/brownout) n'est pas lisible dans ce log.
- Pistes non encore explorées avant l'interruption :
  - `startSoftAp()` et surtout **`selectBestAccessPoint()`** (`src/main.cpp`, fonction
    **toute récente**, commit `2b0aa40` — jamais testée avec un `wifi.json` totalement
    absent / zéro AP connu). Le crash semble se produire pendant ou juste après l'appel à
    `startSoftAp()`.
  - Contenu exact du fallback de `_loadHardware()` (modèle/pins par défaut utilisés quand
    `hwcfg.json` est absent) — un pin audio DSP par défaut invalide (-1 ou 0) pourrait
    expliquer l'erreur LEDC.

### Recommandation pour la suite (utile avec accès USB)

1. Ajouter `monitor_filters = esp32_exception_decoder` à l'env `esp32s3-devkitc1u-n16r8`
   dans `platformio.ini` (pas encore fait) pour avoir un backtrace décodé lisible au lieu de
   texte brut/charabia.
2. Flasher et observer via `pio device monitor -b 115200` (USB direct, baud fiable
   contrairement à l'OTA) pendant tout le cycle de boot pour capturer la vraie bannière de
   reset ROM + un éventuel backtrace complet.
3. Lire `_loadHardware()` (`lib/BWC_unified/bwc.cpp`) et `startSoftAp()`/
   `selectBestAccessPoint()` (`src/main.cpp`) pour confirmer/infirmer les pistes ci-dessus.
4. Une fois le boot stabilisé, reconfigurer manuellement (fs vidé) : WiFi + MQTT (identifiants
   à ressaisir), et Hardware Config connu-bon : **CIO/DSP = "6 wire, 2021 (air+jet)", PCB =
   Custom, pins CIO data/clk/cs = 4/5/6, DSP data/clk/cs/audio = 7/15/16/17**.

## État git

*(état au moment de la rédaction initiale, avant résolution - voir le tableau de commits
dans la section [Résolution](#résolution-session-du-2026-08-06) pour l'état final)*

Branche `port/esp32-s3`. 15 fichiers modifiés, non commités au moment de la rédaction de ce
handoff (voir `git status`/`git diff` pour l'état exact). Remote `origin` =
`git@github.com:Mamath2000/WiFi-remote-for-Bestway-Lay-Z-SPA.git`.
