#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <iostream>

#include "../Service/Errors.h"
#include "../Service/DotSyntax.h"

#define TREE_DBG_
#ifdef TREE_DBG_
    #define TREE_DBG
#else
    #define TREE_DBG if(0)
#endif // TREE_DBG_


template<typename data_T>
class Tree
{
private:

    /// Data
    data_T data_;

    /// Left branch
    Tree<data_T>* left_     =   nullptr;

    /// Right branch
    Tree<data_T>* right_    =   nullptr;

    /// Pointer to the parent
    Tree<data_T>* parent_   =   nullptr;

    /// Prints node
    template<typename... ExtraArgs>
    static int PrintTree(  Tree<data_T>* root, FILE* output,
                    int (*print_data_to_dot)(const data_T&, FILE*, ExtraArgs...),
                    ExtraArgs... to_finction);

    /// Sets connection between nodes
    static int SetConnections(Tree<data_T>* root, FILE* output);

public:

    /// Default constructor
    Tree(Tree<data_T>* parent = nullptr)  noexcept:
        parent_     (parent)
    {
        TREE_DBG std::cout << "Default tree constructor\n";
    }

    /// Constructor with branches
    Tree(Tree<data_T>* left, Tree<data_T>* right, Tree<data_T>* parent = nullptr)   noexcept:
        left_       (left),
        right_      (right),
        parent_     (parent)
    {
        TREE_DBG std::cout << "Tree constructor with branches\n";
    }

    /// Constructor with data
    Tree(const data_T& data)    noexcept:
        data_       (data)
    {
        TREE_DBG std::cout << "Tree constructor with data\n";
    }

    /// Constructor with all parameters
    Tree(const data_T& data, Tree<data_T>* left, Tree<data_T>* right, Tree<data_T>* parent = nullptr):
        left_       (left),
        right_      (right),
        parent_     (parent)
    {
        TREE_DBG std::cout << "Tree constructor with all parameters\n";
        try
        {
            data_ = data;
        }
        catch(...)
        {
            throw;
        }
    }

    /// Copy constructor
    Tree(const Tree<data_T>& that)  = delete;

    /// Move constructor deleted
    Tree(Tree<data_T>&& that)       = delete;

    /// Destructor
    ~Tree()
    {
        TREE_DBG std::cout << "\033[0;31mDefault tree destructor\033[0m\n";
        if(left_)
        {
            delete left_;
            left_ = nullptr;
        }
        if(right_)
        {
            delete right_;
            right_ = nullptr;
        }
        parent_ = nullptr;
    }

    /// Copying
    /**
        \warning src must be correct pointer to an existing object
        \return  dst or nullptr in case of fault
    */
    static Tree<data_T>* Copy(Tree<data_T>* src);

    /// Assignment operator
    Tree<data_T>& operator=(const Tree<data_T>& that)   =   delete;

    // =============================================
    // Getters

    Tree<data_T>* Parent()  const   noexcept
    {
        return parent_;
    }

    Tree<data_T>* Left()    const   noexcept
    {
        return left_;
    }

    Tree<data_T>* Right()   const   noexcept
    {
        return right_;
    }

    const data_T& Data()    const   noexcept
    {
        return data_;
    }

    // =============================================
    // Setters

    Tree<data_T>* Parent(Tree<data_T>* that)    noexcept
    {
        parent_ = that;
        return parent_;
    }

    Tree<data_T>* Left (Tree<data_T>* that)     noexcept
    {
        delete left_;
        left_ = that;
        return left_;
    }

    Tree<data_T>* Right(Tree<data_T>* that)     noexcept
    {
        delete right_;
        right_ = that;
        return right_;
    }

    Tree<data_T>* FastRigth(Tree<data_T>* that) noexcept
    {
        right_ = that;
        return right_;
    }

    Tree<data_T>* FastLeft (Tree<data_T>* that) noexcept
    {
        left_  = that;
        return  left_;
    }

    const data_T& Data(const data_T& that)
    {
        try
        {
            data_ = that;
        }
        catch(...)
        {
            throw;
        }
        return data_;
    }

    // =============================================
    // Visitors

    /// Applies function to the node, then left, then right (if they are not nullptr)
    /**
        \warning apply_func must take only one pointer to the node
    */
    template<typename... ExtraArgs>
    int PrefixVisitor(int(*apply_func)(Tree<data_T>*, ExtraArgs...), ExtraArgs... to_function);

    /// Applies function to the left, then right, then node (if they are not nullptr)
    /**
        \warning apply_func must take only one pointer to the node
    */
    template<typename... ExtraArgs>
    int PostfixVisitor(int (*apply_func)(Tree<data_T>*, ExtraArgs...), ExtraArgs... to_function);

    /// Applies function to the left, then node, then right (if they are not nullptr)
    /**
        \warning apply_func must take only one pointer to the node
    */
    template<typename... ExtraArgs>
    int InfixVisitor(int (*apply_func)(Tree<data_T>*, ExtraArgs...), ExtraArgs... to_function);

    /// Creates output for DOT
    /**
        \param [in] print_data_to_dot Function that prints objects of data's type in DOT format
    */
    template<typename... ExtraArgs>
    int CreateDotOutput(int (*print_data_to_dot)(const data_T&, FILE*, ExtraArgs...),
                        ExtraArgs... to_function);
};

#include "Tree.hpp"

#endif // TREE_H_INCLUDED
