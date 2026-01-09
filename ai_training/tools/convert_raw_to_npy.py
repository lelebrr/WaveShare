# convert_raw_to_npy.py
# Converte os vetores de features salvos no SD (/sd/raw_features/) para .npy
# Uso: python convert_raw_to_npy.py "/caminho/para/o/cartao_sd"

import argparse
import os
import numpy as np
import struct
from pathlib import Path

def parse_args():
    parser = argparse.ArgumentParser(
        description="Converte arquivos .raw salvos pelo LeleWatch em .npy para quantização INT8"
    )
    parser.add_argument(
        "sd_path",
        type=str,
        help="Caminho da raiz do cartão SD (ex: /Volumes/SD_CARD ou D:\\)"
    )
    parser.add_argument(
        "--output",
        type=str,
        default="ai_training/dataset/representative_data",
        help="Pasta onde salvar os .npy (default: ai_training/dataset/representative_data)"
    )
    parser.add_argument(
        "--max_files",
        type=int,
        default=500,
        help="Máximo de amostras a converter (default: 500)"
    )
    return parser.parse_args()

def main():
    args = parse_args()
    sd_path = Path(args.sd_path)
    raw_folder = sd_path / "raw_features"
    output_folder = Path(args.output)

    if not raw_folder.exists():
        print(f"ERRO: Pasta 'raw_features' não encontrada em {sd_path}")
        print("   Crie a pasta /raw_features/ no SD e ative o debug de features no firmware!")
        exit(1)

    raw_files = sorted(raw_folder.glob("feature_*.raw"))
    if len(raw_files) == 0:
        print("Nenhum arquivo feature_*.raw encontrado!")
        print("   No firmware, ative em core/config.h: #define DEBUG_SAVE_RAW_FEATURES 1")
        exit(1)

    print(f"Encontrados {len(raw_files)} arquivos .raw")
    print(f"Convertendo até {args.max_files} amostras → {output_folder}")

    output_folder.mkdir(parents=True, exist_ok=True)

    converted = 0
    for raw_file in raw_files:
        if converted >= args.max_files:
            break

        try:
            # ================================================
            # Formato esperado do .raw (definido no firmware):
            # 4 bytes  → timestamp (uint32)
            # 2 bytes  → número de timesteps (uint16)
            # 2 bytes  → número de features por timestep (uint16)
            # N bytes  → dados float32 (little-endian)
            # ================================================
            with open(raw_file, "rb") as f:
                data = f.read()

            if len(data) < 8:
                print(f"Pulando {raw_file.name} → muito pequeno")
                continue

            offset = 0
            timestamp = struct.unpack_from("<I", data, offset)[0]
            offset += 4
            timesteps = struct.unpack_from("<H", data, offset)[0]
            offset += 2
            features = struct.unpack_from("<H", data, offset)[0]
            offset += 2

            expected_floats = timesteps * features
            if len(data) < offset + expected_floats * 4:
                print(f"Pulando {raw_file.name} → dados incompletos")
                continue

            # Extrai os floats
            floats = struct.unpack_from(f"<{expected_floats}f", data, offset)
            array = np.array(floats, dtype=np.float32)
            array = array.reshape(timesteps, features)  # shape correta!

            # Salva como .npy
            npy_path = output_folder / f"sample_{converted:04d}.npy"
            np.save(npy_path, array)

            converted += 1
            if converted % 50 == 0:
                print(f"   → {converted} amostras convertidas...")

        except Exception as e:
            print(f"Erro processando {raw_file.name}: {e}")
            continue

    print(f"\nCONCLUÍDO!")
    print(f"   {converted} amostras salvas em:")
    print(f"   {output_folder.resolve()}")
    print(f"\nAgora você pode rodar:")
    print(f"   python ai_training/quantize_neura9_int8.py")
    print(f"\nDica: Quanto mais amostras reais (200–500), melhor fica a quantização INT8!")

if __name__ == "__main__":
    main()
