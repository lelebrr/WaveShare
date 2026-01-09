#pragma once
#include "driver/rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define IR_TX_GPIO 17
#define IR_RX_GPIO 16
#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_RX_CHANNEL RMT_CHANNEL_1

extern QueueHandle_t ir_learned_queue;

void ir_init_all();
void ir_init_blaster();  // Added for internal use if needed, but ir_init_all
                         // calls it
void ir_init_receiver(); // Added for internal use
bool ir_save_learned_remote(const char *name);
void ir_send_nec(uint16_t address, uint8_t command);
void ir_send_raw(const uint32_t *data, size_t len);
void ir_tv_nuke();                      // 40 protocolos em 4 segundos
void ir_send_by_name(const char *name); // usa ir_codes.h
