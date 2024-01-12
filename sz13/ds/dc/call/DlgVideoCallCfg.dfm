object DialogVideoCallCfg: TDialogVideoCallCfg
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #35270#39057#21442#25968
  ClientHeight = 221
  ClientWidth = 351
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  DesignSize = (
    351
    221)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 156
    Width = 1356
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 135
    ExplicitWidth = 1340
  end
  object cxLabel6: TcxLabel
    Left = 30
    Top = 30
    Caption = #35270#39057#26684#24335
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 94
  end
  object LabelCamera: TcxLabel
    Left = 45
    Top = 60
    Caption = #25668#20687#22836
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 94
  end
  object cbVideoFormat: TcxComboBox
    Left = 100
    Top = 28
    TabOrder = 2
    Text = 'cbVideoFormat'
    Width = 153
  end
  object rbFrontCamera: TcxRadioButton
    Left = 100
    Top = 61
    Width = 73
    Height = 21
    Caption = #21069#32622
    Checked = True
    TabOrder = 3
    TabStop = True
  end
  object rbBackCamera: TcxRadioButton
    Left = 184
    Top = 61
    Width = 69
    Height = 21
    Caption = #21518#32622
    TabOrder = 4
  end
  object cxLabel1: TcxLabel
    Left = 30
    Top = 120
    Caption = #30830#35748#26041#24335
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 94
  end
  object cbNeedConfirm: TcxCheckBox
    Left = 100
    Top = 120
    Caption = #38656#35201#29992#25143#30830#35748
    TabOrder = 7
  end
  object btnCancel: TcxButton
    Left = 253
    Top = 177
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    TabOrder = 9
    OnClick = btnCancelClick
  end
  object btnOK: TcxButton
    Left = 153
    Top = 177
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    TabOrder = 8
    OnClick = btnOKClick
  end
  object cxLabel2: TcxLabel
    Left = 30
    Top = 90
    Caption = #20276#38899#36873#25321
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 94
  end
  object cbHasVoice: TcxCheckBox
    Left = 100
    Top = 88
    Caption = #20801#35768
    State = cbsChecked
    TabOrder = 6
  end
end
