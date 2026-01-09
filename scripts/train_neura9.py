"""
Script de Treinamento Completo - NEURA9 Defense AI
Treina modelo de classificação de ameaças WiFi
"""

import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import joblib
import json
import os

# Configurações
INPUT_FEATURES = 72
NUM_CLASSES = 10
EPOCHS = 200
BATCH_SIZE = 32

CLASS_NAMES = [
    'SAFE', 'CROWDED', 'OPEN_NETWORK', 'EVIL_TWIN_RISK',
    'DEAUTH_DETECTED', 'ROGUE_AP', 'HIGH_RISK', 
    'BATTERY_CRITICAL', 'GESTURE_COMMAND', 'LEARNING_MODE'
]

def generate_synthetic_dataset(samples_per_class=200):
    """
    Gera dataset sintético para teste (substitua por dados reais!)
    """
    print("Gerando dataset sintético para demonstração...")
    
    X = []
    y = []
    
    for class_id in range(NUM_CLASSES):
        for _ in range(samples_per_class):
            # Gerar features aleatórias com padrões específicos por classe
            features = np.zeros(INPUT_FEATURES)
            
            if class_id == 0:  # SAFE
                features[0] = np.random.randint(1, 10)  # Poucos APs
                features[3] = 0  # Sem deauths
                features[4] = np.random.uniform(0.5, 1.0)  # Bateria OK
                
            elif class_id == 1:  # CROWDED
                features[0] = np.random.randint(20, 50)  # Muitos APs
                features[4] = np.random.uniform(0.3, 1.0)
                
            elif class_id == 4:  # DEAUTH_DETECTED
                features[0] = np.random.randint(5, 20)
                features[3] = np.random.randint(30, 100)  # Muitos deauths
                
            elif class_id == 7:  # BATTERY_CRITICAL
                features[4] = np.random.uniform(0.0, 0.1)  # Bateria baixa
                
            # Adicionar ruído
            features += np.random.normal(0, 0.1, INPUT_FEATURES)
            
            X.append(features)
            y.append(class_id)
    
    return np.array(X), np.array(y)

def create_model():
    """
    Cria arquitetura do modelo NEURA9
    """
    model = keras.Sequential([
        layers.Input(shape=(INPUT_FEATURES,)),
        layers.Dense(64, activation='relu'),
        layers.Dropout(0.3),
        layers.Dense(32, activation='relu'),
        layers.Dropout(0.2),
        layers.Dense(NUM_CLASSES, activation='softmax')
    ], name='NEURA9')
    
    return model

def train_model(X_train, y_train, X_val, y_val):
    """
    Treina o modelo
    """
    model = create_model()
    
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=0.001),
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )
    
    print("\n📊 Arquitetura do Modelo:")
    model.summary()
    
    callbacks = [
        keras.callbacks.ModelCheckpoint(
            'models/neura9_best.h5',
            save_best_only=True,
            monitor='val_accuracy',
            verbose=1
        ),
        keras.callbacks.EarlyStopping(
            monitor='val_loss',
            patience=30,
            restore_best_weights=True,
            verbose=1
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss',
            factor=0.5,
            patience=15,
            min_lr=1e-6,
            verbose=1
        )
    ]
    
    print("\n🎓 Iniciando treininamento...")
    history = model.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        epochs=EPOCHS,
        batch_size=BATCH_SIZE,
        callbacks=callbacks,
        verbose=1
    )
    
    return model, history

def plot_training_history(history):
    """
    Plota histórico de treinamento
    """
    plt.figure(figsize=(15, 5))
    
    # Loss
    plt.subplot(1, 3, 1)
    plt.plot(history.history['loss'], label='Treino', linewidth=2)
    plt.plot(history.history['val_loss'], label='Validação', linewidth=2)
    plt.title('Loss ao Longo do Treinamento', fontsize=14, fontweight='bold')
    plt.xlabel('Época')
    plt.ylabel('Loss')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Accuracy
    plt.subplot(1, 3, 2)
    plt.plot(history.history['accuracy'], label='Treino', linewidth=2)
    plt.plot(history.history['val_accuracy'], label='Validação', linewidth=2)
    plt.title('Accuracy ao Longo do Treinamento', fontsize=14, fontweight='bold')
    plt.xlabel('Época')
    plt.ylabel('Accuracy')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Learning Rate (se disponível)
    plt.subplot(1, 3, 3)
    if 'lr' in history.history:
        plt.plot(history.history['lr'], linewidth=2, color='orange')
        plt.title('Learning Rate', fontsize=14, fontweight='bold')
        plt.xlabel('Época')
        plt.ylabel('LR')
        plt.yscale('log')
        plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('models/training_history.png', dpi=150)
    print("\n📈 Gráfico salvo: models/training_history.png")

