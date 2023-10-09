from machine import Pin
import time

# Defina os pinos para contagem e controle
pino_contagem = Pin(4, Pin.IN)
rele = Pin(5, Pin.OUT)
button = Pin(18, Pin.IN, Pin.PULL_UP)  # Configura o pino 18 como entrada com pull-up
button_state = 0

contagem = 0
ml = 0


def contagem_pulso(pin):
    global contagem
    global ml
    contagem += 1
    if contagem >= 10:
        contagem = 0
        ml += 1
        print(ml)
        

pino_contagem.irq(trigger=Pin.IRQ_RISING, handler=contagem_pulso)



while True:
    time.sleep(0.5)
    if not button.value() and button_state == 0:
        rele.on()
        print("contagem iniciada")
        ml = 0
        while True:
            if ml >= 100:
                print("desliga rele")
                break 
            elif ml < 100:
                pass
        button_state = 1
        rele.off()
    elif button.value():
        button_state = 0