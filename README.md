# 8-bit CPU
This project is a demonstration of how a barebones, but turing complete, 8-bit CPU works at the logic gate/register level, demonstrated using [Logisim](https://github.com/logisim-evolution/logisim-evolution). This project was motivated by [Ben Eater](https://www.youtube.com/@BenEater) and the [video series](https://www.youtube.com/playlist?list=PLowKtXNTBypGqImE405J2565dvjafglHU) produced by him where he builds an 8-bit CPU on breadboards.

Logisim files for this project could be found [here](./logisim-files/).

# Architecture
![Architecture](./pictures/architecture.svg)
This design follows a single data/address bus scheme. All peripherals (registers, counters, RAM etc.) share data amongst themselves using this common bus. The control unit co-ordinates the data sharing between these peripherals.

The CPU is made up of the following components:
#### System Bus
This is the common conduit for data sharing between peripherals. The bus is 8-bit wide and transmits instructions, address and data. While this is not optimal and restricts the size of address space and the size of the instructions, it is a good enough solution for the application at hand: demonstrating computer architecture.

#### A-Register
This is the primary register of the computer and in which data could be stored or from which data could be obtained  by the user using the supplied instructions.

#### B-Register
This is the secondary register of the computer which is used by the CPU to store temporary/intermediate data and cannot be accessed by the user using the supplied instructions to load data from or store data into.

#### ALU (adder and subtractor)
The ALU for this computer consists of only an 8-bit adder which is wired in way that it could also do subtraction using 2's-complement addition. The A and B registers connect directly to the ALU, which continuously sums(or subtracts) the contents of these registers. User cannot access the data stored in the ALU directly but could access it via the A-Register using the supplied instructions.

#### CPU Flags register
This is a 3-bit wide register and is what makes the CPU *Turing Complete*. Without this, the CPU cannot have the conditional jump ability, which in turn renders the computer Turing *In*complete. This is hooked to the ALU and indicates if the current arithmetic operation resulted in a zero result or a result with a carry or a negative result (MSB set). These flags are used by the conditional jump instructions to alter the exectuion path accordingly and constitute an close-loop system with feedback.

#### Program Counter
This is a 4-bit binary counter and it counts from 0 to 15 which supplies the memory address of the current instruction to be executed by the computer. User can manipulate the program count using the supplied jump/conditional jump instructions.

#### Memory Address Register
This is a 4-bit register connected directly to the main memory(RAM) and which supplies the RAM with the memory address from which data is to be read or into which data is to be written in the next clock cycle. User can load data from or store data into any location within the RAM using the supplied instructions.

#### RAM
Being the main memory of the CPU, it stores the program code and the program data. The RAM for this computer is 16 bytes in size, which I do agree is *very* limited, but more than sufficient for the purpose at hand: demonstrating the working of a computer.

#### Instruction Register
This is an 8-bit register which stores the current instruction under exection. The four MSB bits encode the instruction itself and the remaining bits is the operand for the instruction. This register takes in 8 bits of data from the bus and relays back only the four LSB (operand) back to the bus. The four MSB bits goes to the instruction decoder.

#### Control Unit
As the name suggests, this is what controls and coordinates actions between all peripherals of the CPU. The control unit acts as the bus master and dictates which peripheral spits data out onto the bus and which peripherals consume data from the bus as required by the instruction currently under execution. The control unit is comprised of two elements:
1. Micro-Instruction Counter : Each instruction for this CPU is comprised of a series of micro-instructions, each of which is executed per clock cycle. This counter keeps track of micro-instruction under execution.
2. Instruction Decoder : This is an EEPROM which takes in data from the flags register, instruction register and the micro-instruction counter and maps this data in to what is called as a *Contol Word*, which is 16-bits long. Each bit of this control word controls a peripheral in a certain way that enables the control unit to do its job.

#### Display Unit
This is an array of four seven-segment display which is connected to the bus and can render the binary data in the bus into the human-readable decimal form. The display can operate in two modes:
1. Normal Mode : Renders the 8-bit data in the bus into an unsigned decimal number. Can display numbers from 0 to 255.
2. 2's Complement Mode : Renders the 8-bit data in the bus into a signed decimal number. Can display numbers from -128 to +127.

#### System Clock Module
This produces the clock signal to synchronize the circuit's operation and could be operated in two modes:
1. Automatic : The oscillator is connected to the system clock line.
2. Manual : A manual switch is connected to the system clock line. This comes in handy to carefully observe each micro-instruction under exectution and for debugging purposes.

User can stop this module from outputting clock signal using the supplied halt instruction.

# Schematic
![Schematic](./pictures/schematic.png)
*I agree that this is not the most aesthetically pleasing diagram. With a decent amount of persistence, this circuit could be read and understood.*
