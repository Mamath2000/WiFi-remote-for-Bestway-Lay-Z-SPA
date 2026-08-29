var connection

const MAX_LOG_LINES = 500

loadDebugConfig()
connect()

function loadDebugConfig() {
    var req = new XMLHttpRequest()
    req.open('POST', '/getdebug/')
    req.send()
    req.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var json = JSON.parse(req.responseText)
            document.getElementById('mqttDebug').checked = !!json.mqttDebug
            document.getElementById('webDebug').checked = !!json.webDebug
        }
    }
}

function saveDebugConfig() {
    buttonConfirm(document.getElementById('save'))

    var req = new XMLHttpRequest()
    req.open('POST', '/setdebug/')
    var json = {
        mqttDebug: document.getElementById('mqttDebug').checked,
        webDebug: document.getElementById('webDebug').checked
    }
    req.send(JSON.stringify(json))
}

function clearLog() {
    document.getElementById('logbox').innerHTML = ''
}

function connect() {
    connection = new WebSocket("ws://" + location.hostname + ":81/", ["arduino"])

    connection.onclose = function () {
        setTimeout(function () {
            connect()
        }, 5000)
    }

    connection.onmessage = function (e) {
        handlemsg(e)
    }
}

function formatUptime(ms) {
    var totalSeconds = Math.floor(ms / 1000)
    var h = String(Math.floor(totalSeconds / 3600)).padStart(2, '0')
    var m = String(Math.floor((totalSeconds % 3600) / 60)).padStart(2, '0')
    var s = String(totalSeconds % 60).padStart(2, '0')
    return h + ':' + m + ':' + s
}

function tryParseJSONObject(jsonString) {
    try {
        var o = JSON.parse(jsonString)
        if (o && typeof o === "object") {
            return o
        }
    } catch (e) { }
    return false
}

function handlemsg(e) {
    var msgobj = tryParseJSONObject(e.data)
    if (!msgobj || msgobj.CONTENT != "LOG") return

    var logbox = document.getElementById('logbox')
    var atBottom = logbox.scrollTop + logbox.clientHeight >= logbox.scrollHeight - 4

    var line = document.createElement('div')
    line.className = 'logline log-' + msgobj.lvl
    // msgobj.ts is millis() since the ESP booted, not a wall-clock timestamp.
    line.textContent = '[' + formatUptime(msgobj.ts) + '] [' + msgobj.tag + '] ' + msgobj.msg
    logbox.appendChild(line)

    while (logbox.childElementCount > MAX_LOG_LINES) {
        logbox.removeChild(logbox.firstChild)
    }

    if (atBottom) logbox.scrollTop = logbox.scrollHeight
}
