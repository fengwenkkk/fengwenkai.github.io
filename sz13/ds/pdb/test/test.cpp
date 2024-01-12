/******************************************************************************
模块名          ：  Database Call Interface (DCI)
文件名          ：  test.cpp
相关文件        ：
文件实现功能    ：  测试
作者            ：
版本            ：  V0.1
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <io.h>

#include "pdb_dcci.h"
#include "test.h"

#define ENV_CHECK \
{\
    if (NULL == g_pEnv) \
    return FALSE;   \
}

#define BIND_RSLT_BY_POS(usRsltIndex, pData, FieldType, pFieldLen, pbIsNull, piLenInd) \
if (PDB_SUCCESS != pDbConn->BindRsltByPos(usRsltIndex+1, FieldType, pData, pFieldLen, pbIsNull, piLenInd)) \
{ \
    PDBLog(LOG_ERROR, " PDBApp::AnalyseSql bind rslt by pos fail "); \
    { \
        return NULL; \
    } \
};\
usRsltIndex++;

/* 按位置绑定入参 */
#define  BIND_IN_BY_POS(pData, FieldType, FieldLen, ulInoutType) \
if (PDB_SUCCESS != pDbConn->BindByPos(FieldType, pData, FieldLen, ulInoutType, NULL)) \
{ \
    PDBLog(LOG_ERROR, " PDBApp::AnalyseSql bind in by pos fail "); \
    { \
        return NULL; \
    } \
}

/*
//((UINT32)&FieldName - (UINT32)&stTable + ulSkipLen)
#define BIND_RSLT_BY_POS(usRsltIndex, pData, FieldType, pFieldLen, piLenInd) \
usRsltIndex++;  \
pDbConn->BindRsltByPos(usRsltIndex, FieldType, pData, pFieldLen, piLenInd);

#define  BIND_IN_BY_POS(pData, FieldType, FieldLen, ulInoutType) \
pDbConn->BindByPos(FieldType, pData, FieldLen, ulInoutType, NULL);
*/


#define PDB_SQL_RETURN \
{\
    if (iOffset > PDB_MAX_SQL_LEN)\
    return FALSE;\
    else\
    return TRUE;\
}

PDBEnvBase  *g_pEnv = NULL;

/**********************************************************************
 函    数: pdb_ini_env
 功    能: 初始化数据库环境
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
#if 0
BOOL pdb_ini_env(PDBEnvBase **pEnv,
                    PDB_CONNPARA_T *pDbConPara,
                    UINT32 ulDbConNum)
{
    UINT32  i;
    PDBRET      enRet = PDB_ERROR;
    PDBConn *pDbConn = NULL;

    if (NULL == pEnv)
    {
        return FALSE;
    }

    FREE(g_pEnv);

    g_pEnv = new PDBEnv();
    /*初始化数据库环境*/
    if (PDB_SUCCESS != g_pEnv->CrerateEvn())
    {
        printf("CrerateEvn error \n");
        return FALSE;
    }

    /* 创建数据库连接 */
    for (i=0; i<ulDbConNum; i++)
    {
        g_pEnv->CreateConn();
        if (NULL == pDbConn)
        {
            printf("CreateConn error \n");
            return FALSE;
        }

        /* 注册打印函数 */
        pDbConn->RegPrnFunc(pdb_err_printf, 0);

        enRet = pDbConn->Connect(pDbConPara->acServer,
                                    pDbConPara->ulPort,
                                    pDbConPara->acDBName,
                                    pDbConPara->acUserName,
                                    pDbConPara->acPassword,
                                    &pDbConPara->stOption);
        if (PDB_SUCCESS != enRet)
        {
            printf("Connect error \n");
            return FALSE;
        }
    }

    *pEnv = g_pEnv;

    return TRUE;
}
#endif

/**********************************************************************
 函    数: pdb_ini_env
 功    能: 释放数据库环境，在退出时调用
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
#if 0
BOOL pdb_free_env()
{
    g_pEnv->FreeEnv();

    FREE(g_pEnv);

    return true;
}
#endif
/**********************************************************************
函    数: bind_mark
功    能: 数据库绑定标志
输入参数:
全局变量:
返回结果:
************************************************************************/
CHAR* pdb_bind_mark(VOID)
{
    static CHAR acMark[32];

    sprintf(acMark, "%c", PDB_BIND_MARK);

    return acMark;
}

/**********************************************************************
函    数: pdb_convert_sql
功    能: 数据库脚本转换
输入参数:
全局变量:
返回结果:
************************************************************************/
BOOL pdb_convert_sql(CHAR *szSQL)
{
#ifdef PDB_ORACLE
    static CHAR     *szTmpSQL = NULL;
    UINT32          i;
    UINT32          j = 0;
    UINT32          ulBindIndex = 1;

    if (!szTmpSQL)
    {
        szTmpSQL = (CHAR*)malloc(PDB_MAX_SQL_LEN*2);
        if (!szTmpSQL)
        {
            return FALSE;
        }
    }

    j = 0;
    for(i=0; ;i++)
    {
        if (szSQL[i] == '\0')
        {
            szTmpSQL[j++] = '\0';
            break;
        }

        if (szSQL[i] == PDB_BIND_MARK)
        {
            j += sprintf(szTmpSQL+j, ":%d", ulBindIndex++);
        }
        else
        {
            szTmpSQL[j++] = szSQL[i];
        }
    }

    if (strlen(szTmpSQL) > PDB_MAX_SQL_LEN)
    {
        return FALSE;
    }

    strcpy(szSQL, szTmpSQL);
#endif

    return TRUE;
}

