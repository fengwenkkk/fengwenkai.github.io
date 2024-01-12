#include "pdb_base.h"
#include "pdb_def.h"
#include "pdb_stru.h"
#include "pdb_core.h"
#include "pdb_api.h"
#include "pdb_app.h"

#include "test.h"

#define MYSQL_OK 0

#define START_INIT(TableID, TABLE_STRU_T)    \
{       \
    TABLE_STRU_T    *pstTable = NULL;  \
    ulTableID = TableID;        \
    ulFieldIndex = 0;

#define INIT_FIELD(szFieldName, FieldType, FieldName)    \
    pdb_init_field(ulTableID, ulFieldIndex++, szFieldName, FieldType, sizeof(pstTable->FieldName), (UINT32)&pstTable->FieldName);

#define INIT_TABLE(TableName, TableLen, MaxRecNum, FirstKeyField, KeyFieldNum)    \
    pdb_init_table(ulTableID, TableName, szDBName, sizeof(*pstTable), MaxRecNum, FirstKeyField, KeyFieldNum, NULL); \
    }

PDBBOOL pdb_ins_table1(PDBHANDLE    hPDB,
                      PDBUINT32     ulTableID,
                      PDBCHAR       *szTransName,
                      PDBVOID       *pData);

/************************************************************************/
PDBBOOL pdb_init_all_table(VOID)
{
    UINT32      ulTableID = TABLE1;
    UINT32      ulFieldIndex;
    CHAR        *szDBName = "omcv2";

    pdb_init();

    START_INIT(TABLE1, TABLE1_T);

    INIT_FIELD("partid",   PDBT_UINT32, ulPartID);
    INIT_FIELD("id",       PDBT_UINT32, ulID);
    INIT_FIELD("f_Int8",   PDBT_INT8,   cInt8);
    INIT_FIELD("f_Int16",  PDBT_INT16,  sInt16);
    INIT_FIELD("f_Int32",  PDBT_INT32,  iInt32);
    INIT_FIELD("f_Int64",  PDBT_INT64,  i64Int64);
    INIT_FIELD("f_uint8",  PDBT_UINT8,  ucInt8);
    INIT_FIELD("f_uint16", PDBT_UINT16, usInt16);
    INIT_FIELD("f_uint32", PDBT_UINT32, ulInt32);
    INIT_FIELD("f_float",  PDBT_FLOAT,  fFloat);
    INIT_FIELD("f_string", PDBT_STRING, acString);
    //INIT_FIELD("f_byte",   PDBT_BYTE,   aucByte);

    INIT_TABLE("table1", sizeof(*pstTable), 1024, 1, 1);
    //pdb_init_table(ulTableID, "table1", "omcv2", sizeof(TABLE1_T), 1024, 0, 1, NULL);

    if (!pdb_check_table_info())
    {
        return FALSE;
    }

    return TRUE;
}

/************************************************************************/
PDBBOOL pdb_test_get(PDBHANDLE hPDB)
{
    PDBRET      enRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    stTable = {0xdd};

    memset(&stTable, 0xcc, sizeof(stTable));
    stTable.ulID = 2;

    enRet = pdb_get_table(hPDB, ulTableID, &stTable);
    /*
    enRet = pdb_multi_get_table(hPDB, ulTableID, &groupInfo, &groupInfos,
                                sizeof(groupInfo), sizeof(groupInfos), &ulRecNum);
                                */
    return (enRet == PDB_SUCCESS);
}

/************************************************************************/
PDBBOOL pdb_test_ins(PDBHANDLE hPDB)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    stTable = {0xdd};
    UINT32      ulStart, ulEnd;
    UINT16      usExecNum = 50000;
    UINT32      i;
    CHAR        *szTrans = "test";

    memset(&stTable, 0x45, sizeof(stTable));
    stTable.ulID = 2;
    sprintf(stTable.acString, "test");
    memset(stTable.aucByte, 0xbb, sizeof(stTable.aucByte));

    ulStart = time(NULL);

    pdb_trans_begin(hPDB, szTrans);
    for (i=0; i<usExecNum; i++)
    {
        stTable.ulID = i;
        bRet = pdb_ins_table(hPDB, ulTableID, szTrans, &stTable);
    }
    pdb_trans_commit(hPDB, szTrans);

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("multi mod record = %d, time = %d, rate = %.1f\n", usExecNum, ulStart, (FLOAT)ulStart/usExecNum);

    return bRet;
}

/************************************************************************/
PDBBOOL pdb_test_mod(PDBHANDLE hPDB)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    stTable = {0xdd};

    memset(&stTable, 0x56, sizeof(stTable));
    stTable.ulID = 2;
    sprintf(stTable.acString, "leijie");
    memset(stTable.aucByte, 0xaa, sizeof(stTable.aucByte));

    bRet = pdb_mod_table(hPDB, ulTableID, NULL, &stTable);
    return bRet;
}

