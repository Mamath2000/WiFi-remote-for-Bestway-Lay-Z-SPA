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

## 5. Configuration PlatformIO recommandée

Le repo contient déjà un env `esp32s3-zero`. Pour l’ESP32-S3 DevKitC-1U N16R8, ajoutez un environnement dédié.

### Exemple d'environnement PlatformIO

```ini
[env:esp32s3-devkitc1u-n16r8]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
board_build.filesystem = littlefs
board_build.flash_size = 16MB
board_build.psram = true
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
    links2004/WebSockets
    knolleary/PubSubClient
    me-no-dev/AsyncTCP
    plerup/EspSoftwareSerial
    milesburton/DallasTemperature
upload_protocol = espota
upload_port = 192.168.90.160
upload_flags =
    --auth=esp8266
```

> Remarque : PlatformIO ne propose pas encore de board dédiée `esp32-s3-devkitc-1u-n16r8`. On utilise ici `esp32-s3-devkitc-1` comme base et on surcharge le flash/PSRAM.

## 6. Configuration de l’interface web

Dans le menu **Hardware Config** du firmware, utilisez :

- **CIO / DSP** : sélectionnez le modèle 6 fils adapté à la pompe,
- **PCB** : **Custom**,
- **Pins** : `pin1=4, pin2=5, pin3=6, pin4=7, pin5=15, pin6=16, pin7=17, pin8=`.

## 7. Sources officielles et ressources

- Documentation officielle ESP32-S3 DevKitC-1 : https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html
- Module ESP32-S3-WROOM-1U datasheet : [ESP32-S3-WROOM-1 / WROOM-1U datasheet](./reference/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf)

### Schéma pinout officiel

![ESP32-S3 DevKitC-1 pinout](./images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg)

## 8. Notes complémentaires

- Vérifiez toujours la référence exacte **`ESP32-S3-DevKitC-1U-N16R8`** et non `ESP32-S3-DevKitC-1-N16R8`.
- Pour l’usage d’une antenne externe, la variante **-U** est impérative.
- Cette documentation est conçue pour être utilisée en Docusaurus ; le fichier est situé dans `docs/` avec une image et une référence PDF locales.
