inherited FormEmergencyCallInfo: TFormEmergencyCallInfo
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
      object pcMain: TcxPageControl
        Left = 3
        Top = 3
        Width = 1228
        Height = 501
        Align = alClient
        TabOrder = 0
        Properties.ActivePage = tsP2P
        Properties.CustomButtons.Buttons = <>
        Properties.TabWidth = 120
        LookAndFeel.SkinName = 'Office2010Blue'
        ClientRectBottom = 495
        ClientRectLeft = 2
        ClientRectRight = 1222
        ClientRectTop = 28
        object tsP2P: TcxTabSheet
          Caption = #20010#21628
          ImageIndex = 0
          object tlP2PCall: TcxTreeList
            Left = 0
            Top = 0
            Width = 1220
            Height = 467
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
            object cxTreeListColumn7: TcxTreeListColumn
              Caption.Text = #26102#38388
              DataBinding.ValueType = 'String'
              Width = 143
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn9: TcxTreeListColumn
              Caption.Text = #21628#21483#31867#21035
              DataBinding.ValueType = 'Integer'
              Width = 112
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = cxTreeListColumn9GetDisplayText
            end
            object cxTreeListColumn8: TcxTreeListColumn
              Caption.Text = #20027#21483
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn10: TcxTreeListColumn
              Caption.Text = #34987#21483
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlP2PColumn5: TcxTreeListColumn
              Caption.Text = #29366#24577
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlP2PColumn5GetDisplayText
            end
          end
        end
        object tsPTT: TcxTabSheet
          Caption = #32452#21628
          ImageIndex = 1
          object tlGroupCall: TcxTreeList
            Left = 0
            Top = 0
            Width = 1220
            Height = 467
            BorderStyle = cxcbsNone
            Align = alClient
            Bands = <
              item
              end>
            LookAndFeel.NativeStyle = False
            LookAndFeel.SkinName = 'Office2010Blue'
            Navigator.Buttons.CustomButtons = <>
            OptionsData.Editing = False
            OptionsView.GridLines = tlglBoth
            OptionsView.Indicator = True
            OptionsView.ShowRoot = False
            TabOrder = 0
            object cxTreeListColumn2: TcxTreeListColumn
              Caption.Text = #26102#38388
              DataBinding.ValueType = 'String'
              Width = 143
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn4: TcxTreeListColumn
              Caption.Text = #21628#21483#31867#21035
              DataBinding.ValueType = 'Integer'
              Width = 111
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = cxTreeListColumn4GetDisplayText
            end
            object tlGroupCallColumn5: TcxTreeListColumn
              Caption.Text = #36890#35805#32452#21495#30721
              DataBinding.ValueType = 'LargeInt'
              Width = 100
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn3: TcxTreeListColumn
              Caption.Text = #36890#35805#32452#21517#31216
              DataBinding.ValueType = 'String'
              Width = 128
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn5: TcxTreeListColumn
              Caption.Text = #35805#26435#20154
              DataBinding.ValueType = 'Integer'
              Width = 102
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
        end
        object tsEmergencyCall: TcxTabSheet
          Caption = #32039#24613#21628#21483
          ImageIndex = 3
        end
        object tsBrd: TcxTabSheet
          Caption = #20154#24037#24191#25773
          ImageIndex = 4
        end
        object tsHistory: TcxTabSheet
          Caption = #21382#21490#35760#24405
          ImageIndex = 2
          TabVisible = False
          object tlMain: TcxTreeList
            Left = 0
            Top = 100
            Width = 1220
            Height = 367
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
              Caption.Text = #32534#21495
              DataBinding.ValueType = 'Integer'
              Width = 94
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object Column2: TcxTreeListColumn
              Caption.Text = #26102#38388
              DataBinding.ValueType = 'String'
              Width = 143
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = Column2GetDisplayText
            end
            object tlMainColumn5: TcxTreeListColumn
              Caption.Text = #21628#21483#31867#21035
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlMainColumn5GetDisplayText
            end
            object tlMainColumn9: TcxTreeListColumn
              Visible = False
              Caption.Text = #36890#35805#32452#21495#30721
              DataBinding.ValueType = 'LargeInt'
              Width = 100
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlMainColumn3: TcxTreeListColumn
              Visible = False
              Caption.Text = #36890#35805#32452#21517#31216
              DataBinding.ValueType = 'String'
              Width = 139
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlMainColumn6: TcxTreeListColumn
              Visible = False
              Caption.Text = #35805#26435#20154
              DataBinding.ValueType = 'Integer'
              Width = 102
              Position.ColIndex = 5
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlMainColumn7: TcxTreeListColumn
              Caption.Text = #20027#21483
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 6
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlMainColumn8: TcxTreeListColumn
              Caption.Text = #34987#21483
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 7
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
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
            TabOrder = 1
            Height = 100
            Width = 1220
            object btnQuery: TcxButton
              Left = 484
              Top = 18
              Width = 75
              Height = 25
              Caption = #26597#35810
              Default = True
              LookAndFeel.NativeStyle = False
              TabOrder = 8
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
              Left = 16
              Top = 60
              Caption = #32467#26463#26102#38388
              Transparent = True
            end
            object deEndTime: TcxDateEdit
              Left = 88
              Top = 58
              EditValue = 43901d
              Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
              Properties.EditFormat = 'YYYY-MM-DD HH:mm'
              Properties.InputKind = ikRegExpr
              Properties.Kind = ckDateTime
              TabOrder = 1
              Width = 142
            end
            object cxLabel3: TcxLabel
              Left = 265
              Top = 21
              Caption = #31867#21035
              Transparent = True
            end
            object cxLabel4: TcxLabel
              Left = 264
              Top = 60
              Caption = #21495#30721
              Transparent = True
            end
            object cbCDRType: TcxComboBox
              Left = 311
              Top = 19
              Properties.DropDownListStyle = lsFixedList
              TabOrder = 2
              Width = 122
            end
            object edtResourceID: TcxTextEdit
              Left = 311
              Top = 58
              TabOrder = 3
              Width = 122
            end
            object btnNext: TcxButton
              Left = 589
              Top = 18
              Width = 75
              Height = 25
              Caption = #24448#21518
              Enabled = False
              LookAndFeel.NativeStyle = False
              TabOrder = 9
              OnClick = btnNextClick
            end
            object btnPrev: TcxButton
              Left = 698
              Top = 18
              Width = 75
              Height = 25
              Caption = #24448#21069
              Enabled = False
              LookAndFeel.NativeStyle = False
              TabOrder = 10
              OnClick = btnPrevClick
            end
            object btnExport: TcxButton
              Left = 589
              Top = 57
              Width = 75
              Height = 25
              Caption = #23548#20986
              Enabled = False
              LookAndFeel.NativeStyle = False
              TabOrder = 11
              OnClick = btnExportClick
            end
            object btnPrint: TcxButton
              Left = 698
              Top = 57
              Width = 75
              Height = 25
              Caption = #25171#21360
              Enabled = False
              LookAndFeel.NativeStyle = False
              TabOrder = 12
              OnClick = btnPrintClick
            end
          end
        end
      end
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.xls'
    Filter = '*.xls|*.xls|*.csv|*.csv'
    Left = 352
    Top = 176
  end
  object TimerInit: TTimer
    OnTimer = TimerInitTimer
    Left = 191
    Top = 308
  end
end
