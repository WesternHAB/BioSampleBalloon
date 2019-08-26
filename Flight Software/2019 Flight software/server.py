#--------------------------------------------------------------------------------------------------------------------------------------------
#    Name          : server.py
#    Author        : Stephen Amey
#    Date          : Aug. 9, 2019
#    Purpose  	   : This program serves as a test groundstation server for Western University's HAB project.
#--------------------------------------------------------------------------------------------------------------------------------------------


'''

So what exactly does the GUI need to do?

Upon recieving telemetry:
	Update displayed values, e.g. altitude, flight time, pod positions... Basically all the stuff recieved in the telementry.
	
Also need a text-bar object that we can enter a string in, and a send button
	I imagine that since the clients a global variable, it MIGHT be able to see it? Else use that queue thing

'''


#-----------------------------------------------------------------------------------------------------------\
#                                                    Imports                                                |
#-----------------------------------------------------------------------------------------------------------/


import socket
import tkinter as tk
import _thread
import time
import queue


#-----------------------------------------------------------------------------------------------------------\
#                                                   Variables                                               |
#-----------------------------------------------------------------------------------------------------------/


request_queue = queue.Queue()
result_queue = queue.Queue()

server_address = socket.gethostbyname(socket.gethostname())
server_port = 54444 #0

remote_address = ''
remote_port = 0

#Heartbeat
beat_delay = 1.0
last_beat = 0
#Socket setup
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSocket.bind((server_address, server_port))
serverSocket.settimeout(beat_delay)

remoteSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


#-----------------------------------------------------------------------------------------------------------\
#                                              GUI thread functions                                         |
#-----------------------------------------------------------------------------------------------------------/


def submit_to_tkinter(callable, *args, **kwargs):
    request_queue.put((callable, args, kwargs))
    return result_queue.get()

