#include "g_include.h"

namespace gos
{
namespace base64
{
std::string Encode(UINT8 *pucIn, UINT32 ulLen)
{
    if (!pucIn || ulLen == 0)
    {
        return "";
    }

    INT32   iDstLen = ulLen*4/3+4+1;
    CHAR    *szDst = (CHAR*)calloc(1, iDstLen);
    std::string  str;

    if (!gos_base64_encode(pucIn, ulLen, (UINT8 *)szDst, &iDstLen))
    {
        free(szDst);
        return "";
    }

    str = szDst;
    free(szDst);

    return str;
}

std::string EncodeString(const CHAR *szIn)
{
    return Encode((UINT8*)szIn, strlen(szIn));
}

BOOL Decode(const CHAR *szIn, UINT8* &pucOut, UINT32 &ulOutLen)
{
    pucOut = NULL;
    ulOutLen = 0;

    if (!szIn || *szIn == '\0')
    {
        return FALSE;
    }

    UINT32  ulLen = strlen(szIn);
    INT32   iDstLen = 1+((ulLen*6+7)>>3);
    UINT8   *pucDst = (UINT8*)calloc(1, iDstLen);

    if (!gos_base64_decode((UINT8 *)szIn, ulLen, pucDst, &iDstLen))
    {
        free(pucDst);
        return FALSE;
    }

    pucOut = pucDst;
    ulOutLen = iDstLen;

    return TRUE;
}

std::string DecodeString(const CHAR *szIn)
{
    UINT8   *pucOut = NULL;
    UINT32  ulOutLen = 0;

    if (!Decode(szIn, pucOut, ulOutLen))
    {
        return "";
    }

    std::string     strOut((CHAR*)pucOut);

    return strOut;
}

} // namespace base64

} // namespace gos
