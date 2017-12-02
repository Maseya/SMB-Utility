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

UINT32 cyclesRemaining;
UINT32 elapsedTicks;
UINT32 status;
UINT8 AltFlags;

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

#define CMask (1<<0)
#define ZMask (1<<1)
#define IMask (1<<2)
#define DMask (1<<3)
#define BMask (1<<4)
#define VMask (1<<6)
#define NMask (1<<7)

#define GetWord(_addr) \
(Memory[_addr] | Memory[(UINT16)((_addr)+1)])

#define Immediate(_value) ((UINT8)(_value))

#define ZeroPage(_value) (Memory[Immediate(_value)])
#define ZeroPageX(_value) (ZeroPage((_value)+X))
#define ZeroPageY(_value) (ZeroPage((_value)+Y))

#define Absolute(_value) (Memory[(UINT16)(_value)])
#define AbsoluteX(_value) (Absolute(Wrap((_value)+X)))
#define AbsoluteY(_value) (Absolute(Wrap((_value)+Y)))

#define Indirect(_value) (Absolute(ZeroPage(_value)))
#define IndexedIndirect(_value) (Indirect((_value)+X))
#define IndirectIndexed(_value) (AbsoluteY(Indirect(_value)))

#define SetFlag(_i) Flags |= (_i)
#define ClearFlag(_i) Flags &= ~(_i)
#define CheckFlag(_i) (Flags & (_i))
#define WriteFlag(_i, _value) if (_value)SetFlag(_i);else ClearFlag(_i)

#define SetCarry SetFlag(0)
#define ClearCarry ClearFlag(0)
#define CheckCarry CheckFlag(0)

#define CreateReadInstructions(_name) \
\
void _name##Immediate()\
{\
    _name(Immediate(Op));\
    PC++;\
}\
\
void _name##ZeroPage()\
{\
    _name(ZeroPage(Op));\
    PC++;\
}\
\
void _name##ZeroPageX()\
{\
    _name(ZeroPageX(Op));\
    PC++;\
}\
\
void _name##ZeroPageY()\
{\
    _name(ZeroPageY(Op));\
    PC++;\
}\
\
void _name##Absolute()\
{\
    _name(Absolute(Op));\
    PC += 2;\
}\
\
void _name##AbsoluteX()\
{\
    _name(AbsoluteX(Op));\
    PC += 2;\
}\
\
void _name##AbsoluteY()\
{\
    _name(AbsoluteY(Op));\
    PC += 2;\
}\
\
void _name##Indirect()\
{\
    _name(Indirect(Op));\
    PC++;\
}\
\
void _name##IndexedIndirect()\
{\
    _name(IndexedIndirect(Op));\
    PC++;\
}\
\
void _name##IndirectIndexed()\
{\
    _name(IndirectIndexed(Op));\
    PC++;\
}

#define CreateWriteInstructions(_name, _op) \
\
void _name##ZeroPage()\
{\
    ZeroPage(Op) _op _name();\
    PC++;\
}\
\
void _name##ZeroPageX()\
{\
    ZeroPageX(Op) _op _name();\
    PC++;\
}\
\
void _name##ZeroPageY()\
{\
    ZeroPageY(Op) _op _name();\
    PC++;\
}\
\
void _name##Absolute()\
{\
    Absolute(Op) _op _name();\
    PC += 2;\
}\
\
void _name##AbsoluteX()\
{\
    AbsoluteX(Op) _op _name();\
    PC += 2;\
}\
\
void _name##AbsoluteY()\
{\
    AbsoluteY(Op) _op _name();\
    PC += 2;\
}\
\
void _name##Indirect()\
{\
    Indirect(Op) _op _name();\
    PC++;\
}\
\
void _name##IndexedIndirect()\
{\
    IndexedIndirect(Op) _op _name();\
    PC++;\
}\
\
void _name##IndirectIndexed()\
{\
    IndirectIndexed(Op) _op _name();\
    PC++;\
}

