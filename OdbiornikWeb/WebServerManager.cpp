#include "WebServerManager.h"
#include <ArduinoJson.h>

// =====================================================
// Instancja klasy
// =====================================================

WebServerManager* WebServerManager::instance = nullptr;

// =====================================================
// Konstruktor
// =====================================================

WebServerManager::WebServerManager(
    WebData &webDataRef
)
    : server(80),
      webSocket(81),
      webData(webDataRef)
{
    lastPhoneReceive = 0;
    lastSend = 0;
    instance = this;
}


// =====================================================
// Strona WWW
// =====================================================

const char webpage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html lang="pl">

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1.0">

<title>ESP32-S3</title>
<script>
var StickStatus={xPosition:0,yPosition:0,x:0,y:0,cardinalDirection:"C"},JoyStick=function(t,e,i){var n=void 0===(e=e||{}).title?"joystick":e.title,o=void 0===e.width?0:e.width,a=void 0===e.height?0:e.height,r=void 0===e.internalFillColor?"#00AA00":e.internalFillColor,h=void 0===e.internalLineWidth?2:e.internalLineWidth,d=void 0===e.internalStrokeColor?"#003300":e.internalStrokeColor,s=void 0!==e.internalDrawArrows&&1==e.internalDrawArrows,c=void 0===e.externalLineWidth?2:e.externalLineWidth,u=void 0===e.externalStrokeColor?"#008000":e.externalStrokeColor,l=void 0===e.autoReturnToCenter||1==e.autoReturnToCenter,S=void 0!==e.axisOnlyX&&1==e.axisOnlyX,f=void 0!==e.axisOnlyY&&1==e.axisOnlyY;i=i||function(t){};var g=document.getElementById(t);g.style.touchAction="none";var k=document.createElement("canvas");k.id=n,0===o&&(o=g.clientWidth),0===a&&(a=g.clientHeight),k.width=o,k.height=a,g.appendChild(k);var x=k.getContext("2d"),v=0,P=2*Math.PI,L=(k.width-(k.width/2+10))/2,y=L+5,w=L+30,m=k.width/2,M=k.height/2,C=k.width/10,T=-1*C,p=k.height/10,F=-1*p,E=m,W=M;function D(){x.beginPath(),x.arc(m,M,w,0,P,!1),x.lineWidth=c,x.strokeStyle=u,x.stroke()}function I(){x.beginPath(),E<L&&(E=y),E+L>k.width&&(E=k.width-y),W<L&&(W=y),W+L>k.height&&(W=k.height-y),x.arc(E,W,L,0,P,!1);var t=x.createRadialGradient(m,M,5,m,M,200);t.addColorStop(0,r),t.addColorStop(1,d),x.fillStyle=t,x.fill(),x.lineWidth=h,x.strokeStyle=d,x.stroke()}function G(){s&&(arrowLength=L/2,x.strokeStyle=d,x.lineWidth=h,f||Y(x,E-arrowLength,W,E+arrowLength,W,!0),S||Y(x,E,W-arrowLength,E,W+arrowLength,!0))}function Y(t,e,i,n,o,a){arrowHeadSize=.2,t.lineCap="round",dx=n-e,dy=o-i,headLength=Math.sqrt(dx*dx+dy*dy)*arrowHeadSize,t.beginPath(),t.moveTo(e,i),t.lineTo(n,o),t.stroke(),angle=Math.atan2(dy,dx),t.beginPath(),t.moveTo(n-headLength*Math.cos(angle-Math.PI/6),o-headLength*Math.sin(angle-Math.PI/6)),t.lineTo(n,o),t.lineTo(n-headLength*Math.cos(angle+Math.PI/6),o-headLength*Math.sin(angle+Math.PI/6)),t.stroke(),a&&(angle=Math.atan2(-dy,-dx),t.beginPath(),t.moveTo(e-headLength*Math.cos(angle-Math.PI/6),i-headLength*Math.sin(angle-Math.PI/6)),t.lineTo(e,i),t.lineTo(e-headLength*Math.cos(angle+Math.PI/6),i-headLength*Math.sin(angle+Math.PI/6)),t.stroke())}S&&f&&(S=!1,f=!1),"ontouchstart"in document.documentElement?(k.addEventListener("touchstart",(function(t){v=1,R=t.targetTouches[0].identifier}),!1),document.addEventListener("touchmove",(function(t){1===v&&t.targetTouches[0].target===k&&(E=t.targetTouches[0].pageX,W=t.targetTouches[0].pageY,"BODY"===k.offsetParent.tagName.toUpperCase()?(E-=k.offsetLeft,W-=k.offsetTop):(E-=k.offsetParent.offsetLeft,W-=k.offsetParent.offsetTop),f&&(E=m),S&&(W=M),x.clearRect(0,0,k.width,k.height),D(),I(),G(),StickStatus.xPosition=E,StickStatus.yPosition=W,StickStatus.x=((E-m)/y*100).toFixed(),StickStatus.y=((W-M)/y*100*-1).toFixed(),StickStatus.cardinalDirection=O(),i(StickStatus))}),!1),document.addEventListener("touchend",(function(t){if(t.changedTouches[0].identifier!==R)return;v=0,l&&(E=m,W=M);x.clearRect(0,0,k.width,k.height),D(),I(),G(),StickStatus.xPosition=E,StickStatus.yPosition=W,StickStatus.x=((E-m)/y*100).toFixed(),StickStatus.y=((W-M)/y*100*-1).toFixed(),StickStatus.cardinalDirection=O(),i(StickStatus)}),!1)):(k.addEventListener("mousedown",(function(t){v=1}),!1),document.addEventListener("mousemove",(function(t){1===v&&(E=t.pageX,W=t.pageY,"BODY"===k.offsetParent.tagName.toUpperCase()?(E-=k.offsetLeft,W-=k.offsetTop):(E-=k.offsetParent.offsetLeft,W-=k.offsetParent.offsetTop),f&&(E=m),S&&(W=M),x.clearRect(0,0,k.width,k.height),D(),I(),G(),StickStatus.xPosition=E,StickStatus.yPosition=W,StickStatus.x=((E-m)/y*100).toFixed(),StickStatus.y=((W-M)/y*100*-1).toFixed(),StickStatus.cardinalDirection=O(),i(StickStatus))}),!1),document.addEventListener("mouseup",(function(t){v=0,l&&(E=m,W=M);x.clearRect(0,0,k.width,k.height),D(),I(),G(),StickStatus.xPosition=E,StickStatus.yPosition=W,StickStatus.x=((E-m)/y*100).toFixed(),StickStatus.y=((W-M)/y*100*-1).toFixed(),StickStatus.cardinalDirection=O(),i(StickStatus)}),!1)),D(),I(),G();var R=null;function O(){var t="",e=E-m,i=W-M;return i>=F&&i<=p&&(t="C"),i<F&&(t="N"),i>p&&(t="S"),e<T&&("C"===t?t="W":t+="W"),e>C&&("C"===t?t="E":t+="E"),t}this.GetWidth=function(){return k.width},this.GetHeight=function(){return k.height},this.GetPosX=function(){return E},this.GetPosY=function(){return W},this.GetX=function(){return((E-m)/y*1000).toFixed()},this.GetY=function(){return((W-M)/y*1000*-1).toFixed()},this.GetDir=function(){return O()}};
</script>

