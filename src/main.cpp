#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#include "driver/i2s.h"
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

// STATE CHANGE HANDLERS
void connection_state_changed(esp_a2d_connection_state_t state, void *ptr){
    Serial.println(a2dp_sink.to_str(state));
}

void audio_state_changed(esp_a2d_audio_state_t state, void *ptr){
  Serial.println(a2dp_sink.to_str(state));
}

// SETUP: define configs, start bluetooth service
void setup() {
    Serial.begin(115200);

    static i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 16,
        .dma_buf_len = 512,
        .use_apll = true,
        .tx_desc_auto_clear = true
    };

    static i2s_pin_config_t pin_config = {
        .bck_io_num = 25,
        .ws_io_num = 27,
        .data_out_num = 26,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    // INITIALIZE BLUETOOTH
    a2dp_sink.set_i2s_config(i2s_config);
    a2dp_sink.set_pin_config(pin_config);
    a2dp_sink.set_default_bt_mode(ESP_BT_MODE_CLASSIC_BT);
    a2dp_sink.set_on_connection_state_changed(connection_state_changed);
    a2dp_sink.set_on_audio_state_changed(audio_state_changed);
    a2dp_sink.start("TS-PM0");
    a2dp_sink.set_volume(107);
    Serial.println("Bluetooth started");
}

void loop() {
    // nothing needed here yet, init functions to be added
}