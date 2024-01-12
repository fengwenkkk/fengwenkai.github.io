object FormStationCfg: TFormStationCfg
  Left = 0
  Top = 0
  Caption = 'FormStationCfg'
  ClientHeight = 578
  ClientWidth = 1002
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object StationPanel: TPanel
    Left = 0
    Top = 0
    Width = 1002
    Height = 578
    Align = alClient
    TabOrder = 0
    object tlMain: TcxTreeList
      Left = 1
      Top = 81
      Width = 1000
      Height = 496
      Align = alClient
      Bands = <
        item
        end>
      Navigator.Buttons.CustomButtons = <>
      TabOrder = 0
      object ColumnStationID: TcxTreeListColumn
        Caption.Text = #36710#31449#32534#21495
        DataBinding.ValueType = 'String'
        Width = 100
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnStationName: TcxTreeListColumn
        Caption.Text = #36710#31449#21517#31216
        DataBinding.ValueType = 'String'
        Width = 100
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnStationType: TcxTreeListColumn
        Caption.Text = #36710#31449#31867#22411
        DataBinding.ValueType = 'String'
        Width = 100
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = ColumnStationTypeGetDisplayText
      end
    end
    object gbMain: TcxGroupBox
      Left = 1
      Top = 1
      Align = alTop
      TabOrder = 1
      Height = 80
      Width = 1000
      object btnAdd: TcxButton
        Left = 50
        Top = 32
        Width = 75
        Height = 25
        Caption = #22686#21152
        TabOrder = 0
        OnClick = btnAddClick
      end
      object btnDel: TcxButton
        Left = 170
        Top = 32
        Width = 75
        Height = 25
        Caption = #21024#38500
        TabOrder = 1
        OnClick = btnDelClick
      end
      object btnSet: TcxButton
        Left = 290
        Top = 32
        Width = 75
        Height = 25
        Caption = #20462#25913
        TabOrder = 2
        OnClick = btnSetClick
      end
      object btnRefresh: TcxButton
        Left = 410
        Top = 32
        Width = 75
        Height = 25
        Caption = #21047#26032
        TabOrder = 3
        OnClick = btnRefreshClick
      end
    end
  end
end
