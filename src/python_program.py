#!/usr/bin/env python3

import time
import sys
import os


def main():
    #print("You called me with arguments:", sys.argv)

    write_fd = int(sys.argv[2])
    write_to_pipe(write_fd)

    read_fd = int(sys.argv[1])
    read_from_pipe(read_fd) # Blocking


def write_to_pipe(write_fd):
    with os.fdopen(write_fd,'w') as fdfile:
        fdfile.write("I am a serpent.")
        fdfile.close()


def read_from_pipe(read_fd):
    recieved_message = []
    recieved_message = os.read(read_fd, 4096)
    print("Message from C:", recieved_message.decode());


if __name__ == "__main__":
    main()

