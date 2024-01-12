object FormAlarmCfg: TFormAlarmCfg
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'FormAlarmCfg'
  ClientHeight = 450
  ClientWidth = 873
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
  object gbWorkPanel: TcxGroupBox
    Left = 0
    Top = 0
    Align = alTop
    Ctl3D = True
    ParentCtl3D = False
    TabOrder = 0
    Height = 60
    Width = 873
    object btnRefresh: TcxButton
      Left = 720
      Top = 17
      Width = 75
      Height = 25
      Caption = #21047#26032
      TabOrder = 4
      OnClick = btnRefreshClick
    end
    object btnSet: TcxButton
      Left = 620
      Top = 17
      Width = 75
      Height = 25
      Caption = #20462#25913
      TabOrder = 3
      OnClick = pmiSetClick
    end
    object lbDevType: TcxLabel
      Left = 16
      Top = 21
      Caption = #35774#22791#31867#22411
    end
    object cbDevType: TcxComboBox
      Left = 88
      Top = 19
      Properties.OnChange = cbDevTypePropertiesChange
      TabOrder = 0
      Text = #36873#25321#35774#22791#31867#22411
      Width = 142
    end
    object btnAdd: TcxButton
      Left = 420
      Top = 17
      Width = 79
      Height = 25
      Caption = #22686#21152
      TabOrder = 1
      OnClick = pmiAddClick
    end
    object btnDel: TcxButton
      Left = 520
      Top = 17
      Width = 79
      Height = 25
      Caption = #21024#38500
      TabOrder = 2
      OnClick = btnDelClick
    end
    object cxLabel1: TcxLabel
      Left = 1144
      Top = 456
      Caption = 'cxLabel1'
    end
  end
  object tlAlarmCfg: TcxTreeList
    Left = 0
    Top = 60
    Width = 873
    Height = 390
    Align = alClient
    Bands = <
      item
      end>
    LookAndFeel.NativeStyle = False
    LookAndFeel.SkinName = 'Office2010Blue'
    Navigator.Buttons.CustomButtons = <>
    PopupMenu = pmMain
    TabOrder = 1
    OnCustomDrawDataCell = tlAlarmCfgCustomDrawDataCell
    object ColumnAlarmID: TcxTreeListColumn
      Caption.Text = #21578#35686#31867#22411
      DataBinding.ValueType = 'String'
      Width = 189
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmIDGetDisplayText
    end
    object ColumnAlarmLevel: TcxTreeListColumn
      Caption.Text = #21578#35686#31561#32423
      DataBinding.ValueType = 'String'
      Width = 128
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmLevelGetDisplayText
    end
    object ColumnStatus: TcxTreeListColumn
      Caption.Text = #21578#35686#29366#24577
      DataBinding.ValueType = 'String'
      Width = 85
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmStatusGetDisplayText
    end
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
    Left = 200
    Top = 216
    PixelsPerInch = 96
    object pmiSet: TdxBarButton
      Caption = #21578#35686#31561#32423#37197#32622
      Category = 0
      Hint = #21578#35686#31561#32423#37197#32622
      Visible = ivAlways
      OnClick = pmiSetClick
    end
    object pmiRefresh: TdxBarButton
      Caption = #21047#26032
      Category = 0
      Hint = #21047#26032
      Visible = ivAlways
    end
  end
  object pmMain: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiSet'
      end
      item
        Visible = True
        ItemName = 'pmiRefresh'
      end>
    UseOwnFont = False
    Left = 336
    Top = 208
    PixelsPerInch = 96
  end
end
