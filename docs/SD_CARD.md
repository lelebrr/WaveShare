<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Tutorial-Passo%20a%20Passo-blue?style=for-the-badge" alt="Tutorial"/>
  <img src="https://img.shields.io/badge/Nível-Fácil-brightgreen?style=for-the-badge" alt="Easy"/>
  <img src="https://img.shields.io/badge/Tempo-5%20minutos-orange?style=for-the-badge" alt="Time"/>
</p>

# 💾 GUIA COMPLETO: PREPARANDO O CARTÃO SD

Este guia explica **exatamente** como preparar seu cartão MicroSD para funcionar no WavePwn. Siga os passos na ordem.

---

## 📋 Passo 1: O que você precisa

1.  **Cartão MicroSD**:
    *   **Tamanho:** Entre 4GB e 32GB (Recomendado). Cartões de 64GB/128GB funcionam, mas exigem formatação especial.
    *   **Velocidade:** Class 10 é o ideal.
    *   **Marca:** SanDisk, Samsung, Kingston (evite genéricos baratos que queimam fácil).
2.  **Leitor de Cartão**: Para conectar no PC.
3.  **Computador**: Windows, Mac ou Linux.

---

## 🧹 Passo 2: Formatação (CRÍTICO!)

O ESP32 **SÓ LÊ** o formato **FAT32**. Se estiver em exFAT ou NTFS, **NÃO VAI FUNCIONAR**.

### No Windows:
1.  Insira o cartão no PC.
2.  Abra o "Este Computador" (Meu Computador).
3.  Clique com botão direito na unidade do cartão SD.
4.  Selecione **[Formatar...]**.
5.  Em "Sistema de Arquivos", escolha **FAT32**.
    *   *Nota: Se seu cartão tem 64GB+, o Windows não mostra FAT32. Baixe o programa "GUIFormat" (Format32) para conseguir.*
6.  Em "Tamanho da unidade de alocação", deixe **Padrão** (ou 4096 bytes).
7.  Clique em **Iniciar**.

### No Mac:
1.  Abra o "Utilitário de Disco".
2.  Selecione o cartão SD na lateral.
3.  Clique em "Apagar".
4.  Formato: **MS-DOS (FAT)**.
5.  Esquema: **Master Boot Record (MBR)**.
6.  Clique em Apagar.

---

## � GUIA ESPECIAL: Cartões de 64GB, 128GB ou Maior

O Windows **NÃO** deixa você formatar cartões grandes (64GB+) em FAT32 nativamente. Ele só te dá opção de "exFAT" ou "NTFS". **Isso não funciona no ESP32!**

Para usar esses cartões, você precisa de um programa especial.

### 🛠️ Usando a ferramenta "GUIFormat" (Recomendado)

1.  **Baixe o GUIFormat**: [Clique aqui para baixar (fat32format)](http://ridgecrop.co.uk/index.htm?guiformat.htm) ou pesquise no Google por "GUIFormat".
    *   É um arquivo `.exe` simples, não precisa instalar.
2.  **Feche todas as janelas** do Windows Explorer que estejam mostrando o cartão SD.
3.  **Execute o `guiformat.exe`** como Administrador (Botão direito > Executar como Administrador).
4.  **Configure assim:**
    *   **Drive:** Selecione a letra do seu cartão SD (Cuidado para não escolher seu HD!).
    *   **Allocation Unit Size:** Selecione **32768**.
    *   **Volume Label:** Escreva `WAVEPWN`.
    *   **Quick Format:** Marque a caixinha.
5.  Clique em **Start**.
6.  O programa vai perguntar se tem certeza. Clique em **OK**.
7.  Pronto! Agora seu cartão de 64GB/128GB está em FAT32 e legível pelo ESP32.

### ⚠️ Notas Importantes para Cartões Grandes:
*   A formatação pode demorar um pouco mais se você não marcar "Quick Format".
*   Se der erro "Failed to open device", é porque alguma janela do Windows ou antivírus está acessando o cartão. Feche tudo e tente de novo.

---

## 📂 Estrutura de Arquivos

O LeleWatch espera a seguinte estrutura no SD:

```
/
├── config/             # Arquivos de conf (config.json)
├── lelewatch/          # Dados do sistema
│   ├── handshakes/     # Capturas .pcap
│   ├── pmkid/          # Capturas PMKID
│   └── logs/           # Logs do sistema
├── data/               # Assets
    ├── web/
    │   ├── assets/        <-- Imagens do site
    │   └── badges/        <-- Ícones de nível
    ├── logo/              <-- Logos de boot
    ├── sounds/
    │   ├── system/        <-- Sons de beep/click
    │   └── tts/           <-- Vozes da IA
    └── sprites/           <-- Animações dos Mascotes
        ├── system/
        ├── dragon/
        ├── lele/
        └── lisa/
```

> **Dica:** Você pode copiar a pasta `data` inteira do projeto do computador para a raiz do cartão SD. Mantenha os arquivos organizados.

---

## 📥 Passo 4: Colocando os Arquivos

Consulte o arquivo **[ASSETS.md](ASSETS.md)** para saber exatamente qual imagem vai em qual pasta. Resumo rápido:

1.  **Sons (.wav 16kHz):**
    *   Copie `click.wav`, `success.wav` para `data/sounds/system/`.
    *   Copie `hello.wav`, `goodbye.wav` para `data/sounds/tts/`.

2.  **Imagens (.png):**
    *   Copie as animações do dragão para `data/sprites/dragon/`.
    *   Copie ícones de bateria/wifi para `data/sprites/system/`.

---

## 🔌 Passo 5: Inserindo no Device

1.  **Desligue** o seu WavePwn/ESP32 (Tire da tomada/USB).
2.  Insira o cartão SD no slot atrás da tela.
    *   *O contato dourado costuma ficar virado para o lado da placa (PCB), não da tela.*
3.  Empurre até ouvir um **"Click"**.
4.  Ligue o dispositivo.

---

## ❓ Problemas Comuns (Troubleshooting)

### "SD Card Mount Failed"
*   **Causa:** Cartão mal inserido ou sujo.
*   **Solução:** Tire, limpe os contatos com borracha escolar e coloque de novo.

### "No SD Card attached"
*   **Causa:** Slot quebrado ou cartão incompatível.
*   **Solução:** Tente outro cartão menor (ex: 4GB ou 8GB).

### "Filesystem is not FAT32"
*   **Causa:** Você formatou como exFAT.
*   **Solução:** Volte ao Passo 2 e formate como FAT32.

### Meus sons parecem "Esquilos" (Rápido/Fino)
*   **Causa:** O arquivo WAV está em 44.1kHz ou Stereo.
*   **Solução:** Converta para **16kHz Mono** (Veja o guia de Áudio em ASSETS.md).

---

**Agora seu WavePwn tem memória infinita!** 🚀
