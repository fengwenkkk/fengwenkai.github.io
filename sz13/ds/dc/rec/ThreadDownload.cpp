#include "ClientInclude.h"
#include "ThreadDownload.h"
#include "FrmLocalRecManage.h"
#include "DlgExportRecFile.h"

extern BOOL DownloadDCRecFile(char *szDCAddr, char *szFile, char *szLocalFile);
extern BOOL HttpDownloadFile(AnsiString strUrl, AnsiString strLocalFile);

ThreadDownload::ThreadDownload(DOWNLOAD_FILE_INFO_T &stInfo):GThread((void*)NULL)
{
    memcpy(&m_stInfo, &stInfo, sizeof(stInfo));
}

GOS_THREAD_RET ThreadDownload::ThreadEntry(VOID* pvPara)
{
    AnsiString  strResult = "";

    if (m_stInfo.ulRecType == REC_TYPE_IPH)
    {
        if (!DownloadDCRecFile(m_stInfo.acDCAddr, m_stInfo.acFile, m_stInfo.acLocalFile))
        {
            gos_delete_file(m_stInfo.acLocalFile);
            strResult = "����ʧ��";
            GosLog(LOG_ERROR, "download dc(%s) file %s to local %s failed", m_stInfo.acDCAddr, m_stInfo.acFile, m_stInfo.acLocalFile);
        }
        else
        {
            strResult = "�����ɹ�";
            GosLog(LOG_DETAIL, "download file %s succ", m_stInfo.acFile);
        }
    }
    else
    {
        if (!HttpDownloadFile(m_stInfo.acFile, m_stInfo.acLocalFile))
        {
            gos_delete_file(m_stInfo.acLocalFile);
            strResult = "����ʧ��";
            GosLog(LOG_ERROR, "download remote file %s to local %s failed", m_stInfo.acFile, m_stInfo.acLocalFile);
        }
        else
        {
            strResult = "�����ɹ�";
            GosLog(LOG_DETAIL, "download file %s succ", m_stInfo.acFile);
        }
    }

    StrCpy(m_stInfo.acResult, strResult);

    if (!DialogExportRecFile)
    {
        return 0;
    }

    UINT32          ulEventMsgLen = sizeof(m_stInfo);
    UINT32          ulTotalMsgLen = STRUCT_OFFSET(INNER_MSG_T, aucMsg)+ulEventMsgLen;
    INNER_MSG_T     *pstMsg = (INNER_MSG_T*)calloc(1, ulTotalMsgLen);
    HWND            Handle = DialogExportRecFile->Handle;

    if (!pstMsg)
    {
        return 0;
    }

    memcpy(pstMsg->aucMsg, &m_stInfo, sizeof(m_stInfo));

    pstMsg->ulMsgType = EV_DOWNLOAD_MSG;
    pstMsg->ulMsgLen = ulEventMsgLen;

    if (!PostMessage(Handle, WM_INNER_MSG, 0, (LPARAM)pstMsg))
    {
        free(pstMsg);
    }

    return 0;
}
