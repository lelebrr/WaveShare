/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Multi-Mascot System
 * Dragon, Cat, Robot, Skull, Ghost, Phoenix
 * ═══════════════════════════════════════════════════════════════════════════
 */

const MascotSystem = {
    mascots: {
        dragon: {
            name: 'Neura (Dragão)',
            emoji: '🐉',
            moods: {
                idle: '🐲',
                happy: '🐉',
                hunting: '🔥',
                angry: '😤',
                sleeping: '😴',
                attacking: '⚡',
                success: '🎉',
                alert: '👀'
            },
            phrases: {
                idle: ['zzZ... WiFi... zzZ...', 'zzZ... WiFi... zzZ...', 'Aguardando ordens...', 'Bora hackear algo?', 'O espectro está calmo...'],
                hunting: ['Analisando espectro 2.4GHz...', 'Interceptando beacons...', 'Buscando alvos vulneráveis...', 'Rastreando mac address...'],
                success: ['WPA HANDSHAKE CAPTURADO! 🔓', 'Senha encontrada!', 'Arquivo salvo no SD!'],
                attack: ['INICIANDO DEAUTH FLOOD...', 'Desconectando clientes...', 'Injetando pacotes maliciosos...'],
                sleep: ['zzZ... WiFi... zzZ...', 'Standby...', 'Modo low power...']
            },
            color: '#00fff5'
        },
        cat: {
            name: 'Shadow (Gato)',
            emoji: '🐱',
            moods: {
                idle: '😺',
                happy: '😸',
                hunting: '😼',
                angry: '😾',
                sleeping: '😴',
                attacking: '🙀',
                success: '😻',
                alert: '🐱'
            },
            phrases: {
                idle: ['zzZ... Miau... zzZ...', 'zzZ... WiFi... zzZ...', 'Tédio...', 'Tem algum mouse por aí?', 'Esperando a presa...'],
                hunting: ['Farejando pacotes...', 'Caminhando silenciosamente...', 'Preparando o bote...', 'Observando tráfego...'],
                success: ['Peguei o handshake! 😺', 'Presa capturada!', 'Brinquedo novo!'],
                attack: ['ARRANHANDO A REDE! 😼', 'Derrubando conexão!', 'Caos felino iniciado!'],
                sleep: ['zzZ... Miau... zzZ...', 'Soneca...', 'Dormindo...']
            },
            color: '#ff00ff'
        },
        robot: {
            name: 'X-9000 (Robô)',
            emoji: '🤖',
            moods: {
                idle: '🤖',
                happy: '🤖',
                hunting: '🔎',
                angry: '⚠️',
                sleeping: '💤',
                attacking: '💥',
                success: '✅',
                alert: '🚨'
            },
            phrases: {
                idle: ['zzZ... SYSTEM... zzZ...', 'zzZ... WiFi... zzZ...', 'AGUARDANDO INPUT...', 'CPU EM IDLE...', 'READY FOR OP...'],
                hunting: ['EXECUTING SCAN_ROUTINE...', 'ANALYZING RF SPECTRUM...', 'DETECTING VULNERABILITIES...', 'CALCULATING VECTORS...'],
                success: ['HANDSHAKE_CAPTURED', 'HASH EXTRACTED', 'WRITE SUCCEEDED'],
                attack: ['EXECUTING DEAUTH_ATTACK...', 'FLOODING NETWORK...', 'PACKET INJECTION ACTIVE...'],
                sleep: ['zzZ... SYSTEM... zzZ...', 'HIBERNATION MODE...', 'LOW POWER...']
            },
            color: '#00ff9d'
        },
        skull: {
            name: 'Reaper (Caveira)',
            emoji: '💀',
            moods: {
                idle: '💀',
                happy: '☠️',
                hunting: '👻',
                angry: '😈',
                sleeping: '⚰️',
                attacking: '🔥',
                success: '🏴‍☠️',
                alert: '⚠️'
            },
            phrases: {
                idle: ['zzZ... Morte... zzZ...', 'zzZ... WiFi... zzZ...', 'A foice espera...', 'Tudo está muito quieto...', 'Buscando vítimas...'],
                hunting: ['Rastreando almas digitais...', 'Escolhendo o alvo...', 'Marcando para morrer...', 'Escutando o silêncio...'],
                success: ['ALMA CAPTURADA! 💀', 'RIP Rede...', 'Handshake coletado!'],
                attack: ['CEIFANDO CONEXÕES! 🔥', 'Derrubando tudo!', 'O fim chegou!'],
                sleep: ['zzZ... Túmulo... zzZ...', 'Descansando em paz...', 'Eternidade...']
            },
            color: '#ff0055'
        },
        ghost: {
            name: 'Phantom (Fantasma)',
            emoji: '👻',
            moods: {
                idle: '👻',
                happy: '😊',
                hunting: '🔍',
                angry: '😤',
                sleeping: '💤',
                attacking: '👹',
                success: '🎃',
                alert: '👀'
            },
            phrases: {
                idle: ['zzZ... Buuu... zzZ...', 'zzZ... WiFi... zzZ...', 'Invisível...', 'Assombrando o dashboard...', 'Tem alguém aí?'],
                hunting: ['Atravessando firewalls...', 'Flutuando na rede...', 'Espinonando pacotes...', 'Ninguém me vê...'],
                success: ['Assustei o handshake! 👻', 'Captura fantasmagórica!', 'Buuu! Peguei!'],
                attack: ['ASSOMBRAÇÃO EM MASSA! 👹', 'Poltergeist digital!', 'Medo na rede!'],
                sleep: ['zzZ... Ectoplasma... zzZ...', 'Sumindo...', 'Zzz...']
            },
            color: '#bd00ff'
        },
        phoenix: {
            name: 'Blaze (Fênix)',
            emoji: '🦅',
            moods: {
                idle: '🦅',
                happy: '🔥',
                hunting: '👁️',
                angry: '🌋',
                sleeping: '🌙',
                attacking: '☄️',
                success: '⭐',
                alert: '🔔'
            },
            phrases: {
                idle: ['zzZ... Cinzas... zzZ...', 'zzZ... WiFi... zzZ...', 'Chama piloto acesa...', 'Esperando o renascimento...', 'Aquecendo...'],
                hunting: ['Visão térmica ativa...', 'Sobrevoando a rede...', 'Detectando calor...', 'Alvo na mira...'],
                success: ['HANDSHAKE EM CHAMAS! 🔥', 'Vitória flamejante!', 'Renasceu o hash!'],
                attack: ['INCINERANDO CONEXÕES! ☄️', 'Chuva de fogo!', 'Ataque solar!'],
                sleep: ['zzZ... Brasas... zzZ...', 'Esfriando...', 'Zzz...']
            },
            color: '#ffcc00'
        }
    },

    currentMascot: 'dragon',
    currentMood: 'idle',

    init() {
        const saved = localStorage.getItem('wavepwn-mascot');
        if (saved && this.mascots[saved]) {
            this.currentMascot = saved;
        }
        this.render();
    },

    setMascot(mascotId) {
        if (this.mascots[mascotId]) {
            this.currentMascot = mascotId;
            localStorage.setItem('wavepwn-mascot', mascotId);
            this.render();
            console.log(`[Mascot] Changed to: ${this.mascots[mascotId].name}`);
        }
    },

    setMood(mood) {
        const mascot = this.mascots[this.currentMascot];
        if (mascot.moods[mood]) {
            this.currentMood = mood;
            this.render();
        }
    },

    getRandomPhrase(type = 'idle') {
        const mascot = this.mascots[this.currentMascot];
        const phrases = mascot.phrases[type] || mascot.phrases.idle;
        return phrases[Math.floor(Math.random() * phrases.length)];
    },

    render() {
        const mascot = this.mascots[this.currentMascot];

        // Update avatar
        const avatar = document.getElementById('dragon-avatar') || document.getElementById('mascot-avatar');
        if (avatar) {
            avatar.textContent = mascot.moods[this.currentMood] || mascot.emoji;
            avatar.style.filter = `drop-shadow(0 0 20px ${mascot.color})`;
        }

        // Update name
        const nameEl = document.getElementById('dragon-name') || document.getElementById('mascot-name');
        if (nameEl) {
            nameEl.textContent = mascot.name.split(' ')[0];
            nameEl.style.color = mascot.color;
        }

        // Update bubble
        const bubble = document.getElementById('dragon-bubble') || document.getElementById('mascot-bubble');
        if (bubble) {
            bubble.textContent = this.getRandomPhrase(this.currentMood);
            bubble.style.borderColor = mascot.color;
        }

        // Update selector if exists
        const selector = document.getElementById('mascot-selector');
        if (selector) {
            selector.value = this.currentMascot;
        }
    },

    react(event) {
        switch (event) {
            case 'handshake':
                this.setMood('success');
                setTimeout(() => this.setMood('happy'), 3000);
                break;
            case 'attack_start':
                this.setMood('attacking');
                break;
            case 'attack_stop':
                this.setMood('idle');
                break;
            case 'scan':
                this.setMood('hunting');
                break;
            case 'error':
                this.setMood('angry');
                setTimeout(() => this.setMood('idle'), 2000);
                break;
            case 'sleep':
                this.setMood('sleeping');
                break;
            default:
                this.setMood('idle');
        }
    },

    // Get all mascots for selector
    getAll() {
        return Object.entries(this.mascots).map(([id, mascot]) => ({
            id,
            name: mascot.name,
            emoji: mascot.emoji,
            color: mascot.color
        }));
    }
};

// Initialize on load
document.addEventListener('DOMContentLoaded', () => {
    MascotSystem.init();
});

// Exports
window.MascotSystem = MascotSystem;
window.setMascot = (id) => MascotSystem.setMascot(id);
window.mascotReact = (event) => MascotSystem.react(event);