#define CreateRefInstructions(_name) \
\
void _name##A()\
{\
    _name(&A);\
}\
\
void _name##X()\
{\
    _name(&X);\
}\
\
void _name##Y()\
{\
    _name(&Y);\
}\
\
void _name##ZeroPage()\
{\
    _name(&ZeroPage(Op));\
    PC++;\
}\
\
void _name##ZeroPageX()\
{\
    _name(&ZeroPageX(Op));\
    PC++;\
}\
\
void _name##ZeroPageY()\
{\
    _name(&ZeroPageY(Op));\
    PC++;\
}\
\
void _name##Absolute()\
{\
    _name(&Absolute(Op));\
    PC += 2;\
}\
\
void _name##AbsoluteX()\
{\
    _name(&AbsoluteX(Op));\
    PC += 2;\
}\
\
void _name##AbsoluteY()\
{\
    _name(&AbsoluteY(Op));\
    PC += 2;\
}\
\
void _name##Indirect()\
{\
    _name(&Indirect(Op));\
    PC++;\
}\
\
void _name##IndexedIndirect()\
{\
    _name(&IndexedIndirect(Op));\
    PC++;\
}\
\
void _name##IndirectIndexed()\
{\
    _name(&IndirectIndexed(Op));\
    PC++;\
}

static const UINT8 M2X[0x100];
static const UINT8 X2M[0x100];
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
    Push((UINT8)value);
    Push((UINT8)(value >> 8));
}

void PushState()
{
    PushWord(PC);
    Push(Flags);
}

UINT8 Pull()
{
    return Memory[0x100 | S++];
}

UINT16 PullWord()
{
    return (Pull() << 8) | Pull();
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

#define LD(_n) void LD##_n(UINT8 value)\
{\
    _n = value;\
    WriteNZFlags(value);\
}\
CreateReadInstructions(LD##_n)\

LD(A)
LD(X)
LD(Y)
LD(S)

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
    (*value)++;
    WriteNZFlags(*value);
}
CreateRefInstructions(INC)

void DEC(UINT8* value)
{
    (*value)--;
    WriteNZFlags(*value);
}
CreateRefInstructions(DEC)

void Add(UINT8 value)
{
    int result = value + A;

    int sign1 = value & 0x80;
    int sign2 = A & 0x80;
    int sign3 = result & 0x80;

    LDA((UINT8)result);

    // Carry occurs when result exceeds data range (255).
    int carry = result > A;

    // Overflow occurs when signs of input match each other but don't match result.
    int overflow = (sign1 == sign2) & (sign1 != sign3);

    WriteFlag(CMask, carry);
    WriteFlag(VMask, overflow);
}

void ADC(int value)
{
    Add(value + CheckCarry);
}
CreateReadInstructions(ADC)

void SBC(int value)
{
    Add(-value - (1 - CheckCarry));
}
CreateReadInstructions(SBC)

void Compare(UINT8 left, UINT8 right)
{
    int result = left - right;
    WriteFlag(CMask, left >= right);
    WriteFlag(ZMask, result);
    WriteFlag(NMask, result & 0x80);
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

        int result = PC + (INT8)Op + 1;

        // Branch crossed page.
        if ((result & 0x100) ^ (PC ^ 0x100))
        {
            cyclesRemaining -= 2;
            elapsedTicks += 2;
        }

        PC = result;
    }
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
    WriteFlag(VMask, *value & 0x40);

    *value <<= 1;

    WriteNZFlags(*value);
}
CreateRefInstructions(ASL)

void LSR(UINT8 *value)
{
    WriteFlag(CMask, *value & 1);
    ClearFlag(NMask);

    *value >>= 1;

    WriteFlag(ZMask, !*value);
}
CreateRefInstructions(LSR)

void ROL(UINT8 *value)
{
    UINT8 carry = CheckCarry;
    ASL(value);

    *value |= carry;
}
CreateRefInstructions(ROL)

void ROR(UINT8* value)
{
    UINT8 carry = CheckCarry;
    LSR(value);

    if (carry)
    {
        *value |= 0x80;
        SetFlag(NMask);
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
    PC = PullWord();
}

void JMP(int value)
{
    // If (byte)value == 0xFF, the behavior is undefined.
    PC = value;
}
CreateReadInstructions(JMP)

void JSR(int value)
{
    PushWord(PC);
    JMP(value);
}
CreateReadInstructions(JSR)

void RTS()
{
    PC = PullWord();
    PC++;
}

#define TNM(_n, _m)\
void T##_n##_m()\
{\
    int dummy = _n;\
    LD##_n(_m);\
    _m = _n;\
}

TNM(A, X)
TNM(A, Y)
TNM(S, X)
TNM(X, A)
TNM(X, S)
TNM(Y, A)

void NOP()
{
    // Do nothing. Program counter and cycle count are updated in main loop.
}

void m6502zpreset()
{
    X = 0;
    Y = 0;
    IrqPending = 0;
    S = 0xFF;
    AF = 0x2200;

    // Set PC to RESET vector.
    PC = Memory[ResetVector];
}

void ClearInterrupt()
{
    // Remove interrupt bit.
    Flags &= 0xEF;
    Flags |= 0x24;
}

UINT32 m6502zpint(UINT32 cycles)
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
    PC = Memory[InterruptVector];

    // Indicate we've taken the interrupt.
    return IrqPending = 0;
}

