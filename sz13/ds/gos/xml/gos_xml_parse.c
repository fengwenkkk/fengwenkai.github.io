#include "gos_api.h"
#include "gos_func.h"
#include "gos_xml_parse.h"

#ifndef _OSWIN32_
#include <ctype.h>
#endif

BOOL gos_xml_parse_child_node(_GOS_XML_NODE_T *pstNode, CHAR **pszXML);
VOID gos_xml_free_node(_GOS_XML_NODE_T *pstNode);

VOID *gos_xml_mem_alloc(_GOS_XML_DOCUMENT_T *pstDoc, UINT32 ulSize)
{
    VOID *pMem = NULL;

    if (!pstDoc || ulSize == 0)
    {
        return NULL;
    }

    pMem = GOS_ZALLOC(ulSize);

    return pMem;
}

VOID gos_xml_mem_free(_GOS_XML_DOCUMENT_T *pstDoc, VOID *pMem)
{
    GOS_FREE(pMem);
}

CHAR* gos_xml_strdup(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szText)
{
    CHAR  *szNewText;

    if (!szText)
    {
        return NULL;
    }

    if (pstDoc->bParsed)
    {
        szNewText = (CHAR*)gos_xml_mem_alloc(pstDoc, strlen(szText)+1);
        if (szNewText)
        {
            strcpy(szNewText, szText);
        }

        return szNewText;
    }
    else
    {
        return szText;
    }
}

VOID gos_xml_strfree(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szText)
{
    if (!pstDoc || !szText)
    {
        return;
    }

    if (szText < pstDoc->szXML || szText > pstDoc->szXMLEnd)
    {
        gos_xml_mem_free(pstDoc, szText);
    }
}

/*******************************************************************************
* 函数名称: gos_xml_alloc_node_func
* 功    能: 节点申请函数
* 参    数:
* 参数名称          输入/输出       描述
  pstDoc            IN              Doc
* 函数返回:
*
* 说    明:
*******************************************************************************/
_GOS_XML_NODE_T* gos_xml_alloc_node_func(_GOS_XML_DOCUMENT_T *pstDoc)
{
    _GOS_XML_NODE_T  *pstNode;

    // 有空闲节点
    if (pstDoc->stNodeDummy.pstNext != &pstDoc->stNodeDummy)
    {
        pstNode = pstDoc->stNodeDummy.pstNext;

        pstDoc->stNodeDummy.pstNext = pstNode->pstNext;
        pstNode->pstNext->pstPrev = &pstDoc->stNodeDummy;

        pstNode->pstPrev = NULL;
        pstNode->pstNext = NULL;

        pstNode->pstDoc = pstDoc;

        return pstNode;
    }

    // 动态分配
    pstNode = (_GOS_XML_NODE_T*)GOS_ZALLOC(sizeof(_GOS_XML_NODE_T));
    if (!pstNode)
    {
        return NULL;
    }

    pstNode->pstDoc = pstDoc;

    return pstNode;
}

/*******************************************************************************
* 函数名称: gos_xml_free_node_func
* 功    能: 节点释放函数
* 参    数:
* 参数名称          输入/输出       描述
  pstDoc            IN              Doc
  pstNode           IN              节点
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID gos_xml_free_node_func(_GOS_XML_DOCUMENT_T *pstDoc, _GOS_XML_NODE_T *pstNode)
{
    if (!pstNode)
    {
        return;
    }

    gos_xml_strfree(pstDoc, pstNode->szName);
    gos_xml_strfree(pstDoc, pstNode->szValue);

    pstNode->szName = NULL;
    pstNode->szValue = NULL;

    // 静态地址，回收
    if (pstNode >= pstDoc->pstNodeList && pstNode < (pstDoc->pstNodeList + pstDoc->ulStaticNodeNum))
    {
        memset(pstNode, 0, sizeof(_GOS_XML_NODE_T));

        if (pstDoc->stNodeDummy.pstNext)
        {
            pstDoc->stNodeDummy.pstNext->pstPrev = pstNode;
        }
        pstNode->pstNext = pstDoc->stNodeDummy.pstNext;

        pstNode->pstPrev = &pstDoc->stNodeDummy;
        pstDoc->stNodeDummy.pstNext = pstNode;

        return;
    }

    // 动态地址，释放
    gos_xml_mem_free(pstDoc, pstNode);
}

/*******************************************************************************
* 函数名称: gos_xml_alloc_attr_func
* 功    能: 属性节点申请函数
* 参    数:
* 参数名称          输入/输出       描述
  pstDoc            IN              Doc
  pstAttr           IN              节点属性
* 函数返回:
*
* 说    明:
*******************************************************************************/
_GOS_XML_ATTR_T* gos_xml_alloc_attr_func(_GOS_XML_DOCUMENT_T *pstDoc)
{
    _GOS_XML_ATTR_T  *pstAttr;

    // 有空闲节点
    if (pstDoc->stAttrDummy.pstNext != &pstDoc->stAttrDummy)
    {
        pstAttr = pstDoc->stAttrDummy.pstNext;

        pstDoc->stAttrDummy.pstNext = pstAttr->pstNext;
        pstAttr->pstNext->pstPrev = &pstDoc->stAttrDummy;

        pstAttr->pstPrev = NULL;
        pstAttr->pstNext = NULL;

        return pstAttr;
    }

    // 动态分配
    pstAttr = (_GOS_XML_ATTR_T*)GOS_ZALLOC(sizeof(_GOS_XML_ATTR_T));
    if (!pstAttr)
    {
        return NULL;
    }

    return pstAttr;
}

