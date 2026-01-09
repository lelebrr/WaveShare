#pragma once

#include <Arduino.h>
#include <esp_now.h>
#include <functional>
#include <vector>

// ESP-NOW Mesh Protocol Definitions
#define MESH_MAGIC 0xFE7E // "LELE" stylized as valid hex
#define MESH_VERSION 1

enum MeshMessageType : uint8_t {
  MESH_TYPE_PING = 0,
  MESH_TYPE_AUDIO = 1,
  MESH_TYPE_CHAT = 2,
  MESH_TYPE_HANDSHAKE_OFFER = 3,
  MESH_TYPE_HANDSHAKE_DATA = 4,
  MESH_TYPE_GAME_SYNC = 5,
  MESH_TYPE_CONTROL = 6
};

#pragma pack(push, 1)
struct MeshHeader {
  uint16_t magic = MESH_MAGIC;
  uint8_t version = MESH_VERSION;
  MeshMessageType type;
  uint16_t payloadLen;
  uint32_t timestamp;
};

struct MeshAudioPayload {
  uint8_t codec; // 0=RAW8, 1=ADPCM
  uint16_t sequence;
  uint8_t data[230]; // Max ESP-NOW is 250 bytes total usually
};

struct MeshChatPayload {
  uint8_t isGroup;
  char sender[16];
  char text[200];
};

struct MeshPingPayload {
  char deviceName[16];
  uint8_t dragonLevel;
  uint32_t uptime;
};
#pragma pack(pop)

class EspNowMesh {
public:
  EspNowMesh();
  ~EspNowMesh();

  bool begin();
  void update();
  void end();

  // Core functionality
  bool broadcast(const void *data, size_t len);
  bool send(const uint8_t *mac, const void *data, size_t len);

  // Feature wrappers
  bool sendAudio(const uint8_t *audioData, size_t len);
  bool sendChat(const char *message, bool global = true);
  bool sendPing();

  // Callbacks
  using AudioCallback =
      std::function<void(const uint8_t *mac, const uint8_t *data, size_t len)>;
  using ChatCallback = std::function<void(
      const uint8_t *mac, const char *sender, const char *text)>;
  using PingCallback =
      std::function<void(const uint8_t *mac, int rssi, const char *name)>;

  void onAudio(AudioCallback cb) { _audioCb = cb; }
  void onChat(ChatCallback cb) { _chatCb = cb; }
  void onPing(PingCallback cb) { _pingCb = cb; }

  // State
  void setWalkieTalkieMode(bool enabled);
  bool isWalkieTalkieMode() const { return _walkieTalkieMode; }

private:
  static void onDataRecv(const uint8_t *mac, const uint8_t *incomingData,
                         int len);
  static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

  void processPacket(const uint8_t *mac, const uint8_t *data, size_t len);

  AudioCallback _audioCb;
  ChatCallback _chatCb;
  PingCallback _pingCb;

  bool _walkieTalkieMode = false;
  uint8_t _broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
};

extern EspNowMesh MeshSystem;
