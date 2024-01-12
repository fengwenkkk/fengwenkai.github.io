#include "g_include.h"

#ifdef WIN32
#include <string.h>
#endif

#include "pdb_def.h"
#include "pdb_util.h"

/*====================================================================
������          : DCICreateSem
����            : �����ź���
�㷨ʵ��        :
����ȫ�ֱ���    : ��
�������˵��    :
PDBBYTE     *pucName  -- �ź�������
PDBUINT32   ulInitVal -- �ź�����ʼֵ
PDBUINT32   ulMaxVal  -- �ź������ֵ
PDBUINT32   ulFlag    -- �ź�����־
����ֵ˵��      : BOOL
====================================================================*/
HANDLE DCICreateSem(BYTE    *pucName,
                    UINT32  ulInitVal,
                    UINT32  ulMaxVal)
{
    HANDLE hSem = NULL;
#ifdef WIN32
    hSem = CreateMutex (NULL, FALSE, (CHAR*)pucName);
    return hSem;

#else
    int ret;

    hSem = (HANDLE)malloc(sizeof(sem_t));
    if (hSem == NULL)
    {
        return NULL;
    }

    ret = sem_init((sem_t*)hSem, 0, 1);
    if (ret != 0)
    {
        return NULL;
    }
    else
    {
        return hSem;
    }
#endif
}

/*====================================================================
������          : DCICloseSem
����            : �ر��ź���
�㷨ʵ��        :
����ȫ�ֱ���    : ��
�������˵��    : PDBHANDLE hSem -- �ź���ID
����ֵ˵��      : BOOL
====================================================================*/
BOOL DCICloseSem(HANDLE hSem)
{
#ifdef WIN32
    return CloseHandle(hSem);
#else
    sem_t   *pstSem = (sem_t*)hSem;

    if (sem_destroy(pstSem) == -1)
    {
        GOS_FREE(pstSem);
        return FALSE;
    }
    else
    {
        GOS_FREE(pstSem);
        return TRUE;
    }
#endif
}

/*====================================================================
������          : DCISemP
����            : �ź���P����
�㷨ʵ��        :
����ȫ�ֱ���    : ��
�������˵��    :
PDBUINT32 ulSmID      -- �ź���ID
PDBUINT32 ulTimeout   -- ��ʱ(ms)
PDBUINT32 ulCount     -- P������Ŀ
����ֵ˵��      : BOOL
====================================================================*/
BOOL DCISemP(HANDLE hSem, UINT32 ulTimeout, UINT32 ulCount)
{
#ifdef WIN32
    UINT32    ulRet;

    if(ulCount != 1)
    {
        return FALSE;
    }

    ulRet = WaitForSingleObject(hSem, ulTimeout);
    if (ulRet == WAIT_OBJECT_0)
    {
        return TRUE;
    }

    if (ulRet == WAIT_TIMEOUT)
    {
        return FALSE;
    }

    return FALSE;
#else
    struct timespec timeout;

    if(ulCount != 1)
    {
        return FALSE;
    }

    if (ulTimeout == LOCK_FOREVER)
    {
        if(0 == sem_wait((sem_t*)hSem))
        {
            return TRUE;
        }
    }
    else
    {
        if (ulTimeout == NO_WAIT)
        {
            timeout.tv_sec = 0;
            timeout.tv_nsec = 0;
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += ulTimeout/1000;
            timeout.tv_nsec += (ulTimeout%1000)*1000000;
            while (timeout.tv_nsec >= 1000000000)
            {
                timeout.tv_sec ++;
                timeout.tv_nsec -= 1000000000;
            }
        }

        if (0 == sem_timedwait((sem_t*)hSem, &timeout))
        {
            return TRUE;
        }
        else
        {
            if (errno == ETIMEDOUT)
            {
                return FALSE;
            }
        }
    }

    return FALSE;
#endif
}

