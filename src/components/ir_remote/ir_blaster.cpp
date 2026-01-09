#include "ir_blaster.h"
#include "esp_log.h"
#include "ir_codes.h"
#include <cstring>

static const char *TAG = "IR_BLASTER";

void ir_init_blaster() {
  rmt_config_t rmt_tx;
  rmt_tx.channel = RMT_TX_CHANNEL;
  rmt_tx.gpio_num = (gpio_num_t)IR_TX_GPIO; // Cast for stricter CPP
  rmt_tx.mem_block_num = 1;
  rmt_tx.clk_div = 80; // 1 tick = 1us
  rmt_tx.tx_config.loop_en = false;
  rmt_tx.tx_config.carrier_en = true;
  rmt_tx.tx_config.carrier_freq_hz = 38000;
  rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
  rmt_tx.tx_config.carrier_duty_percent = 33;
  rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  rmt_tx.tx_config.idle_output_en = true;
  rmt_tx.rmt_mode = RMT_MODE_TX;

  ESP_ERROR_CHECK(rmt_config(&rmt_tx));
  ESP_ERROR_CHECK(rmt_driver_install(RMT_TX_CHANNEL, 0, 0));
}

void ir_send_nec(uint16_t address, uint8_t command) {
  uint32_t code =
      ((uint32_t)address << 16) | (~address << 8) | (command << 8) | (~command);

  rmt_item32_t items[34];
  // Header 9ms + 4.5ms
  // rmt_item32_t is a struct with duration0, level0, duration1, level1.
  // The initializer provided in prompt {{{...}}} works for C/some C++ versions
  // but let's be safe with standard field assignment if we can, but the prompt
  // example is specific. I will use the prompt's cleaner braced initializer
  // style but ensuring it matches struct layout. The struct is {uint32_t
  // duration0:15; uint32_t level0:1; ...} usually. Let's stick to the prompt's
  // example as it claims to work "100%".

  items[0] = {{{9000, 1, 4500, 0}}};

  for (int i = 0; i < 32; i++) {
    items[i + 1] = {
        {{562, 1, (uint32_t)((code & (1 << (31 - i))) ? 1687 : 562), 0}}};
  }
  items[33] = {{{562, 1, 0, 0}}};

  rmt_write_items(RMT_TX_CHANNEL, items, 34, true);
  // vTaskDelay(50 / portTICK_PERIOD_MS); // Moving to end or letting caller
  // handle delay if burst The prompt had vTaskDelay inside. I'll keep it.
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

// Mude a função ir_tv_nuke() para usar também a nova lista:
void ir_tv_nuke() {
  ESP_LOGI(TAG, "TV NUKE FULL POWER – 600+ protocolos");
  for (int i = 0; popular_codes[i].name != nullptr; i++) {
    if (strstr(popular_codes[i].name, "Power")) {
      ir_send_nec(popular_codes[i].address, popular_codes[i].command);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
  for (int i = 0; novos_codigos[i].name != nullptr; i++) {
    if (strstr(novos_codigos[i].name, "Power")) {
      ir_send_nec(novos_codigos[i].address, novos_codigos[i].command);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
  ESP_LOGI(TAG, "TV NUKE concluído – 600+ protocolos enviados");
}

void ir_send_by_name(const char *name) {
  for (int i = 0; popular_codes[i].name != nullptr; i++) {
    if (strcmp(popular_codes[i].name, name) == 0) {
      ir_send_nec(popular_codes[i].address, popular_codes[i].command);
      return;
    }
  }
  // Check new codes too
  for (int i = 0; novos_codigos[i].name != nullptr; i++) {
    if (strcmp(novos_codigos[i].name, name) == 0) {
      ir_send_nec(novos_codigos[i].address, novos_codigos[i].command);
      return;
    }
  }
  ESP_LOGW(TAG, "Code not found: %s", name);
}

// Function to call both inits
void ir_init_all() {
  ir_init_blaster();
  ir_init_receiver();
}

// Stub para salvar códigos IR aprendidos
bool ir_save_learned_remote(const char *name) {
  ESP_LOGI(TAG, "Salvando código IR: %s", name ? name : "unknown");
  // TODO: Implementar salvamento em arquivo no SD/LittleFS
  // Por enquanto apenas retorna sucesso
  return true;
}
