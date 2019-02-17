#!python3

import irsdk, time, serial, math, sys, keyboard

puertoSerial = 'COM6'

class State:
    ir_connected = False
    last_car_setup_tick = -1
    
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
        0: 0x88,
        1: 0xBF,
        2: 0xF9,
        3: 0xA4,
        4: 0xB0,
        5: 0x99,
        6: 0x92,
        7: 0x82,
        8: 0xF8
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
    currentLap = math.floor(currentLap * 1000)
    if ( (currentLap / 10000) % 10) > 5 :
            currentLap = currentLap + 40000;

    delta = ir['LapDeltaToOptimalLap'] * 100
    gear = ir['Gear']
    deltaSign = 0xFF
    
    if delta < 0:
        deltaSign = 0xBF
    else:
        deltaSign = 0xFF
    
    delta = math.fabs(delta)
    
    if keyboard.is_pressed('4'):  # if key '4' is pressed 
        data = [
            0xCA, 
            convertIntToDigit(math.floor(delta) % 10), 
            convertIntToDigit(math.floor(delta / 10) % 10),
            convertIntToDigitPoint(math.floor(delta / 100) % 10),
            deltaSign,
            0xFF,        
            convertIntToDigit(math.floor(speed) % 10),
            convertIntToDigit(math.floor(speed / 10) % 10),
            convertIntToDigit2(math.floor(speed / 100) % 10),
            
            convertRPMToLEDS1(rpmPercent), 
            convertRPMToLEDS2(rpmPercent),         
            convertGearToDigit(gear)
        ]
    else:
        data = [
            0xCA, 
            convertIntToDigit(math.floor(delta) % 10), 
            convertIntToDigit(math.floor(delta / 10) % 10),
            convertIntToDigitPoint(math.floor(delta / 100) % 10),
            deltaSign,
            0xFF,        
            convertIntToDigit(math.floor(speed) % 10),
            convertIntToDigit(math.floor(speed / 10) % 10),
            convertIntToDigit2(math.floor(speed / 100) % 10),
            
            convertRPMToLEDS1(rpmPercent), 
            convertRPMToLEDS2(rpmPercent),         
            convertGearToDigit(gear)
        ]
    
    if ser.isOpen():
        ser.write(data)
    
    print(currentLap)

if __name__ == '__main__':

    print()
    print('########### My Python iRacing 1.0 ###########')
    print()
    
    ir = irsdk.IRSDK()
    state = State()
    
    try:
        ser = serial.Serial(puertoSerial, 9600)
        print('Conectado al puerto: ', puertoSerial)
        
    except  serial.SerialException:
        print('Error al conectar al puerto: ', puertoSerial)
        sys.exit(1)
        
    try:
        while True:
            check_iracing()
            if state.ir_connected:
                loop()
            #time.sleep(1/60)
            time.sleep(1/2) #Para testeo
    except KeyboardInterrupt:
        print ('Shutdown requested...exiting')