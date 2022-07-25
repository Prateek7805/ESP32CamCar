const char _index[] PROGMEM = R"=====(
<!doctype html>
<html lang=en>
<head>
<meta charset=UTF-8>
<meta http-equiv=X-UA-Compatible content="IE=edge">
<meta name=viewport content="width=device-width,initial-scale=1">
<title>AppendCar</title>
<link rel=stylesheet href=hstyles.css>
</head>
<body>
<div class=main>
<div class="header dfc">
<div class=c3></div>
<div class="c3 dfc">
<p class="title t-center">Append Car</p>
</div>
<div class="c3 dffe"><span class="setting-btn dfc-c"><svg viewBox="0 0 24 24" stroke-linecap=round stroke-linejoin=round><circle cx=12 cy=12 r=3></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg></span></div>
</div>
<div class=content>
<div class=row>
<div class="c2 partition p-left">
<div class=dfc>
<div class=camera-holder>
<img src="" id=camera alt="">
</div>
</div>
<div class="dfec s-dnone my">
<input id=quality class=slider type=range min=0 max=10 value=0>
<p class=text id=t-quality>0</p>
</div>
</div>
<div class="c2 partition p-right">
<div class="dfc-c gamepad">
<div class=dfc>
<button class=g-btn id=_f></button>
</div>
<div class=dfc>
<button class=g-btn id=_l></button>
<button class=g-btn id=stop><svg viewBox="0 0 24 24" fill=none stroke-linecap=round stroke-linejoin=round>
<circle cx=12 cy=12 r=10></circle>
</svg></button>
<button class=g-btn id=_r></button>
</div>
<div class=dfc>
<button class=g-btn id=_b></button>
</div>
</div>
<div class=dfec>
<input id=speed class=slider type=range min=0 max=255 value=0>
<p class=text id=t-speed>0</p>
</div>
<div class=dfe>
<div class=c2>
<div class=dfc>
<span class=l-btn id=hl></span>
</div>
<div class=dfec>
<input id=hb class=slider type=range min=0 max=255 value=0>
<p class=text id=t-hb>0</p>
</div>
</div>
<div class=c2>
<div class=dfc>
<span class=l-btn id=tl></span>
</div>
<div class=dfec>
<input id=tb class=slider type=range min=0 max=255 value=0>
<p class=text id=t-tb>0</p>
</div>
</div>
</div>
</div>
</div>
</div>
<div class="setting dnone">
<div class=row>
<div class="partition c2 p-left">
<p class="text tL">Basic</p>
<div class="dfs-c px py">
<button class=s-btn id=resetMCU>Restart MCU</button>
<button class="s-btn mt" id=resetState>Reset State</button>
<button class="s-btn mt" id=resetCreds>Reset Credentials</button>
<button class="s-btn mt" id=factoryReset>Factory Reset</button>
<a href="" id=capture download><button class="s-btn mt">Capture Current Image</button></a>
<div class="mt dfsc">
<button class=s-btn id=status>Check Status</button> <p class="mx tM text" id=t-status></p>
</div>
</div>
</div>
<div class="partition c2 p-right">
<p class="text tL">Advanced</p>
<div class="dfs-c px py">
<input class=text-b placeholder="AP SSID" id=AP_SSID>
<input type=password class="text-b mt" placeholder="AP PASS" id=AP_PASS>
<button class="s-btn mt" id=AP_SUBMIT>Submit</button>
<input class="text-b mt" placeholder="STA SSID" id=STA_SSID>
<input type=password class="text-b mt" placeholder="STA PASS" id=STA_PASS>
<button class="s-btn mt" id=STA_SUBMIT>Submit</button>
<div class="dfsc mt">
<button class=s-btn id=RSSI>Get RSSI</button><p class="text mx tM" id=t-RSSI></p>
</div>
<div class="dfsc mt">
<p class="text mx tM">STA</p>
<label class=switch>
<input id=WIFIMODE type=checkbox>
<span class=check></span>
</label>
<p class="text mx tM">AP</p>
</div>
</div>
</div>
</div>
</div>
</div>
<script src=hscript.js></script>
</body>
</html>
)=====";