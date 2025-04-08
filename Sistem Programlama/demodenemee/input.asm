ORG 1000H


MVI A, 25H     ; Move immediate value 25H to register A
MOV A, B       ; Move the value in register A to register B
LXI H, 2050H   ; Load immediate value 2050H into register pair HL
LDA 3050H      ; Load the value at memory address 3050H into register A
STA 4050H      ; Store the value in register A to memory address 4050H


ADD B          ; Add the value in register B to register A
ADI 10H        ; Add immediate value 10H to register A
SUB B          ; Subtract the value in register B from register A
SUI 05H        ; Subtract immediate value 05H from register A
INR A          ; Increment the value in register A
DCR A          ; Decrement the value in register A


ANA B          ; Logical AND the value in register B with register A
XRA B          ; Logical XOR the value in register B with register A
ORA B          ; Logical OR the value in register B with register A
CMP B          ; Compare the value in register B with register A
CMA            ; Complement the value in register A


JMP 2000H      ; Jump to memory address 2000H
JC 2100H       ; Jump to memory address 2100H if carry flag is set
JNC 2200H      ; Jump to memory address 2200H if carry flag is not set
JZ 2300H       ; Jump to memory address 2300H if zero flag is set
JNZ 2400H      ; Jump to memory address 2400H if zero flag is not set


HLT            ; Halt the execution
