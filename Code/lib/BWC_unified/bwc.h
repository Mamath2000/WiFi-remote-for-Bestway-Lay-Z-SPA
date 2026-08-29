#pragma once

#if defined (ESP8266)
#elif defined (ESP32)
#else
#error "This library supports 8266/32 only"
#endif

#include "Arduino.h"
//long long needed in arduino core v3+
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
// #include "ESPDateTime.h"
#include <LittleFS.h>
#include <Ticker.h>
#include <vector>
#include <optional>
#include "enums.h"
#include "CIO_4W_MODEL_SPECIFIC.h"
#include "CIO_TYPE1_MODEL_SPECIFIC.h"
#include "CIO_TYPE2_MODEL_SPECIFIC.h"

#include "DSP_4W_MODEL_SPECIFIC.h"
#include "DSP_TYPE1_MODEL_SPECIFIC.h"
#include "DSP_TYPE2_MODEL_SPECIFIC.h"

#include "FW_VERSION.h"
#include "bwc_debug.h"

constexpr int MAXCOMMANDS = 20;

struct command_que_item
{
    int64_t val;
    uint64_t xtime;
    Commands cmd;
    uint32_t interval;
    String text = "";
};
        

class BWC {

    public:
        BWC();
        ~BWC();
        void setup(void);
        void begin();
        void on_save_settings();
        void on_scroll_text();
        void loop();
        void adjust_brightness();
        void play_sound();
        // String get_fromcio();
        // String get_todsp();
        // String get_fromdsp();
        // String get_tocio();
        void stop(void);
        void pause_all(bool action);
        bool add_command(command_que_item command_item);
        bool edit_command(uint8_t index, command_que_item command_item);
        bool del_command(uint8_t index);
        // bool qCommand(int64_t cmd, int64_t val, int64_t xtime, int64_t interval);
        bool newData();
        bool spaLinkEverOk();
        bool spaLinkHealthy(uint32_t staleAfterMs = 15000UL);
        void getJSONStates(String &rtn);
        void getJSONTimes(String &rtn);
        void getJSONSettings(String &rtn);
        void setJSONSettings(const String& message);
        String getJSONCommandQueue();
        uint8_t getState(int state);
        // void saveSettingsFlag();
        void saveSettings();
        void reloadCommandQueue();
        void reloadSettings();
        void getButtonName(String &rtn);
        Buttons getButton();
        void saveDebugInfo(const String& s);
        void saveRebootInfo();
        bool getBtnSeqMatch();
        String getModel();
        void print(const String& txt);
        void loadCommandQueue();
        void restoreStates();

        // String getDebugData();

    public:
        time_t reboot_time_t;
        String reboot_time_str;
        String reset_reason_str;
        int pins[8];
        unsigned int loop_count = 0;
        CIO* cio = nullptr;
        DSP* dsp = nullptr;
        bool hasjets, hasgod;
        bool BWC_DEBUG = false;

    private:
        bool _loadHardware(Models& cioNo, Models& dspNo, int pins[], std::optional<Power>& power_levels);
        bool _handlecommand(Commands cmd, int64_t val, const String &txt);
        void _format_text(const String &txt);
        void _handleCommandQ();
        void _loadSettings();
        void _saveCommandQueue();
        void _updateTimes();
        void _saveStates();
        void _handleStateChanges();
        static bool _compare_command(const command_que_item& i1, const command_que_item& i2);
        bool _load_melody_json(const String &filename);
        void _add_melody(const String &filename);
        void _save_melody(const String& filename);
        void _sweepdown();
        void _sweepup();
        void _beep();
        void _accord();
        void _log();
        void _updateSpaLinkHealth();

    private:
        struct sLinkHealth
        {
            uint32_t prevGoodPackets = 0;
            unsigned long lastChangeMs = 0;
            bool everOk = false;
        };
        sLinkHealth _cioLink, _dspLink;

    private:
        uint64_t _timestamp_secs; // seconds
        double _energy_daily_Ws; //Wattseconds internally
        double _energy_total_Ws; //Wattseconds internally
        double _energy_cost_total;
        double _energy_cost_daily;
        Ticker _save_settings_ticker;
        Ticker _scroll_text_ticker;
        std::vector<command_que_item> _command_que;
        std::vector<sNote> _notes;
        sStates _prev_cio_states, _prev_dsp_states;
        uint32_t _uptime;
        uint32_t _pumptime;
        uint32_t _heatingtime;
        uint32_t _airtime;
        uint32_t _jettime;
        uint32_t _uptime_ms;
        uint32_t _pumptime_ms;
        uint32_t _heatingtime_ms;
        uint32_t _airtime_ms;
        uint32_t _jettime_ms;
        uint32_t _pumptime_daily_ms;
        uint32_t _heatingtime_daily_ms;
        uint32_t _airtime_daily_ms;
        uint32_t _jettime_daily_ms;
        uint32_t _uptime_daily_ms;
        uint32_t _last_reset_day;
        int _note_duration;
        int _energy_power_W;
        int _ticker_count;
        int _btn_sequence[4] = {NOBTN,NOBTN,NOBTN,NOBTN}; //keep track of the four latest button presses
        double _price;
        Buttons _prevbutton = NOBTN;
        int16_t _override_dsp_brt_timer;
        uint8_t _dsp_brightness;
        uint8_t _web_target = 20; 
        bool _scroll = false;
        bool _audio_enabled;
        bool _restore_states_on_start = false;
        bool _save_settings_needed = false;
        bool _save_cmdq_needed = false;
        bool _save_states_needed = false;
        bool _new_data_available = false;
        bool _dsp_tgt_used = true;
        bool _states_are_restored = false;
};

void save_settings_cb(BWC *bwcInstance);

void scroll_text_cb(BWC *bwcInstance);
