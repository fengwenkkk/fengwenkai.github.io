object FormVersion: TFormVersion
  Left = 0
  Top = 0
  Caption = 'FormVersion'
  ClientHeight = 609
  ClientWidth = 1092
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
  object PanelVersion: TPanel
    Left = 0
    Top = 0
    Width = 1092
    Height = 609
    Align = alClient
    TabOrder = 0
    object tlMain: TcxTreeList
      Left = 1
      Top = 61
      Width = 1090
      Height = 547
      BorderStyle = cxcbsNone
      Align = alClient
      Bands = <
        item
        end>
      LookAndFeel.NativeStyle = False
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsView.GridLines = tlglBoth
      OptionsView.Indicator = True
      OptionsView.ShowRoot = False
      TabOrder = 0
      ExplicitTop = 81
      ExplicitHeight = 527
      object ColumnVer: TcxTreeListColumn
        Caption.Text = #29256#26412#21517#31216
        DataBinding.ValueType = 'String'
        Width = 195
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnFile: TcxTreeListColumn
        Caption.Text = #29256#26412#25991#20214
        DataBinding.ValueType = 'String'
        Width = 430
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnType: TcxTreeListColumn
        Caption.Text = #29256#26412#31867#22411
        DataBinding.ValueType = 'Integer'
        Width = 112
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = ColumnTypeGetDisplayText
      end
      object ColumnTime: TcxTreeListColumn
        Caption.Text = #29256#26412#26102#38388
        DataBinding.ValueType = 'String'
        Width = 156
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
    object gbCtrl: TcxGroupBox
      Left = 1
      Top = 1
      Align = alTop
      Caption = ' '
      Ctl3D = False
      PanelStyle.Active = True
      ParentCtl3D = False
      Style.BorderColor = clRed
      Style.BorderStyle = ebs3D
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 1
      Height = 60
      Width = 1090
      object btnDel: TcxButton
        Left = 150
        Top = 18
        Width = 75
        Height = 25
        Caption = #21024#38500
        LookAndFeel.NativeStyle = False
        TabOrder = 1
        OnClick = btnDelClick
      end
      object btnAdd: TcxButton
        Left = 50
        Top = 18
        Width = 75
        Height = 25
        Caption = #22686#21152
        LookAndFeel.NativeStyle = False
        TabOrder = 0
        OnClick = btnAddClick
      end
      object btnRefresh: TcxButton
        Left = 350
        Top = 18
        Width = 75
        Height = 25
        Caption = #21047#26032
        LookAndFeel.NativeStyle = False
        TabOrder = 2
        OnClick = btnRefreshClick
      end
    end
  end
end
