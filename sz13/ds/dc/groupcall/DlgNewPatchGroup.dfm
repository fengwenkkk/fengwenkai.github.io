object DialogNewPatchGroup: TDialogNewPatchGroup
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #26032#24314#27966#25509#32452
  ClientHeight = 144
  ClientWidth = 321
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 18
  object cxLabel6: TcxLabel
    Left = 30
    Top = 30
    Caption = #32452#21517#31216
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 79
  end
  object edtName: TcxTextEdit
    Left = 100
    Top = 28
    Properties.MaxLength = 31
    TabOrder = 0
    Width = 175
  end
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 84
    Width = 321
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 2
    DesignSize = (
      321
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 321
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 119
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #30830#35748
      Default = True
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 219
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
end
