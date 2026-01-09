/**
 * @file main.cpp
 * @brief Ponto de entrada principal do WavePwn/LeleWatch
 *
 * Inicializa todos os subsistemas e executa o loop principal.
 * Gerencia watchdog, boot animation, e estados do sistema.
 */

#include <Arduino.h>
#include <esp_task_wdt.h>

// Core
#include "../include/build_info.h"
#include "../include/config.h"
#include "core/config_manager.h"
#include "core/globals.h"


// Hardware
#include "hardware/audio_driver.h"
#include "hardware/system_hardware.h"

// UI
#include "ui/boot_animation.h"
#include "ui/burn_in_protection.h"
#include "ui/mascot_faces.h"
#include "ui/signal_aura.h"
#include "ui/status_bar.h"
#include "ui/ui_animated_wallpaper.h"
#include "ui/ui_language.h"
#include "ui/ui_radial_menu.h"
#include "ui/ui_transitions.h"

// System
#include "system/crash_handler.h"
#include "system/lan_turtle.h"
#include "system/panic_system.h"

// Voice
#include "voice/voice_assistant.h"
#include "voice/voice_recorder.h"

// Plugins
#include "plugins/age_tracker.h"
#include "plugins/battery_monitor.h"
#include "plugins/memtemp.h"
#include "plugins/plugin_manager.h"
#include "plugins/plugins.h"

// AI
#include "ai/neura9_inference.h"

// Main
#include "pwnagotchi/pwnagotchi.h"

// WiFi Attacks (included later for loop)
#include "wifi/wifi_attacks.h"

// IR Remote
#include "components/ir_remote/ir_blaster.h"

// Flag para animação de boot
static bool bootAnimComplete = false;
static bool mainUIStarted = false;

// Forward declaration
void lvgl_task(void *param);

/**
 * @brief Callback quando boot animation finaliza
 */
void onBootComplete() {
  bootAnimComplete = true;
  Serial.println("[MAIN] Boot animation complete, starting main UI");
}

/**
 * @brief Callback de comandos de voz
 * @param cmd Comando recebido
 */
void onVoiceCommand(VoiceCommand cmd) {
  Serial.printf("[MAIN] Comando de voz recebido: %d\n", cmd);
  // Comando já é executado pelo VoiceAssistant
}

/**
 * @brief Função de setup do Arduino
 * Inicializa todos os subsistemas na ordem correta.
 */
void setup() {
  // Inicia com CPU em baixa frequência para economizar energia no boot
  setCpuFrequencyMhz(80);

  // Inicializa Serial para debug
#if LOG_LEVEL > 0
  Serial.begin(115200);
  delay(500); // Delay para estabilizar Serial USB e evitar crash imediato

  Serial.println();
  Serial.println("╔═══════════════════════════════════════╗");
  Serial.println("║     WavePwn v" SYSTEM_VERSION " - Pwnagotchi Ed.     ║");
  Serial.println("╚═══════════════════════════════════════╝");
  Serial.printf("[INFO] Build: %s (%s)\n", BUILD_GIT_HASH_SHORT,
                BUILD_TIMESTAMP);
  Serial.printf("[INFO] Compiler: %s\n", BUILD_COMPILER);
  Serial.printf("[INFO] Free Heap: %d, PSRAM: %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
#endif

  // Inicializa crash handler (captura razão de reset)
  crashHandler.begin();

  // Carrega configuração
  config_manager.begin();

  // PASSO 1: Inicia o sistema (hardware, display, LVGL)
  pwn.begin();

  // Log de crash no SD se houve crash anterior
  extern void reportCrashToSD();
  reportCrashToSD();

  // Cria task LVGL no Core 0
  xTaskCreatePinnedToCore(lvgl_task,   // Function
                          "LVGL_Task", // Name
                          16384,       // Stack size
                          NULL,        // Param
                          1,           // Priority
                          NULL,        // Handle
                          0            // Core 0 (PRO_CPU)
  );

  // PASSO 2: Registra e inicializa plugins
  Serial.println("[MAIN] Registrando plugins...");
  registerDefaultPlugins();
  pluginManager.begin();

  // PASSO 3: Inicializa NEURA9 (IA de classificação)
  Serial.println("[MAIN] Inicializando NEURA9...");
  if (neura9.begin()) {
    Serial.println("[MAIN] ✓ NEURA9 carregada!");
  } else {
    Serial.println("[MAIN] ⚠ NEURA9 falhou");
  }

  // PASSO 4: Inicializa áudio
  Serial.println("[MAIN] Inicializando áudio...");
  if (audioDriver.begin()) {
    audioDriver.setVolume(50);
    audioDriver.playSound(SOUND_BOOT);
  } else {
    Serial.println("[MAIN] Áudio não disponível");
  }

  // PASSO 5: Inicializa assistente de voz
  Serial.println("[MAIN] Inicializando assistente de voz...");
  if (voiceAssistant.begin()) {
    voiceAssistant.onCommand(onVoiceCommand);
    voiceAssistant.setListening(g_state.voice_enabled);
    Serial.println("[MAIN] ✓ Assistente de voz pronto!");
  } else {
    Serial.println("[MAIN] ⚠ Assistente de voz falhou");
  }

  // PASSO 6: Inicializa componentes visuais
  lang.begin();             // Sistema de idiomas
  uiTransitions.begin();    // Transições de tela
  burnInProtection.begin(); // Proteção AMOLED
  radialMenu.begin();       // Menu radial
  panicSystem.begin();      // Panic System
  voiceRecorder.begin();    // Gravador de voz
  lanTurtle.begin();        // Lan Turtle

  // PASSO 7: IR Remote
  Serial.println("[MAIN] Inicializando IR Remote...");
  ir_init_all();

  // PASSO 8: Inicia animação de boot
  bootAnimation.onComplete(onBootComplete);
  bootAnimation.start();

  Serial.println("[MAIN] Diga 'Hey Dragon' para ativar voz");
  Serial.println("[MAIN] ═════════════════════════════════════");

  // Watchdog Timer (10s timeout)
  Serial.println("[MAIN] Inicializando Watchdog (10s)...");
  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);
}

