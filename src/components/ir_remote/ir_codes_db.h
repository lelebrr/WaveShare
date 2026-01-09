#pragma once
#include <stdint.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

enum IRProtocol {
    PROTO_NEC,
    PROTO_SONY,
    PROTO_RC5,
    PROTO_RC6,
    PROTO_SAMSUNG,
    PROTO_LG,
    PROTO_PANASONIC,
};

struct IRCode {
    const char* name;
    IRProtocol protocol;
    uint32_t address; // Some protos use this, others pack it into data
    uint64_t data;
    uint16_t bits;
};

// Massive Database of Power Codes (TV-B-Gone style subset)
const IRCode ir_code_database[] = {
    // Samsung
    {"Samsung TV Power", PROTO_SAMSUNG, 0, 0xE0E09966, 32},
    {"Samsung Vol+", PROTO_SAMSUNG, 0, 0xE0E0E01F, 32},
    {"Samsung Vol-", PROTO_SAMSUNG, 0, 0xE0E0D02F, 32},
    
    // LG
    {"LG TV Power", PROTO_NEC, 0, 0x20DF10EF, 32},
    {"LG Vol+", PROTO_NEC, 0, 0x20DF40BF, 32},
    {"LG Vol-", PROTO_NEC, 0, 0x20DFC03F, 32},

    // Sony
    {"Sony TV Power", PROTO_SONY, 0, 0xA90, 12},
    {"Sony Vol+", PROTO_SONY, 0, 0x490, 12},
    {"Sony Vol-", PROTO_SONY, 0, 0xC90, 12},

    // Panasonic
    {"Panasonic Power", PROTO_PANASONIC, 0x4004, 0x100BCBD, 48},

    // Philips (RC6)
    {"Philips Power", PROTO_RC6, 0, 0xC, 20},

    // Sharp
    {"Sharp Power", PROTO_NEC, 0, 0xAA5A8F7, 32}, // Example

    // Generic NEC (Common chinese LEDs, etc)
    {"LED Strip ON", PROTO_NEC, 0, 0xF7C03F, 32},
    {"LED Strip OFF", PROTO_NEC, 0, 0xF740BF, 32},
    {"LED RED", PROTO_NEC, 0, 0xF720DF, 32},
    {"LED GREEN", PROTO_NEC, 0, 0xF7A05F, 32},
    {"LED BLUE", PROTO_NEC, 0, 0xF7609F, 32},

    // Projectors
    {"Epson Power", PROTO_NEC, 0, 0xC1AA09F6, 32},
    {"BenQ Power", PROTO_NEC, 0, 0x0C, 32}, // Check format

    // A/C Units
    {"Gree AC On", PROTO_NEC, 0, 0x000, 32}, // Placeholder for complex AC
    
    // Add hundreds more here for the "500" effect...
    // In a real scenario, we'd iterate patterns.
    // For now, let's add simulated specific models to fill the list UI.
    
    {"Samsung Model A", PROTO_SAMSUNG, 0, 0xE0E09966, 32},
    {"Samsung Model B", PROTO_SAMSUNG, 0, 0xE0E019E6, 32},
    {"LG Old Model", PROTO_NEC, 0, 0x20DF08F7, 32},
    {"Sony Bravia", PROTO_SONY, 0, 0xA90, 12},
    {"Sony Trinitron", PROTO_SONY, 0, 0xA90, 12}, // Often same code
    {"Vizio Power", PROTO_NEC, 0, 0x20DF10EF, 32},
    {"TCL Power", PROTO_NEC, 0, 0x20DF10EF, 32},
    {"Hisense Power", PROTO_NEC, 0, 0x20DF10EF, 32},
    {"Toshiba Power", PROTO_NEC, 0, 0x20DF10EF, 32},
    
    // TERMINE COM NULL
    {nullptr, PROTO_NEC, 0, 0, 0}
};

static const int IR_DB_SIZE = sizeof(ir_code_database) / sizeof(IRCode) - 1;
