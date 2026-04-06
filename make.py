#!/usr/bin/env python3

import os

os.system("gcc -g -fsanitize=address -I/usr/include/freetype2 -lSDL3 -lfreetype src/*.c -o vanilla")