UINT32 m6502zpnmi()
{
    PushState();
    ClearInterrupt();

    // Set PC to NMI vector.
    PC = Memory[NmiVector];

    // Indicate that we took the NMI.
    return 0;
}

UINT32 m6502zpexec(UINT32 cycles)
{
    cyclesRemaining = cycles;

    AltFlags = Flags & 0x3C;
    Flags = M2X[Flags];

    UINT8 ticks;

    do
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
        ticks = CycleCost[Op];
        cyclesRemaining -= ticks;
        elapsedTicks += ticks;

        // Update PC to next byte before executing instruction.
        PC++;

        // Perform opcode instruction.
        instruction();
    }
    while (cyclesRemaining > 0);

    // Return success
    return M6502_STATUS_OK;
}

#undef I
#undef O
#undef X
#undef Y
#undef IrqPending
#undef S
#undef AF
#undef AFL
#undef AFH
#undef PC
#undef PCL
#undef PCH
#undef Base
#undef SetPC

static const UINT8 M2X[0x100] =
{
    0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43, 0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43,
    0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43, 0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43,
    0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43, 0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43,
    0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43, 0x00, 0x01, 0x40, 0x41, 0x02, 0x03, 0x42, 0x43,
    0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53, 0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53,
    0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53, 0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53,
    0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53, 0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53,
    0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53, 0x10, 0x11, 0x50, 0x51, 0x12, 0x13, 0x52, 0x53,
    0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3, 0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3,
    0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3, 0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3,
    0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3, 0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3,
    0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3, 0x80, 0x81, 0xc0, 0xc1, 0x82, 0x83, 0xc2, 0xc3,
    0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3, 0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3,
    0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3, 0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3,
    0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3, 0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3,
    0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3, 0x90, 0x91, 0xd0, 0xd1, 0x92, 0x93, 0xd2, 0xd3,
};

static const UINT8 X2M[0x100] =
{
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41, 0x40, 0x41,
    0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03,
    0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43,
    0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03,
    0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43, 0x42, 0x43,
    0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81,
    0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1,
    0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81, 0x80, 0x81,
    0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1, 0xc0, 0xc1,
    0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83,
    0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3,
    0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83, 0x82, 0x83,
    0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3, 0xc2, 0xc3,
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
    ORAIndirectIndexed,// $01
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
    ORAIndexedIndirect,// $11
    NULL,// $12
    NULL,// $13
    NULL,// $14
    ORAZeroPageX,// $15
    NULL,// $16
    NULL,// $17
    CLC,// $18
    ORAAbsoluteY,// $19
    NULL,// $1A
    NULL,// $1B
    NULL,// $1C
    ORAAbsoluteX,// $1D
    NULL,// $1E
    NULL,// $1F

    JSRAbsolute,// $20
    ANDIndirectIndexed,// $21
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
    ANDIndexedIndirect,// $31
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
    EORIndirectIndexed,// $41
    NULL,// $42
    NULL,// $43
    NULL,// $44
    EORZeroPage,// $45
    LSRZeroPage,// $46
    NULL,// $47
    PHA,// $48
    EORImmediate,// $49
    LSRAbsolute,// $4A
    NULL,// $4B
    JMPAbsolute,// $4C
    EORAbsolute,// $4D
    LSRAbsolute,// $4E
    NULL,// $4F

    BVC,// $50
    EORIndexedIndirect,// $51
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
    ADCIndirectIndexed,// $61
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
    ADCIndexedIndirect,// $71
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
    STAIndirectIndexed,// $81
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
    STAIndexedIndirect,// $91
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
    LDAIndirectIndexed,// $A1
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
    LDAIndexedIndirect,// $B1
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
    CMPIndirectIndexed,// $C1
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
    CMPIndexedIndirect,// $D1
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
    SBCIndirectIndexed,// $E1
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
    SBCIndexedIndirect,// $F1
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