/**
 * @brief Loop principal do Arduino
 * Atualiza todos os subsistemas a cada ciclo.
 */
void loop() {
  uint32_t now = millis();

  // Atualiza plugins
  pluginManager.update();

  // Atualiza mascote faces (auto-idle, blink, etc)
  mascotFaces.update();

  // Atualiza proteção burn-in
  burnInProtection.update();

  // Monitora botão de pânico
  panicSystem.update();

  // Atualiza gravador de voz (VAD/Buffer)
  voiceRecorder.update();

  // Lan Turtle logic
  lanTurtle.update();

  // Atualiza áudio
  audioDriver.update();

  // Atualiza assistente de voz
  voiceAssistant.update();

  // Se boot animation ainda rodando
  if (!bootAnimComplete) {
    if (bootAnimation.update()) {
      bootAnimComplete = true;
    }
  } else if (!mainUIStarted) {
    // Boost CPU para UI suave
    setCpuFrequencyMhz(240);

    mainUIStarted = true;
    Serial.println("[MAIN] Carregando UI principal...");

    extern void ui_main_show();
    ui_main_show();

    // Fala boas-vindas
    voiceAssistant.speak(TTS_HELLO);
  }

  // Atualiza status bar se visível
  if (statusBar.isVisible()) {
    statusBar.setChannel(0);
    statusBar.setUptime(agePlugin.getUptimeSeconds());
    statusBar.setBattery(batteryPlugin.getPercent(),
                         batteryPlugin.isCharging());
    statusBar.setMemory(memtempPlugin.getFreeHeap(),
                        memtempPlugin.getTemperature());
    statusBar.update();
  }

  // Check for deep sleep conditions every minute
  static uint32_t last_battery_check = 0;
  if (now - last_battery_check > 60000) {
    sys_hw.checkDeepSleep();
    last_battery_check = now;
  }

  // Idle Timer / Display Timeout
  static bool display_is_on = true;
  if (sys_hw.getTouch().touched) {
    sys_hw.resetIdleTimer();
    if (!display_is_on) {
      sys_hw.setDisplayPower(true);
      display_is_on = true;
      setCpuFrequencyMhz(240);
    }
  }

  if (display_is_on && sys_hw.isIdleTimeout()) {
    Serial.println("[PWR] Idle Timeout -> Display OFF");
    sys_hw.setDisplayPower(false);
    display_is_on = false;
    setCpuFrequencyMhz(80);
  }

  // Main loop do sistema
  pwn.loop();

  // Feed Watchdog
  esp_task_wdt_reset();

  delay(5);
}

/**
 * @brief Task LVGL no Core 0
 * Processa eventos LVGL em loop separado.
 * @param param Parâmetro não utilizado
 */
void lvgl_task(void *param) {
  Serial.println("[LVGL] Task started on Core 0");
  while (true) {
    // Throttling durante ataques pesados
    if (g_suspend_ble_lvgl) {
      vTaskDelay(pdMS_TO_TICKS(500));
    } else {
      lv_timer_handler();
      vTaskDelay(pdMS_TO_TICKS(5));
    }
  }
}
