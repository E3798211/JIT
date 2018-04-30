#ifndef RECDESCEND_H_INCLUDED
#define RECDESCEND_H_INCLUDED

#include <string>
#include <assert.h>
#include "../Service/Service.h"
#include "../Tree/Tree.h"
#include "Syntax.h"


// #define TOK_DBG_
#ifdef TOK_DBG_
    #define TOK_DBG
#else
    #define TOK_DBG if(0)
#endif // TOK_DBG_


class Token
{
private:

    /// Name of the token
    std::string name_;

    /// Type
    int type_   = 0;

    /// Value (for variables)
    int value_  = 0;

public:

    /// Default constructor
    Token()
    {
        TOK_DBG std::cout << "Default token constructor\n";
    }

    /// Constructor with parameters
    Token(const std::string& name, int type, int value = 0):
        name_   (name),
        type_   (type),
        value_  (value)
    {
        TOK_DBG std::cout << "Token constructor with parameters\n";
    }

    /// Destructor
    ~Token()
    {
        TOK_DBG std::cout << "Token destructor\n";
    }

    // =========================================
    // Getters

    const std::string& Name()   const
    {
        return name_;
    }

    int Type()                  const
    {
        return type_;
    }

    int Value()                 const
    {
        return value_;
    }

    // Setters

    std::string& Name(const std::string& name)
    {
        name_ = name;
        return name_;
    }

    int Type(int type)
    {
        type_ = type;
        return type_;
    }

    int Value(int value)
    {
        value_ = value;
        return value_;
    }
};

/// Loads programm
/**
    Inits 'cur_pos'
*/
int LoadProgramm(std::string filename);

/// Finds line with error
/**
    \warning    Expects global 'cur_pos' ititialized
*/
int ErrorLine();

/// Breaks whole programm into tokens and fills array with them
/**
    \warning    Expects global 'cur_pos' ititialized
*/
int BuildSyntaxTree();                                                  // <-- Tree has to be killed

/// Gets N
Tree<Token>* GetN();

/// Gets P
Tree<Token>* GetP();

/// Gets T
Tree<Token>* GetT();

/// Gets E
Tree<Token>* GetE();

#endif // RECDESCEND_H_INCLUDED
