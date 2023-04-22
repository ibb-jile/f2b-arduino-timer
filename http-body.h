const char pageBody[] PROGMEM = R"=====(
<html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0">
    <title>F2B timer</title>
    <style>
        button, input, body, input[type=checkbox] {
            font-size: 14px;
            font-family: 'Roboto', 'Helvetica', sans-serif;
            line-height: 20px;
            margin-right: 10px;
        }

        input {
            width: 50px;
        }

        .inputStyle {
            margin-bottom: 10px;
        }

        .section {
            border-bottom-style: solid;
            border-bottom-color: darkgrey;
            border-bottom-width: 1px;
            margin-bottom: 10px;
            padding-bottom: 10px;
        }

        .tabButton {
            color: white;
            padding: 15px 32px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
        }

        .selectedTab {
            border-bottom-width: 2px;
            border-bottom-style: solid;
            border-bottom-color: white;
        }

        header {
            background-color: rgb(63, 81, 181);
            color: white;
            padding-left: 8px;
            padding-top: 8px;
            padding-bottom: 2px;
        }
        .fabButton {
            position: fixed;
            bottom: 16px;
            right: 16px;
            width: 80px;
            height: 80px;
            border-radius: 50%;
            border: none;
            background-color: rgb(233, 30, 99);
            color: white;
            padding: 16px;
            text-align: center;
            text-decoration: none;
            cursor: pointer;
            box-shadow: 0 1px 1.5px 0 rgba(0,0,0,.12), 0 1px 1px 0 rgba(0,0,0,.24);
        }
    </style>
</head>
<script>let startBtn, stopBtn, timerElement, throttleElement, speedSpan, gElement, optimalGElement, thresholdElement;
let refreshInterval = null;
let delayElement, autostartElement, allowSpeedOptimizerElement, speedChangeStepElement, minSpeedElement;
let maxSpeedElement;
let currentTimeElement, timeProgressElement, throttleProgressElement;
let loadPresetValues = true;
let httpRequest;
let log = [];
let page = 'timerPage';

function setPage(newPage) {
    if (page === newPage) return;

    const timerPage = document.getElementById('timerPage');
    const setupPage = document.getElementById('setupPage');
    const timerTab = document.getElementById('timerTab');
    const setupTab = document.getElementById('setupTab');

    timerPage.style.display = newPage === 'timerPage' ? 'block' : 'none';
    setupPage.style.display = newPage === 'setupPage' ? 'block' : 'none';

    timerTab.className = newPage === 'timerPage' ? 'tabButton selectedTab' : 'tabButton';
    setupTab.className = newPage === 'setupPage' ? 'tabButton selectedTab' : 'tabButton';

    page = newPage;
}

function init() {
    startBtn = document.getElementById('startBtn');
    stopBtn = document.getElementById('stopBtn');
    timerElement = document.getElementById('timerPreset');
    throttleElement = document.getElementById('throttlePreset');
    delayElement = document.getElementById('delayPreset');
    speedSpan = document.getElementById('speedSpan');
    gElement = document.getElementsByTagName('gElement');
    autostartElement = document.getElementById('autostartPreset');
    allowSpeedOptimizerElement = document.getElementById('allowSpeedOptimizer');
    speedChangeStepElement = document.getElementById('speedChangeStep');
    minSpeedElement = document.getElementById('minSpeed');
    maxSpeedElement = document.getElementById('maxSpeed');
    optimalGElement = document.getElementById('optimalG');
    thresholdElement = document.getElementById('threshold');
    currentTimeElement = document.getElementById('currentTime');
    timeProgressElement = document.getElementById('timeProgress');
    throttleProgressElement = document.getElementById('throttleProgress');
    if (refreshInterval === null) {
        refreshInterval = setInterval(refresh, 250);
    }
    refresh();
    setPage('timerPage');

}

function download(filename, text) {
    const element = document.createElement('a');
    element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));

    element.setAttribute('download', filename);
    element.style.display = 'none';

    document.body.appendChild(element);

    element.click();
    document.body.removeChild(element);

}

function onDownload() {
    const text = JSON.stringify({log});
    const filename = "log.json";
    download(filename, text);
}

function getData(callback) {
    sendCommand('GET', '/get', undefined, (response) => {
        const json = JSON.parse(response);
        log.push(json);
        callback(json);
    });
}

