#--------------------------------------------------------------------------------------------------------------------------------------------
#    Name          : server.py
#    Author        : Stephen Amey
#    Date          : Aug. 9, 2019
#    Purpose  	   : This program serves as a test groundstation server for Western University's HAB project.
#--------------------------------------------------------------------------------------------------------------------------------------------


#-----------------------------------------------------------------------------------------------------------\
#                                                    Imports                                                |
#-----------------------------------------------------------------------------------------------------------/


import socket
import time


#-----------------------------------------------------------------------------------------------------------\
#                                                   Variables                                               |
#-----------------------------------------------------------------------------------------------------------/


server_address = socket.gethostbyname(socket.gethostname())
server_port = 54442 #0

remote_address = ''
remote_port = ''

#Heartbeat
beat_delay = 1.0

#Socket setup
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSocket.bind((server_address, server_port))
serverSocket.settimeout(beat_delay)

remoteSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


#-----------------------------------------------------------------------------------------------------------\
#                                                  Program run                                              |
#-----------------------------------------------------------------------------------------------------------/  


server_address = serverSocket.getsockname()[0]
server_port = serverSocket.getsockname()[1]
print('Hosting at : ' + server_address + ':' + str(server_port))

while True:
    if(remote_address != ''):
        message = bytes('HBT\0', 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
       
        message = bytes("POD_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("POD_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("POD_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("POD_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("LOGGING_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("BME_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("CAMERA_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        message = bytes("GPS_BYPASS\0", 'utf-8')
        remoteSocket.sendto(message, (str(remote_address), int(remote_port)))
        break

	#Attempt to receive a packet
    try:      
        message, address = serverSocket.recvfrom(1024)
        print('(' + address[0] + ':' + str(address[1]) + ') : ' + message.decode('utf-8'))		
       	
        #Records the new sender
        remote_address = address[0]
        remote_port = str(address[1])		
    except Exception as e:
        1