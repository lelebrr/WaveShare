#include "ir_blaster.h"

IRBlaster irBlaster;

IRBlaster::IRBlaster() {
    sender = new IRsend(IR_TX_PIN);
}

void IRBlaster::begin() {
    sender->begin();
}

void IRBlaster::sendCode(const IRCode& code) {
    switch (code.protocol) {
        case PROTO_NEC: sender->sendNEC(code.data, code.bits); break;
        case PROTO_SONY: sender->sendSony(code.data, code.bits); break;
        case PROTO_RC5: sender->sendRC5(code.data, code.bits); break;
        case PROTO_RC6: sender->sendRC6(code.data, code.bits); break;
        case PROTO_SAMSUNG: sender->sendSamsung(code.data, code.bits); break;
        case PROTO_LG: sender->sendLG(code.data, code.bits); break;
        case PROTO_PANASONIC: sender->sendPanasonic(code.address, code.data); break;
        default: sender->sendNEC(code.data, code.bits); break; // Default
    }
}

void IRBlaster::sendRaw(const uint16_t* data, uint16_t len, uint16_t freq) {
    sender->sendRaw(data, len, freq);
}

void IRBlaster::nuke() {
    for (int i = 0; ir_code_database[i].name != nullptr; i++) {
        // Simple check: if name contains "Power", send it
        if (strstr(ir_code_database[i].name, "Power")) {
            sendCode(ir_code_database[i]);
            delay(100); // Small delay between codes
        }
    }
}
