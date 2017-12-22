#include "M6502.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NmiVector       0xFFFA
#define ResetVector     0xFFFC
#define InterruptVector 0xFFFE

CONTEXTM6502 context;

void m6502zpGetContext(CONTEXTM6502* dest)
{
    memcpy(dest, &context, sizeof(CONTEXTM6502));
}
void m6502zpSetContext(CONTEXTM6502* source)
{
    memcpy(&context, source, sizeof(CONTEXTM6502));
}
int m6502zppc()
{
    return context.m6502pc;
}

int cyclesRemaining;
int elapsedTicks;
UINT32 status;

#define I context.m6502MemoryRead
#define O context.m6502MemoryWrite
#define X context.m6502x
#define Y context.m6502y
#define IrqPending context.IrqPending
#define S context.m6502s
#define AF context.m6502af
#define A context.A
#define Flags context.Flags
#define PC context.m6502pc
#define PCL context.PCL
#define PCH context.PCH
#define Memory context.m6502Base
#define Op Memory[PC]

#define CBit 0
#define ZBit 1
#define IBit 2
#define DBit 3
#define BBit 4
#define VBit 6
#define NBit 7

#define CMask (1<<CBit)
#define ZMask (1<<ZBit)
#define IMask (1<<IBit)
#define DMask (1<<DBit)
#define BMask (1<<BBit)
#define VMask (1<<VBit)
#define NMask (1<<NBit)

UINT16 AddWithPageCross(UINT16 value, UINT8 index)
{
    UINT16 result = value + index;
    if ((result & ~0xFF) != (value & ~0xFF))
    {
        cyclesRemaining--;
        elapsedTicks++;
    }

    return result;
}

UINT8 GetImmediate8(UINT16 addr)
{
    if (!I)
    {
        return Memory[addr];
    }

    for (struct MemoryReadByte* read = I; (UINT16)read->lowAddr != 0xFFFF; read++)
    {
        if (addr >= read->lowAddr && addr <= read->highAddr)
        {
            return read->memoryCall(addr, read);
        }
    }

    return Memory[addr];
}
UINT16 GetImmediate16(UINT16 addr)
{
    UINT16 result = GetImmediate8(addr) | (GetImmediate8(addr + 1) << 8);
    return result;
}

void SetImmediate8(UINT16 addr, UINT8 value)
{
    if (!O)
    {
        Memory[addr] = value;
        return;
    }

    for (struct MemoryWriteByte* write = O; (UINT16)write->lowAddr != 0xFFFF; write++)
    {
        if (addr >= write->lowAddr && addr <= write->highAddr)
        {
            write->memoryCall(addr, value, write);
            return;
        }
    }

    Memory[addr] = value;
}

UINT8 ZeroPage(UINT16 addr)
{
    return GetImmediate8(addr);
}
UINT8 ZeroPageX(UINT16 addr)
{
    return GetImmediate8(addr) + X;
}
UINT8 ZeroPageY(UINT16 addr)
{
    return GetImmediate8(addr) + Y;
}

UINT16 Absolute(UINT16 addr)
{
    return GetImmediate16(addr);
}
UINT16 AbsoluteX(UINT16 addr)
{
    return AddWithPageCross(GetImmediate16(addr), X);
}
UINT16 AbsoluteY(UINT16 addr)
{
    return AddWithPageCross(GetImmediate16(addr), Y);
}

UINT16 IndirectX(UINT16 addr)
{
    int index = Memory[ZeroPageX(addr)] | (Memory[ZeroPageX(addr + 1)] << 8);
    return Absolute(index);
}
UINT16 IndirectY(UINT16 addr)
{
    UINT8 index = GetImmediate8(addr);
    return AbsoluteY(index);
}

#define SetFlag(_i) Flags |= (_i)
#define ClearFlag(_i) Flags &= ~(_i)
#define CheckFlag(_i) (Flags & (_i))
#define WriteFlag(_i, _value) if (_value)SetFlag(_i);else ClearFlag(_i)

