#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Monstec 2.4G";
const char* password = "santossilva";

const int pino_contagem = 4;
const int relePin = 5;
const int buttonPin = 18;

volatile int contagem = 0;
volatile int ml = 0;
volatile bool button_state = false;
volatile int ml_set_value = 100;

WebServer server(80);

void contagem_pulso() {
  contagem++;
  if (contagem >= 10) {
    contagem = 0;
    ml++;
  }
}
void sv_hdl(void* parameter){
  while(true){
    server.handleClient();
  }
}

void setup() {
  pinMode(relePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, []() {
    String html = web_page();
    server.send(200, "text/html", html);
  });

  server.on("/getSEN", HTTP_GET, []() {
    String response = String(ml);
    server.send(200, "text/plain", response);
});
  server.on("/setML", HTTP_POST, []() {
  String data = server.arg("plain");
  
  int receivedValue = data.toInt();
  Serial.println(receivedValue);
  ml_set_value = receivedValue;
  server.send(200, "text/plain", "Dados recebidos com sucesso");
});

  server.begin();

  attachInterrupt(digitalPinToInterrupt(pino_contagem), contagem_pulso, RISING);

  // Create a task for asy_func
  int pulseValue = 100;
  xTaskCreate(sv_hdl,"taskUm",9999,NULL,1,NULL);
}

void loop() {
  
  
  while (true) {
    delay(100);
    if (!digitalRead(buttonPin) && !button_state) {
      digitalWrite(relePin, HIGH);
      Serial.println("Contagem iniciada");
      ml = 0;
      Serial.println(ml_set_value);
      while (true) {
        if (ml >= ml_set_value) {
          Serial.println("Desliga rele");
          break;
        }
      }
      button_state = true;
      digitalWrite(relePin, LOW);
      Serial.println("Contagem finalisada");
    } else if (digitalRead(buttonPin)) {
      button_state = false;
    }
  }
}

String web_page() {
  String html = R"(
    <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart doser</title>

    <script>
        let interval = 500;
        let val = 0;
        let cont = 0;
        let ajaxRequest = new XMLHttpRequest();

        function ajaxLoad(ajaxURL) {
          ajaxRequest.open('GET', ajaxURL, true);
          ajaxRequest.onreadystatechange = function () {
            if (ajaxRequest.readyState == 4 && ajaxRequest.status == 200) {
              let ajaxResult = ajaxRequest.responseText;
              document.getElementById('fluxo').innerHTML = ajaxResult + " ml";

              if (parseInt(ajaxResult) !== val) {
                val = parseInt(ajaxResult);
                cont = 0;
                interval = 500
              } else if (parseInt(ajaxResult) === val) {
                cont += 1;
              }

              if (cont >= 10) {
                interval = 5000;
              }

              console.log(interval, val, cont);

              // Após a atualização do intervalo, chame `update` novamente com o novo intervalo
              clearInterval(intervalId);
              intervalId = setInterval(update, interval);
            }
          };
          ajaxRequest.send();
        }

        function sendData() {
            const valueToSend = document.getElementById('setValue').value;
            let xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4) {
                if (xhr.status === 200) {
                    console.log('Dados enviados com sucesso');
                } else {
                    console.error('Erro ao enviar dados');
                }
                }
            };
            xhr.open('POST', 'setML', true);
            const dataToSend = valueToSend ;
            xhr.send(dataToSend);
        }

        function update() {
          ajaxLoad('getSEN');
        }

        // Inicialize o intervalo
        let intervalId = setInterval(update, interval);
    </script>




    <style>
        input::-webkit-outer-spin-button,
        input::-webkit-inner-spin-button {
        -webkit-appearance: none;
        margin: 0;
        }
        *{
            font-family: Arial, Helvetica, sans-serif;
            color: #fff;
            font-size: 20px;
            text-align: center;
            box-sizing: border-box;
            font-weight: 300;
            margin: 0px;
        }
        body{
            background-color: #232323;
            
        }
        .container{
            display: flex;
            text-align: center;
            justify-content: space-evenly;
            align-items: center;
        }
        .cards{
            display: flex;
            background-color: #d9d9d959;
            border-radius: 5px;
            width: 217px;
            height: 271px;
            max-width: 217px;
            max-height: 271px;
            align-items: center;
            flex-direction: column;
            padding: 35px;
        }


        #selection{
            justify-content: space-between;
        }
        .cards #setValue{
            
            background-color: #00000000;
            width: 50%;
            border-top: none;
            border-right: none;
            border-left: none;
            border-bottom-width: 5px;
            border-color: #D9D9D9;
            font-size: 30px;
            
        }
        .cards #setValue:focus{
            outline-style: none;
            background-color: #7a7a7a;
        }
        #selection #btn{
            background-color: #00FF8591;
            border-style: none;
            font-size: 15px;
            width: 100px;
            border-radius: 3px;
            padding: 3px;
        }
        #selection #btn:hover{
            background-color: #00be6291;
        }
        #selection button:active{
            background-color: #34ff9d91;
        }


        #added{
            justify-content: space-between
            
        }
        #added span{
            font-size: 35px;
        }

        #valve{
            display: flex;
            justify-content: space-between;
        }
        #valve button{
            background: none;
            border: none;
        }
        #valve svg path:hover{
            fill: #14cc73;
        }
        img{
            width: 200px;
        }
    </style>
