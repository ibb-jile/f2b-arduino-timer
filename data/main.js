let startBtn, stopBtn, timerElement, throttleElement, speedSpan, gElement, optimalGElement, thresholdElement;
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
        httpRequest.open(method, "http://192.168.0.206" + path, true);
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