<style>
html, body {
    margin: 0;
    padding: 0;
    width: 100%;
    height: 100%;
}

body {
    font-family: Arial, sans-serif;
    background: #111;
    color: white;
    text-align: center;
    margin: 0;
    padding: 20px;
}

.container {
    max-width: 500px;
    margin: auto;
}

h1 {
    margin-bottom: 25px;
}

.card {
    background: #222;
    border-radius: 15px;
    padding: 20px;
    margin-bottom: 15px;
}

.row {
    display: flex;
    justify-content: space-between;
    padding: 10px;
    border-bottom: 1px solid #444;
}

.row-center {
    display: flex;
    justify-content: space-evenly;
    padding: 10px;
    border-bottom: 1px solid #444;
}

.value {
    font-weight: bold;
}

.status {
    font-size: 20px;
    margin-bottom: 20px;
}

.parent {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    grid-template-rows: auto minmax(0, 1fr);
    gap: 0;
    width: 100%;
    height: 100%;
    max-width: 100%;
    max-height: 100%;
    box-sizing: border-box;
    justify-items: center;
    align-items: center;
    overflow: hidden;
}

.joystick-size {
    width: min(100%, 100vh);
    aspect-ratio: 1 / 1;
    max-width: 100%;
    max-height: 100%;
    box-sizing: border-box;
}

