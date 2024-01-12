#include "gos_api.h"
#include "gos_xml.h"
#include "g_xml.h"

class GXmlDocImp;

class GXmlNodeImp : public GXmlNode
{
    friend class GXmlDocImp;

private:
    GXmlDocImp*     m_pDoc;
    GOS_XML_NODE_T* m_pstNode;

    GXmlNodeImp*    m_pFirstChild;
    GXmlNodeImp*    m_pLastChild;
    GXmlNodeImp*    m_pNextNode;

    GXmlNodeImp(VOID);
    ~GXmlNodeImp(VOID);

    BOOL            GenChildTree(VOID);
    BOOL            LinkChild(GXmlNodeImp *pNode);

public:
    VOID            Free(VOID);
    GXmlNode*       GetFirstChild(const CHAR *szNodeName=NULL);
    GXmlNode*       GetNext(VOID);
    GXmlNode*       GetSibling(VOID);
    CHAR*           GetName(VOID);
    CHAR*           GetValue(VOID);
    BOOL            GetValue(INT32 *piValue);
    CHAR*           GetAttrValue(const CHAR *szAttrName);
    BOOL            GetAttrValue(const CHAR *szAttrName, CHAR *szValue, UINT32 ulMaxLen);
    BOOL            GetAttrValue(const CHAR *szAttrName, UINT32 *pulValue);
    BOOL            GetAttrValue(const CHAR *szAttrName, INT32 *piValue);
    BOOL            GetAttrValue(const CHAR *szAttrName, INT64 *pi64Value);
    BOOL            GetAttrValue(const CHAR *szAttrName, UINT16 *pusValue);
    BOOL            GetAttrValue(const CHAR *szAttrName, UINT8 *pucValue);
    BOOL            GetAttrValue(const CHAR *szAttrName, FLOAT *pfValue);
    BOOL            GetAttrValue(const CHAR *szAttrName, DOUBLE *pdValue);

    CHAR*           GetChildValue(const CHAR *szChildName);
    VOID            GetChildValue(const CHAR *szChildName, VECTOR<CHAR*> &vChildValue);

    BOOL            AddChild(const CHAR *szChildName, CHAR *szChildValue=NULL);
    BOOL            AddChild(const CHAR *szChildName, INT32 iChildValue);
    BOOL            AddAttr(const CHAR *szAttrName, CHAR *szAttrValue);
    BOOL            AddAttr(const CHAR *szAttrName, INT32 iAttrValue);
    BOOL            DelChild(const CHAR *szChildName,CHAR *szChildValue);

    CHAR*           GetText(BOOL bNewLine=FALSE);
    BOOL            SetText(CHAR *szText);
    VOID            ClearChild();
};

class GXmlDocImp : public GXmlDoc
{
    friend class GXmlNodeImp;

private:
    GOS_XML_DOCUMENT_T* m_pstDoc;
    GXmlNodeImp*        m_pRootNode;

    UINT32              m_ulFreeNodeIndex;

    UINT32              m_ulStaticNodeNum;
    GXmlNodeImp         *m_pStaticNode;

    GXmlNodeImp*        NewNode(GOS_XML_NODE_T *pstDoc);
    VOID                DeleteNode(GXmlNodeImp *pNode);

    BOOL                GenNodeTree(VOID);

public:
    GXmlDocImp(UINT32 ulStaticNodeNum=64);
    ~GXmlDocImp(VOID);

    BOOL                ParseString(CHAR *szXML, BOOL bRemoveComment=TRUE);
    BOOL                ParseFile(CHAR *szFile, BOOL bRemoveComment=TRUE);
    CHAR*               GetText(BOOL bNewLine=FALSE);
    BOOL                SaveToFile(CHAR *szFileName, BOOL bNewLine=FALSE);
    GXmlNode*           GetRootNode(VOID);

    BOOL                SetDeclaration(CHAR *szVersion=(CHAR*)"1.0", CHAR *szEncoding=(CHAR*)"utf-8");
    BOOL                SetRootNode(CHAR *szNodeName);

    BOOL                Validate(VOID);
    VOID                Free(VOID);
};

GXmlNodeImp::GXmlNodeImp(VOID)
{
    m_pDoc = NULL;
    m_pstNode = NULL;

    m_pFirstChild = NULL;
    m_pLastChild = NULL;
    m_pNextNode = NULL;
}

GXmlNodeImp::~GXmlNodeImp(VOID)
{
    Free();
}

