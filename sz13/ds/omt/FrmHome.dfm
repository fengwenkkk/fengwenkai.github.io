inherited FormHome: TFormHome
  ClientHeight = 507
  ClientWidth = 1234
  ExplicitWidth = 1250
  ExplicitHeight = 546
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1234
    Height = 507
    ExplicitWidth = 1234
    ExplicitHeight = 507
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      PanelStyle.Active = True
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Height = 507
      Width = 1234
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.xls'
    Filter = '*.xls|*.xls|*.csv|*.csv'
    Left = 352
    Top = 176
  end
  object TimerInit: TTimer
    OnTimer = TimerInitTimer
    Left = 191
    Top = 308
  end
end