/**********************************************************************
函    数: pdb_init_multi_ins_sql
功    能: 初始化批量insert语句
输入参数:
全局变量:
返回结果:
************************************************************************/
BOOL pdb_init_multi_ins_sql(CHAR *szSQL, UINT16 usBatchNum)
{
    INT32   i = 0;
    INT32   j = 0;
    INT32   ulFieldNum = 10;
    INT32   iOffset = 0;

    iOffset += sprintf(szSQL+iOffset, "INSERT INTO table2 (partid,id,f_int8,f_int16, f_int32,"
                        "f_int64, f_uint8,f_uint16, f_uint32,f_float) "  //,f_string,f_byte
                        "VALUES");

    for (i=0; i<usBatchNum; i++)
    {
        if (i > 0)
        {
            iOffset += sprintf(szSQL+iOffset, ",");
        }

        iOffset += sprintf(szSQL+iOffset, "(");
        for (j = 0; j < ulFieldNum; j++)
        {
            if (j > 0)
            {
                iOffset += sprintf(szSQL+iOffset, ",");
            }

            iOffset += sprintf(szSQL+iOffset, "%s", pdb_bind_mark());
        }
        iOffset += sprintf(szSQL+iOffset, ")");
    }

    PDB_SQL_RETURNING;

    PDB_SQL_RETURN;
}

/**********************************************************************
函    数: init_multi_mod_sql
功    能: 初始化批量insert语句
输入参数:
全局变量:
返回结果:
************************************************************************/
BOOL pdb_init_multi_mod_sql(CHAR *szSQL, UINT16 usBatchNum)
{

    INT32   iOffset = 0;

    iOffset += sprintf(szSQL+iOffset, "UPDATE table2 SET f_int16=?, f_float=? WHERE id=? ");

    PDB_SQL_RETURNING;
    PDB_SQL_RETURN;
}

/**********************************************************************
函    数: init_multi_del_sql
功    能: 初始化批量insert语句
输入参数:
全局变量:
返回结果:
************************************************************************/
BOOL pdb_init_multi_del_sql(CHAR *szSQL, UINT16 usBatchNum)
{
    INT32   i = 0;
    INT32   j = 0;
    INT32   iOffset = 0;

    iOffset += sprintf(szSQL+iOffset, "DELETE FROM table2 WHERE id in (");

    for (i=0; i<usBatchNum; i++)
    {
        if (i > 0)
        {
            iOffset += sprintf(szSQL+iOffset, ",");
        }

        iOffset += sprintf(szSQL+iOffset, "%s", pdb_bind_mark());

    }
    iOffset += sprintf(szSQL+iOffset, ")");

    //iOffset += sprintf(szSQL+iOffset, "DELETE FROM table2 WHERE id=?;");

    PDB_SQL_RETURNING;

    PDB_SQL_RETURN;
}

/**********************************************************************
 函    数: pdb_test_qry
 功    能: 数据库select操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_qry(PDBConnBase *pDbConn)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    //CHAR      *szTransName = "qry";
    UINT16  usRsltIndex = 0;
    UINT32  ulRecNum = 0;
    UINT32  ulAffectedRow = 0;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32  ulMinID =0;
    UINT32  ulMaxID = 52;
    TABLE2_T    stTable = {0};
    /* 查询10列 */
    INT32   aiLenInd[10];
    UINT32  ulLen;

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    memset(aiLenInd, PDB_DATA_IS_NOT_NULL, sizeof(aiLenInd));

    /*设置SQL语言*/
    sprintf(acSql, "SELECT partid,id,f_int8,f_int16, f_int32,"
                    "f_int64, f_uint8,f_uint16, f_uint32,f_float"
                    " from table2");  //WHERE id > ? AND id < ?"); //,f_byte, f_string

    /*
    "SELECT partid,id,f_int8,f_int16, f_int32,"
                    "f_int64, f_uint8,f_uint16, f_uint32,f_float from table2"
    */

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /*  初始化该连接参数：
        默认为单次操作，且执行一次；
        批量需用 SetMultiActParam设置批量参数：
            ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
    */
    pDbConn->Clear();

    /* 绑定入参 */
    /*
    pDbConn->BindByPos(PDBT_UINT32, &ulMinID, sizeof(UINT32), FALSE);
    pDbConn->BindByPos(PDBT_UINT32, &ulMaxID, sizeof(UINT32), FALSE);
    */

    /*绑定出参*/
    ulLen = sizeof(stTable.ulPartID);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulPartID, PDBT_UINT32, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.ulID);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulID, PDBT_UINT32, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.cInt8);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.cInt8, PDBT_INT8, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.sInt16);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.sInt16, PDBT_INT16, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.iInt32);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.iInt32, PDBT_INT32, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.i64Int64);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.i64Int64, PDBT_INT64, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.ucInt8);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ucInt8, PDBT_UINT8, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.usInt16);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.usInt16, PDBT_UINT16, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.ulInt32);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulInt32, PDBT_UINT32, &ulLen, &aiLenInd[usRsltIndex]); //
    ulLen = sizeof(stTable.fFloat);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.fFloat, PDBT_FLOAT, &ulLen, &aiLenInd[usRsltIndex]); //
    //BIND_RSLT_BY_POS(usRsltIndex, &stTable.acString, PDBT_STRING, sizeof(stTable.acString));
    //BIND_RSLT_BY_POS(usRsltIndex, &stTable.aucByte, PDBT_BYTE, sizeof(stTable.aucByte));

    /* 进行数据库查询 */
    /* 采用光标方式(不能和客户端缓存方式同时使用) */
#if 1
    enRet = pDbConn->OpenCursor();
    if (PDB_SUCCESS != enRet)
    {
        return FALSE;
    }
