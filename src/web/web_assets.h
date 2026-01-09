#pragma once
#include <Arduino.h>

const char WEB_APP_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WaveShare Dashboard</title>
    <style>
        :root { --bg: #0a0a0a; --panel: #141414; --accent: #00ff88; --text: #e0e0e0; }
        body { margin: 0; font-family: 'Courier New', monospace; background: var(--bg); color: var(--text); }
        .container { display: flex; height: 100vh; flex-direction: column; }
        header { background: var(--panel); padding: 1rem; border-bottom: 1px solid var(--accent); display: flex; justify-content: space-between; }
        .tabs { display: flex; overflow-x: auto; background: #222; }
        .tab { padding: 1rem; cursor: pointer; border-right: 1px solid #333; flex-grow: 1; text-align: center; }
        .tab.active { background: var(--accent); color: #000; font-weight: bold; }
        .content { flex-grow: 1; padding: 1rem; overflow-y: auto; display: none; }
        .content.active { display: block; }
        .card { background: var(--panel); border: 1px solid #333; padding: 1.5rem; margin-bottom: 1.5rem; border-radius: 8px; }
        h2 { border-bottom: 1px solid var(--accent); padding-bottom: 0.5rem; margin-top: 0; font-size: 1.5rem; }
        button { background: var(--accent); color: #000; border: none; padding: 1rem 1.5rem; font-weight: bold; cursor: pointer; margin-right: 0.5rem; font-size: 1.1rem; border-radius: 4px; touch-action: manipulation; }
        button.red { background: #ff3333; color: white; }
        input[type="text"] { padding: 0.8rem; background: #333; border: 1px solid #555; color: white; width: 70%; font-size: 1.1rem; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(240px, 1fr)); gap: 1.5rem; }
        
        #console { background: #000; height: 200px; overflow-y: auto; padding: 0.5rem; font-size: 0.8rem; border-top: 1px solid #333; }
        .log-entry { margin: 2px 0; }
        .log-time { color: #666; margin-right: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <div>WAVESHARE <span style="font-size:0.8rem">v2.0</span></div>
            <div id="status">Disconnected</div>
        </header>
        
        <div class="tabs">
            <div class="tab active" onclick="showTab('dash')">DASHBOARD</div>
            <div class="tab" onclick="showTab('wifi')">WIFI</div>
            <div class="tab" onclick="showTab('ble')">BLE</div>
            <div class="tab" onclick="showTab('ir')">IR</div>
            <div class="tab" onclick="showTab('tools')">TOOLS</div>
        </div>

        <!-- DASHBOARD -->
        <div id="dash" class="content active">
            <div class="grid">
                <div class="card">
                    <h3>NETWORKS</h3>
                    <div id="stat_networks" style="font-size: 2rem; color: var(--accent)">0</div>
                </div>
                <div class="card">
                    <h3>HANDSHAKES</h3>
                    <div id="stat_handshakes" style="font-size: 2rem; color: #ffcc00">0</div>
                </div>
                <div class="card">
                    <h3>BATTERY</h3>
                    <div id="stat_battery" style="font-size: 2rem;">--%</div>
                </div>
            </div>
            <div class="card">
                <h3>SYSTEM</h3>
                <div id="sys_info">Loading...</div>
                <br>
                <button onclick="apiCall('/api/stop', 'POST')">STOP ALL ATTACKS</button>
            </div>
        </div>

        <!-- WIFI -->
        <div id="wifi" class="content">
            <div class="card">
                <h2>WiFi Manager</h2>
                <button onclick="apiCall('/api/scan')">SCAN NETWORKS</button>
                <button class="red" onclick="apiCall('/api/deauth?channel=0', 'POST')">DEAUTH ALL</button>
                <button onclick="apiCall('/api/handshake', 'POST')">HANDSHAKE CAPTURE</button>
            </div>
            <div id="wifi_list" class="grid"></div>
        </div>

        <!-- BLE -->
        <div id="ble" class="content">
            <div class="card">
                <h2>BLE Chaos</h2>
                <button onclick="apiCall('/api/ble/spam?type=4', 'POST')">START SPAM (ALL)</button>
                <button class="red" onclick="apiCall('/api/ble/stop', 'POST')">STOP SPAM</button>
            </div>
            <div id="ble_list"></div>
        </div>

        <!-- IR -->
        <div id="ir" class="content">
            <div class="card">
                <h2>IR Blaster</h2>
                <div style="margin-bottom: 10px;">
                    <button class="red" style="width:100%" onclick="apiCall('/ir_nuke')">☢ TV NUKE ☢</button>
                </div>
                <h3>Remote Codes</h3>
                <div id="ir_grid" class="grid"></div>
            </div>
            <div class="card">
                <h2>Cloner/Learn</h2>
                <input type="text" id="learn_name" placeholder="Remote Name">
                <button onclick="learnIR()">START LEARNING</button>
            </div>
        </div>

        <!-- TOOLS -->
        <div id="tools" class="content">
             <div class="card">
                <h2>Terminal</h2>
                <input type="text" id="cmd_input" placeholder="Command...">
                <button onclick="sendCmd()">SEND</button>
            </div>
        </div>

        <div id="console"></div>
    </div>

    <script>
        function showTab(id) {
            document.querySelectorAll('.content').forEach(c => c.classList.remove('active'));
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            document.getElementById(id).classList.add('active');
            event.target.classList.add('active');
            if(id === 'ir') loadIRCodes();
            if(id === 'wifi') loadWiFi();
        }

        function log(msg) {
            const div = document.createElement('div');
            div.className = 'log-entry';
            div.innerHTML = `<span class="log-time">[${new Date().toLocaleTimeString()}]</span> ${msg}`;
            const c = document.getElementById('console');
            c.appendChild(div);
            c.scrollTop = c.scrollHeight;
        }

        function apiCall(endpoint, method='GET') {
            log(`${method} ${endpoint}...`);
            fetch(endpoint, { method: method })
                .then(r => r.text())
                .then(t => log(`Response: ${t}`))
                .catch(e => log(`Error: ${e}`));
        }

        function loadIRCodes() {
            fetch('/ir_list').then(r => r.json()).then(data => {
                const grid = document.getElementById('ir_grid');
                grid.innerHTML = '';
                data.codes.forEach(c => {
                    const btn = document.createElement('button');
                    btn.innerText = c.name;
                    btn.onclick = () => apiCall(`/ir_send?name=${encodeURIComponent(c.name)}`);
                    grid.appendChild(btn);
                });
            });
        }
        
        function loadWiFi() {
             fetch('/api/networks').then(r => r.json()).then(data => {
                const list = document.getElementById('wifi_list');
                list.innerHTML = '';
                if(data.networks) {
                    data.networks.forEach(n => {
                        const card = document.createElement('div');
                        card.className = 'card';
                        card.innerHTML = `<b>${n.ssid}</b><br>CH ${n.channel} | ${n.rssi}dBm`;
                        list.appendChild(card);
                    });
                }
            });
        }

        function learnIR() {
            const name = document.getElementById('learn_name').value || 'Unknown';
            apiCall('/ir_learn', 'POST'); // Incomplete in JS logic, usually sends body
        }

        setInterval(() => {
            fetch('/api/status').then(r => r.json()).then(data => {
                document.getElementById('stat_networks').innerText = data.networks;
                document.getElementById('stat_handshakes').innerText = data.handshakes;
                document.getElementById('stat_battery').innerText = data.battery + '%';
                document.getElementById('status').innerText = 'Connected';
                document.getElementById('status').style.color = '#0f0';
            }).catch(() => {
                document.getElementById('status').innerText = 'Disconnected';
                document.getElementById('status').style.color = '#f00';
            });
        }, 2000);
    </script>
</body>
</html>
)rawliteral";
