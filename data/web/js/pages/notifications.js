/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Notifications Config Page
 * Visual notification configuration and testing
 * ═══════════════════════════════════════════════════════════════════════════
 */

const NotificationsPage = {
    config: {
        enabled: true,
        effect: 1,
        color: '#00FFF5',
        duration: 800,
        fade: 1,
        events: [true, true, true, true, true, true, true]
    },
    effects: ['Flash Total', 'Bordas Neon', 'Pulso Central', 'Scanline', 'Glitch', 'Dragão Pisca'],
    events: ['Handshake', 'WPS PIN', 'BLE Próximo', 'Bateria <20%', 'SD Cheio', 'Ataque OK', 'USB Connect'],
    colors: [
        '#00FFF5', '#FF00FF', '#BD00FF', '#00FF9D', '#FFCC00', '#FF0055', '#00A8FF', '#FF6600',
        '#FF0080', '#00FF00', '#39FF14', '#FE01B1', '#01FEFE', '#FE0101', '#FFFF00', '#7B00FF'
    ]
};

async function initNotificationsPage() {
    console.log('[Notifications] Initializing...');
    await loadNotificationConfig();
    renderNotificationConfig();
}

async function loadNotificationConfig() {
    try {
        const response = await fetch('/api/notifications/config');
        const data = await response.json();
        NotificationsPage.config = { ...NotificationsPage.config, ...data };
    } catch (e) {
        console.error('[Notifications] Failed to load config:', e);
    }
}

function renderNotificationConfig() {
    const container = document.getElementById('notifications-config');
    if (!container) return;

    container.innerHTML = `
        <!-- Master Enable -->
        <div class="config-row">
            <label>Notificações Visuais</label>
            <label class="switch">
                <input type="checkbox" id="notify-enabled" ${NotificationsPage.config.enabled ? 'checked' : ''} onchange="updateNotifyConfig('enabled', this.checked)">
                <span class="slider"></span>
            </label>
        </div>
        
        <!-- Effect Type -->
        <div class="config-row">
            <label>Tipo de Efeito</label>
            <select id="notify-effect" onchange="updateNotifyConfig('effect', this.value)">
                ${NotificationsPage.effects.map((e, i) => `<option value="${i}" ${i == NotificationsPage.config.effect ? 'selected' : ''}>${e}</option>`).join('')}
            </select>
        </div>
        
        <!-- Color Palette -->
        <div class="config-row">
            <label>Cor da Notificação</label>
            <div class="color-palette">
                ${NotificationsPage.colors.map(c => `<button class="color-btn ${c == NotificationsPage.config.color ? 'active' : ''}" style="background:${c}" onclick="updateNotifyConfig('color', '${c}')"></button>`).join('')}
            </div>
        </div>
        
        <!-- Duration -->
        <div class="config-row">
            <label>Duração: <span id="duration-val">${NotificationsPage.config.duration}ms</span></label>
            <input type="range" min="300" max="3000" step="100" value="${NotificationsPage.config.duration}" 
                   oninput="document.getElementById('duration-val').textContent = this.value + 'ms'; updateNotifyConfig('duration', parseInt(this.value))">
        </div>
        
        <!-- Fade Intensity -->
        <div class="config-row">
            <label>Intensidade Fade</label>
            <select id="notify-fade" onchange="updateNotifyConfig('fade', parseInt(this.value))">
                <option value="0" ${NotificationsPage.config.fade == 0 ? 'selected' : ''}>Suave</option>
                <option value="1" ${NotificationsPage.config.fade == 1 ? 'selected' : ''}>Médio</option>
                <option value="2" ${NotificationsPage.config.fade == 2 ? 'selected' : ''}>Agressivo</option>
            </select>
        </div>
        
        <!-- Test Button -->
        <div class="config-row">
            <button class="btn btn-lg btn-primary" style="width:100%; font-size:1.5rem; padding: 20px;" onclick="testNotification()">
                🔔 TESTAR NOTIFICAÇÃO
            </button>
        </div>
        
        <!-- Event Checkboxes -->
        <h4 class="mt-3">Notificar em:</h4>
        <div class="event-grid">
            ${NotificationsPage.events.map((e, i) => `
                <label class="event-checkbox">
                    <input type="checkbox" ${NotificationsPage.config.events[i] ? 'checked' : ''} onchange="updateEventEnabled(${i}, this.checked)">
                    <span>${e}</span>
                </label>
            `).join('')}
        </div>
    `;
}

function updateNotifyConfig(key, value) {
    NotificationsPage.config[key] = value;

    // Update active color button
    if (key === 'color') {
        document.querySelectorAll('.color-btn').forEach(btn => {
            btn.classList.toggle('active', btn.style.backgroundColor === value || btn.getAttribute('onclick').includes(value));
        });
    }

    // Send to device
    sendNotifyConfigToDevice();
}

function updateEventEnabled(index, enabled) {
    NotificationsPage.config.events[index] = enabled;
    sendNotifyConfigToDevice();
}

async function sendNotifyConfigToDevice() {
    try {
        await fetch('/api/notifications/config', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(NotificationsPage.config)
        });
    } catch (e) {
        console.error('[Notifications] Failed to save config:', e);
    }
}

async function testNotification() {
    addLog('Testando notificação...', 'info');

    // Visual test in browser
    const overlay = document.createElement('div');
    overlay.className = 'notify-test-overlay';
    overlay.style.cssText = `
        position: fixed; top: 0; left: 0; right: 0; bottom: 0;
        background: ${NotificationsPage.config.color}40;
        box-shadow: inset 0 0 100px ${NotificationsPage.config.color};
        z-index: 9999; pointer-events: none;
        animation: notify-test ${NotificationsPage.config.duration}ms ease-out forwards;
    `;
    document.body.appendChild(overlay);

    setTimeout(() => overlay.remove(), NotificationsPage.config.duration);

    // Send test command to device
    try {
        await fetch('/api/notifications/test', { method: 'POST' });
    } catch (e) {
        console.error('[Notifications] Test failed:', e);
    }
}

// Export
window.initNotificationsPage = initNotificationsPage;
window.updateNotifyConfig = updateNotifyConfig;
window.updateEventEnabled = updateEventEnabled;
window.testNotification = testNotification;
