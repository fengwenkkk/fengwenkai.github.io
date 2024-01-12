#ifndef _PDB_MYSQL_CORE_H
#define _PDB_MYSQL_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    MYSQL               *ConnHandle;
    MYSQL_STMT          *StmtHandle;
    MYSQL_ROW           pszQryRow;
    MYSQL_RES           *pstQryRlst;
}MYSQL_DB_HANDLES_T;

/* PDB���Ӷ��� */
typedef struct
{
    PDB_CONN_PARAM_T    stConnParam;

    HANDLE              EnvHandle;
    MYSQL_DB_HANDLES_T  DBHandles;

    PDB_OPTION_T        stOption;

    CHAR                acTransName[PDB_MAX_TRANS_LEN];

    CHAR                acSQLCmd[PDB_MAX_SQL_LEN+1];

    UINT32              ulBindNum;
    MYSQL_BIND          astBindHandle[PDB_MAX_BIND_NUM];

    UINT32              ulSendBlobParamNum; /* ��ֶη��͵�BLOB���� */
    UINT32              aulBindIndex[PDB_MAX_BIND_NUM]; /* ��ֶη��͵�BLOB���������� */
    VOID*               pavParent[PDB_MAX_BIND_NUM]; /* BLOB�ֶεĸ�������ָ�룬������������*/
    //PDBUINT32         ulBlobParamNum; /* BLOB�ֶθ��� */

    UINT32              ulBindRsltNum;
    MYSQL_BIND          astBindRsltHandle[PDB_MAX_BIND_NUM];

    UINT32              ulBindSkipLen;

    UINT32              ulExecNum;      /* ����ִ�еĴ��� */

    CHAR                acErrInfo[PDB_MAX_ERR_LEN]; /* ����������Ϣ */
    UINT32              ulErrLevel; /* ���󼶱� */
    INT32               iErrCode;   /* ԭʼ������ */

    UINT32              ulRowCount;         /* ��ǰ����Ӱ������� */
    UINT32              ulFetchedNum;
    UINT32              ulExecErrCount;     /* ����ִ��ʧ�ܵĸ��� */

//  PRN_T               stPrn;              /* ��ӡ���� */

}MYSQL_DCI_HANDLE_T;

/* ����Handle */
typedef struct
{
    BOOL                bIsAlloced;     /* �Ƿ񱻷����� */
    BOOL                bIsConnected;   /* �Ƿ��Ѿ��������� */
    BOOL                bIsInUse;       /* �����Ƿ�ʹ���� */
    INT32               iConnId;        /* ����ID*/

    MYSQL_DCI_HANDLE_T  stDCIHandle;

}MYSQL_PDB_HANDLE_T;

BOOL   MySQLCheckHandle(MYSQL_DCI_HANDLE_T *pstHandle);
PDBRET MySQLInit(HANDLE *phEnv);
PDBRET MySQLExit(HANDLE hEnv);
PDBRET MySQLConnect(MYSQL_DCI_HANDLE_T *pstDCIHandle, PDB_CONN_PARAM_T  *pstConnParam, PDB_OPTION_T *pstOption);
PDBRET MySQLDisconnect(MYSQL_DCI_HANDLE_T *pstDCIHandle);

PDBRET MySQLInitStmt(MYSQL_DCI_HANDLE_T *pstDCIHandle);
PDBRET MySQLCloseStmt(MYSQL_DCI_HANDLE_T *pstDCIHandle);
PDBRET MySQLSQLPrepare(MYSQL_DCI_HANDLE_T *pstDCIHandle);
PDBRET MySQLSetPrefetchNum(MYSQL_DCI_HANDLE_T *pstDCIHandle, UINT32 ulRowNum);
PDBRET MySQLSetAutoCommit(MYSQL_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit);
PDBRET MySQLQuery(MYSQL_DCI_HANDLE_T *pstDCIHandle);
PDBRET MySQLExec(MYSQL_DCI_HANDLE_T *pstDCIHandle);
PDBRET MySQLBindRsltByPos(MYSQL_DCI_HANDLE_T    *pstDCIHandle,
                        UINT16          usRsltIndex,
                        UINT16          usRsltType,
                        VOID            *pRsltBuf,
                        UINT32          ulRsltBufLen,
                        BOOL            *pbIsNull,
                        INT32           *piLenInd);
PDBRET MySQLBindRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle);
PDBRET MySQLStoreRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle);
PDBRET MySQLRowFetch(MYSQL_DCI_HANDLE_T* pstDCIHandle);
PDBRET MySQLFreeRslt(MYSQL_DCI_HANDLE_T* pstDCIHandle);
PDBRET MySQLTransBegin(MYSQL_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
PDBRET MySQLTransRollback(MYSQL_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
PDBRET MySQLTransCommit(MYSQL_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
INT32  MySQLRowCount(MYSQL_DCI_HANDLE_T* pstDCIHandle);
PDBRET MySQLBindPara(MYSQL_DCI_HANDLE_T *pstDCIHandle,
                   UINT16       usParaIndex,
                   UINT32       ulParaType,
                   UINT32       ulInoutType,
                   VOID         *pParaValue,
                   UINT32       ulParaLen,
                   INT32        *piLenInd);
PDBRET MySQLGetError(MYSQL_DCI_HANDLE_T  *pstDCIHandle);

#ifdef __cplusplus
}
#endif

#endif
