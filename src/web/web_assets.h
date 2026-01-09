#pragma once

const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WavePwn Dashboard</title>
    <link rel="stylesheet" href="style.css">
    <link rel="icon" type="image/x-icon" href="favicon.ico">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <header>
        <div class="logo">
            <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 12h5l3 9 4-17 5 8h3"/></svg>
            WavePwn <span style="font-weight: 400; font-size: 0.8em; opacity: 0.7;">v2.1</span>
        </div>
        <nav>
            <a href="/" class="nav-link active">Dashboard</a>
            <a href="/config.html" class="nav-link">Configuração</a>
            <a href="/ota/update.html" class="nav-link">Atualizar</a>
        </nav>
        <div class="status-indicator" id="status-dot"></div>
    </header>

    <div class="app-container">
        <!-- Status Grid -->
        <div class="grid">
            <div class="card">
                <h3>Ameaça Atual</h3>
                <div class="stat-value" id="threat-label">Analisando...</div>
                <div class="stat-sub">Confiança: <span id="threat-conf">0%</span></div>
            </div>
            <div class="card">
                <h3>Redes (APs)</h3>
                <div class="stat-value" id="ap-count">0</div>
                <div class="stat-sub">Visíveis</div>
            </div>
            <div class="card">
                <h3>Handshakes</h3>
                <div class="stat-value" id="hs-count">0</div>
                <div class="stat-sub" id="pmkid-count">0 PMKIDs</div>
            </div>
            <div class="card">
                <h3>Sistema</h3>
                <div class="stat-value" id="battery">0%</div>
                <div class="stat-sub" id="uptime">00:00:00</div>
            </div>
        </div>

        <!-- Main Content -->
        <div class="grid" style="grid-template-columns: 2fr 1fr;">
            <!-- Chart -->
            <div class="card">
                <h3>Sinais de Atividade (2.4GHz / 5GHz)</h3>
                <div style="height: 300px; position: relative;">
                    <canvas id="signalChart"></canvas>
                </div>
            </div>

            <!-- Attack Controls -->
            <div class="card">
                <h3>Central de Ataques</h3>
                <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 0.5rem; margin-top: 1rem;">
                     <button class="danger" onclick="startAttack('deauth')">Deauth Flood</button>
                     <button class="warning" onclick="startAttack('beacon')">Beacon Flood</button>
                     <button class="primary" onclick="startAttack('handshake')">Handshake Search</button>
                     <button class="secondary" onclick="stopAttacks()">PARAR TUDO</button>
                </div>
            </div>

            <!-- Quick Actions -->
            <div class="card">
                <h3>Ações Rápidas</h3>
                <div style="display: flex; flex-direction: column; gap: 1rem; margin-top: 1rem;">
                    <button onclick="rebootDevice()">Reiniciar Dispositivo</button>
                    <button class="secondary" onclick="clearLogs()">Limpar Logs</button>
                </div>
            </div>

            <!-- Console -->
            <div class="card console-card">
                <div style="display: flex; justify-content: space-between; padding-bottom: 0.5rem; border-bottom: 1px solid #333;">
                    <h3>Console do Sistema</h3>
                    <span style="color: #666; font-size: 0.7rem;">LIVE</span>
                </div>
                <div class="console-output" id="console-logs">
                    <!-- Logs will appear here -->
                    <div class="log-line"><span class="log-time">[SYSTEM]</span> Conectando ao WebSocket...</div>
                </div>
            </div>
        </div>
    </div>

    <script>
        // WebSocket Connection
        let ws;
        const statusDot = document.getElementById('status-dot');
        const consoleLogs = document.getElementById('console-logs');

        function connectWS() {
            const proto = window.location.protocol === 'https:' ? 'wss' : 'ws';
            ws = new WebSocket(`${proto}://${window.location.hostname}/ws`);

            ws.onopen = () => {
                log('WS', 'Conectado ao dispositivo');
                statusDot.style.backgroundColor = '#22c55e';
                statusDot.style.boxShadow = '0 0 10px #22c55e';
            };

            ws.onclose = () => {
                log('WS', 'Desconectado. Tentando reconectar...');
                statusDot.style.backgroundColor = '#ef4444';
                statusDot.style.boxShadow = 'none';
                setTimeout(connectWS, 2000);
            };

            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    updateDashboard(data);
                } catch (e) {
                    // Ignore JSON errors
                }
            };
        }

        function updateDashboard(data) {
            // Update Stats
            if(data.uptime) document.getElementById('uptime').innerText = formatUptime(data.uptime);
            if(data.battery) document.getElementById('battery').innerText = data.battery + '%';
            if(data.aps) document.getElementById('ap-count').innerText = data.aps;
            if(data.hs) document.getElementById('hs-count').innerText = data.hs;
            if(data.pmkid) document.getElementById('pmkid-count').innerText = data.pmkid + ' PMKIDs';
            
            if(data.ai) {
                const el = document.getElementById('threat-label');
                el.innerText = data.ai;
                // Simple color coding
                if(data.ai === 'SAFE') el.style.color = '#22c55e';
                else if(data.ai === 'EAPOL') el.style.color = '#f59e0b';
                else if(data.ai === 'DEAUTH') el.style.color = '#ef4444';
                else el.style.color = '#3b82f6';
            }

            // Log
            if(data.log && data.log.trim() !== '') {
                log('SYS', data.log);
            }
        }

        function formatUptime(seconds) {
            const h = Math.floor(seconds / 3600);
            const m = Math.floor((seconds % 3600) / 60);
            const s = seconds % 60;
            return `${h.toString().padStart(2,'0')}:${m.toString().padStart(2,'0')}:${s.toString().padStart(2,'0')}`;
        }

        function log(source, msg) {
            const time = new Date().toLocaleTimeString();
            const line = document.createElement('div');
            line.className = 'log-line';
            line.innerHTML = `<span class="log-time">[${time}]</span> <span style="color: #cbd5e1">${msg}</span>`;
            consoleLogs.insertBefore(line, consoleLogs.firstChild);
            if(consoleLogs.children.length > 100) {
                consoleLogs.removeChild(consoleLogs.lastChild);
            }
        }

        function clearLogs() {
            consoleLogs.innerHTML = '';
        }

        function rebootDevice() {
            if(confirm('Tem certeza que deseja reiniciar o dispositivo?')) {
                fetch('/api/reboot', { method: 'POST' }).then(() => {
                    alert('Reiniciando... Aguarde alguns segundos.');
                    setTimeout(() => window.location.reload(), 5000);
                });
            }
        }

        function startAttack(type) {
             fetch('/api/attack/start?type=' + type, { method: 'POST' })
                .then(r => r.json())
                .then(d => {
                    if(d.success) log('ATK', 'Ataque iniciado: ' + type);
                    else log('ATK', 'Falha ao iniciar: ' + d.msg);
                });
        }

        function stopAttacks() {
             fetch('/api/stop', { method: 'POST' });
        }

        // Initialize Chart
        try {
            const ctx = document.getElementById('signalChart').getContext('2d');
            const signalChart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: Array(20).fill(''),
                    datasets: [{
                        label: 'Atividade WiFi',
                        data: Array(20).fill(0),
                        borderColor: '#3b82f6',
                        backgroundColor: 'rgba(59, 130, 246, 0.1)',
                        tension: 0.4,
                        fill: true
                    }]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {
                        legend: { display: false }
                    },
                    scales: {
                        y: {
                            beginAtZero: true,
                            grid: { color: 'rgba(255,255,255,0.05)' },
                            ticks: { color: '#94a3b8' }
                        },
                        x: {
                            grid: { display: false }
                        }
                    },
                    animation: { duration: 0 }
                }
            });

            // Mock chart animation
            setInterval(() => {
                const data = signalChart.data.datasets[0].data;
                data.shift();
                data.push(Math.floor(Math.random() * 50) + 10); // Random activity
                signalChart.update();
            }, 1000);
        } catch(e) {
            console.log("Chart not loaded (offline)");
        }

        // Start
        connectWS();
    </script>
