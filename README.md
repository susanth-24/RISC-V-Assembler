# Assembly to Machine Code Assembler RISCV

## Overview

This project is an assembler that converts assembly code written in a custom RISC-V architecture language into machine code. The assembler supports a variety of RISC-V instructions, including R-type, I-type, S-type, SB-type, U-type, and SJ-type instructions.

## Features

- Supports a subset of RISC-V assembly instructions.
- Converts assembly code to machine code in hexadecimal format.
- Handles different data directives, including .data, .word, .half, .byte, .dword, .asciiz.
- Supports labels and assembler directives.
- Generates machine code output in a specified format.

## Usage

To use the assembler, follow these steps:

1. Write your RISC-V assembly code in a file with a `.asm` extension.
2. Run the assembler with the input assembly file and specify an output file for the machine code.


```bash
g++ main.cpp -o main
```

```bash
./main input.asm output.mc
```

3. The assembler will generate a machine code file (`output.mc`) with the converted instructions.

## Example

**Input (assembly code - input.asm):**

```assembly
# Sample RISC-V Assembly Code
.text
add x1, x2, x3
sub x4, x5, x6
```

**Output (machine code - output.mc):**

```assembly
0x0 0x003100B3
0x4 0x40628233
0x8 0x00000000
```
