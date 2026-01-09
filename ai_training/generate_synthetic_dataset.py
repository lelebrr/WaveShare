#!/usr/bin/env python3
"""
═══════════════════════════════════════════════════════════════════════════
WAVEPWN - NEURA9 AI Training Pipeline
Gerador de dataset sintético para classificação de ameaças WiFi
═══════════════════════════════════════════════════════════════════════════
"""

import numpy as np
import random
import json
from datetime import datetime

# Threat categories
THREAT_CLASSES = [
    'NORMAL',           # 0 - Tráfego normal
    'DEAUTH',           # 1 - Ataque de desautenticação
    'EVIL_TWIN',        # 2 - Evil Twin AP
    'PROBE_FLOOD',      # 3 - Flood de Probe Requests
    'BEACON_SPAM',      # 4 - Beacon Spam
    'ROGUE_AP',         # 5 - AP não autorizado
    'KARMA_ATTACK',     # 6 - Karma/Mana attack
    'WPS_ATTACK',       # 7 - Ataque WPS
    'UNKNOWN'           # 8 - Desconhecido
]

NUM_CLASSES = len(THREAT_CLASSES)
FEATURE_SIZE = 32  # Tamanho do vetor de features

def generate_normal_traffic():
    """Gera features de tráfego normal"""
    return {
        'beacon_rate': random.uniform(8.0, 12.0),       # ~10 beacons/s normal
        'probe_rate': random.uniform(0.1, 2.0),         # Poucas probes
        'deauth_rate': 0.0,                              # Sem deauths
        'data_rate': random.uniform(50, 200),           # Tráfego de dados
        'mgmt_ratio': random.uniform(0.05, 0.15),       # 5-15% management
        'unique_macs': random.randint(5, 50),           # Vários dispositivos
        'rssi_variance': random.uniform(5, 15),         # Variância normal
        'channel_hops': 0,                               # Sem mudanças
        'ssid_len': random.randint(4, 32),              # SSID normal
        'hidden_ssid': random.random() < 0.1,           # ~10% hidden
        'wps_active': random.random() < 0.3,            # ~30% com WPS
        'encryption': random.choice(['WPA2', 'WPA3']),
    }

def generate_deauth_attack():
    """Gera features de ataque de desautenticação"""
    return {
        'beacon_rate': random.uniform(8.0, 12.0),
        'probe_rate': random.uniform(0.1, 2.0),
        'deauth_rate': random.uniform(50, 500),         # ALTO - muitos deauths
        'data_rate': random.uniform(0, 50),             # Baixo tráfego
        'mgmt_ratio': random.uniform(0.8, 0.95),        # Majoritariamente mgmt
        'unique_macs': random.randint(1, 5),            # Poucos MACs
        'rssi_variance': random.uniform(1, 5),          # Baixa variância
        'channel_hops': 0,
        'ssid_len': random.randint(4, 32),
        'hidden_ssid': False,
        'wps_active': False,
        'encryption': 'WPA2',
    }

def generate_evil_twin():
    """Gera features de Evil Twin"""
    return {
        'beacon_rate': random.uniform(15, 30),          # Beacons mais frequentes
        'probe_rate': random.uniform(0.1, 2.0),
        'deauth_rate': random.uniform(0, 10),           # Pode ter alguns deauths
        'data_rate': random.uniform(10, 100),
        'mgmt_ratio': random.uniform(0.2, 0.4),
        'unique_macs': random.randint(1, 3),            # Poucos MACs (clonados)
        'rssi_variance': random.uniform(20, 40),        # Alta variância (2 APs)
        'channel_hops': 0,
        'ssid_len': random.randint(4, 32),
        'hidden_ssid': False,
        'wps_active': False,
        'encryption': random.choice(['OPEN', 'WPA2']),  # Pode ser aberto
    }

def generate_probe_flood():
    """Gera features de Probe Request Flood"""
    return {
        'beacon_rate': random.uniform(8.0, 12.0),
        'probe_rate': random.uniform(100, 1000),        # ALTO - muitas probes
        'deauth_rate': 0,
        'data_rate': random.uniform(0, 20),
        'mgmt_ratio': random.uniform(0.9, 0.99),        # Quase só probes
        'unique_macs': random.randint(50, 200),         # Muitos MACs fake
        'rssi_variance': random.uniform(1, 5),
        'channel_hops': random.randint(5, 14),          # Vários canais
        'ssid_len': random.randint(1, 32),
        'hidden_ssid': False,
        'wps_active': False,
        'encryption': 'NONE',
    }

def generate_beacon_spam():
    """Gera features de Beacon Spam"""
    return {
        'beacon_rate': random.uniform(100, 500),        # ALTO - muitos beacons
        'probe_rate': random.uniform(0.1, 2.0),
        'deauth_rate': 0,
        'data_rate': 0,
        'mgmt_ratio': random.uniform(0.95, 0.99),       # Quase só beacons
        'unique_macs': random.randint(20, 100),         # Muitos APs fake
        'rssi_variance': random.uniform(1, 3),          # Mesmo dispositivo
        'channel_hops': 0,
        'ssid_len': random.randint(1, 32),              # SSIDs variados
        'hidden_ssid': False,
        'wps_active': False,
        'encryption': random.choice(['OPEN', 'WPA2']),
    }

