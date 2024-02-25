#!/usr/bin/env python3

import time
import sys
import os

time.sleep(0.1)

#print("You called me with arguments:", sys.argv)

read_fd = int(sys.argv[1])
write_fd = int(sys.argv[2])

with os.fdopen(write_fd,'w') as fdfile:
    fdfile.write("I am a serpent.")
    fdfile.close()

recieved_message = []
recieved_message = os.read(read_fd, 4096)
print("Message from C:", recieved_message.decode());