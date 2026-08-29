#include <Arduino.h>
#include "ArduinoJson.h"
#include "config.h"
#include "main.h"

void setupHA()
{
    //HeapSelectIram ephemeral;
    //HeapSelectDram ephemeral;


    
    /* Save RAM by storing strings in FLASH */

    PGM_P fs_dev = PSTR("dev");
    PGM_P fs_name = PSTR("name");
    PGM_P fs_uniq_id = PSTR("uniq_id");
    PGM_P fs_stat_t = PSTR("stat_t");
    PGM_P fs_avty_t = PSTR("avty_t");
    PGM_P fs_pl_avail = PSTR("pl_avail");
    PGM_P fs_pl_not_avail = PSTR("pl_not_avail");
    PGM_P fs_val_tpl = PSTR("val_tpl");
    PGM_P fs_cmd_t = PSTR("cmd_t");
    PGM_P fs_cmd_tpl = PSTR("cmd_tpl");
    PGM_P fs_conf_url = PSTR("configuration_url");
    PGM_P fs_connections = PSTR("connections");
    PGM_P fs_identifiers = PSTR("identifiers");
    PGM_P fs_manufacturer = PSTR("manufacturer");
    PGM_P fs_model = PSTR("model");
    PGM_P fs_sw_v = PSTR("sw_version");
    PGM_P fs_alive = PSTR("Alive");
    PGM_P fs_dead = PSTR("Dead");
    PGM_P fs_min = PSTR("min");
    PGM_P fs_max = PSTR("max");
    PGM_P fs_icon = PSTR("icon");
    

    PGM_P fs_max_temp = PSTR("max_temp");
    PGM_P fs_min_temp = PSTR("min_temp");
    PGM_P fs_precision = PSTR("precision");
    PGM_P fs_temp_unit = PSTR("temp_unit");
    PGM_P fs_modes = PSTR("modes");
    PGM_P fs_mode_cmd_t = PSTR("mode_cmd_t");
    PGM_P fs_mode_cmd_tpl = PSTR("mode_cmd_tpl");
    PGM_P fs_mode_stat_t = PSTR("mode_stat_t");
    PGM_P fs_mode_stat_tpl = PSTR("mode_stat_tpl");
    PGM_P fs_act_t = PSTR("act_t");
    PGM_P fs_act_tpl = PSTR("act_tpl");
    PGM_P fs_temp_stat_t = PSTR("temp_stat_t");
    PGM_P fs_temp_stat_tpl = PSTR("temp_stat_tpl");
    PGM_P fs_curr_temp_t = PSTR("curr_temp_t");
    PGM_P fs_curr_temp_tpl = PSTR("curr_temp_tpl");
    PGM_P fs_temp_cmd_t = PSTR("temp_cmd_t");
    PGM_P fs_temp_cmd_tpl = PSTR("temp_cmd_tpl");

    PGM_P fs_expire_after = PSTR("expire_after");
    PGM_P fs_unit_of_meas = PSTR("unit_of_meas");
    PGM_P fs_dev_cla = PSTR("dev_cla");


    /* ArduinoJson crashes when using flash strings on both sides
        so here we pull them back to a String in RAM temporarily. */

    char _dev[4];
    char _name[5];
    char _uniq_id[8];
    char _stat_t[7]; 
    char _avty_t[7];
    char _pl_avail[9]; 
    char _pl_not_avail[13];
    char _val_tpl[8]; 
    char _cmd_t[6];
    char _cmd_tpl[8]; 
    char _conf_url[18];
    char _connections[12];
    char _identifiers[12];
    char _manufacturer[13];
    char _model[6];
    char _sw_v[11];
    char _alive[6];
    char _dead[5]; 
    char _mymin[4]; 
    char _mymax[4]; 
    char _icon[5]; 

    char _max_temp[9];
    char _min_temp[9];
    char _precision[10];
    char _temp_unit[10];
    char _modes[6];
    char _mode_cmd_t[11];
    char _mode_cmd_tpl[13];
    char _mode_stat_t[12];
    char _mode_stat_tpl[14];
    char _act_t[6];
    char _act_tpl[8];
    char _temp_stat_t[12];
    char _temp_stat_tpl[14];
    char _curr_temp_t[12];
    char _curr_temp_tpl[14];
    char _temp_cmd_t[11];
    char _temp_cmd_tpl[13];

    char _expire_after[13];
    char _unit_of_meas[13];
    char _dev_cla[8];


    strcpy_P(_dev, fs_dev);
    strcpy_P(_name, fs_name);
    strcpy_P(_uniq_id, fs_uniq_id);
    strcpy_P(_stat_t, fs_stat_t);
    strcpy_P(_avty_t, fs_avty_t);
    strcpy_P(_pl_avail, fs_pl_avail);
    strcpy_P(_pl_not_avail, fs_pl_not_avail);
    strcpy_P(_val_tpl, fs_val_tpl);
    strcpy_P(_cmd_t, fs_cmd_t);
    strcpy_P(_cmd_tpl, fs_cmd_tpl);
    strcpy_P(_conf_url, fs_conf_url);
    strcpy_P(_connections, fs_connections);
    strcpy_P(_identifiers, fs_identifiers);
    strcpy_P(_manufacturer, fs_manufacturer);
    strcpy_P(_model, fs_model);
    strcpy_P(_sw_v, fs_sw_v);
    strcpy_P(_alive, fs_alive);
    strcpy_P(_dead, fs_dead);
    strcpy_P(_mymin, fs_min);
    strcpy_P(_mymax, fs_max);
    strcpy_P(_icon, fs_icon);

    strcpy_P(_max_temp, fs_max_temp);
    strcpy_P(_min_temp, fs_min_temp);
    strcpy_P(_precision, fs_precision);
    strcpy_P(_temp_unit, fs_temp_unit);
    strcpy_P(_modes, fs_modes);
    strcpy_P(_mode_cmd_t, fs_mode_cmd_t);
    strcpy_P(_mode_cmd_tpl, fs_mode_cmd_tpl);
    strcpy_P(_mode_stat_t, fs_mode_stat_t);
    strcpy_P(_mode_stat_tpl, fs_mode_stat_tpl);
    strcpy_P(_act_t, fs_act_t);
    strcpy_P(_act_tpl, fs_act_tpl);
    strcpy_P(_temp_stat_t, fs_temp_stat_t);
    strcpy_P(_temp_stat_tpl, fs_temp_stat_tpl);
    strcpy_P(_curr_temp_t, fs_curr_temp_t);
    strcpy_P(_curr_temp_tpl, fs_curr_temp_tpl);
    strcpy_P(_temp_cmd_t, fs_temp_cmd_t);
    strcpy_P(_temp_cmd_tpl, fs_temp_cmd_tpl);

    strcpy_P(_expire_after, fs_expire_after);
    strcpy_P(_unit_of_meas, fs_unit_of_meas);
    strcpy_P(_dev_cla, fs_dev_cla);

    /* DEVICE */
    String topic;
    String payload;
    payload.reserve(8192);

    #if defined(ESP8266)
    String mychipid = String((unsigned int)ESP.getChipId());
    #elif defined(ESP32)
    String mychipid = String((uint32_t)ESP.getEfuseMac(), HEX);
    #endif

    const int maxtemp = 104;
    const int mintemp = 68;
    const int defaultExpire = 700;
    const String baseTopic = mqtt_info->mqttBaseTopic;
    const String baseName = mqtt_info->mqttBaseTopic;
    const String configUrl = String(F("http://")) + WiFi.localIP().toString();
    const String macAddress = WiFi.macAddress();
    const String topicStatus = baseTopic + F("/Status");
    const String topicMessage = baseTopic + F("/message");
    const String topicCommand = baseTopic + F("/command");
    const String topicCommandBatch = baseTopic + F("/command_batch");
    const String topicTimes = baseTopic + F("/times");
    const String topicOther = baseTopic + F("/other");
    const String topicGetConfig = baseTopic + F("/get_config");
    const String topicSetConfig = baseTopic + F("/set_config");
    const String topicButton = baseTopic + F("/button");
    const String topicRebootTime = baseTopic + F("/reboot_time");
    const String topicRebootReason = baseTopic + F("/reboot_reason");
    const String topicMqttConnectCount = baseTopic + F("/MQTT_Connect_Count");
    const String topicLogLevel = baseTopic + F("/log_level");

    auto makeUniqueId = [&](const char* suffix) {
        return baseTopic + F("_") + String(suffix) + mychipid;
    };

    auto groupObjectId = [&](const char* suffix) {
        return baseTopic + F("_") + String(suffix) + F("_") + mychipid;
    };

    auto initGroup = [&](DynamicJsonDocument& groupDoc, const String& deviceId, const String& deviceName, bool isRoot) {
        groupDoc.clear();
        groupDoc[_avty_t] = topicStatus;
        groupDoc[_pl_avail] = _alive;
        groupDoc[_pl_not_avail] = _dead;

        JsonObject dev = groupDoc.createNestedObject(_dev);
        dev["ids"].add(deviceId);
        dev["name"] = deviceName;
        dev["mf"] = F("Visualapproach");
        dev["mdl"] = bwc->getModel();
        dev["sw"] = FW_VERSION;
        dev["cu"] = configUrl;
        // dev["cns"].add(serialized("[\"mac\",\"" + macAddress + "\"]"));
        if (!isRoot)
        {
            dev["via_device"] = mychipid + F("_tech");
        }

        JsonObject origin = groupDoc.createNestedObject("o");
        origin["name"] = F("bwc");
        origin["sw"] = FW_VERSION;
    };

    auto createComponent = [&](JsonObject cmps, const char* id, const char* platform, const __FlashStringHelper* friendlyName, bool enabledByDefault = true) {
        JsonObject cmp = cmps.createNestedObject(id);
        cmp["p"] = platform;
        cmp[_uniq_id] = makeUniqueId(id);
        cmp[_name] = friendlyName;
        if (!enabledByDefault)
        {
            cmp["en"] = false;
        }
        return cmp;
    };

    auto publishGroup = [&](DynamicJsonDocument& groupDoc, const char* discoveryId) {
        payload.clear();
        topic = HA_PREFIX_F;
        topic += F("/device/");
        topic += groupObjectId(discoveryId);
        topic += F("/config");
        size_t __ha_len = serializeJson(groupDoc, payload);
        BWC_LOG_P(PSTR("MQTT > HA serialize len=%d Heap=%d\n"), (int)__ha_len, ESP.getFreeHeap());
        if (__ha_len == 0)
        {
            BWC_LOG_P(PSTR("MQTT > HA serialize failed (len=0)\n"), 0);
            return false;
        }
        mqttClient->publish(topic.c_str(), payload.c_str(), true);
        mqttClient->loop();
        return true;
    };

    DynamicJsonDocument groupDoc(16384);
    JsonObject cmps;
    JsonObject cmp;

    initGroup(groupDoc, mychipid + F("_tech"), baseName + F(" Tech"), true);
    cmps = groupDoc.createNestedObject("cmps");

    cmp = createComponent(cmps, "pressed_button", "sensor", F("Pressed button"));
    cmp[_stat_t] = topicButton;

    cmp = createComponent(cmps, "reboot_time", "sensor", F("Reboot time"));
    cmp[_stat_t] = topicRebootTime;
    cmp[_val_tpl] = F("{{as_timestamp(as_datetime(value).isoformat()) | timestamp_custom('%F %T')}}");

    cmp = createComponent(cmps, "reboot_reason", "sensor", F("Reboot reason"));
    cmp[_stat_t] = topicRebootReason;

    cmp = createComponent(cmps, "ssid", "sensor", F("SSID"));
    cmp[_stat_t] = topicOther;
    cmp[_val_tpl] = F("{{ value_json.SSID }}");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "rssi", "sensor", F("RSSI"));
    cmp[_stat_t] = topicOther;
    cmp[_val_tpl] = F("{{ value_json.RSSI }}");
    cmp[_unit_of_meas] = F("dBm");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "ip", "sensor", F("IP"));
    cmp[_stat_t] = topicOther;
    cmp[_val_tpl] = F("{{ value_json.IP }}");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "connect_count", "sensor", F("Connect count"));
    cmp[_stat_t] = topicMqttConnectCount;

    cmp = createComponent(cmps, "error", "sensor", F("Error"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{{ value_json.ERR }}");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "DMI", "sensor", F("Dsp msg int"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ value_json.DMI }}");
    cmp[_unit_of_meas] = F("ms");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:clock");

    cmp = createComponent(cmps, "uptime", "sensor", F("Uptime"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ ( (value_json.UPTIME|int)/3600/24) | round(2) }}");
    cmp[_unit_of_meas] = F("days");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:clock-outline");

    cmp = createComponent(cmps, "restart_esp", "button", F("Restart esp"));
    cmp[_cmd_t] = topicCommand;
    cmp["pl_prs"] = F("{CMD:6,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp[_dev_cla] = F("restart");
    cmp["ic"] = F("mdi:restart");

    cmp = createComponent(cmps, "connection", "binary_sensor", F("Connection"));
    cmp[_stat_t] = topicStatus;
    cmp[_dev_cla] = F("connectivity");
    cmp["pl_on"] = _alive;
    cmp["pl_off"] = _dead;

    // Unlike "connection" (MQTT broker reachability via LWT), this reflects
    // whether the ESP is actually hearing valid packets from the spa itself.
    cmp = createComponent(cmps, "spa_link", "binary_sensor", F("Spa link"));
    cmp[_stat_t] = topicOther;
    cmp[_val_tpl] = F("{% if value_json.SPALINK %}ON{% else %}OFF{% endif %}");
    cmp[_dev_cla] = F("connectivity");
    cmp["exp_aft"] = defaultExpire;

    // ON = verbose per-packet CIO/DSP logging on <base>/log (see bwcLog()/logSink()
    // in main.cpp) - noisy, meant to be toggled on only while actively debugging.
    cmp = createComponent(cmps, "mqtt_debug", "switch", F("MQTT debug logging"), false);
    cmp[_stat_t] = topicLogLevel;
    cmp[_cmd_t] = topicLogLevel;
    cmp["pl_on"] = F("debug");
    cmp["pl_off"] = F("info");
    cmp["stat_on"] = F("debug");
    cmp["stat_off"] = F("info");
    cmp["ic"] = F("mdi:bug-outline");

    cmp = createComponent(cmps, "unit", "switch", F("Temperature unit F-C"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.UNT }}");
    cmp["pl_on"] = F("{CMD:1,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:1,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:circle-outline");

    cmp = createComponent(cmps, "ctrl", "switch", F("Take control"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.GOD }}");
    cmp["pl_on"] = F("{CMD:17,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:17,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["opt"] = false;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:steering");

    if (!publishGroup(groupDoc, "tech"))
    {
        return;
    }

    initGroup(groupDoc, mychipid + F("_control"), baseName + F(" Commandes"), false);
    cmps = groupDoc.createNestedObject("cmps");

    cmp = createComponent(cmps, "powerswitch", "switch", F("Power switch"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.PWR }}");
    cmp["pl_on"] = F("{CMD:24,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:24,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:power");

    cmp = createComponent(cmps, "heat_regulation", "switch", F("Heat regulation"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{% if value_json.RED == 1 %}1{% elif value_json.GRN == 1 %}1{% else %}0{% endif %}");
    cmp["pl_on"] = F("{CMD:3,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:3,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:radiator");

    cmp = createComponent(cmps, "jets", "switch", F("Jets"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.HJT }}");
    cmp["pl_on"] = F("{CMD:11,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:11,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:hydro-power");

    cmp = createComponent(cmps, "airbubbles", "switch", F("Airbubbles"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.AIR }}");
    cmp["pl_on"] = F("{CMD:2,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:2,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:chart-bubble");

    cmp = createComponent(cmps, "pump", "switch", F("Pump"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.FLT }}");
    cmp["pl_on"] = F("{CMD:4,VALUE:true,XTIME:0,INTERVAL:0}");
    cmp["pl_off"] = F("{CMD:4,VALUE:false,XTIME:0,INTERVAL:0}");
    cmp["stat_on"] = 1;
    cmp["stat_off"] = 0;
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:pump");

    cmp = createComponent(cmps, "brightness", "number", F("Brightness"));
    cmp[_stat_t] = topicMessage;
    cmp[_cmd_t] = topicCommand;
    cmp[_val_tpl] = F("{{ value_json.BRT }}");
    cmp[_cmd_tpl] = F("{CMD:12,VALUE:{{ value | int }},XTIME:0,INTERVAL:0}");
    cmp[_mymin] = 0;
    cmp[_mymax] = 8;
    cmp["mode"] = F("slider");

    cmp = createComponent(cmps, "ready", "binary_sensor", F("Ready"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{% if value_json.TMP > 30 %}{% if value_json.TMP >= value_json.TGT-1 %}ON{% else %}OFF{% endif %}{% else %}OFF{% endif %}");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:hot-tub");

    cmp = createComponent(cmps, "heater", "binary_sensor", F("Heater"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{% if value_json.RED == 1 %}ON{% else %}OFF{% endif %}");
    cmp[_dev_cla] = F("heat");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "lock", "binary_sensor", F("Lock"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{% if value_json.LCK == 1 %}OFF{% else %}ON{% endif %}");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "climate", "climate", F("Temperature control"));
    cmp[_max_temp] = maxtemp;
    cmp[_min_temp] = mintemp;
    cmp[_precision] = 1.0;
    cmp[_temp_unit] = F("F");
    JsonArray modes = cmp.createNestedArray(_modes);
    modes.add(F("fan_only"));
    modes.add(F("off"));
    modes.add(F("heat"));
    cmp[_mode_cmd_t] = topicCommandBatch;
    cmp[_mode_cmd_tpl] = F("[{CMD:3,VALUE:{%if value == \"heat\" %}1{% else %}0{% endif %},XTIME:0,INTERVAL:0},{CMD:4,VALUE:{%if value == \"fan_only\" %}1{% elif value == \"heat\" %}1{% else %}0{% endif %},XTIME:0,INTERVAL:0}]");
    cmp[_mode_stat_t] = topicMessage;
    cmp[_mode_stat_tpl] = F("{% if value_json.RED == 1 %}heat{% elif value_json.GRN == 1 %}idle{% elif value_json.FLT == 1 %}fan_only{% else %}off{% endif %}");
    cmp[_act_t] = topicMessage;
    cmp[_act_tpl] = F("{% if value_json.RED == 1 %}heating{% elif value_json.GRN == 1 %}idle{% elif value_json.FLT == 1 %}fan{% else %}off{% endif %}");
    cmp[_temp_stat_t] = topicMessage;
    cmp[_temp_stat_tpl] = F("{{ value_json.TGTF }}");
    cmp[_curr_temp_t] = topicMessage;
    cmp[_curr_temp_tpl] = F("{{ value_json.TMPF }}");
    cmp[_temp_cmd_t] = topicCommand;
    cmp[_temp_cmd_tpl] = F("{CMD:0,VALUE:{{ value|int }},XTIME:0,INTERVAL:0}");

    cmp = createComponent(cmps, "audio", "switch", F("Audio"));
    cmp[_stat_t] = topicGetConfig;
    cmp[_cmd_t] = topicSetConfig;
    cmp[_val_tpl] = F("{{ value_json.AUDIO }}");
    cmp["pl_on"] = F("{AUDIO:true}");
    cmp["pl_off"] = F("{AUDIO:false}");
    cmp["stat_on"] = true;
    cmp["stat_off"] = false;

    cmp = createComponent(cmps, "restore", "switch", F("Restore on start"));
    cmp[_stat_t] = topicGetConfig;
    cmp[_cmd_t] = topicSetConfig;
    cmp[_val_tpl] = F("{{ value_json.RESTORE }}");
    cmp["pl_on"] = F("{RESTORE:true}");
    cmp["pl_off"] = F("{RESTORE:false}");
    cmp["stat_on"] = true;
    cmp["stat_off"] = false;

    if (!publishGroup(groupDoc, "control"))
    {
        return;
    }


    initGroup(groupDoc, mychipid + F("_energy"), baseName + F(" Energie"), false);
    cmps = groupDoc.createNestedObject("cmps");

    cmp = createComponent(cmps, "power", "sensor", F("Power"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ value_json.WATT | int }}");
    cmp[_unit_of_meas] = F("W");
    cmp[_dev_cla] = F("power");
    cmp["stat_cla"] = F("measurement");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:flash");

    cmp = createComponent(cmps, "energy", "sensor", F("index"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ value_json.KWH | round(3) }}");
    cmp[_unit_of_meas] = F("kWh");
    cmp[_dev_cla] = F("energy");
    cmp["stat_cla"] = F("total_increasing");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:flash");

    cmp = createComponent(cmps, "today", "sensor", F("Today"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ value_json.KWHD | round(3) }}");
    cmp[_unit_of_meas] = F("kWh");
    cmp[_dev_cla] = F("energy");
    cmp["stat_cla"] = F("total_increasing");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:flash");

    cmp = createComponent(cmps, "cost", "sensor", F("Cost"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ value_json.COST | round(3) }}");
    cmp[_dev_cla] = F("monetary");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:currency-usd");

    cmp = createComponent(cmps, "cost_today", "sensor", F("Cost today"));
    cmp[_stat_t] = topicTimes;
    cmp[_val_tpl] = F("{{ value_json.COSTD | round(3) }}");
    cmp[_dev_cla] = F("monetary");
    cmp["exp_aft"] = defaultExpire;
    cmp["ic"] = F("mdi:currency-usd");

    cmp = createComponent(cmps, "price", "number", F("Electricity price"));
    cmp[_stat_t] = topicGetConfig;
    cmp[_cmd_t] = topicSetConfig;
    cmp[_val_tpl] = F("{{ value_json.PRICE }}");
    cmp[_cmd_tpl] = F("{PRICE:{{ value | float }}}");
    cmp[_mymin] = -1000.0f;
    cmp[_mymax] = 65535.0f;
    cmp["step"] = 0.0001f;
    cmp["mode"] = F("box");
    cmp[_dev_cla] = F("monetary");

    if (!publishGroup(groupDoc, "energy"))
    {
        return;
    }

    initGroup(groupDoc, mychipid + F("_temperature"), baseName + F(" Temperature"), false);
    cmps = groupDoc.createNestedObject("cmps");

    cmp = createComponent(cmps, "temp_f", "sensor", F("Temp (F)"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{{ value_json.TMPF }}");
    cmp[_unit_of_meas] = "\u00b0F";
    cmp[_dev_cla] = F("temperature");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "temp_c", "sensor", F("Temp (C)"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{{ value_json.TMPC }}");
    cmp[_unit_of_meas] = "\u00b0C";
    cmp[_dev_cla] = F("temperature");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "target_temp_f", "sensor", F("Target temp (F)"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{{ value_json.TGTF }}");
    cmp[_unit_of_meas] = "\u00b0F";
    cmp[_dev_cla] = F("temperature");
    cmp["exp_aft"] = defaultExpire;

    cmp = createComponent(cmps, "target_temp_c", "sensor", F("Target temp (C)"));
    cmp[_stat_t] = topicMessage;
    cmp[_val_tpl] = F("{{ value_json.TGTC }}");
    cmp[_unit_of_meas] = "\u00b0C";
    cmp[_dev_cla] = F("temperature");
    cmp["exp_aft"] = defaultExpire;

    if (!publishGroup(groupDoc, "temperature"))
    {
        return;
    }


}

