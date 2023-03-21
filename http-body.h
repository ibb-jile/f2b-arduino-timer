const char pageBody[] PROGMEM = R"=====(<html><title>F2B timer</title>
<script>var refreshInterval = null;
var startBtn, stopBtn, timerElement, throttleElement, speedSpan, gElement, optimalGElement, thresholdElement;
var delayElement, autostartElement, allowSpeedOptimizerElement, speedChangeStepElement, minSpeedElement;
var maxSpeedElement;
var loadPresetValues = true;

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
    if (refreshInterval === null) {
        refreshInterval = setInterval(refresh, 1000);
    }
}

function getData(callback) {
    var httpRequest = new XMLHttpRequest();
    httpRequest.open('GET', '/get', true);
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
    httpRequest.open(method, path, true);
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

function createTimerString(state) {
    var ellapsedMs = (state.finishAt - state.time);
    var timestamp = new Date(ellapsedMs);
    var minutes = timestamp.getMinutes();
    var seconds = timestamp.getSeconds();
    return minutes + ':' + seconds;
}

function setState(state) {
    if (state.finishAt > 0) {
        showStop();
        stopBtn.innerText = 'STOP (' + createTimerString(state) + ')';
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

function refresh() {
    getData((json) => {
        setPresetValues(json.preset);
        setState(json.current);
        speedSpan.innerText = json.current.speed.toString() + ' %';
        gElement.innerText = 'G: ' + json.current.g.toString();
    });
}

function startTimer() {
    showStop();
    sendStartCommand(() => {
        refreshInterval = setInterval(refresh, 1000);
    });
    console.log('start');
}

function stopTimer() {
    showStart();
    sendStopCommand(() => {
        refresh();
    });
    console.log('stop');
}

function savePresets() {

    savePresetsCommand(() => {
        loadPresetValues = true;
        refresh();
    });
}

refresh();</script>
<body onLoad='init()'><h1>Stunt timer</h1>
<h2>Preset</h2>
<form>
    <input id='timerPreset' max='60000' min='1' type='number'/><label htmlFor='timerPreset'>Timer (s)</label><br><br>
    <input id='throttlePreset' max='100' min='0' type='number'/><label htmlFor='throttlePreset'>Throttle (%)</label><br><br>
    <input id='delayPreset' max='100' min='0' type='number'/><label htmlFor='delayPreset'> Delay before run
    (s)</label><br><br>
    <input type='checkbox' id='autostartPreset' name='autostart' value=0><label htmlFor='autostartPreset'>Auto
    start</label><br><br>
    <input type='checkbox' id='allowSpeedOptimizer' name='allowSpeedOptimizer' value=0><label
        htmlFor='allowSpeedOptimizer'>Enable
    optimization</label><br><br>
    <input id='speedChangeStep' max='100' min='1' type='number'/><label htmlFor='speedChangeStep'>Opt.
    step</label><br><br>
    <input id='minSpeed' max='100' min='0' type='number'/><label htmlFor='minSpeed'>Min throttle</label><br><br>
    <input id='maxSpeed' max='100' min='0' type='number'/><label htmlFor='maxSpeed'>Max throttle</label><br><br>
    <input id='optimalG' max='400' min='0' type='number'/><label htmlFor='optimalG'>Optimal G (x100)</label><br><br>
    <input id='threshold' max='200' min='0' type='number'/><label htmlFor='threshold'>G threshold (x100)</label><br><br>
    <button id='savePresetBtn' onClick='savePresets()'>Save changes</button>
    <br><br>
</form>
<hr/>
<p>
    <button hidden='true' id='startBtn' onClick='startTimer()'>START</button>
    <button hidden='true' id='stopBtn' onClick='stopTimer()'>STOP ()</button>
    <span id='speedSpan'>0 %</span><br><span id='gElement'>G: 0</span></p>
<p>
    <button id='speedUpBtn' onClick='speedUp()'>Speed UP</button>
    <button id='speedDownBtn' onClick='speedDown()'>Speed DOWN</button>
</p>
</body>
</html>
)=====";
