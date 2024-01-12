#ifndef GOS_XML_PARSE_H
#define GOS_XML_PARSE_H

#define GOS_XML_LT      "&lt"       // 表示<字符
#define GOS_XML_GT      "&gt"       // 表示>字符
#define GOS_XML_AMP     "&amp"      // 表示&字符
#define GOS_XML_APOS    "&apos"     // 表示'字符
#define GOS_XML_QUOT    "&quot"     // 表示"字符

#define GOS_XML_NEWLINE_FILE        2

#define GOS_XML_EMPTY_STR(text)     (!(text) || *(CHAR*)(text) == '\0')

typedef struct _GOS_XML_ATTR_T
{
    CHAR                    *szName;
    CHAR                    *szValue;

    struct _GOS_XML_ATTR_T  *pstSibling;

    struct _GOS_XML_ATTR_T  *pstPrev;       // used for link manage
    struct _GOS_XML_ATTR_T  *pstNext;       // used for link manage
}_GOS_XML_ATTR_T;

struct _GOS_XML_DOCUMENT_T;

typedef struct _GOS_XML_NODE_T
{
    struct _GOS_XML_DOCUMENT_T     *pstDoc;

    CHAR                    *szName;
    CHAR                    *szValue;

    struct _GOS_XML_NODE_T  *pstFirstChild; // 第一个Child
    struct _GOS_XML_NODE_T  *pstLastChild;  // 最后一个Child
    struct _GOS_XML_NODE_T  *pstSibling;    // 兄弟节点

    _GOS_XML_ATTR_T         *pstAttr;

    struct _GOS_XML_NODE_T  *pstPrev;       // used for link manage
    struct _GOS_XML_NODE_T  *pstNext;       // used for link manage
}_GOS_XML_NODE_T;

typedef struct _GOS_XML_DOCUMENT_T
{
    BOOL                    bParsed;

    UINT32                  ulStaticXMLLen;
    CHAR                    *szStaticXML;

    CHAR                    *szXML;
    CHAR                    *szXMLEnd;

    CHAR                    *szVersion;
    CHAR                    *szEncoding;

    _GOS_XML_NODE_T         *pstRootNode;

    BOOL                    bRemoveComment;

    _GOS_XML_NODE_T         stNodeDummy;        // Node List空闲队列头
    _GOS_XML_ATTR_T         stAttrDummy;        // Attr List空闲队列头

    UINT32                  ulStaticNodeNum;
    UINT32                  ulStaticAttrNum;

    _GOS_XML_NODE_T         *pstNodeList;
    _GOS_XML_ATTR_T         *pstAttrList;

}_GOS_XML_DOCUMENT_T;

#endif /* GOS_XML_PARSE_H */

