inherited FormAlarmReport: TFormAlarmReport
  ClientHeight = 388
  ClientWidth = 725
  ExplicitWidth = 741
  ExplicitHeight = 427
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 725
    Height = 388
    ExplicitWidth = 725
    ExplicitHeight = 388
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      PanelStyle.Active = True
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      OnMouseUp = gbMainMouseUp
      OnResize = gbMainResize
      Height = 388
      Width = 725
    end
  end
  object TimerCheck: TTimer
    OnTimer = TimerCheckTimer
    Left = 424
    Top = 112
  end
  object TeeGDIPlus1: TTeeGDIPlus
  end
  object TimerInit: TTimer
    Interval = 1
    OnTimer = TimerInitTimer
    Left = 144
    Top = 144
  end
  object pmMain: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiRefresh'
      end>
    UseOwnFont = False
    Left = 120
    Top = 48
    PixelsPerInch = 96
  end
  object BarManager: TdxBarManager
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Microsoft YaHei UI'
    Font.Style = []
    Categories.Strings = (
      'Default')
    Categories.ItemsVisibles = (
      2)
    Categories.Visibles = (
      True)
    PopupMenuLinks = <>
    UseSystemFont = True
    Left = 240
    Top = 48
    PixelsPerInch = 96
    object pmiRefresh: TdxBarButton
      Caption = #21047#26032
      Category = 0
      Hint = #21047#26032
      Visible = ivAlways
    end
  end
end
