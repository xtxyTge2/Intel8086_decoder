# Intel8086_decoder

#
Attempt at writing a 8086/8088 instruction set decoder. The idea for this project came by following along the [Computer Enhance](https://www.computerenhance.com/) series, where its an exercise
to decode some(!) of the 8086 instructions. We attempt to go a few steps further and try to decode some more instructions...

# References
[instruction manual](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf), see pp. 160 for the instruction encoding scheme

# Building
To build the project we need CMake, then run:
```
git clone https://github.com/xtxyTge2/Intel8086_decoder
cd Intel8086_decoder
cmake -S . -B build
cmake --build build
```