</body>
</html>
)rawliteral";

const char PROGMEM STYLE_CSS[] = R"rawliteral(
:root {
    --bg-color: #0f172a;
    --surface-color: #1e293b;
    --primary-color: #3b82f6;
    --primary-hover: #2563eb;
    --accent-color: #8b5cf6;
    --text-primary: #f8fafc;
    --text-secondary: #94a3b8;
    --success-color: #10b981;
    --warning-color: #f59e0b;
    --danger-color: #ef4444;
    --border-radius: 12px;
    --transition: all 0.3s ease;
    --shadow: 0 4px 6px -1px rgb(0 0 0 / 0.1), 0 2px 4px -2px rgb(0 0 0 / 0.1);
    --glass: rgba(30, 41, 59, 0.7);
}

* { box-sizing: border-box; margin: 0; padding: 0; }

body {
    font-family: 'Inter', system-ui, -apple-system, sans-serif;
    background-color: var(--bg-color);
    color: var(--text-primary);
    line-height: 1.6;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
}

h1, h2, h3 { font-weight: 700; margin-bottom: 1rem; }
h3 { font-size: 0.875rem; color: var(--text-secondary); text-transform: uppercase; letter-spacing: 0.05em; }

.app-container { max-width: 1200px; margin: 0 auto; padding: 1.5rem; width: 100%; }

header {
    display: flex; justify-content: space-between; align-items: center;
    margin-bottom: 2rem; padding: 1rem;
    background: var(--surface-color); border-radius: var(--border-radius);
    box-shadow: var(--shadow); border: 1px solid rgba(255, 255, 255, 0.05);
}

.logo {
    display: flex; align-items: center; gap: 0.75rem;
    font-size: 1.25rem; font-weight: 800;
    background: linear-gradient(135deg, var(--primary-color), var(--accent-color));
    -webkit-background-clip: text; -webkit-text-fill-color: transparent;
}