/*******************************************************************************
* 函数名称: gos_xml_free_attr_func
* 功    能: 属性节点释放函数
* 参    数:
* 参数名称          输入/输出       描述
  pstDoc            IN              Doc
  pstAttr           IN              节点属性
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID gos_xml_free_attr_func(_GOS_XML_DOCUMENT_T *pstDoc, _GOS_XML_ATTR_T *pstAttr)
{
    if (!pstAttr)
    {
        return;
    }

    gos_xml_strfree(pstDoc, pstAttr->szName);
    gos_xml_strfree(pstDoc, pstAttr->szValue);

    pstAttr->szName = NULL;
    pstAttr->szValue = NULL;

    // 静态地址，回收
    if (pstAttr >= pstDoc->pstAttrList && pstAttr < (pstDoc->pstAttrList + pstDoc->ulStaticAttrNum) )
    {
        memset(pstAttr, 0, sizeof(_GOS_XML_ATTR_T));

        if (pstDoc->stAttrDummy.pstNext)
        {
            pstDoc->stAttrDummy.pstNext->pstPrev = pstAttr;
        }
        pstAttr->pstNext = pstDoc->stAttrDummy.pstNext;

        pstAttr->pstPrev = &pstDoc->stAttrDummy;
        pstDoc->stAttrDummy.pstNext = pstAttr;

        return;
    }

    // 动态地址，释放
    gos_xml_mem_free(pstDoc, pstAttr);
}

/*******************************************************************************
* 函数名称: gos_xml_trim
* 功    能: 去除字符串的前空格
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              节点
  pstAttr           IN              节点属性
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_trim_left(CHAR **pszXML, UINT32 *pulLen)
{
    CHAR    *szXML;

    if (NULL == pszXML  ||
        NULL == *pszXML )
    {
        return FALSE;
    }

    szXML = *pszXML;
    for (; *szXML!='\0'; szXML++)
    {
        if (!gos_isspace(*szXML))
        {
            break;
        }

        if (pulLen)
        {
            pulLen[0] --;
        }
    }

     *pszXML = szXML;

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_trim
* 功    能: 去除字符串的前后空格
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              节点
  pstAttr           IN              节点属性
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_trim(CHAR **pszXML, UINT32 *pulLen)
{
    CHAR    *szXML;
    CHAR    *szXMLEnd;
    UINT32  ulLen;

    if (NULL == pszXML  ||
        NULL == *pszXML )
    {
        return FALSE;
    }

    szXML = *pszXML;

    for (; *szXML!='\0'; szXML++)
    {
        if (!gos_isspace(*szXML))
        {
            break;
        }

        if (pulLen)
        {
            pulLen[0] --;
        }
    }

    if (pulLen)
    {
        ulLen = pulLen[0];
    }
    else
    {
        ulLen = strlen(szXML);
    }

    for (szXMLEnd=szXML+ulLen-1; szXMLEnd>=szXML; szXMLEnd--)
    {
        if (!gos_isspace(*szXMLEnd))
        {
            break;
        }

        *szXMLEnd = '\0';
        if (pulLen)
        {
            pulLen[0] --;
        }
    }

    *pszXML = szXML;

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_node_append
* 功    能: 节点添加子节点
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              父节点
  pstChildNode      IN              子节点
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_node_append(_GOS_XML_NODE_T *pstNode, _GOS_XML_NODE_T* pstChildNode)
{
    if (!pstNode || !pstChildNode)
    {
        return FALSE;
    }

    if (!pstNode->pstFirstChild)
    {
        pstNode->pstFirstChild = pstChildNode;
        pstNode->pstLastChild = pstChildNode;
        return TRUE;
    }
    else
    {
#if 0
        _GOS_XML_NODE_T *pstLastChildNode;
        for (pstLastChildNode=pstNode->pstFirstChild; ; pstLastChildNode=pstLastChildNode->pstSibling)
        {
            if (NULL == pstLastChildNode->pstSibling)
            {
                pstLastChildNode->pstSibling = pstChildNode;
                break;
            }
        }
#else
        if (!pstNode->pstLastChild)
        {
            return FALSE;
        }

        pstNode->pstLastChild->pstSibling = pstChildNode;
        pstNode->pstLastChild = pstChildNode;
#endif
        return TRUE;
    }
}

/*******************************************************************************
* 函数名称: gos_xml_node_replace
* 功    能: 节点替换子节点
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              父节点
  pstChildNode      IN              子节点
* 函数返回:
*
* 说    明:
*******************************************************************************/
/*BOOL gos_xml_node_replace(_GOS_XML_NODE_T *pstNode, _GOS_XML_NODE_T* pstChildNode, _GOS_XML_NODE_T* pstNewChildNode)
{
    if (!pstNode || !pstChildNode || !pstNewChildNode)
    {
        return FALSE;
    }

    if (pstNode->pstFirstChild == pstNode->pstLastChild)
    {
        gos_xml_free_node(pstChildNode);

        pstNode->pstFirstChild = pstNewChildNode;
        pstNode->pstLastChild = pstNewChildNode;
        return TRUE;
    }
    else
    {
        gos_xml_replace_node

        return TRUE;
    }

    //gos_xml_free_node(pstChildNode);

}*/

