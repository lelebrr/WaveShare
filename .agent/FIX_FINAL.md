# 🔧 CORREÇÃO FINAL SIMPLES

**ÚLTIMO ERRO:** `'IMU' was not declared in this scope`

## ✅ SOLUÇÃO (1 LINHA)

Abra `src/hardware/sensors_driver.cpp` e adicione na linha 9:

```cpp
#include "sensors_driver.h"
#include <Wire.h>
#include <XPowersLib.h>
#include <SensorQMI8658.hpp>  // ← ADICIONAR ESTA LINHA

// Instâncias globais
```

## ✅ ENTÃO COMPILAR

```bash
pio run
```

**ISSO VAI FUNCIONAR!** É só 1 linha faltando!
