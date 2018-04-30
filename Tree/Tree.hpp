#ifndef TREE_HPP_INCLUDED
#define TREE_HPP_INCLUDED

template<typename data_T>
template<typename... ExtraArgs>
int Tree<data_T>::PrintTree(Tree<data_T>* root, FILE* output,
                            int (*print_data_to_dot)(const data_T&, FILE*, ExtraArgs...),
                            ExtraArgs... to_finction)
{
    // assert(output);
    assert(root);
    assert(print_data_to_dot);

    // Printing node's beginning
    fprintf(output, "%d ", root);

    fprintf(output, DOT::BEGIN_DECLARATION);
    fprintf(output, DOT::LABELS);
    fprintf(output, DOT::NEXT_FIELD);

    // HERE WE GO
    fprintf(output, DOT::BEGIN_COLUMN);

    fprintf(output, "%p", root);
    fprintf(output, DOT::NEXT_FIELD);

    // Printing what node's containinig
    print_data_to_dot(root->Data(), output, to_finction...);
    fprintf(output, DOT::NEXT_FIELD);

    // Printing node's end
    fprintf(output, "%p", root->Left());
    fprintf(output, DOT::NEXT_FIELD);
    fprintf(output, "%p", root->Right());
    fprintf(output, DOT::END_COLUMN);

    fprintf(output, DOT::END_DECLARATION);

    return ERROR::OK;
}

template<typename data_T>
int Tree<data_T>::SetConnections(Tree<data_T>* root, FILE* output)
{
    // assert(output);
    assert(root);

    if(root->Left())
    {
        fprintf(output, "%d", root);
        fprintf(output, DOT::TO);
        fprintf(output, "%d", root->Left());
        fprintf(output, DOT::LEFT_DIRECTION);
    }
    if(root->Right())
    {
        fprintf(output, "%d", root);
        fprintf(output, DOT::TO);
        fprintf(output, "%d", root->Right());
        fprintf(output, DOT::RIGHT_DIRECTION);
    }

    return ERROR::OK;
}

template<typename data_T>
Tree<data_T>* Tree<data_T>::Copy(Tree<data_T>* src)
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
template<typename... ExtraArgs>
int  Tree<data_T>::PrefixVisitor(int(*apply_func)(Tree<data_T>*, ExtraArgs...), ExtraArgs... to_function)
{
    assert(apply_func);

    int apply_status = 0;
    try
    {
        // Applying to the node itself
        apply_status = apply_func(this, to_function...);

        // Applying to the left branch
        if(this->Left())
            this->Left()->PrefixVisitor(apply_func, to_function...);

        // Applying to the right branch
        if(this->Right())
            this->Right()->PrefixVisitor(apply_func, to_function...);
    }
    catch(...)
    {
        throw;
    }

    return apply_status;
}

template<typename data_T>
template<typename... ExtraArgs>
int Tree<data_T>::PostfixVisitor(int (*apply_func)(Tree<data_T>*, ExtraArgs...), ExtraArgs... to_function)
{
    assert(apply_func);

    int apply_status = 0;
    try
    {
        // Applying to the left branch
        if(this->Left())
            this->Left()->PrefixVisitor(apply_func, to_function...);

        // Applying to the right branch
        if(this->Right())
            this->Right()->PrefixVisitor(apply_func, to_function...);

        // Applying to the node itself
        apply_status = apply_func(this, to_function...);
    }
    catch(...)
    {
        throw;
    }

    return apply_status;
}

template<typename data_T>
template<typename... ExtraArgs>
int   Tree<data_T>::InfixVisitor(int (*apply_func)(Tree<data_T>*, ExtraArgs...), ExtraArgs... to_function)
{
    assert(apply_func);

    int apply_status = 0;
    try
    {
        // Applying to the left branch
        if(this->Left())
            this->Left()->PrefixVisitor(apply_func, to_function...);

        // Applying to the node itself
        apply_status = apply_func(this, to_function...);

        // Applying to the right branch
        if(this->Right())
            this->Right()->PrefixVisitor(apply_func, to_function...);
    }
    catch(...)
    {
        throw;
    }

    return apply_status;
}

template<typename data_T>
template<typename... ExtraArgs>
int Tree<data_T>::CreateDotOutput(  int (*print_data_to_dot)(const data_T&, FILE*, ExtraArgs...),
                                    ExtraArgs... to_function)
{
    assert(print_data_to_dot);

    FILE* output = fopen(DOT::DEFAULT_DOT_FILENAME, "w");
    if(!output)
    {
        std::cout << "Failed to open " << DOT::DEFAULT_DOT_FILENAME << "\n";
        return -ERROR::FILE_NOT_OPENED;
    }

    // Preparing file
    fprintf(output, DOT::BEGIN_GRAPH);
    fprintf(output, DOT::SET_RECORD_SHAPE);

    // Setting nodes
    this->PrefixVisitor(PrintTree, output, print_data_to_dot, to_function...);
    // Setting connactions
    this->PrefixVisitor(SetConnections, output);

    // Finishing file
    fprintf(output, DOT::END_GRAPH);

    fclose(output);
    return ERROR::OK;
}

#endif // TREE_HPP_INCLUDED