t = None
def threadmain():
    global t

    def timertick():
        try:
            callable, args, kwargs = request_queue.get_nowait()
        except queue.Empty:
            pass
        else:
            print("something in queue")
            retval = callable(*args, **kwargs)
            result_queue.put(retval)

        t.after(500, timertick)

    t = tk.Tk()
    t.configure(width=1220, height=800)
    t.title("Western HAB ground server")
	
    #Flight time
    timeLabel = tk.Label(text="Time")
    timeLabel.place(x=20, y=180)
	
    timeBox = tk.Label(height=1, width=20, bg="white", name='timeBox')
    timeBox.place(x=60, y=180)
	
    #GPS
    GPSLabel = tk.Label(text="GPS")
    GPSLabel.place(x=120, y=30)
	
    altitudeLabel = tk.Label(text="Altitude")
    altitudeLabel.place(x=20, y=60)	
    altitudeBox = tk.Label(height=1, width=10, bg="white", name='altitudeBox')
    altitudeBox.place(x=100, y=60)

    longitudeLabel = tk.Label(text="Longitude")
    longitudeLabel.place(x=20, y=90)	
    longitudeBox = tk.Label(height=1, width=10, bg="white", name='longitudeBox')
    longitudeBox.place(x=100, y=90)
	
    latitudeLabel = tk.Label(text="Latitude")
    latitudeLabel.place(x=20, y=120)	
    latitudeBox = tk.Label(height=1, width=10, bg="white", name='latitudeBox')
    latitudeBox.place(x=100, y=120)
	
    #BME
    BMELabel = tk.Label(text="BME280")
    BMELabel.place(x=320, y=30)
	
    temperatureLabel = tk.Label(text="Temperature")
    temperatureLabel.place(x=220, y=60)	
    temperatureBox = tk.Label(height=1, width=10, bg="white", name='temperatureBox')
    temperatureBox.place(x=300, y=60)

    pressureLabel = tk.Label(text="Pressure")
    pressureLabel.place(x=220, y=90)	
    pressureBox = tk.Label(height=1, width=10, bg="white", name='pressureBox')
    pressureBox.place(x=300, y=90)
	
    humidityLabel = tk.Label(text="Humidity")
    humidityLabel.place(x=220, y=120)	
    humidityBox = tk.Label(height=1, width=10, bg="white", name='humidityBox')
    humidityBox.place(x=300, y=120)
	
	
    #Act1
    Act1Label = tk.Label(text="Actuator 1")
    Act1Label.place(x=520, y=30)
	
    Act1PosLabel = tk.Label(text="Position")
    Act1PosLabel.place(x=420, y=60)	
    Act1PosBox = tk.Label(height=1, width=10, bg="white", name='act1PosBox')
    Act1PosBox.place(x=500, y=60)

    Act1TempLabel = tk.Label(text="Temperature")
    Act1TempLabel.place(x=420, y=90)	
    Act1TempBox = tk.Label(height=1, width=10, bg="white", name='act1TempBox')
    Act1TempBox.place(x=500, y=90)
	
    Act1AOSLabel = tk.Label(text="Act status")
    Act1AOSLabel.place(x=420, y=120)	
    Act1AOSBox = tk.Label(height=1, width=10, bg="white", name='act1AOSBox')
    Act1AOSBox.place(x=500, y=120)
	
    Act1HOSLabel = tk.Label(text="Heater status")
    Act1HOSLabel.place(x=420, y=150)	
    Act1HOSBox = tk.Label(height=1, width=10, bg="white", name='act1HOSBox')
    Act1HOSBox.place(x=500, y=150)

    #Act2
    Act2Label = tk.Label(text="Actuator 2")
    Act2Label.place(x=720, y=30)
	
    Act2PosLabel = tk.Label(text="Position")
    Act2PosLabel.place(x=620, y=60)	
    Act2PosBox = tk.Label(height=1, width=10, bg="white", name='act2PosBox')
    Act2PosBox.place(x=700, y=60)

    Act2TempLabel = tk.Label(text="Temperature")
    Act2TempLabel.place(x=620, y=90)	
    Act2TempBox = tk.Label(height=1, width=10, bg="white", name='act2TempBox')
    Act2TempBox.place(x=700, y=90)
	
    Act2AOSLabel = tk.Label(text="Act status")
    Act2AOSLabel.place(x=620, y=120)	
    Act2AOSBox = tk.Label(height=1, width=10, bg="white", name='act2AOSBox')
    Act2AOSBox.place(x=700, y=120)
	
    Act2HOSLabel = tk.Label(text="Heater status")
    Act2HOSLabel.place(x=620, y=150)	
    Act2HOSBox = tk.Label(height=1, width=10, bg="white", name='act2HOSBox')
    Act2HOSBox.place(x=700, y=150)
	
    #Act3
    Act3Label = tk.Label(text="Actuator 3")
    Act3Label.place(x=920, y=30)
	
    Act3PosLabel = tk.Label(text="Position")
    Act3PosLabel.place(x=820, y=60)	
    Act3PosBox = tk.Label(height=1, width=10, bg="white", name='act3PosBox')
    Act3PosBox.place(x=900, y=60)

    Act3TempLabel = tk.Label(text="Temperature")
    Act3TempLabel.place(x=820, y=90)	
    Act3TempBox = tk.Label(height=1, width=10, bg="white", name='act3TempBox')
    Act3TempBox.place(x=900, y=90)
	
    Act3AOSLabel = tk.Label(text="Act status")
    Act3AOSLabel.place(x=820, y=120)	
    Act3AOSBox = tk.Label(height=1, width=10, bg="white", name='act3AOSBox')
    Act3AOSBox.place(x=900, y=120)
	
    Act3HOSLabel = tk.Label(text="Heater status")
    Act3HOSLabel.place(x=820, y=150)	
    Act3HOSBox = tk.Label(height=1, width=10, bg="white", name='act3HOSBox')
    Act3HOSBox.place(x=900, y=150)

    #Act4
    Act4Label = tk.Label(text="Actuator 4")
    Act4Label.place(x=1120, y=30)
	
    Act4PosLabel = tk.Label(text="Position")
    Act4PosLabel.place(x=1020, y=60)	
    Act4PosBox = tk.Label(height=1, width=10, bg="white", name='act4PosBox')
    Act4PosBox.place(x=1100, y=60)

    Act4TempLabel = tk.Label(text="Temperature")
    Act4TempLabel.place(x=1020, y=90)	
    Act4TempBox = tk.Label(height=1, width=10, bg="white", name='act4TempBox')
    Act4TempBox.place(x=1100, y=90)
	
    Act4AOSLabel = tk.Label(text="Act status")
    Act4AOSLabel.place(x=1020, y=120)	
    Act4AOSBox = tk.Label(height=1, width=10, bg="white", name='act4AOSBox')
    Act4AOSBox.place(x=1100, y=120)
	
    Act4HOSLabel = tk.Label(text="Heater status")
    Act4HOSLabel.place(x=1020, y=150)	
    Act4HOSBox = tk.Label(height=1, width=10, bg="white", name='act4HOSBox')
    Act4HOSBox.place(x=1100, y=150)
	
    #Event box
    eventBox = tk.Label(height=28, width=141, justify="left", bg="white", name="eventBox", anchor="sw")
    eventBox.place(x=40, y=220)
	
    #Commands
    commandBox = tk.Entry(width=135, name="commandBox")
    commandBox.place(x=20, y=740)
    sendButton = tk.Button(text='SEND', name='sendButton', height=2, width=10, command=sendCommand)
    sendButton.place(x=1120, y=727)
    t.bind('<Return>', enterKeyPressed)
	
    #Start the GUI loop
    timertick()
    t.mainloop()
	
