inherited FormDCUser: TFormDCUser
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
      Caption = #35843#24230#21488
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      Height = 429
      Width = 931
      object tlMain: TcxTreeList
        Left = 3
        Top = 16
        Width = 925
        Height = 354
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
        object cxTreeListColumn10: TcxTreeListColumn
          PropertiesClassName = 'TcxComboBoxProperties'
          Properties.Items.Strings = (
            #25152#26377#29992#25143)
          Visible = False
          Caption.Text = #24207#21495
          DataBinding.ValueType = 'Integer'
          Options.Editing = False
          Width = 58
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          SortOrder = soAscending
          SortIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlCallInColumnCallType: TcxTreeListColumn
          Caption.Text = #35843#24230#21488#21495#30721
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object cxTreeListColumn11: TcxTreeListColumn
          Caption.Text = #35843#24230#21488#21517#31216
          DataBinding.ValueType = 'String'
          Width = 132
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlP2PCallColumn1: TcxTreeListColumn
          Visible = False
          Caption.Text = #35843#24230#21488#26435#38480
          DataBinding.ValueType = 'String'
          Width = 115
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlP2PCallColumn1GetDisplayText
        end
        object ColumnZoneRange: TcxTreeListColumn
          Caption.Text = #31649#29702#21306#22495
          DataBinding.ValueType = 'String'
          Width = 414
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnZoneRangeGetDisplayText
        end
      end
      object gbCtrl: TcxGroupBox
        Left = 3
        Top = 370
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
          925
          52)
        Height = 52
        Width = 925
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
end
