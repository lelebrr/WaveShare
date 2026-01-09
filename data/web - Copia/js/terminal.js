/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Terminal Real com xterm.js (Dica 21)
 * Terminal interativo com execução de comandos reais no ESP32
 * ═══════════════════════════════════════════════════════════════════════════
 */

// Terminal instance
let terminal = null;
let terminalFitAddon = null;
const terminalHistory = [];
let historyIndex = -1;

// Available commands
const COMMANDS = {
    help: {
        description: 'Mostra lista de comandos',
        handler: showHelp
    },
    scan: {
        description: 'Inicia scan de redes WiFi',
        handler: () => sendTerminalCommand('scan')
    },
    networks: {
        description: 'Lista redes encontradas',
        handler: () => sendTerminalCommand('networks')
    },
    deauth: {
        description: 'Inicia ataque deauth [bssid]',
        handler: (args) => sendTerminalCommand('deauth', { target: args[0] })
    },
    stop: {
        description: 'Para todos os ataques',
        handler: () => sendTerminalCommand('stop')
    },
    status: {
        description: 'Mostra status do sistema',
        handler: () => sendTerminalCommand('status')
    },
    beacon: {
        description: 'Beacon spam [count]',
        handler: (args) => sendTerminalCommand('beacon', { count: args[0] || 50 })
    },
    ble_spam: {
        description: 'Inicia BLE spam [type]',
        handler: (args) => sendTerminalCommand('ble_spam', { type: args[0] || 'apple' })
    },
    handshakes: {
        description: 'Lista handshakes capturados',
        handler: () => sendTerminalCommand('handshakes')
    },
    clear: {
        description: 'Limpa o terminal',
        handler: clearTerminal
    },
    reboot: {
        description: 'Reinicia o dispositivo',
        handler: () => {
            terminalPrint('\x1b[33m⚠️ Reiniciando em 3 segundos...\x1b[0m');
            setTimeout(() => sendTerminalCommand('reboot'), 3000);
        }
    },
    theme: {
        description: 'Muda o tema [nome]',
        handler: (args) => {
            if (args[0] && typeof setTheme === 'function') {
                setTheme(args[0]);
                terminalPrint(`\x1b[32m✓ Tema alterado para: ${args[0]}\x1b[0m`);
            } else {
                terminalPrint('\x1b[33mTemas: cyberpunk, hacker, stealth, light, dracula, nord, monokai\x1b[0m');
            }
        }
    },
    whoami: {
        description: 'Mostra informações do sistema',
        handler: () => {
            terminalPrint('\x1b[36m╔════════════════════════════════════╗\x1b[0m');
            terminalPrint('\x1b[36m║\x1b[0m  \x1b[1m🐉 LeleWatch / WavePwn\x1b[0m');
            terminalPrint('\x1b[36m║\x1b[0m  Hardware: ESP32-S3 + AMOLED 1.8"');
            terminalPrint('\x1b[36m║\x1b[0m  Firmware: v2.0 Cyberpunk Edition');
            terminalPrint('\x1b[36m║\x1b[0m  AI: NEURA9 TensorFlow Lite');
            terminalPrint('\x1b[36m╚════════════════════════════════════╝\x1b[0m');
        }
    },
    dragon: {
        description: 'Faz o dragão falar [msg]',
        handler: (args) => {
            const msg = args.join(' ') || 'Olá, mestre!';
            if (typeof dragonSay === 'function') {
                dragonSay(msg);
                terminalPrint(`\x1b[35m🐲 Dragão diz: ${msg}\x1b[0m`);
            }
        }
    },
    pwn: {
        description: 'PWN EVERYTHING - Ativa todos os ataques',
        handler: () => {
            terminalPrint('\x1b[31m💀 PWN EVERYTHING ACTIVATED!\x1b[0m');
            terminalPrint('\x1b[33m⚠️ Iniciando todos os ataques em 5s...\x1b[0m');
            sendTerminalCommand('pwn_all');
        }
    },
    cinema: {
        description: 'Ativa modo cinema',
        handler: () => {
            if (typeof toggleCinemaMode === 'function') {
                toggleCinemaMode();
            }
        }
    },
    matrix: {
        description: 'Easter egg - modo matrix',
        handler: () => {
            if (typeof setTheme === 'function') {
                setTheme('hacker');
            }
            terminalPrint('\x1b[32mWake up, Neo...\x1b[0m');
            terminalPrint('\x1b[32mThe Matrix has you...\x1b[0m');
            terminalPrint('\x1b[32mFollow the white rabbit. 🐇\x1b[0m');
        }
    }
};

/**
 * Initialize xterm.js terminal
 */
