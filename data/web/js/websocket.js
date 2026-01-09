/**
 * ═══════════════════════════════════════════════════════════════════════════
 * LELEWATCH - WebSocket Connection Manager
 * ═══════════════════════════════════════════════════════════════════════════
 */

// ═══════════════════════════════════════════════════════════════════════════
// WEBSOCKET CONNECTION
// ═══════════════════════════════════════════════════════════════════════════
function initWebSocket() {
    const wsProtocol = window.location.protocol === 'https:' ? 'wss' : 'ws';
    const wsUrl = `${wsProtocol}://${window.location.hostname}/ws`;

    console.log(`[WebSocket] Connecting to ${wsUrl}...`);

    WavePwn.ws = new WebSocket(wsUrl);

    WavePwn.ws.onopen = () => {
        console.log('[WebSocket] Connected!');
        WavePwn.state.connected = true;
        updateConnectionStatus(true);
        addLog('Conectado ao LeleWatch Core', 'success');

        // Request initial state
        WavePwn.ws.send(JSON.stringify({ cmd: 'get_state' }));
    };

    WavePwn.ws.onclose = () => {
        console.log('[WebSocket] Disconnected');
        WavePwn.state.connected = false;
        updateConnectionStatus(false);
        addLog('Conexão perdida. Reconectando...', 'warning');

        // Reconnect after 2 seconds
        setTimeout(initWebSocket, 2000);
    };

    WavePwn.ws.onerror = (error) => {
        console.error('[WebSocket] Error:', error);
        addLog('Erro de conexão WebSocket', 'error');
    };

    WavePwn.ws.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data);
            handleWebSocketMessage(data);
        } catch (e) {
            console.error('[WebSocket] Parse error:', e);
        }
    };
}

