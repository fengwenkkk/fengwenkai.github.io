#ifndef _GOS_CONF_FILE_H
#define _GOS_CONF_FILE_H

typedef struct
{
    CHAR        acFile[512];
    GHASH       *pKeyValueHash;
    GLIST       *pGroupIDList;
    CHAR        *szConf;
}GCONF;

GCONF*  gos_conf_init(CHAR *szFileName);
VOID    gos_conf_free(GCONF *Conf);

CHAR*   gos_conf_get_string(GCONF *pConf, const CHAR *szKey);
BOOL    gos_conf_get_int32_ex(GCONF *pConf, const CHAR *szKey, INT32 iDefaultValue, INT32 *piValue);
CHAR*   gos_conf_get_group_string(GCONF *pConf, const CHAR *szGroupKey, const CHAR *szKey);
BOOL    gos_conf_get_group_string_ex(GCONF *pConf, const CHAR *szGroupKey, const CHAR *szKey, CHAR *szRetValue, UINT32 ulMaxLen);
BOOL    gos_conf_get_group_int32(GCONF *pConf, const CHAR *szGroupKey, const CHAR *szKey, INT32 iDefaultValue, INT32 *piValue);


#endif
