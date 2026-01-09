/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Main Application Entry Point
 * ═══════════════════════════════════════════════════════════════════════════
 */

// Global state
const WavePwn = {
    state: {
        connected: false,
        theme: 'cyberpunk',
        soundEnabled: true,
        currentView: 'dashboard',
        stats: {
            uptime: 0,
            battery: 0,
            networks: 0,
            handshakes: 0,
            pmkids: 0,
            bleDevices: 0,
            deauthsSent: 0,
            temperature: 0
        },
        dragon: {
            name: 'Neura',
            mood: 'happy',
            personality: 'aggressive'
        }
    },
    ws: null,
    charts: {},
    sounds: {}
};

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════
document.addEventListener('DOMContentLoaded', () => {
    console.log('[WavePwn] Initializing Cyberpunk Command Center...');

    // Show boot animation
    showBootAnimation().then(() => {
        // Initialize all modules
        initTheme();
        initWebSocket();
        initEffects();
        initDragon();
        initNavigation();
        initEasterEggs();
        initGlitchButtons();
        initCharts();

        // Show initial view
        showView('dashboard');

        console.log('[WavePwn] Initialization complete!');
    });
});

// ═══════════════════════════════════════════════════════════════════════════
// BOOT ANIMATION
// ═══════════════════════════════════════════════════════════════════════════
async function showBootAnimation() {
    return new Promise((resolve) => {
        const overlay = document.getElementById('boot-overlay');
        if (!overlay) {
            resolve();
            return;
        }

        const bootText = overlay.querySelector('.boot-text');
        const bootLines = [
            'LELEWATCH SYSTEM INITIALIZING...',
            'LOADING NEURAL NETWORKS...',
            'CONNECTING TO LELEWATCH CORE...',
            'ESTABLISHING SECURE CHANNEL...',
            'SYSTEM READY.'
        ];

        let lineIndex = 0;
        const lineInterval = setInterval(() => {
            if (lineIndex < bootLines.length) {
                bootText.innerHTML += bootLines[lineIndex] + '<br>';
                lineIndex++;
            } else {
                clearInterval(lineInterval);
            }
        }, 400);

        // Fade out after boot completes
        setTimeout(() => {
            overlay.classList.add('fade-out');
            setTimeout(() => {
                overlay.style.display = 'none';
                resolve();
            }, 1000);
        }, 2500);
    });
}

// ═══════════════════════════════════════════════════════════════════════════
// THEME SYSTEM
// ═══════════════════════════════════════════════════════════════════════════
function initTheme() {
    const savedTheme = localStorage.getItem('wavepwn-theme') || 'cyberpunk';
    setTheme(savedTheme);
}

function setTheme(themeName) {
    document.documentElement.setAttribute('data-theme', themeName);
    WavePwn.state.theme = themeName;
    localStorage.setItem('wavepwn-theme', themeName);

    // Update theme selector if exists
    const selector = document.getElementById('theme-selector');
    if (selector) selector.value = themeName;

    console.log(`[Theme] Switched to: ${themeName}`);
}

function toggleNightOps() {
    document.body.classList.toggle('night-ops');

    // Create night ops dot if needed
    if (document.body.classList.contains('night-ops')) {
        if (!document.querySelector('.night-ops-dot')) {
            const dot = document.createElement('div');
            dot.classList.add('night-ops-dot');
            document.body.appendChild(dot);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// NAVIGATION
// ═══════════════════════════════════════════════════════════════════════════
function initNavigation() {
    const navLinks = document.querySelectorAll('.nav-links a, .nav-links button');

    navLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const viewId = link.dataset.view || link.getAttribute('href')?.replace('#', '');
            if (viewId) {
                showView(viewId);

                // Update active state
                navLinks.forEach(l => l.classList.remove('active'));
                link.classList.add('active');
            }
        });
    });

    // Mobile sidebar toggle
    const menuToggle = document.getElementById('menu-toggle');
    const sidebar = document.querySelector('.sidebar');

    if (menuToggle && sidebar) {
        menuToggle.addEventListener('click', () => {
            sidebar.classList.toggle('open');
        });
    }
}

function showView(viewId) {
    // Portal transition effect
    const transition = document.createElement('div');
    transition.classList.add('portal-transition', 'closing');
    document.body.appendChild(transition);

    setTimeout(() => {
        // Hide all views
        document.querySelectorAll('.view').forEach(view => {
            view.classList.remove('active');
        });

        // Show target view
        const targetView = document.getElementById(viewId);
        if (targetView) {
            targetView.classList.add('active');
            WavePwn.state.currentView = viewId;
        }

        // Start opening transition
        transition.classList.remove('closing');

        setTimeout(() => {
            transition.remove();
        }, 1000);
    }, 400);
}

