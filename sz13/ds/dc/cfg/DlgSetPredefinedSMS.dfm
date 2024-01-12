object DialogSetPredefinedSMS: TDialogSetPredefinedSMS
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 295
  ClientWidth = 499
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnShow = FormShow
  DesignSize = (
    499
    295)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 239
    Width = 1016
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
    ExplicitWidth = 1000
  end
  object btnOK: TcxButton
    Left = 144
    Top = 257
    Width = 75
    Height = 27
    Anchors = [akLeft, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 7
    OnClick = btnOKClick
    ExplicitTop = 197
  end
  object btnCancel: TcxButton
    Left = 273
    Top = 257
    Width = 75
    Height = 27
    Anchors = [akLeft, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 8
    OnClick = btnCancelClick
    ExplicitTop = 197
  end
  object cxLabel1: TcxLabel
    Left = 48
    Top = 25
    Caption = #36866#29992#23545#35937
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 112
  end
  object cxLabel2: TcxLabel
    Left = 48
    Top = 60
    Caption = #27169#26495#31867#22411
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 112
  end
  object cbTX: TcxCheckBox
    Left = 130
    Top = 25
    Caption = #36710#36733#21488
    TabOrder = 0
  end
  object cbFX: TcxCheckBox
    Left = 300
    Top = 25
    Caption = #22266#23450#21488
    TabOrder = 1
  end
  object mmInfo: TcxMemo
    Left = 118
    Top = 99
    Properties.ScrollBars = ssVertical
    TabOrder = 3
    Height = 125
    Width = 334
  end
  object cxLabel3: TcxLabel
    Left = 48
    Top = 95
    Caption = #27169#26495#20869#23481
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 112
  end
  object cbSDSType: TcxComboBox
    Left = 118
    Top = 58
    TabOrder = 2
    Text = 'cbSDSType'
    Width = 334
  end
end
