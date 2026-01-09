/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Matrix Rain Effect
 * Lightweight canvas-based Matrix digital rain
 * ═══════════════════════════════════════════════════════════════════════════
 */

function initMatrixRain() {
    const canvas = document.getElementById('matrix-canvas');
    if (!canvas) {
        console.log('[Matrix] Canvas not found, creating...');
        const newCanvas = document.createElement('canvas');
        newCanvas.id = 'matrix-canvas';
        const container = document.getElementById('background-effects');
        if (container) {
            container.appendChild(newCanvas);
            initMatrixRain();
        }
        return;
    }

    const ctx = canvas.getContext('2d');

    // Set canvas size
    function resize() {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    resize();
    window.addEventListener('resize', resize);

    // Characters to display (mix of Latin and Katakana)
    const chars = 'ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    const charArray = chars.split('');

    const fontSize = 14;
    let columns = Math.floor(canvas.width / fontSize);
    let drops = Array(columns).fill(1);

    // Get theme color
    function getMatrixColor() {
        const theme = document.documentElement.getAttribute('data-theme');
        switch (theme) {
            case 'hacker': return '#00ff00';
            case 'stealth': return '#39ff14';
            default: return '#00fff5';
        }
    }

    function draw() {
        // Semi-transparent black to create trail effect
        ctx.fillStyle = 'rgba(10, 10, 15, 0.05)';
        ctx.fillRect(0, 0, canvas.width, canvas.height);

        ctx.fillStyle = getMatrixColor();
        ctx.font = `${fontSize}px 'Share Tech Mono', monospace`;

        for (let i = 0; i < drops.length; i++) {
            // Random character
            const char = charArray[Math.floor(Math.random() * charArray.length)];

            // Draw character
            ctx.fillText(char, i * fontSize, drops[i] * fontSize);

            // Reset drop to top randomly after reaching bottom
            if (drops[i] * fontSize > canvas.height && Math.random() > 0.975) {
                drops[i] = 0;
            }

            drops[i]++;
        }
    }

    // Handle resize
    window.addEventListener('resize', () => {
        columns = Math.floor(canvas.width / fontSize);
        drops = Array(columns).fill(1);
    });

    // Animation loop - slower for performance
    setInterval(draw, 50);

    console.log('[Matrix] Rain initialized');
}

// Export
window.initMatrixRain = initMatrixRain;
