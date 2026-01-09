#include "ir_blaster.h"
#include <IRrecv.h>

IRReceiver irReceiver;

// Buffer size for capture
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 15;

IRReceiver::IRReceiver() {
    receiver = new IRrecv(IR_RX_PIN, kCaptureBufferSize, kTimeout, true);
}

void IRReceiver::begin() {
    receiver->enableIRIn();
}

void IRReceiver::loop() {
    // Polling handled by hasReceived
}

bool IRReceiver::hasReceived() {
    return receiver->decode(&results);
}

String IRReceiver::getLastProtocol() {
    return typeToString(results.decode_type);
}

uint64_t IRReceiver::getLastData() {
    return results.value;
}

void IRReceiver::resume() {
    receiver->resume();
}
