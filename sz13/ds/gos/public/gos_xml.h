#ifndef GOS_XML_H
#define GOS_XML_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef UINT32  GOS_XML_DOCUMENT_T;
typedef UINT32  GOS_XML_NODE_T;

GOS_XML_DOCUMENT_T* gos_xml_create_document(UINT32 ulStaticNodeNum);
BOOL gos_xml_parse_string(GOS_XML_DOCUMENT_T *pstDoc, CHAR *szXML, BOOL bRemoveComment);
BOOL gos_xml_parse_file(GOS_XML_DOCUMENT_T *pDoc, CHAR *szFile, BOOL bRemoveComment);
VOID gos_xml_free_document(GOS_XML_DOCUMENT_T *pstDoc);
VOID gos_xml_destroy_document(GOS_XML_DOCUMENT_T *pDoc);

GOS_XML_NODE_T* gos_xml_get_root_node(GOS_XML_DOCUMENT_T *pstDoc);
GOS_XML_NODE_T* gos_xml_get_first_child(GOS_XML_NODE_T *pstNode, CHAR *szNodeName);
GOS_XML_NODE_T* gos_xml_get_next(GOS_XML_NODE_T *pNode);
GOS_XML_NODE_T* gos_xml_get_sibling(GOS_XML_NODE_T *pstNode);
UINT32  gos_xml_get_children_number(GOS_XML_NODE_T *pNode, CHAR *szNodeName);
CHAR*   gos_xml_get_node_name(GOS_XML_NODE_T *pstNode);
CHAR*   gos_xml_get_node_value(GOS_XML_NODE_T *pstNode);
BOOL    gos_xml_get_node_int_value(GOS_XML_NODE_T *pstNode, INT32 *piValue);
CHAR*   gos_xml_get_childnode_value(GOS_XML_NODE_T *pNode, CHAR *szChildNodeName);
BOOL    gos_xml_get_childnode_int_value(GOS_XML_NODE_T *pNode, CHAR *szChildNodeName, INT32 *piValue);
CHAR*   gos_xml_get_attr_value(GOS_XML_NODE_T *pstNode, const CHAR *szAttrName);
BOOL    gos_xml_get_attr_int_value(GOS_XML_NODE_T *pstNode, const CHAR *szAttrName, INT32 *piValue);
BOOL    gos_xml_get_attr_int64_value(GOS_XML_NODE_T *pstNode, const CHAR *szAttrName, INT64 *pi64Value);
BOOL    gos_xml_get_attr_float_value(GOS_XML_NODE_T *pstNode, const CHAR *szAttrName, FLOAT *pfValue);
BOOL    gos_xml_get_attr_double_value(GOS_XML_NODE_T *pstNode, const CHAR *szAttrName, DOUBLE *pdValue);

BOOL    gos_xml_set_root_node(GOS_XML_DOCUMENT_T *pDoc, CHAR *szNodeName);
GOS_XML_NODE_T* gos_xml_add_node(GOS_XML_NODE_T *pNode, const CHAR *szChildName, CHAR *szChildValue);
BOOL    gos_xml_del_node(GOS_XML_NODE_T *pPNode, GOS_XML_NODE_T *pCNode);
BOOL    gos_xml_add_attr(GOS_XML_NODE_T *pNode, const CHAR *szAttrName, CHAR *szAttrValue);
BOOL    gos_xml_set_declaration(GOS_XML_DOCUMENT_T *pDoc, CHAR *szVersion, CHAR *szEncoding);
CHAR*   gos_xml_get_doc_text(GOS_XML_DOCUMENT_T *pDoc, BOOL bNewLine);
CHAR*   gos_xml_get_node_text(GOS_XML_DOCUMENT_T *pDoc, GOS_XML_NODE_T *pNode, BOOL bNewLine);
BOOL    gos_xml_set_node_text(GOS_XML_DOCUMENT_T *pDoc, GOS_XML_NODE_T *pNode, CHAR *szText);
VOID    gos_xml_clear_child_node(GOS_XML_NODE_T *pNode);
BOOL    gos_xml_save_doc(GOS_XML_DOCUMENT_T *pDoc, CHAR *szFileName, BOOL bNewLine);

#ifdef __cplusplus
}
#endif

#endif

