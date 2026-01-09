#include "esp_now_mesh.h"
#include "../core/globals.h"
#include <WiFi.h>
#include <esp_wifi.h>

EspNowMesh MeshSystem;

static EspNowMesh *_meshInstance = nullptr;

EspNowMesh::EspNowMesh() { _meshInstance = this; }

EspNowMesh::~EspNowMesh() { end(); }

bool EspNowMesh::begin() {
  if (esp_now_init() != ESP_OK) {
    return false;
  }

  esp_now_register_recv_cb(EspNowMesh::onDataRecv);
  esp_now_register_send_cb(EspNowMesh::onDataSent);

  // Register broadcast peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, _broadcastMac, 6);
  peerInfo.channel = 0; // 0 = current channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    // Might already exist
  }

  return true;
}

void EspNowMesh::end() { esp_now_deinit(); }

void EspNowMesh::update() {
  // Handle timeouts or retransmissions if needed
}

void EspNowMesh::onDataRecv(const uint8_t *mac, const uint8_t *incomingData,
                            int len) {
  if (_meshInstance) {
    _meshInstance->processPacket(mac, incomingData, len);
  }
}

void EspNowMesh::onDataSent(const uint8_t *mac_addr,
                            esp_now_send_status_t status) {
  // Handle sent status
}

void EspNowMesh::processPacket(const uint8_t *mac, const uint8_t *data,
                               size_t len) {
  if (len < sizeof(MeshHeader))
    return;

  const MeshHeader *header = (const MeshHeader *)data;
  if (header->magic != MESH_MAGIC || header->version != MESH_VERSION)
    return;

  const uint8_t *payload = data + sizeof(MeshHeader);
  size_t payloadSize = len - sizeof(MeshHeader);

  switch (header->type) {
  case MESH_TYPE_AUDIO:
    if (_audioCb) {
      // For now pass raw payload, later handle codec
      _audioCb(mac, payload, payloadSize);
    }
    break;
  case MESH_TYPE_CHAT:
    if (_chatCb && payloadSize >= sizeof(MeshChatPayload)) {
      const MeshChatPayload *chat = (const MeshChatPayload *)payload;
      // Verify null termination safety or copy
      char safeText[201];
      strncpy(safeText, chat->text, 200);
      safeText[200] = 0;

      char safeSender[17];
      strncpy(safeSender, chat->sender, 16);
      safeSender[16] = 0;

      _chatCb(mac, safeSender, safeText);
    }
    break;
  case MESH_TYPE_PING:
    if (_pingCb && payloadSize >= sizeof(MeshPingPayload)) {
      const MeshPingPayload *ping = (const MeshPingPayload *)payload;
      char safeName[17];
      strncpy(safeName, ping->deviceName, 16);
      safeName[16] = 0;

      // ESP-NOW doesn't give RSSI in callback natively in all SDK versions,
      // but we can try to get it from wifi promiscuous if we were capturing,
      // or just ignore for now and assume proximity.
      // Actually there is a trick to get RSSI but for now let's pass 0.
      _pingCb(mac, 0, safeName);
    }
    break;
  default:
    break;
  }
}

bool EspNowMesh::broadcast(const void *data, size_t len) {
  if (len > ESP_NOW_MAX_DATA_LEN)
    return false;
  return esp_now_send(_broadcastMac, (const uint8_t *)data, len) == ESP_OK;
}

bool EspNowMesh::send(const uint8_t *mac, const void *data, size_t len) {
  if (len > ESP_NOW_MAX_DATA_LEN)
    return false;

  // Check if peer exists, if not add it
  if (!esp_now_is_peer_exist(mac)) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
  }

  return esp_now_send(mac, (const uint8_t *)data, len) == ESP_OK;
}

bool EspNowMesh::sendAudio(const uint8_t *audioData, size_t len) {
  if (len > 230)
    return false; // Safety margin

  uint8_t buffer[250];
  MeshHeader *header = (MeshHeader *)buffer;
  header->magic = MESH_MAGIC;
  header->version = MESH_VERSION;
  header->type = MESH_TYPE_AUDIO;
  header->payloadLen = len;
  header->timestamp = millis();

  // TODO: Actually fill a MeshAudioPayload struct
  // For now direct copy for speed test
  memcpy(buffer + sizeof(MeshHeader), audioData, len);

  return broadcast(buffer, sizeof(MeshHeader) + len);
}

bool EspNowMesh::sendChat(const char *message, bool global) {
  uint8_t buffer[250];
  MeshHeader *header = (MeshHeader *)buffer;
  header->magic = MESH_MAGIC;
  header->version = MESH_VERSION;
  header->type = MESH_TYPE_CHAT;
  header->timestamp = millis();

  MeshChatPayload *payload = (MeshChatPayload *)(buffer + sizeof(MeshHeader));
  payload->isGroup = global;
  // Use configured dragon name from g_state
  extern GlobalState g_state;
  strncpy(payload->sender, g_state.dragon_name[0] ? g_state.dragon_name : "Dragon", 16);
  strncpy(payload->text, message, 200);

  size_t packetLen = sizeof(MeshHeader) + sizeof(MeshChatPayload);
  header->payloadLen = sizeof(MeshChatPayload);

  return broadcast(buffer, packetLen);
}

bool EspNowMesh::sendPing() {
  uint8_t buffer[250];
  MeshHeader *header = (MeshHeader *)buffer;
  header->magic = MESH_MAGIC;
  header->version = MESH_VERSION;
  header->type = MESH_TYPE_PING;
  header->timestamp = millis();

  MeshPingPayload *payload = (MeshPingPayload *)(buffer + sizeof(MeshHeader));
  // Use dragon name from config
  extern GlobalState g_state;
  strncpy(payload->deviceName, g_state.dragon_name[0] ? g_state.dragon_name : "LeleWatch", 16);
  
  // Get level from exp_system if available (else default 1)
  extern uint8_t getDragonLevel();
  payload->dragonLevel = getDragonLevel();
  payload->uptime = millis();

  size_t packetLen = sizeof(MeshHeader) + sizeof(MeshPingPayload);
  header->payloadLen = sizeof(MeshPingPayload);

  return broadcast(buffer, packetLen);
}

void EspNowMesh::setWalkieTalkieMode(bool enabled) {
  _walkieTalkieMode = enabled;
}