.div1 { grid-area: 2 / 1 / 2 / 2; }
.div2 { grid-area: 2 / 2 / 2 / 3; }

</style>

</head>


<body>

<div class="container">

    <h1>ESP32-S3</h1>

    <div class="status">
        WebSocket:
        <span id="status">LACZENIE...</span>
    </div>


    <div class="card">

        <h2>Joystick</h2>

        <div class="row" onclick="toggleButton(12)">
            <span>Smooth mode</span>
            <span class="value" id="smoothMode">NIE</span>
        </div>

        <div class="row" onclick="toggleButton(11)">
            <span>Sterowanie</span>
            <span class="value" id="phoneMode">Joystick</span>
        </div>

        <div class="row-center">
            <span>X</span>
            <span>Y</span>
        </div>

        <div class="row-center">
            <span class="value" id="x">0</span>
            <span class="value" id="y">0</span>
        </div>

    </div>

    <div class="parent">
        <div class="div1 joystick-size" id="joystick1"></div>
        <div class="div2 joystick-size" id="joystick2"></div>
    </div>


    <div class="card">

        <h2>Ustawienia</h2>

        <div class="row">
            <span>Maksymalna prędkość</span>
            <span class="value" id="speedMaxValue">300</span>
        </div>

        <input
            type="range"
            id="speedMax"
            min="300"
            max="1000"
            value="300"
            step="10"
            style="width:100%;"
        >

    </div>

    <div class="card">

        <h2>Przyciski</h2>

        <div class="row" onclick="toggleButton(1)">
            <span>Przycisk I</span>
            <span class="value" id="button1">NIE</span>
        </div>

        <div class="row" onclick="toggleButton(2)">
            <span>Przycisk II</span>
            <span class="value" id="button2">NIE</span>
        </div>

        <div class="row" onclick="toggleButton(3)">
            <span>Przycisk III</span>
            <span class="value" id="button3">NIE</span>
        </div>

        <div class="row" onclick="toggleButton(4)">
            <span>Przycisk IV</span>
            <span class="value" id="button4">NIE</span>
        </div>

    </div>

    <div class="card">

        <h2>Statystyki</h2>

        <div class="row-center">
            <span>Połączenie z hoverboardem</span>
            <span class="value" id="hoverFeedbackOk">Rozłączono</span>
        </div>

        <div class="row">
            <span>Napięcie akumulatora</span>
            <span class="value" id="iVolt">0V</span>
        </div>

        <div class="row">
            <span>Naładowanie baterii</span>
            <span class="value" id="batteryLevel">0%</span>
        </div>

        <div class="row-center">
            <span>Pobór prądu</span>
        </div>

        <div class="row-center">
            <span class="value" id="iAmpL">0A</span>
            <span class="value" id="iAmpR">0A</span>
        </div>

        <div class="row-center">
            <span>Prędkość</span>
        </div>

        <div class="row-center">
            <span class="value" id="iSpeedL">0km/h</span>
            <span class="value" id="iSpeedR">0km/h</span>
        </div>

    </div>
</div>

<script>

let websocket;

var phoneMode = false;

