//---------------------------------------------------------------------------

#ifndef DialogH
#define DialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "MDIForm.h"
//---------------------------------------------------------------------------
class TDialogForm : public TSubForm
{
__published:    // IDE-managed Components
    TPanel *MDIFormMainPanel;
    TPanel *PanelButton;

private:    // User declarations
public:     // User declarations
    __fastcall TDialogForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogForm *DialogForm;
//---------------------------------------------------------------------------
#endif
