#ifndef _PDB_ODBC_CORE_H
#define _PDB_ODBC_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    HANDLE              ConnHandle;
    HANDLE              StmtHandle;
}ODBC_DB_HANDLES_T;

/* PDB���Ӷ��� */
typedef struct
{
    PDB_CONN_PARAM_T    stConnParam;

    HANDLE              EnvHandle;
    ODBC_DB_HANDLES_T   DBHandles;

    PDB_OPTION_T        stOption;

    CHAR                acTransName[PDB_MAX_TRANS_LEN];

    CHAR                acSQLCmd[PDB_MAX_SQL_LEN+1];

    UINT32              ulBindNum;

    UINT32              ulBindSkipLen;

    UINT32              ulExecNum;      /* ����ִ�еĴ��� */

    CHAR                acErrInfo[PDB_MAX_ERR_LEN]; /* ����������Ϣ */
    UINT32              ulErrLevel; /* ���󼶱� */
    INT32               iErrCode;   /* ԭʼ������ */

    UINT32              ulRowCount;         /* ��ǰ����Ӱ������� */
    UINT32              ulFetchedNum;
    UINT32              ulExecErrCount;     /* ����ִ��ʧ�ܵĸ��� */

//  PRN_T               stPrn;              /* ��ӡ���� */

}ODBC_DCI_HANDLE_T;

/* ����Handle */
typedef struct
{
    BOOL                bIsAlloced;     /* �Ƿ񱻷����� */
    BOOL                bIsConnected;   /* �Ƿ��Ѿ��������� */
    BOOL                bIsInUse;       /* �����Ƿ�ʹ���� */
    INT32               iConnId;        /* ����ID*/

    ODBC_DCI_HANDLE_T   stDCIHandle;

}ODBC_PDB_HANDLE_T;

BOOL   ODBCCheckHandle(ODBC_DCI_HANDLE_T *pstHandle);
PDBRET ODBCInit(HANDLE *phEnv);
PDBRET ODBCExit(HANDLE hEnv);
PDBRET ODBCConnect(ODBC_DCI_HANDLE_T *pstDCIHandle, PDB_CONN_PARAM_T  *pstConnParam, PDB_OPTION_T *pstOption);
PDBRET ODBCDisconnect(ODBC_DCI_HANDLE_T *pstDCIHandle);

PDBRET ODBCInitStmt(ODBC_DCI_HANDLE_T *pstDCIHandle);
PDBRET ODBCCloseStmt(ODBC_DCI_HANDLE_T *pstDCIHandle);
PDBRET ODBCSQLPrepare(ODBC_DCI_HANDLE_T *pstDCIHandle);
PDBRET ODBCSetPrefetchNum(ODBC_DCI_HANDLE_T *pstDCIHandle, UINT32 ulRowNum);
PDBRET ODBCSetAutoCommit(ODBC_DCI_HANDLE_T *pstDCIHandle, BOOL bAutoCommit);
PDBRET ODBCQuery(ODBC_DCI_HANDLE_T *pstDCIHandle);
PDBRET ODBCExec(ODBC_DCI_HANDLE_T *pstDCIHandle);
PDBRET ODBCBindRsltByPos(ODBC_DCI_HANDLE_T  *pstDCIHandle,
                        UINT16          usRsltIndex,
                        UINT16          usRsltType,
                        VOID            *pRsltBuf,
                        UINT32          ulRsltBufLen,
                        BOOL            *pbIsNull,
                        INT32           *piLenInd);
PDBRET ODBCBindRslt(ODBC_DCI_HANDLE_T* pstDCIHandle);
PDBRET ODBCStoreRslt(ODBC_DCI_HANDLE_T* pstDCIHandle);
PDBRET ODBCRowFetch(ODBC_DCI_HANDLE_T* pstDCIHandle);
PDBRET ODBCFreeRslt(ODBC_DCI_HANDLE_T* pstDCIHandle);
PDBRET ODBCTransBegin(ODBC_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
PDBRET ODBCTransRollback(ODBC_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);
PDBRET ODBCTransCommit(ODBC_DCI_HANDLE_T* pstDCIHandle, CHAR *szTransName);

INT32  ODBCRowCount(ODBC_DCI_HANDLE_T *pstDCIHandle);
PDBRET ODBCBindPara(ODBC_DCI_HANDLE_T *pstDCIHandle,
                   UINT16       usParaIndex,
                   UINT32       ulParaType,
                   UINT32       ulInoutType,
                   VOID         *pParaValue,
                   UINT32       ulParaLen,
                   INT32        *piLenInd);

PDBRET ODBCGetError(ODBC_DCI_HANDLE_T *pstDCIHandle);

#ifdef __cplusplus
}
#endif

#endif
