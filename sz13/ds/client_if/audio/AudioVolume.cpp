#include "windows.h"
#include "mmsystem.h"
#include "vector"
#include <dshow.h>

#include "Mmdeviceapi.h"
#include "PolicyConfig.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"
#include <endpointvolume.h>

#include "g_include.h"

#pragma comment(lib, "winmm.lib")

#define EXIT_ON_ERROR(hres)     if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)      if ((punk) != NULL)  { (punk)->Release(); (punk) = NULL; }

IMMDevice* GetAudioBoostLevel(AnsiString strName);

void MicphoneBoost(bool bIsBoost)
{
    int mixerNum ;//总的混音器数量
    HMIXER     hMixer; //混音器设备句柄
//  MMRESULT    mmr;//函数调用返回
    MIXERCAPS   MixerCaps; //混音器设备能力信息
    MIXERLINE   MixerLine;//线路的信息

    //获取当前系统总的混音器数量
    mixerNum= mixerGetNumDevs();
    bool ifFind =false;
    for(int i=0;i<mixerNum;i++)
    {
        //打开混音器，第一个参数是记录混音器的handler,第二个参数是要打开的混音器ID
        mixerOpen(&hMixer, i, 0, 0L, MIXER_OBJECTF_MIXER);

        UINT uMxid;

        ifFind = false;

        //取混音器id，第一个参数是混音器的handler，第二个参数记录混音器的id
        //mmr = mixerGetID((HMIXEROBJ)hMixer,&uMxid,MIXER_OBJECTF_HMIXER);

        //获取混音器能力特征，如声音控制 和录音控制
        mixerGetDevCaps((UINT)hMixer, &MixerCaps, sizeof(MixerCaps));

        for(DWORD j=0;j<MixerCaps.cDestinations;j++)
            //MixerCaps.cDestinations表示此混音器设备的audio line目标的数量,
            //如一个audio line目标为“音量控制”，另一个audio line目标为“录音控制”。
        {
            memset(&MixerLine,0,sizeof(MIXERLINE));
            MixerLine.cbStruct      = sizeof(MixerLine);
            MixerLine.dwDestination = j;
            // 音量控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
            // 录音控制目标line的component类型为MIXERLINE_COMPONENTTYPE_DST_WAVEIN
            //MixerLine.dwComponentType=MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

            //mixerGetLineInfo 第三个参数可以是：MIXER_GETLINEINFOF_COMPONENTTYPE，MIXER_GETLINEINFOF_SOURCE，MIXER_GETLINEINFOF_DESTINATION等
            //取录音控制
            mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine, MIXER_GETLINEINFOF_DESTINATION);
            DWORD dwConnections = MixerLine.cConnections;
            for ( DWORD count = 0; count < dwConnections; count++ )
            {
                MixerLine.dwSource = count;
                mixerGetLineInfo((HMIXEROBJ)hMixer,&MixerLine,MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);
                if (MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == MixerLine.dwComponentType)
                {
                    //如果是录音控制的麦克风选项，则跳出
                    ifFind =true;
                    break;
                }
            }
            if (ifFind)
            {
                break;
            }
        }

        if(!ifFind)
            continue;

        //获取麦克风选项
        MIXERLINECONTROLS   MixerLineControls;
        PMIXERCONTROL       paMixerControls;

        paMixerControls = (PMIXERCONTROL)malloc(sizeof(MIXERCONTROL) * MixerLine.cControls);
        MixerLineControls.cbStruct       = sizeof(MixerLineControls);
        MixerLineControls.dwLineID       = MixerLine.dwLineID;
        MixerLineControls.cControls      = MixerLine.cControls;
        MixerLineControls.cbmxctrl       = sizeof(MIXERCONTROL);
        MixerLineControls.pamxctrl       = paMixerControls;
        mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ALL);

        DWORD u ;
        ifFind = false;
        for ( u = 0; u < MixerLine.cControls; u++)
        {
            String  strName = paMixerControls[u].szName;

            //if (wcscmp(paMixerControls[u].szName, _T("麦克风加强"))==0)
            if (strName == "麦克风加强")
            {
                ifFind =true;
                break;
            }
        }
        if (!ifFind)
        {
            continue;
        }

        MIXERCONTROL    MixerControl;

        MixerLineControls.cbStruct    = sizeof(MixerLineControls);
        MixerLineControls.dwControlID = paMixerControls[u].dwControlID;
        MixerLineControls.cbmxctrl    = sizeof(MixerControl);
        MixerLineControls.pamxctrl    = &MixerControl;

        mixerGetLineControls((HMIXEROBJ)hMixer, &MixerLineControls, MIXER_GETLINECONTROLSF_ONEBYID);
        free(paMixerControls);

        MIXERCONTROLDETAILS                    MixerControlDetails;
        PMIXERCONTROLDETAILS_BOOLEAN     pMixerControlDetails_Boolean;
        pMixerControlDetails_Boolean = (PMIXERCONTROLDETAILS_BOOLEAN)malloc(1 * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

        memset(&MixerControlDetails,0,sizeof(MixerControlDetails));
        MixerControlDetails.cbStruct       = sizeof(MixerControlDetails);
        MixerControlDetails.dwControlID    = MixerControl.dwControlID;
        MixerControlDetails.cChannels      =1;
        MixerControlDetails.cMultipleItems =0;
        MixerControlDetails.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        MixerControlDetails.paDetails      = &pMixerControlDetails_Boolean[0];
        // True to turn on boost, False to turn off
        pMixerControlDetails_Boolean[0].fValue = bIsBoost;

        mixerSetControlDetails((HMIXEROBJ)hMixer, &MixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);

        free(pMixerControlDetails_Boolean);

        //关闭混音器
        mixerClose(hMixer);
        if (bIsBoost)
        {
         // MicphoneMut(true);
        }
    }
    return ;
}