#define SetCarry SetFlag(CMask)
#define ClearCarry ClearFlag(CMask)
#define CheckCarry CheckFlag(CMask)

#define CreateReadInstructions(_name) \
void _name##Immediate()\
{\
    _name(GetImmediate8(PC++));\
}\
void _name##ZeroPage()\
{\
    _name(Memory[ZeroPage(PC++)]);\
}\
void _name##ZeroPageX()\
{\
    _name(Memory[ZeroPageX(PC++)]);\
}\
void _name##ZeroPageY()\
{\
    _name(Memory[ZeroPageY(PC++)]);\
}\
void _name##Absolute()\
{\
    UINT16 addr = Absolute(PC);\
    PC += 2;\
    _name(GetImmediate8(addr));\
}\
void _name##AbsoluteX()\
{\
    UINT16 addr = AbsoluteX(PC);\
    PC += 2;\
    _name(GetImmediate8(addr));\
}\
void _name##AbsoluteY()\
{\
    UINT16 addr = AbsoluteY(PC);\
    PC += 2;\
    _name(GetImmediate8(addr));\
}\
void _name##IndirectX()\
{\
    _name(GetImmediate8(IndirectX(PC++)));\
}\
void _name##IndirectY()\
{\
    _name(GetImmediate8(IndirectY(PC++)));\
}

#define CreateWriteInstructions(_name, _op) \
void _name##ZeroPage()\
{\
    Memory[ZeroPage(PC++)] _op _name();\
}\
void _name##ZeroPageX()\
{\
    Memory[ZeroPageX(PC++)] _op _name();\
}\
void _name##ZeroPageY()\
{\
    Memory[ZeroPageY(PC++)] _op _name();\
}\
void _name##Absolute()\
{\
UINT16 addr = Absolute(PC);\
PC += 2; \
UINT8 result = Memory[addr];\
result _op _name();\
SetImmediate8(addr, result); \
}\
void _name##AbsoluteX()\
{\
UINT16 addr = AbsoluteX(PC);\
PC += 2; \
UINT8 result = Memory[addr];\
result _op _name();\
SetImmediate8(addr, result); \
}\
void _name##AbsoluteY()\
{\
UINT16 addr = AbsoluteY(PC);\
PC += 2; \
UINT8 result = Memory[addr];\
result _op _name();\
SetImmediate8(addr, result); \
}\
void _name##IndirectX()\
{\
UINT16 addr = IndirectX(PC++);\
UINT8 result = Memory[addr];\
result _op _name();\
SetImmediate8(addr, result); \
}\
void _name##IndirectY()\
{\
UINT16 addr = IndirectY(PC++);\
UINT8 result = Memory[addr];\
result _op _name();\
SetImmediate8(addr, result); \
}

#define CreateRefInstructions(_name) \
void _name##A()\
{\
    _name(&A);\
}\
void _name##X()\
{\
    _name(&X);\
}\
void _name##Y()\
{\
    _name(&Y);\
}\
void _name##ZeroPage()\
{\
    _name(Memory + ZeroPage(PC++));\
}\
void _name##ZeroPageX()\
{\
    _name(Memory + ZeroPageX(PC++));\
}\
void _name##ZeroPageY()\
{\
    _name(Memory + ZeroPageY(PC++));\
}\
void _name##Absolute()\
{\
    UINT16 addr = Absolute(PC);\
    PC += 2;\
    _name(Memory + addr);\
}\
void _name##AbsoluteX()\
{\
    UINT16 addr = AbsoluteX(PC);\
    PC += 2;\
    _name(Memory + addr);\
}\
void _name##AbsoluteY()\
{\
    UINT16 addr = AbsoluteY(PC);\
    PC += 2;\
    _name(Memory + addr);\
}\
void _name##IndirectX()\
{\
    _name(Memory + IndirectX(PC++));\
}\
void _name##IndirectY()\
{\
    _name(Memory + IndirectY(PC++));\
}

