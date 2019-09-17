#pragma once

#include <cstdio>


namespace LockFree
{
    template<typename T>
    class Queue
    {
        size_t m_Size;
        T* m_Data;
        size_t m_First;
        size_t m_Last;
    public:
        Queue(size_t aSize): m_Size(aSize), m_Data(new T[aSize]), m_First(0), m_Last(0) {}
        ~Queue();
        void Push(const T& aValue);
        void Pop(T& aDest);
    };

    template<typename T>
    Queue<T>::~Queue()
    {
        delete[] m_Data;
    }

    template<typename T>
    void Queue<T>::Pop(T& aDest)
    {
        while (true)
        {
            atomic_noexcept
            {
                if (m_Last == m_First)
                    continue;
                aDest = m_Data[m_First % m_Size];
                m_Data[m_First++ % m_Size].~T();
                return;
            }
        }
    }

    template<typename T>
    void Queue<T>::Push(const T& aValue)
    {
        while (true)
        {
            atomic_noexcept
            {
                if (m_Last == m_First + m_Size)
                    continue;
                m_Data[m_Last++ % m_Size] = aValue;
                return;
            }
        }
    }
} //LockFree
