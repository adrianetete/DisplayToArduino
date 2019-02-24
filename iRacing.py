#!python3

# Data sent to arduino: 
# 
#   First byte is always 0xCA, this is the segment preamble
#   Next 8 bytes goes to 7 Segment 8-Digit Led Display
#   1 byte to first shifter (which controls first 8 LEDs)
#   1 byte to second shifter (which controls second 8 LEDs)
#   1 byte to gear display (7 segment display)
# 
# Data = [
#   0xCA,
#
#   8ºPosition-Display,
#   7ºPosition-Display,
#   6ºPosition-Display,
#   5ºPosition-Display,
#   4ºPosition-Display,
#   3ºPosition-Display,
#   2ºPosition-Display,
#   1ºPosition-Display,
# 
#   First-8-LEDs,
#   Next-8-LEDs,
#   Gear-Display
# ]
#
# Los caracteres que muestra el display son los siguientes:
#   0b11000000;  0xC0;  // caracter[ 0 ]
#   0b11111001;  0xF9;  // caracter[ 1 ]
#   0b10100100;  0xA4;  // caracter[ 2 ]
#   0b10110000;  0xB0;  // caracter[ 3 ]
#   0b10011001;  0x99;  // caracter[ 4 ]
#   0b10010010;  0x92;  // caracter[ 5 ]
#   0b10000010;  0x82;  // caracter[ 6 ]
#   0b11111000;  0xF8;  // caracter[ 7 ]
#   0b10000000;  0x80;  // caracter[ 8 ]
#   0b10010000;  0x90;  // caracter[ 9 ]
#   0b10111111;  0xBF;  // caracter[ - ]
#   0b10001000;  0x88;  // caracter[ R ]


import irsdk, time, serial, math, sys, keyboard, win32api
import serial.tools.list_ports
from threading import Thread

puertoSerial = 'COM5'    
displayInfo = 0
escuchaTeclado = True

class State:
    ir_connected = False
    last_car_setup_tick = -1

def buscarArduino():

    i = 0
    maxCom = 0
    puertoCom = ""
    
    print("Buscando Arduino...")
    
    # Recibe una lista de puertos COM activos
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        # Aumenta el contador de puertos
        maxCom = maxCom + 1

    # en p[0] esta el nombre del puerto
    # en p[1] esta el nombre del dispositivo que hay conectado
    while i < maxCom:
        if "CH340" in p[1]:  # Busca en el nombre del dispositivo "CH340"         
            puertoCom = str(p[0])
            i = maxCom # Salir de bucle    
        i = i + 1
    return puertoCom
    
# Con esta funcion 'hacemos algo' solamente al soltar la tecla, para evitar que se haga en bucle mientras se mantiene pulsada
def capturarTeclado(key):

    global displayInfo
    pulsado = False

    while True and escuchaTeclado:
        if win32api.GetAsyncKeyState(key):           
            pulsado = True
            
        elif pulsado:
            pulsado = False
            # Cambiamos el estado de 'displayInfo'
            if displayInfo < 2:
                displayInfo = displayInfo + 1
            else:
                displayInfo = 0
            
        # Uso esto para evitar que el proceso consuma toda la CPU
        time.sleep(0.05)

    
def convertIntToDigit(num):
    return {
        0: 0xC0,
        1: 0xF9,
        2: 0xA4,
        3: 0xB0,
        4: 0x99,
        5: 0x92,
        6: 0x82,
        7: 0xF8,
        8: 0x80,
        9: 0x90
    }.get(num, 0xFF) #Default 0xFF
    
def convertIntToDigit2(num):
    return {
        0: 0xFF,
        1: 0xF9,
        2: 0xA4,
        3: 0xB0,
        4: 0x99,
        5: 0x92,
        6: 0x82,
        7: 0xF8,
        8: 0x80,
        9: 0x90
    }.get(num, 0xFF) #Default 0xFF

def convertIntToDigitPoint(num):
    return {
        0: 0x40,
        1: 0x79,
        2: 0x24,
        3: 0x30,
        4: 0x19,
        5: 0x12,
        6: 0x02,
        7: 0x78,
        8: 0x00,
        9: 0x10
    }.get(num, 0xFF) #Default 0xFF
    
def convertGearToDigit(num):
    return {
        -1: 0x88,
        0: 0xBF,
        1: 0xF9,
        2: 0xA4,
        3: 0xB0,
        4: 0x99,
        5: 0x92,
        6: 0x82,
        7: 0xF8,
        7: 0x90
    }.get(num, 0xFF) #Default 0xFF
    
def convertRPMToLEDS1(rpmPerc):
    if ( (rpmPerc >= 75.00) and (rpmPerc < 76.25) ):
        return 0x80
    elif ((rpmPerc >= 76.25) and (rpmPerc < 77.50) ):
        return 0xC0
    elif ((rpmPerc >= 77.50) and (rpmPerc < 78.75) ):
        return 0xE0
    elif ((rpmPerc >= 78.75) and (rpmPerc < 80.00) ):
        return 0xF0
    elif ((rpmPerc >= 80.00) and (rpmPerc < 81.25) ):
        return 0xF8
    elif ((rpmPerc >= 81.25) and (rpmPerc < 82.50) ):
        return 0xFC
    elif ((rpmPerc >= 82.50) and (rpmPerc < 83.75) ):
        return 0xFE
    elif rpmPerc >= 83.75:
        return 0xFF
    else:
        return 0x00
        
