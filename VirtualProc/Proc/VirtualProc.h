#ifndef VIRTUALPROC_H_INCLUDED
#define VIRTUALPROC_H_INCLUDED

#include <iostream>
#include <string>

#include "../../Service/Errors.h"
#include "../../Service/Service.h"
#include "../CommonFiles/CPUProperties.h"
#include "../CommonFiles/Commands.h"

// =========================================

// #define PROC_DBG_
#ifdef PROC_DBG_
    #define PROC_DBG
#else
    #define PROC_DBG if(0)
#endif // PROC_DBG_

/**
    \warning DO NOT USE ASSIGNMENT OPERATOR AND OTHER THINGS. VIRTUAL PROC HAS TO BE THE ONLY ONE!
*/
class VirtualProc
{
private:

    /// Registers
    int registers_[N_REGS]  = {};

    /// Random access memory
    int ram_[RAM_SIZE]      = {};

    /// "Memory" for loaded programm
    int program_[MAX_PROGRAM_SIZE]  = {};

// =============================================

    /// Loads file
    /**
        \return Length of the programm
    */
    int LoadFile(std::string filename);

    /// Implements comparing in jumps
    int JumpCode(bool (*check)(int flag_value));

public:

    /// Deleting default constructor
    VirtualProc() = delete;

    /// Explicit constructor
    explicit VirtualProc(std::string exec_filename)
    {
        PROC_DBG std::cout << "Processor constructor\n";

        if(LoadFile(exec_filename))
            throw std::logic_error("Failed to init virtual processor\n");

        for(int i = 0; i < N_REGS; i++)
            registers_[i] = 0;
        // registers_[SP] = 1;         // Stack pointer points over the stack
        registers_[SP] = STACK_BEGIN;         // Stack pointer points over the stack

        program_[MAX_PROGRAM_SIZE - 1] = END;      // <-- to stop execution finally
    }

    /// Destructor
    ~VirtualProc()
    {
        PROC_DBG std::cout << "Processor destructor\n";
    }

    /// Main "loop"
    int Run()
    {
        int status = 0;
        while(program_[registers_[IP]] != END)
        {
            status = Exec();
            if(status)
            {
                std::cout << "Invalid command: couldn't execute " << registers_[IP] << "\n";
                break;
            }

            ShowInfo();
            PROC_DBG std::cout << "ip = " << registers_[IP] << "\n";
            getchar();
        }
        return status;
    }

    /// Executes next command
    /**
        \warning Exec() is responsible for IP's value
    */
    int Exec();                                                                 // <-- Not implemented

    /// Shows info about proc
    int ShowInfo();
};

#endif // VIRTUALPROC_H_INCLUDED
