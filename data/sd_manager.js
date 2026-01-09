function bytesToSize(bytes) {
    const sizes = ['B', 'KB', 'MB', 'GB'];
    if (bytes == 0) return '0 B';
    const i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)));
    return Math.round(bytes / Math.pow(1024, i), 2) + ' ' + sizes[i];
}

function refresh() {
    fetch('/sd_json')
        .then(r => r.json())
        .then(data => {
            if (!data.online) {
                document.getElementById('sd_status').innerHTML = "SD CARD OFFLINE ou não detectado";
                document.getElementById('sd_info').style.display = "none";
                document.getElementById('filelist').style.display = "none";
                return;
            }

            document.getElementById('sd_status').innerHTML = "SD CARD ONLINE";
            document.getElementById('sd_status').className = "card";
            document.getElementById('sd_info').style.display = "block";
            document.getElementById('filelist').style.display = "table";

            document.getElementById('type').textContent = data.type;
            document.getElementById('speed').textContent = data.speed;
            document.getElementById('total').textContent = data.total_mb;
            document.getElementById('free').textContent = data.free_mb;

            const tbody = document.querySelector('#filelist tbody');
            tbody.innerHTML = '';

            data.files.forEach(file => {
                let tr = document.createElement('tr');

                let nameTd = document.createElement('td');
                nameTd.textContent = file.name;
                if (file.is_dir) nameTd.innerHTML += "/";

                let sizeTd = document.createElement('td');
                sizeTd.className = "size";
                sizeTd.textContent = file.is_dir ? "—" : bytesToSize(file.size);

                let actionTd = document.createElement('td');
                if (!file.is_dir) {
                    let dl = document.createElement('a');
                    dl.href = "/sd_download?file=" + encodeURIComponent(file.path);
                    dl.textContent = "Download";
                    dl.className = "btn";
                    actionTd.appendChild(dl);
                }
                let del = document.createElement('button');
                del.textContent = "Apagar";
                del.className = "btn danger";
                del.onclick = () => {
                    if (confirm("Apagar " + file.name + "?")) {
                        fetch('/sd_delete?file=' + encodeURIComponent(file.path));
                        setTimeout(refresh, 800);
                    }
                };
                actionTd.appendChild(del);

                tr.appendChild(nameTd);
                tr.appendChild(sizeTd);
                tr.appendChild(actionTd);
                tbody.appendChild(tr);
            });
        })
        .catch(() => {
            document.getElementById('sd_status').innerHTML = "Erro de comunicação";
        });
}

// Upload de arquivo
document.getElementById('upload').addEventListener('change', function (e) {
    let file = e.target.files[0];
    if (!file) return;
    let fr = new FileReader();
    fr.onload = function () {
        let xhr = new XMLHttpRequest();
        xhr.open('POST', '/sd_upload?filename=' + encodeURIComponent(file.name));
        xhr.send(fr.result);
        xhr.onload = () => {
            if (xhr.status == 200) alert("Upload OK!");
            refresh();
        };
    };
    fr.readAsArrayBuffer(file);
});

refresh();
setInterval(refresh, 10000); // atualiza a cada 10 s