function sendStartCommand(callback) {
    sendCommand('GET', '/timer/start', undefined, callback, true);

}

function sendStopCommand(callback) {
    sendCommand('GET', '/timer/stop', undefined, callback, true);

}

function sendCommand(method, path, data, callback, urgent = false) {
    if (!httpRequest || urgent) {
        if (httpRequest) {
            httpRequest.abort();
        }
        httpRequest = new XMLHttpRequest();
        httpRequest.open(method, "http://192.168.42.1" + path, true);
        httpRequest.setRequestHeader('Content-Type', 'application/json');
        httpRequest.send(data);
        httpRequest.onreadystatechange = () => {
            if (httpRequest.readyState === 4 && httpRequest.status === 200) {
                if (callback) {
                    callback(httpRequest.response);
                }
                httpRequest = null;
            }
        }
    }
}

function setPresetValues(preset) {
    if (loadPresetValues) {
        timerElement.value = preset.timeSec;
        throttleElement.value = preset.speed;
        delayElement.value = preset.delaySec;
        autostartElement.checked = (preset.autostart === 1);
        allowSpeedOptimizerElement.checked = (preset.allowSpeedOptimizer === 1);
        minSpeedElement.value = preset.minSpeed;
        maxSpeedElement.value = preset.maxSpeed;
        speedChangeStepElement.value = preset.speedChangeStep;
        optimalGElement.value = preset.optimalG;
        thresholdElement.value = preset.threshold;
        loadPresetValues = false;
    }
}

function getPresetValues() {
    return {
        timeSec: timerElement.value,
        speed: throttleElement.value,
        delaySec: delayElement.value,
        autostart: autostartElement.checked ? 1 : 0,
        allowSpeedOptimizer: allowSpeedOptimizerElement.checked ? 1 : 0,
        minSpeed: minSpeedElement.value,
        maxSpeed: maxSpeedElement.value,
        speedChangeStep: speedChangeStepElement.value,
        optimalG: optimalGElement.value,
        threshold: thresholdElement.value,
    };
}

function setCurrentTime(json) {
    const leftTime = (json.current.finishAt - json.current.time);
    const state = json.current.state;
    const timerTimeMs = (json.preset.timeSec * 1000);
    const timestamp = state === 1 ? new Date(leftTime - timerTimeMs) : new Date(leftTime);
    setTimeProgress(timestamp, json);
    currentTimeElement.innerText = createTimerString(timestamp, state);
}

function setTimeProgress(timestamp, json) {
    const state = json.current.state;
    const delayMs = json.preset.delaySec * 1000;
    const timerTimeMs = json.preset.timeSec * 1000;
    const time = (state === 1) ? delayMs : timerTimeMs;
    timeProgressElement.value = (timestamp / time) * 100;
}

function createTimerString(timestamp, state) {
    const minutes = timestamp.getMinutes().toString().padStart(2, '0');
    const seconds = timestamp.getSeconds().toString().padStart(2, '0');
    const prefix = (state === 1) ? '- ' : '';

    return `${prefix}${minutes}:${seconds}`;
}

function showStart() {
    startBtn.hidden = false;
    stopBtn.hidden = true;
}

function showStop() {
    startBtn.hidden = true;
    stopBtn.hidden = false;
}

function showInfo(json) {
    const state = json.current.state;
    setPresetValues(json.preset);
    if (state > 0) {
        showStop();
        setCurrentTime(json);
        throttleProgressElement.value = json.current.speed;
        speedSpan.innerText = 'Throttle ' + json.current.speed.toString() + ' %';
        gElement.innerText = 'G: ' + json.current.g?.toString();
    } else {
        showStart();
        currentTimeElement.innerText = "00:00";
        throttleProgressElement.value = 0;
    }
}

function refresh() {
    getData(showInfo);
}

function startTimer() {
    showStop();
    log = [];
    sendStartCommand();
}

function stopTimer() {
    showStart();
    sendStopCommand();
}

function savePresets() {
    const json = {
        preset: getPresetValues()
    };
    sendCommand('PUT', '/set', JSON.stringify(json), () => {
        loadPresetValues = true;
        refresh();
    }, true);
}
</script>
<body onLoad='init()' style="margin: 0px;">
<header>
    <h1>F2B timer</h1>
    <div>
        <a id="timerTab" class="tabButton selectedTab" onclick="setPage('timerPage')">TIMER</a>
        <a id="setupTab" class="tabButton" onclick="setPage('setupPage')">SETUP</a>
    </div>
