inherited FormTrainCfg: TFormTrainCfg
  ClientHeight = 390
  ClientWidth = 821
  OnCreate = FormCreate
  ExplicitWidth = 837
  ExplicitHeight = 429
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 821
    Height = 390
    ExplicitWidth = 821
    ExplicitHeight = 390
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #21015#36710
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      Height = 390
      Width = 821
      object tlMain: TcxTreeList
        Left = 2
        Top = 19
        Width = 481
        Height = 317
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
        object ColumnTrainUnitID: TcxTreeListColumn
          Caption.Text = #36710#20307#21495
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTrainUnitIDGetDisplayText
        end
        object ColumnTrainType: TcxTreeListColumn
          Caption.Text = #31867#22411
          DataBinding.ValueType = 'Integer'
          Width = 135
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTrainTypeGetDisplayText
        end
        object tlMainColumn7: TcxTreeListColumn
          Caption.Text = #36710#36733#21488'1'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn8: TcxTreeListColumn
          Caption.Text = #36710#36733#21488'2'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn3: TcxTreeListColumn
          Caption.Text = #21015#36710#36890#35805#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn4: TcxTreeListColumn
          Visible = False
          Caption.Text = #36710#36742#27573#21015#36710#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn5: TcxTreeListColumn
          Visible = False
          Caption.Text = #21015#36710#24191#25773#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn9: TcxTreeListColumn
          Visible = False
          Caption.Text = #21019#24314#32773
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn6: TcxTreeListColumn
          Visible = False
          Caption.Text = #29366#24577
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
      object gbCtrl: TcxGroupBox
        Left = 2
        Top = 336
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
          817
          52)
        Height = 52
        Width = 817
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
          Left = 285
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
        object btnImport: TcxButton
          Left = 375
          Top = 13
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #23548#20837
          LookAndFeel.NativeStyle = False
          TabOrder = 4
          Visible = False
          OnClick = btnImportClick
        end
      end
      object mmESDKGroup: TcxMemo
        Left = 483
        Top = 19
        Align = alRight
        Lines.Strings = (
          'mmESDKGroup')
        Properties.ScrollBars = ssBoth
        TabOrder = 2
        Visible = False
        Height = 317
        Width = 336
      end
    end
  end
  object OpenDialog: TOpenDialog
    Left = 200
    Top = 184
  end
  object TimerCheck: TTimer
    Interval = 10000
    Left = 416
    Top = 192
  end
end
