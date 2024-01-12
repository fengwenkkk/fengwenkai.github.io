#ifndef G_XML_H
#define G_XML_H

#include <vector>

class GXmlNode
{
public:
    virtual VOID                Free(VOID)=0;
    virtual GXmlNode*           GetFirstChild(const CHAR *szNodeName=NULL)=0;
    virtual GXmlNode*           GetNext(VOID)=0;
    virtual GXmlNode*           GetSibling(VOID)=0;
    virtual CHAR*               GetName(VOID)=0;
    virtual CHAR*               GetValue(VOID)=0;
    virtual BOOL                GetValue(INT32 *piValue)=0;
    virtual CHAR*               GetAttrValue(const CHAR *szAttrName)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, CHAR *szValue, UINT32 ulMaxLen)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, UINT32 *pulValue)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, UINT16 *pusValue)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, INT32 *piValue)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, INT64 *pi64Value)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, UINT8 *pucValue)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, FLOAT *pfValue)=0;
    virtual BOOL                GetAttrValue(const CHAR *szAttrName, DOUBLE *pdValue)=0;

    virtual CHAR*               GetChildValue(const CHAR *szChildName)=0;
    virtual VOID                GetChildValue(const CHAR *szChildName, VECTOR<CHAR*> &vChildValue)=0;

    virtual BOOL                AddChild(const CHAR *szChildName, CHAR *szChildValue=NULL)=0;
    virtual BOOL                AddChild(const CHAR *szChildName, INT32 iChildValue)=0;
    virtual BOOL                AddAttr(const CHAR *szAttrName, CHAR *szAttrValue)=0;
    virtual BOOL                AddAttr(const CHAR *szAttrName, INT32 iAttrValue)=0;

    virtual BOOL                DelChild(const CHAR *szChildName,CHAR *szChildValue) = 0;
    virtual CHAR*               GetText(BOOL bNewLine=FALSE)=0;
    virtual BOOL                SetText(CHAR *szText)=0;
    virtual VOID                ClearChild()=0;
};

class GXmlDoc
{
public:
    static  GXmlDoc*            New(UINT32 ulStaticNodeNum=0);
    static  VOID                Delete(GXmlDoc * &pDoc);
    virtual BOOL                ParseString(CHAR *szXML, BOOL bRemoveComment=TRUE)=0;
    virtual BOOL                ParseFile(CHAR *szFile, BOOL bRemoveComment=TRUE)=0;
    virtual BOOL                SaveToFile(CHAR *szFileName, BOOL bNewLine=FALSE)=0;
    virtual CHAR*               GetText(BOOL bNewLine=FALSE)=0;
    virtual GXmlNode*           GetRootNode(VOID)=0;

    virtual BOOL                SetDeclaration(CHAR *szVersion=(CHAR*)"1.0", CHAR *szEncoding=(CHAR*)"utf-8")=0;
    virtual BOOL                SetRootNode(CHAR *szNodeName)=0;

    virtual VOID                Free(VOID)=0;
};

#endif
