<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/AI-NEURA9-FF6F00?style=for-the-badge&logo=tensorflow&logoColor=white" alt="NEURA9"/>
  <img src="https://img.shields.io/badge/Framework-TensorFlow%20Lite-FF6F00?style=for-the-badge&logo=tensorflow&logoColor=white" alt="TensorFlow"/>
  <img src="https://img.shields.io/badge/Python-3.8+-3776AB?style=for-the-badge&logo=python&logoColor=white" alt="Python"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Classes-10-blue?style=flat-square" alt="Classes"/>
  <img src="https://img.shields.io/badge/Parameters-~6K-green?style=flat-square" alt="Parameters"/>
  <img src="https://img.shields.io/badge/Size-~24KB-purple?style=flat-square" alt="Size"/>
  <img src="https://img.shields.io/badge/Inference-<50ms-orange?style=flat-square" alt="Inference"/>
</p>

---

# 🧠 Tutorial Completo de Treinamento - NEURA9 IA Defensiva

**Modelo:** NEURA9 Defense AI  
**Versão:** 2.1.0  
**Framework:** TensorFlow Lite Micro  
**Plataforma:** ESP32-S3 (Offline 100%)

---

## 📋 Índice

1. [Visão Geral](#visao-geral)
2. [Requisitos](#requisitos)
3. [Coleta de Dados](#coleta-de-dados)
4. [Preparação do Dataset](#preparacao-do-dataset)
5. [Treinamento do Modelo](#treinamento-do-modelo)
6. [Conversão para TFLite](#conversao-para-tflite)
7. [Integração no Firmware](#integracao-no-firmware)
8. [Validação e Testes](#validacao-e-testes)

---

## 🎯 Visão Geral

A **NEURA9** é uma IA defensiva embarcada que classifica o ambiente WiFi em 10 categorias de ameaça em tempo real, 100% offline no ESP32-S3.

### Categorias de Classificação

| ID | Nome | Descrição |
|----|------|-----------|
| 0 | SAFE | Ambiente seguro, sem ameaças detectadas |
| 1 | CROWDED | Muitas redes, alta densidade de APs |
| 2 | OPEN_NETWORK | Redes abertas detectadas (risco médio) |
| 3 | EVIL_TWIN_RISK | Possível Evil Twin Attack |
| 4 | DEAUTH_DETECTED | Ataques de deauth em massa |
| 5 | ROGUE_AP | Access Point malicioso detectado |
| 6 | HIGH_RISK | Ambiente de alto risco geral |
| 7 | BATTERY_CRITICAL | Bateria crítica (modo economia) |
| 8 | GESTURE_COMMAND | Gesto/comando detectado pelo IMU |
| 9 | LEARNING_MODE | Modo de aprendizado/calibração |

### Arquitetura do Modelo

```
Entrada (72 features)
    ↓
Dense Layer (64 neurônios, ReLU)
    ↓
Dense Layer (32 neurônios, ReLU)
    ↓
Output Layer (10 classes, Softmax)
```

**Total:** ~6.000 parâmetros, ~24 KB em memória

---

## 💻 Requisitos

### Software

```bash
# Python 3.8+
python --version

# TensorFlow
pip install tensorflow==2.13.0

# Bibliotecas auxiliares
pip install numpy pandas matplotlib scikit-learn

# TFLite conversion tools
pip install tensorflow-lite
```

### Hardware para Treinamento

- **CPU:** Qualquer (treina em minutos)
- **RAM:** 4 GB mínimo
- **Storage:** 500 MB para datasets

---

## 📊 Coleta de Dados

### Formato do Dataset

Cada amostra deve conter 72 features:

```python
features = [
    # WiFi Stats (0-3)
    aps_seen,           # Número de APs vistos
    handshakes,         # Handshakes capturados
    pmkids,             # PMKIDs capturados
    deauths,            # Deauth packets detectados
    
    # Bateria (4-5)
    battery_percent,    # 0.0 - 1.0
    is_charging,        # 0 ou 1
    
    # Movimento (6)
    is_moving,          # 0 ou 1
    
    # Tempo (7)
    uptime_hours,       # Horas desde boot
    
    # Canal WiFi (8)
    current_channel,    # 1-13
    
    # Features Avançadas (9-71) - Para expansão futura
    # IMU, RTC, estatísticas detalhadas, etc.
    ... (63 features zeradas ou calculadas)
]
```

### Script de Coleta

Crie `scripts/collect_data.py`:

```python
import serial
import json
import time
import csv

def collect_from_device(port='/dev/ttyUSB0', duration=3600):
    """
    Coleta dados do WavePwn via serial por N segundos
    """
    ser = serial.Serial(port, 115200, timeout=1)
    data = []
    
    start_time = time.time()
    
    while time.time() - start_time < duration:
        line = ser.readline().decode('utf-8').strip()
        
        if line.startswith('[NEURA9]'):
            # Parse da linha de log
            # Exemplo: [NEURA9] classe=4 (DEAUTH_DETECTED) conf=0.95
            try:
                parts = line.split()
                classe = int(parts[1].split('=')[1])
                
                # Solicitar features completas via comando serial
                ser.write(b'GET_FEATURES\n')
                features_line = ser.readline().decode('utf-8').strip()
                features = json.loads(features_line)
                
                data.append({
                    'features': features,
                    'label': classe
                })
                
                print(f"Coletado: {len(data)} amostras, classe={classe}")
                
            except Exception as e:
                print(f"Erro: {e}")
                continue
        
        time.sleep(0.1)
    
    ser.close()
    
    # Salvar dataset
    with open('dataset_raw.json', 'w') as f:
        json.dump(data, f, indent=2)
    
    print(f"\nColetados {len(data)} amostras!")
    return data

if __name__ == '__main__':
    collect_from_device(duration=7200)  # 2 horas
```

---

## 🔨 Preparação do Dataset

Crie `scripts/prepare_dataset.py`:

```python
import json
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

def load_and_prepare_dataset(filepath='dataset_raw.json'):
    """
    Carrega e prepara dataset para treinamento
    """
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    # Separar features e labels
    X = np.array([sample['features'] for sample in data])
    y = np.array([sample['label'] for sample in data])
    
    print(f"Dataset shape: {X.shape}")
    print(f"Classes: {np.unique(y)}")
    print(f"Distribuição:")
    for i in range(10):
        count = np.sum(y == i)
        print(f"  Classe {i}: {count} amostras ({100*count/len(y):.1f}%)")
    
    # Split treino/validação/teste
    X_train, X_temp, y_train, y_temp = train_test_split(
        X, y, test_size=0.3, random_state=42, stratify=y
    )
    
    X_val, X_test, y_val, y_test = train_test_split(
        X_temp, y_temp, test_size=0.5, random_state=42, stratify=y_temp
    )
    
    # Normalização
    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_val = scaler.transform(X_val)
    X_test = scaler.transform(X_test)
    
    # Salvar dados preparados
    np.savez('dataset_prepared.npz',
             X_train=X_train, y_train=y_train,
             X_val=X_val, y_val=y_val,
             X_test=X_test, y_test=y_test)
    
    # Salvar scaler para uso futuro
    import joblib
    joblib.dump(scaler, 'scaler.pkl')
    
    print(f"\nDados salvos:")
    print(f"  Treino: {len(X_train)} amostras")
    print(f"  Validação: {len(X_val)} amostras")
    print(f"  Teste: {len(X_test)} amostras")
    
    return X_train, y_train, X_val, y_val, X_test, y_test

if __name__ == '__main__':
    load_and_prepare_dataset()
```

---

## 🎓 Treinamento do Modelo

Crie `scripts/train_neura9.py`:

```python
import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import matplotlib.pyplot as plt

def create_model(input_shape=(72,), num_classes=10):
    """
    Cria modelo NEURA9
    """
    model = keras.Sequential([
        # Input layer
        layers.Input(shape=input_shape),
        
        # Hidden layer 1
        layers.Dense(64, activation='relu', name='dense1'),
        layers.Dropout(0.3),
        
        # Hidden layer 2
        layers.Dense(32, activation='relu', name='dense2'),
        layers.Dropout(0.2),
        
        # Output layer
        layers.Dense(num_classes, activation='softmax', name='output')
    ])
    
    return model

def train_model():
    """
    Treina o modelo NEURA9
    """
    # Carregar dados
    data = np.load('dataset_prepared.npz')
    X_train = data['X_train']
    y_train = data['y_train']
    X_val = data['X_val']
    y_val = data['y_val']
    
    # Criar modelo
    model = create_model()
    
    # Compilar
    model.compile(
        optimizer='adam',
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )
    
    # Callbacks
    callbacks = [
        keras.callbacks.ModelCheckpoint(
            'neura9_best.h5',
            save_best_only=True,
            monitor='val_accuracy'
        ),
        keras.callbacks.EarlyStopping(
            monitor='val_loss',
            patience=20,
            restore_best_weights=True
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss',
            factor=0.5,
            patience=10
        )
    ]
    
    # Treinar
    print("Iniciando treinamento...")
    history = model.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        epochs=200,
        batch_size=32,
        callbacks=callbacks,
        verbose=1
    )
    
    # Plotar histórico
    plt.figure(figsize=(12, 4))
    
    plt.subplot(1, 2, 1)
    plt.plot(history.history['loss'], label='Treino')
    plt.plot(history.history['val_loss'], label='Validação')
    plt.title('Loss')
    plt.legend()
    
    plt.subplot(1, 2, 2)
    plt.plot(history.history['accuracy'], label='Treino')
    plt.plot(history.history['val_accuracy'], label='Validação')
    plt.title('Accuracy')
    plt.legend()
    
    plt.savefig('training_history.png')
    print("\nGráfico salvo: training_history.png")
    
    # Salvar modelo final
    model.save('neura9_final.h5')
    print("Modelo salvo: neura9_final.h5")
    
    return model, history

if __name__ == '__main__':
    model, history = train_model()
```

---

## 🔄 Conversão para TFLite

Crie `scripts/convert_to_tflite.py`:

```python
import tensorflow as tf
import numpy as np

def convert_to_tflite():
    """
    Converte modelo Keras para TensorFlow Lite
    """
    # Carregar modelo treinado
    model = tf.keras.models.load_model('neura9_final.h5')
    
    # Criar conversor
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    
    # Otimizações para ESP32
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    
    # Quantização INT8 (reduz tamanho 4x)
    def representative_dataset():
        data = np.load('dataset_prepared.npz')
        for sample in data['X_train'][:100]:
            yield [sample.reshape(1, -1).astype(np.float32)]
    
    converter.representative_dataset = representative_dataset
    converter.inference_input_type = tf.float32
    converter.inference_output_type = tf.float32
    
    # Converter
    tflite_model = converter.convert()
    
    # Salvar
    with open('neura9_model.tflite', 'wb') as f:
        f.write(tflite_model)
    
    print(f"Modelo TFLite salvo: neura9_model.tflite")
    print(f"Tamanho: {len(tflite_model) / 1024:.2f} KB")
    
    return tflite_model

if __name__ == '__main__':
    convert_to_tflite()
```

---

## 📦 Integração no Firmware

Crie `scripts/generate_cpp_array.py`:

```python
def convert_tflite_to_cpp(tflite_file='neura9_model.tflite',
                          output_file='neura9_defense_model_data.cpp'):
    """
    Converte .tflite para array C++
    """
    with open(tflite_file, 'rb') as f:
        tflite_data = f.read()
    
    # Gerar código C++
    cpp_code = f"""// Modelo NEURA9 - Gerado automaticamente
// Tamanho: {len(tflite_data)} bytes

#include <cstdint>

// Modelo TFLite (quantizado INT8)
alignas(8) const unsigned char neura9_defense_model_tflite[] = {{
"""
    
    # Adicionar bytes (16 por linha)
    for i in range(0, len(tflite_data), 16):
        chunk = tflite_data[i:i+16]
        hex_values = ', '.join(f'0x{b:02x}' for b in chunk)
        cpp_code += f"    {hex_values},\n"
    
    # Fechar array
    cpp_code += """};\n\n"""
    cpp_code += f"const unsigned int neura9_defense_model_tflite_len = {len(tflite_data)};\n"
    
    # Salvar
    with open(output_file, 'w') as f:
        f.write(cpp_code)
    
    print(f"Arquivo C++ gerado: {output_file}")
    print(f"Tamanho do modelo: {len(tflite_data)} bytes ({len(tflite_data)/1024:.2f} KB)")
    print(f"\nCopie este arquivo para: src/ai/neura9_defense_model_data.cpp")

if __name__ == '__main__':
    convert_tflite_to_cpp()
```

---

## ✅ Checklist de Treinamento

- [ ] Coletar pelo menos 1.000 amostras de cada classe
- [ ] Balancear dataset (classes com amostras similares)
- [ ] Normalizar features com StandardScaler
- [ ] Treinar modelo até accuracy > 90%
- [ ] Converter para TFLite quantizado
- [ ] Gerar array C++
- [ ] Substituir stub no firmware
- [ ] Testar no hardware real
- [ ] Ajustar threshold de confiança

---

## 📈 Métricas Esperadas

| Métrica | Valor Alvo |
|---------|------------|
| **Accuracy** | > 90% |
| **Tamanho Modelo** | < 30 KB |
| **Inferência** | < 50 ms |
| **RAM Uso** | < 100 KB |

---

**Desenvolvido para WavePwn v2.1.0**  
*Defendendo redes com IA embarcada* 🧠🛡️
