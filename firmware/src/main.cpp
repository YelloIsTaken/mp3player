// ESP32 MP3 Player firmware.
//
// Hardware: ESP32-WROOM-32E + PCM5102A I2S DAC + TPA6132A2 HP amp
//           + SSD1306 128x64 I2C OLED + microSD over SPI + 6 buttons.
//
// Plays MP3/WAV/FLAC/AAC from /music on the SD card. Persists last track
// and volume to LittleFS so power-cycles resume gracefully.

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LittleFS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>

// ---- Pin map -----------------------------------------------------------
constexpr int PIN_I2S_BCLK   = 26;
constexpr int PIN_I2S_LRCK   = 25;
constexpr int PIN_I2S_DOUT   = 22;

constexpr int PIN_I2C_SDA    = 21;
constexpr int PIN_I2C_SCL    = 4;

constexpr int PIN_SD_SCK     = 18;
constexpr int PIN_SD_MOSI    = 23;
constexpr int PIN_SD_MISO    = 19;
constexpr int PIN_SD_CS      = 5;

constexpr int PIN_BTN_PLAY   = 32;
constexpr int PIN_BTN_NEXT   = 33;
constexpr int PIN_BTN_PREV   = 35;  // input-only, ext pull-up on PCB
constexpr int PIN_BTN_VOLUP  = 14;
constexpr int PIN_BTN_VOLDN  = 39;  // input-only, ext pull-up on PCB
constexpr int PIN_BTN_MENU   = 36;  // input-only, ext pull-up on PCB

constexpr int PIN_HP_DETECT  = 13;
constexpr int PIN_BAT_ADC    = 34;
constexpr int PIN_LED_STATUS = 2;

// ---- Tunables ----------------------------------------------------------
constexpr uint8_t   VOL_MIN          = 0;
constexpr uint8_t   VOL_MAX          = 21;     // ESP32-audioI2S range
constexpr uint8_t   VOL_DEFAULT      = 12;
constexpr uint32_t  DEBOUNCE_MS      = 30;
constexpr uint32_t  LONGPRESS_MS     = 600;
constexpr uint32_t  UI_REFRESH_MS    = 200;
constexpr uint32_t  BAT_SAMPLE_MS    = 5000;
constexpr float     BAT_DIVIDER_RATIO = 2.0f;   // 100k/100k divider
constexpr float     BAT_VREF         = 3.3f;
constexpr float     BAT_FULL_V       = 4.15f;
constexpr float     BAT_EMPTY_V      = 3.30f;
constexpr const char* MUSIC_DIR      = "/music";
constexpr const char* STATE_FILE     = "/state.json";

// ---- Globals -----------------------------------------------------------
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Audio audio;

struct Button {
    int pin;
    bool last;
    uint32_t lastChange;
    uint32_t pressedAt;
    bool longFired;
};

Button btns[] = {
    {PIN_BTN_PLAY,  true, 0, 0, false},
    {PIN_BTN_NEXT,  true, 0, 0, false},
    {PIN_BTN_PREV,  true, 0, 0, false},
    {PIN_BTN_VOLUP, true, 0, 0, false},
    {PIN_BTN_VOLDN, true, 0, 0, false},
    {PIN_BTN_MENU,  true, 0, 0, false},
};
enum BtnIdx { B_PLAY, B_NEXT, B_PREV, B_VOLUP, B_VOLDN, B_MENU };

struct PlayerState {
    std::vector<String> playlist;
    int  index    = 0;
    uint8_t volume = VOL_DEFAULT;
    bool playing  = false;
    String title;
    uint32_t posSec = 0;
    uint32_t durSec = 0;
    float battV = 0.0f;
    bool hpInserted = false;
} st;

uint32_t lastUi = 0;
uint32_t lastBat = 0;

// ---- Helpers -----------------------------------------------------------
void scanMusic() {
    st.playlist.clear();
    File dir = SD.open(MUSIC_DIR);
    if (!dir || !dir.isDirectory()) {
        Serial.printf("No %s on SD card\n", MUSIC_DIR);
        return;
    }
    while (File f = dir.openNextFile()) {
        if (!f.isDirectory()) {
            String name = f.name();
            String lower = name;
            lower.toLowerCase();
            if (lower.endsWith(".mp3") || lower.endsWith(".wav") ||
                lower.endsWith(".flac") || lower.endsWith(".aac") ||
                lower.endsWith(".m4a")  || lower.endsWith(".ogg")) {
                st.playlist.push_back(String(MUSIC_DIR) + "/" + name);
            }
        }
        f.close();
    }
    dir.close();
    std::sort(st.playlist.begin(), st.playlist.end());
    Serial.printf("Found %d tracks\n", (int)st.playlist.size());
}

