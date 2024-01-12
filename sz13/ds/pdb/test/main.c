#include "pdb_base.h"
#include "pdb_def.h"
#include "pdb_stru.h"
#include "pdb_core.h"
#include "pdb_api.h"
#include "pdb_app.h"

#include "test.h"

/**
typedef struct
{
    UINT32      ulPartID;
    UINT32      ulKey;
    CHAR        acFieldString[128];
    BYTE        aucFieldRaw[1024];
}TABLE1_T;

typedef struct
{
    UINT32      ulPartID;
    CHAR        acField1[8];
    INT         iField2;
    BYTE        aucField3[4];
}TABLE2_T;


typedef struct
{
    UINT32      ulPartID;
    UINT32      ulKey;
    CHAR        acFieldString[128];
    BYTE        aucFieldRaw[1024];
}TABLE3_T;
*/



typedef struct
{
    UINT32      groupId;
    CHAR        groupName[64];
    UINT32      groupStat;
    //UINT32        partId;
}TABLE_GROUP_INFO;

typedef struct
{
    UINT32      userId;
    CHAR        userName[64];
    UINT32      groupId;
    UINT32      userStat;
    UINT32      partId;
}TABLE_USER_INFO;

typedef struct
{
    UINT32 ulGroupId;
    UINT32 ulOperId;
    UINT32 ulPartId;
} TABLE_PRIV_INFO;


#if 0

PDBBOOL pdb_test_get2(PDB_HANDLE_T *pstHandle)
{
    PDBRET  enRet;
    UINT32  ulTableID = TABLE2;
    TABLE_USER_INFO userInfo = {0xdd};

    memset(&userInfo, 0xdd, sizeof(userInfo));
    userInfo.userId = 2;
    //sprintf(userInfo.userName, "User1");
    //userInfo.groupId = 1;
    //userInfo.userStat = 1;

    enRet = pdb_get_table(pstHandle, ulTableID, &userInfo);
    if (enRet == PDB_SUCCESS)
        return TRUE;

    return FALSE;
}

PDBBOOL pdb_test_ins_priv(PDBHANDLE hPDB, CHAR *szTransName)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE3;
    UINT16      usExecNum = 15;
    PDBUINT32   *aulPartID;
    TABLE_PRIV_INFO *privInfo;
    UINT16      i;
    UINT32      ulPartID = time(NULL);
    UINT32      ulStart;
    UINT32      ulEnd;
    TABLE_SQL_OPTION_T stSqlOpt;

    memset(&stSqlOpt, 0, sizeof(TABLE_SQL_OPTION_T));
    stSqlOpt.ulTableId = TABLE3;
    stSqlOpt.ucSqlOpt[0] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[1] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[2] = SQL_OPTION_BFW;
    pdb_update_table_sql(&stSqlOpt, SQL_MOD_ADD);

    aulPartID = malloc(usExecNum*sizeof(PDBUINT32));
    privInfo = malloc(usExecNum*sizeof(TABLE_PRIV_INFO));

    *aulPartID = ulPartID;

    for (i=0; i<usExecNum; i++)
    {
        privInfo[i].ulGroupId = i + 1;
        privInfo[i].ulOperId = i*10 + 1;
        privInfo[i].ulPartId = i%2 + 1;
    }

    ulStart = time(NULL);

    bRet = pdb_multi_ins_table(hPDB, ulTableID, szTransName, usExecNum, sizeof(privInfo[0]), privInfo, aulPartID);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    return bRet;
}

#define EXEC_NUM    10//0xffff

/**

*/

PDBBOOL pdb_test_bulk_ins(PDBHANDLE hPDB, PDBUINT32 *pulPartID)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    UINT16      usExecNum = 5;
    PDBUINT32   *aulPartID;
    TABLE_GROUP_INFO *groupInfo;
    UINT16      i;
    UINT32      ulPartID = time(NULL);
    UINT32      ulStart;
    UINT32      ulEnd;
    TABLE_SQL_OPTION_T stSqlOpt;

    memset(&stSqlOpt, 0, sizeof(TABLE_SQL_OPTION_T));
    stSqlOpt.ulTableId = TABLE1;
    stSqlOpt.ucSqlOpt[0] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[1] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[2] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[3] = SQL_OPTION_NONE;
    pdb_update_table_sql(&stSqlOpt, SQL_MOD_ADD);

    aulPartID = malloc(usExecNum*sizeof(PDBUINT32));
    groupInfo = malloc(usExecNum*sizeof(TABLE_GROUP_INFO));

    *pulPartID = ulPartID;

    for (i=0; i<usExecNum; i++)
    {
        groupInfo[i].groupId = i + 1;
        groupInfo[i].groupStat = i%2;
        if(i == 0)
        {
            sprintf(groupInfo[i].groupName, "Administrator");
        }
        else
        {
            sprintf(groupInfo[i].groupName, "NormalGroup%d", i + 1);
        }
    }

    ulStart = time(NULL);

    bRet = pdb_multi_ins_table(hPDB, ulTableID, "", usExecNum, sizeof(groupInfo[0]), groupInfo, aulPartID);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    return bRet;
}

/**

*/

