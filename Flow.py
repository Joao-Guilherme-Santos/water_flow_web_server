import network
import time
import socket
from machine import Pin
import machine
from time import sleep
import _thread

# configuracoes de pino
pino_contagem = Pin(4, Pin.IN)

contagem = 0
ml = 0


def contagem_pulso(pin):
    global contagem
    global ml
    contagem += 1
    if contagem >= 10:
        contagem = 0
        ml += 1
        print('ml adicionado')


# Configurar a rede Wi-Fi
wifi_ssid = "Oi_D417"
wifi_password = "fuTUF26p"
led = machine.Pin(2, machine.Pin.OUT)

# Crie um objeto WLAN
wlan = network.WLAN(network.STA_IF)

# Desactive o ponto de acesso
wlan.active(True)

# Conecte-se à rede Wi-Fi
wlan.connect(wifi_ssid, wifi_password)

# Tenta se conectar
for a in range(1, 60):
    if wlan.isconnected():
        print("Connected to Wi-Fi")
        print(wlan.ifconfig()[0])
        led.on()
        sleep(1)
        led.off()
        break

    else:
        print("\n.")
    sleep(1)


# Pagina web com o codigo AJAX
def web_page():
    html = """
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <font></font>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Esp32</title>
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

                function update() {
                  ajaxLoad('getSEN');
                }

                // Inicialize o intervalo
                let intervalId = setInterval(update, interval);
            </script>
            <style>

                body{
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    font-family:sans-serif;
                }
                #center{
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    font-size: 40px;
                    width: 250px;
                    height: 300px;

                    border-radius: 10px;
                }
                #line{
                    height: 3px;
                    width: 70%;
                    border-radius: 5px;
                    background-color: #829c9c;
                }
                h1{
                    margin-bottom: 80px;
                }
            </style>
        </head>
        <body style="background-color: rgb(251, 255, 234);">
            <div id="center" >
                <h1 style="font-size: 25px;">Medidor de fluxo</h1>
                <span id = "fluxo"> -- ml </span>
                <span id="line"></span>
            </div>
        </body>
    </html>
    """
    return html


# configuracao do socket
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_socket.bind(('', 8080))
tcp_socket.listen(5)

pino_contagem.irq(trigger=Pin.IRQ_RISING, handler=contagem_pulso)

while True:
    # aceita novas conexoes
    conn, addr = tcp_socket.accept()
    print('Nova conexão')

    # comunicacao
    request = conn.recv(1024)
    # print(f'request feito, valor retornado {ml}')

    # respostas
    request = str(request)
    update = request.find('/getSEN')
    if update == 6:
        response = str(ml)
    else:
        response = web_page()

    # configuracoes HTTP
    conn.send('HTTP/1.1 200 OK\n')
    conn.send('Content-Type: text/html\n')
    conn.send('Connection: close\n\n')
    conn.sendall(response)

    conn.close()