def convertRPMToLEDS2(rpmPerc):
    if ( (rpmPerc >= 85.00) and (rpmPerc < 86.25) ):
        return 0x80
    elif ((rpmPerc >= 86.25) and (rpmPerc < 87.50) ):
        return 0xC0
    elif ((rpmPerc >= 87.50) and (rpmPerc < 88.75) ):
        return 0xE0
    elif ((rpmPerc >= 88.75) and (rpmPerc < 90.00) ):
        return 0xF0
    elif ((rpmPerc >= 90.00) and (rpmPerc < 91.25) ):
        return 0xF8
    elif ((rpmPerc >= 91.25) and (rpmPerc < 92.50) ):
        return 0xFC
    elif ((rpmPerc >= 92.50) and (rpmPerc < 93.75) ):
        return 0xFE
    elif rpmPerc >= 95.00:
        return 0xFF
    else:
        return 0x00

def check_iracing():
    if state.ir_connected and not (ir.is_initialized and ir.is_connected):
        state.ir_connected = False        
        state.last_car_setup_tick = -1        
        ir.shutdown()
        print('Desconectado de irsdk')
        
    elif not state.ir_connected and ir.startup() and ir.is_initialized and ir.is_connected:
        state.ir_connected = True
        print('Conectado a irsdk')

def loop():
    speed = (ir['Speed'] * 3.6)
    fuel = ir['FuelLevel']
    rpmMax = ir['DriverInfo']['DriverCarRedLine']
    rpm = ir['RPM']
    rpmPercent = (rpm * 100) / rpmMax
    
    currentLap = ir['LapCurrentLapTime']
    currentLap = currentLap + 40 * math.floor(currentLap/60)
    currentLap = currentLap * 1000

    deltaSessionOptimal = ir['LapDeltaToSessionOptimalLap'] * 100
    deltaSessionBest = ir['LapDeltaToSessionBestLap'] * 100
    
    gear = ir['Gear']
    deltaSign = 0xFF
    deltaSign2 = 0xFF
    
    if deltaSessionOptimal < 0:
        deltaSign = 0xBF
    else:
        deltaSign = 0xFF
    
    deltaSessionOptimal = math.fabs(deltaSessionOptimal)
    
    if deltaSessionBest < 0:
        deltaSign2 = 0xBF
    else:
        deltaSign2 = 0xFF
    
    deltaSessionBest = math.fabs(deltaSessionBest)

    data = [
        0xCA, 
        convertIntToDigit(math.floor(currentLap) % 10), 
        convertIntToDigit(math.floor(currentLap / 10) % 10),
        convertIntToDigit(math.floor(currentLap / 100) % 10),
        convertIntToDigitPoint(math.floor(currentLap / 1000) % 10),
        convertIntToDigit(math.floor(currentLap / 10000) % 10),
        convertIntToDigitPoint(math.floor(currentLap / 100000) % 10),
        convertIntToDigit2(math.floor(currentLap / 1000000) % 10),
        0xFF,
        
        convertRPMToLEDS1(rpmPercent), 
        convertRPMToLEDS2(rpmPercent),         
        convertGearToDigit(gear)
    ]
        
    if displayInfo == 1:
        data = [
            0xCA, 
            convertIntToDigit(math.floor(deltaSessionOptimal) % 10), 
            convertIntToDigit(math.floor(deltaSessionOptimal / 10) % 10),
            convertIntToDigitPoint(math.floor(deltaSessionOptimal / 100) % 10),
            deltaSign,
            0xFF,        
            convertIntToDigit(math.floor(speed) % 10),
            convertIntToDigit(math.floor(speed / 10) % 10),
            convertIntToDigit2(math.floor(speed / 100) % 10),
            
            convertRPMToLEDS1(rpmPercent), 
            convertRPMToLEDS2(rpmPercent),         
            convertGearToDigit(gear)
        ]
        
    elif displayInfo == 2:
        data = [
            0xCA, 
            convertIntToDigit(math.floor(deltaSessionOptimal) % 10), 
            convertIntToDigit(math.floor(deltaSessionOptimal / 10) % 10),
            convertIntToDigitPoint(math.floor(deltaSessionOptimal / 100) % 10),
            deltaSign,
            
            convertIntToDigit(math.floor(deltaSessionBest) % 10), 
            convertIntToDigit(math.floor(deltaSessionBest / 10) % 10),
            convertIntToDigitPoint(math.floor(deltaSessionBest / 100) % 10),
            deltaSign2,
            
            convertRPMToLEDS1(rpmPercent), 
            convertRPMToLEDS2(rpmPercent),         
            convertGearToDigit(gear)
        ]
    
    if ser.isOpen():
        ser.write(data)
    
if __name__ == '__main__':

    print()
    print('########### Mi Python iRacing 1.0 ###########')
    print()
    
    ir = irsdk.IRSDK()
    state = State()
    
    try:
        puertoSerial = buscarArduino()
        
        ser = serial.Serial(puertoSerial, 9600)
        print("Conectado a Arduino en " + puertoSerial + ".")
        
    except  serial.SerialException:
        print("Error al conectar al al Arduino.")
                
    try:
        Thread(target=capturarTeclado, args=(0x34,)).start() # Detectar el botón 4
        
        while True:
            check_iracing()
            if state.ir_connected:
                loop()
            time.sleep(1/60)
            # time.sleep(1/2) #Para testeo
    except KeyboardInterrupt:
    
        print ('Saliendo...')
        escuchaTeclado = False
        sys.exit(1)