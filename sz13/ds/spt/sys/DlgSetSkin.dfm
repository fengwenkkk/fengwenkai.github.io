object DialogSetSkin: TDialogSetSkin
  Left = 0
  Top = 0
  Caption = #26174#31034#25928#26524
  ClientHeight = 181
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  DesignSize = (
    360
    181)
  PixelsPerInch = 96
  TextHeight = 18
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 121
    Width = 360
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 4
    DesignSize = (
      360
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 360
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 157
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #30830#35748
      Default = True
      LookAndFeel.Kind = lfUltraFlat
      LookAndFeel.NativeStyle = False
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 257
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Caption = #25918#24323
      LookAndFeel.Kind = lfUltraFlat
      LookAndFeel.NativeStyle = False
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
  object cxLabel29: TcxLabel
    Left = 32
    Top = 30
    Caption = #25353#38062
    Transparent = True
  end
  object cbSkin: TcxComboBox
    Left = 80
    Top = 68
    Anchors = [akLeft, akTop, akRight]
    Properties.DropDownListStyle = lsFixedList
    Properties.DropDownRows = 24
    Properties.OnChange = cbSkinPropertiesChange
    TabOrder = 1
    Width = 242
  end
  object cxLabel1: TcxLabel
    Left = 32
    Top = 70
    Caption = #30382#32932
    Transparent = True
  end
  object cbCtrl: TcxComboBox
    Left = 80
    Top = 28
    Properties.DropDownRows = 16
    Properties.OnChange = cbCtrlPropertiesChange
    TabOrder = 0
    Text = 'cbCtrl'
    Width = 242
  end
end
