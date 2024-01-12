inherited FormSDSManage: TFormSDSManage
  ClientHeight = 561
  ClientWidth = 931
  OnCreate = FormCreate
  ExplicitWidth = 947
  ExplicitHeight = 600
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 931
    Height = 561
    ExplicitWidth = 931
    ExplicitHeight = 561
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #21382#21490#24037#21333
      PanelStyle.Active = True
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      Height = 561
      Width = 931
      object gbCtrl: TcxGroupBox
        Left = 2
        Top = 2
        Align = alTop
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
        TabOrder = 0
        DesignSize = (
          927
          92)
        Height = 92
        Width = 927
        object btnQuery: TcxButton
          Left = 692
          Top = 17
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #26597#35810
          LookAndFeel.NativeStyle = False
          TabOrder = 5
          OnClick = btnQueryClick
        end
        object btnExport: TcxButton
          Left = 788
          Top = 17
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #23548#20986
          LookAndFeel.NativeStyle = False
          TabOrder = 11
          Visible = False
          OnClick = btnExportClick
        end
        object cxLabel1: TcxLabel
          Left = 24
          Top = 20
          Caption = #24320#22987#26102#38388
          Transparent = True
        end
        object cxLabel2: TcxLabel
          Left = 24
          Top = 55
          Caption = #32467#26463#26102#38388
          Transparent = True
        end
        object cxLabel3: TcxLabel
          Left = 276
          Top = 20
          Caption = #31867#21035
          Transparent = True
        end
        object cbSDSType: TcxComboBox
          Left = 321
          Top = 18
          TabOrder = 2
          Text = 'cbSDSType'
          Width = 140
        end
        object cxLabel5: TcxLabel
          Left = 269
          Top = 55
          Caption = #21457#20196#20154
          Transparent = True
        end
        object edtSendUserName: TcxTextEdit
          Left = 321
          Top = 53
          TabOrder = 3
          Width = 140
        end
        object cxLabel6: TcxLabel
          Left = 480
          Top = 20
          Caption = #20851#38190#35789
          Transparent = True
        end
        object edtKeyword: TcxTextEdit
          Left = 538
          Top = 18
          TabOrder = 4
          Width = 121
        end
        object btnPrint: TcxButton
          Left = 788
          Top = 52
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #25171#21360
          LookAndFeel.NativeStyle = False
          TabOrder = 12
          Visible = False
          OnClick = btnPrintClick
        end
        object deStartTime: TcxDateEdit
          Left = 97
          Top = 18
          Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
          Properties.EditFormat = 'YYYY-MM-DD HH:mm'
          Properties.InputKind = ikRegExpr
          Properties.Kind = ckDateTime
          TabOrder = 0
          Width = 154
        end
        object deEndTime: TcxDateEdit
          Left = 97
          Top = 53
          Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
          Properties.EditFormat = 'YYYY-MM-DD HH:mm'
          Properties.InputKind = ikRegExpr
          Properties.Kind = ckDateTime
          TabOrder = 1
          Width = 154
        end
        object btnOpenAttachFile: TcxButton
          Left = 692
          Top = 52
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #38468#20214
          LookAndFeel.NativeStyle = False
          OptionsImage.Glyph.SourceDPI = 96
          OptionsImage.Glyph.Data = {
            89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
            61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
            00097048597300000EC300000EC301C76FA8640000023C49444154384FCD93FB
            4F926114C79F2E6B65F7BFAD9FFAA5CDCDF2D6E696098197268DCA4699AE524A
            CCE510055B28DEB2B8D44A2D2DB2D59C12200A81DC7AB9BC2FF0027D7B9E17C6
            D662FDD22F9DEDB3739EE79CE77B76DEB397FC17B6BFCC812A1CFC0B2CBF8F68
            C756ECC3A635E88C6B189A58957834FE5EE2A18132F60E5ACA807EA5C4E8321E
            8C2EE1AECEF14A1252F6CC80CF8828167F42CC174B8845E4A96754EECA675697
            E2B390A92DA00287C88DFB76182C4EB8BC61140A4508993C782157225B8A0541
            8490CD49F9F5AF3EC8AF8D43D56765028789FA9E0D3F925998E69C98B37D91BA
            A478115C3257819DC57C0123130E5CEFB3C0E34FA0ABF72513384254FD36C4B8
            0CDCBB1CE6ED1BB8A35D90BAA6F81CA23101A9348BB368EB1EC1E0133B363D71
            B87738B46B1699400DB9DA6B4594CBC2E5E3F0E6831F86A98FE8BA35099F3F06
            66EEED3D34B40EA05F67856DD92BD56DF91290DF7CCE048E920ECD0B44E2192C
            390330CE7E46222D628B7E0FD5EDA73853ABC1A5AEC778BBEAC2F77012269A5F
            FE14C0A697C365F53C133846143D8BD870473044D716A7A33031C6376F0C6BEB
            3E09CF4E5CBA63F9615AC79AB574CF3281E3A44DBD40E776201CE311A23387A2
            3C82D1DF7D059A67759A411B2E764A6B3C416A5B8DD80D7008EE090884D214BE
            0C8BFF2418E1A5FAB34DFA9240BD7CD2D1DC3983E60E0B9A288DEDD368544EA3
            4139857A45890B0A33CE5F31A34E4EBDEC19EA64669C6BD1BFA6023514BA4B36
            0B2127AB70AA0AA7CB9E3D66FFD0BF1821BF006E682B321BC383830000000049
            454E44AE426082}
          TabOrder = 13
          OnClick = btnOpenAttachFileClick
        end
      end
      object tlMain: TcxTreeList
        Left = 2
        Top = 94
        Width = 927
        Height = 138
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
        OnFocusedNodeChanged = tlMainFocusedNodeChanged
        object ColumnID: TcxTreeListColumn
          Caption.Text = #32534#21495
          DataBinding.ValueType = 'Integer'
          Width = 68
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSendTime: TcxTreeListColumn
          Caption.Text = #21457#36865#26102#38388
          DataBinding.ValueType = 'String'
          Width = 135
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSDSType: TcxTreeListColumn
          Caption.Text = #31867#21035
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnSDSTypeGetDisplayText
        end
        object ColumnForamt: TcxTreeListColumn
          Visible = False
          Caption.Text = #26684#24335
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnIndex: TcxTreeListColumn
          Caption.Text = #24207#21495
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSender: TcxTreeListColumn
          Caption.Text = #21457#20196#20154
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnConfirmRate: TcxTreeListColumn
          PropertiesClassName = 'TcxLabelProperties'
          Properties.Alignment.Horz = taCenter
          Properties.Alignment.Vert = taVCenter
          Caption.Text = #30830#35748#29575
          DataBinding.ValueType = 'String'
          Width = 92
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnContent: TcxTreeListColumn
          Caption.Text = #20869#23481
          DataBinding.ValueType = 'String'
          Width = 227
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnNeedReply: TcxTreeListColumn
          Caption.Text = #38656#35201#22238#25191
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnNeedReplyGetDisplayText
        end
        object ColumnAttachFile: TcxTreeListColumn
          Caption.Text = #38468#20214
          DataBinding.ValueType = 'String'
          Width = 300
          Position.ColIndex = 9
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
      object cxSplitter1: TcxSplitter
        Left = 2
        Top = 232
        Width = 927
        Height = 6
        AlignSplitter = salBottom
      end
      object gbDetail: TcxGroupBox
        Left = 2
        Top = 238
        Align = alBottom
        Alignment = alTopCenter
        Caption = #20449#24687#20869#23481
        TabOrder = 3
        Height = 321
        Width = 927
        object PanelSDSInfo: TPanel
          Left = 2
          Top = 19
          Width = 923
          Height = 300
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object mmSDSInfo: TcxMemo
            Left = 0
            Top = 0
            Align = alClient
            Properties.ReadOnly = True
            Properties.ScrollBars = ssVertical
            Style.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleFocused.LookAndFeel.NativeStyle = False
            StyleHot.LookAndFeel.NativeStyle = False
            TabOrder = 0
            Height = 300
            Width = 597
          end
          object tlSDSReply: TcxTreeList
            Left = 603
            Top = 0
            Width = 320
            Height = 300
            Align = alRight
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
            object cxTreeListColumn1: TcxTreeListColumn
              Caption.Text = #25509#25910#20154
              DataBinding.ValueType = 'Integer'
              Width = 80
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object ColumnRecvTime: TcxTreeListColumn
              Caption.Text = #25509#25910#26102#38388
              DataBinding.ValueType = 'Integer'
              Width = 90
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object ColumnConfirmTime: TcxTreeListColumn
              Caption.Text = #30830#35748#26102#38388
              DataBinding.ValueType = 'Integer'
              Width = 90
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
          object PanelMediaPlayer: TPanel
            Left = 0
            Top = 0
            Width = 597
            Height = 300
            Align = alClient
            BevelOuter = bvLowered
            TabOrder = 2
            object PanelImage: TPanel
              Left = 1
              Top = 1
              Width = 595
              Height = 298
              Align = alClient
              BevelOuter = bvNone
              TabOrder = 0
              object ImageAttach: TcxImage
                Left = 0
                Top = 0
                Align = alClient
                TabOrder = 0
                Transparent = True
                Height = 298
                Width = 595
              end
            end
          end
          object SplitterReply: TcxSplitter
            Left = 597
            Top = 0
            Width = 6
            Height = 300
            AlignSplitter = salRight
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
end
