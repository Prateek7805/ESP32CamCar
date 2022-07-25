const char _hscript[] PROGMEM = R"=====(
const STREAMURL=`${window.location.origin}:81/stream`,CAPTUREURL=`${window.location.origin}/capture`,CONTROLURL=`${window.location.origin}/control`,docColors=getComputedStyle(document.querySelector(":root"));var settingBtns=[{id:"resetMCU",com:"rm"},{id:"resetState",com:"rs"},{id:"resetCreds",com:"rc"},{id:"factoryReset",com:"ra"}],state={hl:0,tl:0,hb:0,tb:0,speed:0,WIFIMODE:1,quality:0,setting:0};const ranges=["quality","speed","hb","tb"],btns=["hl","tl"],d=t=>document.getElementById(t),dae=(t,e,a)=>{t.addEventListener(e,a)},dc=(t,e)=>document.getElementsByClassName(t)[e||0],dcn=(t,e)=>t.childNodes[e||0],er=t=>{console.log(t)},clearText=t=>{setTimeout(()=>{d(t).textContent=""},2500)},disp=(t,e)=>{d(t).textContent=e,clearText(t)};function animate({timing:t,draw:e,duration:a}){let n=performance.now();requestAnimationFrame(function d(s){let o=(s-n)/a;o>1&&(o=1);let l=t(o);e(l),o<1&&requestAnimationFrame(d)})}const _send=(t,e)=>(er(`${CONTROLURL}?com=${t}&val=${e}`),fetch(`${CONTROLURL}?com=${t}&val=${e}`)),_sendf=(t,e,a)=>(er(`${CONTROLURL}?com=${t}&val=${e}`),fetch(`${CONTROLURL}?com=${t}&val=${e}`).then(t=>t.json()).then(a)),dnone=t=>{t.style.display="none"},dblock=t=>{t.style.display="block"},toggleSettings=()=>{var t=dc("setting"),e=dc("content"),a=dcn(dc("setting-btn")),n=state.setting;state.setting=1-n,animate({duration:1e3,timing:function(t){return val=1/(1+Math.exp(-(30*t-5)))},draw:function(d){d>.96&&((n?e:t).style.display="block",stream(n),dnone(n?t:e),updateHeights()),a.style.transform=`rotate(${90*(n?-d:d)}deg)`}})},startGamepad=()=>{["_f","_r","_b","_l"].forEach((t,e)=>{const a=d(t);a.innerHTML=`<svg style="transform:rotate(${90*e}deg)"\n        viewBox="0 0 24 24" \n        fill="none" \n        stroke-linecap="round" \n        stroke-linejoin="round">\n        <polyline points="18 15 12 9 6 15"></polyline>\n        </svg>`,dae(a,"touchstart",()=>{_send(a.id,1)}),dae(a,"touchend",()=>{_send(a.id,0)}),dae(a,"mousedown",()=>{_send(a.id,1)}),dae(a,"mouseup",()=>{_send(a.id,0)})})},lightBtnUpdate=t=>{dcn(d(t)).style.fill=state[t]?docColors.getPropertyValue("--hL-on"):docColors.getPropertyValue("--hL-off"),dcn(d(t)).style.filter=state[t]?`drop-shadow(0 0 20px ${docColors.getPropertyValue("--hL-on")})`:"none"},clickLight=t=>{state[t.id]=1-state[t.id],_send(t.id,state[t.id]),lightBtnUpdate(t.id)},startLights=()=>{const t=document.getElementsByClassName("l-btn");Array.from(t).forEach(t=>{t.innerHTML='<svg viewBox="0 0 100,100" fill="none">\n        <path d="M40,15 c-50,0 -50,70 0,70 c10,0 10,0 10,-5 l0,-60 c0,-5 0,-5 -10,-5"></path>\n        <path d="M60,15 l30,0 l0,6 l-30,0 l0,-8"></path>\n        <path d="M60,30.5 l30,0 l0,6 l-30,0 l0,-8"></path>\n        <path d="M60,46 l30,0 l0,6 l-30,0 l0,-8"></path>\n        <path d="M60,61.5 l30,0 l0,6 l-30,0 l0,-8"></path>\n        <path d="M60,77 l30,0 l0,6 l-30,0 l0,-8"></path>\n    </svg>',dae(t,"click",()=>{clickLight(t)})})},updateAllInputs=()=>{ranges.forEach(t=>{d(t).value=state[t],d(`t-${t}`).textContent=state[t]}),btns.forEach(t=>{lightBtnUpdate(t)}),d("WIFIMODE").checked=state.WIFIMODE},startRangeListeners=()=>{ranges.forEach(t=>{const e=d(t);dae(e,"input",()=>{d(`t-${t}`).textContent=e.value}),dae(e,"change",()=>{state[t]=parseInt(e.value),fetch(`${CONTROLURL}?com=${t}&val=${e.value}`),er(state)})})},toNum=t=>parseFloat(t.substring(0,t.lastIndexOf("px"))),updateHeights=()=>{cameraHolder=dc("camera-holder"),camera=d("camera");var t=getComputedStyle(dc("gamepad")),e=getComputedStyle(dc("camera-holder")),a=toNum(t.height),n=toNum(e.width);cameraHolder.style.height=a+"px",camera.style.width=a+"px",camera.style.height=n+"px";var s=n-a;camera.style.top=-s/2+"px",camera.style.left=s/2+"px"},validateCred=(t,e)=>!!t.match(/^[^!#;+\]\/"\t][^+\]"\t]{0,31}$/)&&!(""==e||!e.match(/^[ -~]{8,63}$/)),sendCreds=(t,e,a)=>{var n=t.value,d=e.value;validateCred(n,d)?(_send(`${a}${n}${d}`,n.length),t.value="",e.value=""):(t.value="",e.value="",er("Enter Correct Creds"))},sync=()=>{fetch(`${CONTROLURL}?com=status&val=1`).then(t=>t.json()).then(t=>{state={...state,...t},ranges.forEach(t=>{d(t).value=state[t],d(`t-${t}`).textContent=state[t]}),btns.forEach(t=>{lightBtnUpdate(t)}),d("WIFIMODE").checked=state.WIFIMODE})},stream=t=>{const e=d("camera");e.src=t?STREAMURL:CAPTUREURL};dae(document,"DOMContentLoaded",()=>{stream(!0),d("capture").href=CAPTUREURL,updateHeights(),["_f","_r","_b","_l"].forEach((t,e)=>{const a=d(t);a.innerHTML=`<svg style="transform:rotate(${90*e}deg)"\n        viewBox="0 0 24 24" \n        fill="none" \n        stroke-linecap="round" \n        stroke-linejoin="round">\n        <polyline points="18 15 12 9 6 15"></polyline>\n        </svg>`,dae(a,"touchstart",()=>{_send(a.id,1)}),dae(a,"touchend",()=>{_send(a.id,0)}),dae(a,"mousedown",()=>{_send(a.id,1)}),dae(a,"mouseup",()=>{_send(a.id,0)})}),ranges.forEach(t=>{const e=d(t);dae(e,"input",()=>{d(`t-${t}`).textContent=e.value}),dae(e,"change",()=>{state[t]=parseInt(e.value),fetch(`${CONTROLURL}?com=${t}&val=${e.value}`),er(state)})}),startLights(),dae(d("stop"),"click",()=>{_send("_s",0)}),dae(dc("setting-btn"),"click",toggleSettings),settingBtns.forEach(t=>{dae(d(t.id),"click",()=>{_send(t.com,1)})}),dae(d("STA_SUBMIT"),"click",()=>{sendCreds(d("STA_SSID"),d("STA_PASS"),"WS")}),dae(d("AP_SUBMIT"),"click",()=>{sendCreds(d("AP_SSID"),d("AP_PASS"),"WA")}),sync(),dae(d("RSSI"),"click",()=>{_sendf("rssi",1,t=>{disp("t-RSSI",t.rssi)})}),dae(d("status"),"click",()=>{_sendf("status",1,t=>{for(item in t)if(state[item]!=t[item])return disp("t-status","Error, trying to auto fix"),void sync();disp("t-status","In sync")})}),dae(d("WIFIMODE"),"change",t=>{_send("WM",t.target.checked?1:0)})}),dae(window,"resize",updateHeights);
)=====";