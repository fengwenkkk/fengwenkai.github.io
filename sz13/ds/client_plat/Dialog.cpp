//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Dialog.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDialogForm *DialogForm;
//---------------------------------------------------------------------------
__fastcall TDialogForm::TDialogForm(TComponent* Owner)
    : TSubForm(Owner)
{
    Icon->Width = 0;
    BorderStyle = bsDialog;
}
//---------------------------------------------------------------------------
