object DialogSetPredefinedSMS: TDialogSetPredefinedSMS
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  ClientHeight = 235
  ClientWidth = 483
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
    483
    235)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 179
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object btnOK: TcxButton
    Left = 249
    Top = 199
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 0
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 369
    Top = 199
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 2
    OnClick = btnCancelClick
  end
  object cxLabel1: TcxLabel
    Left = 38
    Top = 25
    Caption = #27169#26495#36866#29992#23545#35937
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 132
  end
  object cxLabel2: TcxLabel
    Left = 23
    Top = 60
    Caption = #30701#28040#24687#27169#26495#20869#23481
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 132
  end
  object cbTX: TcxCheckBox
    Left = 144
    Top = 25
    Caption = #36710#36733#21488
    TabOrder = 4
  end
  object cbFX: TcxCheckBox
    Left = 300
    Top = 25
    Caption = #22266#23450#21488
    TabOrder = 5
  end
  object mmInfo: TcxMemo
    Left = 144
    Top = 60
    Properties.ScrollBars = ssVertical
    TabOrder = 6
    Height = 101
    Width = 308
  end
end
