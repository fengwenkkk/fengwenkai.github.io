inherited FormAlarm: TFormAlarm
  Caption = 'FormAlarm'
  ClientHeight = 510
  ClientWidth = 1314
  Position = poDesigned
  ExplicitWidth = 1330
  ExplicitHeight = 549
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1314
    Height = 510
    ParentShowHint = False
    ExplicitWidth = 1314
    ExplicitHeight = 510
  end
  object pcMain: TcxPageControl
    Left = 0
    Top = 0
    Width = 1314
    Height = 510
    Align = alClient
    TabOrder = 1
    Properties.ActivePage = tsActiveAlarm
    Properties.CustomButtons.Buttons = <>
    Properties.TabWidth = 120
    LookAndFeel.NativeStyle = False
    LookAndFeel.SkinName = 'Office2010Blue'
    OnPageChanging = pcMainPageChanging
    ClientRectBottom = 504
    ClientRectLeft = 2
    ClientRectRight = 1308
    ClientRectTop = 28
    object tsActiveAlarm: TcxTabSheet
      Caption = #27963#36291#21578#35686
      ImageIndex = 0
      ExplicitLeft = 4
      ExplicitTop = 25
      ExplicitHeight = 481
    end
    object tsHistoryAlarm: TcxTabSheet
      AllowCloseButton = False
      Caption = #21382#21490#21578#35686
      ImageIndex = 1
      ExplicitLeft = 4
      ExplicitTop = 25
      ExplicitHeight = 481
    end
    object tsAlarmReport: TcxTabSheet
      Caption = #21578#35686#32479#35745
      ImageIndex = 2
      ExplicitLeft = 4
      ExplicitTop = 25
      ExplicitHeight = 481
    end
  end
  object TimerRefresh: TTimer
    Interval = 60000
    Left = 730
    Top = 170
  end
  object TimerInit: TTimer
    OnTimer = TimerInitTimer
    Left = 690
    Top = 100
  end
end
