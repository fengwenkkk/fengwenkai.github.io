inherited FormMain: TFormMain
  Caption = 'PIS'
  ClientHeight = 571
  ClientWidth = 828
  Position = poScreenCenter
  ExplicitWidth = 844
  ExplicitHeight = 610
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 828
    Height = 571
    ExplicitWidth = 828
    ExplicitHeight = 571
    object btnStart: TcxButton
      Left = 584
      Top = 16
      Width = 75
      Height = 25
      Caption = #24320#22987#24405#20687
      TabOrder = 0
      OnClick = btnStartClick
    end
    object PanelVideo: TPanel
      Left = 17
      Top = 96
      Width = 781
      Height = 454
      Anchors = [akLeft, akTop, akRight, akBottom]
      TabOrder = 1
    end
    object edtURL: TcxTextEdit
      Left = 24
      Top = 17
      TabOrder = 2
      Text = 'rtsp://127.0.0.1:8554/test'
      Width = 537
    end
    object btnStop: TcxButton
      Left = 584
      Top = 56
      Width = 75
      Height = 25
      Caption = #32467#26463#24405#20687
      TabOrder = 3
      OnClick = btnStopClick
    end
    object edtFile: TcxTextEdit
      Left = 24
      Top = 57
      TabOrder = 4
      Text = 'e:\test1.mp4'
      Width = 257
    end
    object cxButton2: TcxButton
      Left = 472
      Top = 56
      Width = 75
      Height = 25
      Caption = #25293#29031
      TabOrder = 5
      OnClick = cxButton2Click
    end
    object btnSDLPlay: TcxButton
      Left = 696
      Top = 16
      Width = 75
      Height = 25
      Caption = 'SDL'#25773#25918
      TabOrder = 6
      OnClick = btnSDLPlayClick
    end
    object cxButton1: TcxButton
      Left = 696
      Top = 56
      Width = 75
      Height = 25
      Caption = 'PCM'#24405#38899
      TabOrder = 7
      OnClick = cxButton1Click
    end
    object cxButton3: TcxButton
      Left = 360
      Top = 56
      Width = 75
      Height = 25
      Caption = #35774#22791#21015#34920
      TabOrder = 8
      OnClick = cxButton3Click
    end
  end
  object Timer: TTimer
    Left = 496
    Top = 140
  end
  object TimerSendPCM: TTimer
    Enabled = False
    Interval = 100
    Left = 384
    Top = 172
  end
  object TimerInit: TTimer
    Interval = 1
    Left = 460
    Top = 172
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Office2010Blue'
    Left = 624
    Top = 188
  end
end
