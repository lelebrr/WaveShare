/**
 * @file ir_receiver.cpp
 * @brief Receptor IR para aprendizado de controles remotos
 *
 * Usa RMT do ESP32 para decodificar sinais IR NEC.
 * Salva controles aprendidos em JSON no SD card.
 */

#include "../../storage/sd_card_manager.h"
#include "ir_blaster.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <driver/rmt.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <freertos/ringbuf.h>
#include <stdio.h>
#include <string.h>
#include <string>


static const char *TAG = "IR_RECEIVER";
QueueHandle_t ir_learned_queue = nullptr;

/**
 * @brief Salva controle IR aprendido no SD card
 * @param name Nome do controle
 * @param addr Endereço IR (NEC)
 * @param cmd Comando IR (NEC)
 */
static void save_learned_control(const char *name, uint16_t addr, uint8_t cmd) {
  DynamicJsonDocument doc(512);
  doc["name"] = name;
  doc["address"] = addr;
  doc["command"] = cmd;
  doc["protocol"] = "NEC";

  std::string final_path = "/ir/my_remotes/" + std::string(name) + ".json";

  // Usa ArduinoJson com String para evitar problemas com FILE*
  String jsonString;
  serializeJson(doc, jsonString);

  FILE *f = fopen(final_path.c_str(), "w");
  if (f) {
    fwrite(jsonString.c_str(), 1, jsonString.length(), f);
    fclose(f);
    ESP_LOGI("IR_LEARN", "Controle salvo: %s", final_path.c_str());
  } else {
    ESP_LOGE("IR_LEARN", "Falha ao salvar: %s", final_path.c_str());
  }
}

/**
 * @brief Handler HTTP para aprendizado de controles IR
 * @param req Request HTTP
 * @return ESP_OK ou ESP_FAIL
 */
esp_err_t ir_learn_web_handler(httpd_req_t *req) {
  char buf[100];
  int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
  if (ret <= 0)
    return ESP_FAIL;
  buf[ret] = 0;

  uint64_t data = 0;
  if (xQueueReceive(ir_learned_queue, &data, 0) == pdTRUE) {
    uint16_t addr = data >> 32;
    uint8_t cmd = data & 0xFF;
    save_learned_control(buf, addr, cmd);

    char resp[200];
    snprintf(resp, sizeof(resp), "Controle \"%s\" aprendido e salvo!", buf);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  } else {
    httpd_resp_send(req, "Nenhum codigo na fila!", HTTPD_RESP_USE_STRLEN);
  }
  return ESP_OK;
}

/**
 * @brief Task de recepção IR
 * Monitora ringbuffer RMT e decodifica sinais NEC.
 */
static void ir_rx_task(void *arg) {
  RingbufHandle_t rb = nullptr;
  rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);

  rmt_rx_start(RMT_RX_CHANNEL, true);
  while (1) {
    size_t len = 0;
    rmt_item32_t *item =
        (rmt_item32_t *)xRingbufferReceive(rb, &len, portMAX_DELAY);
    if (item) {
      if (len > 60) {
        // Detectou NEC - extrai address/command
        uint32_t addr = 0, cmd = 0;
        for (int i = 1; i < 33; i++) {
          if (i < (int)(len / sizeof(rmt_item32_t))) {
            if (item[i].duration1 > 1000) {
              if (i < 17)
                addr |= (1 << (i - 1));
              else
                cmd |= (1 << (i - 17));
            }
          }
        }
        ESP_LOGI(TAG, "Aprendido: Addr=0x%04X Cmd=0x%02X", (unsigned int)addr,
                 (unsigned int)cmd);
        // Manda para fila - web salva no SD
        uint64_t data = ((uint64_t)addr << 32) | cmd;
        xQueueSend(ir_learned_queue, &data, 0);
      }
      vRingbufferReturnItem(rb, item);
    }
  }
}

/**
 * @brief Inicializa receptor IR
 * Configura RMT RX e cria task de recepção.
 */
void ir_init_receiver() {
  rmt_config_t rmt_rx;
  memset(&rmt_rx, 0, sizeof(rmt_rx));
  rmt_rx.channel = RMT_RX_CHANNEL;
  rmt_rx.gpio_num = (gpio_num_t)IR_RX_GPIO;
  rmt_rx.clk_div = 80;
  rmt_rx.mem_block_num = 2;
  rmt_rx.rmt_mode = RMT_MODE_RX;
  rmt_rx.rx_config.filter_en = true;
  rmt_rx.rx_config.filter_ticks_thresh = 100;
  rmt_rx.rx_config.idle_threshold = 10000;

  rmt_config(&rmt_rx);
  rmt_driver_install(RMT_RX_CHANNEL, 1000, 0);

  ir_learned_queue = xQueueCreate(10, sizeof(uint64_t));
  xTaskCreate(ir_rx_task, "ir_rx", 4096, nullptr, 5, nullptr);
}

// Wrapper para acesso externo
extern "C" esp_err_t ir_learn_web_handler_wrapper(httpd_req_t *req) {
  return ir_learn_web_handler(req);
}
