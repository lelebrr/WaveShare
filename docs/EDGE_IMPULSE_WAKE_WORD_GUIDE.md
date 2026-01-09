# 🐉 Guia: Treinar Wake Word "Hey Dragon" no Edge Impulse

## Pré-requisitos
1. Conta no [Edge Impulse](https://edgeimpulse.com)
2. Microfone para gravar amostras

---

## Passo 1: Criar Projeto

1. Acesse https://studio.edgeimpulse.com
2. Clique em **Create new project**
3. Nome: `HeyDragon_WakeWord`
4. Selecione: **Audio** as primary data type

---

## Passo 2: Coletar Dados

### Opção A: Gravar pelo navegador
1. Vá em **Data Acquisition**
2. Clique em **Let's collect some data**
3. Selecione **Built-in microphone**
4. Configure:
   - Label: `hey_dragon`
   - Sample length: 1000ms
   - Frequency: 16000 Hz

5. Grave pelo menos **50 amostras** dizendo "Hey Dragon" ou "Ei Dragão"
6. Repita para outros falantes se possível

### Opção B: Upload de arquivos
- Grave arquivos WAV 16kHz mono com ~1s cada
- Faça upload em **Data Acquisition > Upload data**

### Dados Negativos (Importante!)
1. Crie label `_noise` para ruído ambiente
2. Crie label `_unknown` para outras palavras
3. Grave pelo menos **100 amostras** de cada

---

## Passo 3: Criar Impulse (Pipeline de ML)

1. Vá em **Create impulse**
2. Configure:
   - **Time series data**: Window size = 1000ms, Window increase = 500ms
   - **Processing block**: Audio (MFCC)
   - **Learning block**: Classification

3. Clique **Save Impulse**

---

## Passo 4: Configurar MFCC

1. Vá em **MFCC**
2. Parâmetros recomendados:
   - Number of coefficients: 13
   - Frame length: 0.02
   - Frame stride: 0.01
   - Filter number: 32
   - FFT length: 256
   - Low frequency: 300
   - High frequency: 8000

3. Clique **Generate features**
4. Verifique que as classes estão bem separadas no Feature Explorer

---

## Passo 5: Treinar Modelo

1. Vá em **Classifier**
2. Configure:
   - Neural network architecture: **1D Conv** (padrão)
   - Number of training cycles: 100
   - Learning rate: 0.005
   - Data augmentation: ON

3. Clique **Start training**
4. Aguarde (5-10 minutos)

### Resultados esperados:
- Accuracy > 90%
- Confusion matrix sem muitos erros entre classes

---

## Passo 6: Exportar Modelo

1. Vá em **Deployment**
2. Selecione **TensorFlow Lite (int8 quantized)**
3. Clique **Build**
4. Download do arquivo `.zip`

---

## Passo 7: Converter para Header C

1. Extraia o arquivo `model.tflite` do zip
2. Execute no terminal:

```bash
xxd -i model.tflite > wake_word_model_real.h
```

3. Edite o arquivo para:
   - Adicionar `#pragma once`
   - Adicionar `#include <pgmspace.h>`
   - Adicionar `PROGMEM` após o array

---

## Passo 8: Integrar no Projeto

1. Copie `wake_word_model_real.h` para `src/ai/models/wake_word_model.h`
2. Ajuste os defines se necessário:

```cpp
#define WAKE_WORD_MFCC_FEATURES 13  // Conforme configurado no Edge Impulse
#define WAKE_WORD_TIME_STEPS 49     // Depende do window size
#define WAKE_WORD_THRESHOLD 0.85f   // Ajuste conforme testes
```

---

## Dicas

- **Mais dados = melhor modelo**
- Grave em diferentes ambientes (silencioso, barulhento)
- Varie pronúncia, velocidade, tom de voz
- Teste o modelo no Edge Impulse antes de exportar (Live Classification)

---

## Recursos

- [Edge Impulse Docs](https://docs.edgeimpulse.com)
- [Tutorial Keyword Spotting](https://docs.edgeimpulse.com/docs/tutorials/responding-to-your-voice)
- [ESP32 Deployment Guide](https://docs.edgeimpulse.com/docs/deployment/running-your-impulse-esp32)
