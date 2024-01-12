object DialogSetAlarmLevel: TDialogSetAlarmLevel
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = #21578#35686#37197#32622
  ClientHeight = 244
  ClientWidth = 446
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object dxBevel1: TdxBevel
    Left = 0
    Top = 184
    Width = 446
    Height = 60
    Align = alBottom
    ExplicitTop = 168
    ExplicitWidth = 470
  end
  object cbAlarmLevel: TcxComboBox
    Left = 150
    Top = 112
    TabOrder = 0
    Width = 200
  end
  object lbAlarmType: TcxLabel
    Left = 72
    Top = 74
    Caption = #21578#35686#31867#21035
    ParentFont = False
    Style.Font.Charset = ANSI_CHARSET
    Style.Font.Color = clWindowText
    Style.Font.Height = -15
    Style.Font.Name = #23435#20307
    Style.Font.Style = []
    Style.IsFontAssigned = True
  end
  object cxLabel3: TcxLabel
    Left = 42
    Top = 114
    Caption = #35774#32622#21578#35686#31561#32423
    ParentFont = False
    Style.Font.Charset = ANSI_CHARSET
    Style.Font.Color = clWindowText
    Style.Font.Height = -15
    Style.Font.Name = #23435#20307
    Style.Font.Style = []
    Style.IsFontAssigned = True
  end
  object btnOK: TcxButton
    Left = 150
    Top = 203
    Width = 75
    Height = 25
    Caption = #30830#23450
    TabOrder = 3
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 275
    Top = 203
    Width = 75
    Height = 25
    Caption = #21462#28040
    TabOrder = 4
    OnClick = btnCancelClick
  end
  object cxLabel4: TcxLabel
    Left = 72
    Top = 154
    Caption = #26159#21542#24573#30053
    ParentFont = False
    Style.Font.Charset = ANSI_CHARSET
    Style.Font.Color = clWindowText
    Style.Font.Height = -15
    Style.Font.Name = #23435#20307
    Style.Font.Style = []
    Style.IsFontAssigned = True
  end
  object cbIgnore: TcxCheckBox
    Left = 150
    Top = 152
    Caption = #26159
    ParentFont = False
    Style.Font.Charset = ANSI_CHARSET
    Style.Font.Color = clWindowText
    Style.Font.Height = -15
    Style.Font.Name = #23435#20307
    Style.Font.Style = []
    Style.IsFontAssigned = True
    TabOrder = 6
  end
  object cbAlarmID: TcxComboBox
    Left = 150
    Top = 72
    TabOrder = 7
    Width = 200
  end
  object lbDevType: TcxLabel
    Left = 72
    Top = 34
    Caption = #35774#22791#31867#22411
    ParentFont = False
    Style.Font.Charset = ANSI_CHARSET
    Style.Font.Color = clWindowText
    Style.Font.Height = -15
    Style.Font.Name = #23435#20307
    Style.Font.Style = []
    Style.IsFontAssigned = True
  end
  object edtDevType: TcxTextEdit
    Left = 150
    Top = 32
    TabOrder = 9
    Width = 200
  end
end