function updateConnectionStatus(connected) {
    const statusEl = document.getElementById('connection-status');
    if (!statusEl) return;

    if (connected) {
        statusEl.classList.add('connected');
        statusEl.innerHTML = '<span class="dot"></span> Conectado';
    } else {
        statusEl.classList.remove('connected');
        statusEl.innerHTML = '<span class="dot"></span> Desconectado';
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// MESSAGE HANDLERS
// ═══════════════════════════════════════════════════════════════════════════
function handleWebSocketMessage(data) {
    // Stats update (periodic)
    if (data.uptime !== undefined) {
        updateStats({
            uptime: data.uptime,
            battery: data.battery || 0,
            temperature: data.temp || 0,
            networks: data.aps || 0,
            handshakes: data.hs || 0,
            pmkids: data.pmkid || 0,
            bleDevices: data.ble || 0,
            deauthsSent: data.deauths || 0,
            threat: data.ai || 'SAFE'
        });

        // Update signal chart with activity
        if (data.activity !== undefined) {
            updateChart('signal', data.activity);
        }
    }

    // Log message
    if (data.log) {
        addLog(data.log, data.logType || 'info');
    }

    // Network list update
    if (data.networks) {
        updateNetworkTable(data.networks);
    }

    // BLE devices list
    if (data.ble_devices) {
        updateBLEList(data.ble_devices);
    }

    // Dragon mascot update
    if (data.mascot) {
        updateDragonMood(data.mascot.mood, data.mascot.text);
    }

    // Terminal output
    if (data.term_out) {
        appendTerminalOutput(data.term_out);
    }

    // Attack status
    if (data.attack_status) {
        updateAttackStatus(data.attack_status);
    }

    // Channel data
    if (data.channels) {
        updateChannelChart(data.channels);
    }

    // Security types
    if (data.security_types) {
        updateSecurityChart(data.security_types);
    }

    // Achievement unlocked!
    if (data.achievement) {
        showAchievement(data.achievement);
    }

    // Handshake captured notification (Dica 16 - Confetti)
    if (data.handshake_captured) {
        showHandshakeNotification(data.handshake_captured);
        // Trigger confetti celebration!
        if (typeof celebrateHandshake === 'function') {
            celebrateHandshake();
        }
        // Send Discord notification if configured
        if (typeof sendDiscordNotification === 'function') {
            sendDiscordNotification(
                `🎉 Handshake capturado: **${data.handshake_captured.ssid}**`,
                'handshake'
            );
        }
    }

    // Mascot XP/Level update
    if (data.xp !== undefined || data.level !== undefined) {
        if (typeof updateMascotStats === 'function') {
            updateMascotStats(data);
        }
    }

    // Camera Chaos events
    if ((data.camera_found || data.camera_attack_result) && window.handleCameraResult) {
        window.handleCameraResult(data);
    }

    // Printer Chaos events
    if ((data.printer_found || data.printer_attack_log) && window.handlePrinterResult) {
        window.handlePrinterResult(data);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// UI UPDATE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════
function updateNetworkTable(networks) {
    const tbody = document.querySelector('#networks-table tbody');
    if (!tbody) return;

    tbody.innerHTML = '';

    networks.forEach((net, index) => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>${escapeHtml(net.ssid) || '<hidden>'}</td>
            <td class="font-mono">${net.bssid}</td>
            <td>${net.channel}</td>
            <td>${getSignalBars(net.rssi)} ${net.rssi}dBm</td>
            <td><span class="badge badge-${getSecurityClass(net.security)}">${net.security}</span></td>
            <td>
                <button class="btn btn-sm btn-danger" onclick="startDeauth('${net.bssid}')">⚡</button>
                <button class="btn btn-sm btn-primary" onclick="selectTarget('${net.bssid}', '${escapeHtml(net.ssid)}')">🎯</button>
            </td>
        `;
        tbody.appendChild(tr);
    });

    // Update target selector
    const selector = document.getElementById('target-selector');
    if (selector) {
        selector.innerHTML = '<option value="FF:FF:FF:FF:FF:FF">Broadcast (Todos)</option>';
        networks.forEach(net => {
            const opt = document.createElement('option');
            opt.value = net.bssid;
            opt.textContent = `${net.ssid || '<hidden>'} (${net.bssid})`;
            selector.appendChild(opt);
        });
    }
}

function updateBLEList(devices) {
    const list = document.getElementById('ble-list');
    if (!list) return;

    list.innerHTML = '';

    devices.forEach(device => {
        const li = document.createElement('li');
        li.innerHTML = `
            <span class="ble-name">${escapeHtml(device.name) || 'Desconhecido'}</span>
            <span class="ble-mac font-mono">${device.mac}</span>
            <span class="ble-rssi">${device.rssi}dBm</span>
        `;
        list.appendChild(li);
    });
}

function updateChannelChart(channels) {
    if (!WavePwn.charts.channel) return;
    WavePwn.charts.channel.data.datasets[0].data = channels;
    WavePwn.charts.channel.update();
}

function updateSecurityChart(types) {
    if (!WavePwn.charts.security) return;
    WavePwn.charts.security.data.datasets[0].data = [
        types.wpa2 || 0,
        types.wpa3 || 0,
        types.wep || 0,
        types.open || 0
    ];
    WavePwn.charts.security.update();
}

function updateAttackStatus(status) {
    const indicator = document.getElementById('attack-indicator');
    if (!indicator) return;

    if (status.active) {
        indicator.classList.add('active');
        indicator.textContent = `${status.type}: ${status.progress || 'ATIVO'}`;
    } else {
        indicator.classList.remove('active');
        indicator.textContent = 'Nenhum ataque ativo';
    }
}

function appendTerminalOutput(text) {
    const terminal = document.getElementById('terminal-output');
    if (!terminal) return;

    const line = document.createElement('div');
    line.textContent = text;
    terminal.appendChild(line);
    terminal.scrollTop = terminal.scrollHeight;
}

function showAchievement(achievement) {
    const toast = document.createElement('div');
    toast.className = 'achievement-toast';
    toast.innerHTML = `
        <div class="achievement-icon">🏆</div>
        <div class="achievement-content">
            <div class="achievement-title">${achievement.title}</div>
            <div class="achievement-desc">${achievement.description}</div>
        </div>
    `;
    document.body.appendChild(toast);

    setTimeout(() => {
        toast.classList.add('show');
    }, 100);

    setTimeout(() => {
        toast.classList.remove('show');
        setTimeout(() => toast.remove(), 500);
    }, 5000);
}

function showHandshakeNotification(data) {
    // Play sound
    playKaChing();

    // Show notification
    addLog(`🎉 Handshake capturado: ${data.ssid}`, 'success');

    // Flash the handshake counter
    const counter = document.getElementById('handshakes-count');
    if (counter) {
        counter.classList.add('explosion');
        setTimeout(() => counter.classList.remove('explosion'), 500);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════════════════
function escapeHtml(text) {
    if (!text) return '';
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

function getSignalBars(rssi) {
    if (rssi >= -50) return '████';
    if (rssi >= -60) return '███░';
    if (rssi >= -70) return '██░░';
    if (rssi >= -80) return '█░░░';
    return '░░░░';
}

function getSecurityClass(security) {
    if (!security) return 'open';
    const sec = security.toUpperCase();
    if (sec.includes('WPA3')) return 'wpa3';
    if (sec.includes('WPA2')) return 'wpa2';
    if (sec.includes('WEP')) return 'wep';
    if (sec.includes('OPEN') || sec.includes('OPN')) return 'open';
    return 'unknown';
}

function selectTarget(bssid, ssid) {
    const selector = document.getElementById('target-selector');
    if (selector) selector.value = bssid;

    addLog(`Alvo selecionado: ${ssid} (${bssid})`, 'info');
}

// Export
window.initWebSocket = initWebSocket;
