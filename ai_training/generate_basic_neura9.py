#!/usr/bin/env python3
"""
NEURA9 - Gerador de Modelo Básico Funcional
Cria um modelo TensorFlow Lite simples para substituir o stub
"""

import numpy as np
import tensorflow as tf
from tensorflow import keras
import os

def create_synthetic_dataset(num_samples=1000):
    """
    Gera dataset sintético para treinamento inicial
    """
    print("[1/5] Gerando dataset sintético...")
    
    # 72 features de entrada
    X = []
    y = []
    
    for _ in range(num_samples):
        features = np.zeros(72, dtype=np.float32)
        
        # Simular diferentes cenários
        scenario = np.random.randint(0, 10)
        
        if scenario == 0:  # SAFE
            features[0] = np.random.randint(1, 10)  # Poucos APs
            features[3] = 0  # Sem deauths
            features[4] = np.random.uniform(0.5, 1.0)  # Bateria OK
        
        elif scenario == 1:  # CROWDED
            features[0] = np.random.randint(50, 200)  # Muitos APs
            features[3] = np.random.randint(0, 10)  # Poucos deauths
        
        elif scenario == 2:  # OPEN_NETWORK
            features[0] = np.random.randint(5, 30)
            features[2] = np.random.randint(1, 5)  # Alguns PMKIDs de redes abertas
        
        elif scenario == 3:  # EVIL_TWIN_RISK
            features[0] = np.random.randint(10, 20)
            features[3] = np.random.randint(5, 15)
       
        
        elif scenario == 4:  # DEAUTH_DETECTED
            features[0] = np.random.randint(5, 50)
            features[3] = np.random.randint(50, 200)  # Muitos deauths!
        
        elif scenario == 5:  # ROGUE_AP
            features[0] = np.random.randint(1, 10)
            features[3] = np.random.randint(20, 50)
        
        elif scenario == 6:  # HIGH_RISK
            features[0] = np.random.randint(20, 100)
            features[3] = np.random.randint(30, 100)
            features[4] = np.random.uniform(0.1, 0.3)  # Bateria baixa
        
        elif scenario == 7:  # BATTERY_CRITICAL
            features[4] = np.random.uniform(0.0, 0.05)  # Bateria crítica
            features[5] = 0.0  # Não está carregando
        
        elif scenario == 8:  # GESTURE_COMMAND
            features[6] = 1.0  # Movimento detectado
            features[7] = np.random.uniform(0.0, 1.0)  # Uptime variado
        
        elif scenario == 9:  # LEARNING_MODE
            features[7] = np.random.uniform(0.0, 0.1)  # Pouco uptime (inicio)
        
        X.append(features)
        y.append(scenario)
    
    return np.array(X), np.array(y)

def create_model():
    """
    Cria modelo NEURA9 (72 -> 64 -> 32 -> 10)
    """
    print("[2/5] Criando arquitetura do modelo...")
    
    model = keras.Sequential([
        keras.layers.Input(shape=(72,)),
        keras.layers.Dense(64, activation='relu', name='dense1'),
        keras.layers.Dropout(0.2),
        keras.layers.Dense(32, activation='relu', name='dense2'),
        keras.layers.Dropout(0.1),
        keras.layers.Dense(10, activation='softmax', name='output')
    ])
    
    model.compile(
        optimizer='adam',
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )
    
    model.summary()
    return model

def train_model(model, X_train, y_train):
    """
    Treina o modelo
    """
    print("[3/5] Treinando modelo...")
    
    history = model.fit(
        X_train, y_train,
        epochs=50,
        batch_size=32,
        validation_split=0.2,
        verbose=1
    )
    
    return history

def convert_to_tflite(model, output_path='neura9_model.tflite'):
    """
    Converte para TensorFlow Lite
    """
    print("[4/5] Convertendo para TensorFlow Lite...")
    
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    
    # Otimizações para ESP32
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    
    tflite_model = converter.convert()
    
    with open(output_path, 'wb') as f:
        f.write(tflite_model)
    
    print(f"Modelo TFLite salvo: {output_path}")
    print(f"Tamanho: {len(tflite_model)} bytes ({len(tflite_model)/1024:.2f} KB)")
    
    return tflite_model

def generate_cpp_array(tflite_file='neura9_model.tflite', 
                       output_file='neura9_defense_model_data.cpp'):
    """
    Gera array C++ para embedding
    """
    print("[5/5] Gerando array C++...")
    
    with open(tflite_file, 'rb') as f:
        tflite_data = f.read()
    
    cpp_code = f"""// Auto-gerado - NEURA9 Defense Model
// Tamanho: {len(tflite_data)} bytes ({len(tflite_data)/1024:.2f} KB)
// Gerado em: {np.datetime64('now')}

#include <cstdint>

// Modelo TFLite otimizado
alignas(8) const unsigned char neura9_defense_model_tflite[] = {{
"""
    
    # Adicionar bytes (16 por linha)
    for i in range(0, len(tflite_data), 16):
        chunk = tflite_data[i:i+16]
        hex_values = ', '.join(f'0x{b:02x}' for b in chunk)
        cpp_code += f"    {hex_values},\n"
    
    cpp_code += """};\n\n"""
    cpp_code += f"const unsigned int neura9_defense_model_tflite_len = {len(tflite_data)};\n"
    
    with open(output_file, 'w') as f:
        f.write(cpp_code)
    
    print(f"\\n✅ Arquivo C++ gerado: {output_file}")
    print(f"📝 Copie este arquivo para: src/neura9/neura9_defense_model_data.cpp")
    print(f"📊 Tamanho do modelo: {len(tflite_data)} bytes ({len(tflite_data)/1024:.2f} KB)")
    
    return cpp_code

def main():
    print("=" * 60)
    print("🧠 NEURA9 - Gerador de Modelo Básico Funcional")
    print("=" * 60)
    print()
    
    # 1. Gerar dataset
    X, y = create_synthetic_dataset(num_samples=2000)
    print(f"Dataset: {X.shape[0]} amostras, {X.shape[1]} features")
    print(f"Classes: {np.unique(y)}")
    print()
    
    # 2. Criar modelo
    model = create_model()
    print()
    
    # 3. Treinar
    history = train_model(model, X, y)
    final_acc = history.history['accuracy'][-1]
    final_val_acc = history.history['val_accuracy'][-1]
    print(f"Accuracy final: {final_acc:.4f}")
    print(f"Validation Accuracy: {final_val_acc:.4f}")
    print()
    
    # 4. Converter para TFLite
    tflite_model = convert_to_tflite(model)
    print()
    
    # 5. Gerar array C++
    cpp_code = generate_cpp_array()
    print()
    
    print("=" * 60)
    print("✅ CONCLUÍDO!")
    print("=" * 60)
    print()
    print("Próximos passos:")
    print("1. Copie 'neura9_defense_model_data.cpp' para 'src/neura9/'")
    print("2. Remova ou comente o stub inline em 'src/neura9/inference.cpp'")
    print("3. Recompile o projeto")
    print()

if __name__ == '__main__':
    main()