def evaluate_model(model, X_test, y_test):
    """
    Avalia modelo no conjunto de teste
    """
    print("\n🎯 Avaliando modelo no conjunto de teste...")
    
    test_loss, test_acc = model.evaluate(X_test, y_test, verbose=0)
    
    print(f"✅ Loss: {test_loss:.4f}")
    print(f"✅ Accuracy: {test_acc:.4f} ({test_acc*100:.2f}%)")
    
    # Predições
    y_pred = model.predict(X_test, verbose=0)
    y_pred_classes = np.argmax(y_pred, axis=1)
    
    # Matriz de confusão simplificada
    from sklearn.metrics import classification_report
    
    print("\n📊 Relatório de Classificação:")
    print(classification_report(y_test, y_pred_classes, target_names=CLASS_NAMES))
    
    return test_acc

def convert_to_tflite(model):
    """
    Converte para TensorFlow Lite
    """
    print("\n🔄 Convertendo para TensorFlow Lite...")
    
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    
    # Otimizações
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    
    # Converter
    tflite_model = converter.convert()
    
    # Salvar
    tflite_path = 'models/neura9_model.tflite'
    with open(tflite_path, 'wb') as f:
        f.write(tflite_model)
    
    size_kb = len(tflite_model) / 1024
    print(f"✅ Modelo TFLite salvo: {tflite_path}")
    print(f"✅ Tamanho: {size_kb:.2f} KB")
    
    return tflite_model

def generate_cpp_file(tflite_model):
    """
    Gera arquivo C++ com o modelo
    """
    print("\n🔨 Gerando arquivo C++...")
    
    cpp_code = f"""// Modelo NEURA9 Defense AI - Gerado Automaticamente
// Data: {tf.timestamp()}
// Tamanho: {len(tflite_model)} bytes ({len(tflite_model)/1024:.2f} KB)

#include <cstdint>

// Modelo TensorFlow Lite
alignas(8) const unsigned char neura9_defense_model_tflite[] = {{
"""
    
    # Adicionar bytes
    for i in range(0, len(tflite_model), 16):
        chunk = tflite_model[i:i+16]
        hex_values = ', '.join(f'0x{b:02x}' for b in chunk)
        cpp_code += f"    {hex_values},\n"
    
    cpp_code += """};\n\n"""
    cpp_code += f"const unsigned int neura9_defense_model_tflite_len = {len(tflite_model)};\n"
    
    # Salvar
    cpp_path = '../src/ai/neura9_defense_model_data.cpp'
    with open(cpp_path, 'w') as f:
        f.write(cpp_code)
    
    print(f"✅ Arquivo C++ salvo: {cpp_path}")
    print(f"💡 Copie para o firmware e recompile!")

def main():
    """
    Função principal
    """
    print("=" * 60)
    print("🧠 NEURA9 Defense AI - Treinamento Completo")
    print("=" * 60)
    
    # Criar diretório de modelos
    os.makedirs('models', exist_ok=True)
    
    # 1. Gerar ou carregar dataset
    print("\n📦 Carregando dataset...")
    X, y = generate_synthetic_dataset(samples_per_class=300)
    
    print(f"✅ Dataset: {X.shape[0]} amostras, {X.shape[1]} features")
    print(f"✅ Classes: {np.unique(y)}")
    
    # 2. Split train/val/test
    X_train, X_temp, y_train, y_temp = train_test_split(
        X, y, test_size=0.3, random_state=42, stratify=y
    )
    X_val, X_test, y_val, y_test = train_test_split(
        X_temp, y_temp, test_size=0.5, random_state=42, stratify=y_temp
    )
    
    # 3. Normalização
    print("\n🔧 Normalizando features...")
    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_val = scaler.transform(X_val)
    X_test = scaler.transform(X_test)
    
    # Salvar scaler
    joblib.dump(scaler, 'models/scaler.pkl')
    print("✅ Scaler salvo: models/scaler.pkl")
    
    # 4. Treinar
    model, history = train_model(X_train, y_train, X_val, y_val)
    
    # 5. Plotar histórico
    plot_training_history(history)
    
    # 6. Avaliar
    test_acc = evaluate_model(model, X_test, y_test)
    
    # 7. Salvar modelo Keras
    model.save('models/neura9_final.h5')
    print("\n💾 Modelo Keras salvo: models/neura9_final.h5")
    
    # 8. Converter para TFLite
    tflite_model = convert_to_tflite(model)
    
    # 9. Gerar arquivo C++
    generate_cpp_file(tflite_model)
    
    print("\n" + "=" * 60)
    print("✅ TREINAMENTO CONCLUÍDO COM SUCESSO!")
    print("=" * 60)
    print(f"\n📊 Resultados Finais:")
    print(f"   Accuracy: {test_acc*100:.2f}%")
    print(f"   Tamanho: {len(tflite_model)/1024:.2f} KB")
    print(f"\n💡 Próximo passo: Copie o arquivo C++ para o firmware e recompile!")

if __name__ == '__main__':
    main()
