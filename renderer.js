const information = document.getElementById('info')

information.innerHTML = `Este app esta usando Chrome (v${versions.chrome()}), Node.js (v${versions.node()}) e Electron (v${versions.electron()})`