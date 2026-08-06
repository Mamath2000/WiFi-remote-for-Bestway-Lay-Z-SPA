---
id: esp32-devkitc-v4
title: Migration ESP8266 → ESP32-DevKitC V4 (WROOM-32U, USB-C)
sidebar_position: 2
---

# Migration ESP8266 → ESP32-DevKitC V4 (WROOM-32U, USB-C)

Variante économique de la [migration ESP32-S3](./Migration%20ESP8266%20-ESP32-S3.md) : un ESP32 classique (Xtensa LX6) sur carte DevKitC V4 en connecteur USB-C, pour les cas où le budget PSRAM/16 Mo flash du S3 n'est pas nécessaire.

## 1. Objectif

Fournir une alternative moins chère à l'ESP32-S3-DevKitC-1U N16R8 quand :

- 4 Mo de flash suffisent (pas de besoin de PSRAM),
- l'antenne externe reste souhaitée (module **-32U**, connecteur U.FL/IPEX, comme le `-1U` du S3),
- une carte DevKitC V4 générique en USB-C (déjà en stock ou moins chère) convient.

## 2. Matériel

### Carte retenue

- **ESP32-DevKitC V4**, variante USB-C
- Module : **ESP32-WROOM-32U** (le suffixe **-U** = antenne externe U.FL, comme sur le S3 `-1U` ; à ne pas confondre avec le WROOM-32/32D à antenne PCB intégrée)
- Mémoire : **4 MB flash**, pas de PSRAM
- Antenne : **connecteur U.FL / IPEX**

### Caractéristiques techniques du module ESP32-WROOM-32U

- **CPU** : Xtensa LX6 dual-core @ 240 MHz (contre single-core @ 160 MHz sur l'ESP8266 d'origine, LX7 sur le S3),
- **RAM interne** : 320 KB SRAM, pas de PSRAM,
- **Flash** : 4 Mo QSPI,
- **Connectivité** : WiFi 802.11 b/g/n + Bluetooth Classic/BLE (non utilisé actuellement par le firmware),
- **USB** : pas de contrôleur USB natif sur l'ESP32 classique — la prise USB-C de la carte pilote un pont USB-série externe (CP2102/CH340 selon le fournisseur du DevKitC), utilisé pour la programmation et le moniteur série,
- **Empreinte mesurée du firmware actuel** (build `esp32-devkitc-v4`, compilé et vérifié dans ce dépôt) :
  - RAM : 16.6 % (54 320 / 327 680 o)
  - Flash : **85.2 % (1 116 301 / 1 310 720 o)** — marge nettement plus faible que sur le S3 (31.6 % sur 16 Mo), à surveiller si de nouvelles fonctionnalités sont ajoutées côté firmware.

## 3. Contraintes GPIO et broches réservées

Ces contraintes sont **différentes de celles du S3** (autre puce, autre carte) — ne pas réutiliser le tableau de câblage S3 tel quel.

### Broches à éviter sur l'ESP32-DevKitC V4

- **GPIO0, GPIO2, GPIO5, GPIO12 (MTDI), GPIO15 (MTDO)** : pins de strapping (mode de boot) — à éviter comme sorties actives au reset,
- **GPIO6 à GPIO11** : connectées en interne à la flash SPI intégrée au module — **non utilisables**,
- **GPIO1 (TX0) / GPIO3 (RX0)** : UART0, utilisé par la programmation/le moniteur série — à éviter pour ne pas perturber le debug,
- **GPIO34, GPIO35, GPIO36 (SVP), GPIO39 (SVN)** : entrée seule, pas de pull-up/down interne, **inutilisables en sortie** (donc à exclure pour les signaux CIO/DSP data/clk/cs qui doivent basculer en sortie),
- **GPIO16 / GPIO17** : réservées uniquement sur les modules **WROVER** (PSRAM) — libres et disponibles ici, le WROOM-32U n'a pas de PSRAM.

### Contrainte spécifique au firmware : accès registre rapide

