#!/usr/bin/env python3

import time
import sys
import os

time.sleep(0.1)

fd = int(sys.argv[1])

with os.fdopen(fd,'w') as fdfile:
    fdfile.write("I am a serpent.")
    fdfile.close()
