#ifndef _PDB_APP_H
#define _PDB_APP_H

#include "pdb_def.h"
#include "pdb_common.h"
#include "pdb_dcci.h"

#define MAX_LOGFILE_NUM     10
#define MAX_LOGFILE_SIZE    (16*1024*1024)
#define MAX_PRN_MSG_LEN     1024

/* SQL语句参数数据类型标识 */
#define PDB_INT8_MARK           'c'
#define PDB_INT16_MARK          'w'
#define PDB_INT32_MARK          'i'
#define PDB_INT64_MARK          'l'

#define PDB_UINT8_MARK          'C'
#define PDB_UINT16_MARK         'W'
#define PDB_UINT32_MARK         'I'
#define PDB_UINT64_MARK         'L'

#define PDB_FLOAT_MARK          'f'
#define PDB_DOUBLE_MARK         'd'
#define PDB_STRING_MARK         's'

#define PDB_BLOB_MARK           'b'
#define PDB_LONGBLOB_MARK       'B'
#define PDB_NULL_MARK           'n'

/* 查询类型 */
#define PDB_QRY_USE_CACHE       1 /* 查询使用客户端缓存 */
#define PDB_QRY_USE_CURSOR      2 /* 查询使用光标 */


#define PDB_INVALID_BATCH_NUM   (-1)

//#define FOR_ROW_FETCH(enRet, pPDBApp)   for (enRet=pPDBApp->RowFetch(); enRet==PDB_CONTINUE; enRet=pPDBApp->RowFetch())
#define FOR_ROW_FETCH(enRet, pPDBApp)   while ((enRet=pPDBApp->RowFetch()) == PDB_CONTINUE)

#pragma pack(push, 4)
typedef struct
{
    UINT16      usParamType;            /* 参数声明的数据类型 */
    UINT32      ulParamSize;            /* 参数的长度(字符串及BYTE数组) */
    INT32       iLenInd;
    BOOL        bIsNull;
    VOID        *pvParamValue;
    UINT32      ulInoutType;
}PDB_PARAM_T;

typedef struct
{
    UINT16      usParamType;            /* 参数声明的数据类型 */
    UINT32      ulParamOffset;          /* 参数的相对位置 */
    UINT32      ulParamSize;            /* 参数的长度(字符串及BYTE数组) */
}PDB_PARAM_DESC_T;

#pragma pack(pop)

class PDBRsltParam
{
public:
    PDBRsltParam(){};
    ~PDBRsltParam(){};

    VECTOR<PDB_PARAM_T>& GetParams(){return m_vParams;};
    VOID Clear(){m_vParams.clear();};

    BOOL Bind(INT8 *pvParamValue){return Bind(PDBT_INT8, pvParamValue);}
    BOOL Bind(UINT8 *pvParamValue){return Bind(PDBT_UINT8, pvParamValue);}
    BOOL Bind(INT16 *pvParamValue){return Bind(PDBT_INT16, pvParamValue);}
    BOOL Bind(UINT16 *pvParamValue){return Bind(PDBT_UINT16, pvParamValue);}
    BOOL Bind(INT32 *pvParamValue){return Bind(PDBT_INT32, pvParamValue);}
    BOOL Bind(UINT32 *pvParamValue){return Bind(PDBT_UINT32, pvParamValue);}
    BOOL Bind(INT64 *pvParamValue){return Bind(PDBT_INT64, pvParamValue);}
    BOOL Bind(UINT64 *pvParamValue){return Bind(PDBT_UINT64, pvParamValue);}
    BOOL Bind(FLOAT *pvParamValue){return Bind(PDBT_FLOAT, pvParamValue);}
    BOOL Bind(DOUBLE *pvParamValue){return Bind(PDBT_DOUBLE, pvParamValue);}
    BOOL Bind(CHAR *pvParamValue, UINT32 ulParamSize){return Bind(PDBT_STRING, pvParamValue, ulParamSize);}

    BOOL IsNull(UINT32 ulFieldIndex);
    INT32 GetLen(UINT32 ulFieldIndex);

private:
    BOOL Bind(UINT16 usParamType, VOID *pvParamValue, UINT32 ulParamSize=0);

    VECTOR<PDB_PARAM_T> m_vParams;
};

class PDBParam
{
public:
    PDBParam();
    ~PDBParam(){};

    VECTOR<PDB_PARAM_T>& GetParams();
    VOID Clear();

