WiFi remote for Bestway Lay-Z-SPA — fork Mamath2000
====================================================

Fork de l'excellent projet original de [visualapproach](https://github.com/visualapproach/WiFi-remote-for-Bestway-Lay-Z-SPA), qui transforme un ESP8266 (ou ESP32) en télécommande WiFi pour les spas gonflables Bestway Lay-Z-Spa.

> Le projet original reste la référence pour le hardware (PCB, câblage, manuel de montage). Ce fork porte sur le firmware/logiciel : portage ESP32, simplification, intégration Home Assistant et refonte de l'interface web.

- [Ce qui a changé dans ce fork](#ce-qui-a-changé-dans-ce-fork)
- [Cibles de build](#cibles-de-build)
- [Configuration persistante](#configuration-persistante)
- [Documentation](#documentation)
- [Hardware / build](#hardware--build)
- [Disclaimer](#disclaimer)

---

## Ce qui a changé dans ce fork

#### Portage ESP32
Le firmware, initialement ESP8266 uniquement, compile et tourne maintenant sur ESP32 (ESP32-S3 et ESP32-DevKitC V4), en plus de la cible ESP8266 d'origine. Le portage a nécessité :
- Remplacement des accès registre GPIO directs ESP8266 (`ports.h`) par les registres GPIO rapides ESP32 dans les ISR bit-bang CIO/DSP, pour tenir le timing serré du protocole avec le panneau du spa.
- Fix du flood LEDC (`tone()`/`noTone()`) qui provoquait un boot loop sur ESP32.
- Fix du handshake WebSocket qui ne se terminait jamais sur ESP32 (backend synchrone).
- Un boot séquencé en étapes (WiFi → MQTT → découverte HA → vérification/lien spa → run) au lieu d'un enchaînement ad-hoc dans `loop()`.
- Diagnostics de boot (raison de reset ESP32 publiée sur MQTT, pas juste en Serial) et un suivi d'état du lien spa (`binary_sensor spa_link`) affiché dans le footer web.

Détails complets de l'investigation et des correctifs : [HANDOFF-esp32-boot-debug.md](Code/HANDOFF-esp32-boot-debug.md).

#### Simplification des fonctionnalités
Nettoyage des pages et fonctions peu utilisées ou redondantes de l'interface :
- Suppression de la configuration de capteur ambiant (`hwconfig.html`).
- Suppression des pages `webconfig.html`, `remove.html` et de la page de vérification des mises à jour firmware.
- Sauvegarde de la configuration consolidée en une seule fonction `saveAll` au lieu de plusieurs formulaires séparés.

#### Découpage des sensors en device dans Home Assistant
La découverte MQTT Home Assistant publie désormais plusieurs *devices* liés entre eux (via `via_device`) plutôt qu'un unique device fourre-tout :
- **Tech** (device racine) — connectivité, diagnostics, lien spa.
- **Commandes** — contrôle du spa (pompe, chauffage, air, jets…).
- **Energie** — consommation, coût, temps de fonctionnement quotidiens (pompe/chauffage/air/jets).
- **Temperature** — capteurs de température.

Ce découpage donne une vue plus lisible côté HA (un device par usage) au lieu d'un empilement de dizaines d'entités sur une seule carte.

#### Optimisation de l'IHM
- CSS repensé en mobile-first.
- Affichage pseudo-LCD et retouches d'icônes.
- Libellés et tooltips plus clairs sur la configuration WiFi.
- Page de debug avec viewer de logs live en WebSocket (`debug.html` / `debug.js`), utile pour diagnostiquer sans accès série (notamment sur le portage ESP32).
- Support multilingue (6 langues : en, fr, es, it, de, pt) via des fichiers `.txt` éditables dans `/data`.

#### Paramétrage persistant dans des fichiers de configuration
La configuration est stockée dans des fichiers JSON en LittleFS, éditables via l'IHM ou préchargés dans `data_base/` avant l'upload du filesystem :
- `wifi.json` — SSID/mot de passe, IP statique optionnelle, NTP.
- `mqtt.json` — hôte, credentials, topic de base, intervalle de télémétrie.
- `hwcfg.json` — modèle CIO/DSP, PCB, pins, niveaux de puissance par mode.
- `settings.json` — prix du kWh, audio, restauration d'état.

#### Autres améliorations notables
- Calcul du coût énergétique basé sur la puissance nominale du chauffage, avec point de sauvegarde périodique des compteurs accumulés.
- Suivi des temps de fonctionnement quotidiens (pompe, chauffage, air, jets).
- Log MQTT structuré (`bwcLog`, topic `<base>/log`) avec niveau info/debug togglable à chaud depuis un switch HA (`<base>/log_level`) — pas besoin d'accès série pour diagnostiquer.
- Sélection automatique du meilleur point d'accès WiFi parmi les AP connus avant connexion.
- Fix d'une lecture non bornée du payload MQTT sur `/command`, `/command_batch`, `/set_config`, `/log_level`.
- Cache-Control (max-age=3600) sur les fichiers statiques servis, réduisant la charge réseau de l'IHM.

## Cibles de build

Trois environnements PlatformIO :
- `nodemcuv2` — ESP8266 d'origine.
- `esp32s3-devkitc1u-n16r8` — ESP32-S3 (cible principale de ce fork, testée sur matériel réel).
- `esp32-devkitc-v4` — ESP32 classique (WROOM-32U), câblage documenté mais non validé matériel.

Un `Makefile` expose les commandes courantes (`make build`, `make upload`, `make buildfs`, `make flash`, `make monitor` — voir `make help`). Override de la cible avec `ENV=nodemcuv2 make ...`.

## Configuration persistante

Voir [notes.md](notes.md) pour le détail des fichiers `wifi.json`/`mqtt.json`, le fonctionnement de l'OTA (firmware + filesystem), et des conseils de dépannage WiFi (notamment avec des AP Unifi).

## Documentation

Documentation technique du portage dans [Code/docs/](Code/docs/) (Docusaurus) :
- [Migration ESP8266 → ESP32-S3](<Code/docs/Migration ESP8266 -ESP32-S3.md>)
- [Migration ESP8266 → ESP32-DevKitC V4](<Code/docs/Migration ESP8266 - ESP32-DevKitC-V4.md>)

## Hardware / build

Pour le hardware (BOM, schématiques, PCB, câblage), se référer au [projet original](https://github.com/visualapproach/WiFi-remote-for-Bestway-Lay-Z-SPA) et à son [manuel de montage](bwc-manual.pdf) — aucune modification hardware n'est apportée par ce fork au-delà du support ESP32 (mêmes contraintes de câblage, adapter le brochage selon la carte utilisée, voir la doc de migration ci-dessus).

## Disclaimer

> Comme le projet original : c'est un hack. En cas de casse, c'est votre responsabilité.

> Débranchez le secteur avant toute modification hardware, sous peine de danger électrique.

---

Crédits : projet original par [visualapproach](https://github.com/visualapproach/WiFi-remote-for-Bestway-Lay-Z-SPA), support i18n/UI initial par [dodemodexter](README2.md).
