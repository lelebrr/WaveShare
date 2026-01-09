function send(cmd) {
  fetch('/ir_send?name=' + encodeURIComponent(cmd))
    .then(() => {
      const audio = new Audio('/sounds/sent.wav');
      audio.play();
    });
}

function tvNuke() {
  if (confirm("⚠️ TEM CERTEZA? VAI LIGAR TODAS AS TVs DA VIZINHANÇA AGORA?! ⚠️")) {
    fetch('/ir_nuke');
    document.body.style.background = "#300";
    setTimeout(() => document.body.style.background = "#000", 3000);
    alert("TV NUKE DISPARADO! TODAS AS TVs DA RUA LIGARAM! 🔥🔥🔥");
  }
}

// Carrega controles salvos + presets
fetch('/ir_list').then(r => r.json()).then(codes => {
  const container = document.getElementById('buttons');
  codes.forEach(c => {
    let card = document.createElement('div');
    card.className = 'card';

    let btn = document.createElement('button');
    btn.className = 'btn';

    if (c.name.includes("Power") || c.name.includes("Ligar")) {
      btn.className += ' power';
      btn.textContent = "🔴 " + c.name;
    } else if (c.name.includes("Netflix")) {
      btn.className += ' netflix';
      btn.textContent = "📺 " + c.name;
    } else if (c.name.includes("18") || c.name.includes("Ar")) {
      btn.className += ' ar';
      btn.textContent = "❄️ " + c.name;
    } else {
      btn.className += ' tv';
      btn.textContent = "📡 " + c.name;
    }

    btn.onclick = () => send(c.name);
    card.appendChild(btn);
    container.appendChild(card);
  });
});