VOID GXmlNodeImp::Free(VOID)
{
    GXmlNodeImp* pChild = m_pFirstChild;
    GXmlNodeImp* pNextChild;

    while(pChild)
    {
        pNextChild = pChild->m_pNextNode;

        if (m_pDoc)
        {
            m_pDoc->DeleteNode(pChild);
        }

        pChild = pNextChild;
    }

    m_pDoc = NULL;
    m_pstNode = NULL;

    m_pFirstChild = NULL;
    m_pLastChild = NULL;
    m_pNextNode = NULL;
}

GXmlNode* GXmlNodeImp::GetFirstChild(const CHAR *szNodeName)
{
    if (!m_pFirstChild)
    {
        return NULL;
    }

    if (!szNodeName)
    {
        return m_pFirstChild;
    }

    for (GXmlNodeImp* pChild=m_pFirstChild; pChild; pChild=pChild->m_pNextNode)
    {
        if (strcmp(pChild->GetName(), szNodeName) == 0)
        {
            return pChild;
        }
    }

    return NULL;
}

GXmlNode* GXmlNodeImp::GetSibling(VOID)
{
    CHAR            *szNodeName = GetName();
    GXmlNodeImp     *pNode = m_pNextNode;

    for (; pNode; pNode=pNode->m_pNextNode)
    {
        if (strcmp(pNode->GetName(), szNodeName) == 0)
        {
            return pNode;
        }
    }

    return NULL;
}

GXmlNode* GXmlNodeImp::GetNext(VOID)
{
    return m_pNextNode;
}

CHAR* GXmlNodeImp::GetName(VOID)
{
    return gos_xml_get_node_name(m_pstNode);
}

CHAR* GXmlNodeImp::GetValue(VOID)
{
    return gos_xml_get_node_value(m_pstNode);
}

BOOL GXmlNodeImp::GetValue(INT32 *piValue)
{
    return gos_xml_get_node_int_value(m_pstNode, piValue);
}

CHAR* GXmlNodeImp::GetAttrValue(const CHAR *szAttrName)
{
    return gos_xml_get_attr_value(m_pstNode, szAttrName);
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, CHAR *szValue, UINT32 ulMaxLen)
{
    CHAR    *szAttrValue = GetAttrValue(szAttrName);

    if (!szAttrValue)
    {
        return FALSE;
    }

    if (strlen(szAttrValue) >= ulMaxLen)
    {
        return FALSE;
    }

    strcpy(szValue, szAttrValue);
    return TRUE;
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, UINT32 *pulValue)
{
    return gos_xml_get_attr_int_value(m_pstNode, szAttrName, (INT32*)pulValue);
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, INT32 *piValue)
{
    return gos_xml_get_attr_int_value(m_pstNode, szAttrName, piValue);
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, INT64 *pi64Value)
{
    return gos_xml_get_attr_int64_value(m_pstNode, szAttrName, pi64Value);
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, UINT16 *pusValue)
{
    UINT32      ulValue = 0;

    if (!gos_xml_get_attr_int_value(m_pstNode, szAttrName, (INT32*)&ulValue))
    {
        return FALSE;
    }

    if (ulValue > 0xffff)
    {
        return FALSE;
    }

    *pusValue = (UINT16)ulValue;

    return TRUE;
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, UINT8 *pucValue)
{
    UINT32      ulValue = 0;

    if (!gos_xml_get_attr_int_value(m_pstNode, szAttrName, (INT32*)&ulValue))
    {
        return FALSE;
    }

    if (ulValue > 0xff)
    {
        return FALSE;
    }

    *pucValue = (UINT8)ulValue;

    return TRUE;
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, FLOAT *pfValue)
{
    return gos_xml_get_attr_float_value(m_pstNode, szAttrName, pfValue);
}

BOOL GXmlNodeImp::GetAttrValue(const CHAR *szAttrName, DOUBLE *pdValue)
{
    return gos_xml_get_attr_double_value(m_pstNode, szAttrName, pdValue);
}

BOOL GXmlNodeImp::DelChild(const CHAR *szAttrName, CHAR *szAttrValue)
{
    GXmlNodeImp *pPreChild = NULL;
    GXmlNodeImp *pChild;
    CHAR        *szValue;
    BOOL        bRet = FALSE;

    for (pChild=m_pFirstChild; pChild; pChild=pChild->m_pNextNode)
    {
        szValue = pChild->GetAttrValue(szAttrName);
        if (szValue && strcmp(szValue, szAttrValue) == 0)
        {
            bRet = TRUE;
            break;
        }

        pPreChild = pChild;
    }

    if (!bRet)
    {
        goto end;
    }

    if (m_pFirstChild == pChild)
    {
        m_pFirstChild = pChild->m_pNextNode;
    }

    if (m_pLastChild == pChild)
    {
        m_pLastChild = pPreChild;
    }

    if (pPreChild)
    {
        pPreChild->m_pNextNode = pChild->m_pNextNode;
    }

    goto end;

end:
    if (bRet)
    {
         gos_xml_del_node(m_pstNode, pChild->m_pstNode);
         m_pDoc->DeleteNode(pChild);
    }

    return bRet;
}

