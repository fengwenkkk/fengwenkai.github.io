inherited FormRecBrdInfo: TFormRecBrdInfo
  ClientHeight = 507
  ClientWidth = 1234
  OnCreate = FormCreate
  ExplicitWidth = 1250
  ExplicitHeight = 546
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1234
    Height = 507
    ExplicitWidth = 1234
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
      Width = 1234
      object btnQuery: TcxButton
        Left = 520
        Top = 18
        Width = 75
        Height = 25
        Caption = #26597#35810
        Default = True
        LookAndFeel.NativeStyle = False
        TabOrder = 6
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
      object cxLabel3: TcxLabel
        Left = 913
        Top = 20
        Caption = #31867#21035
        Transparent = True
        Visible = False
      end
      object cbReqCallType: TcxComboBox
        Left = 959
        Top = 18
        Properties.DropDownListStyle = lsFixedList
        TabOrder = 2
        Visible = False
        Width = 122
      end
      object btnNext: TcxButton
        Left = 620
        Top = 18
        Width = 75
        Height = 25
        Caption = #24448#21518
        Enabled = False
        LookAndFeel.NativeStyle = False
        TabOrder = 7
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
        TabOrder = 8
        OnClick = btnPrevClick
      end
    end
    object tlMain: TcxTreeList
      Left = 0
      Top = 60
      Width = 1234
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
        Width = 94
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnTime: TcxTreeListColumn
        Caption.Text = #24191#25773#26102#38388
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnDCUserID: TcxTreeListColumn
        Caption.Text = #21457#36215#26041
        DataBinding.ValueType = 'Integer'
        Width = 133
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnTrainList: TcxTreeListColumn
        Caption.Text = #25509#25910#26041
        DataBinding.ValueType = 'String'
        Width = 148
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnRecID: TcxTreeListColumn
        Caption.Text = #24405#38899#32534#21495
        DataBinding.ValueType = 'Integer'
        Width = 89
        Position.ColIndex = 4
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object ColumnRecInfo: TcxTreeListColumn
        Caption.Text = #24405#38899#20869#23481
        DataBinding.ValueType = 'String'
        Width = 468
        Position.ColIndex = 5
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
  end
end
