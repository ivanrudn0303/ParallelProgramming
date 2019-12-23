#pragma once

#include <atomic>
#include <cstdint>

#define DEPTH 64
#define TOP 0

template<typename T>
struct Node
{
    uint64_t m_Key;
    T m_Value;
    // Atomic counter
    int m_Owners;
    Node<T>* m_Next;
    Node<T>* m_Bot;
    Node<T>* atomic_next(uint64_t aKey);
    Node<T>* atomic_bot();
};

template<typename T>
class SkipList
{
    Node<T> m_Data[DEPTH];
public:
    SkipList();
    void Check();
    bool find(uint64_t a_Key, T& a_Value);
    void insert(uint64_t a_Key, const T& a_Value);
    void remove(uint64_t a_Key);
};
