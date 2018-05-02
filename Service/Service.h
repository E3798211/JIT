#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED

#include <iostream>
#include <string>
#include <ctype.h>
#include <assert.h>

/// Reads whole file
/**
    \return nullptr in case of fault
*/
char* FileRead(std::string filename) noexcept;

/// Counts words in file
size_t CountWords(std::string file_content) noexcept;

/// Skips spaces
char* SkipSpaces(char** str);

/// Compares char with every char in string
bool IsOneOf(const char to_compare, const char* with);

/// Gets word, ended with *symbols
std::string GetWordExceptSymbols(char** str, char* symbols);

/// Gets word
/**
    Sets pointer to the end of the word
*/
std::string GetWord(char** str);

#endif // SERVICE_H_INCLUDED
