object FormSDS: TFormSDS
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  Caption = #35843#24230#21629#20196
  ClientHeight = 450
  ClientWidth = 723
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 15
  object gbMain: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    PanelStyle.Active = True
    TabOrder = 0
    Transparent = True
    Height = 450
    Width = 723
    object pcMain: TcxPageControl
      Left = 2
      Top = 2
      Width = 719
      Height = 446
      Align = alClient
      TabOrder = 0
      Properties.ActivePage = tsSDSTemplate
      Properties.CustomButtons.Buttons = <>
      Properties.TabWidth = 100
      ClientRectBottom = 442
      ClientRectLeft = 4
      ClientRectRight = 715
      ClientRectTop = 26
      object tsLatestSDS: TcxTabSheet
        Caption = #24403#26085#24037#21333
        ImageIndex = 0
      end
      object tsSDSTemplate: TcxTabSheet
        Caption = #24037#21333#27169#26495
        ImageIndex = 1
      end
    end
  end
  object TimerInit: TTimer
    OnTimer = TimerInitTimer
    Left = 347
    Top = 146
  end
end
