#ifndef G_STRING_UTIL_H
#define G_STRING_UTIL_H

namespace gos
{

namespace base64
{
std::string Encode(UINT8 *pucIn, UINT32 ulLen);
std::string EncodeString(const CHAR *szIn);
BOOL Decode(const CHAR *szIn, UINT8* &pucOut, UINT32 &ulOutLen);
std::string DecodeString(const CHAR *szIn);
} // namespace base64

} // namespace gos


#endif
