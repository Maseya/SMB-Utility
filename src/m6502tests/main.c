#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "M6502.h"

extern CONTEXTM6502 context;

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

#define CheckFlag(_i) (Flags & (_i))

static const UINT8 LdaImm8[] =
{
    // A = 0x00, Z = 1, N = 0
    0xa9, 0x00,

    // A = 0x00, Z = 0, N = 0
    0xa9, 0x01,

    // A = 0x80, Z = 0, N = 1
    0xa9, 0x80,

    // A = 0xFF, Z = 0, N = 1
    0xa9, 0xff,

    // A = 0x00, Z = 1, N = 0
    0xa9, 0x00
};

void Reset()
{
    A = 0;
    X = 0;
    Y = 0;
    Flags = 0;
    PC = 0x8000;
    S = 0xFF;
}

int status;

int TestLda(UINT8 expectedA, UINT8 expectedZ, UINT8 expectedN)
{
    ExecuteInstruction();
    printf("Expected: A = 0x%02x, Z = %d, N = %d\n", expectedA, expectedZ, expectedN);
    UINT8 realZ = CheckFlag(ZMask) >> ZBit;
    UINT8 realN = CheckFlag(NMask) >> NBit;
    printf("Result:   A = 0x%02x, Z = %d, N = %d\n", A, realZ, realN);
    return A != expectedA || realZ != expectedZ || realN != expectedN;
}

void TestLdaImm()
{
    Reset();
    memcpy(Memory + PC, LdaImm8, sizeof(LdaImm8));

    printf("Checking LDA #$00\n");
    if (TestLda(0, 1, 0))
    {
        printf("unsuccessful...\n\n");
        status++;
    }
    else
    {
        printf("success!\n\n");
    }
}

int main()
{
    context.m6502Base = malloc(0x10000);
    TestLdaImm();
    free(context.m6502Base);

    if (status)
    {
        printf("%d opcode failed.\n", status);
    }
    else
    {
        printf("All opcode tests passed!\n");
    }

    return status;
}
