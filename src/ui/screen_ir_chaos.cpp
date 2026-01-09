#include "../components/ir_remote/ir_blaster.h"
#include "ui_helpers.h"
#include "ui_home.h"
#include "lvgl.h"
#include <Arduino.h>
#include <cstdio>

static lv_obj_t *_screen = nullptr;
static bool _initialized = false;
static lv_timer_t* _rx_timer = nullptr;

static void btn_send_code_cb(lv_event_t *e) {
    IRCode* code = (IRCode*)lv_event_get_user_data(e);
    if (code) {
        Serial.printf("[IR] Sending %s\n", code->name);
        irBlaster.sendCode(*code);
        
        // Visual feedback
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x00ff00), 0);
        lv_timer_t* t = lv_timer_create([](lv_timer_t* t){
            lv_obj_t* btn = (lv_obj_t*)t->user_data;
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x12122a), 0);
            lv_timer_del(t);
        }, 200, btn);
    }
}

static void btn_nuke_cb(lv_event_t *e) {
    lv_msgbox_create(NULL, "TV NUKE", "Iniciando sequencia de desligamento...\n(Isso leva alguns segundos)", NULL, true);
    // Move to task to avoid blocking UI? For now, blocking 100ms * 50 = 5s might be too much.
    // Ideally use a task. But prompt asked for implementation.
    // Let's launch a simple timer based nuke or just blocking for now as proof of concept, 
    // or better, a task.
    xTaskCreate([](void* p){
        irBlaster.nuke();
        vTaskDelete(NULL);
    }, "ir_nuke", 4096, NULL, 5, NULL);
}

// RX Timer to check for codes
static void rx_update_cb(lv_timer_t* t) {
    if (irReceiver.hasReceived()) {
        uint64_t data = irReceiver.getLastData();
        String proto = irReceiver.getLastProtocol();
        
        // Popup with code
        char buf[64];
        snprintf(buf, 64, "%s: 0x%llX", proto.c_str(), data);
        lv_msgbox_create(NULL, "Sinal Recebido!", buf, NULL, true);
        
        irReceiver.resume();
    }
}

static void btn_cloner_cb(lv_event_t *e) {
    // Start RX
    irReceiver.begin();
    if (!_rx_timer) _rx_timer = lv_timer_create(rx_update_cb, 100, NULL);
    
    lv_msgbox_create(NULL, "Modo Clonador", "Aponte o controle e aperte um botao.", NULL, true);
}

static void create_list_item(lv_obj_t* parent, IRCode* code) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_set_height(btn, 70); // Increased from 50 to 70 for finger friendliness
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x12122a), 0);
    lv_obj_add_event_cb(btn, btn_send_code_cb, LV_EVENT_CLICKED, code);
    
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, code->name);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_20, 0); // Larger font
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xcccccc), 0);
    lv_obj_center(lbl);
}

void screen_ir_chaos() {
    if (_initialized) return;
    
    // Init HW
    irBlaster.begin();
    // Receiver init only on cloner mode to save resources? Or now.
    
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x000000), 0);
    
    lv_obj_t* cont = ui_create_scrollable_menu_container(_screen, "IR CHAOS");
    
    ui_create_back_btn(_screen, [](lv_event_t* e){
        if (_rx_timer) { lv_timer_del(_rx_timer); _rx_timer = nullptr; }
        ui_home_show();
        // Close screen logic
        lv_obj_del(_screen);
        _initialized = false;
        _screen = nullptr;
    });

    // Special Buttons
    lv_obj_t *btn_nuke = lv_btn_create(cont);
    lv_obj_set_width(btn_nuke, LV_PCT(100));
    lv_obj_set_height(btn_nuke, 60);
    lv_obj_set_style_bg_color(btn_nuke, lv_color_hex(0xff0000), 0);
    lv_obj_add_event_cb(btn_nuke, btn_nuke_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_nuke = lv_label_create(btn_nuke);
    lv_label_set_text(lbl_nuke, "☢ TV NUKE ☢");
    lv_obj_center(lbl_nuke);
    
    lv_obj_t *btn_clone = lv_btn_create(cont);
    lv_obj_set_width(btn_clone, LV_PCT(100));
    lv_obj_set_height(btn_clone, 60);
    lv_obj_set_style_bg_color(btn_clone, lv_color_hex(0x0000ff), 0);
    lv_obj_add_event_cb(btn_clone, btn_cloner_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t* lbl_clone = lv_label_create(btn_clone);
    lv_label_set_text(lbl_clone, "📡 CLONADOR 📡");
    lv_obj_center(lbl_clone);
    
    // DB List
    for (int i = 0; ir_code_database[i].name != nullptr; i++) {
        // Cast const away safety or change helper? 
        // ir_code_database is const, but callback userdata is void*.
        create_list_item(cont, (IRCode*)&ir_code_database[i]);
    }

    _initialized = true;
    lv_scr_load(_screen);
}

