inherited FormTextReader: TFormTextReader
  BorderIcons = [biSystemMenu]
  ClientHeight = 538
  ClientWidth = 875
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  ExplicitWidth = 891
  ExplicitHeight = 577
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 875
    Height = 538
    ExplicitWidth = 875
    ExplicitHeight = 538
    object gbCtrl: TcxGroupBox
      Left = 0
      Top = 486
      Align = alBottom
      Caption = ' '
      Ctl3D = False
      PanelStyle.Active = True
      ParentCtl3D = False
      Style.BorderColor = clRed
      Style.BorderStyle = ebs3D
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      DesignSize = (
        875
        52)
      Height = 52
      Width = 875
      object btnClose: TcxButton
        Left = 750
        Top = 12
        Width = 75
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = #20851#38381
        LookAndFeel.NativeStyle = False
        TabOrder = 0
        OnClick = btnCloseClick
      end
      object cbWordWrap: TcxCheckBox
        Left = 32
        Top = 15
        Caption = #33258#21160#25442#34892
        Properties.OnChange = cbWordWrapPropertiesChange
        TabOrder = 1
        Transparent = True
      end
    end
    object mmMain: TcxMemo
      Left = 0
      Top = 0
      Align = alClient
      Lines.Strings = (
        '')
      Properties.HideSelection = False
      Properties.ScrollBars = ssVertical
      TabOrder = 1
      Height = 486
      Width = 875
    end
  end
end