void saveState() {
    File f = LittleFS.open(STATE_FILE, "w");
    if (!f) return;
    f.printf("{\"index\":%d,\"volume\":%u}\n", st.index, st.volume);
    f.close();
}

void loadState() {
    if (!LittleFS.exists(STATE_FILE)) return;
    File f = LittleFS.open(STATE_FILE, "r");
    if (!f) return;
    String s = f.readString();
    f.close();
    int i = s.indexOf("\"index\":");
    int v = s.indexOf("\"volume\":");
    if (i >= 0) st.index  = s.substring(i + 8).toInt();
    if (v >= 0) st.volume = (uint8_t)s.substring(v + 9).toInt();
    if (st.volume > VOL_MAX) st.volume = VOL_DEFAULT;
}

void playCurrent() {
    if (st.playlist.empty()) return;
    if (st.index < 0) st.index = st.playlist.size() - 1;
    if (st.index >= (int)st.playlist.size()) st.index = 0;
    audio.stopSong();
    const char* path = st.playlist[st.index].c_str();
    Serial.printf("Play %s\n", path);
    audio.connecttoFS(SD, path);
    st.playing = true;
    st.title = st.playlist[st.index].substring(strlen(MUSIC_DIR) + 1);
    saveState();
}

void togglePlay() {
    if (st.playing) { audio.pauseResume(); st.playing = false; }
    else if (audio.isRunning()) { audio.pauseResume(); st.playing = true; }
    else playCurrent();
}

void next() { st.index++; playCurrent(); }
void prev() { st.index--; playCurrent(); }

void setVolume(int v) {
    v = constrain(v, (int)VOL_MIN, (int)VOL_MAX);
    st.volume = (uint8_t)v;
    audio.setVolume(st.volume);
    saveState();
}

float readBattery() {
    // 100k/100k divider; ADC is 12-bit, attenuation 11 dB → ~0..3.3 V
    uint32_t raw = 0;
    for (int i = 0; i < 16; ++i) raw += analogRead(PIN_BAT_ADC);
    raw /= 16;
    float v = (raw / 4095.0f) * BAT_VREF * BAT_DIVIDER_RATIO;
    return v;
}

uint8_t batteryPct(float v) {
    float p = (v - BAT_EMPTY_V) / (BAT_FULL_V - BAT_EMPTY_V);
    if (p < 0) p = 0;
    if (p > 1) p = 1;
    return (uint8_t)(p * 100.0f);
}

void drawUi() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Top bar: battery + headphone icon.
    uint8_t pct = batteryPct(st.battV);
    display.drawRect(108, 1, 18, 8, SSD1306_WHITE);
    display.fillRect(126, 3, 2, 4, SSD1306_WHITE);
    display.fillRect(110, 3, (uint16_t)(14 * pct / 100), 4, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.printf("%3u%%", pct);
    if (!st.hpInserted) {
        display.setCursor(36, 0);
        display.print("(spkr)");
    }

    // Title (scrolling-ish: just show truncated).
    display.setCursor(0, 16);
    String t = st.title;
    if (t.length() > 21) t = t.substring(0, 18) + "...";
    display.print(t);

    // Track index.
    display.setCursor(0, 28);
    display.printf("%d / %d", st.index + 1, (int)st.playlist.size());

    // Position.
    display.setCursor(0, 40);
    uint32_t cur = audio.getAudioCurrentTime();
    uint32_t tot = audio.getAudioFileDuration();
    display.printf("%02lu:%02lu / %02lu:%02lu",
                   cur / 60, cur % 60, tot / 60, tot % 60);

    // Volume bar.
    display.drawRect(0, 54, 100, 8, SSD1306_WHITE);
    display.fillRect(2, 56, (uint16_t)(96 * st.volume / VOL_MAX), 4, SSD1306_WHITE);
    display.setCursor(104, 54);
    display.printf("%2u", st.volume);

    // Play/pause indicator.
    if (st.playing) {
        display.fillTriangle(118, 26, 118, 40, 126, 33, SSD1306_WHITE);
    } else {
        display.fillRect(118, 26, 3, 14, SSD1306_WHITE);
        display.fillRect(123, 26, 3, 14, SSD1306_WHITE);
    }

    display.display();
}

