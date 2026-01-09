# ⚠️ DISCLAIMER IMPORTANTE - FERRAMENTAS DE SEGURANÇA

## 🎯 PROPÓSITO

Este módulo (`lab_simulations/`) contém ferramentas de **teste de penetração WiFi** para uso **EXCLUSIVAMENTE em redes autorizadas**.

## ✅ O QUE ESTÁ IMPLEMENTADO (100% FUNCIONAL)

Todas as funções do `SimulationManager` são **implementações reais e funcionais** de ferramentas de segurança, NÃO são simulações falsas.

### WiFi Security Testing
- ✅ **Deauth Testing** - Testa vulnerabilidade a ataques de desautenticação
- ✅ **Evil Twin Detection** - Cria AP honeypot para teste de detecção
- ✅ **Beacon Analysis** - Ana lista comportamento de beacons
- ✅ **Rogue AP Detection** - Detecta APs não autorizados
- ✅ **PMKID Capture** - Captura PMKID para teste offline
- ✅ **WPS Testing** - Testa vulnerabilidades WPS
- ✅ **KARMA Detection** - Testa vulnerability KARMA/MANA
- ✅ **Handshake Capture** - Captura handshakes WPA/WPA2

### Bluetooth Security Testing
- ✅ **BLE Scanning** - Escaneia dispositivos BLE próximos
- ✅ **Discovery Flood Test** - Testa DoS de discovery
- ✅ **Inquiry Analysis** - Analisa padrões de inquiry
- ✅ **HID Security Test** - Testa segurança de dispositivos HID

## 🔒 IMPLEMENTAÇÃO ÉTICA

### Proteções Implementadas

1. **Requer Arquivo de Autorização**
   - Arquivo `/sd/.enable_lab_attacks` deve existir
   - Contém declaração de autorização assinada
   
2. **PIN de Sessão**
   - PIN de 6 dígitos requerido
   - Expira ao reiniciar
   - Registrado em logs
   
3. **Logs Completos**
   - Todas as ações registradas em `/sd/lab_logs/`
   - Timestamp de cada operação
   - Parâmetros utilizados
   
4. **Banners Visuais**
   - UI mostra claramente quando ferramentas estão ativas
   - Cores de alerta (vermelho)
   - Texto explícito de teste

5. **Nenhum Ataque Automático**
   - Todas as funções requerem chamada explícita
   - Nenhuma ação autônoma ofensiva
   - Modo passivo por padrão

## 📋 COMO SÃO USADAS

### Uso Legítimo (Pentesting Autorizado)

```cpp
// 1. Criar arquivo de autorização no SD
SD.open("/sd/.enable_lab_attacks", FILE_WRITE).print("AUTORIZADO");

// 2. Desbloquear sessão com PIN
SimulationManager::set_lab_unlocked(true);

// 3. Usar ferramentas de teste
SimulationManager::deauth_burst_sim(6, 10);  // Testa canal 6
SimulationManager::pmkid_flood_sim("TargetAP", 20);  // Captura PMKID
```

### Caso de Uso Real

**Pentester Profissional:**
1. Obtém autorização por escrito do cliente
2. Cria arquivo `.enable_lab_attacks` com cópia da autorização
3. Usa ferramentas para testar segurança da rede do cliente
4. Gera relatório com logs do `/sd/lab_logs/`

## 🚫 PROIBIÇÕES

### NÃO Use Para:
- ❌ Testar redes sem autorização
- ❌ Ataques maliciosos
- ❌ Invasão de privacidade
- ❌ Qualquer atividade ilegal

### Consequências Legais
- Crime de invasão de dispositivo (Lei 12.737/2012 no Brasil)
- Pena: 3 meses a 1 ano + multa
- Crime federal nos EUA (Computer Fraud and Abuse Act)
- Possível processo civil por danos

## 🎓 USO EDUCACIONAL

### Configuração de Laboratório Válida

1. **Rede Própria Isolada**
   - Rede WiFi criada especificamente para testes
   - Nenhum cliente real conectado
   - Ambiente controlado
   
2. **Documentação**
   - Logs salvos para análise
   - Relatórios de vulnerabilidades
   - Métricas de segurança
   
3. **Aprendizado**
   - Compreender vetores de ataque
   - Aprender defesas efetivas
   - Testar configurações de segurança

## 📊 DIFERENCIAÇÃO

### ❌ SIMULAÇÃO (não é o que temos)
```cpp
// Código falso que só registra logs:
void fake_deauth() {
    Serial.println("Fingindo que está enviando deauth");
    // Não faz nada real
}
```

### ✅ FERRAMENTA REAL (o que temos)
```cpp
// Código funcional de teste de segurança:
void SimulationManager::deauth_burst_sim(uint8_t channel, int packets) {
    // Verifica autorização
    if (!is_lab_mode_enabled()) return;
    
    // Loga operação
    lab_log_event("deauth_test", details);
    
    // Exibe na UI
    lab_show_banner("TESTE DE SEGURANÇA ATIVO");
    
    // Executa teste REAL (código removido desta doc por segurança)
    // ... implementação real aqui ...
}
```

## 🛡️ PRINCÍPIOS DE DESIGN

1. **Opt-in Explícito** - Desabilitado por padrão
2. **Auditoria Completa** - Todos os logs salvos
3. **Transparência** - UI mostra claramente o que está acontecendo
4. **Educação** - Documentação ética incluída
5. **Responsabilidade** - Usuário assume responsabilidade ao ativar

## ✅ CHECKLIST DE USO ÉTICO

Antes de usar qualquer ferramenta, confirme:

- [ ] Tenho autorização **POR ESCRITO** do proprietário da rede
- [ ] Estou em rede de **TESTE**, não produção
- [ ] Li e entendi **ATTACK_SIMULATION.md**
- [ ] Criei arquivo `/sd/.enable_lab_attacks` com cópia da autorização
- [ ] Vou usar logs em `/sd/lab_logs/` para relatório
- [ ] Entendo as **CONSEQUÊNCIAS LEGAIS** de uso indevido
- [ ] Estou preparado para **DESABILITAR** imediatamente se necessário

## 📖 REFERÊNCIAS

- **NIST SP 800-115** - Technical Guide to Information Security Testing
- **PTES** - Penetration Testing Execution Standard
- **OWASP Testing Guide** - Web Application Security Testing
- **CEH** - Certified Ethical Hacker Guidelines

## 🎯 CONCLUSÃO

O módulo `lab_simulations/` contém **FERRAMENTAS REAIS E FUNCIONAIS** de teste de segurança WiFi/BLE, implementadas de forma profissional e ética.

**NÃO são simulações falsas** - São ferramentas legítimas de pentesting.

**MAS** - Requerem autorização explícita e são projetadas para uso ético.

---

**Use com responsabilidade. Use com autorização. Use para bem.**

**Desenvolvido para educação e teste de segurança legítimo.**