/*====================================================================
������          : DCISemV
����            : �ź���V����
�㷨ʵ��        :
����ȫ�ֱ���    : ��
�������˵��    :
PDBUINT32 ulSmID   -- �ź���ID
PDBUINT32 ulCount  -- V������Ŀ
����ֵ˵��      : BOOL
====================================================================*/
BOOL DCISemV(HANDLE hSem, UINT32 ulCount)
{
#ifdef WIN32
    if (0 == ReleaseMutex (hSem))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
#else
    int i;

    for(i=0; i<ulCount; i++)
    {
        if(sem_post((sem_t*)hSem) != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
#endif
}

/*====================================================================
������          : VmCreateLock
����            : ������
�㷨ʵ��        : ��
����ȫ�ֱ���    : ��
�������˵��    : ��
����ֵ˵��      : �����´������ľ�������Ϊ0����ʾ����ʧ��
====================================================================*/
HANDLE DCICreateLock(VOID)
{
    HANDLE       hSem;

    hSem = DCICreateSem(NULL, (UINT32)1, (UINT32)1);
    if (NULL == hSem)
    {
        return NULL;
    }

    return hSem;
}

/*====================================================================
������          : DCIDeleteLock
����            : ɾ����
�㷨ʵ��        :
����ȫ�ֱ���    :
�������˵��    :
����ֵ˵��      :
====================================================================*/
BOOL DCIDeleteLock(HANDLE hLockId)
{
    if (FALSE == DCICloseSem(hLockId))
    {
        return FALSE;
    }

    return TRUE;
}

/*====================================================================
������          : DCILock
����            : ��������
�㷨ʵ��        :
����ȫ�ֱ���    :
�������˵��    :
����ֵ˵��      :
====================================================================*/
BOOL DCILock(HANDLE hLockId, UINT32 ulTimeout)
{
    if (FALSE == DCISemP(hLockId, (UINT32)ulTimeout, (UINT32)1))
    {
        return FALSE;
    }

    return TRUE;
}

/*====================================================================
������          : DCIUnlock
����            : ��������
�㷨ʵ��        :
����ȫ�ֱ���    :
�������˵��    :
����ֵ˵��      :
====================================================================*/
BOOL DCIUnlock(HANDLE hLockId)
{
    if (FALSE == DCISemV (hLockId, (UINT32)1))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL DCIStrNCmpCase(CHAR *szSrc, CHAR *szDst, UINT32 ulLen)
{
    UINT32 i;

    for (i=0; i<ulLen; i++)
    {
        if (toupper(*szSrc++) != toupper(*szDst++))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL RemoveOneCharSql(CHAR *szSql, CHAR cChar)
{
    UINT32  i;
    UINT32  ulLen = strlen(szSql);

    for (i=ulLen-1; i>=0; i--)
    {
        if (gos_isspace(szSql[i]))
        {
            szSql[i] = '\0';
        }
        else
        {
            break;
        }
    }

    if (szSql[i] == cChar)
    {
        szSql[i] = '\0';
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// (?,?,...?)
BOOL RemoveOneValuesSql(CHAR *szSql, UINT16 usParaNum)
{
    UINT16  i;

    if (!RemoveOneCharSql(szSql, ')'))
    {
        return FALSE;
    }

    for (i=0; i<usParaNum; i++)
    {
        if (i > 0)
        {
            if (!RemoveOneCharSql(szSql, ',') )
            {
                return FALSE;
            }
        }

        if (!RemoveOneCharSql(szSql, '?') )
        {
            return FALSE;
        }
    }

    if (!RemoveOneCharSql(szSql, '('))
    {
        return FALSE;
    }

    return TRUE;
}

// (?,?,?,?,?,?,?)
BOOL RemoveDelSql(CHAR *szSql, UINT16 usValuesPairNum)
{
    UINT16  i;

    while (RemoveOneCharSql(szSql, ';'));

    if (!RemoveOneCharSql(szSql, ')'))
    {
        /* δ�ҵ� */
        return FALSE;
    }

    for (i=0; i<usValuesPairNum;i++)
    {
        if (!RemoveOneCharSql(szSql, '?') ||
            !RemoveOneCharSql(szSql, ','))
        {
            return FALSE;
        }
    }

    strcat(szSql, ")");
    return TRUE;
}

// (?,?,...?),(?,?,...?),(?,?,...?)
BOOL RemoveInsertSql(CHAR *szSql, UINT16 usParaNum, UINT16 usValuesPairNum)
{
    UINT16  i;

    while (RemoveOneCharSql(szSql, ';'));

    for (i=0; i<usValuesPairNum;i++)
    {
        if (!RemoveOneValuesSql(szSql, usParaNum))
        {
            return FALSE;
        }

        if (!RemoveOneCharSql(szSql, ','))
        {
            return FALSE;
        }
    }

    return TRUE;
}
