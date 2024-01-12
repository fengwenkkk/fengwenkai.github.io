inherited FormInbox: TFormInbox
  ClientHeight = 351
  ClientWidth = 623
  OnCreate = FormCreate
  OnShow = FormShow
  ExplicitWidth = 639
  ExplicitHeight = 390
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 623
    Height = 351
    ExplicitWidth = 623
    ExplicitHeight = 351
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #30005#23376#24037#21333
      Ctl3D = True
      ParentCtl3D = False
      ParentFont = False
      Style.Edges = [bLeft, bRight, bBottom]
      Style.Font.Charset = ANSI_CHARSET
      Style.Font.Color = clWindowText
      Style.Font.Height = -24
      Style.Font.Name = 'Arial'
      Style.Font.Style = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      Style.IsFontAssigned = True
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      OnDblClick = gbMainDblClick
      Height = 351
      Width = 623
      object tlMain: TcxTreeList
        Left = 3
        Top = 27
        Width = 617
        Height = 317
        BorderStyle = cxcbsNone
        Align = alClient
        Bands = <
          item
            Caption.Text = #30005#23376#24037#21333
          end>
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = 'Arial'
        Font.Style = []
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        Navigator.Buttons.CustomButtons = <>
        OptionsData.Editing = False
        OptionsView.GridLines = tlglBoth
        OptionsView.Indicator = True
        OptionsView.ShowRoot = False
        ParentFont = False
        PopupMenu = pmMain
        TabOrder = 0
        OnFocusedNodeChanged = tlMainFocusedNodeChanged
        object ColumnRecvTime: TcxTreeListColumn
          Caption.AlignVert = vaTop
          Caption.Text = #25509#25910#26102#38388
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSender: TcxTreeListColumn
          Caption.AlignVert = vaTop
          Caption.Text = #21457#36865#32773
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSDSType: TcxTreeListColumn
          Caption.AlignVert = vaTop
          Caption.Text = #31867#22411
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnSDSTypeGetDisplayText
        end
        object ColumnContent: TcxTreeListColumn
          Caption.AlignVert = vaTop
          Caption.Text = #20869#23481
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn5: TcxTreeListColumn
          Visible = False
          Caption.AlignVert = vaTop
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
    end
  end
  object TimerCheck: TTimer
    Interval = 1
    Left = 520
    Top = 168
  end
  object BarManager: TdxBarManager
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Microsoft YaHei UI'
    Font.Style = []
    Categories.Strings = (
      'Default')
    Categories.ItemsVisibles = (
      2)
    Categories.Visibles = (
      True)
    ImageOptions.SmoothGlyphs = True
    ImageOptions.StretchGlyphs = False
    PopupMenuLinks = <>
    UseSystemFont = True
    Left = 224
    Top = 96
    PixelsPerInch = 96
    object dxBarSeparator1: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
    object pmiNewSDS: TdxBarButton
      Caption = #26032#24314
      Category = 0
      Hint = #26032#24314
      Visible = ivAlways
      Glyph.SourceHeight = 16
      Glyph.SourceWidth = 16
      OnClick = pmiNewSDSClick
    end
    object pmiReplySDS: TdxBarButton
      Caption = #22238#25191
      Category = 0
      Hint = #22238#25191
      Visible = ivAlways
      Glyph.SourceHeight = 16
      Glyph.SourceWidth = 16
      OnClick = pmiReplySDSClick
    end
  end
  object pmMain: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiNewSDS'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator1'
      end
      item
        Visible = True
        ItemName = 'pmiReplySDS'
      end>
    UseOwnFont = False
    Left = 80
    Top = 136
    PixelsPerInch = 96
  end
end