#endif

    /* handle中的transName在transcommit或transroolback中才置空 */
    enRet = pDbConn->Query((CHAR*)NULL, acSql);
    if (PDB_SUCCESS != enRet)
    {
        return FALSE;
    }

    /*
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulPartID, PDBT_UINT32, sizeof(stTable.ulPartID));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulID, PDBT_UINT32, sizeof(stTable.ulID));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.cInt8, PDBT_INT8, sizeof(stTable.cInt8));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.sInt16, PDBT_INT16, sizeof(stTable.sInt16));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.iInt32, PDBT_INT32, sizeof(stTable.iInt32));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.i64Int64, PDBT_INT64, sizeof(stTable.i64Int64));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ucInt8, PDBT_UINT8, sizeof(stTable.ucInt8));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.usInt16, PDBT_UINT16, sizeof(stTable.usInt16));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulInt32, PDBT_UINT32, sizeof(stTable.ulInt32));
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.fFloat, PDBT_FLOAT, sizeof(stTable.fFloat));
    */

    /* 绑定结果集 */
    enRet = pDbConn->BindRslt();
    if (PDB_SUCCESS != enRet)
    {
        return FALSE;
    }

    /* 采用客户端缓存方式 */
#if 0
    enRet = pDbConn->StoreClientQryRlt();
    if (PDB_SUCCESS != enRet)
    {
        return FALSE;
    }
#endif

    /* 开始取查询结果 */
    ulRecNum = 0;
    printf("QRY TABLE1 TEST \n");
    printf("partid | id | f_int8 | f_int16 | f_int32 | f_int64 | "
            "f_uint8 | f_uint16 | f_uint32 | f_float \t\n\n");
            //| f_string |f_byte \t\n");
    do
    {
        enRet = pDbConn->RowFetch();
        // 成功获得行返回PDB_CONTINUE；无数据返回PDB_NO_DATA
        if (PDB_CONTINUE == enRet)
        {
#if 0
            if (PDB_DATA_IS_NULL == aiLenInd[0])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.ulPartID);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[1])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.ulID);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[2])
            {
                printf("NULL |");
            }
            else
            {
                printf("%d |", stTable.cInt8);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[3])
            {
                printf("NULL |");
            }
            else
            {
                printf("%hd |", stTable.sInt16);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[4])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.iInt32);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[5])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.i64Int64);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[6])
            {
                printf("NULL |");
            }
            else
            {
                printf("%d |", stTable.ucInt8);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[7])
            {
                printf("NULL |");
            }
            else
            {
                printf("%hd |", stTable.usInt16);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[8])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.ulInt32);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[9])
            {
                printf("NULL |");
            }
            else
            {
                printf("%5.3Lf \t\n", stTable.fFloat);
            }
