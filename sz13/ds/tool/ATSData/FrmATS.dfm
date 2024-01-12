inherited FormATS: TFormATS
  Caption = 'ATS'#25968#25454#26174#31034
  ClientHeight = 687
  ClientWidth = 1180
  Position = poDesktopCenter
  WindowState = wsMaximized
  OnShow = FormShow
  ExplicitWidth = 1196
  ExplicitHeight = 726
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1180
    Height = 687
    ExplicitWidth = 1180
    ExplicitHeight = 687
    object tlATS: TcxTreeList
      Left = 825
      Top = 97
      Width = 355
      Height = 590
      Align = alClient
      Bands = <
        item
          Caption.Text = 'carinfo'
        end>
      LookAndFeel.NativeStyle = False
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsSelection.CellSelect = False
      OptionsView.GridLines = tlglBoth
      OptionsView.Indicator = True
      OptionsView.ShowRoot = False
      OptionsView.TreeLineStyle = tllsNone
      TabOrder = 0
      OnGetCellHint = tlATSGetCellHint
      object Column0: TcxTreeListColumn
        Caption.Text = #24207#21495
        DataBinding.ValueType = 'Integer'
        Width = 100
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object Column1: TcxTreeListColumn
        Caption.Text = #36710#20307#21495
        DataBinding.ValueType = 'Integer'
        Width = 90
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column2: TcxTreeListColumn
        Caption.Text = #26381#21153#21495
        DataBinding.ValueType = 'Integer'
        Width = 84
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column3: TcxTreeListColumn
        Caption.Text = #36710#27425#21495
        DataBinding.ValueType = 'Integer'
        Width = 100
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column10: TcxTreeListColumn
        Caption.Text = #30446#30340#22320
        DataBinding.ValueType = 'String'
        Width = 83
        Position.ColIndex = 4
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object Column4: TcxTreeListColumn
        Caption.Text = #39550#39542#21592
        DataBinding.ValueType = 'Integer'
        Width = 100
        Position.ColIndex = 5
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column5: TcxTreeListColumn
        Caption.Text = #32534#32452
        DataBinding.ValueType = 'Integer'
        Width = 82
        Position.ColIndex = 6
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column7: TcxTreeListColumn
        Caption.Text = #36710#31449
        DataBinding.ValueType = 'Integer'
        Width = 82
        Position.ColIndex = 7
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column6: TcxTreeListColumn
        Caption.Text = #19978#34892#31449
        DataBinding.ValueType = 'Integer'
        Width = 79
        Position.ColIndex = 8
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column8: TcxTreeListColumn
        Caption.Text = #19979#34892#31449
        DataBinding.ValueType = 'Integer'
        Width = 85
        Position.ColIndex = 9
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column9: TcxTreeListColumn
        Caption.Text = #26041#21521
        DataBinding.ValueType = 'Integer'
        Width = 79
        Position.ColIndex = 10
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column11: TcxTreeListColumn
        Caption.Text = #25240#36820
        DataBinding.ValueType = 'Integer'
        Width = 117
        Position.ColIndex = 11
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column13: TcxTreeListColumn
        Caption.Text = #31449#21488
        DataBinding.ValueType = 'Integer'
        Width = 127
        Position.ColIndex = 12
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column16: TcxTreeListColumn
        Caption.Text = #36716#25442#36712
        DataBinding.ValueType = 'Integer'
        Width = 100
        Position.ColIndex = 13
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column14: TcxTreeListColumn
        Caption.Text = #36827#20986#27491#32447
        DataBinding.ValueType = 'Integer'
        Width = 100
        Position.ColIndex = 14
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column12: TcxTreeListColumn
        Caption.Text = #22833#21435#20301#32622
        DataBinding.ValueType = 'Integer'
        Width = 87
        Position.ColIndex = 15
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
        OnGetDisplayText = tlATSGetDisplayText
      end
      object Column15: TcxTreeListColumn
        Caption.Text = #36712#36947
        DataBinding.ValueType = 'String'
        Width = 120
        Position.ColIndex = 16
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
    object cxSplitter1: TcxSplitter
      Left = 817
      Top = 97
      Width = 8
      Height = 590
      Control = PanelLeft
    end
    object pcMain: TcxPageControl
      Left = 0
      Top = 0
      Width = 1180
      Height = 97
      Align = alTop
      TabOrder = 2
      Properties.ActivePage = tsShowFile
      Properties.CustomButtons.Buttons = <>
      Properties.TabHeight = 28
      Properties.TabWidth = 150
      OnPageChanging = pcMainPageChanging
      ClientRectBottom = 93
      ClientRectLeft = 4
      ClientRectRight = 1176
      ClientRectTop = 34
      object tsRealData: TcxTabSheet
        Caption = #23454#26102#25968#25454
        ImageIndex = 1
        object Panel1: TPanel
          Left = 0
          Top = 0
          Width = 1172
          Height = 59
          Align = alClient
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 0
          object Label1: TLabel
            Left = 32
            Top = 23
            Width = 49
            Height = 14
            Caption = 'ATS'#22320#22336
          end
          object LabelATS: TcxLabel
            Left = 504
            Top = 23
            Caption = 'LabelInfo'
            ParentFont = False
            Style.Font.Charset = ANSI_CHARSET
            Style.Font.Color = clRed
            Style.Font.Height = -14
            Style.Font.Name = #23435#20307
            Style.Font.Style = []
            Style.IsFontAssigned = True
            Transparent = True
          end
          object edtATSAddr: TcxTextEdit
            Left = 96
            Top = 19
            TabOrder = 1
            Width = 157
          end
          object btnSet: TcxButton
            Left = 375
            Top = 19
            Width = 75
            Height = 25
            Caption = #30830#35748
            Colors.Default = clSkyBlue
            TabOrder = 2
            OnClick = btnSetClick
          end
          object cbPause: TcxCheckBox
            Left = 276
            Top = 19
            Caption = #26242#20572
            TabOrder = 3
            Transparent = True
          end
        end
      end
      object tsShowFile: TcxTabSheet
        Caption = #26597#30475#25991#20214
        ImageIndex = 0
        object Panel3: TPanel
          Left = 0
          Top = 0
          Width = 1172
          Height = 59
          Align = alClient
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 0
          object Label2: TLabel
            Left = 28
            Top = 26
            Width = 49
            Height = 14
            Caption = 'ATS'#25991#20214
          end
          object btnRefresh: TcxButton
            Left = 661
            Top = 22
            Width = 75
            Height = 25
            Caption = #26174#31034
            Colors.Default = clSkyBlue
            TabOrder = 0
            OnClick = btnRefreshClick
          end
          object LabelInfo: TcxLabel
            Left = 769
            Top = 25
            Caption = 'LabelInfo'
            ParentFont = False
            Style.Font.Charset = ANSI_CHARSET
            Style.Font.Color = clRed
            Style.Font.Height = -14
            Style.Font.Name = #23435#20307
            Style.Font.Style = []
            Style.IsFontAssigned = True
            Transparent = True
          end
          object edtFile: TcxButtonEdit
            Left = 96
            Top = 22
            Properties.Buttons = <
              item
                Default = True
                Kind = bkEllipsis
              end>
            Properties.OnButtonClick = edtFilePropertiesButtonClick
            TabOrder = 2
            Width = 552
          end
        end
      end
    end
    object PanelLeft: TPanel
      Left = 0
      Top = 97
      Width = 817
      Height = 590
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 3
      object tlRec: TcxTreeList
        Left = 0
        Top = 0
        Width = 817
        Height = 590
        Align = alClient
        Bands = <
          item
            Caption.Text = 'carinfo'
          end>
        LookAndFeel.NativeStyle = False
        Navigator.Buttons.CustomButtons = <>
        OptionsData.Editing = False
        OptionsSelection.CellSelect = False
        OptionsView.GridLines = tlglBoth
        OptionsView.Indicator = True
        OptionsView.ShowRoot = False
        OptionsView.TreeLineStyle = tllsNone
        TabOrder = 0
        OnFocusedNodeChanged = tlRecFocusedNodeChanged
        object tlRecColumn10: TcxTreeListColumn
          Visible = False
          Caption.Text = #24207#21495
          DataBinding.ValueType = 'LargeInt'
          Width = 53
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object cxTreeListColumn1: TcxTreeListColumn
          Caption.Text = #26102#38388
          DataBinding.ValueType = 'String'
          Width = 83
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object cxTreeListColumn2: TcxTreeListColumn
          Caption.Text = #22320#22336
          DataBinding.ValueType = 'String'
          Width = 94
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object cxTreeListColumn3: TcxTreeListColumn
          Caption.Text = #20107#21153#26631#35782
          DataBinding.ValueType = 'Integer'
          Width = 81
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
        object cxTreeListColumn4: TcxTreeListColumn
          Caption.Text = #21327#35758#26631#35782
          DataBinding.ValueType = 'Integer'
          Width = 80
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
        object cxTreeListColumn5: TcxTreeListColumn
          Caption.Text = #38271#24230
          DataBinding.ValueType = 'Integer'
          Width = 54
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
        object cxTreeListColumn6: TcxTreeListColumn
          Caption.Text = #21333#20803#26631#35782
          DataBinding.ValueType = 'Integer'
          Width = 79
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
        object tlRecColumn7: TcxTreeListColumn
          Caption.Text = #21151#33021#30721
          DataBinding.ValueType = 'Integer'
          Width = 67
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
        object tlRecColumn8: TcxTreeListColumn
          Caption.Text = #36215#22987#22320#22336
          DataBinding.ValueType = 'Integer'
          Width = 85
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
        object tlRecColumn9: TcxTreeListColumn
          Caption.Text = #23492#23384#22120#20010#25968
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 9
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = tlRecGetDisplayText
        end
      end
      object sgRec: TAdvStringGrid
        Left = 0
        Top = 0
        Width = 817
        Height = 590
        Cursor = crDefault
        Align = alClient
        ColCount = 11
        DrawingStyle = gdsClassic
        FixedCols = 0
        RowCount = 2
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
        ScrollBars = ssBoth
        TabOrder = 1
        HoverRowCells = [hcNormal, hcSelected]
        OnRowChanging = sgRecRowChanging
        OnClickCell = sgRecClickCell
        ActiveCellFont.Charset = DEFAULT_CHARSET
        ActiveCellFont.Color = clWindowText
        ActiveCellFont.Height = -11
        ActiveCellFont.Name = 'Tahoma'
        ActiveCellFont.Style = [fsBold]
        ControlLook.FixedGradientHoverFrom = clGray
        ControlLook.FixedGradientHoverTo = clWhite
        ControlLook.FixedGradientDownFrom = clGray
        ControlLook.FixedGradientDownTo = clSilver
        ControlLook.DropDownHeader.Font.Charset = DEFAULT_CHARSET
        ControlLook.DropDownHeader.Font.Color = clWindowText
        ControlLook.DropDownHeader.Font.Height = -11
        ControlLook.DropDownHeader.Font.Name = 'Tahoma'
        ControlLook.DropDownHeader.Font.Style = []
        ControlLook.DropDownHeader.Visible = True
        ControlLook.DropDownHeader.Buttons = <>
        ControlLook.DropDownFooter.Font.Charset = DEFAULT_CHARSET
        ControlLook.DropDownFooter.Font.Color = clWindowText
        ControlLook.DropDownFooter.Font.Height = -11
        ControlLook.DropDownFooter.Font.Name = 'Tahoma'
        ControlLook.DropDownFooter.Font.Style = []
        ControlLook.DropDownFooter.Visible = True
        ControlLook.DropDownFooter.Buttons = <>
        Filter = <>
        FilterDropDown.Font.Charset = DEFAULT_CHARSET
        FilterDropDown.Font.Color = clWindowText
        FilterDropDown.Font.Height = -11
        FilterDropDown.Font.Name = 'Tahoma'
        FilterDropDown.Font.Style = []
        FilterDropDown.TextChecked = 'Checked'
        FilterDropDown.TextUnChecked = 'Unchecked'
        FilterDropDownClear = '(All)'
        FilterEdit.TypeNames.Strings = (
          'Starts with'
          'Ends with'
          'Contains'
          'Not contains'
          'Equal'
          'Not equal'
          'Larger than'
          'Smaller than'
          'Clear')
        FixedColWidth = 62
        FixedRowHeight = 22
        FixedFont.Charset = ANSI_CHARSET
        FixedFont.Color = clWindowText
        FixedFont.Height = -15
        FixedFont.Name = #23435#20307
        FixedFont.Style = []
        FloatFormat = '%.2f'
        HoverButtons.Buttons = <>
        HoverButtons.Position = hbLeftFromColumnLeft
        HTMLSettings.ImageFolder = 'images'
        HTMLSettings.ImageBaseName = 'img'
        PrintSettings.DateFormat = 'dd/mm/yyyy'
        PrintSettings.Font.Charset = DEFAULT_CHARSET
        PrintSettings.Font.Color = clWindowText
        PrintSettings.Font.Height = -11
        PrintSettings.Font.Name = 'Tahoma'
        PrintSettings.Font.Style = []
        PrintSettings.FixedFont.Charset = DEFAULT_CHARSET
        PrintSettings.FixedFont.Color = clWindowText
        PrintSettings.FixedFont.Height = -11
        PrintSettings.FixedFont.Name = 'Tahoma'
        PrintSettings.FixedFont.Style = []
        PrintSettings.HeaderFont.Charset = DEFAULT_CHARSET
        PrintSettings.HeaderFont.Color = clWindowText
        PrintSettings.HeaderFont.Height = -11
        PrintSettings.HeaderFont.Name = 'Tahoma'
        PrintSettings.HeaderFont.Style = []
        PrintSettings.FooterFont.Charset = DEFAULT_CHARSET
        PrintSettings.FooterFont.Color = clWindowText
        PrintSettings.FooterFont.Height = -11
        PrintSettings.FooterFont.Name = 'Tahoma'
        PrintSettings.FooterFont.Style = []
        PrintSettings.PageNumSep = '/'
        SearchFooter.FindNextCaption = 'Find &next'
        SearchFooter.FindPrevCaption = 'Find &previous'
        SearchFooter.Font.Charset = DEFAULT_CHARSET
        SearchFooter.Font.Color = clWindowText
        SearchFooter.Font.Height = -11
        SearchFooter.Font.Name = 'Tahoma'
        SearchFooter.Font.Style = []
        SearchFooter.HighLightCaption = 'Highlight'
        SearchFooter.HintClose = 'Close'
        SearchFooter.HintFindNext = 'Find next occurrence'
        SearchFooter.HintFindPrev = 'Find previous occurrence'
        SearchFooter.HintHighlight = 'Highlight occurrences'
        SearchFooter.MatchCaseCaption = 'Match case'
        SearchFooter.ResultFormat = '(%d of %d)'
        ShowDesignHelper = False
        SortSettings.DefaultFormat = ssAutomatic
        Version = '8.4.7.0'
        ColWidths = (
          62
          84
          108
          81
          80
          54
          79
          69
          87
          97
          64)
      end
    end
    object cbShowHex: TcxCheckBox
      Left = 928
      Top = 6
      Caption = #26174#31034'16'#36827#21046#25968
      Properties.OnChange = cbShowHexPropertiesChange
      TabOrder = 4
    end
    object cbFilterRec: TcxCheckBox
      Left = 756
      Top = 6
      Caption = #36807#28388#37325#22797#35760#24405
      State = cbsChecked
      TabOrder = 5
      Transparent = True
    end
    object cb2881: TcxCheckBox
      Left = 600
      Top = 6
      Caption = '2881'
      TabOrder = 6
      Transparent = True
      Visible = False
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    SkinName = 'Lilian'
    Left = 648
    Top = 324
  end
  object TimerATS: TTimer
    Interval = 2000
    OnTimer = TimerATSTimer
    Left = 644
    Top = 216
  end
  object OpenDialog: TFileOpenDialog
    DefaultExtension = '*.dat'
    FavoriteLinks = <>
    FileTypes = <
      item
        DisplayName = ''
      end>
    Options = []
    Left = 260
    Top = 312
  end
end
