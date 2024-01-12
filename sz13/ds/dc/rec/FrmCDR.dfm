inherited FormCDR: TFormCDR
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
      object pcMain: TcxPageControl
        Left = 3
        Top = 3
        Width = 1228
        Height = 501
        Align = alClient
        TabOrder = 0
        Properties.ActivePage = tsP2PCall
        Properties.CustomButtons.Buttons = <>
        Properties.TabWidth = 120
        LookAndFeel.SkinName = 'Office2010Blue'
        ClientRectBottom = 497
        ClientRectLeft = 4
        ClientRectRight = 1224
        ClientRectTop = 25
        object tsP2PCall: TcxTabSheet
          Caption = #20010#21628
          ImageIndex = 0
        end
        object tsGroupCall: TcxTabSheet
          Caption = #32452#21628
          ImageIndex = 1
        end
        object tsEmergencyCall: TcxTabSheet
          Caption = #32039#24613#21628#21483
          ImageIndex = 3
          TabVisible = False
        end
        object tsBrd: TcxTabSheet
          Caption = #20154#24037#24191#25773
          ImageIndex = 4
        end
      end
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