</head>
<body>
    <img src="https://i.postimg.cc/SxFJyF5V/logo-monstec-sem-fundo-branca-1.png">
    <div class="container">
        <div class="cards" id="valve">
            <h1>Valve state</h1>
            <button><svg xmlns="http://www.w3.org/2000/svg" width="105" height="105" viewBox="0 0 105 105" fill="">
                <path d="M59.0625 6.5625C59.0625 2.93262 56.1299 0 52.5 0C48.8701 0 45.9375 2.93262 45.9375 6.5625V52.5C45.9375 56.1299 48.8701 59.0625 52.5 59.0625C56.1299 59.0625 59.0625 56.1299 59.0625 52.5V6.5625ZM29.4287 24.7324C32.2178 22.415 32.5869 18.2725 30.2695 15.4834C27.9521 12.6943 23.8096 12.3252 21.0205 14.6426C10.1924 23.666 3.28125 37.2832 3.28125 52.5C3.28125 79.6729 25.3271 101.719 52.5 101.719C79.6729 101.719 101.719 79.6729 101.719 52.5C101.719 37.2832 94.7871 23.666 83.959 14.6426C81.1699 12.3252 77.0273 12.7148 74.71 15.4834C72.3926 18.252 72.7822 22.415 75.5508 24.7324C83.5283 31.3564 88.5732 41.3437 88.5732 52.5C88.5732 72.4336 72.4131 88.5938 52.4795 88.5938C32.5459 88.5938 16.3857 72.4336 16.3857 52.5C16.3857 41.3437 21.4512 31.3564 29.4082 24.7324H29.4287Z" fill="#1FB06A"/>
            </svg></button>
            <span></span>
        </div>
        <div class="cards" id="added">
            <h1>Added ml</h1>
            
            <svg xmlns="http://www.w3.org/2000/svg" width="84" height="76" viewBox="0 0 84 76" fill="none">
                <path d="M31.5 17.75V19.5312L15.75 17.75C12.8461 17.75 10.5 19.8727 10.5 22.5C10.5 25.1273 12.8461 27.25 15.75 27.25L31.5 25.4688L36.5859 24.8898L36.75 24.875L36.9141 24.8898L42 25.4688L57.75 27.25C60.6539 27.25 63 25.1273 63 22.5C63 19.8727 60.6539 17.75 57.75 17.75L42 19.5312V17.75C42 15.1227 39.6539 13 36.75 13C33.8461 13 31.5 15.1227 31.5 17.75ZM5.25 41.5C2.34609 41.5 0 43.6227 0 46.25V55.75C0 58.3773 2.34609 60.5 5.25 60.5H21.6727C24.9867 64.8047 30.5156 67.625 36.75 67.625C42.9844 67.625 48.5133 64.8047 51.8273 60.5H57.75C60.6539 60.5 63 62.6227 63 65.25C63 67.8773 65.3461 70 68.25 70H78.75C81.6539 70 84 67.8773 84 65.25C84 52.1281 72.2531 41.5 57.75 41.5H52.5L48.7922 38.1453C47.8078 37.2547 46.4789 36.75 45.0844 36.75H42V30.2484L36.75 29.6547L31.5 30.2484V36.75H28.432C27.0375 36.75 25.7086 37.2547 24.7242 38.1453L21 41.5H5.25Z" fill="white"/>
            </svg>
            <span id="fluxo">300 ml</span>
        </div>
        <div class="cards" id="selection">
            <h1>Selection ml</h1>
            <input type="number" id="setValue" value="500">
            <input type = "button" onclick="sendData();" id="btn">

        </div>
    </div>
</body>
</html>
  )";
  return html;
}