VOID GetMicList(std::vector<CHAR *> &vMicList)
{
 // BOOL bResult = FALSE;
    int mixerNum = (int)mixerGetNumDevs();
 // DWORD dwLineID = (DWORD)-1;
    HMIXER  hMixer = NULL;

    for(int i=0; i<mixerNum; i++)
    {
        // DWORD    dwSource = 0;

        hMixer = NULL;

        //打开Mixer设备
        if (mixerOpen(&hMixer, i, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
        {
            continue;
        }

        MIXERLINE MixerLine;

        MixerLine.cbStruct = sizeof(MixerLine);
        MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        if (mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine,
            MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
        {
            mixerClose(hMixer);
            continue;
        }

        DWORD   dwConnections = MixerLine.cConnections;
        BOOL    bExist = FALSE;
        CHAR    *szMicName = strdup(MixerLine.Target.szPname);

        for(DWORD j=0; j<dwConnections; j++ )
        {
            //枚举每一个设备，当Source的ID等于当前的迭代记数
            MixerLine.dwSource = j;
            if(mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine,
                MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
            {
                break;
            }

            //如果当前设备类型是麦克风，则跳出循环。
            if (MixerLine.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE )
            {
                bExist = TRUE;
                vMicList.push_back(szMicName);
                break;
            }
        }

        if (!bExist)
        {
            free(szMicName);
        }

        mixerClose(hMixer);
    }
}

BOOL GetMicVolume(AnsiString strMicName, unsigned int *pulVolume)
{
    BOOL bResult = FALSE;
    int mixerNum = (int)mixerGetNumDevs();
    DWORD dwLineID = (DWORD)-1;
    HMIXER  hMixer = NULL;

    *pulVolume = 0;

    for(int i=0; i<mixerNum; i++)
    {
 //     DWORD   dwSource = 0;

        hMixer = NULL;
//          MMRESULT mr = NULL;

        //打开Mixer设备
        if(mixerOpen(&hMixer, i, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
        {
            continue;
        }

        MIXERLINE MixerLine;

        MixerLine.cbStruct = sizeof(MixerLine);
        MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        if (mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine,
            MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
        {
            mixerClose(hMixer);
            continue;
        }

        if (strcmp(MixerLine.Target.szPname, strMicName.c_str()) != 0)
        {
            mixerClose(hMixer);
            continue;
        }

        DWORD dwConnections = MixerLine.cConnections;

        for(DWORD j=0; j<dwConnections; j++ )
        {
            //枚举每一个设备，当Source的ID等于当前的迭代记数
            MixerLine.dwSource = j;
            if(mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine,
                MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
            {
                break;
            }

            //如果当前设备类型是麦克风，则跳出循环。
            if (MixerLine.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE )
            {
                dwLineID = MixerLine.dwLineID;
              //    dwSource = MixerLine.dwSource;
                break;
            }
        }

        //如果没有找到，返回失败。
        if (dwLineID == (DWORD)-1)
        {
            mixerClose(hMixer);
            return FALSE;
        }

        break;
    }

    MIXERCONTROL mxc;
    MIXERLINECONTROLS mxlc;

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
    MIXERCONTROLDETAILS mxcd;

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Get = {0};
    MIXERCONTROLDETAILS mxcd_Set;

    mxlc.cbStruct = sizeof(mxlc);
    mxlc.dwLineID = dwLineID; //lineinfo_Connector.dwLineID;
    mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(mxc);
    mxlc.pamxctrl = &mxc;
    if (mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(hMixer), &mxlc,
        MIXER_OBJECTF_HMIXER|MIXER_GETLINECONTROLSF_ONEBYTYPE)!= MMSYSERR_NOERROR)
    {
        goto end;
    }

    //get volume value range now
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.dwControlID = mxc.dwControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;

    if (mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(hMixer), &mxcd, MIXER_GETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
    {
        goto end;
    }

    mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd_Set.dwControlID = mxc.dwControlID;
    mxcd_Set.cChannels = 1;
    mxcd_Set.cMultipleItems = 0;
    mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd_Set.paDetails = &mxcdVolume_Get;
    if (mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(hMixer), &mxcd_Set,
        MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
    {
        goto end;
    }

    *pulVolume = mxcdVolume_Get.dwValue*100/mxc.Bounds.dwMaximum;

    bResult = TRUE;

end:
    mixerClose(hMixer);
    return bResult;
}

BOOL SetMicVolume(AnsiString strMicName, unsigned int ulVolume)
{
    BOOL bResult = FALSE;
    int mixerNum = (int)mixerGetNumDevs();
    DWORD dwLineID = (DWORD)-1;
    HMIXER  hMixer = NULL;

    for(int i=0; i<mixerNum; i++)
    {
//      DWORD   dwSource = 0;

        hMixer = NULL;
//          MMRESULT mr = NULL;

        //打开Mixer设备
        if(mixerOpen(&hMixer, i, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
        {
            continue;
        }

        MIXERLINE MixerLine;

        MixerLine.cbStruct = sizeof(MixerLine);
        MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        if (mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine,
            MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
        {
            mixerClose(hMixer);
            continue;
        }

        char    *szTmp = MixerLine.Target.szPname;

        if (strcmp(szTmp, strMicName.c_str()) != 0)
        {
            mixerClose(hMixer);
            continue;
        }

        DWORD dwConnections = MixerLine.cConnections;

        for(DWORD j=0; j<dwConnections; j++ )
        {
            //枚举每一个设备，当Source的ID等于当前的迭代记数
            MixerLine.dwSource = j;
            if(mixerGetLineInfo((HMIXEROBJ)hMixer, &MixerLine,
                MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
            {
                break;
            }

            //如果当前设备类型是麦克风，则跳出循环。
            if (MixerLine.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE )
            {
                dwLineID = MixerLine.dwLineID;
            //  dwSource = MixerLine.dwSource;
                break;
            }
        }

        //如果没有找到，返回失败。
        if (dwLineID == (DWORD)-1)
        {
            mixerClose(hMixer);
            return FALSE;
        }

        break;
    }

            /*
            //get line id of destination line
            MIXERLINE lineinfo_Dest;
            ::ZeroMemory(&lineinfo_Dest, sizeof(MIXERLINE));
            lineinfo_Dest.cbStruct = sizeof(MIXERLINE);
            lineinfo_Dest.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
            if(::mixerGetLineInfo(
                (HMIXEROBJ)hMixer,
                &lineinfo_Dest,
                MIXER_OBJECTF_HMIXER   |
                MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
                continue;
            //get id of specified connector
            MIXERLINE lineinfo_Connector;
            ::ZeroMemory(&lineinfo_Connector, sizeof(MIXERLINE));
            lineinfo_Connector.cbStruct = sizeof(MIXERLINE);
            lineinfo_Connector.dwDestination = lineinfo_Dest.dwLineID;
            lineinfo_Connector.dwSource = dwSource;
            if(::mixerGetLineInfo(
                (HMIXEROBJ)hMixer,
                &lineinfo_Connector,
                MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
                continue;
            */

    MIXERCONTROL mxc;
    MIXERLINECONTROLS mxlc;
    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
    MIXERCONTROLDETAILS mxcd;
    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set;
    MIXERCONTROLDETAILS mxcd_Set;

    mxlc.cbStruct = sizeof(mxlc);
    mxlc.dwLineID = dwLineID; //lineinfo_Connector.dwLineID;
    mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(mxc);
    mxlc.pamxctrl = &mxc;
    if (mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(hMixer), &mxlc,
        MIXER_OBJECTF_HMIXER|MIXER_GETLINECONTROLSF_ONEBYTYPE)!= MMSYSERR_NOERROR)
    {
        goto end;
    }

    //get volume value range now
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.dwControlID = mxc.dwControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;

    if (mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(hMixer), &mxcd, MIXER_GETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
    {
        goto end;
    }

    //set volume
    mxcdVolume_Set.dwValue =mxc.Bounds.dwMaximum * ulVolume / 100;

    mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd_Set.dwControlID = mxc.dwControlID;
    mxcd_Set.cChannels = 1;
    mxcd_Set.cMultipleItems = 0;
    mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd_Set.paDetails = &mxcdVolume_Set;
    if (mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(hMixer), &mxcd_Set,
        MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
    {
        goto end;
    }

    bResult = TRUE;

end:
    mixerClose(hMixer);
    return bResult;
}

BOOL GetMicBoostLevel(IMMDevice *pEndptDev, IAudioVolumeLevel** ppVolumeLevel)
{
    HRESULT hr = S_OK;
    DataFlow flow;
    IDeviceTopology *pDeviceTopology = NULL;
    IConnector *pConnFrom = NULL;
    IConnector *pConnTo = NULL;
    IPart *pPartPrev = NULL;
    IPart *pPartNext = NULL;
    *ppVolumeLevel = NULL;
    wchar_t microphoneBoostName[] = L"麦克风加强";//if your system language is English,the name is "microphone boost"

    if (pEndptDev == NULL)
    {
        return FALSE;
    }
    // Get the endpoint device's IDeviceTopology interface.
    hr = pEndptDev->Activate(
        __uuidof(IDeviceTopology), CLSCTX_ALL, NULL,
        (void**)&pDeviceTopology);
    EXIT_ON_ERROR(hr);
        // The device topology for an endpoint device always
        // contains just one connector (connector number 0).
    hr = pDeviceTopology->GetConnector(0, &pConnFrom);
    SAFE_RELEASE(pDeviceTopology)
    EXIT_ON_ERROR(hr);
        // Make sure that this is a capture device.
    hr = pConnFrom->GetDataFlow(&flow);
    EXIT_ON_ERROR(hr);

    if (flow != Out)
    {
        // Error -- this is a rendering device.
        return FALSE;
    }

    // Outer loop: Each iteration traverses the data path
    // through a device topology starting at the input
    // connector and ending at the output connector.
    while (TRUE)
    {
        BOOL bConnected;
        hr = pConnFrom->IsConnected(&bConnected);
        EXIT_ON_ERROR(hr);

        // Does this connector connect to another device?
        if (bConnected == FALSE)
        {
            // This is the end of the data path that
            // stretches from the endpoint device to the
            // system bus or external bus. Verify that
            // the connection type is Software_IO.
            ConnectorType  connType;
            hr = pConnFrom->GetType(&connType);
            EXIT_ON_ERROR(hr);
            if (connType == Software_IO)
            {
                break;  // finished
            }
            EXIT_ON_ERROR(hr = E_FAIL)
        }

        // Get the connector in the next device topology,
        // which lies on the other side of the connection.
        hr = pConnFrom->GetConnectedTo(&pConnTo);
        EXIT_ON_ERROR(hr);
        SAFE_RELEASE(pConnFrom)
        // Get the connector's IPart interface.
        hr = pConnTo->QueryInterface(
        __uuidof(IPart), (void**)&pPartPrev);

        EXIT_ON_ERROR(hr);
        SAFE_RELEASE(pConnTo)
        // Inner loop: Each iteration traverses one link in a
        // device topology and looks for input multiplexers.
        while (TRUE)
        {
            PartType parttype;
            IPartsList *pParts;
            // Follow downstream link to next part.
            hr = pPartPrev->EnumPartsOutgoing(&pParts);
            EXIT_ON_ERROR(hr);

            hr = pParts->GetPart(0, &pPartNext);
            pParts->Release();
            EXIT_ON_ERROR(hr);

            hr = pPartNext->GetPartType(&parttype);
            EXIT_ON_ERROR(hr);

            LPWSTR pName;
            if (SUCCEEDED(pPartNext->GetName(&pName)))
            {
                // Failure of the following call means only that
                // the part is not a boost (micrphone boost).
                if (wcscmp(microphoneBoostName,pName) == 0)
                {
                    //get IAudioVolumeLevel to control volume
                    hr = pPartNext->Activate(CLSCTX_ALL, __uuidof(IAudioVolumeLevel), (void**)ppVolumeLevel);
                    goto Exit;
                }
                CoTaskMemFree(pName);
            }

            GUID subType;
            pPartNext->GetSubType(&subType);
            if (parttype == Connector)
            {
                // We've reached the output connector that
                // lies at the end of this device topology.
                hr = pPartNext->QueryInterface(
                    __uuidof(IConnector),
                    (void**)&pConnFrom);
                EXIT_ON_ERROR(hr);
                SAFE_RELEASE(pPartPrev)
                SAFE_RELEASE(pPartNext)
                break;
            }

            SAFE_RELEASE(pPartPrev)

            pPartPrev = pPartNext;
            pPartNext = NULL;
        }
    }
Exit:
    SAFE_RELEASE(pConnFrom)
    SAFE_RELEASE(pConnTo)
    SAFE_RELEASE(pPartPrev)
    SAFE_RELEASE(pPartNext)

    return hr;
}

int GetDefaultMicBoostVolume()
{
    HRESULT hr = NULL;
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    IMMDevice *defaultDevice = NULL;
    IAudioVolumeLevel* pIaudioVolumeLevel;

    CoInitialize(NULL);
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);

    if (hr != S_OK)
    {
        return 0;
    }

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;

    GetMicBoostLevel(defaultDevice, &pIaudioVolumeLevel);
    defaultDevice->Release();

    if (pIaudioVolumeLevel == NULL)
    {
        return 0;
    }

    float fLevelDB = 0;

    pIaudioVolumeLevel->GetLevel(0, &fLevelDB);//0-30

    return fLevelDB;
}

int GetMicBoostLevel(AnsiString strName)
{
    IMMDevice *pDevice = GetAudioBoostLevel(strName);
    IAudioVolumeLevel* pIaudioVolumeLevel;

    CoInitialize(NULL);

    GetMicBoostLevel(pDevice, &pIaudioVolumeLevel);
    pDevice->Release();

    if (pIaudioVolumeLevel == NULL)
    {
        return 0;
    }

    float fLevelDB = 0;

    pIaudioVolumeLevel->GetLevel(0, &fLevelDB);//0-30

    return fLevelDB;
}

bool SetMicBoostLevel(AnsiString strName, double dLevel)
{
    IMMDevice *pDevice = GetAudioBoostLevel(strName);
    IAudioVolumeLevel* pIaudioVolumeLevel;

    CoInitialize(NULL);

    GetMicBoostLevel(pDevice, &pIaudioVolumeLevel);
    pDevice->Release();

    if (pIaudioVolumeLevel == NULL)
    {
        return false;
    }

    pIaudioVolumeLevel->SetLevel(0, dLevel, NULL);

    return true;
}

BOOL GetAudioInputDevice(REFGUID guidValue, std::vector<AnsiString> &vDevList)
{
    AnsiString  name;
    HRESULT hr = NULL;
    static BOOL bInit = FALSE;

    vDevList.clear();

    // 初始化COM
    if (!bInit)
    {
        hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (FAILED(hr))
        {
            return FALSE;
        }

        bInit = TRUE;
    }

    // 创建系统设备枚举器实例
    ICreateDevEnum *pSysDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
    if (FAILED(hr))
    {
//      CoUninitialize();
        return FALSE;
    }

    // 获取设备类枚举器
    IEnumMoniker *pEnumCat = NULL;
    hr = pSysDevEnum->CreateClassEnumerator(guidValue, &pEnumCat, 0);
    if (hr == S_OK)
    {
        // 枚举设备名称
        IMoniker *pMoniker = NULL;
        ULONG cFetched;
        while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
        {
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **)&pPropBag);
            if (SUCCEEDED(hr))
            {
                // 获取设备友好名
                VARIANT varName;
                VariantInit(&varName);

                hr = pPropBag->Read(L"FriendlyName", &varName, NULL);
                if (SUCCEEDED(hr))
                {
                    name = String(varName.bstrVal);

                    vDevList.push_back(name);
             /*
                    LPOLESTR pOleDisplayName = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(256 * 2));
                    if (pOleDisplayName != NULL)
                    {
                        hr = pMoniker->GetDisplayName(NULL, NULL, &pOleDisplayName);
                        if (SUCCEEDED(hr))
                        {
                            char   MonikerName[256];

                            name = String(pOleDisplayName);

                         // StringCchCopy(MonikerName, 256, pOleDisplayName);
                        }

                        CoTaskMemFree(pOleDisplayName);
                    }  */
                }

                VariantClear(&varName);
                pPropBag->Release();
            }

            pMoniker->Release();
        } // End for While

        pEnumCat->Release();
    }

    pSysDevEnum->Release();
//  CoUninitialize();

    return TRUE;
}

/*
BOOL GetSpeakerList(std::vector<AnsiString> &vList)
{
    return GetAudioInputDevice(CLSID_AudioRendererCategory, vList);
}

BOOL GetMicList(std::vector<AnsiString> &vList)
{
    return GetAudioInputDevice(CLSID_AudioInputDeviceCategory, vList);
}
*/

static GUID IDevice_FriendlyName = { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } };

IMMDevice* GetAudioBoostLevel(AnsiString strName)
{
    IMMDeviceEnumerator* pEnumerator;
    IMMDeviceCollection *pCollection = NULL;
    IMMDevice           *pDevice=NULL;
    IMMDevice           *pOutDev=NULL;
    IPropertyStore      *pProperties=NULL;
    UINT32              deviceCount = 0;
    static BOOL         bInit = FALSE;

    if (!bInit)
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        bInit = TRUE;
    }

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
    if (FAILED(hr))
    {
        goto end;
    }

    hr = pEnumerator->EnumAudioEndpoints( eAll/*eCapture eRender*/, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr))
    {
        goto end;
    }

    hr = pCollection->GetCount(&deviceCount);
    if (FAILED(hr))
    {
        goto end;
    }

    for (UINT32 dev=0;dev<deviceCount;dev++)
    {
        pDevice = NULL;
        hr = pCollection->Item(dev,&pDevice);

        //枚举设备号和名字
        if (hr == S_OK)
        {
            hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
            if (hr == S_OK)
            {
                PROPVARIANT    varName;
                PROPERTYKEY key;

                PropVariantInit(&varName);
                key.pid = 14;
                key.fmtid = IDevice_FriendlyName;

                hr = pProperties->GetValue(key, &varName);
                //hr = pProperties->GetValue(PKEY_Device_FriendlyName, &varName);

                wchar_t     *devid;

                pDevice->GetId(&devid);

                AnsiString  strDevID = (String)devid;

                if (hr == S_OK)
                {
                    AnsiString  strDevName = (String)varName.pwszVal;

                    if (strDevName == strName)
                    {
                        pOutDev = pDevice;
                    }
                }

                PropVariantClear(&varName);
            }

            SAFE_RELEASE(pProperties);
        }
    }

    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    return pOutDev;

end:
    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    return NULL;
}

BOOL GetAudioDev(std::map<AnsiString, AnsiString> &mDev)
{
    IMMDeviceEnumerator* pEnumerator;
    IMMDeviceCollection *pCollection = NULL;
    IMMDevice           *pDevice=NULL;
    IPropertyStore      *pProperties=NULL;
    UINT32              deviceCount = 0;
    static BOOL         bInit = FALSE;

    if (!bInit)
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        bInit = TRUE;
    }

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
    if (FAILED(hr))
    {
        goto end;
    }

    hr = pEnumerator->EnumAudioEndpoints( eAll/*eCapture eRender*/, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr))
    {
        goto end;
    }

    hr = pCollection->GetCount(&deviceCount);
    if (FAILED(hr))
    {
        goto end;
    }

    for (UINT32 dev=0;dev<deviceCount;dev++)
    {
        pDevice = NULL;
        hr = pCollection->Item(dev,&pDevice);

        //枚举设备号和名字
        if (hr == S_OK)
        {
            hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
            if (hr == S_OK)
            {
                PROPVARIANT    varName;
                PROPERTYKEY key;

                PropVariantInit(&varName);
                key.pid = 14;
                key.fmtid = IDevice_FriendlyName;

                hr = pProperties->GetValue(key, &varName);

                wchar_t     *devid;

                pDevice->GetId(&devid);

                AnsiString  strDevID = (String)devid;

                if (hr == S_OK)
                {
                    AnsiString  strDevName = (String)varName.pwszVal;

                    mDev[strDevName] = strDevID;
                }

                PropVariantClear(&varName);
            }

            SAFE_RELEASE(pProperties);
        }
    }

    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    return TRUE;

end:
    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    return FALSE;
}

BOOL GetAudioDev(std::vector<AnsiString> &vDev)
{
    std::map<AnsiString, AnsiString>    mDev;
    std::map<AnsiString, AnsiString>::iterator it;

    vDev.clear();

    if (!GetAudioDev(mDev))
    {
        return FALSE;
    }

    for (it=mDev.begin(); it!=mDev.end(); it++)
    {
        vDev.push_back(it->first);
    }

    return TRUE;
}

static BOOL GetSetAudioVolume(AnsiString strDev, BOOL bGet, UINT32 &ulVolume)
{
//  char     str[4000];
    IMMDeviceEnumerator* pEnumerator;
    IMMDeviceCollection *pCollection = NULL;
    IMMDevice           *pDevice=NULL;
    IMMDevice           *pDeviceSet = NULL;
    IPropertyStore      *pProperties=NULL;
    UINT32              deviceCount = 0;
    static BOOL         bInit = FALSE;

    if (!bInit)
    {
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
        bInit = TRUE;
    }

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),(void**)&pEnumerator);
    if (FAILED(hr))
    {
        goto end;
    }

    hr = pEnumerator->EnumAudioEndpoints( eAll/*eCapture eRender*/, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr))
    {
        goto end;
    }

    hr = pCollection->GetCount(&deviceCount);
    if (FAILED(hr))
    {
        goto end;
    }

    for (UINT32 dev=0;dev<deviceCount;dev++)
    {
        pDevice = NULL;
        hr = pCollection->Item(dev,&pDevice);

        //枚举设备号和名字
        if (hr == S_OK)
        {
            hr = pDevice->OpenPropertyStore(STGM_READ,&pProperties);
            if (hr == S_OK)
            {
                PROPVARIANT     varName;
                PROPERTYKEY     key;

                PropVariantInit(&varName);
                key.pid = 14;
                key.fmtid = IDevice_FriendlyName;

                hr = pProperties->GetValue(key, &varName);

                if (hr == S_OK)
                {
                    AnsiString  strDevName = (String)varName.pwszVal;

                    if (strDevName == strDev)
                    {
                        pDeviceSet = pDevice;
                    }
                }

                PropVariantClear(&varName);
            }

            SAFE_RELEASE(pProperties);
        }

        if (pDeviceSet)
        {
            break;
        }
    }

    if (pDeviceSet)
    {
        IAudioEndpointVolume *endpointVolume = NULL;

        REFIID IID_IAudioEndpointVolume = __uuidof(IAudioEndpointVolume);
        hr = pDeviceSet->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void**)&endpointVolume);
        if (FAILED(hr))
        {
            goto end;
        }

        float level = 0.0f;

        if (bGet)
        {
            hr = endpointVolume->GetMasterVolumeLevelScalar(&level);
            if (FAILED(hr))
            {
                goto end;
            }

            ulVolume = (UINT32)(100*level + 0.5);
        }
        else
        {
            level = ulVolume;
            level /= 100;
            hr = endpointVolume->SetMasterVolumeLevelScalar(level, NULL);
        }
    }

    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    return TRUE;

end:
    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pCollection);
    return FALSE;
}

BOOL GetAudioVolume(AnsiString strDev, UINT32 &ulVolume)
{
    return GetSetAudioVolume(strDev, TRUE, ulVolume);
}

BOOL SetAudioVolume(AnsiString strDev, UINT32 ulVolume)
{
    return GetSetAudioVolume(strDev, FALSE, ulVolume);
}

/*
VOID SetAudioVolume(AnsiString strDevID)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER, IID_IMMDeviceEnumerator, (void**)&m_pIMMEnumerator);

    if (FAILED(hr))
    {
        goto end;
    }

    CComPtr<IMMDevice> pIMMDeivce = NULL;
    //strEndPointID为string类型的设备ID
    if (strDevID.IsEmpty())
    {
        hr = m_pIMMEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pIMMDeivce);
    }
    else
    {
        hr = m_pIMMEnumerator->GetDevice(strDevID.c_str(), &pIMMDeivce);
    }

    if (FAILED(hr))
    {
        goto end;
    }

    hr = pIMMDeivce->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void**)&m_pMicEndptVol);
    if (FAILED(hr))
    {
        goto end;
    }

    //获取麦克风音量
    float level = 0.0f;
    hr = m_pMicEndptVol->GetMasterVolumeLevelScalar(&level);

    //设置麦克风音量
    float level = 0.5f;
    hr = m_pMicEndptVol->SetMasterVolumeLevelScalar(level, NULL);

    //设置系统麦克风静音
    BOOL bMute = TRUE;
    hr = m_pRenderSimpleVol->SetMute(bMute, NULL);

    //获取系统麦克风静音状态
    BOOL bMute = FALSE;
    hr = m_pRenderSimpleVol->GetMute(&bMute);
}  */

AnsiString GetAudioDevID(AnsiString strDev)
{
    std::map<AnsiString, AnsiString>    mDev;

    if (!GetAudioDev(mDev))
    {
        return "";
    }

    if (mDev.find(strDev) == mDev.end())
    {
        return "";
    }

    return mDev[strDev];
}

BOOL SetDefaultSpeaker(AnsiString strDev)
{
    IPolicyConfigVista  *pPolicyConfig;
    ERole               reserved = eConsole;
    String              strDevID = GetAudioDevID(strDev);

    if (strDevID.IsEmpty())
    {
        return FALSE;
    }

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
        NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);

    if (FAILED(hr))
    {
        return FALSE;
    }

    hr = pPolicyConfig->SetDefaultEndpoint(strDevID.c_str(), reserved);
    pPolicyConfig->Release();
    if (FAILED(hr))
    {
        return FALSE;
    }

    return TRUE;
}

#if 1
void test_mic()
{
    std::vector<CHAR *> vMicList;

    GetMicList(vMicList);

    for (UINT32 i=0; i<vMicList.size(); i++)
    {
        printf("MIC = %s\n", vMicList[i]);
    }

    CHAR    *szMic = vMicList[0];
    unsigned int ulVolume = 0;

    GetMicVolume(szMic, &ulVolume);
    printf("MIC Volume = %u\n", ulVolume);

    for (int i=64; i>0; i/=2)
    {
        ulVolume = i;
        SetMicVolume(szMic, ulVolume);
        GetMicVolume(szMic, &ulVolume);
        printf("MIC Volume = %u\n", ulVolume);

        Sleep(3000);
    }

    /*
    GetMicVolume(szMic, &ulVolume);
    printf("MIC Volume = %u\n", ulVolume);

    SetMicVolume(szMic, 10);
    GetMicVolume(szMic, &ulVolume);
    printf("MIC Volume = %u\n", ulVolume);
    */
}

#endif
