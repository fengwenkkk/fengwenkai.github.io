inherited FormIPHCallInfo: TFormIPHCallInfo
  ClientHeight = 507
  ClientWidth = 1020
  OnCreate = FormCreate
  ExplicitWidth = 1036
  ExplicitHeight = 546
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1020
    Height = 507
    ExplicitWidth = 1020
    ExplicitHeight = 507
    object gbCtrl: TcxGroupBox
      Left = 0
      Top = 0
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
      TabOrder = 0
      Height = 60
      Width = 1020
      object btnQuery: TcxButton
        Left = 520
        Top = 18
        Width = 75
        Height = 25
        Caption = #26597#35810
        Default = True
        LookAndFeel.NativeStyle = False
        TabOrder = 4
        OnClick = btnQueryClick
      end
      object cxLabel1: TcxLabel
        Left = 16
        Top = 21
        Caption = #24320#22987#26102#38388
        Transparent = True
      end
      object deStartTime: TcxDateEdit
        Left = 88
        Top = 19
        Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
        Properties.EditFormat = 'YYYY-MM-DD HH:mm'
        Properties.InputKind = ikRegExpr
        Properties.Kind = ckDateTime
        TabOrder = 0
        Width = 154
      end
      object cxLabel2: TcxLabel
        Left = 260
        Top = 21
        Caption = #32467#26463#26102#38388
        Transparent = True
      end
      object deEndTime: TcxDateEdit
        Left = 332
        Top = 19
        Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
        Properties.EditFormat = 'YYYY-MM-DD HH:mm'
        Properties.InputKind = ikRegExpr
        Properties.Kind = ckDateTime
        TabOrder = 1
        Width = 154
      end
      object btnNext: TcxButton
        Left = 620
        Top = 18
        Width = 75
        Height = 25
        Caption = #24448#21518
        Enabled = False
        LookAndFeel.NativeStyle = False
        TabOrder = 5
        OnClick = btnNextClick
      end
      object btnPrev: TcxButton
        Left = 720
        Top = 18
        Width = 75
        Height = 25
        Caption = #24448#21069
        Enabled = False
        LookAndFeel.NativeStyle = False
        TabOrder = 6
        OnClick = btnPrevClick
      end
    end
    object tlMain: TcxTreeList
      Left = 0
      Top = 60
      Width = 1020
      Height = 447
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
      TabOrder = 1
      object ColumnIndex: TcxTreeListColumn
        Caption.Text = #32534#21495
        DataBinding.ValueType = 'Integer'
        Width = 70
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnApplyTime: TcxTreeListColumn
        Caption.Text = #35831#27714#26102#38388
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnTrainUnitID: TcxTreeListColumn
        Caption.Text = #36710#20307#21495
        DataBinding.ValueType = 'Integer'
        Width = 106
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnTrainID: TcxTreeListColumn
        Caption.Text = #36710#27425#21495
        DataBinding.ValueType = 'String'
        Width = 117
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnCarriageID: TcxTreeListColumn
        Caption.Text = #36710#21410#21495
        DataBinding.ValueType = 'Integer'
        Width = 69
        Position.ColIndex = 4
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnDevID: TcxTreeListColumn
        Caption.Text = #23545#35762#22120#32534#21495
        DataBinding.ValueType = 'Integer'
        Width = 102
        Position.ColIndex = 5
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnDCUserID: TcxTreeListColumn
        Caption.Text = #25509#21548#35843#24230#21488
        DataBinding.ValueType = 'String'
        Width = 100
        Position.ColIndex = 6
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnAcceptTime: TcxTreeListColumn
        Caption.Text = #23545#35762#24320#22987#26102#38388
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 7
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnStopTime: TcxTreeListColumn
        Caption.Text = #23545#35762#32467#26463#26102#38388
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 8
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
  end
end