function connectWebSocket() {

    websocket = new WebSocket(
        "ws://" + window.location.hostname + ":81/"
    );


    websocket.onopen = function() {

        document.getElementById("status")
            .innerHTML = "POLACZONO";

    };


    websocket.onclose = function() {

        document.getElementById("status")
            .innerHTML = "ROZLACZONO";

        setTimeout(
            connectWebSocket,
            2000
        );

    };


    websocket.onmessage = function(event) {

        let data = JSON.parse(event.data);

        document.getElementById("x")
            .innerHTML = data.x;

        document.getElementById("y")
            .innerHTML = data.y;
        
        document.getElementById("speedMax")
            .value = data.speedMax;

        document.getElementById("speedMaxValue")
            .innerHTML = data.speedMax;

        document.getElementById("phoneMode")
            .innerHTML =
            data.phoneMode ? "Telefon" : "Joystick";
        
        phoneMode = data.phoneMode;

        document.getElementById("smoothMode")
            .innerHTML =
            data.smoothMode ? "TAK" : "NIE";
        
        document.getElementById("button1")
            .innerHTML =
            data.button1 ? "TAK" : "NIE";
            
        document.getElementById("button2")
            .innerHTML =
            data.button2 ? "TAK" : "NIE";

        document.getElementById("button3")
            .innerHTML =
            data.button3 ? "TAK" : "NIE";

        document.getElementById("button4")
            .innerHTML =
            data.button4 ? "TAK" : "NIE";

        document.getElementById("hoverFeedbackOk")
            .innerHTML =
            data.hoverFeedbackOk ? "Połaczono" : "Rozłączono";
        
        document.getElementById("iVolt")
            .innerHTML = (data.iVolt/100).toFixed(2) + "V";
        
        document.getElementById("batteryLevel")
            .innerHTML = Math.max(0, Math.min(100, (data.iVolt - 2700) / 1500 * 100)) + "%";
        
        document.getElementById("iAmpL")
            .innerHTML = (data.iAmpL/100).toFixed(2) + "A";
        
        document.getElementById("iAmpR")
            .innerHTML = (data.iAmpR/100).toFixed(2) + "A";
        
        document.getElementById("iSpeedL")
            .innerHTML = (data.iSpeedL/100).toFixed(2) + "km/h";
        
        document.getElementById("iSpeedR")
            .innerHTML = (data.iSpeedR/100).toFixed(2) + "km/h";
    };
}

document.getElementById("speedMax").oninput = function() {

    let value = parseInt(this.value);

    document.getElementById("speedMaxValue")
        .innerHTML = value;

    if (websocket.readyState === WebSocket.OPEN) {

        websocket.send(
            JSON.stringify({
                command: "speedMax",
                value: value
            })
        );
    }
};

function toggleButton(number) {

    if (websocket.readyState !== WebSocket.OPEN) {
        return;
    }

    websocket.send(
        JSON.stringify({
            command: "button",
            number: number
        })
    );
}

var joystick1_options = {'internalDrawArrows': true, 'axisOnlyX': true}
var joystick1 = new JoyStick('joystick1', joystick1_options);
      
var joystick2_options = {'internalDrawArrows': true, 'axisOnlyY': true}
var joystick2 = new JoyStick('joystick2', joystick2_options);

function joystick_Func(){
    var js1 = Number(joystick1.GetX());
    var js2 = Number(joystick2.GetY());

    websocket.send(
        JSON.stringify({
            command: "joystick",
            x: js1,
            y: js2
        })
    );
}

setInterval(() => {
    if(phoneMode)
    {
        joystick_Func();
    }
}, 100);

connectWebSocket();

</script>

</body>

</html>

)rawliteral";


// =====================================================
// SETUP SERWERA
// =====================================================

void WebServerManager::begin() {

    Serial.println("==============================");
    Serial.println("Uruchamianie WebServer");

    // -------------------------------------------------
    // HTTP
    // -------------------------------------------------

    server.on(
        "/",
        handleRootStatic
    );

    server.begin();

    Serial.println("HTTP: port 80");

    // -------------------------------------------------
    // WebSocket
    // -------------------------------------------------

    webSocket.begin();

    webSocket.onEvent(webSocketEventStatic);

    Serial.println("WebSocket: port 81");
    Serial.println("WebServer gotowy");
}

