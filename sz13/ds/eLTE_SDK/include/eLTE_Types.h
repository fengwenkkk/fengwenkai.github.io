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
filename        :   eLTE_Types.h
author          :   zWX229156
created         :   2015/1/6
description     :   �������������Լ��ص��������ض���
copyright       :   Copyright (C) 2015-2017
history         :   2015/1/6 ��ʼ�汾
*********************************************************************/
#ifndef __ELTE_TYPES_H__
#define __ELTE_TYPES_H__

#ifdef WIN32
#define __SDK_CALL __stdcall
#else
#define __SDK_CALL
#endif

typedef int               ELTE_INT32;
typedef unsigned int      ELTE_UINT32;
typedef char              ELTE_CHAR;
typedef unsigned char     ELTE_UCHAR;
typedef void              ELTE_VOID;
typedef unsigned long     ELTE_ULONG;
typedef unsigned short    ELTE_USHORT;
typedef long long         ELTE_INT64;

// ��Ϣ����(��Ϣ�ϱ�)
typedef enum
{
    ELTE_Event_NotifyUserStatus = 0,        // �豸״̬�仯�¼�֪ͨ
    ELTE_Event_NotifyP2pvideocallStatus,    // ��ʼʵ���¼�֪ͨ
    ELTE_Event_NotifyResourceStatus,        // Ⱥ���ϵ״̬�仯�¼�֪ͨ
    ELTE_Event_NotifyProvisionChange,       // �豸�������ñ��֪ͨ�¼�
    ELTE_Event_NotifyProvisionAllResync,    // �Զ�������������֪ͨ�¼�
    ELTE_Event_NotifyP2pcallStatus,         // ���״̬�仯�¼�
    ELTE_Event_NotifyGroupStatus,           // ���״̬�仯�¼�
    ELTE_Event_NotifyModuleStatus,          // ģ�����״̬�¼�
    ELTE_Event_NotifyGISReport,             // GIS��Ϣ֪ͨ�¼�
    ELTE_Event_NotifyGISStatus,             // GIS״̬֪ͨ�¼�
    ELTE_Event_NotifySDSReport,             // ����/���Ž����ϱ��¼�
    ELTE_Event_NotifySDSSendStatus,         // ����/���ŷ���״̬֪ͨ�¼�
    ELTE_Event_NotifyServiceStatus,         // ����ֹ֪ͣͨ�¼�
    ELTE_Event_NotifyConfStatus             // ����Ƶ����֪ͨ�¼�
} eLTE_SDK_EVENT_TYPE;

/**************************************************************************
* name          : EventCallBack
* description   : �¼��ص�����(�¼�֪ͨ���쳣�ص���)
* input         : N/A
* output        : iEventType        �¼�����
                  pEventBuf         �¼�����
                  uiBufSize         Buf����
                  pUserData         �û�����
* return        : N/A
**************************************************************************/
typedef ELTE_VOID (__SDK_CALL * EventCallBack)(ELTE_INT32 iEventType, ELTE_VOID* pEventBuf, ELTE_UINT32 uiBufSize, ELTE_VOID* pUserData);

typedef struct _YuvFrame{
    ELTE_INT32 w,h;
    ELTE_UCHAR *planes[4]; /*we usually use 3 planes, 4th is for compatibility */
    ELTE_INT32 strides[4];  /*with ffmpeg's swscale.h */
    int type;           /** 0 receiv remote video 1 local captrue video**/
}YuvFrame;

/**************************************************************************
* name          : YUVFrameCallBack
* description   : ��Ƶ���ݻص�����
* input         : N/A
* output        : iResId            ��ԴID
                  pYuvFrame         ��Ƶ����
                  pUserData         �û�����
* return        : N/A
**************************************************************************/
typedef ELTE_VOID (__SDK_CALL * YUVFrameCallBack)(ELTE_INT32 iResId, YuvFrame* pYuvFrame, ELTE_VOID* pUserData);

typedef ELTE_VOID (__SDK_CALL * YUVFrameCallBack64)(ELTE_INT64 iResId, YuvFrame* pYuvFrame, ELTE_VOID* pUserData);

#endif //__ELTE_TYPES_H__