    BOOL Bind(INT8 *pvParamValue){return Bind(PDBT_INT8, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(UINT8 *pvParamValue){return Bind(PDBT_UINT8, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(INT16 *pvParamValue){return Bind(PDBT_INT16, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(UINT16 *pvParamValue){return Bind(PDBT_UINT16, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(INT32 *pvParamValue){return Bind(PDBT_INT32, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(UINT32 *pvParamValue){return Bind(PDBT_UINT32, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(INT64 *pvParamValue){return Bind(PDBT_INT64, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(UINT64 *pvParamValue){return Bind(PDBT_UINT64, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(FLOAT *pvParamValue){return Bind(PDBT_FLOAT, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(DOUBLE *pvParamValue){return Bind(PDBT_DOUBLE, pvParamValue, PDB_PARAM_INPUT);}
    BOOL Bind(CHAR *pvParamValue, UINT32 ulParamSize){return Bind(PDBT_STRING, pvParamValue, ulParamSize, PDB_PARAM_INPUT);}
    BOOL BindString(CHAR *pvParamValue){return Bind(pvParamValue, strlen(pvParamValue));}

    INT32 GetLen(UINT32 ulFieldIndex);

    BOOL Bind(UINT16 usParamType, VOID *pvParamValue, UINT32 ulInoutType);
    BOOL Bind(UINT16 usParamType, VOID *pvParamValue, UINT32 ulParamSize, UINT32 ulInoutType);

private:
    INT32               m_iBindErrIndex;
    VECTOR<PDB_PARAM_T> m_vParams;
};

class PDBBatchParam
{
public:
    PDBBatchParam(UINT32 ulMaxBatchNum, UINT32 ulSkipLen, PDB_PARAM_DESC_T *pstParamDesc, UINT32 ulParamNum);
    ~PDBBatchParam();

    VECTOR<PDB_PARAM_DESC_T>& GetParams(){return m_vParams;}
    BOOL AddData(VOID *pData, BOOL bReplaceFirst=TRUE);

    UINT32 GetSkipLen(){return m_ulSkipLen;}
    UINT32 GetDataNum(){return m_ulCurrNum;}

    UINT32 GetBatchDataNum(UINT32 ulMaxNum);
    VOID* GetBatchData();
    VOID RemoveBatchData(UINT32 ulDataNum);

    VOID* GetData(){return (VOID*)m_pucData;}

    BOOL IsFull();
    VOID Clear();

private:
    VECTOR<PDB_PARAM_DESC_T> m_vParams;
    UINT32              m_ulSkipLen;

    UINT32              m_ulMaxBatchNum;
    UINT8               *m_pucData;
    UINT32              m_ulCurrNum;
    UINT32              m_ulStartIndex;
};

class PDBApp
{
public:
    PDBApp(PDBEnvBase *pEnv);

    ~PDBApp(VOID);

    BOOL AllocConn();
    BOOL FreeConn();

    BOOL TransBegin();
    BOOL TransCommit();
    BOOL TransRollback();

    /* 执行无变参、非select的、单条sql语句 */
    INT32 Exec(const CHAR *szSQL); //CHAR *szTransName,

    /* 执行insert/modify/delete、单条sql语句 */
    INT32 Update(const CHAR *szSQL);
    INT32 Update(const CHAR *szSQL, PDBParam &Param);
    INT32 UpdateDirect(const CHAR *szSQL);
    INT32 UpdateDirect(const CHAR *szSQL, PDBParam &Param);

    /* 批量执行，自动提交 */
    INT32 UpdateBatch(const CHAR *szSQL, PDBBatchParam *Param, UINT32 ulExecNum);
    INT32 UpdateBatchDirect(const CHAR *szSQL, PDBBatchParam *Param, UINT32 ulExecNum);

    /* 查询 */
    INT32 Query(const CHAR *szSQL, PDBRsltParam &Param);

    /* 获取查询的一行数据 */
    PDBRET RowFetch(VOID);

    /* 取数据结束后，释放qry的数据库资源 */
    BOOL CloseQuery(VOID);

    /* 转换sql语句为标准形式 */
    BOOL ConvertSql(CHAR *szSQL);

    INT32 GetErrCode(VOID);
    CHAR* GetErrInfo(CHAR* szErrInfo=NULL);
    PDBRET ReConnect(VOID);
    BOOL CheckConnStat(VOID);

private:
    PDBEnvBase  *m_pPDBEnv;
    PDBConnBase *m_pPDBConn;
    INT32       m_ulConnId;
    BOOL        m_bIsFetchReady;
    UINT32      m_ulQryType;
    CHAR        m_acTransName[PDB_MAX_TRANS_LEN+1];

    CHAR        m_acTmpSQL[PDB_MAX_SQL_LEN+1];

    BOOL    Init(VOID);

    CHAR* BindUpdateParam(CHAR *szSQL, VECTOR<PDB_PARAM_T> &vParam);
    CHAR* BindUpdateBatchParam(CHAR *szSQL, PDBBatchParam *Param);
    CHAR* BindQueryParam(CHAR *szSQL, VECTOR<PDB_PARAM_T> &vParam);
    INT32 Update(BOOL bAutoCommit, CHAR *szSQL, VECTOR<PDB_PARAM_T> &vParam);
    INT32 Update(BOOL bAutoCommit, CHAR *szSQL);
    INT32 UpdateBatch(BOOL bAutoCommit, CHAR *szSQL, PDBBatchParam *Param, UINT32 ulExecNum);
};

#endif