</header>
<div id="timerPage">
    <div style="width: 100%; height: 100px; text-align: center; margin-top: 30px;">
        <div id="currentTime" style="font-size: 64px; line-height: initial;">00:00</div>
    </div>
    <div style="width: 100%; height: 50px; text-align: center">
        <progress id='timeProgress' max='100' value="0" style="width: 90%"></progress>
    </div>
    <div style="width: 100%; height: 50px; text-align: center">
        <h3 id='speedSpan'>Throttle %</h3>
        <progress id='throttleProgress' max='100' value='50' style="width: 90%;"></progress>
        <div id='gElement'>G: 0</div>
    </div>
    <button style="position: absolute; bottom: 0px; left: 0px;" id="logFile" onclick="onDownload()">Download log file</button>
    <button hidden='true' class="fabButton" style="background-color: rgb(139, 195, 74);" id='startBtn' onClick='startTimer()'>
        <svg xmlns="http://www.w3.org/2000/svg" height="48" viewBox="0 96 960 960" fill="white" width="48"><path d="M320 853V293l440 280-440 280Zm60-280Zm0 171 269-171-269-171v342Z"/></svg>
    </button>
    <button hidden='true' class="fabButton" id='stopBtn' onClick='stopTimer()'>
        <svg xmlns="http://www.w3.org/2000/svg" height="48" viewBox="0 96 960 960" fill="white" width="48"><path d="M300 396v360-360Zm-60 420V336h480v480H240Zm60-60h360V396H300v360Z"/></svg>
    </button>
</div>
<div id="setupPage" style="margin: 12px; display: none;">
    <form>
        <div class="section">
            <h2>Main setup</h2>
            <div class="inputStyle">
                <input type='checkbox' id='autostartPreset' name='autostart' value=0><label for='autostartPreset'>Auto
                start</label>
            </div>
            <div class="inputStyle">
                <input id='timerPreset' max='60000' min='1' type='number'/><label for='timerPreset'>Timer
                (s)</label>
            </div>
            <div class="inputStyle">
                <input id='throttlePreset' max='100' min='0' type='number'/><label for='throttlePreset'>Throttle
                (%)</label>
            </div>
            <div class="inputStyle">
                <input id='delayPreset' max='100' min='0' type='number'/><label for='delayPreset'> Delay before run
                (s)</label></div>
        </div>
        <div class="section">
            <h2>Power optimizer</h2>
            <input type='checkbox' id='allowSpeedOptimizer' name='allowSpeedOptimizer' value=0><label
                for='allowSpeedOptimizer'>Enable
            optimization</label><br><br>
            <input id='speedChangeStep' max='100' min='1' type='number'/><label for='speedChangeStep'>Opt.
            step</label><br><br>
            <input id='minSpeed' max='100' min='0' type='number'/><label for='minSpeed'>Min throttle</label><br><br>
            <input id='maxSpeed' max='100' min='0' type='number'/><label for='maxSpeed'>Max throttle</label><br><br>
            <input id='optimalG' max='400' min='0' type='number'/><label for='optimalG'>Optimal G (x100)</label><br><br>
            <input id='threshold' max='200' min='0' type='number'/><label for='threshold'>G threshold (x100)</label><br><br>
        </div>
        <button id='savePresetBtn' class="fabButton" onClick='savePresets()'><svg xmlns="http://www.w3.org/2000/svg" fill="white" height="48" viewBox="0 96 960 960" width="48"><path d="M840 373v503q0 24-18 42t-42 18H180q-24 0-42-18t-18-42V276q0-24 18-42t42-18h503l157 157Zm-60 27L656 276H180v600h600V400ZM479.765 811Q523 811 553.5 780.735q30.5-30.264 30.5-73.5Q584 664 553.735 633.5q-30.264-30.5-73.5-30.5Q437 603 406.5 633.265q-30.5 30.264-30.5 73.5Q376 750 406.265 780.5q30.264 30.5 73.5 30.5ZM233 472h358V329H233v143Zm-53-72v476-600 124Z"/></svg></button>
    </form>
</div>
</body>
</html>

)=====";
