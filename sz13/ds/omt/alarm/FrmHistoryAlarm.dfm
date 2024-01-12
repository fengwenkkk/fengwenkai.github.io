inherited FormHistoryAlarm: TFormHistoryAlarm
  Top = 457
  BorderStyle = bsNone
  Caption = 'FormHistoryAlarm'
  ClientHeight = 523
  ClientWidth = 1553
  Position = poDesigned
  OnCreate = FormCreate
  ExplicitLeft = -269
  ExplicitWidth = 1553
  ExplicitHeight = 523
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1553
    Height = 523
    ExplicitWidth = 1553
    ExplicitHeight = 523
    object gbTrain: TcxGroupBox
      Left = 0
      Top = 60
      Align = alClient
      Alignment = alTopCenter
      Caption = #21015#36710
      PanelStyle.Active = True
      Style.Edges = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Height = 463
      Width = 1553
      object tlAlarm: TcxTreeList
        Left = 1
        Top = 1
        Width = 1551
        Height = 461
        BorderStyle = cxcbsNone
        Align = alClient
        Bands = <
          item
          end>
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        Navigator.Buttons.CustomButtons = <>
        OptionsData.Editing = False
        OptionsData.Deleting = False
        OptionsSelection.InvertSelect = False
        OptionsView.CheckGroups = True
        OptionsView.GridLines = tlglBoth
        OptionsView.Indicator = True
        TabOrder = 0
        OnCustomDrawDataCell = tlAlarmCustomDrawDataCell
        object ColumnNeID: TcxTreeListColumn
          Caption.Text = #35774#22791#21517#31216
          DataBinding.ValueType = 'String'
          Options.Editing = False
          Width = 102
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnNeIDGetDisplayText
        end
        object ColumnAlarmID: TcxTreeListColumn
          Caption.Text = #21578#35686#31867#21035
          DataBinding.ValueType = 'Integer'
          Width = 120
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnAlarmIDGetDisplayText
        end
        object ColumnAlarmLevel: TcxTreeListColumn
          Caption.Text = #21578#35686#31561#32423
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnAlarmLevelGetDisplayText
        end
        object ColumnAlarmType: TcxTreeListColumn
          PropertiesClassName = 'TcxImageComboBoxProperties'
          Properties.Items = <
            item
              Description = #19978#34892
              ImageIndex = 0
              Value = 1
            end
            item
              Description = #19979#34892
              ImageIndex = 1
              Value = 2
            end>
          Properties.ShowDescriptions = False
          Visible = False
          Caption.Text = #21578#35686#31867#22411
          DataBinding.ValueType = 'Integer'
          Width = 70
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnAlarmRaiseTime: TcxTreeListColumn
          Caption.Text = #21578#35686#20135#29983#26102#38388
          DataBinding.ValueType = 'Integer'
          Width = 180
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnAlarmRaiseTimeGetDisplayText
        end
        object ColumnAlarmInfo: TcxTreeListColumn
          Caption.Text = #21578#35686#20869#23481
          DataBinding.ValueType = 'String'
          Width = 300
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnAlarmReason: TcxTreeListColumn
          Visible = False
          Caption.Text = #21578#35686#21407#22240
          DataBinding.ValueType = 'String'
          Width = 135
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnAlarmDealAdvise: TcxTreeListColumn
          Visible = False
          Caption.Text = #22788#29702#24314#35758
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnAlarmClearTime: TcxTreeListColumn
          Caption.Text = #21578#35686#28165#38500#26102#38388
          DataBinding.ValueType = 'Integer'
          Width = 180
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnAlarmClearTimeGetDisplayText
        end
        object ColumnAlarmClearInfo: TcxTreeListColumn
          Caption.Text = #21578#35686#28165#38500#20449#24687
          DataBinding.ValueType = 'String'
          Width = 169
          Position.ColIndex = 9
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
    end
    object gbWorkPanel: TcxGroupBox
      Left = 0
      Top = 0
      Align = alTop
      PanelStyle.Active = True
      Style.BorderStyle = ebs3D
      Style.Edges = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 1
      Height = 60
      Width = 1553
      object Label3: TLabel
        Left = 16
        Top = 20
        Width = 56
        Height = 14
        Caption = #35774#22791#31867#22411
      end
      object Label1: TLabel
        Left = 260
        Top = 21
        Width = 56
        Height = 14
        Caption = #35774#22791#21517#31216
      end
      object btnQuery: TcxButton
        Left = 1300
        Top = 19
        Width = 75
        Height = 25
        Caption = #26597#35810
        LookAndFeel.NativeStyle = False
        OptionsImage.Glyph.SourceDPI = 96
        OptionsImage.Glyph.Data = {
          89504E470D0A1A0A0000000D494844520000001000000010080600000168F4CF
          F7000000017352474200AECE1CE900000044655849664D4D002A000000080001
          87690004000000010000001A000000000003A00100030000000100010000A002
          00040000000100000010A0030004000000010000001000000000345571F20000
          021E4944415438119D534168D44014FD93DD6D33C96C96BA82A016A4AB14F760
          11948A27A1E041BC7A10048F0AE24548D282488A6261172AD5BB089E3C2878B6
          28F5D68B78944A45B4450B2DAC996C926ED97CFF44B2CD966E0FCE65FE7FF3DF
          9F97373F00072FDFD157FB2AA4A36F6B0AA12052BB30CA55B5F72FF40E597944
          0BC27023037C5BC7222B1626D1BB546CB797EF8866C4D8EE29FF0E0C04002E03
          C215AD54382B1EB73FB3D8B64E75A0B36235E35EB122C5F72BB59DCEF60D4D6F
          FA5FB32EF95D7FF4671581EDB294BAA01D2D2183510DE121DDBF90270C8CD37B
          D11DA9C8246A31C69648E0380246A4692C65A94FDA4B0F67F8A4B4F50F29AE7C
          0BA6CDCB7B8B94290A2373D95B53B717F305E89DD0554EE4F710BAFCA274F8B3
          7C818AE9AD9EF7304AA4748CDB0A506C69F337D961FA7E56232E032455121606
          E1C63B317AEC7A56D0676FDAE1E9C9E1606DFD16C5D748E569066802B22FA8C1
          ABF291EA02BBB7960E43D620BD214BD03B6A043FD75F02633BC2189BB21AD1E1
          7223E6E2FCD57390B04DF96BF3B76FF32D3923EA1927DD7D974FFB8EF1A20F1C
          9090214F9485A1C32FA8122D353CC139C0E4261D7CC3F9E37C00378545ADEED0
          10865D84BB88F86F1EA56BCC62923C182A96C6F539B9725083C031A712EC2E92
          7951A93074A6574BD23E929A4E26AD77B04FA026331BA1BE57085C732249BAAF
          89532323B7E8053E914A417F4A1D102B348C3F84313CC1BC566B9FBEFF07FD05
          976ED54D26A48EAE0000000049454E44AE426082}
        TabOrder = 5
        OnClick = btnQueryClick
      end
      object cbDevType: TcxComboBox
        Left = 88
        Top = 19
        Properties.DropDownRows = 30
        Properties.OnChange = cbDevTypePropertiesChange
        TabOrder = 0
        Width = 142
      end
      object cbDev: TcxComboBox
        Left = 332
        Top = 19
        Properties.DropDownRows = 30
        TabOrder = 1
        Width = 142
      end
      object cxLabel1: TcxLabel
        Left = 505
        Top = 21
        Caption = #24320#22987#26102#38388
        Transparent = True
      end
      object deStartTime: TcxDateEdit
        Left = 577
        Top = 19
        Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
        Properties.EditFormat = 'YYYY-MM-DD HH:mm'
        Properties.InputKind = ikRegExpr
        Properties.Kind = ckDateTime
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Blue'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
        StyleFocused.LookAndFeel.NativeStyle = False
        StyleFocused.LookAndFeel.SkinName = 'Office2010Blue'
        StyleHot.LookAndFeel.NativeStyle = False
        StyleHot.LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 2
        Width = 150
      end
      object cxLabel2: TcxLabel
        Left = 758
        Top = 21
        Caption = #32467#26463#26102#38388
        Transparent = True
      end
      object deEndTime: TcxDateEdit
        Left = 830
        Top = 19
        Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
        Properties.EditFormat = 'YYYY-MM-DD HH:mm'
        Properties.InputKind = ikRegExpr
        Properties.Kind = ckDateTime
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Blue'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
        StyleFocused.LookAndFeel.NativeStyle = False
        StyleFocused.LookAndFeel.SkinName = 'Office2010Blue'
        StyleHot.LookAndFeel.NativeStyle = False
        StyleHot.LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 3
        Width = 150
      end
      object cxLabel3: TcxLabel
        Left = 1009
        Top = 21
        Caption = #21578#35686#20869#23481
        Transparent = True
      end
      object edtAlarmInfoKey: TcxTextEdit
        Left = 1081
        Top = 19
        TabOrder = 4
        Width = 187
      end
      object btnPrev: TcxButton
        Left = 1400
        Top = 19
        Width = 75
        Height = 25
        Caption = #24448#21069
        Enabled = False
        TabOrder = 6
        OnClick = btnPrevClick
      end
      object btnNext: TcxButton
        Left = 1500
        Top = 19
        Width = 75
        Height = 25
        Caption = #24448#21518
        Enabled = False
        TabOrder = 7
        OnClick = btnNextClick
      end
    end
  end
end
