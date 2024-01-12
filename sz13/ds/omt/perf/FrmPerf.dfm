inherited FormPerf: TFormPerf
  ClientHeight = 572
  ClientWidth = 1239
  OnCreate = FormCreate
  ExplicitWidth = 1255
  ExplicitHeight = 611
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1239
    Height = 572
    ExplicitWidth = 1239
    ExplicitHeight = 572
    object pcMain: TcxPageControl
      Left = 0
      Top = 0
      Width = 1239
      Height = 36
      Align = alTop
      TabOrder = 0
      Properties.CustomButtons.Buttons = <>
      Properties.TabWidth = 120
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Office2010Blue'
      OnChange = pcMainChange
      ClientRectBottom = 30
      ClientRectLeft = 2
      ClientRectRight = 1233
      ClientRectTop = 28
      object tsAll: TcxTabSheet
        Caption = #20840#37096#35774#22791#29366#24577
        ImageIndex = 0
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
      end
      object tsBasicStatus: TcxTabSheet
        Caption = #35774#22791#22522#26412#29366#24577
        ImageIndex = 1
        ExplicitLeft = 4
        ExplicitTop = 25
        ExplicitWidth = 0
        ExplicitHeight = 0
      end
      object tsLteStatus: TcxTabSheet
        Caption = 'LTE'#20449#21495#36136#37327
        ImageIndex = 2
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
      end
      object tsTrafficStatus: TcxTabSheet
        Caption = #35774#22791#27969#37327#20449#24687
        ImageIndex = 3
        TabVisible = False
      end
    end
    object gbMain: TcxGroupBox
      Left = 0
      Top = 36
      Align = alClient
      Alignment = alTopCenter
      PanelStyle.Active = True
      ParentShowHint = False
      ShowHint = False
      Style.LookAndFeel.NativeStyle = False
      Style.Shadow = False
      Style.TransparentBorder = True
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 1
      Height = 536
      Width = 1239
      object tlMain: TcxTreeList
        Left = 2
        Top = 62
        Width = 715
        Height = 472
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
        object ColumnTime: TcxTreeListColumn
          Caption.Text = #26102#38388
          DataBinding.ValueType = 'Integer'
          Width = 158
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTimeGetDisplayText
        end
        object ColumnNeID: TcxTreeListColumn
          Caption.Text = #35774#22791#21517#31216
          DataBinding.ValueType = 'String'
          Width = 165
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnNeIDGetDisplayText
        end
        object ColumnDevType: TcxTreeListColumn
          Caption.Text = #35774#22791#31867#22411
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnDevTypeGetDisplayText
        end
        object ColumnOnline: TcxTreeListColumn
          PropertiesClassName = 'TcxImageComboBoxProperties'
          Properties.Images = ImageListDevStatus
          Properties.Items = <
            item
              Description = #22312#32447
              ImageIndex = 0
              Value = 1
            end
            item
              Description = #31163#32447
              ImageIndex = 1
              Value = 0
            end>
          Properties.ShowDescriptions = False
          Caption.Text = #22312#32447#29366#24577
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnOnlineGetDisplayText
        end
        object ColumnCpuUsage: TcxTreeListColumn
          Caption.Text = 'CPU'#20351#29992#29575
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnMemUsage: TcxTreeListColumn
          Caption.Text = #20869#23384#20351#29992#29575
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnDiskUsage: TcxTreeListColumn
          Caption.Text = #30828#30424#20351#29992#29575
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnClusterStatus: TcxTreeListColumn
          Visible = False
          Caption.Text = #20027#22791#29366#24577
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnPCI: TcxTreeListColumn
          Caption.Text = #23567#21306#32534#21495
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnRSRP: TcxTreeListColumn
          Caption.Text = 'RSRP'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 9
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnRSRQ: TcxTreeListColumn
          Caption.Text = 'RSRQ'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 10
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnRSSI: TcxTreeListColumn
          Caption.Text = 'RSSI'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 11
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnSINR: TcxTreeListColumn
          Caption.Text = 'SINR'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 12
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLogsave: TcxTreeListColumn
          Visible = False
          Caption.Text = 'Logsave'#20998#21306#20869#23384#20351#29992#29575
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 23
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnPackage: TcxTreeListColumn
          Visible = False
          Caption.Text = 'Package'#20998#21306#20351#29992#29575
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 24
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnModel: TcxTreeListColumn
          Visible = False
          Caption.Text = 'Model'#20998#21306#20351#29992#29575
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 25
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnWANTx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'WAN'#21457#36865#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 13
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnWANRx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'WAN'#25509#25910#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 14
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN1Tx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN1'#21457#36865#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 15
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN1Rx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN1'#25509#25910#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 16
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN2Tx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN2'#21457#36865#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 17
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN2Rx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN2'#25509#25910#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 18
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN3Tx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN3'#21457#36865#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 19
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN3Rx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN3'#21457#36865#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 20
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN4Tx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN4'#21457#36865#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 21
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnLAN4Rx: TcxTreeListColumn
          Visible = False
          Caption.Text = 'LAN4'#25509#25910#36895#29575
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 22
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
      object gbCtrl: TcxGroupBox
        Left = 2
        Top = 2
        Align = alTop
        Caption = ' '
        Ctl3D = True
        PanelStyle.Active = True
        ParentBackground = False
        ParentCtl3D = False
        Style.BorderColor = clRed
        Style.BorderStyle = ebsNone
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Blue'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 0
        Height = 60
        Width = 1235
        object btnRefresh: TcxButton
          Left = 976
          Top = 19
          Width = 75
          Height = 25
          Caption = #21047#26032
          LookAndFeel.NativeStyle = False
          TabOrder = 5
          OnClick = btnRefreshClick
        end
        object cbDevName: TcxComboBox
          Left = 752
          Top = 19
          Properties.DropDownRows = 30
          TabOrder = 3
          Width = 165
        end
        object cxLabel1: TcxLabel
          Left = 160
          Top = 21
          Caption = #21382#21490#25968#25454
          Transparent = True
        end
        object deStartTime: TcxDateEdit
          Left = 232
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
          TabOrder = 1
          Width = 165
        end
        object cbLastTime: TcxCheckBox
          Left = 88
          Top = 21
          AutoSize = False
          Caption = #26159
          Properties.OnChange = cbLastTimePropertiesChange
          State = cbsChecked
          TabOrder = 0
          Transparent = True
          Height = 22
          Width = 36
        end
        object cbDevType: TcxComboBox
          Left = 492
          Top = 19
          Properties.DropDownRows = 30
          Properties.OnChange = cbDevTypePropertiesChange
          TabOrder = 2
          Width = 165
        end
        object lbDevName: TcxLabel
          Left = 680
          Top = 21
          Caption = #35774#22791#21517#31216
        end
        object lbDevType: TcxLabel
          Left = 420
          Top = 21
          Caption = #35774#22791#31867#22411
        end
        object lbReal: TcxLabel
          Left = 16
          Top = 21
          Caption = #23454#26102#25968#25454
        end
      end
      object Splitter: TcxSplitter
        Left = 717
        Top = 62
        Width = 6
        Height = 472
        AlignSplitter = salRight
        Control = gbChart
      end
      object gbChart: TcxGroupBox
        Left = 723
        Top = 62
        Align = alRight
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
        TabOrder = 3
        Height = 472
        Width = 514
        object chtLte: TChart
          Left = 3
          Top = 3
          Width = 508
          Height = 466
          BackWall.Brush.Gradient.Direction = gdBottomTop
          BackWall.Brush.Gradient.EndColor = clWhite
          BackWall.Brush.Gradient.StartColor = 15395562
          BackWall.Brush.Gradient.Visible = True
          BackWall.Pen.Color = clSkyBlue
          BackWall.Transparent = False
          BackWall.Visible = False
          BottomWall.Visible = False
          Foot.Font.Color = clBlue
          Foot.Font.Name = 'Verdana'
          Gradient.Direction = gdRadial
          Gradient.EndColor = clWhite
          Gradient.MidColor = 15395562
          Gradient.StartColor = 15395562
          LeftWall.Color = 14745599
          LeftWall.Dark3D = False
          Legend.Alignment = laBottom
          Legend.CheckBoxes = True
          Legend.Font.Name = 'Verdana'
          Legend.FontSeriesColor = True
          Legend.Frame.Visible = False
          Legend.LegendStyle = lsSeries
          Legend.Shadow.Transparency = 0
          Legend.Shadow.Visible = False
          Legend.Symbol.DefaultPen = False
          Legend.Symbol.Gradient.Colors = <
            item
              Color = clWhite
              Transparency = 100
            end
            item
              Color = clWhite
              Offset = 0.500000000000000000
            end
            item
              Color = clYellow
              Offset = 1.000000000000000000
            end>
          Legend.Symbol.Gradient.MidColor = clWhite
          Legend.Symbol.Gradient.Visible = True
          Legend.Symbol.Pen.Visible = False
          Legend.Symbol.Shadow.Visible = False
          Legend.Transparent = True
          RightWall.Color = 14745599
          Title.Font.Height = -24
          Title.Font.Name = #23435#20307
          Title.Text.Strings = (
            'LTE'#20449#21495#36136#37327)
          BottomAxis.Axis.Color = clSilver
          BottomAxis.Grid.Color = 11119017
          BottomAxis.LabelsFormat.Font.Name = 'Verdana'
          BottomAxis.TicksInner.Color = 11119017
          BottomAxis.Title.Font.Name = 'Verdana'
          DepthAxis.Axis.Color = 4210752
          DepthAxis.Grid.Color = 11119017
          DepthAxis.LabelsFormat.Font.Name = 'Verdana'
          DepthAxis.TicksInner.Color = 11119017
          DepthAxis.Title.Font.Name = 'Verdana'
          DepthTopAxis.Axis.Color = 4210752
          DepthTopAxis.Grid.Color = 11119017
          DepthTopAxis.LabelsFormat.Font.Name = 'Verdana'
          DepthTopAxis.TicksInner.Color = 11119017
          DepthTopAxis.Title.Font.Name = 'Verdana'
          Frame.Color = clSkyBlue
          LeftAxis.Axis.Color = clSilver
          LeftAxis.Grid.Color = 11119017
          LeftAxis.LabelsFormat.Font.Name = 'Verdana'
          LeftAxis.TicksInner.Color = 11119017
          LeftAxis.Title.Font.Name = 'Verdana'
          RightAxis.Axis.Color = 4210752
          RightAxis.Grid.Color = 11119017
          RightAxis.LabelsFormat.Font.Name = 'Verdana'
          RightAxis.TicksInner.Color = 11119017
          RightAxis.Title.Font.Name = 'Verdana'
          Shadow.Visible = False
          TopAxis.Axis.Color = 4210752
          TopAxis.Grid.Color = 11119017
          TopAxis.LabelsFormat.Font.Name = 'Verdana'
          TopAxis.TicksInner.Color = 11119017
          TopAxis.Title.Font.Name = 'Verdana'
          View3D = False
          View3DWalls = False
          Zoom.Allow = False
          Align = alClient
          BevelOuter = bvNone
          Color = 15918295
          TabOrder = 0
          DefaultCanvas = 'TGDIPlusCanvas'
          ColorPaletteIndex = 13
          object SeriesRSRP: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'RSRP'
            LegendTitle = 'RSRP'
            ColorEachLine = False
            SeriesColor = 14745600
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 992
            Top = 16
          end
          object SeriesRSRQ: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'RSRQ'
            LegendTitle = 'RSRQ'
            ColorEachLine = False
            SeriesColor = clGreen
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 888
            Top = 16
          end
          object SeriesRSSI: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'RSSI'
            LegendTitle = 'RSSI'
            ColorEachLine = False
            SeriesColor = 33023
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 816
            Top = 24
          end
          object SeriesSINR: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'SINR'
            LegendTitle = 'SINR'
            ColorEachLine = False
            SeriesColor = 8388863
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 744
            Top = 16
          end
        end
        object chtBasic: TChart
          Left = 3
          Top = 3
          Width = 508
          Height = 466
          BackWall.Brush.Gradient.Direction = gdBottomTop
          BackWall.Brush.Gradient.EndColor = clWhite
          BackWall.Brush.Gradient.StartColor = 15395562
          BackWall.Brush.Gradient.Visible = True
          BackWall.Pen.Color = clSkyBlue
          BackWall.Transparent = False
          BackWall.Visible = False
          BottomWall.Visible = False
          Foot.Font.Color = clBlue
          Foot.Font.Name = 'Verdana'
          Gradient.Direction = gdRadial
          Gradient.EndColor = clWhite
          Gradient.MidColor = 15395562
          Gradient.StartColor = 15395562
          Gradient.Visible = True
          LeftWall.Color = 14745599
          LeftWall.Dark3D = False
          Legend.Alignment = laBottom
          Legend.CheckBoxes = True
          Legend.Font.Name = 'Verdana'
          Legend.FontSeriesColor = True
          Legend.Frame.Visible = False
          Legend.LegendStyle = lsSeries
          Legend.Shadow.Transparency = 0
          Legend.Shadow.Visible = False
          Legend.Symbol.DefaultPen = False
          Legend.Symbol.Gradient.Colors = <
            item
              Color = clWhite
              Transparency = 100
            end
            item
              Color = clWhite
              Offset = 0.500000000000000000
            end
            item
              Color = clYellow
              Offset = 1.000000000000000000
            end>
          Legend.Symbol.Gradient.MidColor = clWhite
          Legend.Symbol.Gradient.Visible = True
          Legend.Symbol.Pen.Visible = False
          Legend.Symbol.Shadow.Visible = False
          Legend.Transparent = True
          RightWall.Color = 14745599
          Title.Font.Height = -24
          Title.Font.Name = #23435#20307
          Title.Text.Strings = (
            #35774#22791#22522#26412#29366#24577)
          BottomAxis.Axis.Color = clSilver
          BottomAxis.Grid.Color = 11119017
          BottomAxis.LabelsFormat.Font.Name = 'Verdana'
          BottomAxis.TicksInner.Color = 11119017
          BottomAxis.Title.Font.Name = 'Verdana'
          DepthAxis.Axis.Color = 4210752
          DepthAxis.Grid.Color = 11119017
          DepthAxis.LabelsFormat.Font.Name = 'Verdana'
          DepthAxis.TicksInner.Color = 11119017
          DepthAxis.Title.Font.Name = 'Verdana'
          DepthTopAxis.Axis.Color = 4210752
          DepthTopAxis.Grid.Color = 11119017
          DepthTopAxis.LabelsFormat.Font.Name = 'Verdana'
          DepthTopAxis.TicksInner.Color = 11119017
          DepthTopAxis.Title.Font.Name = 'Verdana'
          Frame.Color = clSkyBlue
          LeftAxis.Axis.Color = clSilver
          LeftAxis.Grid.Color = 11119017
          LeftAxis.LabelsFormat.Font.Name = 'Verdana'
          LeftAxis.TicksInner.Color = 11119017
          LeftAxis.Title.Font.Name = 'Verdana'
          RightAxis.Axis.Color = 4210752
          RightAxis.Grid.Color = 11119017
          RightAxis.LabelsFormat.Font.Name = 'Verdana'
          RightAxis.TicksInner.Color = 11119017
          RightAxis.Title.Font.Name = 'Verdana'
          Shadow.Visible = False
          TopAxis.Axis.Color = 4210752
          TopAxis.Grid.Color = 11119017
          TopAxis.LabelsFormat.Font.Name = 'Verdana'
          TopAxis.TicksInner.Color = 11119017
          TopAxis.Title.Font.Name = 'Verdana'
          View3D = False
          View3DWalls = False
          Zoom.Allow = False
          Align = alClient
          BevelOuter = bvNone
          Color = 15918295
          TabOrder = 1
          DefaultCanvas = 'TGDIPlusCanvas'
          PrintMargins = (
            15
            5
            15
            5)
          ColorPaletteIndex = 13
          object SeriesOnline: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = #22312#32447#29366#24577
            LegendTitle = #22312#32447#29366#24577
            ColorEachLine = False
            SeriesColor = 8388863
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 744
            Top = 16
          end
          object SeriesCpu: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'CPU'#20351#29992#29575
            LegendTitle = 'CPU'#20351#29992#29575
            ColorEachLine = False
            SeriesColor = clGreen
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 888
            Top = 16
          end
          object SeriesMem: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = #20869#23384#20351#29992#29575
            LegendTitle = #20869#23384#20351#29992#29575
            ColorEachLine = False
            SeriesColor = 14745600
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 992
            Top = 16
          end
          object SeriesDisk: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = #30828#30424#20351#29992#29575
            LegendTitle = #30828#30424#20351#29992#29575
            ColorEachLine = False
            SeriesColor = 33023
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 816
            Top = 24
          end
        end
        object chtTraffic: TChart
          Left = 3
          Top = 3
          Width = 508
          Height = 466
          BackWall.Brush.Gradient.Direction = gdBottomTop
          BackWall.Brush.Gradient.EndColor = clWhite
          BackWall.Brush.Gradient.StartColor = 15395562
          BackWall.Brush.Gradient.Visible = True
          BackWall.Pen.Color = clSkyBlue
          BackWall.Transparent = False
          BackWall.Visible = False
          BottomWall.Visible = False
          Foot.Font.Color = clBlue
          Foot.Font.Name = 'Verdana'
          Gradient.Direction = gdRadial
          Gradient.EndColor = clWhite
          Gradient.MidColor = 15395562
          Gradient.StartColor = 15395562
          Gradient.Visible = True
          LeftWall.Color = 14745599
          LeftWall.Dark3D = False
          Legend.Alignment = laBottom
          Legend.CheckBoxes = True
          Legend.Font.Name = 'Verdana'
          Legend.FontSeriesColor = True
          Legend.Frame.Visible = False
          Legend.LegendStyle = lsSeries
          Legend.Shadow.Transparency = 0
          Legend.Shadow.Visible = False
          Legend.Symbol.DefaultPen = False
          Legend.Symbol.Gradient.Colors = <
            item
              Color = clWhite
              Transparency = 100
            end
            item
              Color = clWhite
              Offset = 0.500000000000000000
            end
            item
              Color = clYellow
              Offset = 1.000000000000000000
            end>
          Legend.Symbol.Gradient.MidColor = clWhite
          Legend.Symbol.Gradient.Visible = True
          Legend.Symbol.Pen.Visible = False
          Legend.Symbol.Shadow.Visible = False
          Legend.Transparent = True
          RightWall.Color = 14745599
          Title.Font.Height = -24
          Title.Font.Name = #23435#20307
          Title.Text.Strings = (
            #35774#22791#27969#37327#20449#24687)
          BottomAxis.Axis.Color = clSilver
          BottomAxis.Grid.Color = 11119017
          BottomAxis.LabelsFormat.Font.Name = 'Verdana'
          BottomAxis.TicksInner.Color = 11119017
          BottomAxis.Title.Font.Name = 'Verdana'
          DepthAxis.Axis.Color = 4210752
          DepthAxis.Grid.Color = 11119017
          DepthAxis.LabelsFormat.Font.Name = 'Verdana'
          DepthAxis.TicksInner.Color = 11119017
          DepthAxis.Title.Font.Name = 'Verdana'
          DepthTopAxis.Axis.Color = 4210752
          DepthTopAxis.Grid.Color = 11119017
          DepthTopAxis.LabelsFormat.Font.Name = 'Verdana'
          DepthTopAxis.TicksInner.Color = 11119017
          DepthTopAxis.Title.Font.Name = 'Verdana'
          Frame.Color = clSkyBlue
          LeftAxis.Axis.Color = clSilver
          LeftAxis.Grid.Color = 11119017
          LeftAxis.LabelsFormat.Font.Name = 'Verdana'
          LeftAxis.TicksInner.Color = 11119017
          LeftAxis.Title.Font.Name = 'Verdana'
          RightAxis.Axis.Color = 4210752
          RightAxis.Grid.Color = 11119017
          RightAxis.LabelsFormat.Font.Name = 'Verdana'
          RightAxis.TicksInner.Color = 11119017
          RightAxis.Title.Font.Name = 'Verdana'
          Shadow.Visible = False
          TopAxis.Axis.Color = 4210752
          TopAxis.Grid.Color = 11119017
          TopAxis.LabelsFormat.Font.Name = 'Verdana'
          TopAxis.TicksInner.Color = 11119017
          TopAxis.Title.Font.Name = 'Verdana'
          View3D = False
          View3DWalls = False
          Zoom.Allow = False
          Align = alClient
          BevelOuter = bvNone
          Color = 15918295
          TabOrder = 2
          DefaultCanvas = 'TGDIPlusCanvas'
          PrintMargins = (
            15
            5
            15
            5)
          ColorPaletteIndex = 13
          object SeriesWanTx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'WAN'#21475#21457#36865#36895#29575
            LegendTitle = 'WAN'#21475#21457#36865#36895#29575
            ColorEachLine = False
            SeriesColor = 8388863
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 744
            Top = 16
          end
          object SeriesWanRx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'WAN'#21475#25509#25910#36895#29575
            LegendTitle = 'WAN'#21475#25509#25910#36895#29575
            ColorEachLine = False
            SeriesColor = 12285951
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 888
            Top = 16
          end
          object SeriesLan1Tx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN1'#21475#21457#36865#36895#29575
            LegendTitle = 'LAN1'#21475#21457#36865#36895#29575
            ColorEachLine = False
            SeriesColor = 14745600
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 992
            Top = 16
          end
          object SeriesLan1Rx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN1'#21475#25509#25910#36895#29575
            LegendTitle = 'LAN1'#21475#25509#25910#36895#29575
            ColorEachLine = False
            SeriesColor = 16733011
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
            Left = 816
            Top = 24
          end
          object SeriesLan2Tx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN2'#21475#21457#36865#36895#29575
            LegendTitle = 'LAN2'#21475#21457#36865#36895#29575
            ColorEachLine = False
            SeriesColor = 33023
            Title = 'SeriesLan2Tx'
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object SeriesLan2Rx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN2'#21475#25509#25910#36895#29575
            LegendTitle = 'LAN2'#21475#25509#25910#36895#29575
            ColorEachLine = False
            SeriesColor = 5351679
            Title = 'SeriesLan2Rx'
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object SeriesLan3Tx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN3'#21475#21457#36865#36895#29575
            LegendTitle = 'LAN3'#21475#21457#36865#36895#29575
            ColorEachLine = False
            SeriesColor = clGray
            Title = 'SeriesLan3Tx'
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object SeriesLan3Rx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN3'#21475#25509#25910#36895#29575
            LegendTitle = 'LAN3'#21475#25509#25910#36895#29575
            ColorEachLine = False
            SeriesColor = 11053224
            Title = 'SeriesLan3Rx'
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object SeriesLan4Tx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN4'#21475#21457#36865#36895#29575
            LegendTitle = 'LAN4'#21475#21457#36865#36895#29575
            ColorEachLine = False
            SeriesColor = 15115626
            Title = 'SeriesLan4Tx'
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
          object SeriesLan4Rx: TLineSeries
            HoverElement = [heCurrent]
            Legend.Text = 'LAN4'#21475#25509#25910#36895#29575
            LegendTitle = 'LAN4'#21475#25509#25910#36895#29575
            ColorEachLine = False
            SeriesColor = clSkyBlue
            Title = 'SeriesLan4Rx'
            Brush.BackColor = clDefault
            LinePen.Style = psDot
            Pointer.InflateMargins = True
            Pointer.Pen.Visible = False
            Pointer.Style = psCircle
            Pointer.Visible = True
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Y'
            YValues.Order = loNone
          end
        end
      end
    end
  end
  object OpenDialog: TOpenDialog
    Left = 40
    Top = 280
  end
  object TimerCheck: TTimer
    Interval = 10000
    Left = 416
    Top = 192
  end
  object ImageListDevStatus: TcxImageList
    SourceDPI = 96
    FormatVersion = 1
    DesignInfo = 18350412
    ImageInfo = <
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000001000000070000000D0000001100000012000000110000
          000D000000070000000100000000000000000000000000000000000000000000
          0001000000040000000D0E241B691E4D39C226644AEF296D51FF256449EF1D4D
          38C20D241A6A0000000E00000004000000010000000000000000000000010000
          0005040907271D4D3ABC328063FF46AC8BFF4EC19DFF51CAA5FF4DC19CFF43AB
          89FF317F61FF1D4A38BD04090729000000050000000100000000000000040610
          0C35296A52EB42A181FF51C9A4FF4DC8A2FF4AC7A0FF47C59FFF49C6A0FF4CC7
          A1FF4FC8A2FF3F9F7EFF29674EEC040A082B0000000400000000010101102560
          49D747AA8AFF53CBA7FF4DC8A3FF4BC8A1FF4AC7A1FF4AC7A1FF49C6A0FF49C6
          9FFF4AC7A1FF50C9A3FF43A686FF1E4D3ABB0000000D00000001122C216C3F8E
          72FF57CDAAFF4FCAA4FF4DC9A4FF4DC9A4FF4DC9A3FF4CC8A3FF4CC8A3FF4BC8
          A2FF4AC7A1FF4BC8A2FF51CAA6FF3C8B6EFF0F271E6900000005245743BD52B4
          95FF56CDA9FF50CBA7FF51CBA6FF50CBA5FF4FCAA5FF4FC9A5FF4EC9A5FF4EC9
          A4FF4DC8A3FF4CC8A3FF50CAA4FF4DB091FF215340BF0000000A2F745AED5CC9
          A9FF55CEAAFF53CDA9FF52CDA8FF52CCA8FF52CCA7FF51CBA7FF50CAA6FF4FCB
          A6FF4FCAA6FF4FCAA5FF50CBA5FF56C6A5FF2C6E55EB0000000B368165FF68D3
          B5FF56CEABFF56CEABFF55CDABFF55CDABFF54CDAAFF53CDAAFF53CCA8FF52CC
          A8FF51CCA8FF51CBA7FF51CAA7FF61CFB0FF317A5DFA0000000B34785FEC73D2
          B7FF59D0AEFF58CFADFF57CFADFF57CFADFF57CFABFF55CEACFF56CEABFF55CE
          ABFF54CDAAFF53CDA9FF54CEA9FF6DCFB2FF2F735AEA0000000A285F4BBA73C3
          ACFF60D2B2FF5AD0AFFF59D0AEFF59D0AEFF59D0AEFF58D0AEFF57CFACFF57CF
          ADFF56CEABFF56CEABFF5ACFADFF6FC0A8FF265B48BC0000000715302662539F
          86FF7DDDC4FF5DD3B2FF5BD2B0FF5BD2B0FF5BD1B0FF5AD1B0FF5AD0AFFF59D0
          AEFF59D0AEFF59D1ADFF79DAC0FF509C82FF132D236200000003010201092F6F
          59D077C2ACFF84DFC7FF60D3B4FF5DD2B2FF5DD3B2FF5DD2B1FF5CD2B1FF5CD1
          B0FF5DD2B1FF81DEC4FF74BFAAFF275C48B50000000700000001000000010914
          102A3B8269EA6FBAA3FF97E4D0FF78DCC1FF66D7B7FF5FD4B3FF65D6B6FF77DB
          BFFF94E3CFFF6DB8A2FF377D65E9060D0A200000000200000000000000000000
          0001050C091B2A604DB2509E84FF84CCB7FF9AE1CFFFA5EBDAFF9AE1CFFF82CC
          B7FF4F9D83FF295F4CB3050C091C000000020000000000000000000000000000
          00000000000100000003122A22502454439B31705ACC3D8B6FF931705ACC2454
          439B122A22510000000400000001000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000100000004000000070000000A0000000B0000000A0000
          0008000000040000000100000000000000000000000000000000000000000000
          000000000002000000080F0B0960211612BD291D17ED2E1F19FF291D17ED2016
          12BD0F0A08610000000900000003000000000000000000000000000000010000
          000304030220211813B635251EFF3F2E26FF45322AFF47332BFF453229FF3F2C
          25FF33231DFF201511B704030222000000030000000000000000000000020705
          042E2E211AEA3E2D25FF49362DFF4A362DFF48362DFF48352DFF48352CFF4735
          2BFF47342CFF3B2B22FF2B1E18EA0403022500000002000000000100000A2B1E
          18D3423129FF4C3830FF4B382FFF4B372FFF4A372EFF4B362DFF49362DFF4936
          2DFF48352DFF48352DFF3E2D25FF211613B70000000800000001140E0B663C2C
          23FF4E3A32FF4E3A32FF4D3A31FF4C3931FF4C3830FF4B382FFF4B382FFF4B37
          2EFF4A362EFF4A362DFF49362DFF372720FF110C096200000003271D17B94835
          2DFF503C33FF503C33FF503B33FF4F3A32FF4E3A32FF4D3A32FF4C3A31FF4D39
          30FF4B392FFF4B382FFF4B372FFF433128FF241A15BB0000000637271FEC503D
          34FF523E36FF513D35FF503D34FF503C33FF503C34FF4F3B32FF4F3A32FF4E3A
          32FF4E3A31FF4D3A32FF4D3A30FF4A372EFF30221CEA000000073D2C24FF5340
          37FF544037FF543F36FF533F36FF523F36FF523E35FF513E34FF513D35FF503C
          34FF503C33FF4F3B33FF4F3A32FF4D3A32FF36271FFA000000083A2922EB5440
          38FF554139FF554138FF544037FF544038FF543F37FF533F36FF523F36FF523E
          35FF513D35FF513D34FF503D34FF4E3931FF34251EE9000000062D211BB8503C
          34FF58433BFF58433AFF56423AFF564239FF564139FF554138FF544037FF5440
          37FF543F36FF533F36FF533E36FF4A372FFF291E18B90000000518120E5F4835
          2CFF59443CFF58443CFF58433BFF58433AFF58433AFF57433AFF56423AFF5642
          39FF554138FF554038FF544037FF433128FF150F0C5E00000002010100063628
          20CE513E35FF5B473EFF5A463DFF5A453DFF59453CFF58443CFF58443BFF5843
          3AFF57433AFF57423AFF4D3A31FF2B1F1AB20000000400000000000000010A07
          0627403026E9513C34FF5B473EFF5C473FFF5C463EFF5B463EFF5A453DFF5A44
          3DFF58443CFF4D3A31FF3D2D23E80605041D0000000100000000000000000000
          00010604031930231CB14A372EFF554138FF5B463DFF5D473FFF5A453DFF5440
          37FF49362DFF2F221BB10604031A000000010000000000000000000000000000
          0000000000010000000215100C4E2A1F1999382921CB45332AF9372921CB291E
          1899140F0C4F0000000300000001000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000001000000070000000D0000001100000012000000110000
          000D000000070000000100000000000000000000000000000000000000000000
          0001000000040000000D09053B6915097EC2190CA4EF1D0CB2FF180BA3EF1408
          7EC209043A6A0000000E00000004000000010000000000000000000000010000
          000503020F27170E7CBC201AC3FF1D2CE3FF1B34F4FF1B35FBFF1A31F4FF1B27
          E3FF1C16C1FF150A7ABD02010F29000000050000000100000000000000040504
          1A352319ABEB242EDCFF223FFBFF203DFCFF1F3CFBFF1F3AFBFF1E3AFBFF1D3A
          FAFF1D36F8FF1E27DAFF1E12A7EC0302112B000000040000000001000210231A
          9BD72B38E3FF2A47FDFF2744FDFF2642FCFF2542FBFF2441FBFF2341FCFF2240
          FBFF213EFBFF213BFBFF212DE1FF190F7EBB0000000D00000001120F476C3332
          D1FF2F4CFEFF2E4DFDFF2E4BFDFF2C49FCFF2C48FCFF2A48FDFF2A47FDFF2946
          FCFF2543FCFF2542FBFF2441FBFF2928CBFF0E0940690000000525218DBD3949
          EBFF3552FDFF3553FFFF3350FFFF324FFEFF304DFEFF2F4CFEFF2E4BFDFF2D4A
          FDFF2D48FCFF2C48FCFF2B45FDFF2D3BE7FF1F1887BF0000000A362FBCED3C55
          FAFF3C57FFFF3958FFFF3755FEFF3755FEFF3653FEFF3553FFFF3451FFFF3350
          FFFF324FFFFF304DFEFF2F4CFEFF2F48F7FF2C25B2EB0000000B3F3BD1FF4561
          FEFF415EFFFF405EFFFF3E5CFFFF3D5AFFFF3C5AFFFF3A57FFFF3A57FFFF3956
          FEFF3855FFFF3653FEFF3552FDFF3753FEFF342FC3FA0000000B3E3DC2EC526C
          FAFF4664FFFF4663FFFF4561FFFF4361FFFF415EFFFF415EFFFF405DFFFF3E5B
          FFFF3D5AFFFF3C59FFFF3A57FFFF435EFAFF3630BAEA0000000A333499BA5A6D
          F1FF4D6AFFFF4C68FFFF4B67FFFF4A66FFFF4965FFFF4764FFFF4563FFFF4561
          FFFF4360FFFF425FFFFF405EFFFF5060EFFF2E2B93BC000000071B1B4E62555C
          E5FF5F7BFFFF506DFFFF4F6DFFFF4E6AFFFF4E68FFFF4C69FFFF4B68FFFF4A68
          FFFF4A66FFFF4866FFFF5571FFFF4E52E0FF1817496200000003010103094042
          B3D06474F1FF6480FFFF5472FFFF526FFFFF526EFFFF526EFFFF516EFFFF4F6D
          FFFF4F6BFFFF5D79FFFF5D6DF0FF323294B50000000700000001000000010C0C
          202A4C51CEEA6372F0FF708BFEFF607DFFFF5976FFFF5572FFFF5873FFFF5D7A
          FFFF6B85FEFF606DEEFF474AC9E9070715200000000200000000000000000000
          00010707131B3B3D9CB25D64E9FF6C80F6FF758CFCFF7791FFFF738CFCFF6B7E
          F6FF5960E7FF383A9AB30707131C000000020000000000000000000000000000
          000000000001000000031A1B44503335889B4548B5CC5559E1F94447B4CC3235
          879B191A43510000000400000001000000000000000000000000}
      end>
  end
  object TeeGDIPlus1: TTeeGDIPlus
    Active = True
    TeePanel = chtLte
    Left = 40
    Top = 152
  end
end
