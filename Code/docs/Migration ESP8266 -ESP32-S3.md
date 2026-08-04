---
id: esp32-s3-devkitc-1u-n16r8
title: Migration ESP8266 → ESP32-S3 DevKitC-1U N16R8
sidebar_position: 1
---

# Migration ESP8266 → ESP32-S3 DevKitC-1U N16R8

Ce document rassemble les notes de migration existantes et les reformule pour un usage Docusaurus. L'objectif de ce fork est de fournir une version ESP32 moderne du projet `WiFi-remote-for-Bestway-Lay-Z-SPA`, ciblant en particulier l'ESP32-S3 DevKitC-1U N16R8.

## 1. Objectif du fork

Le fork vise à :

- supporter une plateforme moderne et plus stable que l'ESP8266,
- utiliser un ESP32-S3 disposant d'une antenne externe déportée (U.FL/IPEX),
- profiter de 16 Mo de flash et 8 Mo de PSRAM pour des builds plus robustes,
- conserver la compatibilité avec le câblage 6 fils des pompes Bestway/BWC.

## 2. Matériel recommandé

### Carte retenue

- **ESP32-S3-DevKitC-1U-N16R8**
- Module : **ESP32-S3-WROOM-1U**
- Mémoire : **16 MB flash**
- PSRAM : **8 MB octale**
- Antenne : **connecteur U.FL / IPEX**

### Pourquoi ce choix

- le firmware utilise seulement 8 GPIO pour CIO/DSP/sonde + WiFi,
- le module propose une antenne externe, indispensable pour un boîtier pompe fermé,
- l'ESP32-S3 apporte WiFi moderne, Bluetooth LE et une mémoire plus large,
- il existe déjà un environnement ESP32-S3 dans le repo, mais pas pour une carte avec antenne externe.

### Différence avec l'env existant

Le `platformio.ini` actuel contient l'environnement `env:esp32s3-zero` pour la carte **lolin_s3_mini**. Cette carte :

- dispose d'une antenne intégrée, non déportable,
- ne propose que 4 Mo flash et 2 Mo PSRAM,
- n'est pas le bon choix pour ce projet si l'on souhaite une antenne U.FL.

### Caractéristiques techniques du module ESP32-S3-WROOM-1U

- **CPU** : Xtensa LX7 dual-core @ 240 MHz (contre single-core @ 160 MHz sur l'ESP8266 d'origine),
- **RAM interne** : 320 KB SRAM,
- **PSRAM** : 8 Mo en mode **octal SPI** (plus rapide que le quad SPI du module `-zero`, mais mobilise davantage de GPIO réservés, voir ci-dessous),
- **Flash** : 16 Mo QSPI,
- **Connectivité** : WiFi 802.11 b/g/n + Bluetooth LE 5 (BLE non utilisé actuellement par le firmware),
- **USB natif** : contrôleur USB-CDC intégré (GPIO19/20), utilisé ici uniquement pour la programmation/le moniteur série au premier flash,
- **Empreinte mesurée du firmware actuel** (build `esp32s3-devkitc1u-n16r8`) : RAM 16,2 % (53 196 / 327 680 o), Flash 31,6 % (1 057 549 / 3 342 336 o) — large marge disponible pour la suite du portage.

## 3. Contraintes GPIO et broches réservées

### Broches réservées sur les modules octal flash/PSRAM

Sur les modules **ESP32-S3-WROOM-1**, **ESP32-S3-WROOM-1U** et **ESP32-S3-WROOM-2**, certaines broches sont utilisées par le bus SPI/PSRAM et ne doivent pas être exploitées :

- **GPIO35, GPIO36, GPIO37** : bus interne SPI/PSRAM, non disponibles,
- sur un module octal PSRAM, la plage **GPIO26–GPIO37** inclut des signaux flash/PSRAM réservés,
- **GPIO0, GPIO3, GPIO45, GPIO46** : pins de strapping de boot mode — à éviter.

### Broches à éviter sur l’ESP32-S3 DevKitC-1

- **GPIO19 / GPIO20** : USB native (USB_D- / USB_D+), éviter si l'USB OTG n'est pas utilisé,
- **GPIO43 / GPIO44** : UART0 série embarqué (programmation / moniteur USB-UART), à éviter pour ne pas perturber le debug,
- **GPIO38** (v1.1) / **GPIO48** (v1.0) : LED RGB intégrée sur la carte,
- **GPIO0** : boot strap, ne pas connecter d'entrée active pendant le reset.

### Recommandation

Pour ce projet, privilégiez les GPIOs documentés sur les headers latéraux du DevKitC-1 et évitez les broches listées ci-dessus.