Le bit-bang CIO/DSP ISR (`GPIO.out_w1ts`/`out_w1tc`/`enable_w1ts`/`enable_w1tc`/`in`, voir
[Migration ESP8266 -ESP32-S3.md §10](./Migration%20ESP8266%20-ESP32-S3.md#10-retour-dexpérience--timing-gpio-dans-les-isr-bit-bang-ciodsp))
n'est valable que pour les **GPIO < 32** (registres `w1ts`/`w1tc` simples, pas la banque `out1`/`enable1` pour les GPIO ≥ 32). Réserver les GPIO CIO/DSP (data/clk/cs/audio) à la plage **0-31**, en excluant les broches ci-dessus.

## 4. Câblage — proposition à valider (non testée sur ce modèle de carte)

⚠️ Contrairement au tableau du doc S3 (validé sur matériel réel), **ce mapping n'a pas encore
été testé physiquement** sur une ESP32-DevKitC V4. Il respecte les contraintes GPIO ci-dessus et
reprend l'ordre des signaux PCB déjà utilisé côté S3, mais reste à valider/ajuster selon le
câblage réel avant mise sous tension sur le spa.

| PCB (trou H1/H3) | Fonction         | GPIO ESP32-DevKitC V4 (proposition) |
|-------------------|------------------|--------------------------------------|
| D1                | CIO data / RX    | GPIO4                                 |
| D2                | CIO clk / TX     | GPIO13                                |
| D5                | CIO cs / LD      | GPIO14                                |
| D6                | DSP data / TX    | GPIO16                                |
| D4                | DSP clk / RX     | GPIO17                                |
| D3                | DSP cs / LD      | GPIO18                                |
| D7                | DSP audio        | GPIO19                                |
| D0/D8             | Sonde température (DS18B20, OneWire) | GPIO23 *(non câblé si sonde absente)* |

### Alimentation

| PCB                         | ESP32-DevKitC V4 |
|-----------------------------|--------------------|
| G (n'importe quel trou G)   | GND                |
| 3V                          | 3V3                |
| 5V (net commun H1A/H2A)     | 5V (VBUS USB-C ou VIN) |

> Important : comme pour le S3, vérifiez la continuité entre le trou `5V` du PCB et les pastilles `VU`/`Vin` avant de relier à la broche 5V — une erreur sur ce rail peut endommager le module ou le level shifter TXS0108E.

## 5. Configuration PlatformIO (environnement `esp32-devkitc-v4`)

L'environnement existe dans `platformio.ini` (à côté de `esp32s3-devkitc1u-n16r8`) :

```ini
[env:esp32-devkitc-v4]
platform = espressif32
board = esp32dev
framework = arduino
board_build.filesystem = littlefs
board_build.partitions = default.csv
build_unflags = -std=gnu++11 -std=gnu++14 -std=c++11 -std=c++14
build_flags =
	-std=gnu++17
	-DFILTER_6W_SPIKES=1
	-DBWC_DEBUGGING=BWC_DEBUG_OUTPUT_SERIAL
	-DBWC_SPRINKLE_YIELDS
upload_speed = 921600
monitor_speed = 76800
monitor_filters =
	esp32_exception_decoder
	default
extra_scripts = gzip_littlefs.py
lib_deps =
	bblanchon/ArduinoJson@6.21.2
	links2004/WebSockets@^2.7.3
	knolleary/PubSubClient@^2.8.0
	me-no-dev/AsyncTCP@^3.3.2
	plerup/EspSoftwareSerial@^8.2.0
	milesburton/DallasTemperature@^4.0.6
; upload_protocol = esptool
; upload_port = COM_A_ADAPTER
```

> Remarque : `board = esp32dev` (Espressif ESP32 Dev Module) est le board PlatformIO générique
> pour toute carte DevKitC V4 à base de WROOM-32/32U/32D — il n'y a pas de board dédiée par
> variante d'antenne, contrairement au S3 où `esp32-s3-devkitc-1` est déjà le bon choix pour le
> `-1U`.

> Pas de `-DBOARD_HAS_PSRAM` ni de `board_build.psram` : le WROOM-32U n'a pas de PSRAM
> (contrairement au WROOM-1U du S3).

> `upload_protocol`/`upload_port` restent commentés : `esptool` (protocole par défaut du board
> `esp32dev`) auto-détecte le port série USB-C au premier flash. Décommentez et renseignez le
> port (`COM3` sous Windows, `/dev/ttyUSB0`/`/dev/ttyACM0` sous Linux) uniquement si
> l'auto-détection échoue ou si plusieurs cartes sont branchées.

## 6. Installation et compilation

Prérequis identiques au portage S3 (voir
[§6.1-6.2 du doc S3](./Migration%20ESP8266%20-ESP32-S3.md#61-prérequis)) : PlatformIO Core, et le
module Python `intelhex` si `ModuleNotFoundError: No module named 'intelhex'` apparaît au premier
build (`~/.platformio/penv/bin/python -m pip install intelhex`).

Depuis le dossier `Code/` :

```bash
pio run -e esp32-devkitc-v4          # firmware
pio run -e esp32-devkitc-v4 -t buildfs   # filesystem (webInterface + data_base)
```

Les deux cibles compilent avec succès dans ce dépôt (vérifié le 2026-08-06). Résultat attendu pour le firmware :

```
RAM:   [==        ]  16.6% (used 54320 bytes from 327680 bytes)
Flash: [========= ]  85.2% (used 1116301 bytes from 1310720 bytes)
========================= [SUCCESS] =========================
```

### Premier flash (USB-C)

```bash
pio run -e esp32-devkitc-v4 -t upload --upload-port COM3
```

(adapter le port ; nécessaire uniquement pour ce premier flash, tant que le firmware WiFi/OTA n'est pas encore actif sur la carte). Ne pas oublier `-t uploadfs` (ou l'équivalent OTA filesystem) pour le filesystem, en plus du firmware.

## 7. Configuration de l'interface web

Dans le menu **Hardware Config** (`hwconfig.html`) du firmware :

- **CIO / DSP** : sélectionnez le modèle 6 fils adapté à la pompe,
- **PCB** : **Custom**,
- **Pinout** : renseignez les champs avec les valeurs de la colonne ESP32-DevKitC V4 (§4), ou vos propres GPIO si vous ajustez le câblage.

| Champ interface (id)        | Libellé UI     | Signal (§4)   | Valeur ESP32-DevKitC V4 (GPIO brute, proposition) |
|------------------------------|----------------|---------------|------------------------------------------------------|
| CIO → `pin1`                 | data/td/rx     | D1            | **4**                                                 |
| CIO → `pin2`                 | clk/tx         | D2            | **13**                                                |
| CIO → `pin3`                 | cs/ld          | D5            | **14**                                                |
| DSP → `pin4`                 | data/td/tx     | D6            | **16**                                                |
| DSP → `pin5`                 | clk/rx         | D4            | **17**                                                |
| DSP → `pin6`                 | cs/ld          | D3            | **18**                                                |
| DSP → `pin7`                 | audio          | D7            | **19**                                                |
| Sonde température → `pin8`   | sensor pin     | D0/D8         | **23** *(laisser vide si sonde absente)*             |

> ⚠️ Comme sur ESP32-S3, ce champ contient **directement le numéro de GPIO** sur cible ESP32
> (pas l'index D0-D8 traduit en interne comme sur ESP8266). Ne recopiez jamais un jeu de valeurs
> `Pinout` vu sur un device ESP8266 tel quel.

## 8. Sources officielles

- Documentation officielle ESP32-DevKitC V4 : https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html

## 9. Bug de portage corrigé : pins de fallback par défaut

Sur un filesystem vierge (`hwcfg.json` absent, cas d'un premier flash), `BWC::setup()`
(`lib/BWC_unified/bwc.cpp`) retombait sur des pins de secours codées en dur. Avant correction,
cette branche `#else` (partagée par **tout** ESP32, S3 ou classique) utilisait le set
`4,5,6,7,8,9,10,11`, calibré pour l'ESP32-S3-Zero. Or **GPIO6 à GPIO11 sont câblées en interne
au bus SPI de la flash embarquée sur un module WROOM classique** (CLK/SD0-3/CMD) — les configurer
en GPIO/interruption corrompt le bus flash dès `bwc->setup()`, provoquant un crash quasi
immédiat (`Reset reason: 5 (INT_WDT)`, ~18 ms après boot) et un boot-loop, **avant même d'avoir
pu atteindre l'UI web pour configurer le vrai câblage**.

Fix appliqué (`lib/BWC_unified/bwc.cpp`, différenciation via `CONFIG_IDF_TARGET_ESP32S3`) :
fallback S3 inchangé, nouveau fallback dédié pour ESP32 classique n'utilisant aucune GPIO
réservée (flash interne, strapping, JTAG actif par défaut) :

```cpp
pins[0] = 4;   // CIO DATA / CIO RX
pins[1] = 13;  // CIO CLK  / CIO TX
pins[2] = 14;  // CIO CS
pins[3] = 16;  // DSP DATA / DSP TX
pins[4] = 17;  // DSP CLK  / DSP RX
pins[5] = 18;  // DSP CS
pins[6] = 21;  // AUDIO/PWM
pins[7] = 22;  // Sonde température (OneWire)
```

Ce fallback ne sert qu'à démarrer sans planter le temps d'atteindre `hwconfig.html` — il ne
reflète pas nécessairement le câblage physique réel, qui reste à saisir manuellement (§7).

## 10. Notes complémentaires

- Les autres fixes de portage (accès registre GPIO direct dans les ISR bit-bang CIO/DSP, boucle
  `webSocket->loop()`, lecture MQTT bornée, etc.) sont génériques à toute la famille ESP32 et déjà
  documentés dans le [doc S3, §10](./Migration%20ESP8266%20-ESP32-S3.md#10-retour-dexpérience--timing-gpio-dans-les-isr-bit-bang-ciodsp) — aucune adaptation supplémentaire n'est nécessaire pour cette carte.
- Le tableau de câblage §4/§7 est une **proposition non validée sur matériel réel** — à
  confirmer/ajuster lors du premier branchement, puis à mettre à jour dans ce document une fois
  testé (cf. statut "RÉSOLU, validé sur matériel réel" du doc S3 après son propre test physique).
- Marge flash serrée (85.2 % sur 4 Mo) : si de nouvelles fonctionnalités firmware sont ajoutées,
  vérifier l'usage flash avant de cibler cette carte, ou envisager une table de partitions
  alternative (`min_spiffs.csv`) pour agrandir la partition applicative.
