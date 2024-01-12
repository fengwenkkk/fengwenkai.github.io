object DialogSetOperator: TDialogSetOperator
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #35774#32622#35843#24230#21592#22995#21517
  ClientHeight = 144
  ClientWidth = 323
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
    Left = 23
    Top = 30
    Caption = #35843#24230#21592#22995#21517
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 102
  end
  object edtOperator: TcxTextEdit
    Left = 119
    Top = 29
    Properties.MaxLength = 31
    Properties.ReadOnly = False
    TabOrder = 2
    Width = 173
  end
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 84
    Width = 323
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 1
    DesignSize = (
      323
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 323
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 121
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
      Left = 221
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
