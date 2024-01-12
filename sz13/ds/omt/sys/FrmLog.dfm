inherited FormLog: TFormLog
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
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      PanelStyle.Active = True
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Height = 507
      Width = 1234
      object gbCtrl: TcxGroupBox
        Left = 3
        Top = 3
        Align = alTop
        Caption = ' '
        Ctl3D = False
        PanelStyle.Active = True
        ParentCtl3D = False
        Style.BorderColor = clRed
        Style.BorderStyle = ebs3D
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Blue'
        Style.Shadow = False
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 0
        ExplicitLeft = 1
        DesignSize = (
          1228
          60)
        Height = 60
        Width = 1228
        object btnQuery: TcxButton
          Left = 525
          Top = 18
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
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
          Width = 142
        end
        object cxLabel2: TcxLabel
          Left = 242
          Top = 21
          Caption = #32467#26463#26102#38388
          Transparent = True
        end
        object deEndTime: TcxDateEdit
          Left = 314
          Top = 19
          Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
          Properties.EditFormat = 'YYYY-MM-DD HH:mm'
          Properties.InputKind = ikRegExpr
          Properties.Kind = ckDateTime
          TabOrder = 1
          Width = 142
        end
        object cxLabel3: TcxLabel
          Left = 805
          Top = 20
          Caption = #29992#25143
          Transparent = True
          Visible = False
        end
        object edtDCUserID: TcxTextEdit
          Left = 852
          Top = 19
          TabOrder = 2
          Visible = False
          Width = 98
        end
        object btnNext: TcxButton
          Left = 625
          Top = 18
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #24448#21518
          Enabled = False
          LookAndFeel.NativeStyle = False
          TabOrder = 7
          OnClick = btnNextClick
        end
        object btnPrev: TcxButton
          Left = 725
          Top = 18
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #24448#21069
          Enabled = False
          LookAndFeel.NativeStyle = False
          TabOrder = 8
          OnClick = btnPrevClick
        end
        object btnExport: TcxButton
          Left = 1025
          Top = 18
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #23548#20986
          LookAndFeel.NativeStyle = False
          TabOrder = 9
          Visible = False
          OnClick = btnExportClick
        end
        object btnPrint: TcxButton
          Left = 1125
          Top = 18
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #25171#21360
          Enabled = False
          LookAndFeel.NativeStyle = False
          TabOrder = 10
          Visible = False
          OnClick = btnPrintClick
        end
      end
      object tlMain: TcxTreeList
        Left = 3
        Top = 63
        Width = 1228
        Height = 441
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
        object Column1: TcxTreeListColumn
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
          Caption.Text = #26102#38388
          DataBinding.ValueType = 'String'
          Width = 130
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTimeGetDisplayText
        end
        object ColumnUser: TcxTreeListColumn
          Visible = False
          Caption.Text = #35843#24230#21488
          DataBinding.ValueType = 'Integer'
          Width = 101
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn5: TcxTreeListColumn
          Caption.Text = #29992#25143
          DataBinding.ValueType = 'String'
          Width = 108
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnInfo: TcxTreeListColumn
          Caption.Text = #25805#20316#26085#24535
          DataBinding.ValueType = 'String'
          Width = 705
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.xls'
    Filter = '*.xls|*.xls|*.csv|*.csv'
    Left = 964
    Top = 168
  end
end
