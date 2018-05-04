#include "Service.h"

char* FileRead(std::string filename) noexcept
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

size_t CountWords(std::string file_content) noexcept
{
    std::string::iterator it = file_content.begin();
    while(isspace(*it) && it != file_content.end())     it++;       // Skipping spaces

    size_t n_words = 0;
    for( ; it != file_content.end(); it++)
        if(isspace(*it))        n_words++;

    return n_words;
}

char* SkipSpaces(char** str)
{
    assert(*str);
    while(isspace(**str))    (*str)++;

    return *str;
}

std::string GetWord(char** beg)
{
    assert(beg);

    std::string word;
    SkipSpaces(beg);
    while(!isspace(**beg) && (**beg) != '\0')
    {
        word += **beg;
        (*beg)++;
    }

    return word;
}

bool IsOneOf(const char to_compare, const char* with)
{
    while(*with != '\0')
    {
        if(to_compare == *with)     return true;
        with++;
    }
    return false;
}

std::string GetWordExceptSymbols(char** str, const char* symbols)
{
    assert(symbols);

    std::string word;
    while(!IsOneOf(**str, symbols) && **str != '\0')
    {
        word += **str;
        (*str)++;
    }
    return word;
}


