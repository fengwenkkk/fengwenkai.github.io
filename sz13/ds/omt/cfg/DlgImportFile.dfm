object DialogImportFile: TDialogImportFile
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #23548#20837#35774#22791#20449#24687
  ClientHeight = 516
  ClientWidth = 927
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ImportFilePanel: TPanel
    Left = 0
    Top = 0
    Width = 927
    Height = 516
    Align = alClient
    TabOrder = 0
    object gbCtrl: TcxGroupBox
      Left = 1
      Top = 453
      Align = alBottom
      StyleDisabled.BorderColor = clBtnFace
      TabOrder = 0
      Height = 62
      Width = 925
      object btnCancel: TcxButton
        Left = 800
        Top = 23
        Width = 75
        Height = 25
        Caption = #21462#28040
        TabOrder = 0
        OnClick = btnCancelClick
      end
      object btnOK: TcxButton
        Left = 650
        Top = 23
        Width = 75
        Height = 25
        Caption = #30830#23450
        TabOrder = 1
        OnClick = btnOKClick
      end
      object btnExport: TcxButton
        Left = 500
        Top = 24
        Width = 75
        Height = 25
        Caption = #23548#20986#32467#26524
        TabOrder = 2
        OnClick = btnExportClick
      end
    end
    object tlImportDev: TcxTreeList
      Left = 1
      Top = 1
      Width = 925
      Height = 452
      BorderStyle = cxcbsNone
      Align = alClient
      Bands = <
        item
        end>
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsData.Deleting = False
      TabOrder = 1
      object ColumnDevName: TcxTreeListColumn
        Caption.Text = #35774#22791#21517#31216
        DataBinding.ValueType = 'String'
        Width = 122
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnDevType: TcxTreeListColumn
        Caption.Text = #35774#22791#31867#22411
        DataBinding.ValueType = 'String'
        Width = 122
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnMAC: TcxTreeListColumn
        Caption.Text = 'MAC'#22320#22336
        DataBinding.ValueType = 'String'
        Width = 171
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnImportStatus: TcxTreeListColumn
        Caption.Text = #23548#20837#32467#26524
        DataBinding.ValueType = 'String'
        Width = 111
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        SortOrder = soDescending
        SortIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.csv'
    FileName = '*.csv'
    Filter = '*csv'
    Left = 72
    Top = 328
  end
end