// =====================================================
// LOOP SERWERA
// =====================================================

void WebServerManager::loop() {

    server.handleClient();

    webSocket.loop();

    // Wysyłanie danych co 100 ms

    if (millis() - lastSend >= 100) {

        lastSend = millis();

        sendData();
    }

    if(millis() - lastPhoneReceive >= 500)
    {
        webData.phoneX = 0;
        webData.phoneY = 0;
    }
}

// =====================================================
// HTTP
// =====================================================

void WebServerManager::handleRoot() {

    server.send(
        200,
        "text/html",
        webpage
    );
}


void WebServerManager::handleRootStatic() {

    if (instance != nullptr) {
        instance->handleRoot();
    }
}

// =====================================================
// WebSocket
// =====================================================

void WebServerManager::webSocketEvent(
    uint8_t num,
    WStype_t type,
    uint8_t *payload,
    size_t length
) {

    switch (type) {

        // =================================================
        // POŁĄCZENIE
        // =================================================

        case WStype_CONNECTED:

            Serial.printf(
                "WebSocket: klient %u polaczony\n",
                num
            );

            break;


        // =================================================
        // ROZŁĄCZENIE
        // =================================================

        case WStype_DISCONNECTED:

            Serial.printf(
                "WebSocket: klient %u rozlaczony\n",
                num
            );

            // Bezpiecznie zerujemy joystick telefonu
            webData.phoneX = 0;
            webData.phoneY = 0;
            webData.phoneMode = false;

            break;


        // =================================================
        // ODEBRANO TEKST
        // =================================================

        case WStype_TEXT:
        {

            Serial.printf(
                "WebSocket od klienta %u: %.*s\n",
                num,
                (int)length,
                payload
            );


            // =================================================
            // PARSOWANIE JSON
            // =================================================

            JsonDocument doc;

            DeserializationError error =
                deserializeJson(
                    doc,
                    payload,
                    length
                );


            if (error) {

                Serial.print(
                    "Blad parsowania JSON: "
                );

                Serial.println(
                    error.c_str()
                );

                break;
            }


            // =================================================
            // ODCZYT COMMAND
            // =================================================

            const char* command =
                doc["command"];


            if (command == nullptr) {

                Serial.println(
                    "Brak pola command"
                );

                break;
            }


            // =================================================
            // SPEED MAX
            // =================================================

            if (
                strcmp(
                    command,
                    "speedMax"
                ) == 0
            ) {

                if (!doc["value"].isNull()) {

                    int value =
                        doc["value"] | 0;


                    // Ograniczenie:
                    // 300 - 1000

                    value = constrain(
                        value,
                        300,
                        1000
                    );


                    webData.speedMax =
                        value;


                    Serial.printf(
                        "speedMax zmienione na: %ld\n",
                        webData.speedMax
                    );
                }

                break;
            }


            // =================================================
            // JOYSTICK
            // =================================================

            if (
                strcmp(
                    command,
                    "joystick"
                ) == 0
            ) {
                if(webData.phoneMode)
                {
                    // Domyślnie 0

                    int x =
                        doc["x"] | 0;

                    int y =
                        doc["y"] | 0;


                    // Zakres joysticka:
                    // -1000 ... 1000

                    x = constrain(
                        x,
                        -1000,
                        1000
                    );

                    y = constrain(
                        y,
                        -1000,
                        1000
                    );


                    webData.phoneX =
                        x;

                    webData.phoneY =
                        y;


                    Serial.printf(
                        "Joystick: X=%d Y=%d\n",
                        webData.phoneX,
                        webData.phoneY
                    );

                    lastPhoneReceive = millis();
                }
                break;
            }


            // =================================================
            // PRZYCISK
            // =================================================

            if (
                strcmp(
                    command,
                    "button"
                ) == 0
            ) {

                int number =
                    doc["number"] | 0;


                switch (number) {

                    case 1:

                        webData.button1 =
                            !webData.button1;

                        Serial.printf(
                            "Przycisk 1: %s\n",
                            webData.button1
                                ? "TAK"
                                : "NIE"
                        );

                        break;


                    case 2:

                        webData.button2 =
                            !webData.button2;

                        Serial.printf(
                            "Przycisk 2: %s\n",
                            webData.button2
                                ? "TAK"
                                : "NIE"
                        );

                        break;


                    case 3:

                        webData.button3 =
                            !webData.button3;

                        Serial.printf(
                            "Przycisk 3: %s\n",
                            webData.button3
                                ? "TAK"
                                : "NIE"
                        );

                        break;


                    case 4:

                        webData.button4 =
                            !webData.button4;

                        Serial.printf(
                            "Przycisk 4: %s\n",
                            webData.button4
                                ? "TAK"
                                : "NIE"
                        );

                        break;


                    // =========================================
                    // TRYB TELEFONU
                    // =========================================

                    case 11:

                        webData.phoneMode =
                            !webData.phoneMode;

                        Serial.printf(
                            "phoneMode: %s\n",
                            webData.phoneMode
                                ? "ON"
                                : "OFF"
                        );

                        break;


                    // =========================================
                    // TRYB SMOOTH
                    // =========================================

                    case 12:

                        webData.smoothMode =
                            !webData.smoothMode;

                        Serial.printf(
                            "smoothMode: %s\n",
                            webData.smoothMode
                                ? "ON"
                                : "OFF"
                        );

                        break;


                    default:

                        Serial.printf(
                            "Nieprawidlowy numer przycisku: %d\n",
                            number
                        );

                        break;
                }

                break;
            }


            // =================================================
            // NIEZNANA KOMENDA
            // =================================================

            Serial.printf(
                "Nieznana komenda: %s\n",
                command
            );

            break;
        }


        // =================================================
        // INNE
        // =================================================

        default:

            break;
    }
}

