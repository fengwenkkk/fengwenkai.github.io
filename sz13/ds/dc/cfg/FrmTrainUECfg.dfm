inherited FormTrainUECfg: TFormTrainUECfg
  ClientHeight = 429
  ClientWidth = 931
  OnCreate = FormCreate
  ExplicitWidth = 947
  ExplicitHeight = 468
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 931
    Height = 429
    ExplicitWidth = 931
    ExplicitHeight = 429
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #36710#36733#21488
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      Height = 429
      Width = 931
      object tlMain: TcxTreeList
        Left = 2
        Top = 19
        Width = 927
        Height = 356
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
        object Column1: TcxTreeListColumn
          Caption.Text = #32456#31471#21495#30721
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object Column2: TcxTreeListColumn
          Caption.Text = #32456#31471#21517#31216
          DataBinding.ValueType = 'String'
          Width = 135
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object Column3: TcxTreeListColumn
          Caption.Text = #20027#26426#22320#22336
          DataBinding.ValueType = 'String'
          Width = 162
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object Column4: TcxTreeListColumn
          Caption.Text = #36710#20307#21495
          DataBinding.ValueType = 'Integer'
          Width = 149
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = Column4GetDisplayText
        end
        object Column5: TcxTreeListColumn
          Visible = False
          Caption.Text = #36890#35805#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 138
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
      object gbCtrl: TcxGroupBox
        Left = 2
        Top = 375
        Align = alBottom
        Caption = ' '
        Ctl3D = False
        PanelStyle.Active = True
        ParentCtl3D = False
        Style.BorderColor = clRed
        Style.BorderStyle = ebs3D
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Black'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Black'
        TabOrder = 1
        DesignSize = (
          927
          52)
        Height = 52
        Width = 927
        object btnDel: TcxButton
          Left = 105
          Top = 13
          Width = 75
          Height = 25
          Caption = #21024#38500
          LookAndFeel.NativeStyle = False
          TabOrder = 1
          OnClick = btnDelClick
        end
        object btnAdd: TcxButton
          Left = 15
          Top = 13
          Width = 75
          Height = 25
          Caption = #22686#21152
          LookAndFeel.NativeStyle = False
          TabOrder = 0
          OnClick = btnAddClick
        end
        object btnRefresh: TcxButton
          Left = 283
          Top = 13
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #21047#26032
          LookAndFeel.NativeStyle = False
          TabOrder = 3
          OnClick = btnRefreshClick
        end
        object btnSet: TcxButton
          Left = 195
          Top = 13
          Width = 75
          Height = 25
          Caption = #20462#25913
          LookAndFeel.NativeStyle = False
          TabOrder = 2
          OnClick = btnSetClick
        end
      end
    end
  end
end
