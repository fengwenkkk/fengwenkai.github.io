inherited FormPredefinedSMSCfg: TFormPredefinedSMSCfg
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
      Caption = #32456#31471#39044#23450#20041#30701#28040#24687
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
        object ColumnSMSID: TcxTreeListColumn
          Caption.Text = #32534#21495
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSDSType: TcxTreeListColumn
          Caption.Text = #27169#26495#31867#22411
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnSDSTypeGetDisplayText
        end
        object ColumnTemplateType: TcxTreeListColumn
          Caption.Text = #27169#26495#33539#22260
          DataBinding.ValueType = 'Integer'
          Width = 130
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTemplateTypeGetDisplayText
        end
        object ColumnInfo: TcxTreeListColumn
          Caption.Text = #30701#28040#24687#20869#23481
          DataBinding.ValueType = 'String'
          Width = 587
          Position.ColIndex = 3
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
          Left = 104
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
          Top = 14
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
  object OpenDialog: TOpenDialog
    Left = 200
    Top = 184
  end
end
