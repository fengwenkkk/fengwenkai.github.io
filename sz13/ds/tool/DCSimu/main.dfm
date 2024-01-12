inherited FormMain: TFormMain
  Caption = 'DC'
  ClientHeight = 394
  ClientWidth = 817
  Position = poDesktopCenter
  WindowState = wsMaximized
  ExplicitWidth = 833
  ExplicitHeight = 433
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 817
    Height = 394
    ExplicitWidth = 1180
    ExplicitHeight = 687
    object pcMain: TcxPageControl
      Left = 0
      Top = 0
      Width = 608
      Height = 394
      Align = alClient
      TabOrder = 0
      Properties.ActivePage = tsLogin
      Properties.CustomButtons.Buttons = <>
      Properties.TabWidth = 100
      ExplicitLeft = 48
      ExplicitTop = 48
      ExplicitWidth = 289
      ExplicitHeight = 193
      ClientRectBottom = 390
      ClientRectLeft = 4
      ClientRectRight = 604
      ClientRectTop = 25
      object tsLogin: TcxTabSheet
        Caption = #30331#24405
        ImageIndex = 3
        object btnLogin: TcxButton
          Left = 178
          Top = 37
          Width = 75
          Height = 25
          Caption = #30331#24405
          TabOrder = 0
          OnClick = btnLoginClick
        end
        object btnLogout: TcxButton
          Left = 298
          Top = 37
          Width = 75
          Height = 25
          Caption = #30331#20986
          TabOrder = 1
        end
      end
      object cxTabSheet1: TcxTabSheet
        Caption = #30701#20449
        ImageIndex = 0
        ExplicitLeft = 5
        ExplicitTop = 26
        object cxButton1: TcxButton
          Left = 114
          Top = 88
          Width = 75
          Height = 25
          Caption = #21457#36865#30701#20449
          TabOrder = 0
        end
        object cxLabel2: TcxLabel
          Left = 48
          Top = 39
          Caption = #30701#20449#20869#23481
        end
        object cxTextEdit1: TcxTextEdit
          Left = 114
          Top = 38
          TabOrder = 2
          Width = 299
        end
      end
      object cxTabSheet2: TcxTabSheet
        Caption = #20010#21628
        ImageIndex = 1
        ExplicitWidth = 281
        ExplicitHeight = 164
      end
      object cxTabSheet3: TcxTabSheet
        Caption = #32452#21628
        ImageIndex = 2
        ExplicitWidth = 809
      end
    end
    object Panel1: TPanel
      Left = 608
      Top = 0
      Width = 209
      Height = 394
      Align = alRight
      TabOrder = 1
      object cxLabel1: TcxLabel
        Left = 24
        Top = 64
        Caption = #34987#21483
      end
      object edtCallee: TcxTextEdit
        Left = 62
        Top = 63
        TabOrder = 1
        Width = 121
      end
      object LabelMDCStatus: TcxLabel
        Left = 24
        Top = 25
        Caption = 'MDC'#29366#24577#65306
      end
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    SkinName = 'Lilian'
    Left = 344
    Top = 212
  end
  object OpenDialog: TFileOpenDialog
    DefaultExtension = '*.dat'
    FavoriteLinks = <>
    FileTypes = <
      item
        DisplayName = ''
      end>
    Options = []
    Left = 236
    Top = 200
  end
  object TimerCheck: TTimer
    OnTimer = TimerCheckTimer
    Left = 672
    Top = 128
  end
end