#endif
            //printf("%s \t\n", stTable.acString);

            /*
            printf("%ld | %ld | %d | %hd | %ld | %ld | %d | %hd | %ld | %5.3Lf | %s \t\n",
                stTable.ulPartID, stTable.ulID, stTable.cInt8, stTable.sInt16,
                stTable.iInt32, stTable.i64Int64, stTable.ucInt8, stTable.usInt16,
                stTable.ulInt32, stTable.fFloat, stTable.acString); //, stTable.acString, stTable.aucByte);
            */

            //memset(&stTable, 0, sizeof(stTable));
            ulRecNum++;
        }

    } while (PDB_CONTINUE == enRet);

    /* 全部行取结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("qry [test row num : %d]; [affect num: %d] \n",  // [dci row num: %d];
            ulRecNum,
            pDbConn->GetRowCount());
            //ulAffectedRow);

    enRet = pDbConn->FreeRslt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test qry free qry rlt error");
        return FALSE;
    }

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test qry close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("qry record = %d, time = %d, rate = %.1f\n",
            ulRecNum, ulStart, (FLOAT)ulStart/ulRecNum);

    if (enRet != PDB_SUCCESS)
    {
        return FALSE;
    }

    return TRUE;

}

/**********************************************************************
 函    数: pdb_test_ins
 功    能: 数据库insert操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_ins(PDBConnBase *pDbConn)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    CHAR        *szTransName = "inst";
    UINT32   ulExecNum = 50000;
    UINT16  usRsltIndex = 0;
    UINT32  ulRecNum = 0;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32  ulAffectedRow = 0;
    UINT64  uTmp;
    TABLE1_T    stTable = {0};
    UINT32  ulMbLen = 85535; //pow(2, 24)-1;
    UINT32  ulLbLen = 1055350; //1024*1024*32;

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    /*设置SQL语言 */
    /*
    sprintf(acSql, "INSERT INTO table2 (partid,id,f_int8,f_int16, "
                    "f_int32,f_int64, f_uint8,f_uint16, f_uint32,f_float) " //,f_string,f_byte
                    "VALUES(?,?,?,?,?,?,?,?,?,?)"); //,?,?
                    */
    sprintf(acSql, "INSERT INTO table1 (partid,id,f_int8,f_int16, "
        "f_int32, f_int64,f_uint8, f_uint16, "
        "f_uint32, f_uint64, f_decimal, f_float, f_double, f_string, f_bit, " //f_bit,
        "f_tinyblob, f_blob, , f_longblob) " //f_mediumblob
                    "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"); //,?,?
    pdb_convert_sql(acSql);

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /* 事务开始 */
    enRet = pDbConn->TransBegin(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test insert trans begin error");
        return FALSE;
    }

    //memset(&stTable, 0x45, sizeof(stTable));
    stTable.ulPartID = 4;
    stTable.ulID = 4;
    stTable.cInt8 = -pow(2, 7);
    stTable.sInt16 = -pow(2, 15);
    stTable.iInt32 = -pow(2, 31);
    stTable.i64Int64 = -pow(2, 63);
    stTable.ucInt8 = 255;
    stTable.usInt16 = 65535;
    stTable.ulInt32 = pow(2, 32) - 1;
    uTmp = pow(2, 64);
    stTable.u64Int64 = uTmp - 1;
    stTable.fDecimal = 33.666F;
    stTable.fFloat = 556.56F;
    stTable.dDouble = 9999999.23;
    sprintf(stTable.acString, "test");
    stTable.bit1 = 0;

    stTable.pMediumBlob = (UINT8*)malloc(ulMbLen); //pow(2, 24)-1
    if (NULL == stTable.pMediumBlob)
    {
        printf("test insert pMediumBlob error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }

        return FALSE;
    }
    stTable.pLongBlob = (UINT8*)malloc(ulLbLen);
    if (NULL == stTable.pLongBlob)
    {
        printf("test insert pLongBlob error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }

        return FALSE;
    }

    memset(stTable.tinyBlob, 0xcc, sizeof(stTable.tinyBlob));
    memset(stTable.blob, 0x55, sizeof(stTable.blob));
    memset(stTable.pMediumBlob, 0x22, ulMbLen);
    memset(stTable.pLongBlob, 0xee, ulLbLen);
    //memset((VOID*)stTable.bit, 1, 1);

    //for (i=0; i<ulExecNum; i++)
    //{
        //stTable.ulID = i;

        /*  初始化该连接参数：
            默认为单次操作，且执行一次；
            批量需用 SetMultiActParam设置批量参数：
                ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
        */
        enRet = pDbConn->Clear();
        if (PDB_SUCCESS != enRet)
        {
            printf("test insert clear error");

            enRet = pDbConn->TransRollback(szTransName);
            if (PDB_SUCCESS != enRet)
            {
                printf("PDBApp::ExeSql trans rollback error");
                return FALSE;
            }
            return FALSE;
        }

        /* 绑定入参 */
#if 0
        BIND_IN_BY_POS(&stTable.ulPartID, PDBT_UINT32, sizeof(stTable.ulPartID), PDB_PARAM_INPUT);
#endif
        BIND_IN_BY_POS(NULL, PDBT_NULL, sizeof(NULL), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.ulID, PDBT_UINT32, sizeof(stTable.ulID),PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.cInt8, PDBT_INT8, sizeof(stTable.cInt8), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.sInt16, PDBT_INT16, sizeof(stTable.sInt16), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.iInt32, PDBT_INT32,sizeof(stTable.iInt32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.i64Int64, PDBT_INT64,sizeof(stTable.i64Int64), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.ucInt8, PDBT_UINT8,sizeof(stTable.ucInt8), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.usInt16, PDBT_UINT16,sizeof(stTable.usInt16), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.ulInt32, PDBT_UINT32,sizeof(stTable.ulInt32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.u64Int64, PDBT_UINT64,sizeof(stTable.u64Int64), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.fDecimal, PDBT_DECIMAL,sizeof(stTable.fDecimal), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.fFloat, PDBT_FLOAT,sizeof(stTable.fFloat), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.dDouble, PDBT_DOUBLE,sizeof(stTable.dDouble), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.acString, PDBT_STRING,sizeof(stTable.acString), PDB_PARAM_INPUT);
        /* MYSQL的BIT数据类型：作入参时必须用long long类型进行绑定；查询的结果集可获取BIT数据类型 */
        BIND_IN_BY_POS(&stTable.bit1, PDBT_UINT64, sizeof(stTable.bit1), PDB_PARAM_INPUT); // bit类型入参的绑定处理
        /* MYSQL的NULL字段：入参绑定时，bind的buffer字段可任意指定，不必特意指定为NULL亦可 */
        BIND_IN_BY_POS(NULL, PDBT_NULL, sizeof(NULL), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(NULL, PDBT_NULL, sizeof(NULL), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(NULL, PDBT_NULL, sizeof(NULL), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(NULL, PDBT_NULL, sizeof(NULL), PDB_PARAM_INPUT);
#if 0
        BIND_IN_BY_POS(&stTable.tinyBlob, PDBT_TINYBLOB,sizeof(stTable.tinyBlob), FALSE);
        BIND_IN_BY_POS(&stTable.blob, PDBT_BLOB,sizeof(stTable.blob), FALSE);
        BIND_IN_BY_POS(stTable.pMediumBlob, PDBT_MEDIUMBLOB, ulMbLen, FALSE);
        BIND_IN_BY_POS(stTable.pLongBlob, PDBT_LONGBLOB, ulLbLen, FALSE);
        //BIND_IN_BY_POS(&stTable.aucByte, PDBT_BYTE,sizeof(stTable.aucByte), FALSE);
#endif

        /* 执行SQL语句 */
        enRet = pDbConn->Exec(szTransName, acSql);
        if (PDB_SUCCESS != enRet)
        {
            printf("test insert exec sql error");

            enRet = pDbConn->TransRollback(szTransName);
            if (PDB_SUCCESS != enRet)
            {
                printf("PDBApp::ExeSql trans rollback error");
                return FALSE;
            }
            return FALSE;
        }

        ulRecNum += pDbConn->GetRowCount();
    //}

    /* 事务提交 */
    enRet = pDbConn->TransCommit(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test insert trans commit error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 执行结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("insert [affect num: %d] \n", ulRecNum);

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test insert close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("multi insert record = %d, time = %d, rate = %.1f\n",
            ulExecNum, ulStart, (FLOAT)ulStart/ulExecNum);

    return TRUE;
}

/**********************************************************************
 函    数: pdb_test_mod
 功    能: 数据库modify操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_mod(PDBConnBase *pDbConn)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    CHAR        *szTransName = "inst";
    UINT32   ulExecNum = 50000;
    UINT16  usRsltIndex = 0;
    UINT32  ulRecNum = 0;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32     i;
    UINT32  ulAffectedRow = 0;
    TABLE2_T    stTable = {0};

    /*
    INT8 cInt8;
    INT16 sInt16;
    UINT32 ulID;
    */

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    /*设置SQL语言 */
    sprintf(acSql, "UPDATE table2 SET f_int8=?, f_int16=? "
                    "WHERE id=?;");

    //"UPDATE table1 SET f_int8=?, f_int16=?, f_float=?, f_string=?, f_byte=? "
    //              "WHERE id=?;"

    pdb_convert_sql(acSql);

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /* 事务开始 */
    enRet = pDbConn->TransBegin(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test modify trans begin error");
        return FALSE;
    }

    stTable.cInt8 = 88;
    //stTable.fFloat = 8.9f;
    //sprintf(stTable.acString, "updatestr");
    //memset(stTable.aucByte, 0xcc, sizeof(stTable.aucByte));
    //stTable.ulPartID = 1;
    stTable.ulID = 2;

    for (i=0; i<ulExecNum; i++)
    {
        /*  初始化该连接参数：
            默认为单次操作，且执行一次；
            批量需用 SetMultiActParam设置批量参数：
                ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
        */
        enRet = pDbConn->Clear();
        if (PDB_SUCCESS != enRet)
        {
            printf("test modify clear error");

            enRet = pDbConn->TransRollback(szTransName);
            if (PDB_SUCCESS != enRet)
            {
                printf("PDBApp::ExeSql trans rollback error");
                return FALSE;
            }
            return FALSE;
        }

        stTable.sInt16 = 258+i;

        /* 绑定入参 */
        /*
        BIND_IN_BY_POS(stTable, stTable.cInt8, PDBT_INT8, FALSE, 0);
        BIND_IN_BY_POS(stTable, stTable.sInt16, PDBT_INT32, FALSE, 0);
        BIND_IN_BY_POS(stTable, stTable.fFloat, PDBT_FLOAT, FALSE, 0);
        BIND_IN_BY_POS(stTable, stTable.acString, PDBT_STRING, FALSE, 0);
        BIND_IN_BY_POS(stTable, stTable.aucByte, PDBT_STRING, FALSE, 0);
        //BIND_IN_BY_POS(stTable, stTable.ulPartID, PDBT_UINT32, FALSE, 0);
        BIND_IN_BY_POS(stTable, stTable.ulID, PDBT_DOUBLE, FALSE, 0);
        */

        BIND_IN_BY_POS(&stTable.cInt8, PDBT_INT8, sizeof(stTable.cInt8), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&stTable.sInt16, PDBT_INT16, sizeof(stTable.sInt16), PDB_PARAM_INPUT);
        //BIND_IN_BY_POS(stTable, stTable.fFloat, PDBT_FLOAT, FALSE, 0);
        //BIND_IN_BY_POS(stTable, stTable.acString, PDBT_STRING, FALSE, 0);
        //BIND_IN_BY_POS(stTable, stTable.aucByte, PDBT_STRING, FALSE, 0);
        BIND_IN_BY_POS(&stTable.ulID, PDBT_INT32, sizeof(stTable.ulID), PDB_PARAM_INPUT);

        /* 执行SQL语句 */
        enRet = pDbConn->Exec(szTransName, acSql);
        if (PDB_SUCCESS != enRet)
        {
            printf("test modify exec sql error");

            enRet = pDbConn->TransRollback(szTransName);
            if (PDB_SUCCESS != enRet)
            {
                printf("PDBApp::ExeSql trans rollback error");
                return FALSE;
            }
            return FALSE;
        }

        ulRecNum += pDbConn->GetRowCount();
    }

    /* 事务提交 */
    enRet = pDbConn->TransCommit(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test modify trans commit error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 执行结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("modify [affect num: %d] \n", ulRecNum);

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test modify close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("multi mod record = %d, time = %d, rate = %.1f\n",
            ulExecNum, ulStart, (FLOAT)ulStart/ulExecNum);

    return TRUE;
}

/**********************************************************************
 函    数: pdb_test_mod
 功    能: 数据库delete操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_del(PDBConnBase *pDbConn)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    CHAR        *szTransName = "inst";
    UINT32   ulExecNum = 50000;
    UINT16  usRsltIndex = 0;
    UINT32  ulRecNum = 0;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32   i;
    UINT32  ulAffectedRow = 0;
    UINT32  ulID;

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    /*设置SQL语言 */
    sprintf(acSql, "DELETE FROM table2 WHERE id=?;");
    pdb_convert_sql(acSql);

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /* 事务开始 */
    enRet = pDbConn->TransBegin(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test del trans begin error");
        return FALSE;
    }

    ulID = 22;

    for (i=0; i<ulExecNum; i++)
    {
        /*  初始化该连接参数：
            默认为单次操作，且执行一次；
            批量需用 SetMultiActParam设置批量参数：
                ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
        */
        enRet = pDbConn->Clear();
        if (PDB_SUCCESS != enRet)
        {
            printf("test del clear error");

            enRet = pDbConn->TransRollback(szTransName);
            if (PDB_SUCCESS != enRet)
            {
                printf("PDBApp::ExeSql trans rollback error");
                return FALSE;
            }
            return FALSE;
        }

        /* 绑定入参 */
        BIND_IN_BY_POS(&ulID, PDBT_INT32, sizeof(UINT32), PDB_PARAM_INPUT);

        /* 执行SQL语句 */
        enRet = pDbConn->Exec(szTransName, acSql);
        if (PDB_SUCCESS != enRet)
        {
            printf("test del exec sql error");

            enRet = pDbConn->TransRollback(szTransName);
            if (PDB_SUCCESS != enRet)
            {
                printf("PDBApp::ExeSql trans rollback error");
                return FALSE;
            }
            return FALSE;
        }

        ulRecNum += pDbConn->GetRowCount();

    }

    /* 事务提交 */
    enRet = pDbConn->TransCommit(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test del trans commit error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 执行结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("delete [affect num: %d] \n", ulRecNum);

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test del close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("multi del record = %d, time = %d, rate = %.1f\n",
            ulExecNum, ulStart, (FLOAT)ulStart/ulExecNum);

    return TRUE;
}

/**********************************************************************
 函    数: pdb_test_bulk_ins
 功    能: 数据库bulk insert操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
#if 0
BOOL pdb_test_bulk_ins(PDBConnBase *pDbConn, UINT32 ulExecNum)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    CHAR        *szTransName = "bulkinst";
    UINT16  usRsltIndex = 0;
    UINT16   usBatchNum = PDB_BATCH_EXEC_NUM;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32   i;
    UINT32  ulAffectedRow = 0;
    UINT32  ulPartID = time(NULL);
    UINT32  *pulRet;
    TABLE2_T    *pstTable;

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    pstTable = (TABLE2_T*)malloc(ulExecNum*sizeof(TABLE2_T));
    if (NULL == pstTable)
    {
        printf("test bulk ins pstTable is null");
        return FALSE;
    }
    memset(pstTable, 0, ulExecNum*sizeof(TABLE2_T));

    pulRet = (UINT32*)malloc(ulExecNum*sizeof(UINT32));
    if (NULL == pulRet)
    {
        printf("test bulk ins pulRet is null");
        return FALSE;
    }

    /*设置SQL语言 */
    pdb_init_multi_ins_sql(acSql, usBatchNum);
    pdb_convert_sql(acSql);

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /* 事务开始 */
    enRet = pDbConn->TransBegin(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk ins trans begin error");
        return FALSE;
    }

    /* 设置入参值 */
    for (i=0; i<ulExecNum; i++)
    {
        pstTable[i].ulPartID = i+1;
        pstTable[i].ulID = i*3+1;
        pstTable[i].cInt8 = i+3;
        pstTable[i].sInt16 = i*2+1;
        pstTable[i].iInt32 = i*4+1;
        pstTable[i].ucInt8 = i+2;
        pstTable[i].i64Int64 = i*5+1;
        pstTable[i].usInt16 = i*6+1;
        pstTable[i].ulInt32 = i*7+1;
        pstTable[i].fFloat = i*8+1;

        //sprintf(pstTable[i].acString, "bulkinset");

        /* byte数组对应的数据库数据类型为MYSQL_TYPE_MEDIUM_BLOB，不能是string，因为没有结束符 */
        //memset(pstTable[i].aucByte, 0xcc, sizeof(pstTable[i].aucByte));
    }

    /*  初始化该连接参数：
        默认为单次操作，且执行一次；
        批量需用 SetMultiActParam设置批量参数：
            ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
    */
    enRet = pDbConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk ins clear error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    enRet = pDbConn->SetMultiActParam(ulExecNum, usBatchNum, sizeof(TABLE2_T), sizeof(UINT32));
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk ins set multi act param error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    for (i=0; i<usBatchNum; i++)
    {
        BIND_IN_BY_POS(&pstTable[i].ulPartID, PDBT_UINT32, sizeof(pstTable[i].ulPartID), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].ulID, PDBT_UINT32, sizeof(pstTable[i].ulID),PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].cInt8, PDBT_INT8, sizeof(pstTable[i].cInt8), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].sInt16, PDBT_INT16, sizeof(pstTable[i].sInt16), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].iInt32, PDBT_INT32,sizeof(pstTable[i].iInt32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].i64Int64, PDBT_INT64,sizeof(pstTable[i].i64Int64), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].ucInt8, PDBT_UINT8,sizeof(pstTable[i].ucInt8), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].usInt16, PDBT_UINT16,sizeof(pstTable[i].usInt16), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].ulInt32, PDBT_UINT32,sizeof(pstTable[i].ulInt32), PDB_PARAM_INPUT);
        BIND_IN_BY_POS(&pstTable[i].fFloat, PDBT_FLOAT,sizeof(pstTable[i].fFloat), PDB_PARAM_INPUT);

        //BIND_IN_BY_POS(&pstTable[i].acString, PDBT_STRING,sizeof(pstTable[i].acString), FALSE);
        //BIND_IN_BY_POS(&pstTable[i].aucByte, PDBT_BYTE,sizeof(pstTable[i].aucByte), FALSE);
    }