BOOL GXmlNodeImp::AddChild(const CHAR *szChildName, CHAR *szChildValue)
{
    GOS_XML_NODE_T *pstChild = gos_xml_add_node(m_pstNode, szChildName, szChildValue);

    if (!pstChild)
    {
        return FALSE;
    }

    GXmlNodeImp *pNode = m_pDoc->NewNode(pstChild);//new GXmlNodeImp();
    if (!pNode)
    {
        return FALSE;
    }

    pNode->m_pstNode = pstChild;

    return LinkChild(pNode);
}

BOOL GXmlNodeImp::AddChild(const CHAR *szChildName, INT32 iChildValue)
{
    CHAR    acChildValue[16];

    sprintf(acChildValue, "%d", iChildValue);
    return AddChild(szChildName, acChildValue);
}

BOOL GXmlNodeImp::LinkChild(GXmlNodeImp *pNode)
{
    if (!pNode)
    {
        return FALSE;
    }

    pNode->m_pNextNode = NULL;

    if (!m_pFirstChild)
    {
        m_pFirstChild = pNode;
    }
    else
    {
        m_pLastChild->m_pNextNode = pNode;
    }

    m_pLastChild = pNode;

    return TRUE;
}

BOOL GXmlNodeImp::AddAttr(const CHAR *szAttrName, CHAR *szAttrValue)
{
    return gos_xml_add_attr(m_pstNode, szAttrName, szAttrValue);
}

BOOL GXmlNodeImp::AddAttr(const CHAR *szAttrName, INT32 iAttrValue)
{
    CHAR    acAttrValue[16];

    sprintf(acAttrValue, "%d", iAttrValue);
    return AddAttr(szAttrName, acAttrValue);
}

CHAR* GXmlNodeImp::GetText(BOOL bNewLine)
{
    return gos_xml_get_node_text(m_pDoc->m_pstDoc, m_pstNode, bNewLine);
}

BOOL GXmlNodeImp::SetText(CHAR *szText)
{
    return gos_xml_set_node_text(m_pDoc->m_pstDoc, m_pstNode, szText);
}

VOID GXmlNodeImp::ClearChild()
{
    gos_xml_clear_child_node(m_pstNode);
}

BOOL GXmlNodeImp::GenChildTree(VOID)
{
    GOS_XML_NODE_T      *pstNode;
    GXmlNodeImp         *pNode;

    pstNode = gos_xml_get_first_child(m_pstNode, NULL);
    while(pstNode)
    {
        pNode = m_pDoc->NewNode(pstNode);
        if (!pNode)
        {
            return FALSE;
        }

        pNode->m_pstNode = pstNode;

        if (!pNode->GenChildTree())
        {
            return FALSE;
        }

        if (!LinkChild(pNode))
        {
            return FALSE;
        }

        pstNode = gos_xml_get_next(pstNode);
    }

    return TRUE;
}

CHAR* GXmlNodeImp::GetChildValue(const CHAR *szChildName)
{
    GXmlNode        *pNode = GetFirstChild(szChildName);
    CHAR            *szValue = pNode->GetValue();

    return szValue;
}

VOID GXmlNodeImp::GetChildValue(const CHAR *szChildName, VECTOR<CHAR*> &vChildValue)
{
    GXmlNode        *pNode = GetFirstChild(szChildName);
    CHAR            *szValue;

    vChildValue.clear();

    while(pNode)
    {
        szValue = pNode->GetValue();
        vChildValue.push_back(szValue);
        pNode = pNode->GetSibling();
    }
}

GXmlDocImp::GXmlDocImp(UINT32 ulStaticNodeNum)
{
    m_pRootNode = NULL;

    if (0 == ulStaticNodeNum)
    {
        ulStaticNodeNum = 64;
    }

    m_pstDoc = gos_xml_create_document(ulStaticNodeNum);

    m_ulStaticNodeNum = ulStaticNodeNum;
    if (m_ulStaticNodeNum > 0)
    {
        m_pStaticNode = new GXmlNodeImp[m_ulStaticNodeNum];
    }
    else
    {
        m_pStaticNode = NULL;
    }
}

BOOL GXmlDocImp::Validate(VOID)
{
    if (!m_pstDoc)
    {
        return FALSE;
    }

    return TRUE;
}

GXmlDocImp::~GXmlDocImp(VOID)
{
    Free();

    if (m_pstDoc)
    {
        gos_xml_destroy_document(m_pstDoc);
        m_pstDoc = NULL;
    }

    if (m_pStaticNode)
    {
        delete[] m_pStaticNode;
        m_pStaticNode = NULL;
    }
}

