// the web socket connection
var connection

// command mapping
const cmdMap = {
    setTarget: 0,
    setTargetSelector: 0,
    toggleUnit: 1,
    toggleBubbles: 2,
    toggleHeater: 3,
    togglePump: 4,
    //resetq: 5,
    restartEsp: 6,
    //gettarget: 7,
    resetTotals: 8,
    toggleHydroJets: 11,
    setBrightness: 12,
    setBrightnessSelector: 12,
    setBeep: 13,
    resetDaily: 16,
    toggleGodmode: 17,
    setFullpower: 18,
    printText: 19
}

// button element ID mapping
const btnMap = {
    toggleUnit: "UNT",
    toggleBubbles: "AIR",
    toggleHeater: "HTR",
    togglePump: "FLT",
    toggleHydroJets: "HJT",
    toggleGodmode: "GOD"
}

// to be used for setting the control values once after loading original values from the web socket
var initControlValues = true

// display brightness multiplier. lower value results lower brightness levels (1-30)
const dspBrtMultiplier = 16

// update states
updateTempState = false
updateBrtState = false

// initial connect to the web socket
connect()

function resetTotals()
{
	if (confirm('Do you really want to reset the totals?'))
	{
		buttonConfirm(document.getElementById('reset'));
		sendCommand('resetTotals');
	}
}
function connect() {
    connection = new WebSocket("ws://" + location.hostname + ":81/", ["arduino"])

    connection.onopen = function () {
        document.body.classList.add("connected")
        initControlValues = true
    }

    connection.onerror = function (error) {
        console.log("WebSocket Error ", error)
        document.body.classList.add("error")
        connection.close()
    }

    connection.onclose = function () {
        console.log("WebSocket connection closed, reconnecting in 5 s")
        document.body.classList.add("error")
        setTimeout(function () {
            connect()
        }, 5000)
    }

    connection.onmessage = function (e) {
        handlemsg(e)
    }
}

String.prototype.pad = function (String, len) {
    var str = this
    while (str.length < len) {
        str = String + str
    }
    return str
}

function tryParseJSONObject(jsonString) {
    try {
        var o = JSON.parse(jsonString)

        // Handle non-exception-throwing cases:
        // Neither JSON.parse(false) or JSON.parse(1234) throw errors, hence the type-checking,
        // but... JSON.parse(null) returns null, and typeof null === "object",
        // so we must check for that, too. Thankfully, null is falsey, so this suffices:
        if (o && typeof o === "object") {
            return o
        }
    } catch (e) { }

    return false
}