#ifdef PDB_ORACLE
    memset(pulRet, 0xff, ulExecNum*sizeof(UINT32));
    enRet = pDbConn->BindReturning(pulRet);
    if (!enRet)
    {
        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }
#endif

    /* 执行SQL语句 */
    enRet = pDbConn->Exec(szTransName, acSql);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk insert exec sql error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 事务提交 */
    enRet = pDbConn->TransCommit(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk insert trans commit error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 执行结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("bulk insert [affect num: %d] \n", pDbConn->GetRowCount());

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk insert close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("bulk insert record = %d, time = %d, rate = %.1f\n",
            ulExecNum, ulStart, (FLOAT)ulStart/ulExecNum);
    return TRUE;
}

/**********************************************************************
 函    数: pdb_test_bulk_mod
 功    能: 数据库bulk update操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_bulk_mod(PDBConnBase *pDbConn, UINT32 ulExecNum)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    CHAR        *szTransName = "bulkmod";
    UINT16  usRsltIndex = 0;
    UINT16   usBatchNum = 1;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32   i;
    UINT32  ulAffectedRow = 0;
    UINT32  ulPartID = time(NULL);
    UINT32  *pulRet;
    TABLE2_T    *pstTable;

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    pstTable = (TABLE2_T*)malloc(ulExecNum*sizeof(TABLE2_T));
    if (NULL == pstTable)
    {
        printf("test bulk mod pstTable is null");
        return FALSE;
    }
    memset(pstTable, 0, ulExecNum*sizeof(TABLE2_T));

    pulRet = (UINT32*)malloc(ulExecNum*sizeof(UINT32));
    if (NULL == pulRet)
    {
        printf("test bulk mod pulRet is null");
        return FALSE;
    }

    /*设置SQL语言 */
    pdb_init_multi_mod_sql(acSql, usBatchNum);
    pdb_convert_sql(acSql);

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /* 事务开始 */
    enRet = pDbConn->TransBegin(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk mod trans begin error");
        return FALSE;
    }

    /* 设置入参值 */
    for (i=0; i<ulExecNum; i++)
    {
        pstTable[i].sInt16 = -10; //(i*2+1)*3;
        pstTable[i].fFloat = 88.888f; //(i*8+1)*3;
        //sprintf(pstTable[i].acString, "hul");
        //pstTable[i].ulPartID = i+1;
        pstTable[i].ulID = i*3+1;
    }

    /*  初始化该连接参数：
        默认为单次操作，且执行一次；
        批量需用 SetMultiActParam设置批量参数：
            ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
    */
    enRet = pDbConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk mod clear error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    enRet = pDbConn->SetMultiActParam(ulExecNum, usBatchNum, sizeof(TABLE2_T), sizeof(UINT32));
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk mod set multi act param error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    //for (i=0; i<usBatchNum; i++)
    //{
    BIND_IN_BY_POS(&pstTable[0].sInt16, PDBT_INT16, sizeof(pstTable[0].sInt16), PDB_PARAM_INPUT);
    BIND_IN_BY_POS(&pstTable[0].fFloat, PDBT_FLOAT, sizeof(pstTable[0].fFloat), PDB_PARAM_INPUT);
    //BIND_IN_BY_POS(&pstTable[0].acString, PDBT_STRING, sizeof(pstTable[0].acString), FALSE);
    BIND_IN_BY_POS(&pstTable[0].ulID, PDBT_UINT32, sizeof(pstTable[0].ulID),PDB_PARAM_INPUT);

