#include "ui_launcher.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_home.h"
#include "ui_menu_wifi.h"
#include "ui_menu_ble.h"
#include "ui_menu_ai.h"
#include "ui_menu_tools.h"
#include "ui_menu_voice.h"
// #include "screen_ir_chaos.h" // Need proper header or forward decl

// Forward declaration for IR screen (assuming function exists)
extern void screen_ir_chaos(); 

static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

static void btn_app_cb(lv_event_t *e) {
    UIScreen target = (UIScreen)(intptr_t)lv_event_get_user_data(e);
    // Map custom screens that are not in enum or handle directly
    if (target == UI_SCREEN_WIFI_CHAOS) {
        ui_menu_wifi_show();
    } else if (target == UI_SCREEN_BLE_CHAOS) {
        ui_menu_ble_show();
    } else if (target == UI_SCREEN_PLUGINS) { // Mapping AI/Tools/etc to plugins or generic enums?
        // Check user data pointer
        const char* name = (const char*)lv_event_get_user_data(e);
        // Better: use specific callbacks or int IDs
    }
}

// Specific callbacks for simplicity
static void open_wifi(lv_event_t* e) { ui_menu_wifi_show(); }
static void open_ble(lv_event_t* e) { ui_menu_ble_show(); }
static void open_ir(lv_event_t* e) { screen_ir_chaos(); }
static void open_ai(lv_event_t* e) { ui_menu_ai_show(); }
static void open_tools(lv_event_t* e) { ui_menu_tools_show(); }
static void open_voice(lv_event_t* e) { ui_menu_voice_show(); }
static void open_settings(lv_event_t* e) { ui_settings_show(); }

static void create_app_icon(lv_obj_t* parent, const char* symbol, const char* label, lv_event_cb_t cb, uint32_t color) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 100, 100); // 2 columns in 240px width
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);
    lv_obj_set_style_radius(btn, 16, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t* icon = lv_label_create(btn);
    lv_label_set_text(icon, symbol);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0); // Large Icon
    lv_obj_set_style_text_color(icon, lv_color_hex(color), 0);
    
    lv_obj_t* txt = lv_label_create(btn);
    lv_label_set_text(txt, label);
    lv_obj_set_style_text_font(txt, &lv_font_montserrat_14, 0);
}

void ui_launcher_init() {
    if (_initialized) return;
    
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);
    
    // Container for grid
    lv_obj_t* cont = ui_create_scrollable_menu_container(_screen, "MENU");
    // Override flex to ROW wrap for grid effect?
    // ui_create_scrollable_menu_container sets COLUMN. 
    // We want a grid. Let's create a grid container INSIDE the scrollable one?
    // Or just change flow.
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_column(cont, 10, 0);
    lv_obj_set_style_pad_row(cont, 10, 0);
    
    // Apps
    create_app_icon(cont, LV_SYMBOL_WIFI, "WiFi", open_wifi, 0x00ff88);
    create_app_icon(cont, LV_SYMBOL_BLUETOOTH, "BLE", open_ble, 0x0088ff);
    create_app_icon(cont, "☢", "IR", open_ir, 0xff0000); // Need symbol support or ensure font has it
    create_app_icon(cont, "🧠", "AI", open_ai, 0xff00ff);
    create_app_icon(cont, "🛠", "Tools", open_tools, 0xffaa00);
    create_app_icon(cont, "🎤", "Voice", open_voice, 0x00ffff);
    create_app_icon(cont, LV_SYMBOL_SETTINGS, "Setup", open_settings, 0xaaaaaa);
    
    // Eco Mode
    create_app_icon(cont, LV_SYMBOL_BATTERY_FULL, "Eco", [](lv_event_t* e){
        extern void ui_eco_clock_show();
        ui_eco_clock_show();
    }, 0x00ff00);

    // Back button override? No, scrollable container has a header usually?
    // The helper "ui_create_scrollable_menu_container" creates a header with title.
    // It doesn't create a back button automatically.
    // We should add a "Home" button or Back.
    ui_create_back_btn(_screen, [](lv_event_t* e){
        ui_home_show();
        // Hide/Del launcher?
        // ui_launcher_hide(); 
    });

    _initialized = true;
}

void ui_launcher_show() {
    if (!_initialized) ui_launcher_init();
    lv_scr_load(_screen);
}

void ui_launcher_hide() {
    if (_screen) {
        lv_obj_del(_screen);
        _screen = nullptr;
        _initialized = false;
    }
}