void handleButton(int idx, bool pressed, bool longPress) {
    if (longPress) {
        switch (idx) {
            case B_MENU: /* reserved: enter settings */ break;
            case B_PLAY: /* reserved: power-off prompt */ break;
            default: break;
        }
        return;
    }
    if (!pressed) return;  // act on press, not release
    switch (idx) {
        case B_PLAY:  togglePlay(); break;
        case B_NEXT:  next(); break;
        case B_PREV:  prev(); break;
        case B_VOLUP: setVolume(st.volume + 1); break;
        case B_VOLDN: setVolume(st.volume - 1); break;
        case B_MENU:  /* short press: cycle UI screen */ break;
    }
}

void pollButtons() {
    uint32_t now = millis();
    for (int i = 0; i < (int)(sizeof(btns) / sizeof(btns[0])); ++i) {
        bool raw = digitalRead(btns[i].pin);   // active-low
        if (raw != btns[i].last && (now - btns[i].lastChange) > DEBOUNCE_MS) {
            btns[i].lastChange = now;
            btns[i].last = raw;
            if (!raw) {  // pressed
                btns[i].pressedAt = now;
                btns[i].longFired = false;
            } else {     // released
                if (!btns[i].longFired) handleButton(i, true, false);
            }
        }
        // long-press detection
        if (!btns[i].last && !btns[i].longFired &&
            (now - btns[i].pressedAt) > LONGPRESS_MS) {
            btns[i].longFired = true;
            handleButton(i, true, true);
        }
    }
}

// ---- ESP32-audioI2S callbacks -----------------------------------------
void audio_eof_mp3(const char *info)  { Serial.printf("eof: %s\n", info); next(); }
void audio_id3data(const char *info)  { st.title = String(info); }
void audio_showstation(const char *)  {}
void audio_showstreamtitle(const char*){}

// ---- Setup / loop ------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println("\nESP32 MP3 Player booting...");

    pinMode(PIN_LED_STATUS, OUTPUT);
    digitalWrite(PIN_LED_STATUS, HIGH);

    // Buttons. IO34/35/36/39 are input-only and have NO internal pull-ups —
    // PCB provides external 100k pull-ups on those nets.
    pinMode(PIN_BTN_PLAY,  INPUT_PULLUP);
    pinMode(PIN_BTN_NEXT,  INPUT_PULLUP);
    pinMode(PIN_BTN_VOLUP, INPUT_PULLUP);
    pinMode(PIN_BTN_PREV,  INPUT);
    pinMode(PIN_BTN_VOLDN, INPUT);
    pinMode(PIN_BTN_MENU,  INPUT);
    pinMode(PIN_HP_DETECT, INPUT_PULLUP);

    analogReadResolution(12);
    analogSetPinAttenuation(PIN_BAT_ADC, ADC_11db);

    // I2C / OLED.
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED init failed");
    } else {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(20, 24);
        display.println("MP3 Player");
        display.setCursor(28, 38);
        display.println("Booting...");
        display.display();
    }

    // SD card on HSPI.
    SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
    if (!SD.begin(PIN_SD_CS, SPI, 25000000)) {
        Serial.println("SD card mount failed");
    } else {
        Serial.printf("SD: %lluMB\n", SD.cardSize() / (1024ULL * 1024ULL));
    }

    // LittleFS for persistent state.
    if (!LittleFS.begin(true)) Serial.println("LittleFS mount failed");
    loadState();

    // I2S audio out.
    audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRCK, PIN_I2S_DOUT);
    audio.setVolume(st.volume);

    scanMusic();
    if (!st.playlist.empty()) {
        if (st.index >= (int)st.playlist.size()) st.index = 0;
        playCurrent();
    }
    digitalWrite(PIN_LED_STATUS, LOW);
}

void loop() {
    audio.loop();
    pollButtons();

    uint32_t now = millis();

    if (now - lastBat > BAT_SAMPLE_MS) {
        lastBat = now;
        st.battV = readBattery();
        st.hpInserted = (digitalRead(PIN_HP_DETECT) == LOW);
    }

    if (now - lastUi > UI_REFRESH_MS) {
        lastUi = now;
        drawUi();
    }
}
