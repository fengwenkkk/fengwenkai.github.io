object FormMain: TFormMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'PIS'#32452#25773#25509#21475#27979#35797
  ClientHeight = 229
  ClientWidth = 367
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 14
  object Label1: TLabel
    Left = 40
    Top = 40
    Width = 70
    Height = 14
    Caption = #36710#36733#21488#22320#22336
  end
  object Label2: TLabel
    Left = 52
    Top = 80
    Width = 56
    Height = 14
    Caption = #32593#20851#22320#22336
  end
  object Label3: TLabel
    Left = 17
    Top = 120
    Width = 91
    Height = 14
    Caption = 'PIS'#26381#21153#22120#22320#22336
  end
  object edtLocalAddr: TEdit
    Left = 120
    Top = 37
    Width = 161
    Height = 22
    TabOrder = 0
    Text = '0.0.0.0'
  end
  object edtGateway: TEdit
    Left = 120
    Top = 77
    Width = 161
    Height = 22
    TabOrder = 1
    Text = '1.1.1.1'
  end
  object edtPISAddr: TEdit
    Left = 120
    Top = 117
    Width = 161
    Height = 22
    TabOrder = 2
    Text = '2.2.2.2'
  end
  object btnSet: TButton
    Left = 120
    Top = 176
    Width = 75
    Height = 25
    Caption = #27979#35797
    TabOrder = 3
    OnClick = btnSetClick
  end
  object btnClose: TButton
    Left = 232
    Top = 176
    Width = 75
    Height = 25
    Caption = #20851#38381
    TabOrder = 4
    OnClick = btnCloseClick
  end
end
