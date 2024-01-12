#include "gos_api.h"
#include "gos_func.h"
#include "gos_file.h"
#include "gos_xml_parse.h"
#include "gos_xml.h"

extern CHAR* gos_xml_strdup(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szText);
extern BOOL gos_xml_load_xml(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szXML);
extern BOOL gos_xml_parse_root_node(_GOS_XML_DOCUMENT_T *pstDoc, CHAR **pszXML);
extern BOOL gos_xml_parse_child_node(_GOS_XML_NODE_T *pstNode, CHAR **pszXML);
extern VOID gos_xml_free_node(_GOS_XML_NODE_T *pstNode);
extern BOOL gos_xml_trim(CHAR **pszXML, UINT32 *pulLen);
extern BOOL gos_xml_trim_left(CHAR **pszXML, UINT32 *pulLen);
extern VOID gos_xml_strfree(_GOS_XML_DOCUMENT_T *pstDoc, CHAR *szText);

extern _GOS_XML_NODE_T* gos_xml_alloc_node_func(_GOS_XML_DOCUMENT_T *pstDoc);
extern _GOS_XML_ATTR_T* gos_xml_alloc_attr_func(_GOS_XML_DOCUMENT_T *pstDoc);
extern BOOL gos_xml_node_append(_GOS_XML_NODE_T *pstNode, _GOS_XML_NODE_T* pstChildNode);
extern BOOL gos_xml_attr_append(_GOS_XML_NODE_T *pstNode, _GOS_XML_ATTR_T* pstAttr);
extern VOID gos_xml_free_node_func(_GOS_XML_DOCUMENT_T *pstDoc, _GOS_XML_NODE_T *pstNode);
extern VOID gos_xml_free_attr_func(_GOS_XML_DOCUMENT_T *pstDoc, _GOS_XML_ATTR_T *pstAttr);
extern INT32 gos_xml_get_doc_size(_GOS_XML_DOCUMENT_T *pstDoc, INT32 iNewLine, INT32 iIndent);
extern BOOL gos_xml_replace_node(_GOS_XML_NODE_T *pstNode, _GOS_XML_NODE_T *pstNewNode);
extern BOOL gos_xml_parse_child_node(_GOS_XML_NODE_T *pstNode, CHAR **pszXML);
extern BOOL gos_xml_parse_node(_GOS_XML_NODE_T *pstNode, CHAR *szNode);
/*******************************************************************************
* 函数名称: gos_xml_create_document
* 功    能: 创建XML Document
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
GOS_XML_DOCUMENT_T* gos_xml_create_document(UINT32 ulStaticNodeNum)
{
    _GOS_XML_DOCUMENT_T *pstDoc;
    UINT32              ulSize;
    UINT32              ulStaticAttrNum;
    UINT32              ulStaticXMLLen;

    if (ulStaticNodeNum < 16)
    {
        ulStaticNodeNum = 16;
    }

    ulStaticAttrNum = ulStaticNodeNum*4;
    ulStaticXMLLen = ulStaticNodeNum*8;
    if (ulStaticXMLLen < 1024)
    {
        ulStaticXMLLen = 1024;
    }

    ulSize = sizeof(_GOS_XML_DOCUMENT_T) +
             sizeof(_GOS_XML_NODE_T) * ulStaticNodeNum +
             sizeof(_GOS_XML_ATTR_T) * ulStaticAttrNum +
             ulStaticXMLLen;

    pstDoc = (_GOS_XML_DOCUMENT_T*)GOS_ZALLOC(ulSize);
    if (!pstDoc)
    {
        return NULL;
    }

    pstDoc->ulStaticNodeNum = ulStaticNodeNum;
    pstDoc->ulStaticAttrNum = ulStaticAttrNum;
    pstDoc->ulStaticXMLLen = ulStaticXMLLen;

    pstDoc->pstNodeList = (_GOS_XML_NODE_T*)(pstDoc+1);
    pstDoc->pstAttrList = (_GOS_XML_ATTR_T*)(pstDoc->pstNodeList + ulStaticNodeNum);
    pstDoc->szStaticXML = (CHAR*)(pstDoc->pstAttrList + ulStaticAttrNum);

    return (GOS_XML_DOCUMENT_T*)pstDoc;
}

/*******************************************************************************
* 函数名称: gos_xml_parse_string
* 功    能: 解析字符串格式的XML信息
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_parse_string(GOS_XML_DOCUMENT_T *pDoc, CHAR *szXML, BOOL bRemoveComment)
{
    _GOS_XML_DOCUMENT_T *pstDoc = NULL;
    INT32               i;

    if (GOS_XML_EMPTY_STR(szXML))
    {
        return FALSE;
    }

    if (!pDoc)
    {
        return FALSE;
    }

    pstDoc = (_GOS_XML_DOCUMENT_T*)pDoc;
    gos_xml_free_document((GOS_XML_DOCUMENT_T*)pstDoc);

    pstDoc->szXML = NULL;
    pstDoc->szXMLEnd = NULL;
    pstDoc->szVersion = NULL;
    pstDoc->szEncoding = NULL;
    pstDoc->pstRootNode = NULL;

    memset(&pstDoc->stNodeDummy, 0, sizeof(pstDoc->stNodeDummy));
    memset(&pstDoc->stAttrDummy, 0, sizeof(pstDoc->stAttrDummy));

    pstDoc->bParsed = FALSE;
    pstDoc->bRemoveComment = bRemoveComment;

    // 设置管理队列
    for (i=0; i<(INT32)pstDoc->ulStaticNodeNum-1; i++)
    {
        pstDoc->pstNodeList[i].pstNext = &pstDoc->pstNodeList[i+1];
        pstDoc->pstNodeList[i+1].pstPrev = &pstDoc->pstNodeList[i];
    }

    pstDoc->stNodeDummy.pstNext = &pstDoc->pstNodeList[0];
    pstDoc->pstNodeList[0].pstPrev = &pstDoc->stNodeDummy;
    pstDoc->pstNodeList[pstDoc->ulStaticNodeNum-1].pstNext = &pstDoc->stNodeDummy;
    pstDoc->stNodeDummy.pstPrev = &pstDoc->pstNodeList[pstDoc->ulStaticNodeNum-1];

    for (i=0; i<(INT32)pstDoc->ulStaticAttrNum-1; i++)
    {
        pstDoc->pstAttrList[i].pstNext = &pstDoc->pstAttrList[i+1];
        pstDoc->pstAttrList[i+1].pstPrev = &pstDoc->pstAttrList[i];
    }

    pstDoc->stAttrDummy.pstNext = &pstDoc->pstAttrList[0];
    pstDoc->pstAttrList[0].pstPrev = &pstDoc->stAttrDummy;
    pstDoc->pstAttrList[pstDoc->ulStaticAttrNum-1].pstNext = &pstDoc->stAttrDummy;
    pstDoc->stAttrDummy.pstPrev = &pstDoc->pstAttrList[pstDoc->ulStaticAttrNum-1];

    // 加载并预处理XML字符串
    if (!gos_xml_load_xml(pstDoc, szXML))
    {
        goto end;
    }

    // 解析根节点
    szXML = pstDoc->szXML;
    if (!gos_xml_parse_root_node(pstDoc, &szXML))
    {
        goto end;
    }

    gos_xml_trim(&szXML, NULL);

    // Parse子节点
    while(1)
    {
        if (!gos_xml_parse_child_node(pstDoc->pstRootNode, &szXML))
        {
            goto end;
        }

        gos_xml_trim_left(&szXML, NULL);
        if (GOS_XML_EMPTY_STR(szXML))
        {
            break;
        }
    }

    pstDoc->bParsed = TRUE;
    return TRUE;

end:
    gos_xml_free_document((GOS_XML_DOCUMENT_T*)pstDoc);

    return FALSE;
}

/*******************************************************************************
* 函数名称: gos_xml_parse_file
* 功    能: 解析XML文件
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_parse_file(GOS_XML_DOCUMENT_T *pDoc, CHAR *szFile, BOOL bRemoveComment)
{
    CHAR    *szXML;
    HANDLE  hFile = NULL;
    BOOL    bRet = FALSE;
    UINT32  ulFileLen;

    if (GOS_XML_EMPTY_STR(szFile))
    {
        return FALSE;
    }

    hFile = gos_fopen(szFile, "rb");
    if (!hFile)
    {
        return FALSE;
    }

    ulFileLen = gos_get_fsize(hFile);

    szXML = (CHAR*)GOS_MALLOC(ulFileLen+1);
    if (!szXML)
    {
        goto end;
    }

    if ((INT32)ulFileLen != gos_fread(hFile, szXML, ulFileLen))
    {
        goto end;
    }

    *(szXML+ulFileLen) = '\0';

    bRet = gos_xml_parse_string(pDoc, szXML, bRemoveComment);

end:
    gos_fclose(hFile);

    GOS_FREE(szXML);

    return bRet;
}

/*******************************************************************************
* 函数名称: gos_xml_free_document
* 功    能: 释放XML Document
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID gos_xml_free_document(GOS_XML_DOCUMENT_T *pDoc)
{
    _GOS_XML_DOCUMENT_T *pstDoc = (_GOS_XML_DOCUMENT_T *)pDoc;

    if (!pstDoc)
    {
        return;
    }

    gos_xml_free_node(pstDoc->pstRootNode);
    pstDoc->pstRootNode = NULL;

    if (pstDoc->szXML && pstDoc->szXML != pstDoc->szStaticXML)
    {
        GOS_FREE(pstDoc->szXML);
    }

    GOS_FREE(pstDoc->szVersion);
    GOS_FREE(pstDoc->szEncoding);
}


/*******************************************************************************
* 函数名称: gos_xml_destroy_document
* 功    能: 删除XML Document
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID gos_xml_destroy_document(GOS_XML_DOCUMENT_T *pDoc)
{
    gos_xml_free_document(pDoc);
    GOS_FREE(pDoc);
}

/*******************************************************************************
* 函数名称: gos_xml_get_root_node
* 功    能: 获取根节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
GOS_XML_NODE_T* gos_xml_get_root_node(GOS_XML_DOCUMENT_T *pDoc)
{
    _GOS_XML_DOCUMENT_T *pstDoc = (_GOS_XML_DOCUMENT_T *)pDoc;

    if (!pstDoc)
    {
        return NULL;
    }

    return (GOS_XML_NODE_T*)pstDoc->pstRootNode;
}

/*******************************************************************************
* 函数名称: gos_xml_get_first_child
* 功    能: 获取第一个子节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
GOS_XML_NODE_T* gos_xml_get_first_child(GOS_XML_NODE_T *pNode, CHAR *szNodeName)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T *)pNode;
    _GOS_XML_NODE_T *pstChild;

    if (!pstNode)
    {
        return NULL;
    }

    if (!szNodeName)
    {
        return (GOS_XML_NODE_T*)pstNode->pstFirstChild;
    }

    for (pstChild=pstNode->pstFirstChild; pstChild; pstChild=pstChild->pstSibling)
    {
        if (strcmp(szNodeName, pstChild->szName) == 0)
        {
            return (GOS_XML_NODE_T*)pstChild;
        }
    }

    return NULL;
}

/*******************************************************************************
* 函数名称: gos_xml_get_next
* 功    能: 获取下一个节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
GOS_XML_NODE_T* gos_xml_get_next(GOS_XML_NODE_T *pNode)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T*)pNode;

    if (!pstNode)
    {
        return NULL;
    }

    return (GOS_XML_NODE_T*)pstNode->pstSibling;
}

/*******************************************************************************
* 函数名称: gos_xml_get_child_number
* 功    能: 获取子节点个数
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
UINT32 gos_xml_get_children_number(GOS_XML_NODE_T *pNode, CHAR *szNodeName)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T *)pNode;
    _GOS_XML_NODE_T *pstChild;
    UINT32          ulNum = 0;

    if (!pstNode)
    {
        return 0;
    }

    if (!szNodeName)
    {
        return 0;
    }

    for (pstChild=pstNode->pstFirstChild; pstChild; pstChild=pstChild->pstSibling)
    {
        if (strcmp(szNodeName, pstChild->szName) == 0)
        {
            ulNum ++;
        }
    }

    return ulNum;
}

/*******************************************************************************
* 函数名称: gos_xml_get_sibling
* 功    能: 获取兄弟节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
GOS_XML_NODE_T* gos_xml_get_sibling(GOS_XML_NODE_T *pNode)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T*)pNode;

    if (!pstNode)
    {
        return NULL;
    }

    if (!pstNode->pstSibling)
    {
        return NULL;
    }

    if (strcmp(pstNode->pstSibling->szName, pstNode->szName) == 0)
    {
        return (GOS_XML_NODE_T*)pstNode->pstSibling;
    }
    else
    {
        return NULL;
    }
}

/*******************************************************************************
* 函数名称: gos_xml_get_node_name
* 功    能: 获取节点名称
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR* gos_xml_get_node_name(GOS_XML_NODE_T *pNode)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T*)pNode;

    if (!pstNode)
    {
        return NULL;
    }

    return pstNode->szName;
}

/*******************************************************************************
* 函数名称: gos_xml_get_node_value
* 功    能: 获取节点值
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR* gos_xml_get_node_value(GOS_XML_NODE_T *pNode)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T*)pNode;

    if (!pstNode)
    {
        return NULL;
    }

    return pstNode->szValue;
}

/*******************************************************************************
* 函数名称: gos_xml_get_node_int_value
* 功    能: 获取节点值(整数)
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_get_node_int_value(GOS_XML_NODE_T *pNode, INT32 *piValue)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T*)pNode;

    if (!pstNode || !piValue)
    {
        return FALSE;
    }

    return gos_atoi(pstNode->szValue, piValue);
}

/*******************************************************************************
* 函数名称: gos_xml_get_childnode_value
* 功    能: 获取子节点的值
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR* gos_xml_get_childnode_value(GOS_XML_NODE_T *pNode, CHAR *szChildNodeName)
{
    GOS_XML_NODE_T  *pstFirstChildNode;

    pstFirstChildNode = gos_xml_get_first_child(pNode, szChildNodeName);
    if (pstFirstChildNode)
    {
        return gos_xml_get_node_value(pstFirstChildNode);
    }

    return NULL;
}

/*******************************************************************************
* 函数名称: gos_xml_get_childnode_int_value
* 功    能: 获取子节点的值
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_get_childnode_int_value(GOS_XML_NODE_T *pNode, CHAR *szChildNodeName, INT32 *piValue)
{
    GOS_XML_NODE_T  *pstFirstChildNode;
    CHAR            *szValue = NULL;

    pstFirstChildNode = gos_xml_get_first_child(pNode, szChildNodeName);
    if (pstFirstChildNode)
    {
        szValue = gos_xml_get_node_value(pstFirstChildNode);
        if (szValue)
        {
            return gos_atoi(szValue, piValue);
        }
    }

    return FALSE;
}

/*******************************************************************************
* 函数名称: gos_xml_get_attr
* 功    能: 获取节点属性值
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR* gos_xml_get_attr_value(GOS_XML_NODE_T *pNode, const CHAR *szAttrName)
{
    _GOS_XML_NODE_T *pstNode = (_GOS_XML_NODE_T*)pNode;
    _GOS_XML_ATTR_T *pstAttr;

    if (!pstNode)
    {
        return NULL;
    }

    for (pstAttr=pstNode->pstAttr; pstAttr; pstAttr=pstAttr->pstSibling)
    {
        if (strcmp(szAttrName, pstAttr->szName) == 0)
        {
            return pstAttr->szValue;
        }
    }

    return NULL;
}

/*******************************************************************************
* 函数名称: gos_xml_get_attr_int_value
* 功    能: 获取节点属性值(整数)
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_get_attr_int_value(GOS_XML_NODE_T *pNode, const CHAR *szAttrName, INT32 *piValue)
{
    CHAR *szValue;

    if (!piValue)
    {
        return FALSE;
    }

    szValue = gos_xml_get_attr_value(pNode, szAttrName);
    if (GOS_XML_EMPTY_STR(szValue))
    {
        return FALSE;
    }

    return gos_atoi(szValue, piValue);
}

/*******************************************************************************
* 函数名称: gos_xml_get_attr_int64_value
* 功    能: 获取节点属性值(整数)
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_get_attr_int64_value(GOS_XML_NODE_T *pNode, const CHAR *szAttrName, INT64 *pi64Value)
{
    CHAR *szValue;

    if (!pi64Value)
    {
        return FALSE;
    }

    szValue = gos_xml_get_attr_value(pNode, szAttrName);
    if (GOS_XML_EMPTY_STR(szValue))
    {
        return FALSE;
    }

    return gos_atoi64(szValue, pi64Value);
}

/*******************************************************************************
* 函数名称: gos_xml_get_attr_float_value
* 功    能: 获取节点属性值(FLOAT)
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_get_attr_float_value(GOS_XML_NODE_T *pNode, const CHAR *szAttrName, FLOAT *pfValue)
{
    DOUBLE  dValue;

    if (!gos_xml_get_attr_double_value(pNode, szAttrName, &dValue))
    {
        return FALSE;
    }

    pfValue[0] = (FLOAT)dValue;
    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_get_attr_double_value
* 功    能: 获取节点属性值(DOUBLE)
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_get_attr_double_value(GOS_XML_NODE_T *pNode, const CHAR *szAttrName, DOUBLE *pdValue)
{
    CHAR *szValue;

    if (!pdValue)
    {
        return FALSE;
    }

    szValue = gos_xml_get_attr_value(pNode, szAttrName);
    if (GOS_XML_EMPTY_STR(szValue))
    {
        return FALSE;
    }

    return gos_atof(szValue, pdValue);
}

/*******************************************************************************
* 函数名称: gos_xml_set_root_node
* 功    能: 设置根节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_set_root_node(GOS_XML_DOCUMENT_T *pDoc, CHAR *szNodeName)
{
    _GOS_XML_DOCUMENT_T  *pstDoc = (_GOS_XML_DOCUMENT_T*)pDoc;

    if (!pstDoc || GOS_XML_EMPTY_STR(szNodeName))
    {
        return FALSE;
    }

    // 设置为允许读写模式
    pstDoc->bParsed = TRUE;

    if (!pstDoc->pstRootNode)
    {
        pstDoc->pstRootNode = gos_xml_alloc_node_func(pstDoc);
        if (!pstDoc->pstRootNode)
        {
            return FALSE;
        }
    }
    else
    {
        gos_xml_strfree(pstDoc, szNodeName);
    }

    memset(pstDoc->pstRootNode, 0, sizeof(_GOS_XML_NODE_T));
    pstDoc->pstRootNode->pstDoc = pstDoc;
    pstDoc->pstRootNode->szName = gos_xml_strdup(pstDoc, szNodeName);

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_set_root_node
* 功    能: 设置根节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
GOS_XML_NODE_T* gos_xml_add_node(GOS_XML_NODE_T *pNode, const CHAR *szChildName, CHAR *szChildValue)
{
    _GOS_XML_NODE_T     *pstNode = (_GOS_XML_NODE_T *)pNode;
    _GOS_XML_NODE_T     *pstChildNode;
    CHAR                *szNodeName = NULL;
    CHAR                *szNodeValue = NULL;

    if(!pstNode)
    {
        return NULL;
    }

    if(!pstNode->pstDoc)
    {
        return NULL;
    }

    if(GOS_XML_EMPTY_STR(szChildName))
    {
        return NULL;
    }

    szNodeName = gos_xml_strdup(pstNode->pstDoc, (CHAR*)szChildName);
    if (!szNodeName)
    {
        return NULL;
    }

    if (!GOS_XML_EMPTY_STR(szChildValue))
    {
        szNodeValue = gos_xml_strdup(pstNode->pstDoc, szChildValue);
        if (!szNodeValue)
        {
            gos_xml_strfree(pstNode->pstDoc, szNodeName);
            return NULL;
        }
    }

    pstChildNode = gos_xml_alloc_node_func(pstNode->pstDoc);
    if (!pstChildNode)
    {
        gos_xml_strfree(pstNode->pstDoc, szNodeName);
        gos_xml_strfree(pstNode->pstDoc, szNodeValue);
        return NULL;
    }

    pstChildNode->szName = szNodeName;
    pstChildNode->szValue = szNodeValue;
    if (!gos_xml_node_append(pstNode, pstChildNode))
    {
        gos_xml_free_node_func(pstNode->pstDoc, pstChildNode);
        return NULL;
    }

    return (GOS_XML_NODE_T*)pstChildNode;
}

/*******************************************************************************
* 函数名称: gos_xml_del_node
* 功    能: 删除节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_del_node(GOS_XML_NODE_T *pNode, GOS_XML_NODE_T *pChildNode)
{
    _GOS_XML_NODE_T     *pstNode = (_GOS_XML_NODE_T *)pNode;
    _GOS_XML_NODE_T     *pstChildNode = NULL;
    _GOS_XML_NODE_T     *pstPreChild = NULL;
    BOOL                bRet = FALSE;

    if (!pNode || !pChildNode)
    {
        goto end;
    }

    // 找到节点位置
    for (pstChildNode=pstNode->pstFirstChild; pstChildNode; pstChildNode=pstChildNode->pstSibling)
    {
        if (pstChildNode == (_GOS_XML_NODE_T *)pChildNode)
        {
            bRet = TRUE;
            break;
        }

        pstPreChild = pstChildNode;
    }

    if (!bRet)
    {
        goto end;
    }

    if (pstChildNode == pstNode->pstFirstChild) // 第一个
    {
        pstNode->pstFirstChild = pstChildNode->pstSibling;
    }

    if (pstChildNode == pstNode->pstLastChild) // 最后一个
    {
        pstNode->pstLastChild = pstPreChild;
    }

    if (pstPreChild)
    {
        pstPreChild->pstSibling = pstChildNode->pstSibling;
    }

end:
    if (bRet)
    {
        gos_xml_free_node(pstChildNode);
    }

    return bRet;
}

/*******************************************************************************
* 函数名称: gos_xml_add_attr
* 功    能: 增加属性
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_add_attr(GOS_XML_NODE_T *pNode, const CHAR *szAttrName, CHAR *szAttrValue)
{
    _GOS_XML_NODE_T     *pstNode = (_GOS_XML_NODE_T *)pNode;
    _GOS_XML_ATTR_T     *pstAttr;

    if (!pstNode || pstNode->pstDoc ||
        GOS_XML_EMPTY_STR(szAttrName) ||
        GOS_XML_EMPTY_STR(szAttrValue) )
    {
        return FALSE;
    }

    pstAttr = gos_xml_alloc_attr_func(pstNode->pstDoc);
    if (!pstAttr)
    {
        return FALSE;
    }

    pstAttr->szName = gos_xml_strdup(pstNode->pstDoc, (CHAR*)szAttrName);
    pstAttr->szValue = gos_xml_strdup(pstNode->pstDoc, szAttrValue);

    if (!gos_xml_attr_append(pstNode, pstAttr))
    {
        gos_xml_free_attr_func(pstNode->pstDoc, pstAttr);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* 函数名称: gos_xml_set_declaration
* 功    能: 设置XML文件头声明
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_set_declaration(GOS_XML_DOCUMENT_T *pDoc, CHAR *szVersion, CHAR *szEncoding)
{
    _GOS_XML_DOCUMENT_T  *pstDoc = (_GOS_XML_DOCUMENT_T*)pDoc;

    if (!pstDoc ||
        GOS_XML_EMPTY_STR(szVersion) ||
        GOS_XML_EMPTY_STR(szEncoding) )
    {
        return FALSE;
    }

    // 设置为允许读写模式
    pstDoc->bParsed = TRUE;

    pstDoc->szVersion = gos_xml_strdup(pstDoc, szVersion);
    if (!pstDoc->szVersion)
    {
        return FALSE;
    }

    pstDoc->szEncoding = gos_xml_strdup(pstDoc, szEncoding);
    if (!pstDoc->szEncoding)
    {
        gos_xml_strfree(pstDoc, pstDoc->szVersion);
        return FALSE;
    }

    return TRUE;
}

INT32 gos_xml_add_indent(CHAR *szText, INT32 iIndent)
{
    INT32   iSize;

    if (iIndent <= 0)
    {
        return 0;
    }

    iSize = sprintf(szText, "%*s", iIndent*4, "");
    return iSize;
}

INT32 _gos_xml_get_node_text(CHAR *szText, _GOS_XML_NODE_T* pstNode, CHAR *szNewLine, INT32 iIndent)
{
    INT32           iSize = 0;
    INT32           iChildSize = 0;
    _GOS_XML_NODE_T *pstChildNode;
    _GOS_XML_ATTR_T *pstAttr;
    BOOL            bChild;

    if (!pstNode)
    {
        return -1;
    }

    bChild = (pstNode->pstFirstChild || pstNode->szValue);

    iSize += gos_xml_add_indent(szText, iIndent);
    iSize += sprintf(szText+iSize, "<%s", pstNode->szName);

    if (pstNode->pstAttr)
    {
        for (pstAttr=pstNode->pstAttr; pstAttr; pstAttr=pstAttr->pstSibling)
        {
            iSize += sprintf(szText+iSize, " %s=\"%s\"", pstAttr->szName, pstAttr->szValue);
        }

        if (!bChild)
        {
            iSize += sprintf(szText+iSize, "/>");
            if (szNewLine)
            {
                iSize += sprintf(szText+iSize, szNewLine);
            }

            return iSize;
        }
    }

    if (bChild)
    {
        iSize += sprintf(szText+iSize, ">");
        if (pstNode->pstFirstChild && szNewLine)
        {
            iSize += sprintf(szText+iSize, szNewLine);
        }
    }

    if (pstNode->szValue)
    {
        if (pstNode->pstFirstChild)
        {
            iSize += gos_xml_add_indent(szText+iSize, iIndent+1);
        }

        iSize += sprintf(szText+iSize, "\"%s\"", pstNode->szValue);

        if (pstNode->pstFirstChild && szNewLine)
        {
            iSize += sprintf(szText+iSize, szNewLine);
        }
    }

    for (pstChildNode=pstNode->pstFirstChild; pstChildNode; pstChildNode=pstChildNode->pstSibling)
    {
        iChildSize = _gos_xml_get_node_text(szText+iSize, pstChildNode, szNewLine, iIndent+1);
        if (iChildSize <= 0)
        {
            return -1;
        }

        iSize += iChildSize;
    }

    if (bChild)
    {
        if (pstNode->pstFirstChild)
        {
            iSize += gos_xml_add_indent(szText+iSize, iIndent);
        }

        iSize += sprintf(szText+iSize, "</%s>", pstNode->szName);
        if (szNewLine)
        {
            iSize += sprintf(szText+iSize, szNewLine);
        }
    }

    return iSize;
}

/*******************************************************************************
* 函数名称: gos_xml_get_doc_text
* 功    能: 获取XML文档全文
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR *gos_xml_get_doc_text(GOS_XML_DOCUMENT_T *pDoc, BOOL bNewLine)
{
    _GOS_XML_DOCUMENT_T *pstDoc = (_GOS_XML_DOCUMENT_T*)pDoc;
    INT32               iSize = -1;
    INT32               i = 0;
    CHAR                *szText = NULL;
    INT32               iIndent = 0;
    CHAR                *szNewLine = (CHAR*)"";

    if (!pstDoc)
    {
        return FALSE;
    }

    if (bNewLine)
    {
        iIndent = 0;
    }
    else
    {
        iIndent = -2;
    }

    if (bNewLine == GOS_XML_NEWLINE_FILE)
    {
#ifdef _OSWIN32_
        szNewLine = (CHAR*)"\r\n";
#else
        szNewLine = (CHAR*)"\n";
#endif
    }
    else if (bNewLine)
    {
        szNewLine = (CHAR*)"\r\n";
    }

    iSize = gos_xml_get_doc_size(pstDoc, strlen(szNewLine), iIndent);
    if (iSize <= 0)
    {
        return NULL;
    }

    szText = (CHAR*)GOS_MALLOC(iSize+1);
    if (!szText)
    {
        return NULL;
    }

    // declaration
    i = sprintf(szText, "<?xml version=\"%s\" encoding=\"%s\"?>", pstDoc->szVersion, pstDoc->szEncoding);
    if (szNewLine)
    {
        i += sprintf(szText+i, szNewLine);
    }

    // node
    iSize = _gos_xml_get_node_text(szText+i, pstDoc->pstRootNode, szNewLine, iIndent);
    if (iSize <= 0)
    {
        GOS_FREE(szText);
        return NULL;
    }

    return szText;
}

/*******************************************************************************
* 函数名称: gos_xml_get_node_text
* 功    能: 获取XML某节点内容
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
CHAR *gos_xml_get_node_text(GOS_XML_DOCUMENT_T *pDoc, GOS_XML_NODE_T *pNode, BOOL bNewLine)
{
    _GOS_XML_DOCUMENT_T *pstDoc = (_GOS_XML_DOCUMENT_T*)pDoc;
    _GOS_XML_NODE_T     *pstNode = (_GOS_XML_NODE_T*)pNode;
    INT32               iSize = 0;
    CHAR                *szText = NULL;
    INT32               iIndent = 0;
    CHAR                *szNewLine = (CHAR*)"";

    if (!pstDoc)
    {
        return FALSE;
    }

    if (bNewLine)
    {
        iIndent = 0;
    }
    else
    {
        iIndent = -2;
    }

    if (bNewLine == GOS_XML_NEWLINE_FILE)
    {
#ifdef _OSWIN32_
        szNewLine = (CHAR*)"\r\n";
#else
        szNewLine = (CHAR*)"\n";
#endif
    }
    else if (bNewLine)
    {
        szNewLine = (CHAR*)"\r\n";
    }

    iSize = gos_xml_get_doc_size(pstDoc, strlen(szNewLine), iIndent);
    if (iSize <= 0)
    {
        return NULL;
    }

    szText = (CHAR*)GOS_MALLOC(iSize+1);
    if (!szText)
    {
        return NULL;
    }

    // node
    iSize = _gos_xml_get_node_text(szText, pstNode, szNewLine, iIndent);
    if (iSize <= 0)
    {
        GOS_FREE(szText);
        return NULL;
    }

    return szText;
}

/*******************************************************************************
* 函数名称: gos_xml_set_node_text
* 功    能: 设置XML某节点内容
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_set_node_text(GOS_XML_DOCUMENT_T *pDoc, GOS_XML_NODE_T *pNode, CHAR *szText)
{
    _GOS_XML_DOCUMENT_T *pstDoc = (_GOS_XML_DOCUMENT_T*)pDoc;
    _GOS_XML_NODE_T     *pstNode = (_GOS_XML_NODE_T*)pNode;
    _GOS_XML_NODE_T     *pstNewNode = gos_xml_alloc_node_func(pstDoc);
    BOOL                bRet;
    UINT32              ulLen;

    if (!pstNewNode || !pstNode || !szText || *szText == '\0')
    {
        return FALSE;
    }

    if (*szText == '<')
    {
        szText++;
    }

    gos_trim_string(szText);
    ulLen = strlen(szText);
    if (ulLen <= 1)
    {
        gos_xml_free_node(pstNewNode);
        return FALSE;
    }

    if (szText[ulLen-1] == '>')
    {
        szText[ulLen-1] = '\0';
    }

    if (!gos_xml_parse_node(pstNewNode, szText))
    {
        gos_xml_free_node(pstNewNode);
        return FALSE;
    }

    bRet = gos_xml_replace_node(pstNode, pstNewNode->pstFirstChild);

    gos_xml_free_node(pstNewNode);

    return bRet;
}

/*******************************************************************************
* 函数名称: gos_xml_clear_child_node
* 功    能: 清除XML某节点的所有子节点
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
VOID gos_xml_clear_child_node(GOS_XML_NODE_T *pNode)
{
    _GOS_XML_NODE_T     *pstNode = (_GOS_XML_NODE_T*)pNode;
    _GOS_XML_NODE_T     *pstChildNode;
    _GOS_XML_NODE_T     *pstNextChildNode;

    if (!pstNode)
    {
        return;
    }

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

/*******************************************************************************
* 函数名称: gos_xml_save_doc
* 功    能: 保存XML文档全文
* 参    数:
* 参数名称          输入/输出         描述
*
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL gos_xml_save_doc(GOS_XML_DOCUMENT_T *pDoc, CHAR *szFileName, BOOL bNewLine)
{
    HANDLE      hFile = NULL;
    CHAR        *szFile = NULL;

    if (bNewLine)
    {
        bNewLine = GOS_XML_NEWLINE_FILE;
    }

    szFile = gos_xml_get_doc_text(pDoc, bNewLine);
    if (!szFile)
    {
        return FALSE;
    }

    hFile = gos_fopen(szFileName, "w+b");
    if (!hFile)
    {
        GOS_FREE(szFile);
        return FALSE;
    }

    gos_fwrite(hFile, szFile, strlen(szFile));
    gos_fclose(hFile);

    GOS_FREE(szFile);

    return TRUE;
}

