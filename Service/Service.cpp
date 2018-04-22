#include "Service.h"

char* FileRead(const std::string& filename) noexcept
{
    FILE* input = fopen(filename.c_str(), "r");
    if(!input)
    {
        return nullptr;
    }

    if(fseek(input, 0, SEEK_END))
    {
        return nullptr;
    }

    int filesize = ftell(input);
    if(filesize == EOF)
    {
        return nullptr;
    }

    char* file_content = nullptr;
    try
    {
        file_content = new char [filesize];
    }
    catch(const std::bad_alloc& ex)
    {
        return nullptr;
    }

    rewind(input);
    fread(file_content, 1, filesize, input);

    fclose(input);
    return file_content;
}

size_t CountWords(std::string file_content)
{
    std::string::iterator it = file_content.begin();
    while(isspace(*it) && it != file_content.end())     it++;       // Skipping spaces

    size_t n_words = 0;
    for( ; it != file_content.end(); it++)
        if(isspace(*it))        n_words++;

    return n_words;
}


