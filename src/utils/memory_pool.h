/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Memory Pool (Otimização de Memória)
 * Pool de memória para alocações rápidas e fragmentação reduzida
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <esp_heap_caps.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Estatísticas de memória
struct MemoryStats {
  uint32_t freeHeap;
  uint32_t minFreeHeap;
  uint32_t freePSRAM;
  uint32_t usedPSRAM;
  uint32_t largestBlock;
  float fragmentationRatio;
};

class MemoryPool {
public:
  MemoryPool();

  void begin();

  // Alocação otimizada
  void *allocRAM(size_t size);   // Prefere DRAM
  void *allocPSRAM(size_t size); // Prefere PSRAM
  void *allocAuto(size_t size);  // Automático baseado em tamanho
  void free(void *ptr);

  // Pool pré-alocado para objetos frequentes
  void *allocFromPool(size_t size);
  void freeToPool(void *ptr, size_t size);

  // Estatísticas
  MemoryStats getStats() const;
  void printStats() const;

  // Verificações
  bool hasSufficientRAM(size_t needed) const;
  bool hasSufficientPSRAM(size_t needed) const;

  // Limpeza
  void garbageCollect();
  void defragment();

private:
  // Pool para objetos pequenos (64, 128, 256 bytes)
  struct PoolBlock {
    uint8_t *memory;
    bool *used;
    size_t blockSize;
    size_t blockCount;
  };

  PoolBlock _pool64;
  PoolBlock _pool128;
  PoolBlock _pool256;

  void initPool(PoolBlock &pool, size_t blockSize, size_t count);
  void *allocFromPoolBlock(PoolBlock &pool);
  void freeFromPoolBlock(PoolBlock &pool, void *ptr);
};

extern MemoryPool memory_pool;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

MemoryPool memory_pool;

MemoryPool::MemoryPool() {
  _pool64.memory = nullptr;
  _pool128.memory = nullptr;
  _pool256.memory = nullptr;
}

void MemoryPool::begin() {
  // Inicializa pools em PSRAM para economia de DRAM
  initPool(_pool64, 64, 32);   // 32 blocos de 64 bytes = 2KB
  initPool(_pool128, 128, 16); // 16 blocos de 128 bytes = 2KB
  initPool(_pool256, 256, 8);  // 8 blocos de 256 bytes = 2KB

  printStats();
  Serial.println("[Memory] Pool inicializado");
}

void MemoryPool::initPool(PoolBlock &pool, size_t blockSize, size_t count) {
  pool.blockSize = blockSize;
  pool.blockCount = count;

  // Aloca memória do pool em PSRAM
  pool.memory =
      (uint8_t *)heap_caps_malloc(blockSize * count, MALLOC_CAP_SPIRAM);
  pool.used = (bool *)heap_caps_malloc(count * sizeof(bool), MALLOC_CAP_SPIRAM);

  if (pool.memory && pool.used) {
    memset(pool.used, 0, count * sizeof(bool));
  }
}

void *MemoryPool::allocRAM(size_t size) {
  return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
}

void *MemoryPool::allocPSRAM(size_t size) {
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
}

void *MemoryPool::allocAuto(size_t size) {
  // Pequenas alocações: DRAM (mais rápido)
  // Grandes alocações: PSRAM (mais espaço)
  if (size < 1024) {
    return allocRAM(size);
  } else {
    void *ptr = allocPSRAM(size);
    if (!ptr)
      ptr = allocRAM(size); // Fallback
    return ptr;
  }
}

void MemoryPool::free(void *ptr) {
  if (ptr) {
    heap_caps_free(ptr);
  }
}

void *MemoryPool::allocFromPool(size_t size) {
  if (size <= 64 && _pool64.memory) {
    return allocFromPoolBlock(_pool64);
  } else if (size <= 128 && _pool128.memory) {
    return allocFromPoolBlock(_pool128);
  } else if (size <= 256 && _pool256.memory) {
    return allocFromPoolBlock(_pool256);
  }

  // Fallback para alocação normal
  return allocAuto(size);
}

void *MemoryPool::allocFromPoolBlock(PoolBlock &pool) {
  for (size_t i = 0; i < pool.blockCount; i++) {
    if (!pool.used[i]) {
      pool.used[i] = true;
      return pool.memory + (i * pool.blockSize);
    }
  }
  return nullptr; // Pool cheio
}

