/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/********************************************************************
filename        :   eLTE_SDK.h
author          :   zWX229156
created         :   2015/1/5
description     :   ���� DLL Ӧ�ó���ĵ�������
copyright       :   Copyright (C) 2015-2017
history         :   2015/1/5 ��ʼ�汾
*********************************************************************/

#ifndef __ELTE_SDK_H__
#define __ELTE_SDK_H__

#include "eLTE_Types.h"

// �����ṩDLL�����ӿ�
#ifdef WIN32
#ifdef  ELTE_SDK_EXPORTS
#define ELTE_SDK_API __declspec(dllexport)
#else
#define ELTE_SDK_API __declspec(dllimport)
#endif
#else
#define ELTE_SDK_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**************************************************************************
    * name          : ELTE_SDK_SetLogPath
    * description   : ������־·������ʹ�þ���·������鿴��־�ļ�
    * input         : pLogFilePath ��־����·��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLogPath(const ELTE_CHAR* pLogFilePath);

    /**************************************************************************
    * name          : ELTE_SDK_SetLogLevel
    * description   : ������־����
    * input         : iLogLevel ��־����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLogLevel(ELTE_UINT32 iLogLevel);

    /**************************************************************************
    * name          : ELTE_SDK_ReleaseBuffer
    * description   : �ͷ�SDK�ڲ�������ڴ�
    * input         : pBuffer �ڴ�ָ��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL  ELTE_SDK_ReleaseBuffer(ELTE_CHAR* pBuffer);

    /**************************************************************************
    * name          : ELTE_SDK_GetVersion
    * description   : ��ȡSDK�汾��
    * input         : pVersion ��ȡSDK�汾��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetVersion(ELTE_CHAR** pVersion);

    /**************************************************************************
    * name          : ELTE_SDK_Init
    * description   : ��ʼ��SDK
    * input         : N/A
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Init(ELTE_INT32 iMediaBypass);

    /**************************************************************************
    * name          : ELTE_SDK_Cleanup
    * description   : �ͷ�SDK
    * input         : N/A
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Cleanup();

    /**************************************************************************
    * name          : ELTE_SDK_SetEventCallBack
    * description   : ������Ϣ�ص�����
    * input         : fEventCallBack �û����õ��¼��ص�������Ϊ�ձ�ʾȡ�����õĻص�����
                      pUserData      �û�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetEventCallBack(EventCallBack fEventCallBack, ELTE_VOID* pUserData);

    /**************************************************************************
    * name          : ELTE_SDK_SetYUVFrameCallBack
    * description   : ������Ƶ���ݻص�����
    * input         : fYUVFrameCallBack �û����õ���Ƶ���ݻص�������Ϊ�ձ�ʾȡ�����õĻص�����
                      pUserData      �û�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetYUVFrameCallBack(YUVFrameCallBack fYUVFrameCallBack, ELTE_VOID* pUserData);

    /**************************************************************************
    * name          : ELTE_SDK_SetYUVFrameCallBack64
    * description   : ������Ƶ���ݻص�����
    * input         : fYUVFrameCallBack �û����õ���Ƶ���ݻص�������Ϊ�ձ�ʾȡ�����õĻص�����
                      pUserData      �û�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetYUVFrameCallBack64(YUVFrameCallBack64 fYUVFrameCallBack, ELTE_VOID* pUserData);

    /**************************************************************************
    * name          : ELTE_SDK_Login
    * description   : ��¼
    * input         : pUserID         ����Ա�˺�
                      pPWD            ����Ա����
                      pServerIP       ���Ȼ�IP��ַ
                      pLocalIP        ��������IP��ַ
                      uiServerSIPPort ���Ȼ�SIP����˿�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Login(const ELTE_CHAR* pUserID, const ELTE_CHAR* pPWD, const ELTE_CHAR* pServerIP, const ELTE_CHAR* pLocalIP, ELTE_UINT32 uiServerSIPPort);

    /**************************************************************************
    * name          : ELTE_SDK_Logout
    * description   : ע��
    * input         : pUserID ����Ա�˺�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Logout(const ELTE_CHAR* pUserID);

    /**************************************************************************
    * name          : ELTE_SDK_TriggerStatusReport
    * description   : ����״̬�ϱ�
    * input         : iEnableStatusReport �Ƿ���״̬�ϱ�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TriggerStatusReport(ELTE_INT32 iEnableStatusReport);

    /**************************************************************************
    * name          : ELTE_SDK_ProvisionManagerInitMRS
    * description   : ý���������ʼ��
    * input         : pServerIP ý�������IP��ַ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ProvisionManagerInitMRS(const ELTE_CHAR* pServerIP);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcGroups
    * description   : ��ȡ����̨Ⱥ���б�
    * input         : pUserID ����̨���û���
    * output        : pDcGroups �õ���̨����������Ⱥ���б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcGroups(const ELTE_CHAR* pUserID, ELTE_CHAR** pDcGroups);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcGroups
    * description   : �����ͻ�ȡ����̨�������Ⱥ���б�
    * input         : pUserID ����̨���û���
                      pType ����
    * output        : pDcGroups �õ���̨�����ĸ����͵�����Ⱥ���б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcGroupsByType(const ELTE_CHAR* pUserID, const ELTE_CHAR* pType, ELTE_CHAR** pDcGroups);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcUsers
    * description   : ��ȡ����̨�û��б�
    * input         : pUserID ����̨���û���
    * output        : pDcUsers �õ���̨������������û��б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcUsers(const ELTE_CHAR* pUserID, ELTE_CHAR** pDcUsers);
    /**************************************************************************
    * name          : ELTE_SDK_GetDcUsers
    * description   : �����ͻ�ȡ����̨��������û��б�
    * input         : pUserID ����̨���û���
                      pType   ����
    * output        : pDcUsers �õ���̨������ĸ����͵������û��б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcUsersCloneByType(const ELTE_CHAR* pUserID,const ELTE_CHAR* pType, ELTE_CHAR** pDcUsers);

    /**************************************************************************
    * name          : ELTE_SDK_GetGroupUsers
    * description   : ��ȡһ��Ⱥ���Ա�б�
    * input         : pGroupID Ⱥ���
    * output        : pGroupUsers ��Ⱥ������г�Ա�б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGroupUsers(const ELTE_CHAR* pGroupID, ELTE_CHAR** pGroupUsers);

    /**************************************************************************
    * name          : ELTE_SDK_GetGroupInfo
    * description   : ��ȡһ��Ⱥ����ϸ��Ϣ
    * input         : pGroupID Ⱥ���
    * output        : pGroupInfo ��Ⱥ�����ϸ��Ϣ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGroupInfo(const ELTE_CHAR* pGroupID, ELTE_CHAR** pGroupInfo);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserInfo
    * description   : ��ȡһ���û���ϸ��Ϣ
    * input         : pUserID ����Ա�˺�
    * output        : pUserInfo ���û�����ϸ��Ϣ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserInfo(const ELTE_CHAR* pUserID, ELTE_CHAR** pUserInfo);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcInfo
    * description   : ��ȡһ���û���ϸ��Ϣ
    * input         : pUserID ����̨���û���
    * output        : pDcInfo �õ���̨��ϸ��Ϣ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcInfo(const ELTE_CHAR* pUserID, ELTE_CHAR** pDcInfo);

    /**************************************************************************
    * name          : ELTE_SDK_UnSubscribeGroup
    * description   : ȡ������Ⱥ��
    * input         : pGroupID              Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_UnSubscribeGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_SubscribeGroup
    * description   : ����Ⱥ��
    * input         : pGroupID              Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SubscribeGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_JoinGroup
    * description   : ����̨�ٺ����ĳȺ��Ⱥ��Ự����Ҫ����̨���Ǹ�Ⱥ���Ա�����������ǽ�����̨��ΪȺ���Ա��
    * input         : pGroupID              Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_JoinGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_SubJoinGroup
    * description   : ���Ĳ��Զ�����Ⱥ��
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SubJoinGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_CreateDynamicGroup
    * description   : ������̬Ⱥ��
    * input         : pDGNAParam ��̬�������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CreateDynamicGroup(const ELTE_CHAR* pDGNAParam);

    /**************************************************************************
    * name          : ELTE_SDK_MultiGrpCreate
    * description   : ����̨������ѡ��
    * input         : pMultiGrpParam ��ѡ���������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_MultiGrpCreate(const ELTE_CHAR* pMultiGrpParam);

    /**************************************************************************
    * name          : ELTE_SDK_AddMultiGrpMember
    * description   : ����̨���Ӷ�ѡ���Ա
    * input         : pMultiGrpParam ��ѡ���������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AddMultiGrpMember(const ELTE_CHAR* pMultiGrpParam);

    /**************************************************************************
    * name          : ELTE_SDK_AddMultiGrpMember
    * description   : ����̨ɾ����ѡ���Ա
    * input         : pMultiGrpParam ��ѡ���������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_DeleteMultiGrpMember(const ELTE_CHAR* pMultiGrpParam);

    /**************************************************************************
    * name          : ELTE_SDK_CancelDynamicGroup
    * description   : ɾ����̬Ⱥ��
    * input         : pResourceID ��̬�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CancelDynamicGroup(const ELTE_CHAR* pResourceID);
    /**************************************************************************
    * name          : ELTE_SDK_CancelMultiGrp
    * description   : ɾ����ѡ��
    * input         : pResourceID ��ѡ�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CancelMultiGrp(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_GetTempGroupID
    * description   : ��ȡ��ʱ���
    * input         : N/A
    * output        : iGroupID ��ʱ�����
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetTempGroupID(ELTE_INT32* iGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_GetTempGroupID64(long long��)
    * description   : ��ȡ��ʱ���
    * input         : N/A
    * output        : iGroupID ��ʱ�����
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetTempGroupID64(ELTE_INT64* iGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_CreateTempGroup
    * description   : ������ʱ��
    * input         : pTEMPGParam ��ʱ�������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CreateTempGroup(const ELTE_CHAR* pTEMPGParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserRECFileInfoList
    * description   : ��ȡ¼���ļ��б�
    * input         : pQueryXml ��ѯ¼������Xml
    * output        : pRspXml   ��ѯ¼���ļ�����б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserRECFileInfoList(const ELTE_CHAR* pQueryXml, ELTE_CHAR** pRspXml);

    /**************************************************************************
    * name          : ELTE_SDK_StartRealPlay
    * description   : ��ʼ����ʵʱ��Ƶ
    * input         : pResourceID ��ԴID
                      pVideoParam ý�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartRealPlay(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoParam);

    /**************************************************************************
    * name          : ELTE_SDK_StartCircleDisPlay
    * description   : ʵ�ֶ�һ������ͷ������Ƶ��ѯ
    * input         : pResourceID ��ԴID
                      pCircleDisPlayXml ý�����
                      pTimerPeriod ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartCircleDisPlay(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pCircleDisPlayXml,const ELTE_CHAR* pTimerPeriod);

    /**************************************************************************
    * name          : ELTE_SDK_StartVideoDial
    * description   : ��ʼ��Ƶ���
    * input         : pResourceID ��ԴID
                      pVideoParam ý�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartVideoDial(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoParam);

    /**************************************************************************
    * name          : ELTE_SDK_SetPlayWindow
    * description   : ���ò���ʵʱ��Ƶ����
    * input         : pResourceID ��ԴID
                      pVideoParam ���ھ��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetPlayWindow(const ELTE_CHAR* pResourceID, ELTE_VOID* hWnd);
    /**************************************************************************
    * name          : ELTE_SDK_SetLocalPlayWindow
    * description   : ������Ƶ������ش���
    * input         : pResourceID ��ԴID
                      pVideoParam ���ھ��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLocalPlayWindow(const ELTE_CHAR* pResourceID, ELTE_VOID* hWnd);

    /**************************************************************************
    * name          : ELTE_SDK_LocalSnapshot
    * description   : ץ����Ƶ����
    * input         : pResourceID ��ԴID
                      pSnapshotPath ͼƬ����·��+�ļ���
                      ulSnapshotFormat ץͼ��ʽ��1 JPG��2 BMP
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_LocalSnapshot(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSnapshotPath, ELTE_ULONG ulSnapshotFormat);

    /**************************************************************************
    * name          : ELTE_SDK_SetPlayWindowSize
    * description   : ���ò���ʵʱ��Ƶ���ڵĴ�С
    * input         : pResourceID ��ԴID
                      ulWidth           ���
                      ulHeight          �߶�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetPlayWindowSize(const ELTE_CHAR* pResourceID, ELTE_ULONG ulWidth, ELTE_ULONG ulHeight);
    /**************************************************************************
    * name          : ELTE_SDK_SetPlayWindowSize
    * description   : ������Ƶ������ش��ڵĴ�С
    * input         : pResourceID ��ԴID
                      ulWidth           ���
                      ulHeight          �߶�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLocalPlayWindowSize(const ELTE_CHAR* pResourceID, ELTE_ULONG ulWidth, ELTE_ULONG ulHeight);

    /**************************************************************************
    * name          : ELTE_SDK_PTZControl
    * description   : ��ʼ��̨����
    * input         : pResourceID      ����̨���������ԴID
                      iPTZControlCode  ��̨��������
                      iPTZControlValue ��̨���������Ӧ�Ĳ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTZControl(const ELTE_CHAR* pResourceID, ELTE_UINT32 iPTZControlCode, ELTE_UINT32 iPTZControlValue);

    /**************************************************************************
    * name          : ELTE_SDK_StopRealPlay
    * description   : ֹͣ����ʵʱ��Ƶ
    * input         : pResourceID      ����̨���������ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopRealPlay(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StopCircleDisPlay
    * description   : ����̨��ֹ���Ҷ����ڽ�����ѯ������ͷ
    * input         : pResourceID     ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopCircleDisPlay(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PVideoReject
    * description   : �ܾ���Ƶ�ش�
    * input         : pResourceID      ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PVideoReject(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PDial
    * description   : �����������
    * input         : pResourceID  ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PDial(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PHalfDpxDial
    * description   : �ͷŰ�˫�����
    * input         : pResourceID  ����ĶԶ���ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PHalfDpxRelease(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PHalfDpxDial
    * description   : �����˫�����
    * input         : pResourceID  ����ĶԶ���ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PHalfDpxDial(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PRecv
    * description   : �����������
    * input         : pResourceID  ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PRecv(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PReject
    * description   : �ܽ��������
    * input         : pResourceID  ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PReject(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PHangup
    * description   : �Ҷ��������
    * input         : pResourceID  ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PHangup(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PBreakoff
    * description   : ����̨ǿ����
    * input         : pResourceID  ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PBreakoff(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PBreakin
    * description   : ������ǿ����+�����µ����
    * input         : pResourceID  ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PBreakin(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTDial
    * description   : ����������̶�����ʱ���������Ȩ
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTDial(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTBreakin
    * description   : ����������̶�����ʱ���������Ȩ
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTBreakin(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTDialMultiGrp
    * description   : ����̨�����ѡ���������Ȩ
    * input         : pGroupID  ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTDialMultiGrp(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTRelease
    * description   : �ͷŻ�Ȩ
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTRelease(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTReleaseMultiGrp
    * description   : ����̨�ͷŶ�ѡ�黰Ȩ
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTReleaseMultiGrp(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTHangup
    * description   : �˳����
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTHangup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTEmergency
    * description   : ����������
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTEmergency(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_GroupBreakoff
    * description   : ����̨ǿ�����
    * input         : pGroupID  Ⱥ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GroupBreakoff(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_VolMute
    * description   : ����ִ̨�о���
    * input         : pResourceID ��ԴID
                      pMuteParam ��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VolMute(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pMuteParam);

    /**************************************************************************
    * name          : ELTE_SDK_MuteControl
    * description   : ����̨���Ⱥ����û����������˲���ֻ��Ҫ��עALL_MUTE��
    * input         : pResourceID ��ԴID
                      pMuteParam ��������
                                SINGLE_UNMUTE   0, //����ȥ����
                                SINGLE_MUTE     1, //��������
                                ALL_UNMUTE      2, //����ͳһȥ����
                                ALL_MUTE        3  //����ͳһ����
                                ALL_MUTE_PTT    4,
                                ALL_UNMUTE_PTT  5
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_MuteControl(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pMuteParam);

    /**************************************************************************
    * name          : ELTE_SDK_VolUnMute
    * description   : ����̨ȡ������
    * input         : pResourceID ��ԴID
                        pUnMuteParam ȡ����������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VolUnMute(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pUnMuteParam);

    /**************************************************************************
    * name          : ELTE_SDK_VolumeControl
    * description   : ���Ƶ�ǰͨ������
    * input         : pResourceID ��ԴID
                      pVolumeControlParam ��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VolumeControl(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVolumeControlParam);

    /**************************************************************************
    * name          : ELTE_SDK_GISSubscribe
    * description   : ����/ȥ����GIS�ն�
    * input         : pResourceID ��ԴID
                      pGISParam GIS����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GISSubscribe(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pGISParam);

    /**************************************************************************
    * name          : ELTE_SDK_ResUnsubBatch
    * description   : ����ȥ�����û�
    * input         : pResourceID ��ԴID
                      pSublst ȥ�����б�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ResUnsubBatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSublst);

    /**************************************************************************
    * name          : ELTE_SDK_ResSubBatch
    * description   : ���������û�
    * input         : pResourceID ��ԴID
                      pSublst �����б�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ResSubBatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSublst);


    /**************************************************************************
    * name          : ELTE_SDK_SDSSendMessage
    * description   : ���Ͷ�/����
    * input         : pResourceID ��ԴID
                      pSDSParam ��/���Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SDSSendMessage(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSDSParam);

    /**************************************************************************
    * name          : ELTE_SDK_RealTimeMsgSend
    * description   : ����̨���ͼ�ʱ��Ϣ
    * input         : pResourceID ��ԴID
                      pSDSParam ��/���Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RealTimeMsgSend(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSDSParam);

    /**************************************************************************
    * name          : ELTE_SDK_RealTimeMsgSend
    * description   : ����̨���ͼ�ʱ��Ϣ��ִ
    * input         : pResourceID ��ԴID
                      pSDSParam ��/���Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ReplyMsgSend(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSDSParam);

    /**************************************************************************
    * name          : ELTE_SDK_SDSExportMessage
    * description   : �����ѷ��ͺͽ��յĶ���/���š�
    * input         : pResourceID ��ԴID����¼�˺�
                      pPath ·��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SDSExportMessage(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pPath);

    /**************************************************************************
    * name          : ELTE_SDK_StartVideoDispatch
    * description   : ��ʼ��Ƶ�ַ�
    * input         : pResourceID ��ԴID
                      pVideoDispatchParam ��Ƶ�ַ�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartVideoDispatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoDispatchParam);

    /**************************************************************************
    * name          : ELTE_SDK_StopVideoDispatch
    * description   : ֹͣ��Ƶ�ַ�
    * input         : pResourceID ��ԴID
                      pVideoDispatchParam ��Ƶ�ַ�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopVideoDispatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoDispatchParam);

    /**************************************************************************
    * name          : ELTE_SDK_RecvVideoPlay
    * description   : ����̨������Ƶ�ش�
    * input         : pResourceID ��ԴID
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RecvVideoPlay(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StartVWall
    * description   : ������Ƶ��ǽ/������ǽ
    * input         : pResVWallID ��ԴID
                      pVWallStartParam ��Ƶ��ǽ����/������ǽ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VWallStart(const ELTE_CHAR* pResVWallID, const ELTE_CHAR* pVWallStartParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcVWallIDList
    * description   : ��ȡ��Ƶ��ǽID�б�
    * input         : DstObjId ��Ƶ��ǽ��Ŀ�ĵأ�Ϊһ����Ƶͨ��
                      pVWallIDList ��Ƶ��ǽĿ��ID�б�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcVWallIDList(ELTE_CHAR** pVWallIDList);

    /**************************************************************************
    * name          : ELTE_SDK_VWallStop
    * description   : ��ֹ��Ƶ��ǽ/��ֹ������ǽ
    * input         : pResVWallID ��Ƶ��ǽ��Դ��Ϊһ������ͷ
                      pVWallStopParam ��Ƶ��ǽ����/������ǽ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VWallStop(const ELTE_CHAR* pResVWallID, const ELTE_CHAR* pVWallStopParam);

    /**************************************************************************
    * name          : ELTE_SDK_TelephoneDial
    * description   : ����PSTN/PLMN�绰����
    * input         : pTelNumber PSTN/PLMN�绰����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TelephoneDial(const ELTE_CHAR* pTelNumber);

    /**************************************************************************
    * name          : ELTE_SDK_TelephoneHangup
    * description   : ֹͣPSTN/PLMN�绰����
    * input         : pTelNumber PSTN/PLMN�绰����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TelephoneHangup(const ELTE_CHAR* pTelNumber);

    /**************************************************************************
    * name          : ELTE_SDK_StartDiscreetListen
    * description   : �������ܼ���
    * input         : pResourceID ��Դ���󣬿�����Ⱥ����û�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartDiscreetListen(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StopDiscreetListen
    * description   : ֹͣ���ܼ���
    * input         : pResourceID ��Դ���󣬿�����Ⱥ����û�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopDiscreetListen(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StartEnvironmentListen
    * description   : ���𻷾�����
    * input         : pResourceID ��Դ���󣬿�����Ⱥ����û�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartEnvironmentListen(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PTransfer
    * description   : �����˹�ת��
    * input         : pResourceID ��Դ����һ���Ƿ����˹�ת�ӵĵ���Ա
                      pP2PTransferParam �˹�ת�Ӳ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PTransfer(const ELTE_CHAR* pResourceID,const ELTE_CHAR* pP2PTransferParam);

    /**************************************************************************
    * name          : ELTE_SDK_CreatePatchGroup
    * description   : �����ɽ���
    * input         : pP2PTransferParam �ɽ������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CreatePatchGroup(const ELTE_CHAR* pPatchGroupParam);

    /**************************************************************************
    * name          : ELTE_SDK_CancelPatchGroup
    * description   : ȡ���Ѵ����ɽ���
    * input         : pPatchGroupID ��Դ����һ���Ǵ����ɽ������š�
                                    ͨ��notifyResourceStatus���������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CancelPatchGroup(const ELTE_CHAR* pPatchGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_AddPatchGroupMember
    * description   : �����ɽ����Ա
    * input         : pPatchGroupParam �ɽ������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AddPatchGroupMember(const ELTE_CHAR* pPatchGroupParam);

    /**************************************************************************
    * name          : ELTE_SDK_DeletePatchGroupMember
    * description   : ɾ���ɽ����Ա
    * input         : pPatchGroupParam �ɽ������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_DeletePatchGroupMember(const ELTE_CHAR* pPatchGroupParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetPatchGroups
    * description   : ��ȡָ������̨�������ɽ����б�
    * input         : pDcUser ����̨�û�
                      pPatchGroups �õ���̨������ɽ�����Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPatchGroups(const ELTE_CHAR* pDcUser, ELTE_CHAR** pPatchGroups);

    /**************************************************************************
    * name          : ELTE_SDK_GetSpeaker4OrMic4GroupOrIndividual
    * description   : ��ȡ�û�����Ⱥ�������������˷�
    * input         : pResID �û�����Ⱥ���,���Բ�ָ������NULL
                      pResType ��Դ������ 0���û� �� 1��Ⱥ�� �ش�����
                      pDeviceType �豸���� 0�������� �� 1����˷� �ش�����
    * output        : pSpeakerName ������������˷�����
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetSpeaker4OrMic4GroupOrIndividual(const ELTE_CHAR* pResID,      const ELTE_CHAR* pResType,
                                                                                   const ELTE_CHAR* pDeviceType, ELTE_CHAR** pSpeakerOrMicName);

    /**************************************************************************
    * name          : ELTE_SDK_GetMultiSelectedGrpInfoVec
    * description   : ��ȡ��ǰ����̨���������ж�ѡ����Ϣ
    * input         :   pDcUser ����̨�û�
                        pMultiSelectedGrpInfoVec ��ǰ����̨���������ж�ѡ����Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetMultiSelectedGrpInfoVec(const ELTE_CHAR* pDcUser, ELTE_CHAR** pMultiSelectedGrpInfoVec);

    /**************************************************************************
    * name          : ELTE_SDK_GetDynamicGroupMembers
    * description   : ��ȡ��̬���Ա�б�
    * input         :   pGroupId ��̬�����
                        pDynamicGroupMembers
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDynamicGroupMembers(const ELTE_CHAR* pGroupId, ELTE_CHAR** pDynamicGroupMembers);

    /**************************************************************************
    * name          : ELTE_SDK_GetUEAbilityClone
    * description   : ��ȡ�ն�������Ϣ
    * input         :   pUeId �ն˺���
                        pProvUEAblity ��ȡ�ն�������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUEAbilityClone(const ELTE_CHAR* pUeId, ELTE_CHAR** pProvUEAblity);

    /**************************************************************************
    * name          : ELTE_SDK_GetGroupMemberByPatchId
    * description   : ��ȡָ���ɽ������ĳ�Ա�б�
    * input         : pPatchGroupId �ɽ����
                      pPatchGroupMembers ���ɽ����Ա��Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGroupMemberByPatchId(const ELTE_CHAR* pPatchGroupId, ELTE_CHAR** pPatchGroupMembers);

    /**************************************************************************
    * name          : ELTE_SDK_GetPatchGroupInfo
    * description   : ��ȡָ���ɽ���������Ϣ
    * input         : pPatchGroupId �ɽ����
                      pPatchGroupInfo ���ɽӵ�������Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPatchGroupInfo(const ELTE_CHAR* pPatchGroupId, ELTE_CHAR** pPatchGroupInfo);

    /**************************************************************************
    * name          : ELTE_SDK_GetPatchInfoCloneByMemberId
    * description   : ͨ������̨��ͨ��Ż�ȡ����̨�ɽ�����Ϣ
    * input         : pMemberID ����̨���ɽӵ���ͨ�����
    * output        : pPatchGroupInfo ���ɽӵ�������Ϣ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPatchInfoCloneByMemberId(const ELTE_CHAR* pMemberID, ELTE_CHAR** pPatchGroupInfo);


    /**************************************************************************
    * name          : ELTE_SDK_GetGisSubscription
    * description   : ��ȡ����̨����GIS�ϱ����ն��б�
    * input         : pResourceID ����̨ID
    * output        : pGisQuerySubList ����GIS�ϱ����ն��б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGisSubscription(const ELTE_CHAR* pResourceID, ELTE_CHAR** pGisQuerySubList);

    /**************************************************************************
    * name          : ELTE_SDK_ModifyDynamicGroup
    * description   : �޸Ķ�̬Ⱥ��
    * input         : pResourceID ������̬��ĵ���̨ ID
                      pDynamicGroupInfo ��̬���޸���Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ModifyDynamicGroup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pDynamicGroupInfo);

    /**************************************************************************
    * name          : ELTE_SDK_TempUserJoinGroup
    * description   : ��������ʱ�û�
    * input         : pResourceID Ⱥ��ID
                      pPhonePatchParam ����ӵ��û���Ϣ
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TempUserJoinGroup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pPhonePatchParam);

    /**************************************************************************
    * name          : ELTE_SDK_StartRecord
    * description   : ��ʼ����Ƶ¼��
    * input         : pResourceID ��֮ͨ��������ն˵���Դ����
                      pTypeParam ¼��¼������ѡ��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartRecord(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pTypeParam);

    /**************************************************************************
    * name          : ELTE_SDK_StopRecord
    * description   : ��ֹ����Ƶ¼��
    * input         : pResourceID ��֮ͨ��������ն˵���Դ����
                      pTypeParam ¼��¼������ѡ��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopRecord(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pTypeParam);

    /**************************************************************************
    * name          : ELTE_SDK_EnumSpeakers
    * description   : ö�ٵ���̨��������
    * input         : N/A
    * output        : pSpeakers ��ȡ���������Ƶ��ַ�������
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_EnumSpeakers(ELTE_CHAR** pSpeakers);

    /**************************************************************************
    * name          : ELTE_SDK_EnumMicrophones
    * description   : ö�ٵ���̨����˷�
    * input         : N/A
    * output        : pMicrophones ��ȡ��˷����Ƶ��ַ�������
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_EnumMicrophones(ELTE_CHAR** pMicrophones);


        /**************************************************************************
    * name          : ELTE_SDK_AssignSpeaker4Group
    * description   : ����Ⱥ��������
    * input         : pGroupID Ⱥ��ID
                    ��pSpeaker Ҫ���õ�����������, ����ΪNULLʱ��ѡ��Ĭ��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignSpeaker4Group(const ELTE_CHAR* pGroupID, const ELTE_CHAR* pSpeaker);

    /**************************************************************************
    * name          : ELTE_SDK_AssignMic4Group
    * description   : ����Ⱥ����˷�
    * input         : pGroupID Ⱥ��ID
                    ��pMicrophone Ҫ���õ���˷�����, ����ΪNULLʱ��ѡ��Ĭ����˷�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignMic4Group(const ELTE_CHAR* pGroupID, const ELTE_CHAR* pMicrophone);

    /**************************************************************************
    * name          : ELTE_SDK_AssignSpeaker4Individual
    * description   : �����û�������
    * input         : pResourceID �û�ID
                    ��pSpeaker Ҫ���õ�����������, ����ΪNULLʱ��ѡ��Ĭ��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignSpeaker4Individual(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSpeaker);

    /**************************************************************************
    * name          : ELTE_SDK_AssignMic4Individual
    * description   : �����û���˷�
    * input         : pResourceID Ⱥ��ID
                    ��pMicrophone Ҫ���õ���˷�����, ����ΪNULLʱ��ѡ��Ĭ����˷�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignMic4Individual(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pMicrophone);

    /**************************************************************************
    * name          : ELTE_SDK_GetLocalIp
    * description   : ��ȡ����Ip��ַ
    * input         : N/A
    * output        : pLocalIp ��ȡ����Ip��ַ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetLocalIps(ELTE_CHAR** pLocalIps);

    /**************************************************************************
    * name          : ELTE_SDK_GetDepartmentInfoList
    * description   : ��ȡ������Ϣ�б�
    * input         : N/A
    * output        : pDepartmentInfoList ������Ϣ�б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDepartmentInfoList(ELTE_CHAR** pDepartmentInfoList);

    /**************************************************************************
    * name          : ELTE_SDK_IsInDcManagedDepartmentRange
    * description   : ��ѯĳ�������Ƿ�鱾����̨����
    * input         : pDepartmentID ��Ҫ��ѯ�Ĳ���ID
    * output        : pResult ��ѯ�Ľ�� "true"�����ǣ�"false"�����
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_IsInDcManagedDepartmentRange(const ELTE_CHAR* pDepartmentID, ELTE_CHAR** pQueryResult);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcCameraAttribute
    * description   : ��ȡ��������ͷ����
    * input         : pUserID ����Ա�˺�
    * output        : pCameraAttributeList ����ͷ�����б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcCameraAttribute(const ELTE_CHAR* pUserID, ELTE_CHAR** pCameraAttributeList);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserLevelInfo
    * description   : ��ȡ��������ͷ�㼶��Ϣ
    * output        : pUserLevelInformationList ����ͷ�㼶��Ϣ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserLevelInfo(ELTE_CHAR** pUserLevelInformationList);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserCapLvlRelInfo
    * description   : ��ȡ����ͷ�㼶����Ȩ��
    * output        : pUserCapLevelRelationInfoList ����ͷ�㼶����Ȩ����Ϣ
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserCapLvlRelInfo(ELTE_CHAR** pUserCapLevelRelationInfoList);

    /**************************************************************************
    * name          : ELTE_SDK_GetGisIpcList
    * description   : ��ȡ��������ͷGPS��Ϣ
    * input         : pUserID ����Ա�˺�
    * output        : pProvGisIpcList ����ͷGPS��Ϣ�б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGisIpcList(const ELTE_CHAR* pUserID, ELTE_CHAR** pProvGisIpcList);

    /**************************************************************************
    * name          : ELTE_SDK_GetLocalCameraStatus
    * description   : ��ȡ��������ͷGPS��Ϣ
    * input         : pUserID ����Ա�˺�
    * output        : pLocalCameraStatus ״̬
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetLocalCameraStatus(const ELTE_CHAR* pUserID,ELTE_CHAR** pLocalCameraStatus);


    //==============added for ICP start===================================
    /**************************************************************************
    * name          : ELTE_SDK_Unified_Login
    * description   : ͳһ��¼
    * input         : pUserID         ����Ա�˺�
                      pPWD            ����Ա����
                      pServerIP       udc IP��ַ
                      pLocalIP        ��������IP��ַ
                      uiServerPort udc ����˿�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Unified_Login(const ELTE_CHAR* pUserID, const ELTE_CHAR* pPWD, const ELTE_CHAR* pServerIP, const ELTE_CHAR* pLocalIP, ELTE_UINT32 uiServerPort);

    /**************************************************************************
    * name          : ELTE_SDK_Unified_Logout
    * description   : ע��
    * input         : pUserID ����Ա�˺�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Unified_Logout(const ELTE_CHAR* pUserID);

    /**************************************************************************
    * name          : ELTE_SDK_Unified_ModPwd
    * description   : �޸�����
    * input         : pUserID ����Ա�˺�
                      pOldPWD ������
                      pNewPWD ������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Unified_ModPwd(const ELTE_CHAR* pUserID, const ELTE_CHAR* pOldPWD, const ELTE_CHAR* pNewPWD);

    /**************************************************************************
    * name          : ELTE_SDK_HoldCall
    * description   : ���б���/���鱣�֣�type=0Ϊ���б��֣�type=1Ϊ���鱣��
    * input         : pResourceID ��ԴID
                      pHoldCallParam���б��ֲ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_HoldCall(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pHoldCallParam);

    /**************************************************************************
    * name          : ELTE_SDK_UnHoldCall
    * description   : ����ȥ����/����ȥ���֣�type=0Ϊ����ȥ���֣�type=1Ϊ����ȥ����
    * input         : pResourceID ��ԴID
                      pHoldCallParam���б��ֲ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_UnHoldCall(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pUnHoldCallParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetDeviceIDRelationship
    * description   : ��ȡ�ƶ��豸GBID��ISDN֮��Ĺ�ϵ
    * input         : pQueryParam ��ѯ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDeviceIDRelationship(const ELTE_CHAR* pQueryParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_CreateConf
    * description   : ��������
    * input         : pResourceID ��ԴID
                      pConfCreateConf �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_CreateConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfCreateConf);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_EndConf
    * description   : ��������
    * input         : pResourceID ��ԴID
                      pConfEndConfParam �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_EndConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfEndConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_JoinConf
    * description   : �������
    * input         : pResourceID ��ԴID
                      pConfJoinConfParam  �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_JoinConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfJoinConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AudioAcceptConf
    * description   : ����������������
    * input         : pResourceID ��ԴID
                      pConfAcceptConfParam �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AudioAcceptConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfAcceptConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AudioRejectConf
    * description   : �ܾ�������������
    * input         : pResourceID ��ԴID
                      pConfRejectConf �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AudioRejectConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfRejectConf);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_VideoAcceptConf
    * description   : ������Ƶ��������
    * input         : pResourceID ��ԴID
                      pConfAcceptConfParam �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_VideoAcceptConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfAcceptConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_VideoRejectConf
    * description   : �ܾ���Ƶ��������
    * input         : pResourceID ��ԴID
                      pConfRejectConf �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_VideoRejectConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfRejectConf);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AddMember
    * description   : ��ӻ����Ա
    * input         : pResourceID ��ԴID
                      pConfAddMembersParam ��ӳ�Ա����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AddMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfAddMembersParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_SubConf
    * description   : ���Ļ���
    * input         : pResourceID ��ԴID
                      pConfSubscribeParam ���Ļ������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    //ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_SubConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfSubscribeParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_UnSubConf
    * description   : ȥ���Ļ���
    * input         : pResourceID ��ԴID
                      pConfUnsubscribeParam ȥ���Ļ������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    //ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_UnSubConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfUnsubscribeParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AudioHangup
    * description   : �Ҷ���Ƶ����
    * input         : pResourceID ��ԴID
                      pConfHangupMemberParam �Ҷϲ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AudioHangup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfHangupParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_VideoHangup
    * description   : �Ҷ���Ƶ����
    * input         : pResourceID ��ԴID
                      pConfHangupMemberParam �Ҷϲ���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_VideoHangup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfHangupParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_ReCallMember
    * description   : �غ���Ա
    * input         : pResourceID ��ԴID
                      pConfReCallMemberParam �غ���Ա����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_ReCallMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfReCallMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_ExitConf
    * description   : �뿪����
    * input         : pResourceID ��ԴID
                      pConfExitConfParam �������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_ExitConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfExitConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_MuteMember
    * description   : ���龲����Ա
    * input         : pResourceID ��ԴID
                      pConfMuteMemberParam ��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_MuteMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfMuteMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_Mute
    * description   : ���龲��
    * input         : pResourceID ��ԴID
                      pConfMuteConfParam ��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_Mute(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfMuteConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_BroadCastPicture
    * description   : �㲥�����
    * input         : pResourceID ��ԴID
                      pConfBroadCastPictureParam �㲥����߲���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_BroadCastPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfBroadCastPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_CancelBroadCastPicture
    * description   : ȡ���㲥�����
    * input         : pResourceID ��ԴID
                      pConfCancelBroadCastPictureParam ȡ���㲥����߲���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_CancelBroadCastPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfCancelBroadCastPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_WatchMember
    * description   : ѡ����Ա
    * input         : pResourceID ��ԴID
                      pConfWatchMemberParam ѡ����Ա����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_WatchMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfWatchMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_WatchMixPicture
    * description   : ѡ����Ƶ�໭��
    * input         : pResourceID ��ԴID
                      pConfWatchMixPictureParam ѡ����Ƶ�໭�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_WatchMixPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfWatchMixPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_BroadcastMixPicture
    * description   : �㲥��Ƶ�໭��
    * input         : pResourceID ��ԴID
                      pConfBroadcastMixPictureParam �㲥��Ƶ�໭�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_BroadcastMixPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfBroadcastMixPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_CancelBroadcastMixPicture
    * description   : ȡ���㲥��Ƶ�໭��
    * input         : pResourceID ��ԴID
                      pConfCancelBroadcastMixPictureParam ȡ���㲥��Ƶ�໭�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_CancelBroadcastMixPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfCancelBroadcastMixPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_FreeMode
    * description   : ��������ģʽ
    * input         : pResourceID ��ԴID
                      pConfFreeModeParam ����ģʽ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_FreeMode(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfFreeModeParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_HangupMember
    * description   : �Ҷϳ�Ա
    * input         : pResourceID ��ԴID
                      pConfHangupMemberParam �Ҷϳ�Ա����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_HangupMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfHangupMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_DelMember
    * description   : ɾ����Ա
    * input         : pResourceID ��ԴID
                      pConfDelMemberParam ɾ����Ա����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_DelMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfDelMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_QueryVWallDisplayMatrixInfo
    * description   : ��ѯ������ǽ��ʾģʽ
    * input         : pResourceID ��ԴID
    * output        : N/A����ѯ������ϱ�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_QueryVWallDisplayMatrixInfo(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_QueryVWallLayers
    * description   : ��ѯ������ǽͼ��
    * input         : pResourceID ��ԴID
    * output        : N/A����ѯ������ϱ�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_QueryVWallLayers(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_QueryURLByConfID
    * description   : ��ѯ����¼����Ϣ
    * input         : pResourceID ��ԴID
                      pConfQueryURLByConfIDParam ��ѯ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_QueryURLByConfID(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfQueryURLByConfIDParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_QueryConfAttendee
    * description   : ��ѯ��������ڵĻ�����Ϣ
    * input         : pResourceID ��ԴID
                      pConfQueryConfParam ��ѯ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_QueryConfAttendee(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfQueryConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_CallSendDTMF
    * description   : ���β���
    * input         : pResourceID ��ԴID
                      pDtmfParam���β��Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CallSendDTMF(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pDtmfParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetVdmSessions
    * description   : ��ȡVDM�Ự��Դ��VDM����ʹ�ã�
    * input         : N/A
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetVdmSessions();

    /**************************************************************************
    * name          : ELTE_SDK_SetConflictManagerSwitch
    * description   : ���ó�ͻ�����ܿ��أ�Ĭ��Ϊ�أ�
    * input         : iSwitch ���ز�����0�رգ�1�򿪣�
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetConflictManagerSwitch(const ELTE_INT32 iSwitch);

    /**************************************************************************
    * name          : ELTE_SDK_GetIpccIpAndPort
    * description   : ��ȡIPCC��ַ
    * input         : N/A
    * output        : pIpccIps IPCC�ĵ�ַ�Ͷ˿ں�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetIpccIpAndPort(ELTE_CHAR** pIpccIps);

    /**************************************************************************
    * name          : ELTE_SDK_GetIpccInfos
    * description   : ��ȡIPCC��Ϣ
    * input         : N/A
    * output        : pIpccInfos IPCC����Ϣ�б�
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetIpccInfos(ELTE_CHAR** pIpccInfos);

    /**************************************************************************
    * name          : ELTE_SDK_GetUnifiedToken
    * description   : ��ȡͳһ��¼Token
    * input         : N/A
    * output        : pToken ͳһ��¼��Token
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUnifiedToken(ELTE_CHAR** pToken);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_SetSpokenMember
    * description   : ���÷�����
    * input         : pResourceID ��ԴID
                      pConfSetSpokenMemberParam ���÷����˲���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_SetSpokenMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfSetSpokenMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetPDTGroupInfo
    * description   : ��ѯ�ն˵�ǰͨ������Ϣ
    * input         : pResourceID ��ԴID
                      pPDTGroupQueryParam ��ѯ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPDTGroupInfo(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pPDTGroupQueryParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_GetContinuousPresenceInfo
    * description   : ��ѯ����໭����Ϣ
    * input         : pResourceID ��ԴID
                      pConfQueryConfParam ��ѯ����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_GetContinuousPresenceInfo(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfQueryParam);
    //==============added for ICP end===================================


    /**************************************************************************
    * name          : ELTE_SDK_RTSPStart
    * description   : rtsp����¼��¼��
    * input         : pRTSPStartParam ���Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPStart(const ELTE_CHAR* pRTSPStartParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPClose
    * description   : �ر�rtsp����¼��¼��
    * input         : pRTSPCloseParam �رղ��Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPClose(const ELTE_CHAR* pRTSPCloseParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPPause
    * description   : ��ͣrtsp����¼��¼��
    * input         : pRTSPStartParam ��ͣ���Ų���
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPPause(const ELTE_CHAR* pRTSPPauseParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPPlay
    * description   : rtsp���ò�����ʼ��
    * input         : ELTE_SDK_RTSPPlay ���ò�����ʼ�����
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPPlay(const ELTE_CHAR* pRTSPPlayParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPRecv
    * description   : rtsp������Ƶ
    * input         : pResourceID  rtsp sessionId
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPRecv(const ELTE_CHAR* pResourceID);
    /**************************************************************************
    * name          : ELTE_SDK_SetListenGroup
    * description   : ���ü�����
    * input         : pResourceID ��ԴID
                      pSetListenGroupParam ���ü��������
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetListenGroup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSetListenGroupParam);
    /**************************************************************************
    * name          : ELTE_SDK_SetRing
    * description   : ���������������iRingType:0----���壻1-----������
    * input         : iRingType ���忪��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetRing(const ELTE_INT32 iRingType);

    /**************************************************************************
    * name          : ELTE_SDK_SetLinkedPhone
    * description   : �󶨻��������Ľӿ�
    * input         : pLinkedPhoneParam ������������
                        iPhoneISDNΪ�󶨵�Ӳ�������룻iLinkedModeΪ��ģʽ0---�󶨣�1----��󶨣�iAutoanswerΪ��������������Ӧ��ģʽ��
                        0---Ϊ�˹�Ӧ��1---Ϊ�Զ�Ӧ��Ĭ��Ϊ1�Զ�Ӧ��
    * output        : N/A
    * return        : �ɹ�����0��ʧ�ܷ��ش�����
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLinkedPhone(const ELTE_CHAR* pLinkedPhoneParam);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif
