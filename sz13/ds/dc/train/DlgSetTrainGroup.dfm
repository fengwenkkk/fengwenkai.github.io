object DialogSetTrainGroup: TDialogSetTrainGroup
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #21015#36710#36716#32452
  ClientHeight = 195
  ClientWidth = 390
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
    Left = 48
    Top = 30
    Caption = #21015#36710#36710#27425#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object edtTrainID: TcxTextEdit
    Left = 144
    Top = 29
    Properties.MaxLength = 31
    Properties.ReadOnly = True
    TabOrder = 4
    Width = 173
  end
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 135
    Width = 390
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 2
    DesignSize = (
      390
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 390
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 188
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
      Left = 288
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
  object cxLabel1: TcxLabel
    Left = 33
    Top = 70
    Caption = #21015#36710#27880#20876#20301#32622
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object cbTrainPos: TcxComboBox
    Left = 144
    Top = 69
    TabOrder = 1
    Width = 173
  end
end