static const UINT8 OpSize[0x100];
static const UINT8 CycleCost[0x100];

typedef void(*Instruction)();

static const Instruction Instructions[0x100];

// When indexing, an extra cycles comes form wrapping around our word boundary.
UINT16 Wrap(int value)
{
    if (value >= 0x10000)
    {
        cyclesRemaining--;
        elapsedTicks++;
    }

    return value;
}

void Push(UINT8 value)
{
    Memory[0x100 | S--] = value;
}

void PushWord(UINT16 value)
{
    Push((UINT8)(value >> 8));
    Push((UINT8)value);
}

void PushState()
{
    PushWord(PC);
    Push(Flags);
}

UINT8 Pull()
{
    return Memory[0x100 | ++S];
}

UINT16 PullWord()
{
    return Pull() | (Pull() << 8);
}

void WriteNZFlags(UINT8 value)
{
    WriteFlag(ZMask, !value);
    WriteFlag(NMask, value & 0x80);
}

void BRK()
{
    // Push program counter and processor status.
    PushState();

    // Load IRQ interrupt vector.
    PC = Memory[InterruptVector];

    // Set break flag
    SetFlag(BMask);
}

void LDN(UINT8 *n, UINT8 value)
{
    *n = value;
    WriteNZFlags(*n);
}