def updateDisplays(message_text):
    #Gets the first 3 characters
    identifier = message_text[1:6]

    if(identifier == "INTLZ"):
        #Nothing
        print("INIT MSG")
    elif(identifier == "EVENT"):
        #Just print it to the console
        t.children["eventBox"].configure(text=(t.children["eventBox"]["text"] + "\n" + message_text))
        eventFile = open("eventLog.txt", "a")
        eventFile.write(message_text + "\n")
        eventFile.close()
    else:
        telemetryFile = open("telemetryLog.txt", "a")
        telemetryFile.write(message_text)
        telemetryFile.close()
	
        #Assumed telemetry, so parse it
        fields = (message_text).split(",")
		
        #Time
        t.children["timeBox"].configure(text=fields[2])
		
        #GPS
        t.children["altitudeBox"].configure(text=fields[4])
        t.children["longitudeBox"].configure(text=fields[6])
        t.children["latitudeBox"].configure(text=fields[7])
		
        #BME
        t.children["temperatureBox"].configure(text=fields[8])
        t.children["pressureBox"].configure(text=fields[9])
        t.children["humidityBox"].configure(text=fields[10])
		
        #Act1
        t.children["act1PosBox"].configure(text=fields[11])
        t.children["act1TempBox"].configure(text=fields[12])
        t.children["act1AOSBox"].configure(text=(("OVR_CLOSE", "OVR_OPEN","AUTO")[int(fields[13])]))
        t.children["act1HOSBox"].configure(text=(("OVR_DISABLE", "OVR_ENABLE","AUTO")[int(fields[14])]))
		
        #Act2
        t.children["act2PosBox"].configure(text=fields[15])
        t.children["act2TempBox"].configure(text=fields[16])
        t.children["act2AOSBox"].configure(text=(("OVR_CLOSE", "OVR_OPEN","AUTO")[int(fields[17])]))
        t.children["act2HOSBox"].configure(text=(("OVR_DISABLE", "OVR_ENABLE","AUTO")[int(fields[18])]))
		
        #Act3
        t.children["act3PosBox"].configure(text=fields[19])
        t.children["act3TempBox"].configure(text=fields[20])
        t.children["act3AOSBox"].configure(text=(("OVR_CLOSE", "OVR_OPEN","AUTO")[int(fields[21])]))
        t.children["act3HOSBox"].configure(text=(("OVR_DISABLE", "OVR_ENABLE","AUTO")[int(fields[22])]))
		
        #Act4
        t.children["act4PosBox"].configure(text=fields[23])
        t.children["act4TempBox"].configure(text=fields[24])
        t.children["act4AOSBox"].configure(text=(("OVR_CLOSE", "OVR_OPEN","AUTO")[int(fields[25])]))
        t.children["act4HOSBox"].configure(text=(("OVR_DISABLE", "OVR_ENABLE","AUTO")[int(fields[26].split("\r")[0])]))  
		
		
def sendCommand():
    if(remote_address != '' and remote_port != '' and t.children["commandBox"].get() != ""):
        t.children["eventBox"].configure(text=(t.children["eventBox"]["text"] + "\nCOMMAND: " + t.children["commandBox"].get()))
        message = bytes("GROUNDSTATION," + t.children["commandBox"].get(), 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        t.children["commandBox"].delete(0, 'end')

def enterKeyPressed(event):
    sendCommand()


#-----------------------------------------------------------------------------------------------------------\
#                                                  Program run                                              |
#-----------------------------------------------------------------------------------------------------------/  


if __name__ == '__main__':
    #If the files do not currently exist, create them
    open("eventLog.txt", "w")
    open("telemetryLog.txt", "w")

    #Hosts the server
    server_address = serverSocket.getsockname()[0]
    server_port = serverSocket.getsockname()[1]
    print('Hosting at : ' + server_address + ':' + str(server_port))
	
    #Starts the GUI
    _thread.start_new_thread(threadmain, ())

    #Enters the main loop
    while True:
        #Send a heart-beat every second
        if(remote_address != '' and remote_port != '' and (time.time() - last_beat) >= beat_delay):
            last_beat = time.time()
            message = bytes("GROUNDSTATION,HBT\0", 'utf-8')
            remoteSocket.sendto(message, (str(remote_address), int(remote_port)))

	    #Attempt to receive a packet
        try:      
            message, address = serverSocket.recvfrom(1024)
            print('(' + address[0] + ':' + str(address[1]) + ') : ' + message.decode('utf-8'))
            updateDisplays(message.decode('utf-8'))

		    #If no client address yet defined
            if(remote_address == ''):
                #Records the new sender
                remote_address = address[0]
                remote_port = str(address[1])	
			
        except Exception as e:	
            print(e)
            1