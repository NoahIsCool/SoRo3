from inputs import get_gamepad
import socket

# create an INET, STREAMing socket
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#now connect to the web server on port 80 - the normal http port
#s.connect(("127.255.255.255", 80))
pos = [ 90,90,90,90,90,1]

while 1:
     events = get_gamepad()
     for event in events:
          if event.code is 'ABS_Y':#not 'SYN_REPORT':
          
               print(event.code, event.state)
               
               if event.code is 'ABS_Y' and not (-5000 < event.state < 5000):#shoulder L/R
                    pos[0] += (360 / event.state )
                    break                 
               if event.code is 'ABS_X' and not (-5000 < event.state < 5000):#shulder U/D
                    pos[1] += event.state
                    break                 
                            
               if event.code is 'ABS_RY' and not( -5000 < event.state < 5000):#elbow Y
                    pos[2] += event.state
                    break                 
#               if event.code is 'ABS_RX':##if the arm had an elbow x
#                    pos[3] += event.code
#                    break
               
               if event.code is 'ABS_Z' and not( -5 < event.state < 5):#claw down
                    pos[3] += event.state
                    break
               if event.code is 'ABS_RZ' and not( -5 < event.state < 5):#claw up
                    pos[4] += event.state
                    break

               if event.code is 'BTN_TL':#claw grab
                    pos[5] = event.state
                    break
                 
#          if event.code is 'ABS_HAT0X':
#          if event.code is 'ABS_HAT0Y':
#          if event.code is 'BTN_TR':
#          if event.code is 'BTN_NORTH':
#          if event.code is 'BTN_SOUTH':
#          if event.code is 'BTN_EAST':
#          if event.code is 'BTN_WEST':
#          if event.code is 'BTN_START':
#          if event.code is 'BTN_SELECT':
#          if event.code is 'BTN_THUMBL':
#          if event.code is 'BTN_THUMBR':

