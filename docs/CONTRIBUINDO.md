<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Contribuições-Bem--vindas-brightgreen?style=for-the-badge" alt="Contributions"/>
  <img src="https://img.shields.io/badge/PRs-Welcome-blue?style=for-the-badge" alt="PRs"/>
  <img src="https://img.shields.io/badge/Código-C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++"/>
</p>

---

# 🤝 GUIA DE CONTRIBUIÇÃO - WavePwn

Obrigado por considerar contribuir com o WavePwn! Este guia ajudará você a começar.

---

## 📋 Índice

1. [Código de Conduta](#-código-de-conduta)
2. [Como Contribuir](#-como-contribuir)
3. [Configurando o Ambiente](#-configurando-o-ambiente)
4. [Padrões de Código](#-padrões-de-código)
5. [Processo de Pull Request](#-processo-de-pull-request)
6. [Reportando Bugs](#-reportando-bugs)
7. [Sugerindo Features](#-sugerindo-features)

---

## 📜 Código de Conduta

### ✅ Comportamento Esperado
- Seja respeitoso e inclusivo
- Aceite críticas construtivas
- Foque no que é melhor para a comunidade
- Mostre empatia com outros contribuidores

### ❌ Comportamento Inaceitável
- Uso de linguagem ou imagens ofensivas
- Trolling ou comentários depreciativos
- Assédio público ou privado
- Publicar informações privadas de outros

---

## 🚀 Como Contribuir

### 1. 🍴 Fork o Repositório
```bash
# Clone seu fork
git clone https://github.com/SEU-USERNAME/wavepwn.git
cd wavepwn
```

### 2. 🌿 Crie uma Branch
```bash
# Para features
git checkout -b feature/nome-da-feature

# Para bugfixes
git checkout -b fix/descricao-do-bug

# Para docs
git checkout -b docs/descricao
```

### 3. 💻 Faça suas Mudanças
- Siga os padrões de código
- Adicione testes quando possível
- Atualize documentação relevante

### 4. ✅ Commit
```bash
git add .
git commit -m "feat: adiciona nova funcionalidade X"
```

### 5. 📤 Push e PR
```bash
git push origin feature/nome-da-feature
```
Então abra um Pull Request no GitHub.

---

## 🛠️ Configurando o Ambiente

### Requisitos
| Ferramenta | Versão | Propósito |
|------------|--------|-----------|
| PlatformIO | Latest | Build system |
| Python | 3.8+ | Scripts de treinamento |
| VS Code | Latest | IDE recomendada |
| Git | Latest | Controle de versão |

### Instalação
```bash
# 1. Instale PlatformIO CLI
pip install platformio

# 2. Clone o repositório
git clone https://github.com/SEU-USERNAME/wavepwn.git
cd wavepwn

# 3. Compile
pio run

# 4. Upload (com hardware conectado)
pio run -t upload
```

---

## 📝 Padrões de Código

### C++ Style Guide

```cpp
// ✅ Nomes de classes em PascalCase
class PluginManager {

// ✅ Nomes de funções em camelCase
void updateStatus();

// ✅ Nomes de variáveis em snake_case
int battery_percent;

// ✅ Constantes em SCREAMING_SNAKE_CASE
#define MAX_NETWORKS 50

// ✅ Comentários em português ou inglês
// Atualiza o status da bateria
void updateBattery();
};
```

### Estrutura de Arquivos
```
src/
├── modulo/
│   ├── modulo.h      # Header com declarações
│   └── modulo.cpp    # Implementação
```

### Commits
Siga [Conventional Commits](https://www.conventionalcommits.org/):

| Prefixo | Uso |
|---------|-----|
| `feat:` | Nova feature |
| `fix:` | Correção de bug |
| `docs:` | Documentação |
| `style:` | Formatação |
| `refactor:` | Refatoração |
| `test:` | Testes |
| `chore:` | Manutenção |

**Exemplos:**
```
feat: adiciona suporte a WPA3
fix: corrige crash no scan WiFi
docs: atualiza README com badges
```

---

## 🔄 Processo de Pull Request

### Checklist
- [ ] Código segue os padrões do projeto
- [ ] Testes passam localmente
- [ ] Documentação atualizada
- [ ] Commit messages seguem convenção
- [ ] Não quebra funcionalidades existentes

### Template de PR
```markdown
## Descrição
Breve descrição das mudanças.

## Tipo de Mudança
- [ ] Bug fix
- [ ] Nova feature
- [ ] Breaking change
- [ ] Documentação

## Como Testar
1. Passo 1
2. Passo 2

## Screenshots (se aplicável)
```

---

## 🐛 Reportando Bugs

### Template de Issue
```markdown
## Descrição do Bug
Descrição clara do problema.

## Passos para Reproduzir
1. Vá para '...'
2. Clique em '...'
3. Veja o erro

## Comportamento Esperado
O que deveria acontecer.

## Screenshots
Se aplicável.

## Ambiente
- Firmware Version: [e.g. 2.1.0]
- Hardware: [e.g. Waveshare ESP32-S3-Touch-AMOLED-1.8]
- PlatformIO Version: [e.g. 6.9.0]
```

---

## 💡 Sugerindo Features

### Template de Feature Request
```markdown
## Descrição da Feature
Descrição clara da funcionalidade.

## Problema que Resolve
Qual problema esta feature resolve?

## Solução Proposta
Como você imagina a solução?

## Alternativas Consideradas
Outras abordagens pensadas.
```

---

## 🏆 Reconhecimento

Contribuidores são reconhecidos no README principal e na seção de Agradecimentos!

---

## 📞 Dúvidas?

- 📖 Leia a [documentação](./README.md)
- 🔍 Procure issues existentes
- 💬 Abra uma nova issue

---

**Obrigado por contribuir! 🙏**

*"Juntos construímos ferramentas melhores"* 🛠️
