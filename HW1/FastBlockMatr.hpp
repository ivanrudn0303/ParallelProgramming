#pragma once

#include <stddef.h>
size_t requiredSize(size_t aSize, size_t aBase)
{
    return aSize / aBase + (aSize % aBase != 0);
}

template<typename T, size_t N>
struct Block
{
    T Data[N][N];
    Block(): Data{}
    {}

    Block operator*(const Block& aRht) const
    {
        Block sRes;
        for (size_t i = 0; i < N; ++i)
            for (size_t j = 0; j < N; ++j)
                for (size_t k = 0; k < N; ++k)
                    sRes.Data[i][j] += this->Data[i][k] * aRht.Data[k][j];
        return sRes;
    }

    Block& operator*=(const Block& aRht)
    {
        *this = (*this) * aRht;
        return *this;
    }

    Block operator+(const Block& aRht) const
    {
        Block sRes = *this;
        for (size_t i = 0 ; i < N; ++i)
            for (size_t j = 0; j < N; ++j)
                sRes.Data[i][j] += aRht.Data[i][j];
        return sRes;
    }

    Block& operator+=(const Block& aRht)
    {
        *this = (*this) + aRht;
        return *this;
    }
};

template<typename T, size_t N = 4>
class Matrix
{
private:
    Block<T, N>* m_Data;
    size_t m_Rows;
    size_t m_Columns;

public:
    Matrix operator*(const Matrix& aRht) const;
    Matrix& operator*=(const Matrix& aRht);
    T& operator()(size_t aRow, size_t aColumn);

    Matrix(size_t aRows, size_t aColumns):m_Data(new Block<T, N>[requiredSize(aRows, N) * requiredSize(aColumns, N)]), m_Rows(aRows), m_Columns(aColumns)
    {}

    ~Matrix()
    {
        delete[] m_Data;
    }
};

template<typename T, size_t N>
T& Matrix<T, N>::operator()(size_t aRow, size_t aColumn)
{
    size_t sBlockRow = aRow / N;
    size_t sBlockColumn = aColumn / N;
    return m_Data[sBlockRow * requiredSize(m_Columns, N) + sBlockColumn].Data[aRow % N][aColumn % N];
}

template<typename T, size_t N>
Matrix<T, N> Matrix<T, N>::operator*(const Matrix<T, N>& aRht) const
{
    Matrix<T, N> sResult(this->m_Rows, aRht.m_Columns);
    for (size_t i = 0; i < requiredSize(sResult.m_Rows, N); ++i)
        for (size_t j = 0; j < requiredSize(sResult.m_Columns, N); ++j)
        {
            Block<T, N> sSum;
            for (size_t k = 0; k < requiredSize(this->m_Columns, N); ++k)
                sSum += this->m_Data[i * requiredSize(this->m_Columns, N) + k] * aRht.m_Data[k * requiredSize(aRht.m_Columns, N) + j];
            sResult.m_Data[i * requiredSize(sResult.m_Columns, N) + j] = sSum;
        }
    return sResult;
}