def generate_rogue_ap():
    """Gera features de Rogue AP"""
    return {
        'beacon_rate': random.uniform(8.0, 15.0),
        'probe_rate': random.uniform(0.1, 5.0),
        'deauth_rate': 0,
        'data_rate': random.uniform(10, 100),
        'mgmt_ratio': random.uniform(0.1, 0.3),
        'unique_macs': random.randint(1, 10),
        'rssi_variance': random.uniform(5, 15),
        'channel_hops': 0,
        'ssid_len': random.randint(4, 32),
        'hidden_ssid': random.random() < 0.3,
        'wps_active': random.random() < 0.5,
        'encryption': 'OPEN',                           # Geralmente aberto
    }

def generate_karma_attack():
    """Gera features de Karma Attack"""
    return {
        'beacon_rate': random.uniform(50, 200),         # Responde a tudo
        'probe_rate': random.uniform(10, 50),
        'deauth_rate': 0,
        'data_rate': random.uniform(5, 50),
        'mgmt_ratio': random.uniform(0.6, 0.8),
        'unique_macs': random.randint(1, 3),
        'rssi_variance': random.uniform(1, 5),
        'channel_hops': 0,
        'ssid_len': random.randint(1, 32),              # SSIDs dinâmicos
        'hidden_ssid': False,
        'wps_active': False,
        'encryption': 'OPEN',
    }

def generate_wps_attack():
    """Gera features de WPS Attack"""
    return {
        'beacon_rate': random.uniform(8.0, 12.0),
        'probe_rate': random.uniform(0.1, 2.0),
        'deauth_rate': 0,
        'data_rate': random.uniform(10, 50),
        'mgmt_ratio': random.uniform(0.2, 0.4),
        'unique_macs': random.randint(1, 3),
        'rssi_variance': random.uniform(1, 10),
        'channel_hops': 0,
        'ssid_len': random.randint(4, 32),
        'hidden_ssid': False,
        'wps_active': True,                             # WPS sempre ativo
        'encryption': 'WPA2',
        'wps_attempts': random.randint(100, 1000),      # Muitas tentativas
    }

# Generators por classe
GENERATORS = {
    0: generate_normal_traffic,
    1: generate_deauth_attack,
    2: generate_evil_twin,
    3: generate_probe_flood,
    4: generate_beacon_spam,
    5: generate_rogue_ap,
    6: generate_karma_attack,
    7: generate_wps_attack,
    8: generate_normal_traffic,  # Unknown usa normal como base
}

def features_to_vector(features):
    """Converte dict de features para vetor normalizado de 32 elementos"""
    vector = np.zeros(FEATURE_SIZE, dtype=np.float32)
    
    # Normalização para [0, 1]
    vector[0] = min(features.get('beacon_rate', 0) / 500, 1.0)
    vector[1] = min(features.get('probe_rate', 0) / 1000, 1.0)
    vector[2] = min(features.get('deauth_rate', 0) / 500, 1.0)
    vector[3] = min(features.get('data_rate', 0) / 200, 1.0)
    vector[4] = features.get('mgmt_ratio', 0)
    vector[5] = min(features.get('unique_macs', 0) / 200, 1.0)
    vector[6] = min(features.get('rssi_variance', 0) / 40, 1.0)
    vector[7] = min(features.get('channel_hops', 0) / 14, 1.0)
    vector[8] = min(features.get('ssid_len', 0) / 32, 1.0)
    vector[9] = 1.0 if features.get('hidden_ssid', False) else 0.0
    vector[10] = 1.0 if features.get('wps_active', False) else 0.0
    vector[11] = {'OPEN': 0.0, 'WEP': 0.25, 'WPA': 0.5, 'WPA2': 0.75, 'WPA3': 1.0, 'NONE': 0.0}.get(features.get('encryption', 'OPEN'), 0.0)
    vector[12] = min(features.get('wps_attempts', 0) / 1000, 1.0)
    
    # Adiciona ruído para tornar mais realista
    noise = np.random.normal(0, 0.02, FEATURE_SIZE)
    vector = np.clip(vector + noise, 0, 1)
    
    return vector

def generate_dataset(samples_per_class=1000):
    """Gera dataset completo"""
    X = []
    y = []
    
    for class_id in range(NUM_CLASSES):
        generator = GENERATORS.get(class_id, generate_normal_traffic)
        
        for _ in range(samples_per_class):
            features = generator()
            vector = features_to_vector(features)
            X.append(vector)
            y.append(class_id)
    
    X = np.array(X, dtype=np.float32)
    y = np.array(y, dtype=np.int32)
    
    # Shuffle
    indices = np.random.permutation(len(X))
    X = X[indices]
    y = y[indices]
    
    return X, y

def save_dataset(X, y, prefix='neura9_dataset'):
    """Salva dataset em formato numpy e JSON"""
    np.save(f'{prefix}_X.npy', X)
    np.save(f'{prefix}_y.npy', y)
    
    # Salva metadata
    metadata = {
        'created': datetime.now().isoformat(),
        'samples': len(X),
        'features': FEATURE_SIZE,
        'classes': NUM_CLASSES,
        'class_names': THREAT_CLASSES,
        'samples_per_class': len(X) // NUM_CLASSES,
    }
    
    with open(f'{prefix}_metadata.json', 'w') as f:
        json.dump(metadata, f, indent=2)
    
    print(f"Dataset salvo: {len(X)} amostras, {FEATURE_SIZE} features, {NUM_CLASSES} classes")

if __name__ == '__main__':
    print("Gerando dataset NEURA9...")
    X, y = generate_dataset(samples_per_class=2000)
    save_dataset(X, y)
    print("Dataset gerado com sucesso!")
