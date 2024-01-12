#ifndef GOS_JSON_H
#define GOS_JSON_H

typedef struct
{
    CHAR        *szKey;
    CHAR        *szValue;
}JSON_KEY_VALUE_T;

typedef struct
{
    BOOL        bParseSucc;
    CHAR        *szJson;

    GLIST       *pKeyValueList;
    GLIST       *pSubJsonList;
}GJSON;

GJSON*  gos_json_init(VOID);
VOID    gos_json_free(GJSON* Json);
BOOL    gos_json_parse(GJSON* Json, CHAR *szJson);
BOOL    gos_json_parse_array(GJSON* Json, CHAR *szJson);

CHAR*   gos_json_get_string (GJSON* Json, CHAR *szKey);
BOOL    gos_json_get_string_ex(GJSON* Json, CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen);
BOOL    gos_json_get_int32  (GJSON* Json, CHAR *szKey, INT32 *piValue);
BOOL    gos_json_get_uint32 (GJSON* Json, CHAR *szKey, UINT32 *pulValue);
BOOL    gos_json_get_int16  (GJSON* Json, CHAR *szKey, INT16 *psValue);
BOOL    gos_json_get_uint16 (GJSON* Json, CHAR *szKey, UINT16 *pusValue);
BOOL    gos_json_get_int8   (GJSON* Json, CHAR *szKey, INT8 *pcValue);
BOOL    gos_json_get_uint8  (GJSON* Json, CHAR *szKey, UINT8 *pucValue);
BOOL    gos_json_get_int64  (GJSON* Json, CHAR *szKey, INT64 *pi64Value);
BOOL    gos_json_get_ip     (GJSON* Json, CHAR *szKey, UINT8 *pucValue);
GJSON*  gos_json_get_unique_sub_json(GJSON* Json);
GLIST*  gos_json_get_sub_json(GJSON *Json);

// GJson Param
typedef struct
{
    CHAR        *szParam;
    UINT32      ulLen;
}GJSON_PARAM;

typedef struct
{
    CHAR        *szParam;
    UINT32      ulLen;
}GJSON_TUPLE_PARAM;

GJSON_PARAM* gos_json_param_init();
VOID    gos_json_param_free         (GJSON_PARAM *JsonParam);
CHAR*   gos_json_param_get_string   (GJSON_PARAM* JsonParam);

VOID    gos_json_param_add_string   (GJSON_PARAM* JsonParam, const CHAR *szName, CHAR *szValue);
VOID    gos_json_param_add_int64    (GJSON_PARAM* JsonParam, const CHAR *szName, INT64 i64Value);
VOID    gos_json_param_add_uint32   (GJSON_PARAM* JsonParam, const CHAR *szName, UINT32 ulValue);
VOID    gos_json_param_add_int32    (GJSON_PARAM* JsonParam, const CHAR *szName, INT32 iValue);
VOID    gos_json_param_add_uint16   (GJSON_PARAM* JsonParam, const CHAR *szName, UINT16 usValue);
VOID    gos_json_param_add_int16    (GJSON_PARAM* JsonParam, const CHAR *szName, INT16 sValue);
VOID    gos_json_param_add_uint8    (GJSON_PARAM* JsonParam, const CHAR *szName, UINT8 ucValue);
VOID    gos_json_param_add_int8     (GJSON_PARAM* JsonParam, const CHAR *szName, INT8 cValue);
VOID    gos_json_param_add_ip       (GJSON_PARAM* JsonParam, const CHAR *szName, UINT8 *pucValue);

VOID    gos_json_param_add_json_tuple_param(GJSON_PARAM* JsonParam, const CHAR *szName, GJSON_TUPLE_PARAM *pJsonTupleParam);
VOID    gos_json_param_add_json_param      (GJSON_PARAM* JsonParam, const CHAR *szName, GJSON_PARAM *SubJsonParam);

// // GJson Tuple Param
GJSON_TUPLE_PARAM* gos_json_tuple_param_init();
VOID    gos_json_tuple_param_free(GJSON_TUPLE_PARAM *JsonTupleParam);
VOID    gos_json_tuple_param_add_json_param(GJSON_TUPLE_PARAM* JsonTupleParam, GJSON_PARAM *JsonParam);
CHAR*   gos_json_tuple_param_get_string(GJSON_TUPLE_PARAM* JsonTupleParam);

//---------------------------------------------------------------------------
#endif
