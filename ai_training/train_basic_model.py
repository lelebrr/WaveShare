#!/usr/bin/env python3
"""
═══════════════════════════════════════════════════════════════════════════
WAVEPWN - NEURA9 Model Training
Treina modelo TensorFlow Lite para classificação de ameaças WiFi
═══════════════════════════════════════════════════════════════════════════
"""

import numpy as np
import os

# Tenta importar TensorFlow
try:
    import tensorflow as tf
    from tensorflow import keras
    from tensorflow.keras import layers
    HAS_TF = True
except ImportError:
    print("TensorFlow não disponível. Instale com: pip install tensorflow")
    HAS_TF = False

FEATURE_SIZE = 32
NUM_CLASSES = 9

def create_model():
    """Cria modelo neural simples otimizado para ESP32"""
    model = keras.Sequential([
        # Input layer
        layers.Input(shape=(FEATURE_SIZE,)),
        
        # Hidden layers - pequenos para ESP32
        layers.Dense(32, activation='relu'),
        layers.Dropout(0.2),
        layers.Dense(16, activation='relu'),
        layers.Dropout(0.1),
        
        # Output layer
        layers.Dense(NUM_CLASSES, activation='softmax')
    ])
    
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=0.001),
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )
    
    return model

def train_model(X_train, y_train, X_val, y_val, epochs=50):
    """Treina o modelo"""
    model = create_model()
    
    print(f"Treinando modelo com {len(X_train)} amostras...")
    print(model.summary())
    
    # Callbacks
    early_stop = keras.callbacks.EarlyStopping(
        monitor='val_loss', 
        patience=10,
        restore_best_weights=True
    )
    
    # Treina
    history = model.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        epochs=epochs,
        batch_size=32,
        callbacks=[early_stop],
        verbose=1
    )
    
    # Avalia
    loss, accuracy = model.evaluate(X_val, y_val, verbose=0)
    print(f"\nAcurácia final: {accuracy*100:.2f}%")
    
    return model, history

def representative_data_gen():
    """Gerador de dados representativos para quantização"""
    # Carrega uma pequena amostra do dataset
    if os.path.exists('neura9_dataset_X.npy'):
        data = np.load('neura9_dataset_X.npy')
        # Usa primeiras 100 amostras
        for i in range(min(100, len(data))):
            yield [data[i].astype(np.float32).reshape(1, FEATURE_SIZE)]
    else:
        # Fallback se dataset não existir (apenas para teste script)
        for _ in range(100):
            yield [np.random.rand(1, FEATURE_SIZE).astype(np.float32)]

def convert_to_tflite(model, output_path='neura9_model.tflite'):
    """Converte modelo para TensorFlow Lite com Quantização INT8"""
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    
    # Otimizações para ESP32 (Quantização total)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    
    # Full Integer Quantization
    converter.representative_dataset = representative_data_gen
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8  # or tf.uint8
    converter.inference_output_type = tf.int8 # or tf.uint8
    
    tflite_model = converter.convert()
    
    with open(output_path, 'wb') as f:
        f.write(tflite_model)
    
    print(f"Modelo TFLite (INT8) salvo: {output_path} ({len(tflite_model)} bytes)")
    return tflite_model

def generate_cpp_array(tflite_model, output_path='neura9_model_data.h'):
    """Gera header C++ com array do modelo"""
    
    with open(output_path, 'w') as f:
        f.write('/**\n')
        f.write(' * NEURA9 - Trained TFLite Model Data\n')
        f.write(' * Auto-generated - Do not edit\n')
        f.write(f' * Model size: {len(tflite_model)} bytes\n')
        f.write(' * Quantization: INT8\n')
        f.write(' */\n\n')
        f.write('#pragma once\n\n')
        f.write('#include <stdint.h>\n\n')
        f.write(f'const unsigned int neura9_model_len = {len(tflite_model)};\n\n')
        f.write('alignas(16) const uint8_t neura9_model_data[] = {\n')
        
        # Escreve bytes em linhas de 16
        for i in range(0, len(tflite_model), 16):
            chunk = tflite_model[i:i+16]
            hex_str = ', '.join(f'0x{b:02x}' for b in chunk)
            f.write(f'    {hex_str},\n')
        
        f.write('};\n')
    
    print(f"Header C++ gerado: {output_path}")

def main():
    if not HAS_TF:
        print("Erro: TensorFlow não disponível")
        return
    
    # Carrega dataset
    if not os.path.exists('neura9_dataset_X.npy'):
        print("Dataset não encontrado. Execute generate_synthetic_dataset.py primeiro.")
        return
    
    X = np.load('neura9_dataset_X.npy')
    y = np.load('neura9_dataset_y.npy')
    
    # Split train/val (80/20)
    split = int(len(X) * 0.8)
    X_train, X_val = X[:split], X[split:]
    y_train, y_val = y[:split], y[split:]
    
    print(f"Dataset: {len(X_train)} treino, {len(X_val)} validação")
    
    # Treina
    model, history = train_model(X_train, y_train, X_val, y_val)
    
    # Converte para TFLite
    tflite_model = convert_to_tflite(model)
    
    # Gera header C++
    generate_cpp_array(tflite_model)
    
    print("\n✅ Pipeline completo!")
    print("Copie neura9_model_data.h para src/neura9/")

if __name__ == '__main__':
    main()
