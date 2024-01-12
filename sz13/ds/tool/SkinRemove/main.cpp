#include <vcl.h>
#pragma hdrstop

#include "g_include.h"
#include "ds_public.h"
#include "MsgDialog.h"
#include "RemoteMsg.h"

#include "ClientUtil.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cxButtons"
#pragma link "cxGraphics"
#pragma link "cxLookAndFeelPainters"
#pragma link "cxLookAndFeels"
#pragma link "dxSkinLilian"

#pragma link "dxSkinsCore"
#pragma link "dxSkinsDefaultPainters"
#pragma link "dxSkinsForm"

#pragma link "cxContainer"
#pragma link "cxControls"
#pragma link "cxDropDownEdit"
#pragma link "cxEdit"
#pragma link "cxMaskEdit"
#pragma link "cxTextEdit"
#pragma link "cxCustomData"
#pragma link "cxInplaceContainer"
#pragma link "cxStyles"
#pragma link "cxTL"
#pragma link "cxTLdxBarBuiltInMenu"
#pragma link "cxPC"
#pragma link "dxSkinscxPCPainter"
#pragma link "cxGroupBox"
#pragma link "cxMemo"
#pragma link "cxLabel"
#pragma link "cxClasses"

#pragma link "dxBarBuiltInMenu"
#pragma link "cxRadioGroup"
#pragma link "cxProgressBar"

#pragma link "cxButtonEdit"
#pragma link "cxImageList"
#pragma resource "*.dfm"

TFormMain *FormMain;

VECTOR<AnsiString>      g_vFile;
VECTOR<AnsiString>      g_vExpSkin;

AnsiString      g_strSkinStart = "#pragma link \"dxSkin";

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TSubForm(Owner)
{
    if (FileExists("exp_skin.ini"))
    {
        mmExpSkin->Lines->LoadFromFile("exp_skin.ini");
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnStartClick(TObject *Sender)
{
    g_strSkinStart = g_strSkinStart.LowerCase();

    g_vFile.clear();

    g_vExpSkin.clear();
    g_vExpSkin.push_back("#pragma link \"dxSkinsCore");
    g_vExpSkin.push_back("#pragma link \"dxSkinsDefaultPainters");
    g_vExpSkin.push_back("#pragma link \"dxSkinsForm");
    g_vExpSkin.push_back("#pragma link \"dxSkinscxPCPainter");

    for (UINT32 i=0; i<mmExpSkin->Lines->Count; i++)
    {
        if (!mmExpSkin->Lines->Strings[i].IsEmpty())
        {
            g_vExpSkin.push_back(g_strSkinStart + mmExpSkin->Lines->Strings[i].Trim());
        }
    }

    for (UINT32 i=0; i<g_vExpSkin.size(); i++)
    {
        g_vExpSkin[i] = g_vExpSkin[i].LowerCase();
    }

    AnsiString  strDir = edtProject->Text.Trim();

    char    acDir[256];

    StrCpy(acDir, strDir);
    gos_format_path(acDir);

    char    *szEnd = gos_right_strchr(acDir, '\\');

    if (!szEnd)
    {
        return;
    }

    *szEnd = '\0';

    GetCppFile(acDir);

    pbMain->Properties->Max = g_vFile.size();

    for (UINT32 i=0; i<g_vFile.size(); i++)
    {
        RemoveSkin(g_vFile[i].c_str());
        pbMain->Position = i+1;

        pbMain->Invalidate();
        pbMain->Update();

        Update();
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::RemoveSkin(CHAR *szFile)
{
    bool    bSkin;
    bool    bNeedSave = false;

    mmFile->Lines->BeginUpdate();
    mmFile->Lines->LoadFromFile(szFile);

    for (UINT32 i=0; i<mmFile->Lines->Count; )
    {
        if (mmFile->Lines->Strings[i].IsEmpty() )
        {
            i++;
            continue;
        }

        if (mmFile->Lines->Strings[i][1] != ' '  &&
            mmFile->Lines->Strings[i][1] != '\t' &&
            mmFile->Lines->Strings[i][1] != '#'  )
        {
            i++;
            continue;
        }

        AnsiString strLine = mmFile->Lines->Strings[i].LowerCase();

        bSkin = gos_str_like(strLine.c_str(), g_strSkinStart.c_str());
        if (bSkin)
        {
            for (UINT32 j=0; j<g_vExpSkin.size(); j++)
            {
                if (gos_str_like(strLine.c_str(), g_vExpSkin[j].c_str()))
                {
                    bSkin = false;
                    break;
                }
            }
        }

        if (bSkin)
        {
            bNeedSave = true;
            mmFile->Lines->Delete(i);
        }
        else
        {
            i++;
        }
    }

    mmFile->Lines->EndUpdate();

    if (bNeedSave)
    {
        mmFile->Lines->SaveToFile(szFile);
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::GetCppFile(CHAR *szDir)
{
    HANDLE      hDir;
    CHAR        acFile[256];
    BOOL        bIsDir;
    CHAR        *szFilePostfix;
 /* AnsiString  strExpFile = edtProject->Text.Trim();

    strExpFile = strExpFile.SubString(1, strExpFile.Length() - 6);
    strExpFile += "cpp";    */

    hDir = gos_open_dir(szDir);
    if (!hDir)
    {
        return;
    }

    while (gos_get_next_file(hDir, acFile, &bIsDir))
    {
        if (bIsDir)
        {
            GetCppFile(acFile);
            continue;
        }

        szFilePostfix = gos_get_file_postfix(acFile);
        if (szFilePostfix && strcasecmp(szFilePostfix, "dfm") == 0)
        {
            strcpy(acFile+strlen(acFile)-3, "cpp");
            g_vFile.push_back(acFile);
            continue;
        }
    }

    gos_close_dir(hDir);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::btnExitClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::edtProjectClick(TObject *Sender)
{
    if (!OpenDialog->Execute())
    {
        return;
    }

    edtProject->Text = OpenDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::edtProjectPropertiesChange(TObject *Sender)
{
    btnStart->Enabled = FileExists(edtProject->Text);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    mmExpSkin->Lines->SaveToFile("exp_skin.ini");
}
//---------------------------------------------------------------------------

