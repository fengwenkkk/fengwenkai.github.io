object DialogSetRecBrd: TDialogSetRecBrd
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
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
    Left = 144
    Top = 197
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 2
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 264
    Top = 197
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 4
    OnClick = btnCancelClick
  end
  object cxLabel1: TcxLabel
    Left = 23
    Top = 25
    Caption = #39044#24405#38899#24191#25773#32534#21495
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 132
  end
  object cxLabel2: TcxLabel
    Left = 23
    Top = 60
    Caption = #39044#24405#38899#24191#25773#20869#23481
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 132
  end
  object mmInfo: TcxMemo
    Left = 144
    Top = 60
    Properties.MaxLength = 0
    Properties.ScrollBars = ssVertical
    TabOrder = 1
    Height = 101
    Width = 308
  end
  object edtRecID: TcxTextEdit
    Left = 144
    Top = 23
    Properties.ReadOnly = False
    TabOrder = 0
    Width = 121
  end
end