#define LD(_n) void LD##_n(UINT8 value)\
{\
    LDN(&_n, value);\
}\
CreateReadInstructions(LD##_n)\

LD(A)
LD(X)
LD(Y)

#define BIN(_bin, _op) void _bin(UINT8 value)\
{\
    LDA(A _op value);\
}\
CreateReadInstructions(_bin)\

BIN(AND, &)
BIN(ORA, | )
BIN(EOR, ^)

void BIT(UINT8 value)
{
    WriteNZFlags(A & value);
    WriteFlag(VMask, value & VMask);
    WriteFlag(NMask, value & NMask);
}
CreateReadInstructions(BIT)

UINT8 STA()
{
    return A;
}
CreateWriteInstructions(STA, =)

UINT8 STX()
{
    return X;
}
CreateWriteInstructions(STX, =)

UINT8 STY()
{
    return Y;
}
CreateWriteInstructions(STY, =)

void INC(UINT8* value)
{
    if (value != &A && value != &X && value != &Y)
    {
        UINT16 addr = (UINT16)(value - Memory);
        UINT8 result = GetImmediate8(addr) + 1;
        SetImmediate8(addr, result);
        WriteNZFlags(result);
    }
    else
    {
        (*value)++;
        WriteNZFlags(*value);
    }
}
CreateRefInstructions(INC)

void DEC(UINT8* value)
{
    if (value != &A && value != &X && value != &Y)
    {
        UINT16 addr = (UINT16)(value - Memory);
        UINT8 result = GetImmediate8(addr) - 1;
        SetImmediate8(addr, result);
        WriteNZFlags(result);
    }
    else
    {
        (*value)--;
        WriteNZFlags(*value);
    }
}
CreateRefInstructions(DEC)

void ADC(int value)
{
    UINT16 result = A + value + CheckCarry;

    UINT8 sign1 = value & 0x80;
    UINT8 sign2 = A & 0x80;
    UINT8 sign3 = result & 0x80;

    LDA((UINT8)result);

    // Carry occurs when result exceeds data range (255).
    int carry = result >= 0x100;

    // Overflow occurs when signs of input match each other but don't match result.
    int overflow = (sign1 == sign2) & (sign1 != sign3);

    WriteFlag(CMask, carry);
    WriteFlag(VMask, overflow);
}
CreateReadInstructions(ADC)

void SBC(int value)
{
    UINT16 result = A - value - (1 - CheckCarry);

    UINT8 sign1 = -value & 0x80;
    UINT8 sign2 = A & 0x80;
    UINT8 sign3 = result & 0x80;

    LDA((UINT8)result);

    // Carry occurs when result exceeds data range (255).
    int carry = result < 0x100;

    // Overflow occurs when signs of input match each other but don't match result.
    int overflow = (sign1 == sign2) & (sign1 != sign3);

    WriteFlag(CMask, carry);
    WriteFlag(VMask, overflow);
}
CreateReadInstructions(SBC)

void Compare(UINT8 left, UINT8 right)
{
    WriteFlag(CMask, left >= right);
    WriteFlag(ZMask, left == right);
    WriteFlag(NMask, (left - right) & 0x80);
}

void CMP(UINT8 value)
{
    Compare(A, value);
}
CreateReadInstructions(CMP)

void CPX(int value)
{
    Compare(X, value);
}
CreateReadInstructions(CPX)

void CPY(int value)
{
    Compare(Y, value);
}
CreateReadInstructions(CPY)

void Branch(int test)
{
    if (test)
    {
        cyclesRemaining--;
        elapsedTicks++;

        // Don't forget the INT8 cast for signed branching.
        int result = PC + (INT8)Memory[PC] + 1;

        // Branch crossed page.
        if ((result & 0x100) != (PC ^ 0x100))
        {
            cyclesRemaining -= 2;
            elapsedTicks += 2;
        }

        PC = result;
    }
    else
        PC++;
}

void BCC()
{
    Branch(!CheckCarry);
}

void BCS()
{
    Branch(CheckCarry);
}

void BNE()
{
    Branch(!CheckFlag(ZMask));
}

void BEQ()
{
    Branch(CheckFlag(ZMask));
}

void BMI()
{
    Branch(CheckFlag(NMask));
}

void BPL()
{
    Branch(!CheckFlag(NMask));
}

void BVC()
{
    Branch(!CheckFlag(VMask));
}

void BVS()
{
    Branch(CheckFlag(VMask));
}

void CLC()
{
    ClearFlag(CMask);
}

void CLD()
{
    ClearFlag(DMask);
}

void CLI()
{
    ClearFlag(IMask);
    if (IrqPending)
    {
        m6502zpint();
    }
}

void CLV()
{
    ClearFlag(VMask);
}

void SEC()
{
    SetFlag(CMask);
}

void SED()
{
    SetFlag(DMask);
}

void SEI()
{
    SetFlag(IMask);
}

void ASL(UINT8* value)
{
    WriteFlag(CMask, *value & 0x80);

    if (value != &A && value != &X && value != &Y)
    {
        UINT16 addr = (UINT16)(value - Memory);
        UINT8 result = GetImmediate8(addr) << 1;
        SetImmediate8(addr, result);
        WriteNZFlags(result);
    }
    else
    {
        (*value) <<= 1;
        WriteNZFlags(*value);
    }
}
CreateRefInstructions(ASL)

void LSR(UINT8 *value)
{
    WriteFlag(CMask, *value & 1);

    if (value != &A && value != &X && value != &Y)
    {
        UINT16 addr = (UINT16)(value - Memory);
        UINT8 result = GetImmediate8(addr) >> 1;
        SetImmediate8(addr, result);
        WriteNZFlags(result);
    }
    else
    {
        (*value) >>= 1;
        WriteNZFlags(*value);
    }
}
CreateRefInstructions(LSR)

void ROL(UINT8 *value)
{
    UINT8 carry = CheckCarry;

    WriteFlag(CMask, *value & 0x80);

    if (value != &A && value != &X && value != &Y)
    {
        UINT16 addr = (UINT16)(value - Memory);
        UINT8 result = GetImmediate8(addr);
        result <<= 1;
        result |= carry;
        SetImmediate8(addr, result);
        WriteNZFlags(result);
    }
    else
    {
        (*value) <<= 1;
        (*value) |= carry;
        WriteNZFlags(*value);
    }
}
CreateRefInstructions(ROL)

void ROR(UINT8* value)
{
    UINT8 carry = CheckCarry;

    WriteFlag(CMask, *value & 1);

    if (value != &A && value != &X && value != &Y)
    {
        UINT16 addr = (UINT16)(value - Memory);
        UINT8 result = GetImmediate8(addr);
        result >>= 1;
        result |= carry << 7;
        SetImmediate8(addr, result);
        WriteNZFlags(result);
    }
    else
    {
        (*value) >>= 1;
        (*value) |= carry << 7;
        WriteNZFlags(*value);
    }
}
CreateRefInstructions(ROR)

void PHA()
{
    Push(A);
}

void PHP()
{
    Push(Flags);
}

void PLA()
{
    LDA(Pull());
}

void PLP()
{
    Flags = Pull();
}

void RTI()
{
    Flags = Pull();
    Flags |= 0x20;
    PC = PullWord();

    if (!CheckFlag(IMask) && IrqPending)
    {
        m6502zpint();
    }
}

void JMP(int value)
{
    // If (byte)value == 0xFF, the behavior is undefined.
    PC = value;
}
void JMPAbsolute()
{
    JMP(GetImmediate16(PC));
}

void JMPIndirect()
{
    JMP(GetImmediate16(GetImmediate16(PC)));
}

void JSR(int value)
{
    PushWord(PC + 1);
    JMP(value);
}
void JSRAbsolute()
{
    JSR(GetImmediate16(PC));
}

void RTS()
{
    PC = PullWord();
    PC++;
}

#define TNM(_n, _m)\
void T##_n##_m()\
{\
    LDN(&_m, _n);\
}

TNM(A, X)
TNM(A, Y)
TNM(S, X)
TNM(X, A)
TNM(Y, A)

void TXS()
{
    // This opcode doesn't affect the state flags.
    S = X;
}

void NOP()
{
    // Do nothing. Program counter and cycle count are updated in main loop.
}

UINT16 GetWord(UINT16 addr)
{
    return Memory[addr] | (Memory[(UINT16)(addr + 1)] << 8);
}

void m6502zpreset()
{
    X = 0;
    Y = 0;
    IrqPending = 0;
    S = 0xFF;
    AF = 0x2200;

    // Set PC to RESET vector.
    PC = GetWord(ResetVector);
}

void ClearInterrupt()
{
    // Remove interrupt bit.
    Flags &= 0xEF;
    Flags |= 0x24;
}

UINT32 m6502zpint()
{
    // Are interrupts disabled?
    if (CheckFlag(IMask))
    {
        // Indicate we have a pending IRQ.
        return IrqPending = 1;
    }

    PushState();
    ClearInterrupt();

    // Set PC to Interrupt vector.
    PC = GetWord(InterruptVector);

    // Indicate we've taken the interrupt.
    return IrqPending = 0;
}

UINT32 m6502zpnmi()
{
    PushState();
    ClearInterrupt();

    // Set PC to NMI vector.
    PC = GetWord(NmiVector);

    // Indicate that we took the NMI.
    return 0;
}

UINT32 m6502zpexec(UINT32 cycles)
{
    cyclesRemaining = cycles;

    do
    {
        if (ExecuteInstruction() != M6502_STATUS_OK)
            return PC;
    }
    while (cyclesRemaining > 0);

    // Return success
    return M6502_STATUS_OK;
}

int ExecuteInstruction()
{
    // Get opcode instruction.
    Instruction instruction = Instructions[Op];

    // Check for invalid instruction (NULL)
    if (!instruction)
    {
        // Return address of instruction (not successful run).
        return PC;
    }

    // Update cycle count for this operation.
    UINT8 ticks = CycleCost[Op];
    cyclesRemaining -= ticks;
    elapsedTicks += ticks;

    // Update PC to next byte before executing instruction.
    PC++;

    // Perform opcode instruction.
    instruction();

    return M6502_STATUS_OK;
}

static const UINT8 OpSize[0x100] =
{
    0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 2, 2, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0,
    2, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0,
    0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0,
    0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0,
    0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 0, 2, 0, 0,
    1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 1, 1, 1, 0, 0, 2, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0,
    1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 2, 2, 2, 0,
    1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 0, 0, 0, 2, 2, 0,
};

static const UINT8 CycleCost[0x100] =
{
    7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 1, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
    2, 8, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
    2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
    2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
};

static const Instruction Instructions[0x100] =
{
    BRK,// $00
    ORAIndirectX,// $01
    NULL,// $02
    NULL,// $03
    NULL,// $04
    ORAZeroPage,// $05
    ASLZeroPage,// $06
    NULL,// $07
    PHP,// $08
    ORAImmediate,// $09
    ASLA,// $0A
    NULL,// $0B
    NULL,// $0C
    ORAAbsolute,// $0D
    ASLAbsolute,// $0E
    NULL,// $0F

    BPL,// $10
    ORAIndirectY,// $11
    NULL,// $12
    NULL,// $13
    NULL,// $14
    ORAZeroPageX,// $15
    ASLZeroPageX,// $16
    NULL,// $17
    CLC,// $18
    ORAAbsoluteY,// $19
    NULL,// $1A
    NULL,// $1B
    NULL,// $1C
    ORAAbsoluteX,// $1D
    ASLAbsoluteX,// $1E
    NULL,// $1F

    JSRAbsolute,// $20
    ANDIndirectX,// $21
    NULL,// $22
    NULL,// $23
    BITZeroPage,// $24
    ANDZeroPage,// $25
    ROLZeroPage,// $26
    NULL,// $27
    PLP,// $28
    ANDImmediate,// $29
    ROLA,// $2A
    NULL,// $2B
    BITAbsolute,// $2C
    ANDAbsolute,// $2D
    ROLAbsolute,// $2E
    NULL,// $2F

    BMI,// $30
    ANDIndirectY,// $31
    NULL,// $32
    NULL,// $33
    NULL,// $34
    ANDZeroPageX,// $35
    ROLZeroPageX,// $36
    NULL,// $37
    SEC,// $38
    ANDAbsoluteY,// $39
    NULL,// $3A
    NULL,// $3B
    NULL,// $3C
    ANDAbsoluteX,// $3D
    ROLAbsoluteX,// $3E
    NULL,// $3F

    RTI,// $40
    EORIndirectX,// $41
    NULL,// $42
    NULL,// $43
    NULL,// $44
    EORZeroPage,// $45
    LSRZeroPage,// $46
    NULL,// $47
    PHA,// $48
    EORImmediate,// $49
    LSRA,// $4A
    NULL,// $4B
    JMPAbsolute,// $4C
    EORAbsolute,// $4D
    LSRAbsolute,// $4E
    NULL,// $4F

    BVC,// $50
    EORIndirectY,// $51
    NULL,// $52
    NULL,// $53
    NULL,// $54
    EORZeroPageX,// $55
    LSRZeroPageX,// $56
    NULL,// $57
    CLI,// $58
    EORAbsoluteY,// $59
    NULL,// $5A
    NULL,// $5B
    NULL,// $5C
    EORAbsoluteX,// $5D
    LSRAbsoluteX,// $5E
    NULL,// $5F

    RTS,// $60
    ADCIndirectX,// $61
    NULL,// $62
    NULL,// $63
    NULL,// $64
    ADCZeroPage,// $65
    RORZeroPage,// $66
    NULL,// $67
    PLA,// $68
    ADCImmediate,// $69
    RORA,// $6A
    NULL,// $6B
    JMPIndirect,// $6C
    ADCAbsolute,// $6D
    RORAbsolute,// $6E
    NULL,// $6F

    BVS,// $70
    ADCIndirectY,// $71
    NULL,// $72
    NULL,// $73
    NULL,// $74
    ADCZeroPageX,// $75
    RORZeroPageX,// $76
    NULL,// $77
    SEI,// $78
    ADCAbsoluteY,// $79
    NULL,// $7A
    NULL,// $7B
    NULL,// $7C
    ADCAbsoluteX,// $7D
    RORAbsoluteX,// $7E
    NULL,// $7F

    NULL,// $80
    STAIndirectX,// $81
    NULL,// $82
    NULL,// $83
    STYZeroPage,// $84
    STAZeroPage,// $85
    STXZeroPage,// $86
    NULL,// $87
    DECY,// $88
    NULL,// $89
    TXA,// $8A
    NULL,// $8B
    STYAbsolute,// $8C
    STAAbsolute,// $8D
    STXAbsolute,// $8E
    NULL,// $8F

    BCC,// $90
    STAIndirectY,// $91
    NULL,// $92
    NULL,// $93
    STYZeroPageX,// $94
    STAZeroPageX,// $95
    STXZeroPageY,// $96
    NULL,// $97
    TYA,// $98
    STAAbsoluteY,// $99
    TXS,// $9A
    NULL,// $9B
    NULL,// $9C
    STAAbsoluteX,// $9D
    NULL,// $9E
    NULL,// $9F

    LDYImmediate,// $A0
    LDAIndirectX,// $A1
    LDXImmediate,// $A2
    NULL,// $A3
    LDYZeroPage,// $A4
    LDAZeroPage,// $A5
    LDXZeroPage,// $A6
    NULL,// $A7
    TAY,// $A8
    LDAImmediate,// $A9
    TAX,// $AA
    NULL,// $AB
    LDYAbsolute,// $AC
    LDAAbsolute,// $AD
    LDXAbsolute,// $AE
    NULL,// $AF

    BCS,// $B0
    LDAIndirectY,// $B1
    NULL,// $B2
    NULL,// $B3
    LDYZeroPageX,// $B4
    LDAZeroPageX,// $B5
    LDXZeroPageY,// $B6
    NULL,// $B7
    CLV,// $B8
    LDAAbsoluteY,// $B9
    TSX,// $BA
    NULL,// $BB
    LDYAbsoluteX,// $BC
    LDAAbsoluteX,// $BD
    LDXAbsoluteY,// $BE
    NULL,// $BF

    CPYImmediate,// $C0
    CMPIndirectX,// $C1
    NULL,// $C2
    NULL,// $C3
    CPYZeroPage,// $C4
    CMPZeroPage,// $C5
    DECZeroPage,// $C6
    NULL,// $C7
    INCY,// $C8
    CMPImmediate,// $C9
    DECX,// $CA
    NULL,// $CB
    CPYAbsolute,// $CC
    CMPAbsolute,// $CD
    DECAbsolute,// $CE
    NULL,// $CF

    BNE,// $D0
    CMPIndirectY,// $D1
    NULL,// $D2
    NULL,// $D3
    NULL,// $D4
    CMPZeroPageX,// $D5
    DECZeroPageX,// $D6
    NULL,// $D7
    CLD,// $D8
    CMPAbsoluteY,// $D9
    NULL,// $DA
    NULL,// $DB
    NULL,// $DC
    CMPAbsoluteX,// $DD
    DECAbsoluteX,// $DE
    NULL,// $DF

    CPXImmediate,// $E0
    SBCIndirectX,// $E1
    NULL,// $E2
    NULL,// $E3
    CPXZeroPage,// $E4
    SBCZeroPage,// $E5
    INCZeroPage,// $E6
    NULL,// $E7
    INCX,// $E8
    SBCImmediate,// $E9
    NOP,// $EA
    NULL,// $EB
    CPXAbsolute,// $EC
    SBCAbsolute,// $ED
    INCAbsolute,// $EE
    NULL,// $EF

    BEQ,// $F0
    SBCIndirectY,// $F1
    NULL,// $F2
    NULL,// $F3
    NULL,// $F4
    SBCZeroPageX,// $F5
    INCZeroPageX,// $F6
    NULL,// $F7
    SED,// $F8
    SBCAbsoluteY,// $F9
    NULL,// $FA
    NULL,// $FB
    NULL,// $FC
    SBCAbsoluteX,// $FD
    INCAbsoluteX,// $FE
    NULL,// $FF
};