/************************************************************************/
PDBBOOL pdb_test_del(PDBHANDLE hPDB)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    stTable = {0xdd};

    memset(&stTable, 0x56, sizeof(stTable));
    stTable.ulID = 2;

    bRet = pdb_del_table(hPDB, ulTableID, NULL, &stTable);
    return bRet;
}

/************************************************************************/
PDBBOOL pdb_test_bulk_ins(PDBHANDLE hPDB, UINT16 usExecNum)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    *astTable;
    UINT16      i;
    UINT32      ulPartID = time(NULL);
    UINT32      ulStart;
    UINT32      ulEnd;
    CHAR        *szTransName = "ins";

    astTable = malloc(usExecNum*sizeof(TABLE1_T));
    memset(astTable, 0x23, usExecNum*sizeof(TABLE1_T));

    for (i=0; i<usExecNum; i++)
    {
        astTable[i].ulPartID = i % 8;
        astTable[i].ulID = i;
        sprintf(astTable[i].acString, "%09d", i);
        memset(astTable[i].aucByte, 0x3f, sizeof(astTable[0].aucByte));
    }

    ulStart = GetTickCount();

    pdb_trans_begin(hPDB, szTransName);
    bRet = pdb_multi_ins_table(hPDB, ulTableID, szTransName, usExecNum, sizeof(astTable[0]), astTable, NULL);
    pdb_trans_commit(hPDB, szTransName);

    ulEnd = GetTickCount();
    ulStart = ulEnd - ulStart;
    if (0 == ulStart) ulStart = 1;

    printf("multi ins record = %d, time = %.2f, rate = %.1f\n", usExecNum, ((FLOAT)ulStart)/(FLOAT)1000.0, (FLOAT)1000*usExecNum/ulStart);

    free(astTable);

    return bRet;
}

/************************************************************************/
PDBBOOL pdb_test_bulk_mod(PDBHANDLE hPDB, UINT16 usExecNum)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    *astTable;
    UINT16      i;
    UINT32      ulStart;
    UINT32      ulEnd;
    CHAR        *szTransName = NULL;//"mod";

    astTable = malloc(usExecNum*sizeof(TABLE1_T));
    memset(astTable, 0x78, usExecNum*sizeof(TABLE1_T));

    for (i=0; i<usExecNum; i++)
    {
        astTable[i].ulPartID = i % 8;
        astTable[i].ulID = i;
        sprintf(astTable[i].acString, "%09d", i);
        memset(astTable[i].aucByte, 0x3f, sizeof(astTable[0].aucByte));
    }

    ulStart = time(NULL);

    pdb_trans_begin(hPDB, szTransName);
    bRet = pdb_multi_mod_table(hPDB, ulTableID, szTransName, usExecNum, sizeof(astTable[0]), astTable, NULL);
    pdb_trans_commit(hPDB, szTransName);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    printf("multi mod record = %d, time = %d, rate = %.1f\n", usExecNum, ulStart, (FLOAT)ulStart/usExecNum);

    free(astTable);

    return bRet;
}

/************************************************************************/
PDBBOOL pdb_test_bulk_del(PDBHANDLE hPDB, UINT16 usExecNum)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    TABLE1_T    *astTable;
    UINT16      i;
    UINT32      ulStart;
    UINT32      ulEnd;
    CHAR        *szTransName = NULL;//"del";

    astTable = malloc(usExecNum*sizeof(TABLE1_T));
    memset(astTable, 0x78, usExecNum*sizeof(TABLE1_T));

    for (i=0; i<usExecNum; i++)
    {
        astTable[i].ulID = i;
    }

    ulStart = time(NULL);

    pdb_trans_begin(hPDB, szTransName);
    bRet = pdb_multi_del_table(hPDB, ulTableID, szTransName, usExecNum, sizeof(astTable[0]), astTable, NULL);
    pdb_trans_commit(hPDB, szTransName);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    printf("multi del record = %d, time = %d, rate = %.1f\n", usExecNum, ulStart, (FLOAT)ulStart/usExecNum);

    free(astTable);

    return bRet;
}

#define BIND_PARA(field_type, field_value, field_len) \
{\
    usFieldLen = field_len;\
    if (0 != DCIBindPara(pstDCIHandle, ++usBindIndex, field_type, field_value, usFieldLen)) \
    {\
        return PDB_ERROR;\
    }\
}


