object DataModulePrint: TDataModulePrint
  OldCreateOrder = False
  Height = 150
  Width = 215
  object Printer: TdxComponentPrinter
    DateFormat = 4
    LongOperationTime = 1000
    PreviewOptions.Caption = #25171#21360#39044#35272
    PreviewOptions.PreviewBoundsRect = {0000000000000000C003000080020000}
    TimeFormat = 3
    Version = 0
    Left = 71
    Top = 40
    PixelsPerInch = 96
  end
  object StyleTreeList: TcxStyleRepository
    Left = 151
    Top = 40
    PixelsPerInch = 96
    object StylePrinter: TcxStyle
      AssignedValues = [svColor, svFont]
      Color = clWindow
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = #40657#20307
      Font.Style = []
    end
    object StylePrintTitle: TcxStyle
      AssignedValues = [svColor, svFont]
      Color = clHighlightText
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -27
      Font.Name = #23435#20307
      Font.Style = [fsBold]
    end
    object StyleContent: TcxStyle
      AssignedValues = [svFont]
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = #23435#20307
      Font.Style = []
    end
  end
end