/*******************************************************************************
* 函数名称: gos_xml_attr_append
* 功    能: 节点添加属性值
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              节点
  pstAttr           IN              节点属性
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_attr_append(_GOS_XML_NODE_T *pstNode, _GOS_XML_ATTR_T* pstAttr)
{
    _GOS_XML_ATTR_T *pstLastAttr;

    if (!pstNode || !pstAttr ||
        GOS_XML_EMPTY_STR(pstAttr->szName))
    {
        return FALSE;
    }

    if (!pstNode->pstAttr)
    {
        pstNode->pstAttr = pstAttr;
        return TRUE;
    }
    else
    {
        for (pstLastAttr=pstNode->pstAttr; ; pstLastAttr=pstLastAttr->pstSibling)
        {
            if (NULL == pstLastAttr->pstSibling)
            {
                pstLastAttr->pstSibling = pstAttr;
                break;
            }
        }

        return TRUE;
    }
}

BOOL gos_xml_parse_declaration(_GOS_XML_DOCUMENT_T *pstDoc, CHAR **pszXML, UINT32 *pulLen)
{
    CHAR    *szXML;
    CHAR    *szTmp;

    if (!pszXML || !pulLen)
    {
        return FALSE;
    }

    szXML = *pszXML;
    if (!szXML)
    {
        return FALSE;
    }

    // 解析Declearation
    // <?xml version="1.0" encoding="utf-8"?>
    if (szXML[0] != '<' && szXML[1] != '?')
    {
        return FALSE;
    }

    szXML += 2;
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || memcmp(szXML, "xml", 3) != 0)
    {
        return FALSE;
    }

    szXML += 3;
    if (!gos_isspace(*szXML))
    {
        return FALSE;
    }

    szXML++;

    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || memcmp(szXML, "version", 7) != 0)
    {
        return FALSE;
    }

    szXML += 7;
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || *szXML != '=')
    {
        return FALSE;
    }

    szXML++;
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || *szXML != '"')
    {
        return FALSE;
    }

    pstDoc->szVersion = ++szXML;
    for (; *szXML!='"'; szXML++)
    {
        if (*szXML == '\0')
        {
            return FALSE;
        }
    }

    szTmp = (CHAR*)GOS_ZALLOC(szXML-pstDoc->szVersion+2);
    if (!szTmp)
    {
        return FALSE;
    }
    strncpy(szTmp, pstDoc->szVersion, szXML-pstDoc->szVersion);
    pstDoc->szVersion = szTmp;

    szXML++;

    // now like  encoding="utf-8"?>
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || memcmp(szXML, "encoding", 8) != 0)
    {
        return FALSE;
    }

    szXML += 8;
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || *szXML != '=')
    {
        return FALSE;
    }

    szXML++;
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML || *szXML != '"')
    {
        return FALSE;
    }

    pstDoc->szEncoding = ++szXML;
    for (; *szXML!='"'; szXML++)
    {
        if (*szXML == '\0')
        {
            return FALSE;
        }
    }

    szTmp = (CHAR*)GOS_ZALLOC(szXML-pstDoc->szEncoding+2);
    if (!szTmp)
    {
        return FALSE;
    }

    strncpy(szTmp, pstDoc->szEncoding, szXML-pstDoc->szEncoding);
    pstDoc->szEncoding = szTmp;

    szXML++;

    // now like ?>
    gos_xml_trim_left(&szXML, NULL);
    szXML = strstr(szXML, "?>");
    if (!szXML || szXML[0] != '?' || szXML[1] != '>')
    {
        return FALSE;
    }

    szXML += 2;
    gos_xml_trim_left(&szXML, NULL);
    if (!szXML)
    {
        return FALSE;
    }

    *pulLen -= (szXML - *pszXML);
    *pszXML = szXML;

    gos_xml_trim_left(pszXML, pulLen);

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_load_xml
* 功    能: 加载XML字符串
* 参    数:
* 参数名称          输入/输出       描述
  pszXML            IN/OUT          XML字符串
  pulLen            OUT             XML字符串长度
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_load_xml(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szXML)
{
    BOOL    bComment;
    UINT32  ulLen;
    BOOL    bString;
    BOOL    bFirst;
    INT32   iLeft = 0;
    INT32   iRight = 0;
    CHAR    *szCurr;
    CHAR    *szTmp;

    if (!szXML)
    {
        return FALSE;
    }

    for (; *szXML!='\0'; szXML++)
    {
        if (!gos_isspace(*szXML))
        {
            break;
        }
    }

    ulLen = strlen(szXML);

    // 去除<?...?>前的注释
    while (1)
    {
        gos_xml_trim_left(&szXML, &ulLen);
        if (!szXML)
        {
            return FALSE;
        }

        if (szXML[0] != '<')
        {
            return FALSE;
        }

        if (szXML[1] == '?')
        {
            if (!gos_xml_parse_declaration(pstDoc, &szXML, &ulLen))
            {
                return FALSE;
            }
            else
            {
                break;
            }
        }

        if (pstDoc->bRemoveComment)
        {
            if (szXML[1] == '!' && szXML[2] == '-' && szXML[3] == '-')
            {
                szTmp = strstr(szXML+4, "-->");
                if (!szTmp)
                {
                    return FALSE;
                }

                szTmp += 3;
                ulLen -= (szTmp - szXML);
                szXML = szTmp;
                continue;
            }
        }

        //return FALSE;
        break; // by thomas 2014.6.12
    }

    if (ulLen >= pstDoc->ulStaticXMLLen)
    {
        pstDoc->szXML = (CHAR*)GOS_ZALLOC(ulLen+1);
        if (!pstDoc->szXML)
        {
            return FALSE;
        }
    }
    else
    {
        pstDoc->szXML = pstDoc->szStaticXML;
    }

    // 去除注释
    if (!pstDoc->bRemoveComment)
    {
        memcpy(pstDoc->szXML, szXML, ulLen+1);
        gos_xml_trim(&pstDoc->szXML, &ulLen);

        pstDoc->szXMLEnd = pstDoc->szXML + ulLen;

        return TRUE;
    }

    szCurr = pstDoc->szXML;
    bComment = FALSE;
    bString = FALSE;
    bFirst = TRUE;
    while(*szXML != '\0')
    {
        if (!bComment)
        {
            //if (memcmp(szXML, "<!--", 4) == 0)
            if ('\"' == *szXML)
            {
                bString = !bString;
            }

            if (!bString)
            {
                if ('<' == *szXML)
                {
                    if (iLeft == iRight)
                    {
                        if (memcmp(szXML, "<!--", 4) == 0)
                        {
                            bComment = TRUE;
                            szXML += 4;
                            //szXML[3] = '\0';
                            continue;
                        }
                    }

                    iLeft++;
                }
                else if ('>' == *szXML)
                {
                    iRight++;
                }
            }

            if (bFirst)
            {
                *szCurr++ = *szXML++;
                bFirst = FALSE;
            }
            else
            {
                if (!bString && gos_isspace(*szXML) && gos_isspace(*(szCurr-1)) )
                {
                    szXML++;
                    continue;
                }
                *szCurr++ = *szXML++;
            }
            continue;
        }
        else // bComment
        {
            if (memcmp(szXML, "-->", 3) == 0)
            {
                bComment = FALSE;
                szXML += 3;
                continue;
            }
            else
            {
                szXML++;
            }
        }
    }

    if (bComment || bString)
    {
        return FALSE;
    }

    *szCurr = '\0';
    for (pstDoc->szXMLEnd=szCurr-1; pstDoc->szXMLEnd>=pstDoc->szXML; pstDoc->szXMLEnd--)
    {
        if (!gos_isspace(*pstDoc->szXMLEnd))
        {
            pstDoc->szXMLEnd++;
            *pstDoc->szXMLEnd = '\0';

            break;
        }
    }

    return TRUE;
}

// a="aaa" b="bbbb"
BOOL gos_xml_parse_attr(CHAR **pszXML, CHAR **pszAttrName, CHAR **pszAttrValue)
{
    CHAR    *szCurr;
    CHAR    *szAttrName;
    CHAR    *szAttrValue;

    if (!pszXML)
    {
        return FALSE;
    }

    szCurr = *pszXML;
    if (!szCurr)
    {
        return FALSE;
    }

    szAttrName = szCurr;
    for (; *szCurr!='='; szCurr++)
    {
        if (*szCurr == '\0' || *szCurr == '"')
        {
            return FALSE;
        }
    }

    *szCurr++ = '\0';
    gos_xml_trim(&szAttrName, NULL);
    if (GOS_XML_EMPTY_STR(szAttrName))
    {
        return FALSE;
    }

    gos_xml_trim_left(&szCurr, NULL);
    if (!szCurr || *szCurr != '"')
    {
        return FALSE;
    }

    // skip the "
    szCurr++;
    szAttrValue = szCurr;
    for (; *szCurr!='"'; szCurr++)
    {
        if (*szCurr == '\0')
        {
            return FALSE;
        }
    }

    *szCurr++ = '\0';

    *pszXML = szCurr;
    *pszAttrName = szAttrName;
    *pszAttrValue = szAttrValue;

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_parse_attr_node
* 功    能: 解析某个节点的属性值
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              父节点
  szNode            IN/OUT          XML字符串(格式形如：aaa ddd=ff eee=gg)
* 函数返回:
*
* 说    明:
*******************************************************************************/
_GOS_XML_NODE_T* gos_xml_parse_attr_node(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szNode)
{
    BOOL    bRet = FALSE;
    CHAR    *szNodeName = szNode;
    CHAR    *szCurr = szNode;
    CHAR    *szAttrName;
    CHAR    *szAttrValue;
    _GOS_XML_NODE_T  *pstChildNode;
    _GOS_XML_ATTR_T  *pstAttr;

    // 获取节点名称
    for (szCurr=szNode; *szCurr!='\0'; szCurr++)
    {
        if (gos_isspace(*szCurr))
        {
            *szCurr++ = '\0';
            break;
        }
    }

    if (*szNodeName == '\0')
    {
        return NULL;
    }

    pstChildNode = gos_xml_alloc_node_func(pstDoc);
    if (!pstChildNode)
    {
        return NULL;
    }

    pstChildNode->szName = gos_xml_strdup(pstDoc, szNodeName);
    if (!pstChildNode->szName)
    {
        goto end;
    }

    // 没有属性，结束
    gos_xml_trim_left(&szCurr, NULL);
    if (*szCurr == '\0')
    {
        bRet = TRUE;
        goto end;
    }

    // 获取属性列表
    while(1)
    {
        gos_xml_trim_left(&szCurr, NULL);
        if (*szCurr == '\0')
        {
            break;
        }

        if (!gos_xml_parse_attr(&szCurr, &szAttrName, &szAttrValue))
        {
            goto end;
        }

        // 添加到父节点
        pstAttr = gos_xml_alloc_attr_func(pstDoc);
        if (!pstAttr)
        {
            goto end;
        }

        memset(pstAttr, 0, sizeof(_GOS_XML_ATTR_T));
        pstAttr->szName = gos_xml_strdup(pstDoc, szAttrName);
        pstAttr->szValue = gos_xml_strdup(pstDoc, szAttrValue);

        if (!gos_xml_attr_append(pstChildNode, pstAttr))
        {
            gos_xml_free_attr_func(pstDoc, pstAttr);
            goto end;
        }
    }

    bRet = TRUE;

end:
    pstChildNode->pstDoc = pstDoc;

    if (!bRet)
    {
        gos_xml_free_node(pstChildNode);
        return NULL;
    }
    else
    {
        return pstChildNode;
    }
}

