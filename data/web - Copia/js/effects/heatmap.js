/**
 * WAVEPWN - WiFi Heatmap (Dicas 9, 22)
 * Mapa de calor visual de redes WiFi por canal
 */

const WiFiHeatmap = {
    canvas: null,
    ctx: null,
    networks: [],
    animationId: null
};

function initWiFiHeatmap(canvasId) {
    WiFiHeatmap.canvas = document.getElementById(canvasId);
    if (!WiFiHeatmap.canvas) return;
    WiFiHeatmap.ctx = WiFiHeatmap.canvas.getContext('2d');
    resizeHeatmap();
    window.addEventListener('resize', resizeHeatmap);
    startHeatmapAnimation();
}

function resizeHeatmap() {
    if (!WiFiHeatmap.canvas) return;
    const parent = WiFiHeatmap.canvas.parentElement;
    WiFiHeatmap.canvas.width = parent.offsetWidth;
    WiFiHeatmap.canvas.height = parent.offsetHeight || 300;
}

function updateHeatmapData(networks) {
    WiFiHeatmap.networks = networks || [];
}

function startHeatmapAnimation() {
    function animate() {
        drawHeatmap();
        WiFiHeatmap.animationId = requestAnimationFrame(animate);
    }
    animate();
}

function drawHeatmap() {
    const { canvas, ctx, networks } = WiFiHeatmap;
    if (!ctx) return;

    const w = canvas.width;
    const h = canvas.height;

    // Clear with dark background
    ctx.fillStyle = 'rgba(10, 10, 15, 0.1)';
    ctx.fillRect(0, 0, w, h);

    // Draw channel grid
    ctx.strokeStyle = 'rgba(0, 255, 245, 0.1)';
    ctx.lineWidth = 1;
    for (let i = 1; i <= 14; i++) {
        const x = (i / 15) * w;
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, h);
        ctx.stroke();
        ctx.fillStyle = 'rgba(0, 255, 245, 0.3)';
        ctx.font = '10px monospace';
        ctx.fillText(`CH${i}`, x - 10, h - 5);
    }

    // Draw networks as heat blobs
    networks.forEach((net, idx) => {
        const channel = net.channel || 6;
        const rssi = net.rssi || -70;
        const x = (channel / 15) * w;
        const strength = Math.min(100, Math.max(0, 100 + rssi));
        const radius = 20 + (strength / 100) * 60;

        // Gradient blob
        const gradient = ctx.createRadialGradient(x, h / 2, 0, x, h / 2, radius);
        const hue = (idx * 30) % 360;
        gradient.addColorStop(0, `hsla(${hue}, 100%, 50%, 0.8)`);
        gradient.addColorStop(0.5, `hsla(${hue}, 100%, 50%, 0.3)`);
        gradient.addColorStop(1, `hsla(${hue}, 100%, 50%, 0)`);

        ctx.fillStyle = gradient;
        ctx.beginPath();
        ctx.arc(x, h / 2 + Math.sin(Date.now() / 1000 + idx) * 10, radius, 0, Math.PI * 2);
        ctx.fill();

        // SSID label
        if (net.ssid) {
            ctx.fillStyle = '#fff';
            ctx.font = '11px sans-serif';
            ctx.fillText(net.ssid.substring(0, 12), x - 30, h / 2);
        }
    });
}

function stopHeatmap() {
    if (WiFiHeatmap.animationId) {
        cancelAnimationFrame(WiFiHeatmap.animationId);
    }
}

window.initWiFiHeatmap = initWiFiHeatmap;
window.updateHeatmapData = updateHeatmapData;
window.stopHeatmap = stopHeatmap;