## 4. Câblage validé — BWC PCB_V2B vers ESP32-S3-DevKitC-1U N16R8

Configuration testée avec une **pompe 6 fils** (pré-2021 / 2021 air / 54149E).

### Table de signal

| PCB (trou H1/H3) | Fonction         | GPIO ESP32-S3-DevKitC-1U N16R8 |
|------------------|------------------|----------------------------------|
| D1               | CIO data / RX    | GPIO4                            |
| D2               | CIO clk / TX     | GPIO5                            |
| D5               | CIO cs / LD      | GPIO6                            |
| D6               | DSP data / TX    | GPIO7                            |
| D4               | DSP clk / RX     | GPIO15                           |
| D3               | DSP cs / LD      | GPIO16                           |
| D7               | DSP audio        | GPIO17                           |

### Alimentation

| PCB                         | ESP32-S3 DevKitC-1U N16R8 |
|-----------------------------|-----------------------------|
| G (n'importe quel trou G)   | GND                         |
| 3V                          | 3V3                         |
| 5V (net commun H1A/H2A)     | 5V (VBUS USB-C)             |

> Important : vérifiez la continuité entre le trou `5V` du PCB et les pastilles `VU`/`Vin` avant de relier à la broche 5V de l’ESP32. Une erreur sur ce rail peut endommager le module ou le level shifter TXS0108E.

### Entrées réservées pour plus tard

- **D0 / D8** : sonde de température DS18B20 (OneWire), non câblée dans cette version.

## 5. Configuration PlatformIO (environnement `esp32s3-devkitc1u-n16r8`)

L'environnement dédié existe désormais dans `platformio.ini` (à côté de `esp32s3-zero`) :

```ini
[env:esp32s3-devkitc1u-n16r8]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
board_build.filesystem = littlefs
board_build.flash_size = 16MB
board_build.partitions = default_8MB.csv
board_build.psram = true
build_unflags = -std=gnu++11 -std=gnu++14 -std=c++11 -std=c++14
build_flags = 
	-std=gnu++17
	-DBOARD_HAS_PSRAM
	-DFILTER_6W_SPIKES=1
	-DBWC_DEBUGGING=BWC_DEBUG_OUTPUT_SERIAL
	-DBWC_SPRINKLE_YIELDS
upload_speed = 921600
monitor_speed = 115200
extra_scripts = gzip_littlefs.py
lib_deps = 
	bblanchon/ArduinoJson@6.21.2
	links2004/WebSockets@^2.7.3
	knolleary/PubSubClient@^2.8.0
	me-no-dev/AsyncTCP@^3.3.2
	plerup/EspSoftwareSerial@^8.2.0
	milesburton/DallasTemperature@^4.0.6
upload_protocol = espota
upload_port = 192.168.90.160
upload_flags = 
	--auth=esp8266
```

> Remarque : PlatformIO ne propose pas de board dédiée `esp32-s3-devkitc-1u-n16r8`. On utilise ici `esp32-s3-devkitc-1` comme base et on surcharge flash/PSRAM/partitions.

> ⚠️ **Table de partitions et flash 16 Mo** : `default_8MB.csv` ne décrit que les 8 premiers Mo (jusqu'à l'offset `0x800000`). Sur cette carte qui a **16 Mo** de flash, la seconde moitié n'est donc pas adressée par la table de partitions actuelle. Ce n'est pas bloquant vu la marge mesurée (Flash utilisée à 31,6 % sur 8 Mo), mais si vous voulez exploiter les 16 Mo (LittleFS plus grand, double OTA plus confortable...), il faudra passer à `default_16MB.csv` ou une table sur mesure — à valider avant de changer, car cela modifie le layout flash existant.

## 6. Installation et compilation

### 6.1 Prérequis

- Python 3
- PlatformIO Core (CLI), installé soit via `pip`/`pipx`, soit via l'extension VS Code **PlatformIO IDE** (qui crée son propre venv dans `~/.platformio/penv`)

### 6.2 Vérifier/installer PlatformIO Core

```bash
# si installé via pip
pip install --user platformio
pio --version

# si installé via l'extension VS Code (venv dédié)
~/.platformio/penv/bin/pio --version
```

> Si `pio` n'est pas dans le `PATH` (cas fréquent avec l'installation via l'extension VS Code), utilisez le binaire complet `~/.platformio/penv/bin/pio`, ou ajoutez-le au `PATH` :
> ```bash
> export PATH="$HOME/.platformio/penv/bin:$PATH"
> ```

### 6.3 Compiler pour l'ESP32-S3 DevKitC-1U N16R8

Depuis le dossier `Code/` (celui qui contient `platformio.ini`) :

```bash
cd Code
pio run -e esp32s3-devkitc1u-n16r8
```

Au premier lancement, PlatformIO télécharge automatiquement :
- la plateforme `espressif32` et la toolchain `xtensa-esp32s3`,
- les librairies listées dans `lib_deps` (ArduinoJson, WebSockets, PubSubClient, AsyncTCP, EspSoftwareSerial, DallasTemperature).

### 6.4 Problème connu : module Python `intelhex` manquant

Sur une installation fraîche, la génération du bootloader peut échouer avec :

```
ModuleNotFoundError: No module named 'intelhex'
```

C'est une dépendance de `esptool` absente par défaut du venv PlatformIO. Correction :

```bash
~/.platformio/penv/bin/python -m pip install intelhex
```

(adapter le chemin du venv si PlatformIO est installé autrement, ex. `pipx` ou venv perso).

### 6.5 Résultat attendu

```
RAM:   [==        ]  16.2% (used 53196 bytes from 327680 bytes)
Flash: [===       ]  31.6% (used 1057549 bytes from 3342336 bytes)
========================= [SUCCESS] =========================
```

Le binaire final est généré dans `.pio/build/esp32s3-devkitc1u-n16r8/firmware.bin`.

### 6.6 Flasher la carte

- **Premier flash (USB-C)** :
  ```bash
  pio run -e esp32s3-devkitc1u-n16r8 -t upload --upload-port /dev/ttyUSB0
  ```
  (adapter le port : `/dev/ttyACM0` sous Linux selon l'énumération USB-CDC, `COM3` sous Windows, etc. — nécessaire uniquement pour ce premier flash, tant que le firmware WiFi/OTA n'est pas encore actif sur la carte.)

- **Mises à jour OTA** (une fois le firmware actif sur le réseau) — l'environnement est préconfiguré pour l'upload OTA :
  ```bash
  pio run -e esp32s3-devkitc1u-n16r8 -t upload
  ```
  Utilise `upload_port = 192.168.90.160` et `--auth=esp8266` définis dans `platformio.ini`. **Adaptez l'IP à votre carte.**

## 7. Configuration de l’interface web

Dans le menu **Hardware Config** (`hwconfig.html`) du firmware :

- **CIO / DSP** : sélectionnez le modèle 6 fils adapté à la pompe,
- **PCB** : **Custom**,
- **Pinout** : renseignez les champs ci-dessous, avec les valeurs de la colonne ESP32-S3.

| Champ interface (id)        | Libellé UI     | Signal (§4)   | Valeur ESP32-S3 (GPIO brute) |
|------------------------------|----------------|---------------|-------------------------------|
| CIO → `pin1`                 | data/td/rx     | D1            | **4**                         |
| CIO → `pin2`                 | clk/tx         | D2            | **5**                         |
| CIO → `pin3`                 | cs/ld          | D5            | **6**                         |
| DSP → `pin4`                 | data/td/tx     | D6            | **7**                         |
| DSP → `pin5`                 | clk/rx         | D4            | **15**                        |
| DSP → `pin6`                 | cs/ld          | D3            | **16**                        |
| DSP → `pin7`                 | audio          | D7            | **17**                        |
| Sonde température → `pin8`   | sensor pin     | D0/D8         | *(non câblé, laisser vide)*   |

> ⚠️ **Ce champ ne signifie pas la même chose selon la cible.** Sur ESP8266, la valeur saisie est un **index D0-D8** (traduit en GPIO réelle via une table interne, `bwc.cpp` fonction `_loadHardware`). Sur ESP32, ce champ contient **directement le numéro de GPIO**, sans traduction. Ne recopiez donc jamais tel quel un jeu de valeurs `Pinout` vu sur un device ESP8266 vers un device ESP32 (ou inversement) — utilisez la table ci-dessus, qui donne les deux référentiels en regard.

## 8. Sources officielles et ressources

- Documentation officielle ESP32-S3 DevKitC-1 : https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html
- Module ESP32-S3-WROOM-1U datasheet : [ESP32-S3-WROOM-1 / WROOM-1U datasheet](./reference/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf)

### Schéma pinout officiel

![ESP32-S3 DevKitC-1 pinout](./images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg)

## 9. Notes complémentaires

- Vérifiez toujours la référence exacte **`ESP32-S3-DevKitC-1U-N16R8`** et non `ESP32-S3-DevKitC-1-N16R8`.
- Pour l’usage d’une antenne externe, la variante **-U** est impérative.
- Cette documentation est conçue pour être utilisée en Docusaurus ; le fichier est situé dans `docs/` avec une image et une référence PDF locales.