/*******************************************************************************
* 函数名称: gos_xml_parse_node
* 功    能: 解析某个节点
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              父节点
  szNode            IN/OUT          XML字符串(格式形如：aaa ddd=ff>...</aaa)
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_parse_node(_GOS_XML_NODE_T *pstNode, CHAR *szNode)
{
    CHAR    *szNodeName;
    CHAR    *szLastNodeName;
    CHAR    *szCurr;
    _GOS_XML_NODE_T  *pstChildNode;

    if (!gos_xml_trim(&szNode, NULL))
    {
        return FALSE;
    }

    // 判断是否存在</aaa后缀
    szLastNodeName = szNode+strlen(szNode)-1;
    for (; szLastNodeName>=szNode; szLastNodeName--)
    {
        if (szLastNodeName[0] == '<' &&
            szLastNodeName[1] == '/' )
        {
            *szLastNodeName = '\0';
            szLastNodeName += 2;
            gos_xml_trim(&szLastNodeName, NULL);
            break;
        }
    }

    if (GOS_XML_EMPTY_STR(szLastNodeName))
    {
        return FALSE;
    }

    // 获取name
    szNodeName = szNode;
    for (szCurr=szNode; *szCurr!='>'; szCurr++)
    {
        if (*szCurr == '\0')
        {
            return FALSE;
        }
    }

    *szCurr = '\0';
    gos_xml_trim(&szNodeName, NULL);

    pstChildNode = gos_xml_parse_attr_node(pstNode->pstDoc, szNodeName);
    if (!pstChildNode)
    {
        return FALSE;
    }

    if (!gos_xml_node_append(pstNode, pstChildNode))
    {
        return FALSE;
    }

    // 判断前后的节点名称是否一致
    szNodeName = pstChildNode->szName;
    if (strcmp(szNodeName, szLastNodeName) != 0)
    {
        return FALSE;
    }

    // Parse子节点
    szCurr++;
    while(1)
    {
        if (!gos_xml_parse_child_node(pstChildNode, &szCurr))
        {
            return FALSE;
        }

        gos_xml_trim_left(&szCurr, NULL);
        if (NULL == szCurr || *szCurr == '\0')
        {
            break;
        }
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_parse_child_node
* 功    能: 解析某个节点下的XML信息
* 参    数:
* 参数名称          输入/输出       描述
  pstNode           IN              父节点
  pszXML            IN/OUT          XML字符串
* 函数返回: 待解析的下一个子节点的XML字符串
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_parse_child_node(_GOS_XML_NODE_T *pstNode, CHAR **pszXML)
{
    CHAR    *szCurr;
    CHAR    *szNode;
    INT32   iLayer = 0;
    INT32   iLeft = 0;
    INT32   iRight = 0;
    INT32   iEnd = 0;
    INT32   iAttrLeft = 0;
    INT32   iAttrRight = 0;
    INT32   iAttrEnd = 0;
    CHAR    cLastMark = 0;
    BOOL    bString;
    _GOS_XML_NODE_T *pstChildNode = NULL;

    if (!pszXML)
    {
        return FALSE;
    }

    szCurr = *pszXML;
    if (NULL == szCurr)
    {
        return FALSE;
    }

    gos_xml_trim_left(&szCurr, NULL);
    if (*szCurr == '\0')
    {
        return TRUE;
    }

    if (*szCurr != '<')
    {
        if (*(szCurr+strlen(szCurr)-1) != '>')
        {
            if (pstNode->szValue)
            {
                return FALSE;
            }
            else
            {
                pstNode->szValue = gos_xml_strdup(pstNode->pstDoc, szCurr);
                *pszXML = NULL;

                return TRUE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    iLeft = 1;
    iLayer = 1;

    szCurr++;
    gos_xml_trim_left(&szCurr, NULL);
    szNode = szCurr;

    bString = FALSE;
    for(; *szCurr!='\0'; szCurr++)
    {
        if (*szCurr == '\"')
        {
            bString = !bString;
            continue;
        }

        if (bString)
        {
            continue;
        }

        if (*szCurr == '<')
        {
            iLeft ++;
            iLayer ++;

            cLastMark = *szCurr;
        }
        else if (*szCurr == '/')
        {
            //<aaa b=dfg/>
            if (szCurr[1] == '>')
            {
                iAttrEnd ++;
                if (iLeft == 1 && iRight == 0)
                {
                    *szCurr = '\0';
                    gos_xml_trim(&szNode, NULL);
                    pstChildNode = gos_xml_parse_attr_node(pstNode->pstDoc, szNode);
                    if (!pstChildNode)
                    {
                        return FALSE;
                    }

                    if (!gos_xml_node_append(pstNode, pstChildNode))
                    {
                        return FALSE;
                    }

                    *pszXML = szCurr+2;
                    return TRUE;
                }
                else
                {
                    iLeft --;
                    iRight --;
                    iAttrLeft ++;
                    iAttrRight ++;
                }
            }
            else
            {
                iEnd ++;
            }

            cLastMark = *szCurr;
        }
        else if (*szCurr == '>')
        {
            iRight ++;
            iLayer --;

            // mode <aaa> ... </aaa>
            if (iLayer == 0 && iEnd > 0 && cLastMark == '/' && iEnd*2 == iLeft)
            {
                *szCurr = '\0';
                *pszXML = szCurr+1;

                szCurr--;
                for (; szCurr>=szNode; szCurr--)
                {
                    if (gos_isspace(*szCurr))
                    {
                        *szCurr = '\0';
                    }
                    else
                    {
                        break;
                    }
                }

                if (szCurr <= szNode)
                {
                    return FALSE;
                }

                // szNode is aaa ddd=ff>...</aaa
                gos_xml_trim_left(&szNode, NULL);
                if (!gos_xml_parse_node(pstNode, szNode))
                {
                    return FALSE;
                }

                return TRUE;
            }

            cLastMark = *szCurr;
        }
    }

    if (bString)
    {
        return FALSE;
    }

    return FALSE;
}

/*******************************************************************************
* 函数名称: gos_xml_free_node
* 功    能: 释放节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID _gos_xml_free_node(_GOS_XML_NODE_T *pstNode)
{
    _GOS_XML_ATTR_T  *pstAttr;
    _GOS_XML_ATTR_T  *pstNextAttr;
    _GOS_XML_NODE_T  *pstChildNode;
    _GOS_XML_NODE_T  *pstNextChildNode;
    _GOS_XML_DOCUMENT_T *pstDoc;

    if (!pstNode)
    {
        return;
    }

    pstDoc = pstNode->pstDoc;
    gos_xml_strfree(pstDoc, pstNode->szName);
    gos_xml_strfree(pstDoc, pstNode->szValue);

    pstNode->szName = NULL;
    pstNode->szValue = NULL;

    // 释放属性
    for (pstAttr=pstNode->pstAttr; pstAttr!=NULL; )
    {
        pstNextAttr = pstAttr->pstSibling;
        gos_xml_free_attr_func(pstDoc, pstAttr);

        pstAttr = pstNextAttr;
    }

    pstNode->pstAttr = NULL;

    // 释放子节点
    for (pstChildNode=pstNode->pstFirstChild; pstChildNode!=NULL; )
    {
        pstNextChildNode = pstChildNode->pstSibling;

        gos_xml_free_node(pstChildNode);

        pstChildNode = pstNextChildNode;
    }

    pstNode->pstFirstChild = NULL;
    pstNode->pstLastChild = NULL;
}

VOID gos_xml_free_node(_GOS_XML_NODE_T *pstNode)
{
    _GOS_XML_DOCUMENT_T *pstDoc;

    if (!pstNode)
    {
        return;
    }

    pstDoc = pstNode->pstDoc;

    _gos_xml_free_node(pstNode);

    gos_xml_free_node_func(pstDoc, pstNode);
}

/*******************************************************************************
* 函数名称: gos_xml_replace_node
* 功    能: 替换节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_replace_node(_GOS_XML_NODE_T *pstNode, _GOS_XML_NODE_T *pstNewNode)
{
    _GOS_XML_DOCUMENT_T *pstDoc;
    CHAR                *szNodeName = NULL;
    CHAR                *szNodeValue = NULL;
    BOOL                bRet = FALSE;

    if (!pstNode || !pstNewNode || !pstNewNode->szName)
    {
        return FALSE;
    }

    pstDoc = pstNode->pstDoc;
    pstNewNode->pstSibling = pstNode->pstSibling;

    _gos_xml_free_node(pstNode);

    szNodeName = (CHAR*)strdup(pstNewNode->szName);
    if (!szNodeName)
    {
        goto end;
    }

    if (pstNewNode->szValue)
    {
        szNodeValue = (CHAR*)strdup(pstNewNode->szValue);
        if (!szNodeValue)
        {
            goto end;
        }
    }

    pstNode->szName = szNodeName;
    pstNode->szValue = szNodeValue;
    pstNode->pstAttr = pstNewNode->pstAttr;
    pstNode->pstFirstChild = pstNewNode->pstFirstChild;
    pstNode->pstLastChild = pstNewNode->pstLastChild;
    pstNode->pstSibling = pstNewNode->pstSibling;
    pstNode->pstDoc = pstDoc;

    pstNewNode->pstAttr = NULL;
    pstNewNode->pstFirstChild = NULL;
    pstNewNode->pstLastChild = NULL;
    pstNewNode->pstSibling = NULL;

    bRet = TRUE;

end:
    if (!bRet)
    {
        GOS_FREE(szNodeName);
        GOS_FREE(szNodeValue);
    }

    return bRet;
}

/******************************************************************************
* 函数名称: gos_xml_parse_root_node
* 功    能: 解析根节点
* 参    数:
* 参数名称          输入/输出       描述
  pszXML            IN/OUT          XML字符串
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_parse_root_node(_GOS_XML_DOCUMENT_T *pstDoc, CHAR **pszXML)
{
    BOOL    bRet = FALSE;
    CHAR    *szAttr = NULL;
    CHAR    *szCurr;
    CHAR    *szNodeName;
    UINT32  ulLen;
    _GOS_XML_NODE_T *pstChildNode = NULL;

    ulLen = (UINT32)(pstDoc->szXMLEnd - pstDoc->szXML);

    // like <aaa bb="cc"> </aaa>
    szCurr = *pszXML;
    if (*szCurr != '<' || ulLen <= 7 ||
        szCurr[ulLen-1] != '>' )
    {
        goto end;
    }

    ulLen -= 2;
    szCurr++;
    szCurr[ulLen] = '\0';

    // get node name
    for (szNodeName=szCurr+ulLen-2; szNodeName>szCurr; szNodeName--)
    {
        if (szNodeName[0] == '<' && szNodeName[1] == '/')
        {
            break;
        }
    }

    if (szNodeName <= szCurr)
    {
        goto end;
    }

    *szNodeName = '\0';
    szNodeName += 2;
    gos_xml_trim(&szNodeName, NULL);

    gos_xml_trim_left(&szCurr, NULL);

    if (!gos_str_like(szCurr, szNodeName))
    {
        goto end;
    }

#if 0    // remvoe by thomas for support attr at root node
    szCurr += strlen(szNodeName);
    gos_xml_trim_left(&szCurr, NULL);
    if (*szCurr != '>')
    {
        // goto end;
    }
#else
    // like aaa bb="cc" >
    gos_xml_trim_left(&szCurr, NULL);

    szAttr = szCurr;
    for (; *szCurr!='>'; szCurr++)
    {
        if (szCurr >= szNodeName)
        {
            goto end;
        }
    }

    *szCurr = '\0';
    gos_xml_trim(&szAttr, NULL);
    pstChildNode = gos_xml_parse_attr_node(pstDoc, szAttr);
    if (!pstChildNode)
    {
        goto end;
    }

    pstDoc->pstRootNode = pstChildNode;

#endif

    szCurr++;

    gos_xml_trim_left(&szCurr, NULL);
    *pszXML = szCurr;

    bRet = TRUE;

end:
    return bRet;
}

INT32 gos_xml_get_node_size(_GOS_XML_NODE_T *pstNode, INT32 iNewLine, INT32 iIndent)
{
    _GOS_XML_NODE_T *pstChildNode;
    _GOS_XML_ATTR_T *pstAttr;
    INT32           iSize = 0;
    INT32           iChildSize = 0;
    INT32           iAttrSize = 0;

    if (!pstNode || !pstNode->pstDoc || GOS_XML_EMPTY_STR(pstNode->szName))
    {
        return -1;
    }

    if (iNewLine)
    {
        iSize += iNewLine*2;
    }

    if (iIndent > 0)
    {
        iSize += (iIndent*4*2);
    }

    iSize += strlen("<></>") + strlen(pstNode->szName)*2;
    if (pstNode->szValue)
    {
        iSize += strlen(pstNode->szValue) + 2;
        if (iNewLine)
        {
            iSize += iNewLine;
        }

        if (iIndent > 0)
        {
            iSize += (iIndent*4);
        }
    }

    pstChildNode = pstNode->pstFirstChild;
    while(pstChildNode)
    {
        if (iIndent <= 0)
        {
            iIndent = -2;
        }

        iChildSize = gos_xml_get_node_size(pstChildNode, iNewLine, iIndent+1);
        if (iChildSize <= 0)
        {
            return -1;
        }

        iSize += iChildSize;
        pstChildNode = pstChildNode->pstSibling;
    }

    pstAttr = pstNode->pstAttr;
    while(pstAttr)
    {
        if (GOS_XML_EMPTY_STR(pstAttr->szName) ||
            GOS_XML_EMPTY_STR(pstAttr->szValue) )
        {
            return -1;
        }

        iAttrSize = strlen(pstAttr->szName) + strlen(pstAttr->szValue) + strlen(" =\"\"");
        iSize += iAttrSize;
        pstAttr = pstAttr->pstSibling;
    }

    return iSize;
}

INT32 gos_xml_get_doc_size(_GOS_XML_DOCUMENT_T *pstDoc, INT32 iNewLine, INT32 iIndent)
{
    INT32           iSize = 0;
    INT32           iNodeSize = 0;

    if (!pstDoc ||
        GOS_XML_EMPTY_STR(pstDoc->szVersion) ||
        GOS_XML_EMPTY_STR(pstDoc->szEncoding) ||
        !pstDoc->pstRootNode )
    {
        return -1;
    }

    // Declaration
    iSize += strlen("<? xml version=\"") + strlen(pstDoc->szVersion) +
             strlen("\" encoding=\"") + strlen(pstDoc->szEncoding) + strlen("\"?>");

    if (iNewLine)
    {
        iSize += iNewLine;
    }

    // node
    iNodeSize = gos_xml_get_node_size(pstDoc->pstRootNode, iNewLine, iIndent+1);
    if (iNodeSize <= 0)
    {
        return -1;
    }

    iSize += iNodeSize;

    return iSize;
}