void WebServerManager::webSocketEventStatic(
    uint8_t num,
    WStype_t type,
    uint8_t *payload,
    size_t length
) {

    if (instance != nullptr) {

        instance->webSocketEvent(
            num,
            type,
            payload,
            length
        );
    }
}

// =====================================================
// Wysyłanie danych
// =====================================================

void WebServerManager::sendData() {

    String message = "{";

    message += "\"x\":";
    message += webData.phoneMode ? webData.phoneX : webData.x;

    message += ",\"y\":";
    message += webData.phoneMode ? webData.phoneY :webData.y;

    message += ",\"speedMax\":";
    message += webData.speedMax;

    message += ",\"phoneMode\":";
    message += webData.phoneMode ? "true" : "false";

    message += ",\"smoothMode\":";
    message += webData.smoothMode ? "true" : "false";

    message += ",\"button1\":";
    message += webData.button1 ? "true" : "false";

    message += ",\"button2\":";
    message += webData.button2 ? "true" : "false";

    message += ",\"button3\":";
    message += webData.button3 ? "true" : "false";

    message += ",\"button4\":";
    message += webData.button4 ? "true" : "false";

    message += ",\"hoverFeedbackOk\":";
    message += webData.hoverboardData.hoverFeedbackOk ? "true" : "false";

    message += ",\"iSpeedL\":";
    message += webData.hoverboardData.iSpeedL;

    message += ",\"iSpeedR\":";
    message += webData.hoverboardData.iSpeedR;

    message += ",\"iAmpL\":";
    message += webData.hoverboardData.iAmpL;

    message += ",\"iAmpR\":";
    message += webData.hoverboardData.iAmpR;
    
    message += ",\"iVolt\":";
    message += webData.hoverboardData.iVolt;

    message += "}";

    webSocket.broadcastTXT(
        message
    );
}

// =====================================================
// Pobieranie czasu od ostatniej aktualizacji
// =====================================================

unsigned long WebServerManager::getLastPhoneReceive()
{
    return lastPhoneReceive;
}