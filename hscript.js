
const STREAMURL = `${window.location.origin}:81/stream`;
//const STREAMURL = `maxresdefault.jpg`
const CAPTUREURL = `${window.location.origin}/capture`;
const CONTROLURL = `${window.location.origin}/control`;
const docColors = getComputedStyle(document.querySelector(':root'));
var settingBtns = [{
    id: "resetMCU",
    com: "rm"
},
{
    id: "resetState",
    com: "rs"
},
{
    id: "resetCreds",
    com: "rc"
},
{
    id: "factoryReset",
    com: "ra"
}];

var state = {
    hl: 0,
    tl: 0,
    hb: 0,
    tb: 0,
    speed: 0,
    WIFIMODE: 1,
    quality: 0,
    setting: 0
}

const ranges = ['quality', 'speed', 'hb', 'tb'];
const btns = ['hl', 'tl'];
const d = (i) => {
    return document.getElementById(i);
}
const dae = (o, e, f) => {
    o.addEventListener(e, f);
}
const dc = (c, i) => {
    return document.getElementsByClassName(c)[i || 0];
}
const dcn = (n, i) => {
    return n.childNodes[i || 0];
}
const er = (m) => {
    console.log(m);
}
const clearText = (i) => {
    setTimeout(() => {
        d(i).textContent = "";
    }, 2500);
}
const disp = (i, m) => {
    d(i).textContent = m;
    clearText(i);
}
function animate({ timing, draw, duration }) {
    let start = performance.now();
    requestAnimationFrame(function animate(time) {
        let timeFraction = (time - start) / duration;
        if (timeFraction > 1) timeFraction = 1;
        let progress = timing(timeFraction)
        draw(progress);
        if (timeFraction < 1) {
            requestAnimationFrame(animate);
        }
    });
}

const _send = (com , val) => {
    er(`${CONTROLURL}?com=${com}&val=${val}`);
    return fetch(`${CONTROLURL}?com=${com}&val=${val}`);
} 
const _sendf = (com, val, f) => {
    er(`${CONTROLURL}?com=${com}&val=${val}`);
    return fetch(`${CONTROLURL}?com=${com}&val=${val}`).then(resp=>resp.json()).then(f);
}

const dnone = (o) => {
    o.style.display = 'none';
}
const dblock = (o) => {
    o.style.display = 'block';
}

const toggleSettings = () => {
    var settings = dc('setting');
    var content = dc('content');
    var icon = dcn(dc('setting-btn'));
    var contentShow = state.setting;

    state.setting = 1 - contentShow;
    animate({
        duration: 1000,
        timing: function (timeFraction) {
            return val = (1 / (1 + Math.exp(-(30 * timeFraction - 5))));
        },
        draw: function (progress) {
            if (progress > 0.96) {
                dblock(contentShow ? content : settings);
                stream(contentShow);
                dnone(!contentShow ? content : settings);
                updateHeights();
            }

            icon.style.transform = `rotate(${(!contentShow ? progress : -progress) * 90}deg)`;
        }
    });
}

const startGamepad = () => {
    const cont = ['_f', '_r', '_b', '_l'];
    cont.forEach((item, index) => {
        const gbtn = d(item);
        gbtn.innerHTML = `<svg style="transform:rotate(${90 * index}deg)"
        viewBox="0 0 24 24" 
        fill="none" 
        stroke-linecap="round" 
        stroke-linejoin="round">
        <polyline points="18 15 12 9 6 15"></polyline>
        </svg>`;
        dae(gbtn, 'touchstart', () => { _send(gbtn.id, 1) });
        dae(gbtn, 'touchend', () => { _send(gbtn.id, 0) });
        dae(gbtn, 'mousedown', () => { _send(gbtn.id, 1) });
        dae(gbtn, 'mouseup', () => { _send(gbtn.id, 0) });
    });
}

