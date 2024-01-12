object FormDevInfo: TFormDevInfo
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  Caption = #21578#35686#35814#24773
  ClientHeight = 583
  ClientWidth = 1099
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object tlDevAlarm: TcxTreeList
    Left = 0
    Top = 0
    Width = 1099
    Height = 583
    Align = alClient
    Bands = <
      item
      end>
    Navigator.Buttons.CustomButtons = <>
    TabOrder = 0
    object ColumnDevName: TcxTreeListColumn
      Caption.Text = #35774#22791#21517#31216
      DataBinding.ValueType = 'String'
      Width = 100
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnDevType: TcxTreeListColumn
      Caption.Text = #35774#22791#31867#22411
      DataBinding.ValueType = 'String'
      Width = 100
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnDevTypeGetDisplayText
    end
    object ColumnAlarmType: TcxTreeListColumn
      Caption.Text = #21578#35686#31867#21035
      DataBinding.ValueType = 'String'
      Width = 136
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmIDGetDisplayText
    end
    object ColumnAlarmLevel: TcxTreeListColumn
      Caption.Text = #21578#35686#31561#32423
      DataBinding.ValueType = 'String'
      Width = 100
      Position.ColIndex = 3
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmLevelGetDisplayText
    end
    object ColumnAlarmRaiseTime: TcxTreeListColumn
      Caption.Text = #20135#29983#26102#38388
      DataBinding.ValueType = 'String'
      Width = 238
      Position.ColIndex = 4
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmRaiseTimeGetDisplayText
    end
    object ColumnAlarmInfo: TcxTreeListColumn
      Visible = False
      Caption.Text = #21578#35686#20869#23481
      DataBinding.ValueType = 'String'
      Width = 312
      Position.ColIndex = 5
      Position.RowIndex = 0
      Position.BandIndex = 0
      SortOrder = soDescending
      SortIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnAlarmReason: TcxTreeListColumn
      Visible = False
      Caption.Text = #21578#35686#21407#22240
      DataBinding.ValueType = 'String'
      Width = 100
      Position.ColIndex = 6
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
end