GXmlNodeImp* GXmlDocImp::NewNode(GOS_XML_NODE_T *pstNode)
{
    GXmlNodeImp *pNode = NULL;

    if (m_ulStaticNodeNum > 0 && m_ulFreeNodeIndex < m_ulStaticNodeNum)
    {
        pNode = m_pStaticNode+m_ulFreeNodeIndex;

        m_ulFreeNodeIndex ++;

        //if (!pNode->m_pstNode)
        {
            pNode->m_pDoc = this;
            pNode->m_pstNode = pstNode;

            pNode->m_pFirstChild = NULL;
            pNode->m_pLastChild = NULL;
            pNode->m_pNextNode = NULL;

            return pNode;
        }
    }

    pNode = new GXmlNodeImp();

    if (!pNode)
    {
        return NULL;
    }

    pNode->m_pDoc = this;
    pNode->m_pstNode = pstNode;

    pNode->m_pFirstChild = NULL;
    pNode->m_pLastChild = NULL;
    pNode->m_pNextNode = NULL;

    return pNode;
}

VOID GXmlDocImp::DeleteNode(GXmlNodeImp* pNode)
{
    if (!pNode)
    {
        return;
    }

    GXmlNodeImp *pChild = pNode->m_pFirstChild;
    GXmlNodeImp *pNextChild;

    while(pChild)
    {
        pNextChild = pChild->m_pNextNode;
        DeleteNode(pChild);
        pChild = pNextChild;
    }

    if (m_pStaticNode && pNode >= m_pStaticNode && pNode <= (m_pStaticNode+m_ulStaticNodeNum-1))
    {
        pNode->m_pDoc = NULL;
        pNode->m_pstNode = NULL;

        pNode->m_pFirstChild = NULL;
        pNode->m_pLastChild = NULL;
        pNode->m_pNextNode = NULL;
    }
    else
    {
        pNode->m_pFirstChild = NULL;
        delete pNode;
    }
}

BOOL GXmlDocImp::GenNodeTree(VOID)
{
    GOS_XML_NODE_T      *pstNode;

    pstNode = gos_xml_get_root_node(m_pstDoc);
    m_pRootNode = NewNode(pstNode);
    if (!m_pRootNode)
    {
        return FALSE;
    }

    return m_pRootNode->GenChildTree();
}

BOOL GXmlDocImp::ParseString(CHAR *szXML, BOOL bRemoveComment)
{
    Free();

    if (!gos_xml_parse_string(m_pstDoc, szXML, bRemoveComment))
    {
        return FALSE;
    }

    return GenNodeTree();
}

BOOL GXmlDocImp::ParseFile(CHAR *szFile, BOOL bRemoveComment)
{
    Free();

    if (!gos_xml_parse_file(m_pstDoc, szFile, bRemoveComment))
    {
        return FALSE;
    }

    return GenNodeTree();
}

CHAR* GXmlDocImp::GetText(BOOL bNewLine)
{
    return gos_xml_get_doc_text(m_pstDoc, bNewLine);
}

BOOL GXmlDocImp::SaveToFile(CHAR *szFileName, BOOL bNewLine)
{
    return gos_xml_save_doc(m_pstDoc, szFileName, bNewLine);
}

GXmlNode* GXmlDocImp::GetRootNode(VOID)
{
    return m_pRootNode;
}

BOOL GXmlDocImp::SetDeclaration(CHAR *szVersion, CHAR *szEncoding)
{
    return gos_xml_set_declaration(m_pstDoc, szVersion, szEncoding);
}

BOOL GXmlDocImp::SetRootNode(CHAR *szNodeName)
{
    return gos_xml_set_root_node(m_pstDoc, szNodeName);
}

VOID GXmlDocImp::Free(VOID)
{
    if (m_pRootNode)
    {
        DeleteNode(m_pRootNode);
        m_pRootNode = NULL;
    }

    m_ulFreeNodeIndex = 0;
}

GXmlDoc* GXmlDoc::New(UINT32 ulStaticNodeNum)
{
    GXmlDocImp *pDoc = new GXmlDocImp(ulStaticNodeNum);

    if (pDoc)
    {
        if (!pDoc->Validate())
        {
            delete pDoc;
            return NULL;
        }
    }

    return (GXmlDoc*)pDoc;
}

VOID GXmlDoc::Delete(GXmlDoc *&pDoc)
{
    GXmlDocImp *pDocImpl = (GXmlDocImp*)pDoc;

    if (pDocImpl)
    {
        delete pDocImpl;

        pDoc = NULL;
    }
}
