// M6502 Emulator

#ifndef _M6502_H_
#define _M6502_H_

// TODO(swr): This is BAD. But the data types are interfering with Windows integral
// types, so this is the only solution for now. I'll have to fix the typing when I get
// time.
#include <Windows.h>

#define M6502_STATUS_OK 0x80000000

UINT32 m6502zpnmi(void);
UINT32 m6502zpint();
UINT32 m6502zpexec(UINT32);
int ExecuteInstruction();
void m6502zpreset(void);
UINT32 m6502zpGetElapsedTicks(UINT32);

#ifndef _MEMORYREADWRITEBYTE_
#define _MEMORYREADWRITEBYTE_

struct MemoryWriteByte {
    UINT32 lowAddr;
    UINT32 highAddr;
    void (*memoryCall)(UINT32, UINT8, struct MemoryWriteByte*);
    void* pUserArea;
};

struct MemoryReadByte {
    UINT32 lowAddr;
    UINT32 highAddr;
    UINT8 (*memoryCall)(UINT32, struct MemoryReadByte*);
    void* pUserArea;
};

#endif  // _MEMORYREADWRITEBYTE_

struct m6502context {
    UINT8* m6502Base;

    struct MemoryReadByte* m6502MemoryRead;
    struct MemoryWriteByte* m6502MemoryWrite;

    union {
        UINT16 m6502af; /* A & Flags */
        struct {
            UINT8 A;
            UINT8 Flags;
        };
    };
    union {
        UINT16 m6502pc; /* Program counter */
        struct {
            UINT8 PCL;
            UINT8 PCH;
        };
    };
    UINT8 m6502x; /* X register */
    UINT8 m6502y; /* Y register */
    UINT8 m6502s; /* S register */
    UINT8 IrqPending;
};

typedef struct m6502context CONTEXTM6502;
typedef struct m6502bscontext CONTEXTM6502BS;

void m6502zpGetContext(CONTEXTM6502* context);
void m6502zpSetContext(CONTEXTM6502* source);
int m6502zppc();

#endif  // _M6502_H_