#ifdef PDB_ORACLE
    memset(pulRet, 0xff, ulExecNum*sizeof(UINT32));
    enRet = pDbConn->BindReturning(pulRet);
    if (!enRet)
    {
        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }
#endif
    //}


    /* 执行SQL语句 */
    enRet = pDbConn->Exec(szTransName, acSql);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk mod exec sql error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 事务提交 */
    enRet = pDbConn->TransCommit(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk mod trans commit error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 执行结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("bulk mod [affect num: %d] \n", pDbConn->GetRowCount());

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk mod close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("bulk mod record = %d, time = %d, rate = %.1f\n",
            ulExecNum, ulStart, (FLOAT)ulStart/ulExecNum);

    if (pstTable)
    {
        free(pstTable);
    }

    return TRUE;
}

/**********************************************************************
 函    数: pdb_test_bulk_del
 功    能: 数据库bulk delete操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_bulk_del(PDBConnBase *pDbConn, UINT32 ulExecNum)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    CHAR        *szTransName = "bulkdel";
    UINT16  usRsltIndex = 0;
    UINT16   usBatchNum = PDB_BATCH_EXEC_NUM;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32   i;
    UINT32  ulPartID = time(NULL);
    UINT32  *pulRet;
    UINT32  *pulID;

    if (NULL == pDbConn)
    {
        return FALSE;
    }

    pulID = (UINT32*)malloc(ulExecNum*sizeof(UINT32));
    if (NULL == pulID)
    {
        printf("test bulk del pulID is null");
        return FALSE;
    }
    memset(pulID, 0xff, ulExecNum*sizeof(UINT32));

    pulRet = (UINT32*)malloc(ulExecNum*sizeof(UINT32));
    if (NULL == pulRet)
    {
        printf("test bulk del pulRet is null");
        return FALSE;
    }

    /*设置SQL语言 */
    pdb_init_multi_del_sql(acSql, usBatchNum);
    pdb_convert_sql(acSql);

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /* 事务开始 */
    enRet = pDbConn->TransBegin(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk del trans begin error");
        return FALSE;
    }

    /* 设置入参值 */
    for (i=0; i<ulExecNum; i++)
    {
        pulID[i] = i*3+1;
    }

    /*  初始化该连接参数：
        默认为单次操作，且执行一次；
        批量需用 SetMultiActParam设置批量参数：
            ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
    */
    enRet = pDbConn->Clear();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk del clear error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    enRet = pDbConn->SetMultiActParam(ulExecNum, usBatchNum, sizeof(UINT32), sizeof(UINT32));
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk del set multi act param error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    //pData = (PDBUINT8*)pulID;

    for (i=0; i<usBatchNum; i++)
    {
        BIND_IN_BY_POS(&pulID[i], PDBT_UINT32, sizeof(UINT32),PDB_PARAM_INPUT);
    //pData += sizeof(UINT32);
    }

