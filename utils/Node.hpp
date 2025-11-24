
// node.hpp
#ifndef NODE
#define NODE

#include <string>

template <typename T>

struct Node
{
    T value;
    Node<T> *left;
    Node<T> *right;

    Node(T val)
    {
        value = val;
        left = nullptr;
        right = nullptr;
    }
};

#endif
