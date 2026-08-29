# PlatformIO shortcuts for the ESP32-S3 build (override with `make ENV=nodemcuv2 ...`)
ENV ?= esp32s3-devkitc1u-n16r8

.PHONY: build upload buildfs uploadfs flash monitor clean help

help:
	@echo "make build     - compile firmware ($(ENV))"
	@echo "make upload    - build + OTA upload firmware"
	@echo "make buildfs   - build filesystem image (webInterface/ + data_base/)"
	@echo "make uploadfs  - build + OTA upload filesystem image"
	@echo "make flash     - upload firmware then filesystem (most common after web/i18n changes)"
	@echo "make monitor   - open serial monitor"
	@echo "make clean     - clean build artifacts"
	@echo "Override target board with ENV=nodemcuv2"

build:
	pio run -e $(ENV)

upload:
	pio run -e $(ENV) -t upload

buildfs:
	pio run -e $(ENV) -t buildfs

uploadfs:
	pio run -e $(ENV) -t uploadfs

flash: upload uploadfs

monitor:
	pio device monitor -e $(ENV)

clean:
	pio run -e $(ENV) -t clean
