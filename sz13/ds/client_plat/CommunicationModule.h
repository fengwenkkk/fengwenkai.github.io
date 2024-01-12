/**
 * @file            CommunicationModule.h
 * @brief           通信模块的实现
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-11-04 17:37:18
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef DS_CLIENT_PLAT_COMMUNICATION_MODULE_H__
#define DS_CLIENT_PLAT_COMMUNICATION_MODULE_H__

#include "g_include.h"
#include "RemoteMsg.h"

class CM : public gos::singleton<CM>
{
    friend class gos::singleton<CM>;

public:
    bool Init(const std::string& strIP, UINT16 usPort, CLIENT_CONNECTION_CHANGE* f, const INT32& iClientType)
    {
        BYTE aucIP[4] = {0};
        gos_get_ip((char*)strIP.c_str(), aucIP);

        m_pModule = RemoteMsg::New(aucIP, usPort, true);

        if (!m_pModule)
        {
            GosLog(LOG_FATAL, "Get Remote Msg failed!");

            return false;
        }

        m_pModule->SetClientType(iClientType);
        m_pModule->SetClientConnectionChange(f);

        for (int i = 0; i < 100; i++)
        {
            if (m_pModule->IsConnected())
            {
                break;
            }

            gos_sleep_1ms(10);
        }

        if (!m_pModule->IsConnected())
        {
            GosLog(LOG_FATAL, "Connect to server failed! ");

            return false;
        }
        InnerMsgReady();
        return true;
    }

    template <typename MSG_INFO>
    std::string GetMsgJsonString(const std::string& strMsgName, const MSG_INFO& stInfo)
    {
        MSG_C<MSG_INFO> stMsg;
        stMsg.strMsgName = strMsgName;
        stMsg.ulMsgSeqID = gos_get_current_time();
        stMsg.stMsgInfo = stInfo;

        std::string strMsg;
        if (!stMsg.StructToJson(strMsg))
        {
            return "";
        }
        return strMsg;
    }

    template <typename MSG_INFO>
    bool SendServer(const std::string& strMsgName, const MSG_INFO& stInfo)
    {
        std::string strSend = GetMsgJsonString<MSG_INFO>(strMsgName, stInfo);
        if (strSend.empty())
        {
            return false;
        }
        LOG << "strMsgName: " << strMsgName << ", "
            << "strSend: " << strSend;
        return m_pModule->SendReq(EV_APP_DIS_REQ, (VOID*)strSend.c_str(), strSend.size() + 1);
    }

    template <typename MSG_INFO>
    bool SendServerSync(const std::string& strMsgName, const MSG_INFO& stInfo, std::string& strOut)
    {
        std::string strMsg = GetMsgJsonString(strMsgName, stInfo);
        if (strMsg.empty())
        {
            return false;
        }

        unsigned ulRspMsgLen = 0;
        char acRsp[4096] = {0};

        if (!m_pModule->SyncSend(EV_APP_DIS_REQ, (VOID*)strMsg.c_str(), strMsg.size() + 1, acRsp, ARRAY_SIZE(acRsp),
                                 &ulRspMsgLen, 3))
        {
            return false;
        }
        else
        {
            strOut = acRsp;
            return true;
        }
    }

private:
    CM() {}

    RemoteMsg* m_pModule;
};

#endif  ///< DS_CLIENT_PLAT_COMMUNICATION_MODULE_H__