function handlemsg(e) {
    console.log(e.data)
    var msgobj = tryParseJSONObject(e.data)
    if (!msgobj) return
    console.log(msgobj)

    if (msgobj.CONTENT == "OTHER") {
        // MQTT status
        mqtt_states = [
            "CONNECTION_TIMEOUT", // -4 / the server didn't respond within the keepalive time
            "CONNECTION_LOST", // -3 / the network connection was broken
            "CONNECT_FAILED", // -2 / the network connection failed
            "DISCONNECTED", // -1 / the client is disconnected cleanly
            "CONNECTED", // 0 / the client is connected
            "CONNECT_BAD_PROTOCOL", // 1 / the server doesn't support the requested version of MQTT
            "CONNECT_BAD_CLIENT_ID", // 2 / the server rejected the client identifier
            "CONNECT_UNAVAILABLE", // 3 / the server was unable to accept the connection
            "CONNECT_BAD_CREDENTIALS", // 4 / the username/password were rejected
            "CONNECT_UNAUTHORIZED", // 5 / the client was not authorized to connect
        ]
        document.getElementById("mqtt").innerHTML = "MQTT: " + mqtt_states[msgobj.MQTT + 4]
        var spalinkEl = document.getElementById("spalink")
        spalinkEl.innerHTML = "Spa link: " + (msgobj.SPALINK ? "OK" : "check wiring")
        spalinkEl.classList.toggle("status-ok", !!msgobj.SPALINK)
        spalinkEl.classList.toggle("status-bad", !msgobj.SPALINK)
        document.getElementById("fw").innerHTML = "Firmware: " + msgobj.FW
        document.getElementById("model").innerHTML = "Model: " + msgobj.MODEL
        document.getElementById("rssi").innerHTML = "RSSI: " + msgobj.RSSI

        // hydro jets available
        document.getElementById("jets").style.display = msgobj.HASJETS ? "table-cell" : "none"
        document.getElementById("jetsswitch").style.display = msgobj.HASJETS ? "table-cell" : "none"
        // godmode available
        document.getElementById("god").style.display = msgobj.HASGOD ? "table-cell" : "none"
        document.getElementById("godswitch").style.display = msgobj.HASGOD ? "table-cell" : "none"
    }

    if (msgobj.CONTENT == "STATES") {
        // temperature
        document.getElementById("atlabel").innerHTML = msgobj.TMP.toString()
        document.getElementById("ttlabel").innerHTML = msgobj.TGT.toString()

        // buttons
        document.getElementById("AIR").checked = msgobj.AIR
        if (document.getElementById("UNT").checked != msgobj.UNT) {
            document.getElementById("UNT").checked = msgobj.UNT
            initControlValues = true
        }
        document.getElementById("FLT").checked = msgobj.FLT
        document.getElementById("HJT").checked = msgobj.HJT
        document.getElementById("GOD").checked = msgobj.GOD
        document.getElementById("HTR").checked = msgobj.RED || msgobj.GRN

        // heater button color
        document.getElementById("htrspan").classList.remove("heateron")
        document.getElementById("htrspan").classList.remove("heateroff")
        if (msgobj.RED || msgobj.GRN) {
            document.getElementById("htrspan").classList.add(msgobj.RED ? "heateron" : msgobj.GRN ? "heateroff" : "n-o-n-e")
        }

        // display
        document.getElementById("display").innerHTML = "" + String.fromCharCode(msgobj.CH1, msgobj.CH2, msgobj.CH3) + ""
        document.getElementById("display").style.color = rgb(255 - dspBrtMultiplier * 8 + dspBrtMultiplier * (parseInt(msgobj.BRT) + 1), 0, 0)

        // set control values (once)
        if (initControlValues) {
            var minTemp = msgobj.UNT ? 20 : 68
            var maxTemp = msgobj.UNT ? 40 : 104
            document.getElementById("temp").min = minTemp
            document.getElementById("temp").max = maxTemp
            document.getElementById("selectorTemp").min = minTemp
            document.getElementById("selectorTemp").max = maxTemp

            document.getElementById("temp").value = msgobj.TGT
            document.getElementById("brt").value = msgobj.BRT

            initControlValues = false
        }

        document.getElementById("sliderTempVal").innerHTML = msgobj.TGT
        document.getElementById("sliderBrtVal").innerHTML = msgobj.BRT

        // get selector elements
        var elemSelectorTemp = document.getElementById("selectorTemp")
        var elemSelectorBrt = document.getElementById("selectorBrt")

        // change values only if element is not active (selected for input)
        // also change only if an update is not in progress
        if (document.activeElement !== elemSelectorTemp && !updateTempState) {
            elemSelectorTemp.value = msgobj.TGT
            elemSelectorTemp.parentElement.querySelector(".numDisplay").textContent = msgobj.TGT
        }
        if (document.activeElement !== elemSelectorBrt && !updateBrtState) elemSelectorBrt.value = msgobj.BRT

        // reset update states when the set target matches the input
        if (elemSelectorTemp.value == msgobj.TGT) updateTempState = false
        if (elemSelectorBrt.value == msgobj.BRT) updateBrtState = false
    }

    if (msgobj.CONTENT == "TIMES") {
        var date = new Date(msgobj.TIME * 1000)
        document.getElementById("time").innerHTML = date.toLocaleString()

        document.getElementById("cost").innerHTML = msgobj.COST.toFixed(2)
    }
}

function sendCommand(cmd) {
    console.log(cmd)
    console.log(typeof cmdMap[cmd])
    // check command
    if (typeof cmdMap[cmd] == "undefined") {
        console.log("invalid command")
        return
    }

    // get the current unit (true=C, false=F)
    var unit = document.getElementById("UNT").checked

    // get and set value
    var value = 0
    if (cmd == "setTarget" || cmd == "setTargetSelector") {
        value = parseInt(document.getElementById(cmd == "setTarget" ? "temp" : "selectorTemp").value)
        value = getProperValue(value, unit ? 20 : 68, unit ? 40 : 104)
        document.getElementById("sliderTempVal").innerHTML = value.toString()
        document.getElementById("selectorTemp").value = value.toString()
        document.getElementById("selectorTemp").setAttribute("value", value.toString())
        updateTempState = true
    } else if (cmd == "setBrightness" || cmd == "setBrightnessSelector") {
        value = parseInt(document.getElementById(cmd == "setBrightness" ? "brt" : "selectorBrt").value)
        value = getProperValue(value, 0, 8)
        document.getElementById("sliderBrtVal").innerHTML = value.toString()
        document.getElementById("selectorBrt").value = value.toString()
        document.getElementById("display").style.color = rgb(255 - dspBrtMultiplier * 8 + dspBrtMultiplier * (value + 1), 0, 0)
        updateBrtState = true
    } else if (btnMap[cmd] && (cmd == "toggleUnit" || cmd == "toggleBubbles" || cmd == "toggleHeater" || cmd == "togglePump" || cmd == "toggleHydroJets" || cmd == "toggleGodmode")) {
        value = document.getElementById(btnMap[cmd]).checked
        initControlValues = true
    }

    var obj = {}
    obj["CMD"] = cmdMap[cmd]
    obj["VALUE"] = value
    obj["XTIME"] = Math.floor(Date.now() / 1000)
    obj["INTERVAL"] = 0
    obj["TXT"] = ""
    var json = JSON.stringify(obj)
    connection.send(json)
    console.log(json)
}

function getProperValue(val, min, max) {
    return val < min ? min : val > max ? max : val
}

function rgb(r, g, b) {
    r = Math.floor(r)
    g = Math.floor(g)
    b = Math.floor(b)
    return ["rgb(", r, ",", g, ",", b, ")"].join("")
}
