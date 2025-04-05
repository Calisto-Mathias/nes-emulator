#ifndef CPU_H
#define CPU_H
#include "CPUOpcodes.h"
#include "MainBus.h"

namespace sn
{
    class CPU
    {
    private:
        MainBus &m_bus;
        
        int m_cycles;
        int m_skipCycles;
        
        Address r_PC;
        Byte r_SP;
        Byte r_A;
        Byte r_X;
        Byte r_Y;
        
        bool f_N;
        bool f_V;
        bool f_D;
        bool f_I;
        bool f_Z;
        bool f_C;
        
        bool m_pendingNMI;
        bool m_pendingIRQ;

        void setZN(Byte value);
        void interruptSequence(InterruptType type);
        void skipPageCrossCycle(Address a, Address b);
        Byte pullStack();
        void pushStack(Byte value);
        Address readAddress(Address addr);

        bool executeType2(Byte opcode);
        bool executeType1(Byte opcode);
        bool executeType0(Byte opcode);
        bool executeBranch(Byte opcode);
        bool executeImplied(Byte opcode);
        
    public:
        CPU(MainBus &mem);
        
        void step();
        void log();
        void interrupt(InterruptType type);
        void skipDMACycles();
        void reset();
        void reset(Address start_addr);
        
        Address getPC() { return r_PC; }
    };
};
#endif // CPU_H
