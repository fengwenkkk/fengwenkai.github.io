inherited FormATS: TFormATS
  Caption = 'ATS(SZ14) '
  ClientHeight = 687
  ClientWidth = 1329
  Position = poDesktopCenter
  ExplicitLeft = -244
  ExplicitWidth = 1345
  ExplicitHeight = 726
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1329
    Height = 687
    Visible = False
    ExplicitWidth = 1329
    ExplicitHeight = 687
  end
  object gbMain: TcxGroupBox
    Left = 0
    Top = 0
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alClient
    PanelStyle.Active = True
    Style.BorderStyle = ebs3D
    TabOrder = 1
    Height = 687
    Width = 1329
    object pcMain: TcxPageControl
      Left = 3
      Top = 3
      Width = 1323
      Height = 681
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alClient
      TabOrder = 0
      Properties.ActivePage = tsATSFile
      Properties.CustomButtons.Buttons = <>
      Properties.TabHeight = 28
      Properties.TabWidth = 150
      LookAndFeel.NativeStyle = False
      ClientRectBottom = 675
      ClientRectLeft = 2
      ClientRectRight = 1317
      ClientRectTop = 37
      object tsATSData: TcxTabSheet
        Caption = #25968#25454#35774#32622
        ImageIndex = 2
        object Panel1: TPanel
          Left = 0
          Top = 0
          Width = 1315
          Height = 142
          Align = alTop
          BevelInner = bvRaised
          BevelOuter = bvLowered
          TabOrder = 0
          object Panel2: TPanel
            Left = 2
            Top = 2
            Width = 1311
            Height = 79
            Align = alClient
            BevelInner = bvRaised
            BevelOuter = bvLowered
            ParentColor = True
            TabOrder = 0
            object Label1: TLabel
              Left = 221
              Top = 16
              Width = 42
              Height = 14
              Caption = #24403#21069#31449
            end
            object Label2: TLabel
              Left = 221
              Top = 46
              Width = 42
              Height = 14
              Caption = #26381#21153#21495
            end
            object Label3: TLabel
              Left = 435
              Top = 16
              Width = 42
              Height = 14
              Caption = #30446#30340#22320
            end
            object Label4: TLabel
              Left = 643
              Top = 16
              Width = 28
              Height = 14
              Caption = #26041#21521
            end
            object Label5: TLabel
              Left = 13
              Top = 16
              Width = 42
              Height = 14
              Caption = #36710#20307#21495
            end
            object Label6: TLabel
              Left = 13
              Top = 46
              Width = 42
              Height = 14
              Caption = #36710#27425#21495
            end
            object Label7: TLabel
              Left = 435
              Top = 46
              Width = 42
              Height = 14
              Caption = #24207#21015#21495
            end
            object Label8: TLabel
              Left = 883
              Top = 16
              Width = 42
              Height = 14
              Caption = #29289#29702#21306
            end
            object Label9: TLabel
              Left = 869
              Top = 46
              Width = 56
              Height = 14
              Caption = #21306#20869#24207#21495
            end
            object Label10: TLabel
              Left = 643
              Top = 46
              Width = 28
              Height = 14
              Caption = #20301#32622
            end
            object cbTransferTrack: TcxCheckBox
              Left = 688
              Top = 44
              Caption = #36716#25442#36712
              Properties.ReadOnly = False
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 5
            end
            object cbTurnbackTrack: TcxCheckBox
              Left = 781
              Top = 44
              Caption = #25240#36820#36712
              Properties.ReadOnly = False
              Properties.OnEditValueChanged = cbTurnbackTrackPropertiesEditValueChanged
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 6
            end
            object edtCurrStation: TcxTextEdit
              Left = 281
              Top = 12
              Properties.ReadOnly = False
              Properties.OnEditValueChanged = edtCurrStationPropertiesEditValueChanged
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 2
              Width = 121
            end
            object edtDestID: TcxTextEdit
              Left = 495
              Top = 12
              Properties.ReadOnly = False
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 4
              Width = 121
            end
            object edtServiceNumber: TcxTextEdit
              Left = 281
              Top = 42
              Properties.MaxLength = 3
              Properties.ReadOnly = False
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 3
              Width = 121
            end
            object edtTrainID: TcxTextEdit
              Left = 73
              Top = 42
              Properties.ReadOnly = True
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 1
              Width = 121
            end
            object edtTrainUnitID: TcxTextEdit
              Left = 73
              Top = 12
              Properties.ReadOnly = True
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 0
              Width = 121
            end
            object edtOrderNumber: TcxTextEdit
              Left = 495
              Top = 42
              Properties.MaxLength = 4
              Properties.ReadOnly = False
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 7
              Width = 121
            end
            object btnInit: TcxButton
              Left = 1127
              Top = 41
              Width = 75
              Height = 25
              Caption = #21021#22987#21270
              LookAndFeel.NativeStyle = False
              TabOrder = 8
              OnClick = btnInitClick
            end
            object rbUpSide: TcxRadioButton
              Left = 688
              Top = 15
              Width = 49
              Height = 17
              Caption = #19978#34892
              Checked = True
              TabOrder = 9
              TabStop = True
              OnClick = rbUpSideClick
              GroupIndex = 10
            end
            object rbDownSide: TcxRadioButton
              Left = 781
              Top = 15
              Width = 49
              Height = 17
              Caption = #19979#34892
              TabOrder = 10
              OnClick = rbUpSideClick
              GroupIndex = 10
            end
            object edtIndexInSection: TcxTextEdit
              Left = 943
              Top = 42
              Properties.ReadOnly = False
              Style.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              TabOrder = 11
              Width = 121
            end
            object cbSection: TcxComboBox
              Left = 943
              Top = 14
              Properties.Sorted = True
              TabOrder = 12
              Text = 'cbSection'
              Width = 121
            end
          end
          object Panel3: TPanel
            Left = 2
            Top = 81
            Width = 1311
            Height = 59
            Align = alBottom
            BevelInner = bvRaised
            BevelOuter = bvLowered
            TabOrder = 1
            object btnDeposit: TcxButton
              Left = 31
              Top = 18
              Width = 75
              Height = 25
              Caption = #20837#24211
              TabOrder = 5
              OnClick = btnDepositClick
            end
            object btnDepart: TcxButton
              Left = 31
              Top = 18
              Width = 75
              Height = 25
              Caption = #20986#24211
              Colors.Default = clSkyBlue
              LookAndFeel.NativeStyle = False
              TabOrder = 0
              OnClick = btnDepartClick
            end
            object btnTransferTrack: TcxButton
              Left = 137
              Top = 18
              Width = 75
              Height = 25
              Caption = #20837#36716#25442#36712
              LookAndFeel.NativeStyle = False
              TabOrder = 1
              OnClick = btnTransferTrackClick
            end
            object btnNextStation: TcxButton
              Left = 327
              Top = 18
              Width = 75
              Height = 25
              Caption = #19979#19968#31449
              LookAndFeel.NativeStyle = False
              TabOrder = 2
              OnClick = btnNextStationClick
            end
            object btnTurnBack: TcxButton
              Left = 427
              Top = 18
              Width = 75
              Height = 25
              Caption = #25240#36820
              LookAndFeel.NativeStyle = False
              TabOrder = 3
              OnClick = btnTurnBackClick
            end
            object btnRefresh: TcxButton
              Left = 731
              Top = 18
              Width = 75
              Height = 25
              Caption = #21047#26032
              Colors.Default = clSkyBlue
              LookAndFeel.NativeStyle = False
              TabOrder = 4
              OnClick = btnRefreshClick
            end
            object btnLeaveStation: TcxButton
              Left = 542
              Top = 18
              Width = 75
              Height = 25
              Caption = #20986#31449
              LookAndFeel.NativeStyle = False
              TabOrder = 6
              OnClick = btnLeaveStationClick
            end
            object btnEnterStation: TcxButton
              Left = 623
              Top = 18
              Width = 75
              Height = 25
              Caption = #20837#31449
              LookAndFeel.NativeStyle = False
              TabOrder = 7
              OnClick = btnEnterStationClick
            end
            object btnLeaveTransferTrack: TcxButton
              Left = 218
              Top = 18
              Width = 75
              Height = 25
              Caption = #20986#36716#25442#36712
              LookAndFeel.NativeStyle = False
              TabOrder = 8
              OnClick = btnLeaveTransferTrackClick
            end
            object btnSelectAll: TcxButton
              Left = 973
              Top = 18
              Width = 75
              Height = 25
              Caption = #20840#36873
              LookAndFeel.NativeStyle = False
              TabOrder = 9
              OnClick = btnSelectAllClick
            end
            object btnUnSelectAll: TcxButton
              Left = 1085
              Top = 18
              Width = 75
              Height = 25
              Caption = #20840#19981#36873
              LookAndFeel.NativeStyle = False
              TabOrder = 10
              OnClick = btnUnSelectAllClick
            end
          end
        end
        object tlTrain: TcxTreeList
          Left = 0
          Top = 142
          Width = 1315
          Height = 496
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
          TabOrder = 1
          OnFocusedNodeChanged = tlTrainFocusedNodeChanged
          object ColumnTrainUnitID: TcxTreeListColumn
            Caption.Text = #36710#20307#21495
            DataBinding.ValueType = 'Integer'
            Width = 90
            Position.ColIndex = 0
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnServiceNumber: TcxTreeListColumn
            Caption.Text = #26381#21153#21495
            DataBinding.ValueType = 'String'
            Width = 84
            Position.ColIndex = 1
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnOrderNumber: TcxTreeListColumn
            Caption.Text = #21015#36710#24207#21015#21495
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 2
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnUpsideStation: TcxTreeListColumn
            Caption.Text = #19978#34892#31449
            DataBinding.ValueType = 'Integer'
            Width = 79
            Position.ColIndex = 3
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnStation: TcxTreeListColumn
            Caption.Text = #24403#21069#31449
            DataBinding.ValueType = 'Integer'
            Width = 82
            Position.ColIndex = 4
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnStationGetDisplayText
          end
          object ColumnDownsideStation: TcxTreeListColumn
            Caption.Text = #19979#34892#31449
            DataBinding.ValueType = 'Integer'
            Width = 85
            Position.ColIndex = 5
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnDirection: TcxTreeListColumn
            Caption.Text = #26041#21521
            DataBinding.ValueType = 'Integer'
            Width = 79
            Position.ColIndex = 6
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnDirectionGetDisplayText
          end
          object ColumnDest: TcxTreeListColumn
            Caption.Text = #30446#30340#22320
            DataBinding.ValueType = 'String'
            Width = 83
            Position.ColIndex = 7
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnTurnbackTrackFlag: TcxTreeListColumn
            Caption.Text = #22312#25240#36820#36712
            DataBinding.ValueType = 'Integer'
            Width = 117
            Position.ColIndex = 8
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnTurnbackTrackFlagGetDisplayText
          end
          object ColumnTransferTrackFlag: TcxTreeListColumn
            Caption.Text = #22312#36716#25442#36712
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 9
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnTransferTrackFlagGetDisplayText
          end
          object ColumnLostPos: TcxTreeListColumn
            Caption.Text = #22833#21435#20301#32622
            DataBinding.ValueType = 'Integer'
            Width = 87
            Position.ColIndex = 10
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnPlatformID: TcxTreeListColumn
            Caption.Text = #25152#22312#31449#21488#32534#21495
            DataBinding.ValueType = 'Integer'
            Width = 127
            Position.ColIndex = 11
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnInOutDepot: TcxTreeListColumn
            Caption.Text = #26159#21542#36827#20986#32447
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 12
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnSectionID: TcxTreeListColumn
            Caption.Text = #29289#29702#21306
            DataBinding.ValueType = 'Integer'
            Width = 109
            Position.ColIndex = 13
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnSectionIDGetDisplayText
          end
          object ColumnIndexInSection: TcxTreeListColumn
            Caption.Text = #21306#20869#24207#21495
            DataBinding.ValueType = 'Integer'
            Width = 80
            Position.ColIndex = 14
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
        end
      end
      object tsATSFile: TcxTabSheet
        Caption = #21382#21490#25991#20214
        ImageIndex = 0
        object Panel5: TPanel
          Left = 0
          Top = 0
          Width = 1315
          Height = 50
          Align = alTop
          BevelInner = bvRaised
          BevelOuter = bvLowered
          ParentColor = True
          TabOrder = 0
          object Label12: TLabel
            Left = 28
            Top = 18
            Width = 49
            Height = 14
            Caption = 'ATS'#25991#20214
          end
          object btnLoadFile: TcxButton
            Left = 661
            Top = 13
            Width = 75
            Height = 25
            Caption = #26174#31034
            Colors.Default = clSkyBlue
            TabOrder = 0
            OnClick = btnLoadFileClick
          end
          object LabelInfo: TcxLabel
            Left = 748
            Top = 18
            Caption = 'LabelInfo'
            ParentFont = False
            Style.Font.Charset = ANSI_CHARSET
            Style.Font.Color = clRed
            Style.Font.Height = -14
            Style.Font.Name = #23435#20307
            Style.Font.Style = []
            Style.IsFontAssigned = True
            Transparent = True
            Visible = False
          end
          object edtFile: TcxButtonEdit
            Left = 96
            Top = 14
            Properties.Buttons = <
              item
                Default = True
                Kind = bkEllipsis
              end>
            Properties.OnButtonClick = edtFilePropertiesButtonClick
            TabOrder = 2
            Width = 552
          end
          object cbPlayRec: TcxCheckBox
            Left = 864
            Top = 16
            Caption = #22238#25918#38388#38548'(s)'
            Properties.ReadOnly = False
            Properties.OnEditValueChanged = cbPlayRecPropertiesEditValueChanged
            Style.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleFocused.LookAndFeel.NativeStyle = False
            StyleHot.LookAndFeel.NativeStyle = False
            TabOrder = 3
          end
          object cbFilterRec: TcxCheckBox
            Left = 1221
            Top = 13
            Caption = #36807#28388#37325#22797#35760#24405
            State = cbsChecked
            TabOrder = 4
            Transparent = True
            Visible = False
          end
          object cbShowHex: TcxCheckBox
            Left = 1094
            Top = 16
            Caption = #26174#31034'16'#36827#21046#25968
            Properties.OnChange = cbShowHexPropertiesChange
            TabOrder = 5
          end
          object edtPlayPeriod: TcxSpinEdit
            Left = 969
            Top = 14
            Properties.MaxValue = 30.000000000000000000
            Properties.MinValue = -1.000000000000000000
            TabOrder = 6
            Width = 56
          end
        end
        object PanelLeft: TPanel
          Left = 0
          Top = 50
          Width = 1315
          Height = 588
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 1
          object cxSplitter1: TcxSplitter
            Left = 742
            Top = 0
            Width = 6
            Height = 588
            Control = tlRec
          end
          object tlATS: TcxTreeList
            Left = 748
            Top = 0
            Width = 567
            Height = 588
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
            TabOrder = 1
            OnGetCellHint = tlATSGetCellHint
            object Column0: TcxTreeListColumn
              Caption.Text = #24207#21495
              DataBinding.ValueType = 'Integer'
              Width = 52
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlATSGetDisplayText
            end
            object Column1: TcxTreeListColumn
              Caption.Text = #36710#20307#21495
              DataBinding.ValueType = 'String'
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
              DataBinding.ValueType = 'String'
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
              DataBinding.ValueType = 'String'
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
              OnGetDisplayText = tlATSGetDisplayText
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
              Caption.Text = #29289#29702#21306
              DataBinding.ValueType = 'String'
              Width = 120
              Position.ColIndex = 16
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlATSGetDisplayText
            end
            object tlATSColumn18: TcxTreeListColumn
              Caption.Text = #36923#36753#21306
              DataBinding.ValueType = 'String'
              Width = 100
              Position.ColIndex = 17
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlATSGetDisplayText
            end
            object tlATSColumn19: TcxTreeListColumn
              Caption.Text = #21306#20869#39034#24207
              DataBinding.ValueType = 'String'
              Width = 100
              Position.ColIndex = 18
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlATSGetDisplayText
            end
            object tlATSColumn20: TcxTreeListColumn
              Caption.Text = #39550#39542#27169#24335
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 19
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlATSGetDisplayText
            end
          end
          object tlRec: TcxTreeList
            Left = 0
            Top = 0
            Width = 742
            Height = 588
            Align = alLeft
            Bands = <
              item
              end>
            Navigator.Buttons.CustomButtons = <>
            TabOrder = 2
            Visible = False
            OnClick = tlRecClick
            OnFocusedNodeChanged = tlRecFocusedNodeChanged
            object cxTreeList1Column1: TcxTreeListColumn
              Caption.Text = #24207#21495
              DataBinding.ValueType = 'String'
              Width = 65
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column2: TcxTreeListColumn
              Caption.Text = #25509#25910#26102#38388
              DataBinding.ValueType = 'String'
              Width = 100
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column3: TcxTreeListColumn
              Caption.Text = #22320#22336
              DataBinding.ValueType = 'String'
              Width = 100
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column4: TcxTreeListColumn
              Caption.Text = #21015#36710#25968
              DataBinding.ValueType = 'String'
              Width = 52
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column5: TcxTreeListColumn
              Caption.Text = #28040#24687#38271#24230
              DataBinding.ValueType = 'String'
              Width = 69
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column6: TcxTreeListColumn
              Caption.Text = #21457#36865#26102#38388
              DataBinding.ValueType = 'String'
              Width = 79
              Position.ColIndex = 5
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column7: TcxTreeListColumn
              Caption.Text = #32447#36335#21495
              DataBinding.ValueType = 'String'
              Width = 53
              Position.ColIndex = 6
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column8: TcxTreeListColumn
              Caption.Text = #28040#24687'ID'
              DataBinding.ValueType = 'String'
              Width = 57
              Position.ColIndex = 7
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column9: TcxTreeListColumn
              Caption.Text = #29256#26412#21495
              DataBinding.ValueType = 'String'
              Width = 55
              Position.ColIndex = 8
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeList1Column10: TcxTreeListColumn
              Caption.Text = #35760#24405#32034#24341
              DataBinding.ValueType = 'Integer'
              Width = 73
              Position.ColIndex = 9
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
        end
      end
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    SkinName = 'Office2010Blue'
    Left = 712
    Top = 188
  end
  object TimerRefresh: TTimer
    OnTimer = TimerRefreshTimer
    Left = 480
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
  object TimerSaveATSRec: TTimer
    Interval = 3000
    OnTimer = TimerSaveATSRecTimer
    Left = 232
    Top = 204
  end
end
