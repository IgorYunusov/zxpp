#ifndef Z80_H
#define Z80_H

// #include "instruction.h"
#include "defines.h"
#include "memory.h"
#include "instructions.h"
#include "devices.h"

#include <iostream>
#include <stdint.h>
#include <set>
#include <tuple>
#include <vector>
#include <chrono>
// #include <unordered_map>
#include <assert.h>
#include <array>

typedef std::tuple<uint8_t, uint8_t, uint8_t> opcode;

static const std::set<uint8_t> prefixes = { 0xDD, 0xFD, 0xED, 0xCB };

#define CLOCK_TIME ( 1.0 / 3500000.0 )

// Parse the next instruction from given memory location
int parseNextInstruction(uint8_t* location);

struct Word {                   // Endianness dependent!
    uint8_t low;
    uint8_t high;
};

struct WordFlags {
    union {
        uint8_t byte;           // Accumulator
        struct {                // Flags
            bool CF : 1;        // Carry
            bool NF : 1;        // Last operation was addition (0) or subtraction (1)
            bool PF : 1;        // Parity or signed overflow flag
            bool XF : 1;        // Undocumented: Copy of bit 3 of the result
            bool HF : 1;        // Half carry
            bool YF : 1;        // Undocumented: Copy of bit 5 of the result
            bool ZF : 1;        // Zero flag
            bool SF : 1;        // Is result negative? (Copy of MSB)
        };
    } low;
    uint8_t high;
};

struct Z80Registers {
    uint16_t PC;                // Program counter
    uint16_t SP;                // Stack pointer

    // Index registers
    union {
        uint16_t word;
        Word bytes;
    } IX;
    union {
        uint16_t word;
        Word bytes;
    } IY;

    union {                     // Interrupt register and memory refresh register
        uint16_t word;
        Word bytes;
    } IR;

    // Accumulator and flags
    union {
        uint16_t word;
        WordFlags bytes;
    } AF;

    // General purpose registers
    union {
        uint16_t word;
        Word bytes;
    } BC;

    union {
        uint16_t word;
        Word bytes;
    } DE;

    union {
        uint16_t word;
        Word bytes;
    } HL;

    // Alternate register set
    union {
        uint16_t word;
        WordFlags bytes;
    } AFx;

    union {
        uint16_t word;
        Word bytes;
    } BCx;

    union {
        uint16_t word;
        Word bytes;
    } DEx;

    union {
        uint16_t word;
        Word bytes;
    } HLx;
};

class Z80IOPorts {
    public:
        void registerDevice(IDevice* device);

        void writeToPort(uint16_t port, uint8_t value);
        uint8_t readPort(uint16_t port);

    private:
        std::vector<IDevice*> m_devices;
};

class Z80 {
    public:
        Z80();
        void init();                    // Set power-on defaults
        Z80Registers* getRegisters();
        Z80IOPorts* getIoPorts();
        void setIFF1(bool b);
        void setIFF2(bool b);
        bool getIFF2();
        void halt();
        int getInterruptMode();
        void setInterruptMode(int m);

        void simulateFrame(Spectrum48KMemory* m);

        void printState(Spectrum48KMemory* m);
    private:
        void nextInstruction(Spectrum48KMemory* m);
        int runInstruction(int instruction, Spectrum48KMemory* m);
        Z80Registers m_registers;
        Z80IOPorts m_ioPorts;
        bool m_IFF1;                    // Interrupt flip-flops
        bool m_IFF2;

        // TODO: execute NOPs while halted
        bool m_isHalted;
        bool m_isWaiting;               // WAIT pin active
        int m_interruptMode;

        std::array<Instruction, NUM_INSTRUCTIONS> m_instructionSet;

        int m_cyclesSinceLastFrame;
};

#endif