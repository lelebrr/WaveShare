/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Wallpapers Page Javascript
 * Gallery, upload, and management
 * ═══════════════════════════════════════════════════════════════════════════
 */

const WallpapersPage = {
    gallery: [],
    currentIndex: 0
};

async function initWallpapersPage() {
    console.log('[Wallpapers] Initializing...');
    await loadWallpaperGallery();
}

async function loadWallpaperGallery() {
    try {
        const response = await fetch('/api/wallpapers/list');
        const data = await response.json();
        WallpapersPage.gallery = data.wallpapers || [];
        renderWallpaperGallery();
    } catch (e) {
        console.error('[Wallpapers] Failed to load gallery:', e);
    }
}

function renderWallpaperGallery() {
    const container = document.getElementById('wallpaper-gallery');
    if (!container) return;

    container.innerHTML = '';

    if (WallpapersPage.gallery.length === 0) {
        container.innerHTML = '<p class="text-muted">Nenhum wallpaper encontrado. Faça upload!</p>';
        return;
    }

    WallpapersPage.gallery.forEach((wp, idx) => {
        const item = document.createElement('div');
        item.className = 'wallpaper-item' + (wp.current ? ' active' : '');
        item.innerHTML = `
            <img src="/wallpapers/${wp.filename}" alt="${wp.filename}" loading="lazy">
            <div class="wallpaper-info">
                <span class="name">${wp.filename}</span>
                <span class="size">${formatBytes(wp.size)}</span>
            </div>
            <div class="wallpaper-actions">
                <button class="btn btn-sm btn-primary" onclick="setWallpaper('${wp.filename}')">Usar</button>
                <button class="btn btn-sm btn-danger" onclick="deleteWallpaper('${wp.filename}')">🗑️</button>
            </div>
        `;
        container.appendChild(item);
    });
}

async function setWallpaper(filename) {
    try {
        await fetch('/api/wallpapers/set', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ filename })
        });
        addLog(`Wallpaper alterado: ${filename}`, 'success');
        await loadWallpaperGallery();
    } catch (e) {
        addLog('Erro ao definir wallpaper', 'error');
    }
}

async function deleteWallpaper(filename) {
    if (!confirm(`Deletar wallpaper: ${filename}?`)) return;

    try {
        await fetch('/api/wallpapers/delete', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ filename })
        });
        addLog(`Wallpaper deletado: ${filename}`, 'success');
        await loadWallpaperGallery();
    } catch (e) {
        addLog('Erro ao deletar wallpaper', 'error');
    }
}

async function uploadWallpaper(file) {
    if (!file) return;
    if (!file.name.endsWith('.png') && !file.name.endsWith('.PNG')) {
        addLog('Apenas arquivos PNG são aceitos', 'error');
        return;
    }
    if (file.size > 150 * 1024) {
        addLog('Arquivo muito grande (max 150KB)', 'error');
        return;
    }

    const formData = new FormData();
    formData.append('wallpaper', file);

    const progressEl = document.getElementById('wallpaper-upload-progress');
    if (progressEl) progressEl.style.display = 'block';

    try {
        const response = await fetch('/api/wallpapers/upload', {
            method: 'POST',
            body: formData
        });

        if (response.ok) {
            addLog(`Wallpaper enviado: ${file.name}`, 'success');
            await loadWallpaperGallery();
        } else {
            throw new Error('Upload failed');
        }
    } catch (e) {
        addLog('Erro ao enviar wallpaper', 'error');
    } finally {
        if (progressEl) progressEl.style.display = 'none';
    }
}

// Drag and drop support
function initWallpaperDropZone() {
    const dropZone = document.getElementById('wallpaper-drop-zone');
    if (!dropZone) return;

    ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(evName => {
        dropZone.addEventListener(evName, e => {
            e.preventDefault();
            e.stopPropagation();
        });
    });

    ['dragenter', 'dragover'].forEach(evName => {
        dropZone.addEventListener(evName, () => dropZone.classList.add('drag-over'));
    });

    ['dragleave', 'drop'].forEach(evName => {
        dropZone.addEventListener(evName, () => dropZone.classList.remove('drag-over'));
    });

    dropZone.addEventListener('drop', e => {
        const files = e.dataTransfer.files;
        if (files.length > 0) uploadWallpaper(files[0]);
    });
}

function formatBytes(bytes) {
    if (bytes < 1024) return bytes + ' B';
    if (bytes < 1048576) return (bytes / 1024).toFixed(1) + ' KB';
    return (bytes / 1048576).toFixed(1) + ' MB';
}

// Export
window.initWallpapersPage = initWallpapersPage;
window.loadWallpaperGallery = loadWallpaperGallery;
window.setWallpaper = setWallpaper;
window.deleteWallpaper = deleteWallpaper;
window.uploadWallpaper = uploadWallpaper;