const lightBtnUpdate = (id) => {
    dcn(d(id)).style.fill = state[id] ? docColors.getPropertyValue('--hL-on') : docColors.getPropertyValue('--hL-off');
    dcn(d(id)).style.filter = state[id] ? `drop-shadow(0 0 20px ${docColors.getPropertyValue('--hL-on')})` : 'none';
}
const clickLight = (light) => {
    state[light.id] = 1 - state[light.id];
    _send(light.id, state[light.id]);
    lightBtnUpdate(light.id);
}
const startLights = () => {
    const btns = document.getElementsByClassName('l-btn');
    Array.from(btns).forEach((item) => {
        item.innerHTML = `<svg viewBox="0 0 100,100" fill="none">
        <path d="M40,15 c-50,0 -50,70 0,70 c10,0 10,0 10,-5 l0,-60 c0,-5 0,-5 -10,-5"></path>
        <path d="M60,15 l30,0 l0,6 l-30,0 l0,-8"></path>
        <path d="M60,30.5 l30,0 l0,6 l-30,0 l0,-8"></path>
        <path d="M60,46 l30,0 l0,6 l-30,0 l0,-8"></path>
        <path d="M60,61.5 l30,0 l0,6 l-30,0 l0,-8"></path>
        <path d="M60,77 l30,0 l0,6 l-30,0 l0,-8"></path>
    </svg>`;
        dae(item, 'click', () => { clickLight(item) });
    });
}
const updateAllInputs = () => {
    ranges.forEach(item => {
        d(item).value = state[item];
        d(`t-${item}`).textContent = state[item];
    });
    btns.forEach(item => {
        lightBtnUpdate(item);
    });
    d('WIFIMODE').checked = state.WIFIMODE;
}
const startRangeListeners = () => {

    ranges.forEach((item) => {
        const range = d(item);
        dae(range, 'input', () => {
            d(`t-${item}`).textContent = range.value;
        });
        dae(range, 'change', () => {
            state[item] = parseInt(range.value);
            fetch(`${CONTROLURL}?com=${item}&val=${range.value}`);
            er(state);
        })
    })
}

const toNum = (st) => {
    return parseFloat(st.substring(0, st.lastIndexOf('px')))
}

const updateHeights = () => {

    cameraHolder = dc('camera-holder');
    camera = d('camera');
    var gamepadCss = getComputedStyle(dc('gamepad'));
    var cameraCss = getComputedStyle(dc('camera-holder'));

    var gamepadHeight = toNum(gamepadCss.height);
    var cameraWidth = toNum(cameraCss.width);
    cameraHolder.style.height = gamepadHeight + 'px';
    camera.style.width = gamepadHeight + 'px';
    camera.style.height = cameraWidth + 'px';
    var diff = cameraWidth - gamepadHeight;
    camera.style.top = -(diff / 2) + 'px';
    camera.style.left = (diff / 2) + 'px';
}

const validateCred = (SSID, PASS) => {
    if (!SSID.match(/^[^!#;+\]\/"\t][^+\]"\t]{0,31}$/)) {
        return false;
    } if ((PASS == "") || !PASS.match(/^[ -~]{8,63}$/)) {
        return false;
    }
    return true;
}
const sendCreds = (ssid, pass, type) => {
    var SSID = ssid.value;
    var PASS = pass.value;
    if (validateCred(SSID, PASS)) {
        _send(`${type}${SSID}${PASS}`, SSID.length);
        ssid.value = "";
        pass.value = "";
    }
    else {
        ssid.value = "";
        pass.value = "";
        er("Enter Correct Creds");
    }
}

const sync = () => {
    const com = "status";
    const val = 1;
    fetch(`${CONTROLURL}?com=${com}&val=${val}`).then(resp => resp.json()).then(data => {
        state = {
            ...state,
            ...data
        };
        updateAllInputs();
    });
}
const stream = (flag) => {
    const cam = d('camera');
    if (flag) {
        cam.src = STREAMURL;
    } else {
        cam.src = CAPTUREURL;
    }
}
dae(document, 'DOMContentLoaded', () => {
    stream(true);
    d('capture').href = CAPTUREURL;
    updateHeights();
    startGamepad();
    startRangeListeners();
    startLights();
    dae(d('stop'), 'click', () => { _send('_s', 0) });
    dae(dc('setting-btn'), 'click', toggleSettings);

    settingBtns.forEach((item) => {
        dae(d(item.id), 'click', () => {
            _send(item.com, 1);
        }
        );
    });

    dae(d('STA_SUBMIT'), 'click', () => {
        sendCreds(d('STA_SSID'), d('STA_PASS'), 'WS');
    });
    dae(d('AP_SUBMIT'), 'click', () => {
        sendCreds(d('AP_SSID'), d('AP_PASS'), 'WA');
    });

    
    sync();
    dae(d('RSSI'), 'click', () => {
        _sendf('rssi', 1, (data) => {
            disp("t-RSSI", data.rssi);
        });
    });
    dae(d('status'), 'click', () => {
        _sendf('status', 1, (data) => {
            for (item in data) {
                if (state[item] != data[item]) {
                    disp("t-status", 'Error, trying to auto fix');
                    sync();
                    return;
                }
            }
            disp("t-status", "In sync");
        });
    });

    dae(d('WIFIMODE'), 'change', (e)=>{
        _send("WM", e.target.checked? 1 : 0);
    });
});

dae(window, 'resize', updateHeights);