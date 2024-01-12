inherited FormMain: TFormMain
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'AEC'
  ClientHeight = 146
  ClientWidth = 432
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  ExplicitWidth = 448
  ExplicitHeight = 185
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Left = 299
    Top = -55
    Width = 432
    Height = 137
    Align = alNone
    Visible = False
    ExplicitLeft = 299
    ExplicitTop = -55
    ExplicitWidth = 432
    ExplicitHeight = 137
    object cxLabel3: TcxLabel
      Left = 20
      Top = 101
      Caption = #40614#20811#39118
      Properties.Alignment.Horz = taRightJustify
      Properties.Alignment.Vert = taVCenter
      Transparent = True
      AnchorX = 66
      AnchorY = 110
    end
    object cbMic: TcxComboBox
      Left = 80
      Top = 98
      Anchors = [akLeft, akTop, akRight]
      Properties.DropDownListStyle = lsFixedList
      TabOrder = 1
      Width = 320
    end
    object cbSpeaker: TcxComboBox
      Left = 80
      Top = 52
      Anchors = [akLeft, akTop, akRight]
      Properties.DropDownListStyle = lsFixedList
      TabOrder = 2
      Width = 320
    end
    object cxLabel1: TcxLabel
      Left = 20
      Top = 55
      Caption = #25196#22768#22120
      Properties.Alignment.Horz = taRightJustify
      Properties.Alignment.Vert = taVCenter
      Transparent = True
      AnchorX = 66
      AnchorY = 64
    end
  end
  object btnStart: TcxButton
    Left = 83
    Top = 85
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #24320#22987
    TabOrder = 2
    OnClick = btnStartClick
  end
  object btnStop: TcxButton
    Left = 195
    Top = 85
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #20572#27490
    TabOrder = 3
    OnClick = btnStopClick
  end
  object MediaPlayer: TMediaPlayer
    Left = 18
    Top = 8
    Width = 85
    Height = 30
    VisibleButtons = [btPlay, btPause, btStop]
    DoubleBuffered = True
    Visible = False
    ParentDoubleBuffered = False
    TabOrder = 1
    OnNotify = MediaPlayerNotify
  end
  object btnPlay: TcxButton
    Left = 315
    Top = 85
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #22238#25918
    TabOrder = 4
    OnClick = btnPlayClick
  end
  object cxLabel2: TcxLabel
    Left = 83
    Top = 39
    Anchors = [akRight, akBottom]
    Caption = #22686#30410'(dB)'
    Properties.Alignment.Horz = taRightJustify
    Properties.Alignment.Vert = taVCenter
    Transparent = True
    AnchorX = 143
    AnchorY = 48
  end
  object edtVolumeRate: TcxSpinEdit
    Left = 149
    Top = 36
    Anchors = [akRight, akBottom]
    AutoSize = False
    Properties.Alignment.Horz = taLeftJustify
    Properties.LargeIncrement = 5.000000000000000000
    Properties.MaxValue = 20.000000000000000000
    Properties.MinValue = -20.000000000000000000
    TabOrder = 6
    Height = 24
    Width = 55
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Lilian'
    Left = 32
    Top = 104
  end
  object TimerPlay: TTimer
    Enabled = False
    Interval = 40
    OnTimer = TimerPlayTimer
    Left = 264
    Top = 8
  end
  object TimerSend: TTimer
    Enabled = False
    Interval = 1
    OnTimer = TimerSendTimer
    Left = 32
    Top = 56
  end
end
