#include <Arduino.h>
#include "AudioTools.h"
#include "AudioTools/Communication/A2DPStream.h"

// pin definitions
#define PIN_BCK         25
#define PIN_WS          27
#define PIN_DATA_OUT    26

// 44.1khz, 2-channel, 16-bit
AudioInfo info(44100, 2, 16);

A2DPStream a2dp;
I2SStream i2s;

// ACCOUNTING FOR MY GARBAGE SPEAKERS. THIS WOULD NOT
// BE NEEDED IF I DIDNT HAVE TRASH SPEAKERS. OH MY GOD.
const float HP_CUTOFF = 80.0;

FilteredStream<int16_t, float> hp_stream(i2s, 2);
Equalizer3Bands eq(hp_stream);
HighPassFilter<float> hp_left(HP_CUTOFF, 44100);
HighPassFilter<float> hp_right(HP_CUTOFF, 44100);

StreamCopy copier(eq, a2dp, 2048);  // a2dp -> eq -> i2s

// run as task rather than in main loop to prevent interrupts
void audioTask(void *param) {
  while (true) {
    copier.copy();
  }
}

void setup() {
    Serial.begin(115200);
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

    // config i2s
    auto i2s_cfg = i2s.defaultConfig(TX_MODE);
    i2s_cfg.copyFrom(info);
    i2s_cfg.pin_bck  = PIN_BCK;
    i2s_cfg.pin_ws   = PIN_WS;
    i2s_cfg.pin_data = PIN_DATA_OUT;
    i2s.begin(i2s_cfg);

    // config eq, lower bass so it doesn't sound awful :sob:
    auto eq_cfg = eq.defaultConfig();
    eq_cfg.copyFrom(info);
    eq_cfg.sample_rate     = 44100;
    eq_cfg.channels        = 2;
    eq_cfg.bits_per_sample = 16;
    eq_cfg.gain_low    = 1.0;
    eq_cfg.gain_medium = 1.0;
    eq_cfg.gain_high   = 1.0;

    // pass filters before eq starts
    hp_stream.setFilter(0, &hp_left);
    hp_stream.setFilter(1, &hp_right);
    hp_stream.begin(info);
    // start eq
    eq.begin(eq_cfg);

    // config bluetooth
    auto a2dp_cfg = a2dp.defaultConfig(RX_MODE);
    a2dp_cfg.name = "TS-PM0";
    a2dp.begin(a2dp_cfg);

    // start audio task on core 1 (highest priority)
    xTaskCreatePinnedToCore(audioTask, "audio", 4096, nullptr, 5, nullptr, 1);

    Serial.println("Bluetooth started");
}

void loop() {
    // nothing !11
}