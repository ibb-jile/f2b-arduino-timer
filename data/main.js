var startBtn, stopBtn, timerElement, throttleElement, speedSpan, gElement, optimalGElement, thresholdElement;
var refreshInterval = null;
var delayElement, autostartElement, allowSpeedOptimizerElement, speedChangeStepElement, minSpeedElement;
var maxSpeedElement;
var currentTimeElement, timeProgressElement, throttleProgressElement;
var loadPresetValues = true;
var page = 'timerPage';
var url = "http://192.168.0.206";

function setPage(newPage) {
    if (page === newPage) {
        return;
    }
    page = newPage;
    var timerPage = document.getElementById('timerPage');
    var setupPage = document.getElementById('setupPage');
    var timerTab = document.getElementById('timerTab');
    var setupTab = document.getElementById('setupTab');
    if (page === 'timerPage') {
        timerPage.style.display = 'block';
        timerTab.className = 'tabButton selectedTab';
        setupPage.style.display = 'none';
        setupTab.className = 'tabButton';
    } else {
        timerPage.style.display = 'none';
        setupPage.style.display = 'block';
        timerTab.className = 'tabButton';
        setupTab.className = 'tabButton selectedTab';
    }
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

function getData(callback) {
    var httpRequest = new XMLHttpRequest();
    httpRequest.open('GET', url + '/get', true);
    httpRequest.send();
    httpRequest.onreadystatechange = () => {
        if (httpRequest.readyState === 4 && httpRequest.status === 200) {
            var json = JSON.parse(httpRequest.response);
            callback(json);
        }
    }
}

function savePresetsCommand(callback) {
    var json = {
        preset: {
            timeSec: timerElement.value,
            speed: throttleElement.value,
            delaySec: delayElement.value,
            autostart: (autostartElement.checked) ? 1 : 0,
            allowSpeedOptimizer: allowSpeedOptimizerElement.checked ? 1 : 0,
            minSpeed: minSpeedElement.value,
            maxSpeed: maxSpeedElement.value,
            speedChangeStep: speedChangeStepElement.value,
            optimalG: optimalGElement.value,
            threshold: thresholdElement.value
        }
    };
    sendCommand('PUT', '/set', JSON.stringify(json), callback);
}

function sendStartCommand(callback) {
    sendCommand('GET', '/timer/start', undefined, callback);
}

function sendStopCommand(callback) {
    sendCommand('GET', '/timer/stop', undefined, callback);
}

function speedUp() {
    sendCommand('GET', '/speed/up', undefined, () => {
    });
}

function speedDown() {
    sendCommand('GET', '/speed/down', undefined, () => {
    });
}

function sendCommand(method, path, data, callback) {
    var httpRequest = new XMLHttpRequest();
    httpRequest.open(method, url + path, true);
    httpRequest.setRequestHeader('Content-Type', 'application/json');
    httpRequest.send(data);
    httpRequest.onreadystatechange = () => {
        if (httpRequest.readyState === 4 && httpRequest.status === 200) {
            callback();
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

function getCurrentTime(json) {
    var leftTime = (json.current.finishAt - json.current.time);
    var timestamp;
    if (json.current.state === 1) {
        timestamp = new Date(leftTime - (json.preset.timeSec * 1000));

    } else {
        timestamp = new Date(leftTime);
    }
    return timestamp;
}

function setTimeProgress(timestamp, json) {
    var timePercent;
    if (json.current.state === 1) {
        timePercent = (timestamp / (json.preset.delaySec * 1000)) * 100;
    } else {
        timePercent = (timestamp / (json.preset.timeSec * 1000)) * 100;
    }
    timeProgressElement.value = timePercent;
}

function createTimerString(timestamp, state) {
    var minutes = timestamp.getMinutes();
    var seconds = timestamp.getSeconds();
    var result = (state === 1) ? '- ' : '';
    if (minutes < 10) {
        result += '0';
    }
    result += minutes + ':';
    if (seconds < 10) {
        result += '0';
    }
    result += seconds;
    return result;
}

function setState(json) {
    if (json.current.finishAt > 0) {
        showStop();
        var timestamp = getCurrentTime(json);
        setTimeProgress(timestamp, json);
        currentTimeElement.innerText = createTimerString(timestamp, json.current.state);
    } else {
        showStart();
    }
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
    setPresetValues(json.preset);
    setState(json);
    throttleProgressElement.value = json.current.speed;
    speedSpan.innerText = 'Throttle ' + json.current.speed.toString() + ' %';
    gElement.innerText = 'G: ' + json.current.g?.toString();
}

function refresh() {
    getData((json) => {
        showInfo(json);
        listenUdpPort();
    });
}

function startTimer() {
    showStop();
    sendStartCommand(() => {
    });
    console.log('start');
}

function stopTimer() {
    showStart();
    sendStopCommand(() => {
    });
    console.log('stop');
}

function savePresets() {

    savePresetsCommand(() => {
        loadPresetValues = true;
        refresh();
    });
}