function initXterm() {
    // Check if xterm is available
    if (typeof Terminal === 'undefined') {
        console.log('[Terminal] xterm.js not loaded, using fallback');
        return;
    }

    const container = document.getElementById('xterm-container');
    if (!container) {
        console.log('[Terminal] Container not found');
        return;
    }

    // Create terminal instance
    terminal = new Terminal({
        theme: getTerminalTheme(),
        fontFamily: "'Share Tech Mono', 'Courier New', monospace",
        fontSize: 14,
        cursorBlink: true,
        cursorStyle: 'block',
        scrollback: 1000,
        rows: 20
    });

    // Load fit addon if available
    if (typeof FitAddon !== 'undefined') {
        terminalFitAddon = new FitAddon.FitAddon();
        terminal.loadAddon(terminalFitAddon);
    }

    // Open terminal
    terminal.open(container);

    // Fit to container
    if (terminalFitAddon) {
        terminalFitAddon.fit();
        window.addEventListener('resize', () => terminalFitAddon.fit());
    }

    // Welcome message
    showWelcome();

    // Input handling
    let currentLine = '';
    terminal.onKey(({ key, domEvent }) => {
        const ev = domEvent;
        const printable = !ev.altKey && !ev.ctrlKey && !ev.metaKey;

        if (ev.key === 'Enter') {
            terminal.write('\r\n');
            processCommand(currentLine);
            terminalHistory.push(currentLine);
            historyIndex = terminalHistory.length;
            currentLine = '';
            showPrompt();
        } else if (ev.key === 'Backspace') {
            if (currentLine.length > 0) {
                currentLine = currentLine.slice(0, -1);
                terminal.write('\b \b');
            }
        } else if (ev.key === 'ArrowUp') {
            if (historyIndex > 0) {
                historyIndex--;
                clearCurrentLine(currentLine);
                currentLine = terminalHistory[historyIndex] || '';
                terminal.write(currentLine);
            }
        } else if (ev.key === 'ArrowDown') {
            if (historyIndex < terminalHistory.length - 1) {
                historyIndex++;
                clearCurrentLine(currentLine);
                currentLine = terminalHistory[historyIndex] || '';
                terminal.write(currentLine);
            }
        } else if (ev.key === 'Tab') {
            // Auto-complete
            const matches = Object.keys(COMMANDS).filter(cmd => cmd.startsWith(currentLine));
            if (matches.length === 1) {
                clearCurrentLine(currentLine);
                currentLine = matches[0];
                terminal.write(currentLine);
            } else if (matches.length > 1) {
                terminal.write('\r\n');
                terminalPrint(matches.join('  '));
                showPrompt();
                terminal.write(currentLine);
            }
        } else if (printable) {
            currentLine += key;
            terminal.write(key);
        }
    });

    console.log('[Terminal] xterm.js initialized');
}

function clearCurrentLine(line) {
    for (let i = 0; i < line.length; i++) {
        terminal.write('\b \b');
    }
}

function getTerminalTheme() {
    const theme = document.documentElement.getAttribute('data-theme') || 'cyberpunk';

    const themes = {
        cyberpunk: {
            background: '#0a0a0f',
            foreground: '#00fff5',
            cursor: '#ff00ff',
            cursorAccent: '#0a0a0f',
            selection: 'rgba(0, 255, 245, 0.3)',
            black: '#000000',
            red: '#ff0055',
            green: '#00ff9d',
            yellow: '#ffcc00',
            blue: '#00a8ff',
            magenta: '#ff00ff',
            cyan: '#00fff5',
            white: '#ffffff'
        },
        hacker: {
            background: '#000000',
            foreground: '#00ff00',
            cursor: '#00ff00',
            cursorAccent: '#000000',
            selection: 'rgba(0, 255, 0, 0.3)',
            black: '#000000',
            red: '#ff0000',
            green: '#00ff00',
            yellow: '#99ff00',
            blue: '#00ff88',
            magenta: '#00cc00',
            cyan: '#00ff00',
            white: '#00ff00'
        },
        dracula: {
            background: '#282a36',
            foreground: '#f8f8f2',
            cursor: '#f8f8f2',
            cursorAccent: '#282a36',
            selection: 'rgba(189, 147, 249, 0.3)',
            black: '#21222c',
            red: '#ff5555',
            green: '#50fa7b',
            yellow: '#f1fa8c',
            blue: '#6272a4',
            magenta: '#ff79c6',
            cyan: '#8be9fd',
            white: '#f8f8f2'
        },
        light: {
            background: '#f5f7fa',
            foreground: '#1e293b',
            cursor: '#2563eb',
            cursorAccent: '#f5f7fa',
            selection: 'rgba(37, 99, 235, 0.2)',
            black: '#1e293b',
            red: '#ef4444',
            green: '#10b981',
            yellow: '#f59e0b',
            blue: '#3b82f6',
            magenta: '#7c3aed',
            cyan: '#0891b2',
            white: '#f8fafc'
        }
    };

    return themes[theme] || themes.cyberpunk;
}