#ifdef PDB_ORACLE
    memset(pulRet, 0xff, ulExecNum*sizeof(UINT32));
    enRet = pDbConn->BindReturning(pulRet);
    if (!enRet)
    {
        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }
#endif

    /* 执行SQL语句 */
    enRet = pDbConn->Exec(szTransName, acSql);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk del exec sql error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 事务提交 */
    enRet = pDbConn->TransCommit(szTransName);
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk del trans commit error");

        enRet = pDbConn->TransRollback(szTransName);
        if (PDB_SUCCESS != enRet)
        {
            printf("PDBApp::ExeSql trans rollback error");
            return FALSE;
        }
        return FALSE;
    }

    /* 执行结束，查看影响行数 */
    printf("bulk del [affect num: %d] \n", pDbConn->GetRowCount());

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bulk del close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("bulk del record = %d, time = %d, rate = %.1f\n",
            ulExecNum, ulStart, (FLOAT)ulStart/ulExecNum);

    return TRUE;
}
#endif

/**********************************************************************
 函    数: pdb_test_qry_table2
 功    能: 数据库select操作
 参数说明:
 全局变量:
 返回结果: TRUE or FALSE
************************************************************************/
BOOL pdb_test_qry_table1(PDBConnBase *pDbConn)
{
    PDBRET      enRet = PDB_ERROR;
    CHAR        acSql[PDB_MAX_SQL_LEN+1];
    //CHAR      *szTransName = "qry";
    UINT16  usRsltIndex = 0;
    UINT32  ulRecNum = 0;
    UINT32  ulAffectedRow = 0;
    UINT32  ulStart = 0;
    UINT32  ulEnd = 0;
    UINT32  ulMinID =0;
    UINT32  ulMaxID = 52;
    UINT32  ulMbLen = pow(2, 24)-1; //pow(2, 24)-1; //85535
    UINT32  ulLbLen = 1024*1024*32; //1024*1024*32; //1055350;
    TABLE1_T    stTable = {0};

    /* 查询4列 */
    INT32   aiLenInd[4];
    UINT32  ulLen;
    //UINT8     blob[pow(2, 24)-1];
    //PDBUINT8  bit:1;

    if (NULL == pDbConn)
    {
        printf("test qry query pDbConn is null");
        return FALSE;
    }

    //pIsNull = (PDBUINT8*)malloc(ulIsNullArrLen);
    memset(aiLenInd, 0, sizeof(aiLenInd));

    printf("dbconn isconnected : %d \n", pDbConn->IsConnected());
    printf("dbconn isal loced : %d \n", pDbConn->IsAlloced());

    /*设置SQL语言*/
    sprintf(acSql, "SELECT partid,id from table1");   //,f_bit,f_mediumblob

    ulStart = time(NULL);

    /* 初始化数据库语句句柄 */
    pDbConn->InitStmt();

    /*  初始化该连接参数：
        默认为单次操作，且执行一次；
        批量需用 SetMultiActParam设置批量参数：
            ulExecNum/usBatchNum/ulInSkipLen/ulOutSkipLen
    */
    pDbConn->Clear();

    /* 绑定入参 */
    /*
    pDbConn->BindByPos(PDBT_UINT32, &ulMinID, sizeof(UINT32), FALSE);
    pDbConn->BindByPos(PDBT_UINT32, &ulMaxID, sizeof(UINT32), FALSE);
    */

    stTable.pMediumBlob = (UINT8*)malloc(ulMbLen); //pow(2, 24)-1
    if (NULL == stTable.pMediumBlob)
    {
        printf("test insert pMediumBlob error");
        return FALSE;
    }
    memset(stTable.pMediumBlob, 0, ulMbLen);

    /*绑定出参*/
    ulLen = sizeof(stTable.ulPartID);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulPartID, PDBT_UINT32, &ulLen, &aiLenInd[usRsltIndex-1]); //
    ulLen = sizeof(stTable.ulID);
    BIND_RSLT_BY_POS(usRsltIndex, &stTable.ulID, PDBT_UINT32, &ulLen, &aiLenInd[usRsltIndex-1]); //
    /* MYSQL BIT字段 */
    //BIND_RSLT_BY_POS(usRsltIndex, &stTable.bit1, PDBT_BIT, 1, &aucNull[usRsltIndex-1]);
    //BIND_RSLT_BY_POS(usRsltIndex, stTable.pMediumBlob, PDBT_MEDIUMBLOB, ulMbLen, &aucNull[usRsltIndex-1]);

    /* 进行数据库查询 */
    /* 采用光标方式(不能和客户端缓存方式同时使用) */
