#ifndef VIRTUALPROC_H_INCLUDED
#define VIRTUALPROC_H_INCLUDED

#include <iostream>
#include <string>

#include "../Service/Service.h"

// =========================================

#define PROC_DBG_
#ifdef PROC_DBG_
    #define PROC_DBG
#else
    #define PROC_DBG if(0)
#endif // PROC_DBG_

enum PROC_ERR
{
    OK,
    LOAD_ERR
};

const int PROG_END      = -1;

const size_t RAM_SIZE           = 100000;
const size_t RAM_BEGIN          =  50000;
const size_t STACK_BEGIN        =  49999;
const size_t MAX_PROGRAM_SIZE   = 100000;

enum REGS
{
    AX,
    BX,
    CX,
    DX,

    R8,
    R9,
    R10,
    R11,

    BP,
    SP,
    IP,

    N_REGS
};

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

    /// Executing file's name
    std::string filename;

// =============================================

    /// Loads file
    /**
        \return Length of the programm
    */
    int LoadFile(const std::string& filename);                                  // <-- Not implemented

public:

    /// Deleting default constructor
    VirtualProc() = delete;

    /// Explicit constructor
    explicit VirtualProc(std::string exec_file_name):
        filename    (exec_file_name)
    {
        PROC_DBG std::cout << "Processor constructor\n";

        if(LoadFile(exec_file_name))
            throw std::logic_error("Failed to init virtual processor\n");

        program_[MAX_PROGRAM_SIZE - 1] = PROG_END;      // <-- to stop execution finally
    }

    /// Destructor
    ~VirtualProc()
    {
        PROC_DBG std::cout << "Processor destructor\n";
    }

    /// Main "loop"
    int Run()
    {
        while(registers_[IP] != PROG_END)
        {
            try                             { Exec(); }
            catch(const std::exception& ex) { throw;  }
        }
    }

    /// Executes next command
    /**
        \warning Exec() is responsible for IP's value
    */
    int Exec();                                                                 // <-- Not implemented
};

#endif // VIRTUALPROC_H_INCLUDED