function showWelcome() {
    terminalPrint('\x1b[36m╔══════════════════════════════════════════════════════════╗\x1b[0m');
    terminalPrint('\x1b[36m║\x1b[0m  \x1b[1;35m██╗    ███████╗██╗     ███████╗██╗    ██╗ █████╗ ████████╗ ██████╗██╗  ██╗\x1b[0m');
    terminalPrint('\x1b[36m║\x1b[0m  \x1b[1;35m██║    ██╔════╝██║     ██╔════╝██║    ██║██╔══██╗╚══██╔══╝██╔════╝██║  ██║\x1b[0m');
    terminalPrint('\x1b[36m║\x1b[0m  \x1b[1;35m██║    █████╗  ██║     █████╗  ██║ █╗ ██║███████║   ██║   ██║     ███████║\x1b[0m');
    terminalPrint('\x1b[36m║\x1b[0m  \x1b[1;35m██║    ██╔══╝  ██║     ██╔══╝  ██║███╗██║██╔══██║   ██║   ██║     ██╔══██║\x1b[0m');
    terminalPrint('\x1b[36m║\x1b[0m  \x1b[1;35m███████╗███████╗███████╗███████╗╚███╔███╔╝██║  ██║   ██║   ╚██████╗██║  ██║\x1b[0m');
    terminalPrint('\x1b[36m║\x1b[0m  \x1b[1;35m╚══════╝╚══════╝╚══════╝╚══════╝ ╚══╝╚══╝ ╚═╝  ╚═╝   ╚═╝    ╚═════╝╚═╝  ╚═╝\x1b[0m');
    terminalPrint('\x1b[36m╚══════════════════════════════════════════════════════════╝\x1b[0m');
    terminalPrint('');
    terminalPrint('\x1b[33m  🐉 LeleWatch Terminal v2.0 - Type "help" for commands\x1b[0m');
    terminalPrint('');
    showPrompt();
}

function showPrompt() {
    if (terminal) {
        terminal.write('\x1b[32mdragon@lelewatch\x1b[0m:\x1b[34m~\x1b[0m$ ');
    }
}

function terminalPrint(text) {
    if (terminal) {
        terminal.writeln(text);
    }
}

function clearTerminal() {
    if (terminal) {
        terminal.clear();
        showWelcome();
    }
}

function processCommand(input) {
    const parts = input.trim().split(/\s+/);
    const cmd = parts[0].toLowerCase();
    const args = parts.slice(1);

    if (!cmd) return;

    if (COMMANDS[cmd]) {
        COMMANDS[cmd].handler(args);
    } else {
        terminalPrint(`\x1b[31m❌ Comando não encontrado: ${cmd}\x1b[0m`);
        terminalPrint('\x1b[33mDigite "help" para ver os comandos disponíveis\x1b[0m');
    }
}

function showHelp() {
    terminalPrint('');
    terminalPrint('\x1b[1;36m📋 Comandos Disponíveis:\x1b[0m');
    terminalPrint('\x1b[36m─────────────────────────────────────────\x1b[0m');

    Object.entries(COMMANDS).forEach(([name, cmd]) => {
        terminalPrint(`  \x1b[32m${name.padEnd(12)}\x1b[0m ${cmd.description}`);
    });

    terminalPrint('');
    terminalPrint('\x1b[33mDica: Use TAB para auto-completar, ↑↓ para histórico\x1b[0m');
}

async function sendTerminalCommand(command, params = {}) {
    try {
        terminalPrint(`\x1b[90m[Enviando: ${command}...]\x1b[0m`);

        const response = await fetch(`/api/terminal`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ command, ...params })
        });

        const data = await response.json();

        if (data.output) {
            data.output.split('\n').forEach(line => {
                terminalPrint(line);
            });
        }

        if (data.error) {
            terminalPrint(`\x1b[31m❌ ${data.error}\x1b[0m`);
        }

        if (data.success) {
            terminalPrint(`\x1b[32m✓ ${data.message || 'Comando executado'}\x1b[0m`);
        }
    } catch (error) {
        terminalPrint(`\x1b[31m❌ Erro de conexão: ${error.message}\x1b[0m`);
    }
}

// Handle terminal output from WebSocket
function handleTerminalOutput(output) {
    if (terminal && output) {
        output.split('\n').forEach(line => {
            terminalPrint(line);
        });
    }
}

// Update terminal theme when global theme changes
function updateTerminalTheme() {
    if (terminal) {
        terminal.options.theme = getTerminalTheme();
    }
}

// Export functions
window.initXterm = initXterm;
window.terminalPrint = terminalPrint;
window.handleTerminalOutput = handleTerminalOutput;
window.updateTerminalTheme = updateTerminalTheme;
window.clearTerminal = clearTerminal;