#if 1
    enRet = pDbConn->OpenCursor();
    if (PDB_SUCCESS != enRet)
    {
        printf("test qry open qry cursor error");
        return FALSE;
    }
#endif

    /* handle中的transName在transcommit或transroolback中才置空 */
    enRet = pDbConn->Query((CHAR*)NULL, acSql);
    if (PDB_SUCCESS != enRet)
    {
        printf("test qry query error");
        return FALSE;
    }

    /* 绑定结果集 */
    enRet = pDbConn->BindRslt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test bind qry rlt error");
        return FALSE;
    }

    /* 采用客户端缓存方式 */
#if 0
    enRet = pDbConn->StoreClientQryRlt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test stroe client qry rlt error");
        return FALSE;
    }
#endif

    /* 开始取查询结果 */
    ulRecNum = 0;
    printf("QRY TABLE2 TEST \n");
    printf("partid | id \t\n");
    do
    {
        enRet = pDbConn->RowFetch();
        // 成功获得行返回PDB_CONTINUE；无数据返回PDB_NO_DATA
        if (PDB_CONTINUE == enRet)
        {
            if (PDB_DATA_IS_NULL == aiLenInd[0])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.ulPartID);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[1])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.ulID);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[2])
            {
                printf("NULL |");
            }
            else
            {
                printf("%ld |", stTable.bit1);
            }

            if (PDB_DATA_IS_NULL == aiLenInd[3])
            {
                printf("NULL |");
                memset(stTable.pMediumBlob, 0, ulMbLen);
            }
            else
            {
                printf("MEDIUM BLOB[%d]", ulMbLen);
            }

            printf("\t\n");

            stTable.bit1;
            ulRecNum++;

            /* 重新初始化 */
            //memset(stTable.pMediumBlob, 0, ulMbLen);
        }

    } while (PDB_CONTINUE == enRet);

    /* 全部行取结束，查看影响行数 */
    //pDbConn->RowCount(&ulAffectedRow);
    printf("qry [test row num : %d]; [affect num: %d] \n",  // [dci row num: %d];
            ulRecNum,
            pDbConn->GetRowCount());
            //ulAffectedRow);

    enRet = pDbConn->FreeRslt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test qry free qry rlt error");
        return FALSE;
    }

    /* 关闭stmt */
    enRet = pDbConn->CloseStmt();
    if (PDB_SUCCESS != enRet)
    {
        printf("test qry close stmt error");
        return FALSE;
    }

    ulEnd = time(NULL);
    ulStart = ulEnd - ulStart;
    printf("qry record = %d, time = %d, rate = %.1f\n",
            ulRecNum, ulStart, (FLOAT)ulStart/ulRecNum);

    if (enRet != PDB_SUCCESS)
    {
        return FALSE;
    }

    return TRUE;
}



CHAR* pdb_get_filePath(UINT32   ulIndex,
                         CHAR   *szLogFileDir,
                         CHAR   *szLogFileName)
{
    static CHAR acFilePath[MAX_FILE_PATH_LEN];

    if (0 == ulIndex)
    {
        sprintf(acFilePath, "%s//%s", szLogFileDir, szLogFileName);
    }
    else
    {
        sprintf(acFilePath, "%s//%s%d", szLogFileDir, szLogFileName, ulIndex);
    }

    return acFilePath;
}
