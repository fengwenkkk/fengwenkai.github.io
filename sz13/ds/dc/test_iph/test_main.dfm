object Form1: TForm1
  Left = 665
  Top = 318
  Caption = 'Form1'
  ClientHeight = 229
  ClientWidth = 484
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object btnSend: TButton
    Left = 108
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Send'
    TabOrder = 0
    OnClick = btnSendClick
  end
  object btnStop: TButton
    Left = 272
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 1
    OnClick = btnStopClick
  end
  object TimerSend: TTimer
    Enabled = False
    Interval = 50
    OnTimer = TimerSendTimer
    Left = 80
    Top = 160
  end
end