/**********************************************************************
 函    数: pdb_ins_table
 功    能: 插入表记录
 输入参数:
 全局变量:
 返回结果:
************************************************************************/
PDBBOOL pdb_ins_table1(PDBHANDLE    hPDB,
                      PDBUINT32     ulTableID,
                      PDBCHAR       *szTransName,
                      PDBVOID       *pData)
{
    PDBRET              enRet = PDB_ERROR;
    PDBBOOL             bRet;
    PDBCHAR             *szSQL;
    PDB_TABLE_INFO_T    *pstTableInfo;
    PDB_HANDLE_T        *pstHandle = (PDB_HANDLE_T*)hPDB;
    DCI_HANDLE_T        *pstDCIHandle;
    PDBStmtHandle       hStmtHandle;
    PDBINT32            iRowCount = 0;
    TABLE1_T            *pstTable = pData;
    PDBUINT16           usBindIndex = 0;
    PDBUINT16           usFieldLen = 0;

    PDB_HANDLE_CHECK(pstHandle);

#if 0

    pstTableInfo = pdb_get_table_info(ulTableID);
    szSQL = pdb_get_sql(ulTableID, PDB_SQL_SYNTAX_INS);

    /* 初始化 */
    PDBClear(pstHandle);

    /* 绑定参数 */
    /*
    bRet = pdb_bind_all_field(pstHandle, pstTableInfo, pData);
    if (!bRet)
    {
        return FALSE;
    }*/

    pstDCIHandle = (DCI_HANDLE_T *)pstHandle->pDCIHandle;

    /* 执行 */
    //enRet = PDBExec(pstHandle, szTransName, szSQL);

    if (szTransName == NULL || szTransName[0] == '\0')
        pstDCIHandle->acTransName[0] = '\0';
    else
        strcpy((PDBCHAR *)pstDCIHandle->acTransName, szTransName);

    strncpy(pstDCIHandle->acSQLCmd, szSQL, PDB_MAX_SQL_LEN-1);
    pstDCIHandle->acSQLCmd[PDB_MAX_SQL_LEN-1] = '\0';

    pstHandle->ulRowCount = 0;
    DCIGetHandle(pstDCIHandle, &hStmtHandle);

    /* SQL Command prepare */
    //enRet = DCISQLPrepare(pstDCIHandle, &hStmtHandle);
    if (MYSQL_OK != mysql_stmt_prepare(hStmtHandle.MySQLStmtHandle,
                                       pstDCIHandle->acSQLCmd, strlen(pstDCIHandle->acSQLCmd)))
    {
        return PDB_ERROR;
    }

    BIND_PARA(PDBT_UINT32  , &pstTable->ulPartID , sizeof(pstTable->ulPartID ));
    BIND_PARA(PDBT_UINT32  , &pstTable->ulID     , sizeof(pstTable->ulID     ));
    BIND_PARA(PDBT_INT8    , &pstTable->cInt8    , sizeof(pstTable->cInt8    ));
    BIND_PARA(PDBT_INT16   , &pstTable->sInt16   , sizeof(pstTable->sInt16   ));
    BIND_PARA(PDBT_INT32   , &pstTable->iInt32   , sizeof(pstTable->iInt32   ));
    BIND_PARA(PDBT_INT64   , &pstTable->i64Int64 , sizeof(pstTable->i64Int64 ));
    BIND_PARA(PDBT_UINT8   , &pstTable->ucInt8   , sizeof(pstTable->ucInt8   ));
    BIND_PARA(PDBT_UINT16  , &pstTable->usInt16  , sizeof(pstTable->usInt16  ));
    BIND_PARA(PDBT_UINT32  , &pstTable->ulInt32  , sizeof(pstTable->ulInt32  ));
    BIND_PARA(PDBT_FLOAT   , &pstTable->fFloat   , sizeof(pstTable->fFloat   ));


    /* Bind parameters */
    /*
    if((enRet = PDBBindPara(pstHandle)) != PDB_SUCCESS)
    {
        return PDB_ERROR;
    }*/

    /*
    for (i=0; i<pstHandle->stBindInfo.ulBindParaNum; i++)
    {
        enRet = DCIBindPara(pstDCIHandle,
                            (PDBUINT16)(i+1),
                            pstHandle->stBindInfo.astBindPara[i].ulType,
                            (PDBVOID*)pstHandle->stBindInfo.astBindPara[i].pValue,
                            (PDBUINT32)pstHandle->stBindInfo.astBindPara[i].iLen);

        PDB_CHECK_ERR(pstHandle, enRet);
    }
*/

    /* Exec SQL Command */
    pstDCIHandle->stOption.usExecNum = pstHandle->usExecNum;
    //enRet = DCIExec(pstDCIHandle, &hStmtHandle);

    pstDCIHandle->usExecErrCount = 0;

    //if (pstDCIHandle->MySQLHandles.ulBindNum > 0)
    if (1)
    {
        if (MYSQL_OK != mysql_stmt_bind_param(hStmtHandle.MySQLStmtHandle, pstDCIHandle->MySQLHandles.BindHandle))
        {
            DCIDiagStmtError(pstDCIHandle);
            pstDCIHandle->usExecErrCount ++;

            return PDB_ERROR_BIND;
        }
    }
    else
    {
        return PDB_ERROR_EXEC;
    }

    if (MYSQL_OK != mysql_stmt_execute(hStmtHandle.MySQLStmtHandle))
    {
        DCIDiagStmtError(pstDCIHandle);
        pstDCIHandle->usExecErrCount ++;

        return PDB_ERROR_EXEC;
    }

    //pstHandle->ulRowCount = mysql_stmt_affected_rows(hStmtHandle.MySQLStmtHandle);
#endif

    PDB_RETURN(enRet);
}
