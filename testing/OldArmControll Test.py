from inputs import get_gamepad
while 1:
     events = get_gamepad()
     for event in events:
         if event.code is 'ABS_Y':
             print(event.ev_type, event.code, event.state)