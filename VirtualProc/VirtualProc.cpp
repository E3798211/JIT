#include "VirtualProc.h"

int VirtualProc::LoadFile(const std::string& filename)
{
    char* file_content = FileRead(filename);
    if(!file_content)
    {
        PROC_DBG std::cout << "Failed loading programm file\n";
        return -PROC_ERR::LOAD_ERR;
    }

    size_t prog_len = CountWords(file_content);

    /*
     *  Two first words are version and signature. Can be added lately.
     */

    if(prog_len >= MAX_PROGRAM_SIZE)
    {
        PROC_DBG std::cout << "Programm size is too big. Cannot be loaded\n";
        delete [] file_content;
        return -PROC_ERR::LOAD_ERR;
    }

    char* file_content_iterator = file_content;
    for(size_t i = 0; i < prog_len; i++)
        program_[i] = strtol(file_content_iterator, &file_content_iterator, 10);

    delete [] file_content;
    return PROC_ERR::OK;
}

int VirtualProc::Exec()
{
    switch (program_[registers_[IP]])
    {
        //
    }

    return PROC_ERR::OK;
}