PDBBOOL pdb_test_bulk_mod(PDB_HANDLE_T *pstHandle, PDBUINT32 ulPartID)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    UINT16      usExecNum = EXEC_NUM;
    PDBUINT32   *aulPartID;
    TABLE_GROUP_INFO    *groupInfo;
    UINT16      i;
    UINT32      ulStart;
    UINT32      ulEnd;
    TABLE_SQL_OPTION_T stSqlOpt;

    memset(&stSqlOpt, 0, sizeof(TABLE_SQL_OPTION_T));
    stSqlOpt.ulTableId = TABLE1;
    stSqlOpt.ucSqlOpt[0] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[1] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[2] = SQL_OPTION_WHERE;
    stSqlOpt.ucSqlOpt[3] = SQL_OPTION_BFW;
    pdb_update_table_sql(&stSqlOpt, SQL_MOD_UPD);

    aulPartID = malloc(usExecNum*sizeof(PDBUINT32));
    groupInfo = malloc(usExecNum*sizeof(TABLE_GROUP_INFO));

    for (i=0; i<usExecNum; i++)
    {
        groupInfo[i].groupId = i + 101;
        groupInfo[i].groupStat = 1;
//      groupInfo[i].partId = 15;
        sprintf(groupInfo[i].groupName, "GroupMultiMod_%d", i + 1);
    }

    ulStart = time(NULL);

    bRet = pdb_multi_mod_table(pstHandle, ulTableID, "", usExecNum, sizeof(groupInfo[0]), groupInfo, aulPartID);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    return bRet;
}

/**
PDBBOOL pdb_test_bulk_del(PDB_HANDLE_T *pstHandle, PDBUINT32 ulPartID)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    UINT16      usExecNum = EXEC_NUM;
    PDBUINT32   *aulPartID;
    TABLE1_T    *astTable;
    UINT16      i;
    UINT32      ulStart;
    UINT32      ulEnd;

    aulPartID = malloc(usExecNum*sizeof(PDBUINT32));
    astTable = malloc(usExecNum*sizeof(TABLE1_T));

    for (i=0; i<usExecNum; i++)
    {
        astTable[i].ulPartID = i;
        astTable[i].ulKey = ulPartID + i;
        sprintf(astTable[i].acFieldString, "%0*d", sizeof(astTable[i].acFieldString)-1, astTable[i].ulKey);
        memset(astTable[i].aucFieldRaw, 0xff, sizeof(astTable[0].aucFieldRaw));
    }

    ulStart = time(NULL);

    bRet = pdb_multi_del_table(pstHandle, ulTableID, "", usExecNum, sizeof(astTable[0]), astTable, aulPartID);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    return bRet;
}
*/

PDBBOOL pdb_test_bulk_del(PDBHANDLE hPDB, PDBUINT32 ulPartID)
{
    PDBBOOL     bRet;
    UINT32      ulTableID = TABLE1;
    UINT16      usExecNum = 5;
    PDBUINT32   *aulPartID;
    TABLE_GROUP_INFO    *groupInfo;
    UINT16      i;
    UINT32      ulStart;
    UINT32      ulEnd;
    TABLE_SQL_OPTION_T stSqlOpt;

    PDB_HANDLE_T *pstHandle = (PDB_HANDLE_T *)hPDB;

    memset(&stSqlOpt, 0, sizeof(TABLE_SQL_OPTION_T));
    stSqlOpt.ulTableId = TABLE1;
    stSqlOpt.ucSqlOpt[0] = SQL_OPTION_WHERE;
    stSqlOpt.ucSqlOpt[1] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[2] = SQL_OPTION_BFW;
    stSqlOpt.ucSqlOpt[3] = SQL_OPTION_BFW;
    pdb_update_table_sql(&stSqlOpt, SQL_MOD_DEL);

    aulPartID = malloc(usExecNum*sizeof(PDBUINT32));
    groupInfo = malloc(usExecNum*sizeof(TABLE_GROUP_INFO));

    for (i=0; i<5; i++)
    {
        groupInfo[i].groupId = i + 6;
    }

    ulStart = time(NULL);

    bRet = pdb_multi_del_table(pstHandle, ulTableID, "", usExecNum, sizeof(groupInfo[0]), groupInfo, aulPartID);

    ulEnd = time(NULL);

    ulStart = ulEnd - ulStart;

    free(aulPartID);
    free(groupInfo);

    return bRet;
}
#endif



#define CHECK_TEST(a)   if ((a) != TRUE) goto fail;

int main()
{
    PDBRET          enRet;
    CHAR            *szServer = "127.0.0.1";
    CHAR            *szDBName = "omcv2";
    CHAR            *szUserName = "root";
    CHAR            *szPassword = "root";
    UINT16          usPort = 13306;
    PDBUINT32       ulPartID=0;
    PDBHANDLE       hPDB;
    UINT16          usExecNum = 50000;//*PDB_BATCH_INS_NUM*;

    ulPartID = PDB_TAB_MARK;
    hPDB = NULL;

    pdb_init_all_table();

    enRet = PDBInit();
    enRet = pdb_connect(&hPDB, szServer, usPort, szDBName, szUserName, szPassword);
/*
    if (0 != mysql_server_init(0, NULL, NULL))
    {
        return PDB_ERROR;
    }
*/

#if 0
    CHECK_TEST(pdb_test_del(hPDB));
    CHECK_TEST(pdb_test_ins(hPDB));
    CHECK_TEST(pdb_test_get(hPDB));
    CHECK_TEST(pdb_test_mod(hPDB));
    CHECK_TEST(pdb_test_get(hPDB));
#endif

    //CHECK_TEST(pdb_test_bulk_del(hPDB, usExecNum));
    CHECK_TEST(pdb_test_bulk_ins(hPDB, usExecNum));

    //pdb_test_ins(hPDB);
    //pdb_test_mod(hPDB);
    //pdb_test_del(&stPDBHandle);
    //pdb_test_bulk_del(hPDB, &ulPartID);
    //pdb_test_bulk_ins(hPDB, &ulPartID);
    //pdb_test_bulk_mod(&stPDBHandle, &ulPartID);

    goto end;

fail:
    printf("test failed \n");
end:
    enRet = PDBDisconnect(hPDB);

    enRet = PDBExit();

    return 0;
}
