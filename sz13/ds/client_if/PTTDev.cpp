#include <windows.h>
#include <MMSystem.h>
#include "PTTDev.h"

#include "g_include.h"

#pragma comment(lib, "Winmm.lib")

#define JS_BUTTON_OFF           0
#define JS_BUTTON_ON            1

#define JS_BUTTON_UNKNOWN       0xffffffff

BOOL GetJoyStickButtonStatus(UINT32 ulJoyStickID, UINT32 *pulStatus)
{
    UINT32  ulJoyNums = joyGetNumDevs();

    if (ulJoyNums == 0)
    {
        return FALSE;
    }

    JOYINFOEX   stJoyInfo;

    stJoyInfo.dwSize = sizeof(JOYINFOEX);
    stJoyInfo.dwFlags = JOY_RETURNALL;

    MMRESULT    ret = joyGetPosEx(JOYSTICKID1, &stJoyInfo);

    if (ret != JOYERR_NOERROR)
    {
        return FALSE;
    }

    if (stJoyInfo.dwButtons == 0)
    {
        *pulStatus = JS_BUTTON_OFF;
        return TRUE;
    }
    else if (stJoyInfo.dwButtons == 1)
    {
        *pulStatus = JS_BUTTON_ON;
        return TRUE;
    }
    else if (stJoyInfo.dwButtons == 2)
    {
        *pulStatus = JS_BUTTON_ON;
        return TRUE;
    }
    else if (stJoyInfo.dwButtons == 3)
    {
        *pulStatus = JS_BUTTON_ON;
        return TRUE;
    }

    *pulStatus = JS_BUTTON_UNKNOWN;

    return FALSE;
}

BOOL IsPTTDevDown()
{
    UINT32  ulStatus = JS_BUTTON_OFF;

    if (GetJoyStickButtonStatus(JOYSTICKID1, &ulStatus))
    {
        if (ulStatus == JS_BUTTON_ON)
        {
            return TRUE;
        }
    }

    return FALSE;
}