// ═══════════════════════════════════════════════════════════════════════════
// GLITCH BUTTON EFFECT
// ═══════════════════════════════════════════════════════════════════════════
function initGlitchButtons() {
    document.querySelectorAll('.btn, button').forEach(btn => {
        btn.addEventListener('click', function () {
            this.classList.add('glitch-click-active');

            // Play click sound if enabled
            if (WavePwn.state.soundEnabled && WavePwn.sounds.click) {
                WavePwn.sounds.click.play();
            }

            setTimeout(() => {
                this.classList.remove('glitch-click-active');
            }, 300);
        });
    });
}

// ═══════════════════════════════════════════════════════════════════════════
// EFFECTS
// ═══════════════════════════════════════════════════════════════════════════
function initEffects() {
    // Initialize Matrix rain
    initMatrixRain();

    // Initialize floating particles
    initParticles();

    // Add scanlines overlay
    if (!document.querySelector('.scanlines-overlay')) {
        const scanlines = document.createElement('div');
        scanlines.classList.add('scanlines-overlay');
        document.body.appendChild(scanlines);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// CHARTS
// ═══════════════════════════════════════════════════════════════════════════
function initCharts() {
    // Signal strength chart
    const signalCanvas = document.getElementById('signal-chart');
    if (signalCanvas && typeof Chart !== 'undefined') {
        WavePwn.charts.signal = new Chart(signalCanvas, {
            type: 'line',
            data: {
                labels: Array(30).fill(''),
                datasets: [{
                    label: 'Atividade WiFi',
                    data: Array(30).fill(0),
                    borderColor: getComputedStyle(document.documentElement).getPropertyValue('--primary').trim(),
                    backgroundColor: 'rgba(0, 255, 245, 0.1)',
                    tension: 0.4,
                    fill: true,
                    pointRadius: 0
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: { legend: { display: false } },
                scales: {
                    y: {
                        beginAtZero: true,
                        grid: { color: 'rgba(255,255,255,0.05)' },
                        ticks: { color: '#606070' }
                    },
                    x: { display: false }
                },
                animation: { duration: 0 }
            }
        });
    }

    // Channel usage chart
    const channelCanvas = document.getElementById('channel-chart');
    if (channelCanvas && typeof Chart !== 'undefined') {
        WavePwn.charts.channel = new Chart(channelCanvas, {
            type: 'bar',
            data: {
                labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13'],
                datasets: [{
                    label: 'APs por Canal',
                    data: Array(13).fill(0),
                    backgroundColor: 'rgba(0, 255, 245, 0.6)',
                    borderColor: '#00fff5',
                    borderWidth: 1
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: { legend: { display: false } },
                scales: {
                    y: {
                        beginAtZero: true,
                        grid: { color: 'rgba(255,255,255,0.05)' },
                        ticks: { color: '#606070' }
                    },
                    x: {
                        grid: { display: false },
                        ticks: { color: '#606070' }
                    }
                }
            }
        });
    }

    // Security type pie chart
    const securityCanvas = document.getElementById('security-chart');
    if (securityCanvas && typeof Chart !== 'undefined') {
        WavePwn.charts.security = new Chart(securityCanvas, {
            type: 'doughnut',
            data: {
                labels: ['WPA2', 'WPA3', 'WEP', 'Open'],
                datasets: [{
                    data: [0, 0, 0, 0],
                    backgroundColor: ['#00fff5', '#ff00ff', '#ffcc00', '#ff0055'],
                    borderWidth: 0
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        position: 'bottom',
                        labels: { color: '#a0a0b0' }
                    }
                }
            }
        });
    }
}

function updateChart(chartName, newValue) {
    const chart = WavePwn.charts[chartName];
    if (!chart) return;

    if (chartName === 'signal') {
        chart.data.datasets[0].data.push(newValue);
        chart.data.datasets[0].data.shift();
        chart.update('none');
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SOUND EFFECTS
// ═══════════════════════════════════════════════════════════════════════════
function initSounds() {
    // Only load if sounds are enabled and available
    try {
        WavePwn.sounds.kaChng = new Audio('/assets/sounds/ka-ching.mp3');
        WavePwn.sounds.click = new Audio('/assets/sounds/click.mp3');
        WavePwn.sounds.alert = new Audio('/assets/sounds/alert.mp3');
        WavePwn.sounds.ambient = new Audio('/assets/sounds/ambient.mp3');
        WavePwn.sounds.ambient.loop = true;
        WavePwn.sounds.ambient.volume = 0.2;
    } catch (e) {
        console.log('[Sound] Audio not available');
    }
}

function playKaChing() {
    if (WavePwn.state.soundEnabled && WavePwn.sounds.kaChng) {
        WavePwn.sounds.kaChng.currentTime = 0;
        WavePwn.sounds.kaChng.play().catch(() => { });
    }
}

function toggleAmbientSound() {
    if (!WavePwn.sounds.ambient) return;

    if (WavePwn.sounds.ambient.paused) {
        WavePwn.sounds.ambient.play().catch(() => { });
    } else {
        WavePwn.sounds.ambient.pause();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// UI UPDATE HELPERS
// ═══════════════════════════════════════════════════════════════════════════
function updateStats(data) {
    // Update internal state
    Object.assign(WavePwn.state.stats, data);

    // Update DOM elements
    updateElement('uptime', formatUptime(data.uptime));
    updateElement('battery', data.battery + '%');
    updateElement('temperature', data.temperature + '°C');
    updateElement('networks-count', data.networks);
    updateElement('handshakes-count', data.handshakes, true);
    updateElement('pmkids-count', data.pmkids);
    updateElement('ble-count', data.bleDevices);
    updateElement('deauths-count', data.deauthsSent);

    // Update threat indicator
    const threatEl = document.getElementById('threat-level');
    if (threatEl && data.threat) {
        threatEl.textContent = data.threat;
        threatEl.className = `threat-badge threat-${data.threat.toLowerCase()}`;
    }
}

function updateElement(id, value, animate = false) {
    const el = document.getElementById(id);
    if (!el) return;

    const oldValue = el.textContent;
    el.textContent = value;

    // Animate if value changed
    if (animate && oldValue !== String(value)) {
        el.classList.add('counter-update');
        setTimeout(() => el.classList.remove('counter-update'), 300);

        // Play ka-ching for handshakes
        if (id === 'handshakes-count' && parseInt(value) > parseInt(oldValue)) {
            playKaChing();
            el.classList.add('ka-ching');
            setTimeout(() => el.classList.remove('ka-ching'), 1000);
        }
    }
}

function formatUptime(seconds) {
    const h = Math.floor(seconds / 3600);
    const m = Math.floor((seconds % 3600) / 60);
    const s = seconds % 60;
    return `${h.toString().padStart(2, '0')}:${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
}

function addLog(message, type = 'info') {
    const logContainer = document.getElementById('log-container');
    if (!logContainer) return;

    const logLine = document.createElement('div');
    logLine.classList.add('log-line', `log-${type}`);
    const time = new Date().toLocaleTimeString();
    logLine.innerHTML = `<span class="log-time">[${time}]</span> <span class="log-msg">${message}</span>`;

    logContainer.insertBefore(logLine, logContainer.firstChild);

    // Keep only last 100 logs
    while (logContainer.children.length > 100) {
        logContainer.removeChild(logContainer.lastChild);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// API COMMANDS
// ═══════════════════════════════════════════════════════════════════════════
async function sendCommand(command, params = {}) {
    // Send via WebSocket if connected
    if (WavePwn.ws && WavePwn.ws.readyState === WebSocket.OPEN) {
        WavePwn.ws.send(JSON.stringify({ cmd: command, ...params }));
        addLog(`Comando enviado: ${command}`, 'info');
        return;
    }

    // Fallback to HTTP API
    try {
        const response = await fetch(`/api/${command}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(params)
        });
        const data = await response.json();
        addLog(`${command}: ${data.status || 'OK'}`, 'success');
        return data;
    } catch (error) {
        addLog(`Erro: ${error.message}`, 'error');
        throw error;
    }
}

// Attack shortcuts
function startDeauth(target) {
    sendCommand('deauth', { target });
}

function startBeaconFlood() {
    sendCommand('beacon');
}

function startHandshakeCapture() {
    sendCommand('handshake');
}

function stopAllAttacks() {
    sendCommand('stop');
}

function pwnEverything() {
    const btn = document.getElementById('pwn-btn');
    if (!btn) return;

    // Countdown
    let countdown = 60;
    btn.disabled = true;
    btn.textContent = `PWN ATIVO: ${countdown}s`;
    btn.classList.add('active');

    // Start all attacks
    sendCommand('pwn_all');

    const interval = setInterval(() => {
        countdown--;
        btn.textContent = `PWN ATIVO: ${countdown}s`;

        if (countdown <= 0) {
            clearInterval(interval);
            sendCommand('stop');
            btn.disabled = false;
            btn.textContent = '☠️ PWN EVERYTHING ☠️';
            btn.classList.remove('active');
        }
    }, 1000);
}

// ═══════════════════════════════════════════════════════════════════════════
// EXPORTS
// ═══════════════════════════════════════════════════════════════════════════
window.WavePwn = WavePwn;
window.showView = showView;
window.setTheme = setTheme;
window.toggleNightOps = toggleNightOps;
window.sendCommand = sendCommand;
window.startDeauth = startDeauth;
window.startBeaconFlood = startBeaconFlood;
window.startHandshakeCapture = startHandshakeCapture;
window.stopAllAttacks = stopAllAttacks;
window.pwnEverything = pwnEverything;
window.toggleAmbientSound = toggleAmbientSound;
