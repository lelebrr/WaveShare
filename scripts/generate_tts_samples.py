#!/usr/bin/env python3
"""
TTS Audio Generator for WaveShare Dragon Watch
Generates synthesized voice samples using Google TTS (gTTS)
and converts them to C header format for embedding in firmware.

Requirements:
    pip install gtts pydub

Usage:
    python generate_tts_samples.py

Output:
    tts_samples_real.h - Ready to replace src/voice/tts_samples.h
"""

from gtts import gTTS
from pydub import AudioSegment
import os
import struct

# Output configuration
OUTPUT_SAMPLE_RATE = 16000
OUTPUT_BITS = 8  # 8-bit unsigned PCM

# TTS Messages in Portuguese (Brazilian)
MESSAGES = {
    "HELLO": "Olá! Eu sou o Dragão, seu assistente.",
    "SCANNING": "Escaneando redes...",
    "NETWORKS_FOUND": "Redes encontradas!",
    "ATTACK_STARTED": "Ataque iniciado!",
    "ATTACK_STOPPED": "Ataque finalizado.",
    "HANDSHAKE_CAPTURED": "Handshake capturado!",
    "THREAT_ALERT": "Alerta! Ameaça detectada!",
    "STATUS_REPORT": "Relatório de status...",
    "LISTENING": "Estou ouvindo...",
    "CMD_NOT_RECOGNIZED": "Comando não reconhecido.",
    "OK": "OK!",
    "ERROR": "Erro!",
    "BLE_STARTED": "BLE Spam iniciado!",
    "GOODBYE": "Até logo!",
    "WAKE_WORD": "Sim, estou aqui!",
}

def generate_audio(text: str, output_path: str):
    """Generate TTS audio file"""
    tts = gTTS(text=text, lang='pt-br')
    mp3_path = output_path.replace('.raw', '.mp3')
    tts.save(mp3_path)
    
    # Convert to 16kHz mono 8-bit
    audio = AudioSegment.from_mp3(mp3_path)
    audio = audio.set_frame_rate(OUTPUT_SAMPLE_RATE)
    audio = audio.set_channels(1)
    audio = audio.set_sample_width(1)  # 8-bit
    
    # Export as raw
    audio.export(output_path, format='raw')
    
    # Cleanup MP3
    os.remove(mp3_path)
    
    return len(audio.raw_data)

def raw_to_c_array(raw_path: str, array_name: str) -> tuple:
    """Convert raw audio to C array string"""
    with open(raw_path, 'rb') as f:
        data = f.read()
    
    # Convert to unsigned 8-bit (center at 128)
    # gTTS outputs signed, we need unsigned for simpler playback
    unsigned_data = bytes([(b + 128) & 0xFF for b in data])
    
    lines = []
    for i in range(0, len(unsigned_data), 16):
        chunk = unsigned_data[i:i+16]
        hex_vals = ', '.join(f'0x{b:02X}' for b in chunk)
        lines.append(f'    {hex_vals},')
    
    return '\n'.join(lines), len(unsigned_data)

def main():
    print("🐉 WaveShare Dragon TTS Generator")
    print("=" * 50)
    
    # Create temp directory
    os.makedirs('temp_tts', exist_ok=True)
    
    samples = {}
    
    for key, text in MESSAGES.items():
        print(f"Generating: {key} - \"{text[:30]}...\"")
        raw_path = f'temp_tts/tts_{key.lower()}.raw'
        
        try:
            length = generate_audio(text, raw_path)
            c_array, size = raw_to_c_array(raw_path, f'TTS_{key}_DATA')
            samples[key] = (c_array, size)
            print(f"  ✓ {size} bytes generated")
        except Exception as e:
            print(f"  ✗ Error: {e}")
            samples[key] = ("    0x80, // Error placeholder", 1)
    
    # Generate header file
    print("\nGenerating tts_samples_real.h...")
    
    header = '''#pragma once
/**
 * @file tts_samples.h
 * @brief Pre-generated TTS audio samples (synthesized voice)
 * 
 * Audio format: 8-bit unsigned PCM, 16kHz, Mono
 * Generated using Google TTS (pt-BR)
 * 
 * AUTO-GENERATED FILE - Do not edit manually!
 */

#include <stdint.h>
#include <pgmspace.h>

'''
    
    # Add each sample
    for key, (c_array, size) in samples.items():
        header += f'''// ============================================================
// TTS: "{MESSAGES[key]}"
// ============================================================
const uint8_t TTS_{key}_DATA[] PROGMEM = {{
{c_array}
}};
const size_t TTS_{key}_LEN = {size};

'''
    
    # Add lookup table
    header += '''// ============================================================
// Lookup table for TTSMessage enum
// ============================================================
struct TTSSample {
    const uint8_t* data;
    size_t length;
};

const TTSSample TTS_SAMPLES[] = {
'''
    
    for key in MESSAGES.keys():
        header += f'    {{TTS_{key}_DATA, TTS_{key}_LEN}},\n'
    
    header += '''};

#define TTS_SAMPLE_COUNT (sizeof(TTS_SAMPLES) / sizeof(TTSSample))
'''
    
    with open('tts_samples_real.h', 'w') as f:
        f.write(header)
    
    print("✅ Done! Copy tts_samples_real.h to src/voice/tts_samples.h")
    
    # Cleanup
    import shutil
    shutil.rmtree('temp_tts')

if __name__ == '__main__':
    main()
