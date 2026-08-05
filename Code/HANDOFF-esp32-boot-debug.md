# Handoff — portage ESP32-S3 + refactor boot séquencé

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

Branche `port/esp32-s3`. 15 fichiers modifiés, non commités au moment de la rédaction de ce
handoff (voir `git status`/`git diff` pour l'état exact). Remote `origin` =
`git@github.com:Mamath2000/WiFi-remote-for-Bestway-Lay-Z-SPA.git`.
