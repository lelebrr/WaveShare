/**
 * WAVEPWN - Mascot Selector Web Component
 * Sistema de seleção de mascotes para o dashboard
 */

const MascotSelector = {
    mascots: [],
    current: null
};

async function initMascotSelector() {
    await loadMascotList();
    await loadCurrentMascot();
    console.log('[Mascot] Selector initialized');
}

async function loadMascotList() {
    try {
        const response = await fetch('/api/mascot/list');
        MascotSelector.mascots = await response.json();
    } catch (e) {
        // Use defaults
        MascotSelector.mascots = [
            { id: 0, type: 'dragon', name: 'Neura', emoji: '🐉', description: 'Dragão cyber' },
            { id: 1, type: 'cat', name: 'Hackito', emoji: '🐱', description: 'Gato hacker' },
            { id: 2, type: 'fox', name: 'Vulpix', emoji: '🦊', description: 'Raposa cyber' },
            { id: 3, type: 'wolf', name: 'Byte', emoji: '🐺', description: 'Lobo digital' },
            { id: 4, type: 'lizard', name: 'Pixel', emoji: '🦎', description: 'Lagarto pixel' },
            { id: 5, type: 'octopus', name: 'Octo', emoji: '🐙', description: 'Polvo multitarefa' },
            { id: 6, type: 'robot', name: 'Robo-9', emoji: '🤖', description: 'Robô retro' },
            { id: 7, type: 'alien', name: 'Zyx', emoji: '👾', description: 'Alien 8-bit' }
        ];
    }
}

async function loadCurrentMascot() {
    try {
        const response = await fetch('/api/mascot/current');
        MascotSelector.current = await response.json();
    } catch (e) {
        MascotSelector.current = { id: 0, name: 'Neura' };
    }
}

function renderMascotSelector(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
        <div class="mascot-selector">
            <h3>🐉 Escolha seu Mascote</h3>
            
            <div class="mascot-current">
                <div class="current-preview">
                    <span class="mascot-emoji-big">${MascotSelector.current?.emoji || '🐉'}</span>
                </div>
                <div class="current-info">
                    <input type="text" id="mascot-name" value="${MascotSelector.current?.name || 'Neura'}" 
                           maxlength="16" placeholder="Nome do mascote">
                    <button class="btn btn-sm btn-primary" onclick="saveMascotName()">Salvar</button>
                </div>
            </div>
            
            <div class="mascot-grid">
                ${MascotSelector.mascots.map(m => `
                    <div class="mascot-card ${m.id === MascotSelector.current?.id ? 'selected' : ''}" 
                         onclick="selectMascot(${m.id})">
                        <div class="mascot-emoji">${m.emoji}</div>
                        <div class="mascot-name">${m.name}</div>
                        <div class="mascot-desc">${m.description}</div>
                    </div>
                `).join('')}
            </div>
        </div>
    `;
}

async function selectMascot(id) {
    try {
        await fetch('/api/mascot/select', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: `id=${id}`
        });
        await loadCurrentMascot();
        renderMascotSelector('mascot-container');
        if (typeof addLog === 'function') addLog('Mascote alterado!', 'success');
    } catch (e) {
        console.error('[Mascot] Select failed', e);
    }
}

async function saveMascotName() {
    const input = document.getElementById('mascot-name');
    if (!input) return;

    try {
        await fetch('/api/mascot/rename', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: `name=${encodeURIComponent(input.value)}`
        });
        await loadCurrentMascot();
        if (typeof addLog === 'function') addLog('Nome salvo!', 'success');
    } catch (e) {
        console.error('[Mascot] Rename failed', e);
    }
}

// Inject styles
function injectMascotStyles() {
    if (document.getElementById('mascot-styles')) return;
    const style = document.createElement('style');
    style.id = 'mascot-styles';
    style.textContent = `
        .mascot-selector h3 { margin-bottom: 20px; }
        .mascot-current { display: flex; align-items: center; gap: 20px; margin-bottom: 20px; padding: 20px; background: var(--bg-card); border-radius: var(--border-radius); }
        .current-preview { width: 80px; height: 80px; display: flex; align-items: center; justify-content: center; background: var(--bg-tertiary); border-radius: 50%; }
        .mascot-emoji-big { font-size: 48px; }
        .current-info { display: flex; flex-direction: column; gap: 10px; }
        .mascot-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 15px; }
        .mascot-card { padding: 15px; background: var(--bg-card); border: 2px solid var(--border-color); border-radius: var(--border-radius); cursor: pointer; text-align: center; transition: all 0.2s; }
        .mascot-card:hover { border-color: var(--primary); transform: scale(1.05); }
        .mascot-card.selected { border-color: var(--primary); box-shadow: 0 0 15px var(--primary-dim); }
        .mascot-emoji { font-size: 36px; margin-bottom: 10px; }
        .mascot-name { font-weight: 600; color: var(--text-primary); }
        .mascot-desc { font-size: 0.75rem; color: var(--text-muted); margin-top: 5px; }
        @media (max-width: 600px) { .mascot-grid { grid-template-columns: repeat(2, 1fr); } }
    `;
    document.head.appendChild(style);
}

document.addEventListener('DOMContentLoaded', injectMascotStyles);

window.initMascotSelector = initMascotSelector;
window.renderMascotSelector = renderMascotSelector;
window.selectMascot = selectMascot;
window.saveMascotName = saveMascotName;