nav { display: flex; gap: 1rem; }
.nav-link { color: var(--text-secondary); text-decoration: none; font-weight: 600; padding: 0.5rem 1rem; border-radius: 0.5rem; transition: var(--transition); }
.nav-link:hover, .nav-link.active { background: rgba(59, 130, 246, 0.1); color: var(--primary-color); }

.grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 1.5rem; margin-bottom: 2rem; }

.card {
    background: var(--surface-color); border-radius: var(--border-radius);
    padding: 1.5rem; box-shadow: var(--shadow);
    border: 1px solid rgba(255, 255, 255, 0.05); transition: var(--transition);
}
.stat-value { font-size: 2rem; font-weight: 800; color: var(--text-primary); }
.stat-sub { font-size: 0.875rem; color: var(--text-secondary); }

.status-indicator { width: 10px; height: 10px; border-radius: 50%; background-color: var(--success-color); animation: pulse 2s infinite; }
@keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }

.console-card { background: #000; font-family: 'Fira Code', monospace; }
.console-output { height: 300px; overflow-y: auto; padding: 1rem; color: #22c55e; font-size: 0.875rem; display: flex; flex-direction: column; }
.log-line { margin-bottom: 0.25rem; word-break: break-all; }
.log-time { color: #64748b; margin-right: 0.5rem; }

button { cursor: pointer; background: var(--primary-color); color: white; border: none; padding: 0.75rem 1.5rem; border-radius: 0.5rem; font-weight: 600; transition: var(--transition); }
button:hover { background: var(--primary-hover); transform: translateY(-1px); }
button.secondary { background: transparent; border: 1px solid rgba(255, 255, 255, 0.1); }
button.danger { background: var(--danger-color); }

button.warning { background: var(--warning-color); color: #000; }

.form-group { margin-bottom: 1rem; }
.form-group label { display: block; margin-bottom: 0.5rem; color: var(--text-secondary); }
.form-group input, .form-group select {
    width: 100%; padding: 0.75rem; border-radius: 0.5rem;
    background: rgba(0,0,0,0.2); border: 1px solid rgba(255,255,255,0.1);
    color: white; font-family: inherit;
}
.form-group input:focus, .form-group select:focus {
    outline: none; border-color: var(--primary-color);
}

@media (max-width: 640px) { .grid { grid-template-columns: 1fr; } header { flex-direction: column; } }
)rawliteral";

const char PROGMEM CONFIG_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Configuração - WavePwn</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <div class="logo">WavePwn v2.1</div>
        <nav>
            <a href="/" class="nav-link">Dashboard</a>
            <a href="/config.html" class="nav-link active">Configuração</a>
        </nav>
    </header>
    <div class="app-container">
        <div class="card">
            <h2>Configurações do Dispositivo</h2>
            <form id="configForm" onsubmit="saveConfig(event)">
                <div class="form-group">
                    <label>Nome do Dispositivo</label>
                    <input type="text" id="devName" value="WavePwn">
                </div>
                <div class="form-group">
                    <label>Tema da UI</label>
                    <select id="theme">
                        <option value="0">Hacker Dark</option>
                        <option value="1">Cyberpunk</option>
                        <option value="2">Matrix</option>
                    </select>
                </div>
                <div class="form-group">
                    <label>Tempo de Scan WiFi (ms)</label>
                    <input type="number" id="scanTime" value="600">
                </div>
                 <div class="form-group">
                    <label>Mascote na Tela</label>
                    <select id="mascot">
                        <option value="1">Ativado</option>
                        <option value="0">Desativado</option>
                    </select>
                </div>
                
                <div style="margin-top: 1rem; display: flex; gap: 1rem;">
                    <button type="submit">Salvar Alterações</button>
                    <button type="button" class="secondary" onclick="window.location.href='/'">Voltar</button>
                </div>
            </form>
        </div>
    </div>
    <script>
        function saveConfig(e) {
            e.preventDefault();
            const data = {
                name: document.getElementById('devName').value,
                theme: document.getElementById('theme').value,
                scanTime: document.getElementById('scanTime').value,
                mascot: document.getElementById('mascot').value
            };
            fetch('/api/config/device', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(data)
            }).then(() => alert('Configurações salvas!'));
        }
    </script>
</body>
</html>
)rawliteral";

const char PROGMEM UPDATE_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Atualizar - WavePwn</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <header>
        <div class="logo">WavePwn v2.1</div>
        <nav>
            <a href="/" class="nav-link">Dashboard</a>
            <a href="/config.html" class="nav-link">Configuração</a>
            <a href="/ota/update.html" class="nav-link active">Atualizar</a>
        </nav>
    </header>
    <div class="app-container">
        <div class="card">
            <h2>Atualização de Firmware (OTA)</h2>
            <p>Selecione o arquivo .bin para atualizar o firmware.</p>
            
            <form method="POST" action="/update" enctype="multipart/form-data" style="margin-top: 2rem;">
                <input type="file" name="update" accept=".bin">
                <br><br>
                <button type="submit">Iniciar Atualização</button>
            </form>
        </div>
    </div>
</body>
</html>
)rawliteral";
