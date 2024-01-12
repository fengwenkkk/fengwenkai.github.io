#ifndef ARRAY_UTIL_H
#define ARRAY_UTIL_H

template<typename _TYPE>
_TYPE* InitArray(UINT32 ulDimSize)
{
    return (_TYPE*)calloc(1, ulDimSize*sizeof(_TYPE));
}

template<typename _TYPE>
VOID FreeArray(_TYPE* &p)
{
    if (p)
    {
        free(p);
        p = NULL;
    }
}

template<typename _TYPE>
_TYPE** InitArray2(UINT32 ulDim1Size, UINT32 ulDim2Size)
{
    _TYPE   **p = (_TYPE**)calloc(ulDim1Size, sizeof(_TYPE*)+ulDim2Size*sizeof(_TYPE));

    if (!p)
    {
        return NULL;
    }

    _TYPE *q = (_TYPE*)(p+ulDim1Size);

    for (UINT32 i=0; i<ulDim1Size; i++)
    {
        p[i] = q;
        q += ulDim2Size;
    }

    return p;
}

template<typename _TYPE>
VOID FreeArray2(_TYPE** &p)
{
    if (p)
    {
        free(p);
        p = NULL;
    }
}

template<typename _TYPE>
_TYPE*** InitArray3(UINT32 ulDim1Size, UINT32 ulDim2Size, UINT32 ulDim3Size)
{
    UINT32  i, j;
    _TYPE   ***p = (_TYPE***)calloc(ulDim1Size, sizeof(_TYPE**)+ulDim2Size*sizeof(_TYPE*)+ulDim2Size*ulDim3Size*sizeof(_TYPE));

    if (!p)
    {
        return NULL;
    }

    _TYPE **q = (_TYPE**)(p+ulDim1Size);
    _TYPE *r = (_TYPE*)(q+ulDim1Size*ulDim2Size);

    for (i=0; i<ulDim1Size; i++)
    {
        p[i] = q;
        q += ulDim2Size;
    }

    for (i=0; i<ulDim1Size; i++)
    {
        for (j=0; j<ulDim2Size; j++)
        {
            p[i][j] = r;

            r += ulDim3Size;
        }
    }

    return p;
}

template<typename _TYPE>
VOID FreeArray3(_TYPE*** &p)
{
    if (p)
    {
        free(p);
        p = NULL;
    }
}

template<typename _TYPE>
_TYPE** InitArray(UINT32 ulDim1Size, UINT32 ulDim2Size)
{
    return InitArray2<_TYPE>(ulDim1Size, ulDim2Size);
}

template<typename _TYPE>
VOID FreeArray(_TYPE** &p)
{
    FreeArray2<_TYPE>(p);
}

template<typename _TYPE>
_TYPE** InitArray(UINT32 ulDim1Size, UINT32 ulDim2Size, UINT32 ulDim3Size)
{
    return InitArray3<_TYPE>(ulDim1Size, ulDim2Size, ulDim3Size);
}

template<typename _TYPE>
VOID FreeArray(_TYPE*** &p)
{
    FreeArray3<_TYPE>(p);
}

#endif
