object FormUserCfg: TFormUserCfg
  Left = 0
  Top = 0
  ClientHeight = 553
  ClientWidth = 1003
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 14
  object UserPanel: TPanel
    Left = 0
    Top = 0
    Width = 1003
    Height = 553
    Align = alClient
    TabOrder = 0
    object tlMain: TcxTreeList
      Left = 1
      Top = 61
      Width = 1001
      Height = 491
      Align = alClient
      Bands = <
        item
        end>
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsSelection.MultiSelect = True
      PopupMenu = pmUserMan
      TabOrder = 0
      ExplicitTop = 81
      ExplicitHeight = 471
      Data = {
        00000500A60000000F00000044617461436F6E74726F6C6C6572310400000012
        000000546378537472696E6756616C7565547970651200000054637853747269
        6E6756616C75655479706512000000546378537472696E6756616C7565547970
        6512000000546378537472696E6756616C75655479706501000000445855464D
        5401010000000000000008080000000000000000FFFFFFFFFFFFFFFFFFFFFFFF
        0A0801000000}
      object ColumnID: TcxTreeListColumn
        Caption.Text = #24207#21495
        DataBinding.ValueType = 'String'
        Options.Moving = False
        Width = 71
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnUserName: TcxTreeListColumn
        Caption.Text = #29992#25143#21517
        DataBinding.ValueType = 'String'
        Options.Moving = False
        Width = 144
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnUserPwd: TcxTreeListColumn
        Caption.Text = #29992#25143#23494#30721
        DataBinding.ValueType = 'String'
        Options.Moving = False
        Width = 186
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnUserType: TcxTreeListColumn
        Caption.Text = #31867#22411
        DataBinding.ValueType = 'String'
        Options.Moving = False
        Width = 208
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = ColumnUserTypeGetDisplayText
      end
    end
    object gbCtrl: TcxGroupBox
      Left = 1
      Top = 1
      Margins.Right = 0
      ParentCustomHint = False
      Align = alTop
      ParentBackground = False
      ParentFont = False
      Style.Font.Charset = DEFAULT_CHARSET
      Style.Font.Color = clWindowText
      Style.Font.Height = -11
      Style.Font.Name = 'Tahoma'
      Style.Font.Style = []
      Style.IsFontAssigned = True
      TabOrder = 1
      Height = 60
      Width = 1001
      object btnAdd: TcxButton
        Left = 50
        Top = 18
        Width = 75
        Height = 25
        Caption = #22686#21152
        TabOrder = 0
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = #23435#20307
        Font.Style = []
        ParentFont = False
        OnClick = btnAddClick
      end
      object btnDel: TcxButton
        Left = 150
        Top = 18
        Width = 75
        Height = 25
        Caption = #21024#38500
        TabOrder = 1
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = #23435#20307
        Font.Style = []
        ParentFont = False
        OnClick = btnDelClick
      end
      object btnSet: TcxButton
        Left = 250
        Top = 18
        Width = 75
        Height = 25
        Caption = #20462#25913
        TabOrder = 2
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = #23435#20307
        Font.Style = []
        ParentFont = False
        OnClick = btnSetClick
      end
      object btnRefresh: TcxButton
        Left = 350
        Top = 18
        Width = 75
        Height = 25
        Caption = #21047#26032
        TabOrder = 3
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = #23435#20307
        Font.Style = []
        ParentFont = False
        OnClick = btnRefreshClick
      end
    end
  end
  object dxbmanPop: TdxBarManager
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
    Left = 456
    Top = 152
    PixelsPerInch = 96
    object pmiAdd: TdxBarButton
      Caption = #22686#21152#26032#29992#25143
      Category = 0
      Hint = #22686#21152#26032#29992#25143
      Visible = ivAlways
      OnClick = btnAddClick
    end
    object pmiDel: TdxBarButton
      Caption = #21024#38500#29992#25143
      Category = 0
      Hint = #21024#38500#29992#25143
      Visible = ivAlways
      OnClick = btnDelClick
    end
    object pmiSet: TdxBarButton
      Caption = #20462#25913#29992#25143
      Category = 0
      Hint = #20462#25913#29992#25143
      Visible = ivAlways
      OnClick = btnSetClick
    end
    object pmiRefresh: TdxBarButton
      Caption = #21047#26032
      Category = 0
      Hint = #21047#26032
      Visible = ivAlways
      OnClick = btnRefreshClick
    end
  end
  object pmUserMan: TdxBarPopupMenu
    BarManager = dxbmanPop
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiAdd'
      end
      item
        Visible = True
        ItemName = 'pmiDel'
      end
      item
        Visible = True
        ItemName = 'pmiSet'
      end
      item
        Visible = True
        ItemName = 'pmiRefresh'
      end>
    UseOwnFont = False
    Left = 520
    Top = 152
    PixelsPerInch = 96
  end
end