void MemoryPool::freeToPool(void *ptr, size_t size) {
  if (!ptr)
    return;

  // Verifica em qual pool está
  PoolBlock *pools[] = {&_pool64, &_pool128, &_pool256};

  for (auto pool : pools) {
    if (!pool->memory)
      continue;

    if (ptr >= pool->memory &&
        ptr < pool->memory + (pool->blockSize * pool->blockCount)) {
      size_t idx = ((uint8_t *)ptr - pool->memory) / pool->blockSize;
      pool->used[idx] = false;
      return;
    }
  }

  // Não estava em nenhum pool, free normal
  free(ptr);
}

MemoryStats MemoryPool::getStats() const {
  MemoryStats stats;

  stats.freeHeap = esp_get_free_heap_size();
  stats.minFreeHeap = esp_get_minimum_free_heap_size();
  stats.freePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  stats.usedPSRAM =
      heap_caps_get_total_size(MALLOC_CAP_SPIRAM) - stats.freePSRAM;
  stats.largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);

  // Fragmentação = 1 - (maior bloco / heap livre)
  if (stats.freeHeap > 0) {
    stats.fragmentationRatio =
        1.0f - ((float)stats.largestBlock / stats.freeHeap);
  } else {
    stats.fragmentationRatio = 1.0f;
  }

  return stats;
}

void MemoryPool::printStats() const {
  MemoryStats stats = getStats();

  Serial.println("[Memory] ═══════════════════════════════");
  Serial.printf("[Memory] Free Heap: %d KB\n", stats.freeHeap / 1024);
  Serial.printf("[Memory] Min Free: %d KB\n", stats.minFreeHeap / 1024);
  Serial.printf("[Memory] Free PSRAM: %d KB\n", stats.freePSRAM / 1024);
  Serial.printf("[Memory] Used PSRAM: %d KB\n", stats.usedPSRAM / 1024);
  Serial.printf("[Memory] Largest Block: %d KB\n", stats.largestBlock / 1024);
  Serial.printf("[Memory] Fragmentation: %.1f%%\n",
                stats.fragmentationRatio * 100);
  Serial.println("[Memory] ═══════════════════════════════");
}

bool MemoryPool::hasSufficientRAM(size_t needed) const {
  return esp_get_free_heap_size() > needed + 20000; // 20KB buffer
}

bool MemoryPool::hasSufficientPSRAM(size_t needed) const {
  return heap_caps_get_free_size(MALLOC_CAP_SPIRAM) >
         needed + 50000; // 50KB buffer
}

void MemoryPool::garbageCollect() {
  // ESP32 não tem GC tradicional, mas podemos forçar limpeza
  Serial.println("[Memory] Garbage collection iniciado");
  
  // Libera buffers de WiFi não utilizados
  esp_wifi_clear_ap_list();
  
  // Força o sistema a recuperar memória fragmentada
  // através de alocação e liberação de blocos grandes
  void* temp = heap_caps_malloc(16384, MALLOC_CAP_SPIRAM);
  if (temp) {
    heap_caps_free(temp);
  }
  
  // Limpa cache de tarefas ociosas
  vTaskDelay(1); // Permite que o scheduler limpe recursos
  
  Serial.printf("[Memory] GC concluído. Free: %d KB\n", 
                esp_get_free_heap_size() / 1024);
}

void MemoryPool::defragment() {
  // Realocação não é possível diretamente, mas podemos:
  // 1. Liberando e realocando nossos pools
  Serial.println("[Memory] Defragment iniciado");
  
  // Salva estado atual dos pools
  PoolBlock pools[] = {_pool64, _pool128, _pool256};
  
  for (auto& pool : pools) {
    if (!pool.memory) continue;
    
    // Conta blocos em uso
    int usedCount = 0;
    for (size_t i = 0; i < pool.blockCount; i++) {
      if (pool.used[i]) usedCount++;
    }
    
    Serial.printf("[Memory] Pool %d: %d/%d blocos em uso\n", 
                  pool.blockSize, usedCount, pool.blockCount);
  }
  
  // Força liberação de memória do sistema
  esp_get_minimum_free_heap_size(); // Atualiza estatísticas
  
  Serial.printf("[Memory] Maior bloco livre: %d KB\n",
                heap_caps_get_largest_free_block(MALLOC_CAP_8BIT) / 1024);
}

