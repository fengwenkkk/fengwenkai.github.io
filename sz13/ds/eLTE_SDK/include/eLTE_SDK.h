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
description     :   定义 DLL 应用程序的导出函数
copyright       :   Copyright (C) 2015-2017
history         :   2015/1/5 初始版本
*********************************************************************/

#ifndef __ELTE_SDK_H__
#define __ELTE_SDK_H__

#include "eLTE_Types.h"

// 这里提供DLL导出接口
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
    * description   : 设置日志路径，请使用绝对路径方便查看日志文件
    * input         : pLogFilePath 日志保存路径
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLogPath(const ELTE_CHAR* pLogFilePath);

    /**************************************************************************
    * name          : ELTE_SDK_SetLogLevel
    * description   : 设置日志级别
    * input         : iLogLevel 日志级别
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLogLevel(ELTE_UINT32 iLogLevel);

    /**************************************************************************
    * name          : ELTE_SDK_ReleaseBuffer
    * description   : 释放SDK内部申请的内存
    * input         : pBuffer 内存指针
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL  ELTE_SDK_ReleaseBuffer(ELTE_CHAR* pBuffer);

    /**************************************************************************
    * name          : ELTE_SDK_GetVersion
    * description   : 获取SDK版本号
    * input         : pVersion 获取SDK版本号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetVersion(ELTE_CHAR** pVersion);

    /**************************************************************************
    * name          : ELTE_SDK_Init
    * description   : 初始化SDK
    * input         : N/A
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Init(ELTE_INT32 iMediaBypass);

    /**************************************************************************
    * name          : ELTE_SDK_Cleanup
    * description   : 释放SDK
    * input         : N/A
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Cleanup();

    /**************************************************************************
    * name          : ELTE_SDK_SetEventCallBack
    * description   : 设置消息回调函数
    * input         : fEventCallBack 用户设置的事件回调函数，为空表示取消设置的回调函数
                      pUserData      用户数据
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetEventCallBack(EventCallBack fEventCallBack, ELTE_VOID* pUserData);

    /**************************************************************************
    * name          : ELTE_SDK_SetYUVFrameCallBack
    * description   : 设置视频数据回调函数
    * input         : fYUVFrameCallBack 用户设置的视频数据回调函数，为空表示取消设置的回调函数
                      pUserData      用户数据
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetYUVFrameCallBack(YUVFrameCallBack fYUVFrameCallBack, ELTE_VOID* pUserData);

    /**************************************************************************
    * name          : ELTE_SDK_SetYUVFrameCallBack64
    * description   : 设置视频数据回调函数
    * input         : fYUVFrameCallBack 用户设置的视频数据回调函数，为空表示取消设置的回调函数
                      pUserData      用户数据
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetYUVFrameCallBack64(YUVFrameCallBack64 fYUVFrameCallBack, ELTE_VOID* pUserData);

    /**************************************************************************
    * name          : ELTE_SDK_Login
    * description   : 登录
    * input         : pUserID         调度员账号
                      pPWD            调度员密码
                      pServerIP       调度机IP地址
                      pLocalIP        本地主机IP地址
                      uiServerSIPPort 调度机SIP服务端口
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Login(const ELTE_CHAR* pUserID, const ELTE_CHAR* pPWD, const ELTE_CHAR* pServerIP, const ELTE_CHAR* pLocalIP, ELTE_UINT32 uiServerSIPPort);

    /**************************************************************************
    * name          : ELTE_SDK_Logout
    * description   : 注销
    * input         : pUserID 调度员账号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Logout(const ELTE_CHAR* pUserID);

    /**************************************************************************
    * name          : ELTE_SDK_TriggerStatusReport
    * description   : 触发状态上报
    * input         : iEnableStatusReport 是否开启状态上报
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TriggerStatusReport(ELTE_INT32 iEnableStatusReport);

    /**************************************************************************
    * name          : ELTE_SDK_ProvisionManagerInitMRS
    * description   : 媒体服务器初始化
    * input         : pServerIP 媒体服务器IP地址
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ProvisionManagerInitMRS(const ELTE_CHAR* pServerIP);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcGroups
    * description   : 获取调度台群组列表
    * input         : pUserID 调度台的用户号
    * output        : pDcGroups 该调度台所属的所有群组列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcGroups(const ELTE_CHAR* pUserID, ELTE_CHAR** pDcGroups);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcGroups
    * description   : 按类型获取调度台所管理的群组列表
    * input         : pUserID 调度台的用户号
                      pType 类型
    * output        : pDcGroups 该调度台所属的改类型的所有群组列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcGroupsByType(const ELTE_CHAR* pUserID, const ELTE_CHAR* pType, ELTE_CHAR** pDcGroups);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcUsers
    * description   : 获取调度台用户列表
    * input         : pUserID 调度台的用户号
    * output        : pDcUsers 该调度台所管理的所有用户列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcUsers(const ELTE_CHAR* pUserID, ELTE_CHAR** pDcUsers);
    /**************************************************************************
    * name          : ELTE_SDK_GetDcUsers
    * description   : 按类型获取调度台所管理的用户列表
    * input         : pUserID 调度台的用户号
                      pType   类型
    * output        : pDcUsers 该调度台所管理的该类型的所有用户列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcUsersCloneByType(const ELTE_CHAR* pUserID,const ELTE_CHAR* pType, ELTE_CHAR** pDcUsers);

    /**************************************************************************
    * name          : ELTE_SDK_GetGroupUsers
    * description   : 获取一个群组成员列表
    * input         : pGroupID 群组号
    * output        : pGroupUsers 该群组的所有成员列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGroupUsers(const ELTE_CHAR* pGroupID, ELTE_CHAR** pGroupUsers);

    /**************************************************************************
    * name          : ELTE_SDK_GetGroupInfo
    * description   : 获取一个群组详细信息
    * input         : pGroupID 群组号
    * output        : pGroupInfo 该群组的详细信息
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGroupInfo(const ELTE_CHAR* pGroupID, ELTE_CHAR** pGroupInfo);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserInfo
    * description   : 获取一个用户详细信息
    * input         : pUserID 调度员账号
    * output        : pUserInfo 该用户的详细信息
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserInfo(const ELTE_CHAR* pUserID, ELTE_CHAR** pUserInfo);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcInfo
    * description   : 获取一个用户详细信息
    * input         : pUserID 调度台的用户号
    * output        : pDcInfo 该调度台详细信息
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcInfo(const ELTE_CHAR* pUserID, ELTE_CHAR** pDcInfo);

    /**************************************************************************
    * name          : ELTE_SDK_UnSubscribeGroup
    * description   : 取消订阅群组
    * input         : pGroupID              群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_UnSubscribeGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_SubscribeGroup
    * description   : 订阅群组
    * input         : pGroupID              群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SubscribeGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_JoinGroup
    * description   : 调度台迟后加入某群组群组会话（需要调度台已是该群组成员，本操作不是将调度台变为群组成员）
    * input         : pGroupID              群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_JoinGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_SubJoinGroup
    * description   : 订阅并自动加入群组
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SubJoinGroup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_CreateDynamicGroup
    * description   : 创建动态群组
    * input         : pDGNAParam 动态组参数信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CreateDynamicGroup(const ELTE_CHAR* pDGNAParam);

    /**************************************************************************
    * name          : ELTE_SDK_MultiGrpCreate
    * description   : 调度台创建多选组
    * input         : pMultiGrpParam 多选组组参数信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_MultiGrpCreate(const ELTE_CHAR* pMultiGrpParam);

    /**************************************************************************
    * name          : ELTE_SDK_AddMultiGrpMember
    * description   : 调度台增加多选组成员
    * input         : pMultiGrpParam 多选组组参数信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AddMultiGrpMember(const ELTE_CHAR* pMultiGrpParam);

    /**************************************************************************
    * name          : ELTE_SDK_AddMultiGrpMember
    * description   : 调度台删除多选组成员
    * input         : pMultiGrpParam 多选组组参数信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_DeleteMultiGrpMember(const ELTE_CHAR* pMultiGrpParam);

    /**************************************************************************
    * name          : ELTE_SDK_CancelDynamicGroup
    * description   : 删除动态群组
    * input         : pResourceID 动态组组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CancelDynamicGroup(const ELTE_CHAR* pResourceID);
    /**************************************************************************
    * name          : ELTE_SDK_CancelMultiGrp
    * description   : 删除多选组
    * input         : pResourceID 多选组组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CancelMultiGrp(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_GetTempGroupID
    * description   : 获取临时组号
    * input         : N/A
    * output        : iGroupID 临时组组号
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetTempGroupID(ELTE_INT32* iGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_GetTempGroupID64(long long型)
    * description   : 获取临时组号
    * input         : N/A
    * output        : iGroupID 临时组组号
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetTempGroupID64(ELTE_INT64* iGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_CreateTempGroup
    * description   : 创建临时组
    * input         : pTEMPGParam 临时组参数信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CreateTempGroup(const ELTE_CHAR* pTEMPGParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserRECFileInfoList
    * description   : 获取录像文件列表
    * input         : pQueryXml 查询录像条件Xml
    * output        : pRspXml   查询录像文件结果列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserRECFileInfoList(const ELTE_CHAR* pQueryXml, ELTE_CHAR** pRspXml);

    /**************************************************************************
    * name          : ELTE_SDK_StartRealPlay
    * description   : 开始播放实时视频
    * input         : pResourceID 资源ID
                      pVideoParam 媒体参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartRealPlay(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoParam);

    /**************************************************************************
    * name          : ELTE_SDK_StartCircleDisPlay
    * description   : 实现对一组摄像头进行视频轮询
    * input         : pResourceID 资源ID
                      pCircleDisPlayXml 媒体参数
                      pTimerPeriod 周期
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartCircleDisPlay(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pCircleDisPlayXml,const ELTE_CHAR* pTimerPeriod);

    /**************************************************************************
    * name          : ELTE_SDK_StartVideoDial
    * description   : 开始视频点呼
    * input         : pResourceID 资源ID
                      pVideoParam 媒体参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartVideoDial(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoParam);

    /**************************************************************************
    * name          : ELTE_SDK_SetPlayWindow
    * description   : 设置播放实时视频窗口
    * input         : pResourceID 资源ID
                      pVideoParam 窗口句柄
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetPlayWindow(const ELTE_CHAR* pResourceID, ELTE_VOID* hWnd);
    /**************************************************************************
    * name          : ELTE_SDK_SetLocalPlayWindow
    * description   : 设置视频点呼本地窗口
    * input         : pResourceID 资源ID
                      pVideoParam 窗口句柄
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLocalPlayWindow(const ELTE_CHAR* pResourceID, ELTE_VOID* hWnd);

    /**************************************************************************
    * name          : ELTE_SDK_LocalSnapshot
    * description   : 抓拍视频画面
    * input         : pResourceID 资源ID
                      pSnapshotPath 图片保存路径+文件名
                      ulSnapshotFormat 抓图格式，1 JPG，2 BMP
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_LocalSnapshot(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSnapshotPath, ELTE_ULONG ulSnapshotFormat);

    /**************************************************************************
    * name          : ELTE_SDK_SetPlayWindowSize
    * description   : 设置播放实时视频窗口的大小
    * input         : pResourceID 资源ID
                      ulWidth           宽度
                      ulHeight          高度
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetPlayWindowSize(const ELTE_CHAR* pResourceID, ELTE_ULONG ulWidth, ELTE_ULONG ulHeight);
    /**************************************************************************
    * name          : ELTE_SDK_SetPlayWindowSize
    * description   : 设置视频点呼本地窗口的大小
    * input         : pResourceID 资源ID
                      ulWidth           宽度
                      ulHeight          高度
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLocalPlayWindowSize(const ELTE_CHAR* pResourceID, ELTE_ULONG ulWidth, ELTE_ULONG ulHeight);

    /**************************************************************************
    * name          : ELTE_SDK_PTZControl
    * description   : 开始云台控制
    * input         : pResourceID      带云台的摄像机资源ID
                      iPTZControlCode  云台控制命令
                      iPTZControlValue 云台控制命令对应的参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTZControl(const ELTE_CHAR* pResourceID, ELTE_UINT32 iPTZControlCode, ELTE_UINT32 iPTZControlValue);

    /**************************************************************************
    * name          : ELTE_SDK_StopRealPlay
    * description   : 停止播放实时视频
    * input         : pResourceID      带云台的摄像机资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopRealPlay(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StopCircleDisPlay
    * description   : 调度台终止并挂断正在进行轮询的摄像头
    * input         : pResourceID     资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopCircleDisPlay(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PVideoReject
    * description   : 拒绝视频回传
    * input         : pResourceID      资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PVideoReject(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PDial
    * description   : 发起语音点呼
    * input         : pResourceID  资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PDial(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PHalfDpxDial
    * description   : 释放半双工点呼
    * input         : pResourceID  点呼的对端资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PHalfDpxRelease(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PHalfDpxDial
    * description   : 发起半双工点呼
    * input         : pResourceID  点呼的对端资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PHalfDpxDial(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PRecv
    * description   : 接收语音点呼
    * input         : pResourceID  资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PRecv(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PReject
    * description   : 拒接语音点呼
    * input         : pResourceID  资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PReject(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PHangup
    * description   : 挂断语音点呼
    * input         : pResourceID  资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PHangup(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PBreakoff
    * description   : 调度台强拆点呼
    * input         : pResourceID  资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PBreakoff(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PBreakin
    * description   : 抢话（强拆点呼+发起新点呼）
    * input         : pResourceID  资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PBreakin(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTDial
    * description   : 发起组呼（固定和临时组呼）或抢权
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTDial(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTBreakin
    * description   : 发起组呼（固定和临时组呼）或抢权
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTBreakin(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTDialMultiGrp
    * description   : 调度台发起多选组组呼或抢权
    * input         : pGroupID  组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTDialMultiGrp(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTRelease
    * description   : 释放话权
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTRelease(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTReleaseMultiGrp
    * description   : 调度台释放多选组话权
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTReleaseMultiGrp(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTHangup
    * description   : 退出组呼
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTHangup(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_PTTEmergency
    * description   : 发起紧急组呼
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_PTTEmergency(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_GroupBreakoff
    * description   : 调度台强拆组呼
    * input         : pGroupID  群组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GroupBreakoff(const ELTE_CHAR* pGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_VolMute
    * description   : 调度台执行静音
    * input         : pResourceID 资源ID
                      pMuteParam 静音参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VolMute(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pMuteParam);

    /**************************************************************************
    * name          : ELTE_SDK_MuteControl
    * description   : 调度台针对群组或用户对象静音。此操作只需要关注ALL_MUTE。
    * input         : pResourceID 资源ID
                      pMuteParam 操作参数
                                SINGLE_UNMUTE   0, //单个去静音
                                SINGLE_MUTE     1, //单个静音
                                ALL_UNMUTE      2, //批量统一去静音
                                ALL_MUTE        3  //批量统一静音
                                ALL_MUTE_PTT    4,
                                ALL_UNMUTE_PTT  5
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_MuteControl(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pMuteParam);

    /**************************************************************************
    * name          : ELTE_SDK_VolUnMute
    * description   : 调度台取消静音
    * input         : pResourceID 资源ID
                        pUnMuteParam 取消静音参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VolUnMute(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pUnMuteParam);

    /**************************************************************************
    * name          : ELTE_SDK_VolumeControl
    * description   : 控制当前通话音量
    * input         : pResourceID 资源ID
                      pVolumeControlParam 音量参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VolumeControl(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVolumeControlParam);

    /**************************************************************************
    * name          : ELTE_SDK_GISSubscribe
    * description   : 订阅/去订阅GIS终端
    * input         : pResourceID 资源ID
                      pGISParam GIS参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GISSubscribe(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pGISParam);

    /**************************************************************************
    * name          : ELTE_SDK_ResUnsubBatch
    * description   : 批量去订阅用户
    * input         : pResourceID 资源ID
                      pSublst 去订阅列表
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ResUnsubBatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSublst);

    /**************************************************************************
    * name          : ELTE_SDK_ResSubBatch
    * description   : 批量订阅用户
    * input         : pResourceID 资源ID
                      pSublst 订阅列表
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ResSubBatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSublst);


    /**************************************************************************
    * name          : ELTE_SDK_SDSSendMessage
    * description   : 发送短/彩信
    * input         : pResourceID 资源ID
                      pSDSParam 短/彩信参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SDSSendMessage(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSDSParam);

    /**************************************************************************
    * name          : ELTE_SDK_RealTimeMsgSend
    * description   : 调度台发送即时消息
    * input         : pResourceID 资源ID
                      pSDSParam 短/彩信参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RealTimeMsgSend(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSDSParam);

    /**************************************************************************
    * name          : ELTE_SDK_RealTimeMsgSend
    * description   : 调度台发送即时消息回执
    * input         : pResourceID 资源ID
                      pSDSParam 短/彩信参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ReplyMsgSend(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSDSParam);

    /**************************************************************************
    * name          : ELTE_SDK_SDSExportMessage
    * description   : 导出已发送和接收的短信/彩信。
    * input         : pResourceID 资源ID，登录账号
                      pPath 路径
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SDSExportMessage(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pPath);

    /**************************************************************************
    * name          : ELTE_SDK_StartVideoDispatch
    * description   : 开始视频分发
    * input         : pResourceID 资源ID
                      pVideoDispatchParam 视频分发参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartVideoDispatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoDispatchParam);

    /**************************************************************************
    * name          : ELTE_SDK_StopVideoDispatch
    * description   : 停止视频分发
    * input         : pResourceID 资源ID
                      pVideoDispatchParam 视频分发参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopVideoDispatch(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pVideoDispatchParam);

    /**************************************************************************
    * name          : ELTE_SDK_RecvVideoPlay
    * description   : 调度台接收视频回传
    * input         : pResourceID 资源ID
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RecvVideoPlay(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StartVWall
    * description   : 发起视频上墙/会议上墙
    * input         : pResVWallID 资源ID
                      pVWallStartParam 视频上墙参数/会议上墙参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VWallStart(const ELTE_CHAR* pResVWallID, const ELTE_CHAR* pVWallStartParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcVWallIDList
    * description   : 获取视频上墙ID列表
    * input         : DstObjId 视频上墙的目的地，为一个视频通道
                      pVWallIDList 视频上墙目的ID列表
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcVWallIDList(ELTE_CHAR** pVWallIDList);

    /**************************************************************************
    * name          : ELTE_SDK_VWallStop
    * description   : 终止视频下墙/终止会议下墙
    * input         : pResVWallID 视频上墙的源，为一个摄像头
                      pVWallStopParam 视频下墙参数/会议下墙参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_VWallStop(const ELTE_CHAR* pResVWallID, const ELTE_CHAR* pVWallStopParam);

    /**************************************************************************
    * name          : ELTE_SDK_TelephoneDial
    * description   : 发起PSTN/PLMN电话呼叫
    * input         : pTelNumber PSTN/PLMN电话号码
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TelephoneDial(const ELTE_CHAR* pTelNumber);

    /**************************************************************************
    * name          : ELTE_SDK_TelephoneHangup
    * description   : 停止PSTN/PLMN电话呼叫
    * input         : pTelNumber PSTN/PLMN电话号码
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TelephoneHangup(const ELTE_CHAR* pTelNumber);

    /**************************************************************************
    * name          : ELTE_SDK_StartDiscreetListen
    * description   : 发起缜密监听
    * input         : pResourceID 资源对象，可以是群组或用户
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartDiscreetListen(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StopDiscreetListen
    * description   : 停止缜密监听
    * input         : pResourceID 资源对象，可以是群组或用户
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopDiscreetListen(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_StartEnvironmentListen
    * description   : 发起环境侦听
    * input         : pResourceID 资源对象，可以是群组或用户
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartEnvironmentListen(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_P2PTransfer
    * description   : 发起人工转接
    * input         : pResourceID 资源对象，一般是发起人工转接的调度员
                      pP2PTransferParam 人工转接参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_P2PTransfer(const ELTE_CHAR* pResourceID,const ELTE_CHAR* pP2PTransferParam);

    /**************************************************************************
    * name          : ELTE_SDK_CreatePatchGroup
    * description   : 创建派接组
    * input         : pP2PTransferParam 派接组参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CreatePatchGroup(const ELTE_CHAR* pPatchGroupParam);

    /**************************************************************************
    * name          : ELTE_SDK_CancelPatchGroup
    * description   : 取消已创建派接组
    * input         : pPatchGroupID 资源对象，一般是创建派接组的组号。
                                    通过notifyResourceStatus读出的组号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CancelPatchGroup(const ELTE_CHAR* pPatchGroupID);

    /**************************************************************************
    * name          : ELTE_SDK_AddPatchGroupMember
    * description   : 增加派接组成员
    * input         : pPatchGroupParam 派接组参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AddPatchGroupMember(const ELTE_CHAR* pPatchGroupParam);

    /**************************************************************************
    * name          : ELTE_SDK_DeletePatchGroupMember
    * description   : 删除派接组成员
    * input         : pPatchGroupParam 派接组参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_DeletePatchGroupMember(const ELTE_CHAR* pPatchGroupParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetPatchGroups
    * description   : 获取指定调度台的所有派接组列表
    * input         : pDcUser 调度台用户
                      pPatchGroups 该调度台管理的派接组信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPatchGroups(const ELTE_CHAR* pDcUser, ELTE_CHAR** pPatchGroups);

    /**************************************************************************
    * name          : ELTE_SDK_GetSpeaker4OrMic4GroupOrIndividual
    * description   : 获取用户或者群组的扬声器或麦克风
    * input         : pResID 用户或者群组号,可以不指定，传NULL
                      pResType 资源号类型 0：用户 ， 1：群组 必传参数
                      pDeviceType 设备类型 0：扬声器 ， 1：麦克风 必传参数
    * output        : pSpeakerName 扬声器或者麦克风名称
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetSpeaker4OrMic4GroupOrIndividual(const ELTE_CHAR* pResID,      const ELTE_CHAR* pResType,
                                                                                   const ELTE_CHAR* pDeviceType, ELTE_CHAR** pSpeakerOrMicName);

    /**************************************************************************
    * name          : ELTE_SDK_GetMultiSelectedGrpInfoVec
    * description   : 获取当前调度台创建的所有多选组信息
    * input         :   pDcUser 调度台用户
                        pMultiSelectedGrpInfoVec 当前调度台创建的所有多选组信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetMultiSelectedGrpInfoVec(const ELTE_CHAR* pDcUser, ELTE_CHAR** pMultiSelectedGrpInfoVec);

    /**************************************************************************
    * name          : ELTE_SDK_GetDynamicGroupMembers
    * description   : 获取动态组成员列表
    * input         :   pGroupId 动态组组号
                        pDynamicGroupMembers
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDynamicGroupMembers(const ELTE_CHAR* pGroupId, ELTE_CHAR** pDynamicGroupMembers);

    /**************************************************************************
    * name          : ELTE_SDK_GetUEAbilityClone
    * description   : 获取终端能力信息
    * input         :   pUeId 终端号码
                        pProvUEAblity 获取终端能力信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUEAbilityClone(const ELTE_CHAR* pUeId, ELTE_CHAR** pProvUEAblity);

    /**************************************************************************
    * name          : ELTE_SDK_GetGroupMemberByPatchId
    * description   : 获取指定派接组管理的成员列表
    * input         : pPatchGroupId 派接组号
                      pPatchGroupMembers 该派接组成员信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGroupMemberByPatchId(const ELTE_CHAR* pPatchGroupId, ELTE_CHAR** pPatchGroupMembers);

    /**************************************************************************
    * name          : ELTE_SDK_GetPatchGroupInfo
    * description   : 获取指定派接组属性信息
    * input         : pPatchGroupId 派接组号
                      pPatchGroupInfo 该派接的属性信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPatchGroupInfo(const ELTE_CHAR* pPatchGroupId, ELTE_CHAR** pPatchGroupInfo);

    /**************************************************************************
    * name          : ELTE_SDK_GetPatchInfoCloneByMemberId
    * description   : 通过调度台普通组号获取调度台派接组信息
    * input         : pMemberID 调度台被派接的普通组号码
    * output        : pPatchGroupInfo 该派接的属性信息
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPatchInfoCloneByMemberId(const ELTE_CHAR* pMemberID, ELTE_CHAR** pPatchGroupInfo);


    /**************************************************************************
    * name          : ELTE_SDK_GetGisSubscription
    * description   : 获取调度台订阅GIS上报的终端列表
    * input         : pResourceID 调度台ID
    * output        : pGisQuerySubList 订阅GIS上报的终端列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGisSubscription(const ELTE_CHAR* pResourceID, ELTE_CHAR** pGisQuerySubList);

    /**************************************************************************
    * name          : ELTE_SDK_ModifyDynamicGroup
    * description   : 修改动态群组
    * input         : pResourceID 创建动态组的调度台 ID
                      pDynamicGroupInfo 动态组修改信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_ModifyDynamicGroup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pDynamicGroupInfo);

    /**************************************************************************
    * name          : ELTE_SDK_TempUserJoinGroup
    * description   : 组呼添加临时用户
    * input         : pResourceID 群组ID
                      pPhonePatchParam 待添加的用户信息
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_TempUserJoinGroup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pPhonePatchParam);

    /**************************************************************************
    * name          : ELTE_SDK_StartRecord
    * description   : 开始音视频录制
    * input         : pResourceID 与之通话的组或终端的资源对象
                      pTypeParam 录音录像的相关选项
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StartRecord(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pTypeParam);

    /**************************************************************************
    * name          : ELTE_SDK_StopRecord
    * description   : 终止音视频录制
    * input         : pResourceID 与之通话的组或终端的资源对象
                      pTypeParam 录音录像的相关选项
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_StopRecord(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pTypeParam);

    /**************************************************************************
    * name          : ELTE_SDK_EnumSpeakers
    * description   : 枚举调度台的扬声器
    * input         : N/A
    * output        : pSpeakers 获取扬声器名称的字符串数组
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_EnumSpeakers(ELTE_CHAR** pSpeakers);

    /**************************************************************************
    * name          : ELTE_SDK_EnumMicrophones
    * description   : 枚举调度台的麦克风
    * input         : N/A
    * output        : pMicrophones 获取麦克风名称的字符串数组
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_EnumMicrophones(ELTE_CHAR** pMicrophones);


        /**************************************************************************
    * name          : ELTE_SDK_AssignSpeaker4Group
    * description   : 设置群组扬声器
    * input         : pGroupID 群组ID
                    ：pSpeaker 要设置的扬声器名称, 设置为NULL时，选择默认扬声器
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignSpeaker4Group(const ELTE_CHAR* pGroupID, const ELTE_CHAR* pSpeaker);

    /**************************************************************************
    * name          : ELTE_SDK_AssignMic4Group
    * description   : 设置群组麦克风
    * input         : pGroupID 群组ID
                    ：pMicrophone 要设置的麦克风名称, 设置为NULL时，选择默认麦克风
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignMic4Group(const ELTE_CHAR* pGroupID, const ELTE_CHAR* pMicrophone);

    /**************************************************************************
    * name          : ELTE_SDK_AssignSpeaker4Individual
    * description   : 设置用户扬声器
    * input         : pResourceID 用户ID
                    ：pSpeaker 要设置的扬声器名称, 设置为NULL时，选择默认扬声器
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignSpeaker4Individual(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSpeaker);

    /**************************************************************************
    * name          : ELTE_SDK_AssignMic4Individual
    * description   : 设置用户麦克风
    * input         : pResourceID 群组ID
                    ：pMicrophone 要设置的麦克风名称, 设置为NULL时，选择默认麦克风
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_AssignMic4Individual(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pMicrophone);

    /**************************************************************************
    * name          : ELTE_SDK_GetLocalIp
    * description   : 获取本地Ip地址
    * input         : N/A
    * output        : pLocalIp 获取本地Ip地址
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetLocalIps(ELTE_CHAR** pLocalIps);

    /**************************************************************************
    * name          : ELTE_SDK_GetDepartmentInfoList
    * description   : 获取部门信息列表
    * input         : N/A
    * output        : pDepartmentInfoList 部门信息列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDepartmentInfoList(ELTE_CHAR** pDepartmentInfoList);

    /**************************************************************************
    * name          : ELTE_SDK_IsInDcManagedDepartmentRange
    * description   : 查询某个部门是否归本调度台管理
    * input         : pDepartmentID 需要查询的部门ID
    * output        : pResult 查询的结果 "true"代表是，"false"代表否
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_IsInDcManagedDepartmentRange(const ELTE_CHAR* pDepartmentID, ELTE_CHAR** pQueryResult);

    /**************************************************************************
    * name          : ELTE_SDK_GetDcCameraAttribute
    * description   : 获取所有摄像头属性
    * input         : pUserID 调度员账号
    * output        : pCameraAttributeList 摄像头属性列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDcCameraAttribute(const ELTE_CHAR* pUserID, ELTE_CHAR** pCameraAttributeList);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserLevelInfo
    * description   : 获取所有摄像头层级信息
    * output        : pUserLevelInformationList 摄像头层级信息
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserLevelInfo(ELTE_CHAR** pUserLevelInformationList);

    /**************************************************************************
    * name          : ELTE_SDK_GetUserCapLvlRelInfo
    * description   : 获取摄像头层级管理权限
    * output        : pUserCapLevelRelationInfoList 摄像头层级管理权限信息
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUserCapLvlRelInfo(ELTE_CHAR** pUserCapLevelRelationInfoList);

    /**************************************************************************
    * name          : ELTE_SDK_GetGisIpcList
    * description   : 获取所有摄像头GPS信息
    * input         : pUserID 调度员账号
    * output        : pProvGisIpcList 摄像头GPS信息列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetGisIpcList(const ELTE_CHAR* pUserID, ELTE_CHAR** pProvGisIpcList);

    /**************************************************************************
    * name          : ELTE_SDK_GetLocalCameraStatus
    * description   : 获取所有摄像头GPS信息
    * input         : pUserID 调度员账号
    * output        : pLocalCameraStatus 状态
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetLocalCameraStatus(const ELTE_CHAR* pUserID,ELTE_CHAR** pLocalCameraStatus);


    //==============added for ICP start===================================
    /**************************************************************************
    * name          : ELTE_SDK_Unified_Login
    * description   : 统一登录
    * input         : pUserID         调度员账号
                      pPWD            调度员密码
                      pServerIP       udc IP地址
                      pLocalIP        本地主机IP地址
                      uiServerPort udc 服务端口
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Unified_Login(const ELTE_CHAR* pUserID, const ELTE_CHAR* pPWD, const ELTE_CHAR* pServerIP, const ELTE_CHAR* pLocalIP, ELTE_UINT32 uiServerPort);

    /**************************************************************************
    * name          : ELTE_SDK_Unified_Logout
    * description   : 注销
    * input         : pUserID 调度员账号
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Unified_Logout(const ELTE_CHAR* pUserID);

    /**************************************************************************
    * name          : ELTE_SDK_Unified_ModPwd
    * description   : 修改密码
    * input         : pUserID 调度员账号
                      pOldPWD 旧密码
                      pNewPWD 新密码
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Unified_ModPwd(const ELTE_CHAR* pUserID, const ELTE_CHAR* pOldPWD, const ELTE_CHAR* pNewPWD);

    /**************************************************************************
    * name          : ELTE_SDK_HoldCall
    * description   : 呼叫保持/会议保持，type=0为呼叫保持，type=1为会议保持
    * input         : pResourceID 资源ID
                      pHoldCallParam呼叫保持参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_HoldCall(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pHoldCallParam);

    /**************************************************************************
    * name          : ELTE_SDK_UnHoldCall
    * description   : 呼叫去保持/会议去保持，type=0为呼叫去保持，type=1为会议去保持
    * input         : pResourceID 资源ID
                      pHoldCallParam呼叫保持参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_UnHoldCall(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pUnHoldCallParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetDeviceIDRelationship
    * description   : 获取移动设备GBID和ISDN之间的关系
    * input         : pQueryParam 查询条件
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetDeviceIDRelationship(const ELTE_CHAR* pQueryParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_CreateConf
    * description   : 创建会议
    * input         : pResourceID 资源ID
                      pConfCreateConf 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_CreateConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfCreateConf);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_EndConf
    * description   : 结束会议
    * input         : pResourceID 资源ID
                      pConfEndConfParam 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_EndConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfEndConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_JoinConf
    * description   : 主动入会
    * input         : pResourceID 资源ID
                      pConfJoinConfParam  会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_JoinConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfJoinConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AudioAcceptConf
    * description   : 接听语音会议来电
    * input         : pResourceID 资源ID
                      pConfAcceptConfParam 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AudioAcceptConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfAcceptConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AudioRejectConf
    * description   : 拒绝语音会议来电
    * input         : pResourceID 资源ID
                      pConfRejectConf 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AudioRejectConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfRejectConf);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_VideoAcceptConf
    * description   : 接听视频会议来电
    * input         : pResourceID 资源ID
                      pConfAcceptConfParam 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_VideoAcceptConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfAcceptConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_VideoRejectConf
    * description   : 拒绝视频会议来电
    * input         : pResourceID 资源ID
                      pConfRejectConf 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_VideoRejectConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfRejectConf);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AddMember
    * description   : 添加会议成员
    * input         : pResourceID 资源ID
                      pConfAddMembersParam 添加成员参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AddMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfAddMembersParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_SubConf
    * description   : 订阅会议
    * input         : pResourceID 资源ID
                      pConfSubscribeParam 订阅会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    //ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_SubConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfSubscribeParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_UnSubConf
    * description   : 去订阅会议
    * input         : pResourceID 资源ID
                      pConfUnsubscribeParam 去订阅会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    //ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_UnSubConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfUnsubscribeParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_AudioHangup
    * description   : 挂断音频会议
    * input         : pResourceID 资源ID
                      pConfHangupMemberParam 挂断参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_AudioHangup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfHangupParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_VideoHangup
    * description   : 挂断视频会议
    * input         : pResourceID 资源ID
                      pConfHangupMemberParam 挂断参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_VideoHangup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfHangupParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_ReCallMember
    * description   : 重呼成员
    * input         : pResourceID 资源ID
                      pConfReCallMemberParam 重呼成员参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_ReCallMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfReCallMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_ExitConf
    * description   : 离开会议
    * input         : pResourceID 资源ID
                      pConfExitConfParam 会议参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_ExitConf(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfExitConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_MuteMember
    * description   : 会议静音成员
    * input         : pResourceID 资源ID
                      pConfMuteMemberParam 静音参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_MuteMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfMuteMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_Mute
    * description   : 会议静音
    * input         : pResourceID 资源ID
                      pConfMuteConfParam 静音参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_Mute(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfMuteConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_BroadCastPicture
    * description   : 广播与会者
    * input         : pResourceID 资源ID
                      pConfBroadCastPictureParam 广播与会者参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_BroadCastPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfBroadCastPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_CancelBroadCastPicture
    * description   : 取消广播与会者
    * input         : pResourceID 资源ID
                      pConfCancelBroadCastPictureParam 取消广播与会者参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_CancelBroadCastPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfCancelBroadCastPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_WatchMember
    * description   : 选看成员
    * input         : pResourceID 资源ID
                      pConfWatchMemberParam 选看成员参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_WatchMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfWatchMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_WatchMixPicture
    * description   : 选看视频多画面
    * input         : pResourceID 资源ID
                      pConfWatchMixPictureParam 选看视频多画面参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_WatchMixPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfWatchMixPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_BroadcastMixPicture
    * description   : 广播视频多画面
    * input         : pResourceID 资源ID
                      pConfBroadcastMixPictureParam 广播视频多画面参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_BroadcastMixPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfBroadcastMixPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_CancelBroadcastMixPicture
    * description   : 取消广播视频多画面
    * input         : pResourceID 资源ID
                      pConfCancelBroadcastMixPictureParam 取消广播视频多画面参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_CancelBroadcastMixPicture(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfCancelBroadcastMixPictureParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_FreeMode
    * description   : 进入自由模式
    * input         : pResourceID 资源ID
                      pConfFreeModeParam 自由模式参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_FreeMode(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfFreeModeParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_HangupMember
    * description   : 挂断成员
    * input         : pResourceID 资源ID
                      pConfHangupMemberParam 挂断成员参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_HangupMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfHangupMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_DelMember
    * description   : 删除成员
    * input         : pResourceID 资源ID
                      pConfDelMemberParam 删除成员参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_DelMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfDelMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_QueryVWallDisplayMatrixInfo
    * description   : 查询大屏上墙显示模式
    * input         : pResourceID 资源ID
    * output        : N/A，查询结果会上报
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_QueryVWallDisplayMatrixInfo(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_QueryVWallLayers
    * description   : 查询大屏上墙图层
    * input         : pResourceID 资源ID
    * output        : N/A，查询结果会上报
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_QueryVWallLayers(const ELTE_CHAR* pResourceID);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_QueryURLByConfID
    * description   : 查询会议录制信息
    * input         : pResourceID 资源ID
                      pConfQueryURLByConfIDParam 查询参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_QueryURLByConfID(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfQueryURLByConfIDParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_QueryConfAttendee
    * description   : 查询与会者所在的会议信息
    * input         : pResourceID 资源ID
                      pConfQueryConfParam 查询参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_QueryConfAttendee(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfQueryConfParam);

    /**************************************************************************
    * name          : ELTE_SDK_CallSendDTMF
    * description   : 二次拨号
    * input         : pResourceID 资源ID
                      pDtmfParam二次拨号参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_CallSendDTMF(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pDtmfParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetVdmSessions
    * description   : 获取VDM会话资源（VDM心跳使用）
    * input         : N/A
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetVdmSessions();

    /**************************************************************************
    * name          : ELTE_SDK_SetConflictManagerSwitch
    * description   : 设置冲突管理总开关（默认为关）
    * input         : iSwitch 开关参数（0关闭，1打开）
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetConflictManagerSwitch(const ELTE_INT32 iSwitch);

    /**************************************************************************
    * name          : ELTE_SDK_GetIpccIpAndPort
    * description   : 获取IPCC地址
    * input         : N/A
    * output        : pIpccIps IPCC的地址和端口号
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetIpccIpAndPort(ELTE_CHAR** pIpccIps);

    /**************************************************************************
    * name          : ELTE_SDK_GetIpccInfos
    * description   : 获取IPCC信息
    * input         : N/A
    * output        : pIpccInfos IPCC的信息列表
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetIpccInfos(ELTE_CHAR** pIpccInfos);

    /**************************************************************************
    * name          : ELTE_SDK_GetUnifiedToken
    * description   : 获取统一登录Token
    * input         : N/A
    * output        : pToken 统一登录的Token
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetUnifiedToken(ELTE_CHAR** pToken);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_SetSpokenMember
    * description   : 设置发言人
    * input         : pResourceID 资源ID
                      pConfSetSpokenMemberParam 设置发言人参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_SetSpokenMember(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfSetSpokenMemberParam);

    /**************************************************************************
    * name          : ELTE_SDK_GetPDTGroupInfo
    * description   : 查询终端当前通话组信息
    * input         : pResourceID 资源ID
                      pPDTGroupQueryParam 查询参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_GetPDTGroupInfo(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pPDTGroupQueryParam);

    /**************************************************************************
    * name          : ELTE_SDK_Conf_GetContinuousPresenceInfo
    * description   : 查询会议多画面信息
    * input         : pResourceID 资源ID
                      pConfQueryConfParam 查询参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_Conf_GetContinuousPresenceInfo(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pConfQueryParam);
    //==============added for ICP end===================================


    /**************************************************************************
    * name          : ELTE_SDK_RTSPStart
    * description   : rtsp播放录音录像
    * input         : pRTSPStartParam 播放参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPStart(const ELTE_CHAR* pRTSPStartParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPClose
    * description   : 关闭rtsp播放录音录像
    * input         : pRTSPCloseParam 关闭播放参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPClose(const ELTE_CHAR* pRTSPCloseParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPPause
    * description   : 暂停rtsp播放录音录像
    * input         : pRTSPStartParam 暂停播放参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPPause(const ELTE_CHAR* pRTSPPauseParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPPlay
    * description   : rtsp设置播放起始点
    * input         : ELTE_SDK_RTSPPlay 设置播放起始点参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPPlay(const ELTE_CHAR* pRTSPPlayParam);

    /**************************************************************************
    * name          : ELTE_SDK_RTSPRecv
    * description   : rtsp播放视频
    * input         : pResourceID  rtsp sessionId
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_RTSPRecv(const ELTE_CHAR* pResourceID);
    /**************************************************************************
    * name          : ELTE_SDK_SetListenGroup
    * description   : 设置监听组
    * input         : pResourceID 资源ID
                      pSetListenGroupParam 设置监听组参数
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetListenGroup(const ELTE_CHAR* pResourceID, const ELTE_CHAR* pSetListenGroupParam);
    /**************************************************************************
    * name          : ELTE_SDK_SetRing
    * description   : 设置语音点呼振铃iRingType:0----振铃；1-----不振铃
    * input         : iRingType 振铃开关
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetRing(const ELTE_INT32 iRingType);

    /**************************************************************************
    * name          : ELTE_SDK_SetLinkedPhone
    * description   : 绑定话机联动的接口
    * input         : pLinkedPhoneParam 话机联动参数
                        iPhoneISDN为绑定的硬话机号码；iLinkedMode为绑定模式0---绑定，1----解绑定；iAutoanswer为话机联动场景下应答模式，
                        0---为人工应答，1---为自动应答，默认为1自动应答
    * output        : N/A
    * return        : 成功返回0，失败返回错误码
    **************************************************************************/
    ELTE_SDK_API ELTE_INT32 __SDK_CALL ELTE_SDK_SetLinkedPhone(const ELTE_CHAR* pLinkedPhoneParam);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif
