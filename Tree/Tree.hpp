#ifndef TREE_HPP_INCLUDED
#define TREE_HPP_INCLUDED

template<typename data_T>
Tree<data_T>* Tree<data_T>::Copy(Tree<data_T>* src)                  // <-- NOT IMPLEMENTED
{
    if(!src)    return nullptr;

    // Allocating memory
    Tree<data_T>* dst = nullptr;
    try
    {
        dst = new Tree<data_T>;
    }
    catch(const std::bad_alloc& ex)
    {
        std::cout << "Failed to allocate " << sizeof(Tree<data_T>) << " bytes\n";
        return nullptr;
    }

    // Copying src.data_
    try
    {
        dst->Data(src->Data());
    }
    catch(...)
    {
        throw;
    }

    // Copying branches
    if(src->Left())
    {
        try
        {
            dst->Left(Copy(src->Left()));
        }
        catch(...)
        {
            throw;
        }
    }
    if(src->Right())
    {
        try
        {
            dst->Right(Copy(src->Right()));
        }
        catch(...)
        {
            throw;
        }
    }

    TREE_DBG
    {
        std::cout << "Tree:Copy() returning:\n";
        std::cout << "dst       = " << dst << "\n";
        std::cout << "dst.left  = " << dst->Left() << "\n";
        std::cout << "dst.right = " << dst->Right() << "\n";
    }
    return dst;
}

template<typename data_T>
int Tree<data_T>::PrefixVisitor(int (*apply_func)(Tree<data_T>*))
{
    assert(apply_func);

    int apply_status = 0;
    try
    {
        // Applying to the node itself
        apply_status = apply_func(this);

        // Applying to the left branch
        if(this->Left())
            this->Left()->PrefixVisitor(apply_func);

        // Applying to the right branch
        if(this->Right())
            this->Right()->PrefixVisitor(apply_func);
    }
    catch(...)
    {
        throw;
    }

    return apply_status;
}

template<typename data_T>
int Tree<data_T>::PostfixVisitor(int (*apply_func)(Tree<data_T>*))
{
    assert(apply_func);

    int apply_status = 0;
    try
    {
        // Applying to the left branch
        if(this->Left())
            this->Left()->PrefixVisitor(apply_func);

        // Applying to the right branch
        if(this->Right())
            this->Right()->PrefixVisitor(apply_func);

        // Applying to the node itself
        apply_status = apply_func(this);
    }
    catch(...)
    {
        throw;
    }

    return apply_status;
}

template<typename data_T>
int Tree<data_T>::InfixVisitor(int (*apply_func)(Tree<data_T>*))
{
    assert(apply_func);

    int apply_status = 0;
    try
    {
        // Applying to the left branch
        if(this->Left())
            this->Left()->PrefixVisitor(apply_func);

        // Applying to the node itself
        apply_status = apply_func(this);

        // Applying to the right branch
        if(this->Right())
            this->Right()->PrefixVisitor(apply_func);
    }
    catch(...)
    {
        throw;
    }

    return apply_status;
}

template<typename data_T>
int Tree<data_T>::CreateDotOutput()
{
    FILE* output = fopen(DEFAULT_DOT_CODE_OUTPUT, "w");
    fclose(output);
}

#endif // TREE_HPP_INCLUDED
