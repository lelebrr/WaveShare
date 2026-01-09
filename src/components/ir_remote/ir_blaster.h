#pragma once
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include "ir_codes_db.h"

// Define GPIOs
#define IR_TX_PIN 17
#define IR_RX_PIN 16

class IRBlaster {
public:
    IRBlaster();
    void begin();
    void sendCode(const IRCode& code);
    void sendRaw(const uint16_t* data, uint16_t len, uint16_t freq = 38);
    void nuke(); // Send all power codes
    
private:
    IRsend* sender;
};

class IRReceiver {
public:
    IRReceiver();
    void begin();
    void loop(); // Call in main loop or task
    bool hasReceived();
    String getLastProtocol();
    uint64_t getLastData();
    void resume();

private:
    IRrecv* receiver;
    decode_results results;
};

extern IRBlaster irBlaster;
extern IRReceiver irReceiver;

// Legacy function stub for web_server compatibility
inline bool ir_save_learned_remote(const char* name) {
    // Check if receiver has captured a code
    if (irReceiver.hasReceived()) {
        // In a full implementation, save this to SD Card
        Serial.printf("[IR_LEARN] Captured code for '%s': 0x%llX (%s)\n", 
                      name, irReceiver.getLastData(), irReceiver.getLastProtocol().c_str());
        irReceiver.resume();
        return true;
    }
    return false; // No code captured
}
