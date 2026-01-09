# quantize_neura9_int8.py
# Rodar com Python 3.9+ (recomendo venv)
# pip install tensorflow==2.16.1 numpy

import tensorflow as tf
import numpy as np
import pathlib
import os

# ================================================================
# CONFIGURAÇÕES (ajuste só aqui)
# ================================================================
MODEL_INPUT_PATH   = "ai_training/models/neura9_float32.tflite"   # seu modelo atual
MODEL_OUTPUT_PATH  = "src/neura9/neura9_int8.tflite"              # onde o ESP32 vai ler
REP_DATA_PATH      = "ai_training/dataset/representative_data/"   # pasta com .npy de amostras reais

# ================================================================
# 1. Carrega o modelo float original
# ================================================================
print("[1/5] Carregando modelo float32...")
converter = tf.lite.TFLiteConverter.from_tflite_model_file(MODEL_INPUT_PATH)
# Forçar controle total
converter.experimental_new_converter = True

# ================================================================
# 2. Dataset representativo (OBRIGATÓRIO para boa precisão)
# ================================================================
def representative_data_gen():
    """Yield 100–300 amostras reais do que o modelo vê no dispositivo"""
    files = list(pathlib.Path(REP_DATA_PATH).glob("*.npy"))
    if len(files) == 0:
        raise FileNotFoundError(f"Nenhum .npy encontrado em {REP_DATA_PATH}")
    
    print(f"[2/5] Usando {len(files)} amostras representativas para calibração...")
    for path in files[:300]:  # máximo 300 é suficiente
        sample = np.load(path).astype(np.float32)
        # Garante shape correta: (1, timesteps, features)
        sample = sample.reshape(1, *sample.shape)
        yield [sample]

converter.representative_dataset = representative_data_gen

# ================================================================
# 3. Configura quantização FULL INT8 (pesos + ativações)
# ================================================================
print("[3/5] Configurando quantização INT8 completa...")
converter.optimizations = [tf.lite.Optimize.DEFAULT]

# Força todas as ops para int8 (incluindo input/output)
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

# Input/Output devem ser int8 também
converter.inference_input_type  = tf.int8   # ou tf.uint8 se preferir
converter.inference_output_type = tf.int8

# ================================================================
# 4. Converte
# ================================================================
print("[4/5] Convertendo... (pode demorar 10–40 segundos)")
try:
    quantized_model = converter.convert()
except Exception as e:
    print("ERRO na conversão:", e)
    print("Dica: seu modelo tem op não suportada por int8? Veja erros acima.")
    exit(1)

# ================================================================
# 5. Salva modelo + array C para o ESP32
# ================================================================
print(f"[5/5] Salvando modelo quantizado → {MODEL_OUTPUT_PATH}")
os.makedirs(os.path.dirname(MODEL_OUTPUT_PATH), exist_ok=True)
with open(MODEL_OUTPUT_PATH, "wb") as f:
    f.write(quantized_model)

# Gera o .h automaticamente (pronto para colar em src/neura9/model.h)
model_size = len(quantized_model)
print(f"Modelo INT8 gerado: {model_size / 1024:.1f} KB (economia de ~75%)")

header_path = "src/neura9/model_quantized.h"
with open(header_path, "w") as f:
    f.write("// Modelo NEURA9 quantizado INT8 - gerado automaticamente\n")
    f.write("// NÃO EDITE MANUALMENTE!\n\n")
    f.write("#pragma once\n\n")
    f.write("#include <cstdint>\n\n")
    f.write(f"alignas(8) const uint8_t g_model[] = {{\n")
    for i, byte in enumerate(quantized_model):
        if i % 12 == 0:
            f.write("  ")
        f.write(f"0x{byte:02x}")
        f.write("," if i < model_size - 1 else "\n")
        if i % 12 == 11:
            f.write("\n")
    f.write("};\n")
    f.write(f"const unsigned int g_model_len = {model_size};\n")

print(f"Cabeçalho C++ gerado → {header_path}")
print("PRONTO! Agora faça:")
print("   1. Substitua src/neura9/model.h pelo novo model_quantized.h")
print("   2. Em src/ai/inference.cpp, troque:")
print("      #include \"model.h\"  →  #include \"model_quantized.h\"")
print("   3. Recompile e teste!")

# ================================================================
# DICA EXTRA: Como gerar as amostras representativas (uma vez só)
# ================================================================
print("\nPara melhores resultados, gere amostras reais assim:")
print("   - Use o device em modo captura por 30 minutos")
print("   - Salve todos os vetores de features antes da inferência em /sd/raw_features/")
print("   - Rode o script abaixo para converter para .npy:")
print("       python tools/convert_raw_to_npy.py")
