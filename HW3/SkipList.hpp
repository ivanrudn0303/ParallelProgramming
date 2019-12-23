#include "SkipList.h"

#include <iostream>
#include <mutex>
#include <random>


int random_level()
{
    thread_local std::mt19937 sGen(std::random_device{}());
    std::bernoulli_distribution sCoin(0.5);
    int sLevel = 1;
    for (; sLevel != DEPTH && sCoin(sGen); sLevel++);
    return sLevel;
}

template<typename T>
void atomic_change_and_release(Node<T>** aOld, Node<T>** aNew)
{
    if (*aOld == *aNew)
        return;
    atomic_noexcept
    {
        (*aOld)->m_Owners--;
        *aOld = *aNew;
    }
}


template<typename T>
Node<T>* Node<T>::atomic_next(uint64_t aKey)
{
    atomic_noexcept
    {
        if (m_Next && aKey >= m_Next->m_Key)
        {
            m_Next->m_Owners++;
            return m_Next;
        }
        else
            return nullptr;
    }
}

template<typename T>
Node<T>* Node<T>::atomic_bot()
{
    if (!m_Bot)
        return nullptr;
    __sync_fetch_and_add(&m_Bot->m_Owners, 1);
    return m_Bot;
}

template<typename T>
Node<T>* atomic_find_prev(Node<T>* aCurrent, uint64_t aKey)
{
    __sync_fetch_and_add(&aCurrent->m_Owners, 1);
    Node<T>* sRet = aCurrent;
    Node<T>* sNext = aCurrent->atomic_next(aKey);
    while (sNext)
    {
        if (aKey == sNext->m_Key)
        {
            __sync_fetch_and_sub(&sNext->m_Owners, 1);
            return sRet;
        }
        atomic_change_and_release(&sRet, &sNext);
        sNext = sRet->atomic_next(aKey);
    }
    __sync_fetch_and_sub(&sRet->m_Owners, 1);
    return nullptr;
}

template<typename T>
Node<T>* recursive_insert(Node<T>* sCurrent, uint64_t aKey, const T& aValue);

template<typename T>
SkipList<T>::SkipList()
{
    for (int i = 0; i < DEPTH; ++i)
    {
        m_Data[i].m_Next = nullptr;
        m_Data[i].m_Key = 0;
        m_Data[i].m_Value = {};
        m_Data[i].m_Owners = 0;
        if (i + 1 < DEPTH)
            m_Data[i].m_Bot = &m_Data[i+1];
        else
            m_Data[i].m_Bot = nullptr;
    }
}

template<typename T>
bool SkipList<T>::find(uint64_t aKey, T& aValue)
{
    Node<T>* sCurrent = &m_Data[TOP];
    __sync_fetch_and_add(&sCurrent->m_Owners, 1);
    while (sCurrent)
    {
        if (aKey == sCurrent->m_Key)
        {
            aValue = sCurrent->m_Value;
            __sync_fetch_and_sub(&sCurrent->m_Owners, 1);
            return true;
        }
        Node<T>* sNext = sCurrent->atomic_next(aKey);
        if (sNext)
            atomic_change_and_release(&sCurrent, &sNext);
        else
        {
            Node<T>* sBot = sCurrent->atomic_bot();
            // if sBot equals nullptr, just leave while
            atomic_change_and_release(&sCurrent, &sBot);
        }
    }
    return false;
}

template<typename T>
void SkipList<T>::remove(uint64_t aKey)
{
    // insertion for each depth level
    Node<T>* sCurrent = &m_Data[TOP];
    __sync_fetch_and_add(&sCurrent->m_Owners, 1);
    while (sCurrent)
    {
        Node<T>* sNext = atomic_find_prev(sCurrent, aKey);
        if (!sNext)
        {
            Node<T>* sBot = sCurrent->atomic_bot();
            // if sBot equals nullptr, just leave while
            atomic_change_and_release(&sCurrent, &sBot);
            continue;
        }

        atomic_change_and_release(&sCurrent, &sNext);
        Node<T>* sTmp = nullptr;
        atomic_noexcept
        {
            if (nullptr != sCurrent->m_Next && aKey == sCurrent->m_Next->m_Key && 0 == sCurrent->m_Next->m_Owners)
            {
                sTmp = sCurrent->m_Next;
                sCurrent->m_Next = sTmp->m_Next;
            }
        }
        delete sTmp;
    }
}

template<typename T>
void SkipList<T>::insert(uint64_t aKey, const T& aValue)
{
    int sLevel = random_level();
    Node<T>* sCurrent = &m_Data[DEPTH - sLevel];
    __sync_fetch_and_add(&sCurrent->m_Owners, 1);
    recursive_insert(sCurrent, aKey, aValue);
}

template<typename T>
Node<T>* recursive_insert(Node<T>* sCurrent, uint64_t aKey, const T& aValue)
{
    while (sCurrent)
    {
        Node<T>* sNext = sCurrent->atomic_next(aKey);
        if (sNext)
            atomic_change_and_release(&sCurrent, &sNext);
        else
            break;
    }

    Node<T>* sBot = sCurrent->atomic_bot();
    Node<T>* sRet = nullptr;
    if (sBot)
        sRet = recursive_insert(sBot, aKey, aValue);

    Node<T>* sDataToInsert = new Node<T>{aKey, aValue, {}, nullptr, sRet};
    while (sCurrent)
    {
        Node<T>* sNext = sCurrent->atomic_next(aKey);
        if (sNext)
            atomic_change_and_release(&sCurrent, &sNext);
        else
        {
            atomic_noexcept
            {
                if (nullptr != sCurrent->m_Next && aKey >= sCurrent->m_Next->m_Key)
                    continue;
                else
                {
                    sDataToInsert->m_Next = sCurrent->m_Next;
                    sCurrent->m_Next = sDataToInsert;
                    sCurrent->m_Owners--;
                    break;
                }
            }
        }
    }
    return sDataToInsert;
}

template<typename T>
void SkipList<T>::Check()
{
    for (int i = 0; i < DEPTH; ++i)
    {
        Node<T>* sCurrent = &m_Data[i];
        while (sCurrent)
        {
            if (sCurrent->m_Owners > 0)
                std::cout << "error for level = " << i << " key = " << sCurrent->m_Key << " owners = " << sCurrent->m_Owners << std::endl;
            if (sCurrent->m_Next && sCurrent->m_Key > sCurrent->m_Next->m_Key)
                std::cout << "error for level = " << i << " key = " << sCurrent->m_Key << " is more than " << sCurrent->m_Next->m_Key << std::endl;
            sCurrent = sCurrent->m_Next;
        }
    }
}
