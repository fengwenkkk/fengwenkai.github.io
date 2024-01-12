inherited FormPIS: TFormPIS
  Caption = 'PIS - SZ13-20220720'
  ClientHeight = 622
  ClientWidth = 1089
  Position = poScreenCenter
  OnClose = FormClose
  ExplicitWidth = 1105
  ExplicitHeight = 661
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1089
    Height = 622
    Visible = False
    ExplicitWidth = 1089
    ExplicitHeight = 622
    object MediaPlayer1: TMediaPlayer
      Left = 224
      Top = 224
      Width = 253
      Height = 30
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 0
    end
  end
  object pcMain: TcxPageControl
    Left = 0
    Top = 0
    Width = 1089
    Height = 622
    Align = alClient
    TabOrder = 1
    Properties.ActivePage = tsIPH
    Properties.CustomButtons.Buttons = <>
    Properties.TabHeight = 28
    Properties.TabWidth = 100
    ClientRectBottom = 615
    ClientRectLeft = 3
    ClientRectRight = 1082
    ClientRectTop = 37
    object tsIPH: TcxTabSheet
      Caption = 'IPH'
      ImageIndex = 0
      object Panel2: TPanel
        Left = 0
        Top = 0
        Width = 1079
        Height = 578
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 0
        object btnIPHApply: TcxButton
          Left = 302
          Top = 494
          Width = 75
          Height = 25
          Caption = #23545#35762#30003#35831
          TabOrder = 4
          OnClick = btnIPHApplyClick
        end
        object cxGroupBox1: TcxGroupBox
          Left = 16
          Top = 65
          Caption = #35774#22791'1'
          TabOrder = 1
          Height = 339
          Width = 389
          object edtCarriageID1: TcxTextEdit
            Left = 96
            Top = 29
            TabOrder = 0
            Text = '1'
            Width = 121
          end
          object edtIPHDevID1: TcxTextEdit
            Left = 96
            Top = 61
            TabOrder = 1
            Text = '1'
            Width = 121
          end
          object btnIPHhandup1: TcxButton
            Left = 96
            Top = 158
            Width = 75
            Height = 25
            Caption = #25346#26029
            Enabled = False
            TabOrder = 3
            OnClick = btnIPHhandup1Click
          end
          object edtDCAddr1: TcxTextEdit
            Left = 96
            Top = 189
            TabOrder = 8
            Width = 163
          end
          object cxLabel2: TcxLabel
            Left = 32
            Top = 30
            Caption = #36710#21410#21495
            Transparent = True
          end
          object cxLabel3: TcxLabel
            Left = 32
            Top = 62
            Caption = #25253#35686#22120
            Transparent = True
          end
          object cxLabel5: TcxLabel
            Left = 22
            Top = 188
            Caption = #23545#31471#22320#22336
            Transparent = True
          end
          object cxLabel15: TcxLabel
            Left = 32
            Top = 117
            Caption = #35270#39057#27969
            Transparent = True
          end
          object edtIPHUrl1: TcxTextEdit
            Left = 96
            Top = 117
            TabOrder = 2
            Text = 'rtsp://192.0.1.133:554/h264/ch1/sub/av_stream'
            Width = 265
          end
          object cxLabel18: TcxLabel
            Left = 18
            Top = 225
            Caption = #35843#24230#35821#38899
            Transparent = True
          end
          object edtIPHAudio1: TcxTextEdit
            Left = 96
            Top = 225
            TabOrder = 10
            Width = 163
          end
          object cxLabel19: TcxLabel
            Left = 46
            Top = 261
            Caption = #29366#24577
            Transparent = True
          end
          object edtIPHStatus1: TcxTextEdit
            Left = 96
            Top = 261
            TabOrder = 12
            Width = 163
          end
          object cxLabel34: TcxLabel
            Left = 32
            Top = 93
            Caption = #36710#38376#21495
            Transparent = True
          end
          object edtIPHDoorID1: TcxTextEdit
            Left = 96
            Top = 89
            TabOrder = 14
            Text = '1'
            Width = 121
          end
        end
        object cxGroupBox2: TcxGroupBox
          Left = 428
          Top = 65
          Caption = #35774#22791'2'
          TabOrder = 2
          Height = 337
          Width = 385
          object edtCarriageID2: TcxTextEdit
            Left = 92
            Top = 29
            TabOrder = 0
            Text = '0'
            Width = 121
          end
          object edtIPHDevID2: TcxTextEdit
            Left = 92
            Top = 62
            TabOrder = 1
            Text = '5'
            Width = 121
          end
          object btnIPHhandup2: TcxButton
            Left = 96
            Top = 157
            Width = 75
            Height = 25
            Caption = #25346#26029
            Enabled = False
            TabOrder = 3
            OnClick = btnIPHhandup2Click
          end
          object edtDCAddr2: TcxTextEdit
            Left = 96
            Top = 188
            TabOrder = 8
            Width = 165
          end
          object cxLabel6: TcxLabel
            Left = 32
            Top = 30
            Caption = #36710#21410#21495
            Transparent = True
          end
          object cxLabel7: TcxLabel
            Left = 32
            Top = 62
            Caption = #25253#35686#22120
            Transparent = True
          end
          object cxLabel8: TcxLabel
            Left = 24
            Top = 188
            Caption = #23545#31471#22320#22336
            Transparent = True
          end
          object cxLabel16: TcxLabel
            Left = 32
            Top = 117
            Caption = #35270#39057#27969
            Transparent = True
          end
          object edtIPHUrl2: TcxTextEdit
            Left = 92
            Top = 116
            TabOrder = 2
            Text = 'rtsp://192.0.1.133:554/h264/ch1/sub/av_stream'
            Width = 265
          end
          object cxLabel20: TcxLabel
            Left = 22
            Top = 229
            Caption = #35843#24230#35821#38899
            Transparent = True
          end
          object edtIPHAudio2: TcxTextEdit
            Left = 96
            Top = 229
            TabOrder = 10
            Width = 163
          end
          object cxLabel21: TcxLabel
            Left = 50
            Top = 265
            Caption = #29366#24577
            Transparent = True
          end
          object edtIPHStatus2: TcxTextEdit
            Left = 96
            Top = 265
            TabOrder = 12
            Width = 163
          end
          object cxLabel33: TcxLabel
            Left = 32
            Top = 92
            Caption = #36710#38376#21495
            Transparent = True
          end
          object edtIPHDoorID2: TcxTextEdit
            Left = 92
            Top = 90
            TabOrder = 14
            Text = '5'
            Width = 121
          end
        end
        object cxLabel1: TcxLabel
          Left = 52
          Top = 26
          Caption = #36710#20307#21495
          Transparent = True
        end
        object edtTrainUnitID: TcxTextEdit
          Left = 116
          Top = 25
          TabOrder = 0
          Text = '1402'
          Width = 121
        end
        object LabelMicHint: TcxLabel
          Left = 52
          Top = 410
          Caption = #40614#20811#39118#24050#32463#25171#24320
          ParentColor = False
          Style.Color = clRed
          Style.TextColor = clRed
          Transparent = True
          Visible = False
        end
        object cbAECMode: TcxCheckBox
          Left = 248
          Top = 414
          Caption = #25171#24320#22238#22768#25233#21046
          Properties.OnChange = cbAECModePropertiesChange
          State = cbsChecked
          TabOrder = 6
          Transparent = True
        end
        object cxButton13: TcxButton
          Left = 570
          Top = 494
          Width = 75
          Height = 25
          Caption = #22238#22768#27979#35797
          TabOrder = 7
          Visible = False
          OnClick = cxButton13Click
        end
        object edtIPHRecvAudioAddr: TcxTextEdit
          Left = 332
          Top = 25
          TabOrder = 8
          Text = '172.0.0.1'
          Width = 121
        end
        object cxLabel29: TcxLabel
          Left = 256
          Top = 26
          Caption = 'IPH'#22320#22336
          Transparent = True
        end
        object edtIPHRecvAudioPort: TcxTextEdit
          Left = 524
          Top = 25
          TabOrder = 10
          Text = '52000'
          Width = 121
        end
        object cxLabel30: TcxLabel
          Left = 465
          Top = 26
          Caption = 'IPH'#31471#21475
          Transparent = True
        end
        object btnIPHTalkAck: TcxButton
          Left = 435
          Top = 494
          Width = 75
          Height = 25
          Caption = 'ACK'
          Enabled = False
          TabOrder = 12
          OnClick = btnIPHTalkAckClick
        end
      end
    end
    object tsRecBrd: TcxTabSheet
      Caption = #39044#24405#38899#24191#25773
      ImageIndex = 1
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 1079
        Height = 578
        Align = alClient
        BevelOuter = bvNone
        ParentColor = True
        TabOrder = 0
        object btnRecBrdRsp: TcxButton
          Left = 312
          Top = 20
          Width = 75
          Height = 25
          Caption = #24191#25773#24212#31572
          TabOrder = 0
          OnClick = btnRecBrdRspClick
        end
        object edtCurrRecID: TcxTextEdit
          Left = 130
          Top = 21
          TabOrder = 1
          Width = 121
        end
        object mmBrd: TcxMemo
          Left = 130
          Top = 60
          Properties.ScrollBars = ssBoth
          TabOrder = 3
          Height = 349
          Width = 535
        end
        object cxLabel13: TcxLabel
          Left = 58
          Top = 61
          Caption = #21382#21490#24191#25773
          Transparent = True
        end
        object cxLabel14: TcxLabel
          Left = 30
          Top = 22
          Caption = #24403#21069#24191#25773#32534#21495
          Transparent = True
        end
      end
    end
    object cxTabSheet1: TcxTabSheet
      Caption = #32452#25773#21457#29616
      ImageIndex = 2
      object LabelInfo: TLabel
        Left = 408
        Top = 44
        Width = 8
        Height = 15
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = #23435#20307
        Font.Style = []
        ParentFont = False
      end
      object edtCurrAddr: TcxTextEdit
        Left = 196
        Top = 157
        TabOrder = 4
        Width = 185
      end
      object edtGateway: TcxTextEdit
        Left = 196
        Top = 201
        TabOrder = 5
        Width = 185
      end
      object edtSAAddr: TcxTextEdit
        Left = 196
        Top = 245
        TabOrder = 10
        Width = 185
      end
      object cxLabel9: TcxLabel
        Left = 116
        Top = 90
        Caption = #26412#22320#22320#22336
        Transparent = True
      end
      object cxLabel10: TcxLabel
        Left = 116
        Top = 158
        Caption = #24403#21069#22320#22336
        Transparent = True
      end
      object cxLabel11: TcxLabel
        Left = 116
        Top = 202
        Caption = #32593#20851#22320#22336
        Transparent = True
      end
      object cxLabel12: TcxLabel
        Left = 102
        Top = 246
        Caption = #36710#36733#21488#22320#22336
        Transparent = True
      end
      object btnStartDiscover: TcxButton
        Left = 408
        Top = 88
        Width = 75
        Height = 25
        Caption = #24320#22987
        TabOrder = 1
        OnClick = btnStartDiscoverClick
      end
      object cbLocalAddr: TcxComboBox
        Left = 196
        Top = 89
        TabOrder = 2
        Text = 'cbLocalAddr'
        Width = 185
      end
      object cxLabel22: TcxLabel
        Left = 74
        Top = 42
        Caption = #25351#23450#36710#36733#21488#22320#22336
        Transparent = True
      end
      object edtTrainAddr: TcxTextEdit
        Left = 196
        Top = 41
        TabOrder = 0
        Text = '192.0.1.133'
        Width = 185
      end
      object btnCreateTcpSocket: TcxButton
        Left = 568
        Top = 90
        Width = 75
        Height = 25
        Caption = #24320#22987#30417#21548
        TabOrder = 3
        OnClick = btnCreateTcpSocketClick
      end
    end
    object cxTabSheet2: TcxTabSheet
      Caption = 'PIS'#25509#21475
      ImageIndex = 3
      object cbAutoMode: TcxCheckBox
        Left = 132
        Top = 37
        Caption = #33258#21160#39550#39542#27169#24335
        State = cbsChecked
        TabOrder = 0
        Transparent = True
      end
      object cbOpenDevCover: TcxCheckBox
        Left = 268
        Top = 37
        Caption = #24320#21551#35774#22791#30422
        TabOrder = 1
        Transparent = True
      end
      object mmGetTrainInfo: TcxMemo
        Left = 132
        Top = 128
        Properties.ScrollBars = ssBoth
        TabOrder = 2
        Height = 301
        Width = 493
      end
      object cbIPHRunningStatus: TcxCheckBox
        Left = 396
        Top = 37
        Caption = 'IPH'#27491#24120#36816#34892
        TabOrder = 3
        Transparent = True
      end
      object cbPAMaster: TcxCheckBox
        Left = 524
        Top = 37
        Caption = #20027#29992'PA'
        TabOrder = 4
        Transparent = True
      end
      object cbPARunningStatus: TcxComboBox
        Left = 212
        Top = 77
        TabOrder = 5
        Text = 'PA'#36816#34892#29366#24577
        Width = 133
      end
      object cxLabel28: TcxLabel
        Left = 132
        Top = 78
        Caption = 'PA'#36816#34892#29366#24577
        Transparent = True
      end
      object btnIPHDevReset: TcxButton
        Left = 396
        Top = 76
        Width = 75
        Height = 25
        Caption = 'IPH'#22797#20301
        TabOrder = 7
        OnClick = btnIPHDevResetClick
      end
      object cbIPHPauseFail: TcxCheckBox
        Left = 628
        Top = 37
        Caption = 'IPH'#25346#36215#22833#36133
        TabOrder = 8
        Transparent = True
      end
    end
    object cxTabSheet3: TcxTabSheet
      Caption = #21015#36710#29366#24577
      ImageIndex = 4
      object cxLabel4: TcxLabel
        Left = 80
        Top = 54
        Caption = #21015#36710#33258#26816#29366#24577
        Transparent = True
      end
      object edtSelfCheckInfo: TcxTextEdit
        Left = 184
        Top = 53
        TabOrder = 0
        Width = 149
      end
      object cxLabel17: TcxLabel
        Left = 108
        Top = 98
        Caption = #24515#36339#28040#24687
        Transparent = True
      end
      object edtHeartBeatTime: TcxTextEdit
        Left = 184
        Top = 97
        TabOrder = 2
        Width = 145
      end
    end
    object cxTabSheet4: TcxTabSheet
      Caption = #24405#38899
      ImageIndex = 5
      DesignSize = (
        1079
        578)
      object Label1: TLabel
        Left = 440
        Top = 41
        Width = 84
        Height = 14
        Caption = #24403#21069#27979#35797#38899#37327
      end
      object btnStartRec: TcxButton
        Left = 216
        Top = 24
        Width = 75
        Height = 25
        Caption = #24320#22987
        TabOrder = 0
        OnClick = btnStartRecClick
      end
      object btnStopRec: TcxButton
        Left = 312
        Top = 24
        Width = 75
        Height = 25
        Caption = #32467#26463
        Enabled = False
        TabOrder = 1
        OnClick = btnStopRecClick
      end
      object edtRecTime: TcxTextEdit
        Left = 72
        Top = 25
        TabOrder = 2
        Width = 121
      end
      object cxButton1: TcxButton
        Left = 216
        Top = 128
        Width = 75
        Height = 25
        Caption = 'Open MIC'
        TabOrder = 11
        OnClick = cxButton1Click
      end
      object cxButton2: TcxButton
        Left = 312
        Top = 128
        Width = 75
        Height = 25
        Caption = 'Close MIC'
        TabOrder = 12
        OnClick = cxButton2Click
      end
      object cxButton3: TcxButton
        Left = 56
        Top = 72
        Width = 75
        Height = 25
        Caption = 'Send File'
        TabOrder = 7
        OnClick = cxButton3Click
      end
      object cxButton4: TcxButton
        Left = 56
        Top = 128
        Width = 75
        Height = 25
        Caption = 'Stop Send'
        TabOrder = 10
        OnClick = cxButton4Click
      end
      object cxButton5: TcxButton
        Left = 216
        Top = 72
        Width = 75
        Height = 25
        Caption = #24405#38899
        TabOrder = 8
        OnClick = cxButton5Click
      end
      object cxButton6: TcxButton
        Left = 312
        Top = 72
        Width = 75
        Height = 25
        Caption = #20572#27490
        TabOrder = 9
        OnClick = cxButton6Click
      end
      object edtCurrMicVolume: TcxTextEdit
        Left = 530
        Top = 38
        TabOrder = 5
        Width = 57
      end
      object mmMicVolume: TcxMemo
        Left = 424
        Top = 68
        Anchors = [akLeft, akTop, akRight, akBottom]
        Properties.ScrollBars = ssBoth
        TabOrder = 6
        Height = 486
        Width = 629
      end
      object MediaPlayer: TMediaPlayer
        Left = 134
        Top = 200
        Width = 253
        Height = 30
        DoubleBuffered = True
        FileName = 'D:\1K'#27491#24358#27874'.wav'
        Visible = False
        ParentDoubleBuffered = False
        TabOrder = 13
      end
      object cxButton11: TcxButton
        Left = 608
        Top = 37
        Width = 75
        Height = 25
        Caption = #24320#22987
        TabOrder = 3
        OnClick = cxButton11Click
      end
      object cxButton12: TcxButton
        Left = 712
        Top = 37
        Width = 75
        Height = 25
        Caption = #32467#26463
        TabOrder = 4
        OnClick = cxButton12Click
      end
    end
    object tsWavAna: TcxTabSheet
      Caption = 'WAV'#25991#20214#20998#26512
      ImageIndex = 6
      DesignSize = (
        1079
        578)
      object btnAna: TcxButton
        Left = 860
        Top = 24
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Caption = #20998#26512
        TabOrder = 0
        OnClick = btnAnaClick
      end
      object cxLabel23: TcxLabel
        Left = 32
        Top = 26
        Caption = 'WAV'#25991#20214
        Transparent = True
      end
      object edtWavFile: TcxButtonEdit
        Left = 104
        Top = 25
        Anchors = [akLeft, akTop, akRight]
        Properties.Buttons = <
          item
            Default = True
            Kind = bkEllipsis
          end>
        Properties.OnButtonClick = edtWavFilePropertiesButtonClick
        TabOrder = 2
        Width = 729
      end
      object cxLabel24: TcxLabel
        Left = 32
        Top = 66
        Caption = #22768#36947#25968
        Transparent = True
      end
      object cxLabel25: TcxLabel
        Left = 176
        Top = 66
        Caption = #37327#21270#23383#33410#25968
        Transparent = True
      end
      object edtSampleBytes: TcxTextEdit
        Left = 270
        Top = 65
        Properties.ReadOnly = True
        TabOrder = 5
        Width = 43
      end
      object cxLabel26: TcxLabel
        Left = 340
        Top = 66
        Caption = #26102#38271
        Transparent = True
      end
      object edtWavLength: TcxTextEdit
        Left = 390
        Top = 65
        Properties.ReadOnly = True
        TabOrder = 6
        Width = 71
      end
      object edtChanNum: TcxTextEdit
        Left = 104
        Top = 65
        Properties.ReadOnly = True
        TabOrder = 4
        Width = 43
      end
      object btnSaveChart: TcxButton
        Left = 860
        Top = 64
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Caption = #20445#23384
        TabOrder = 1
        OnClick = btnSaveChartClick
      end
      object cht: TChart
        Left = 24
        Top = 112
        Width = 1033
        Height = 449
        BackWall.Brush.Gradient.Direction = gdBottomTop
        BackWall.Brush.Gradient.EndColor = clWhite
        BackWall.Brush.Gradient.StartColor = 15395562
        BackWall.Brush.Gradient.Visible = True
        BackWall.Transparent = False
        Foot.Font.Color = clBlue
        Foot.Font.Name = 'Verdana'
        Gradient.Direction = gdBottomTop
        Gradient.EndColor = clWhite
        Gradient.MidColor = 15395562
        Gradient.StartColor = 15395562
        Gradient.Visible = True
        LeftWall.Color = 14745599
        Legend.Font.Name = 'Verdana'
        Legend.Shadow.Transparency = 0
        Legend.Visible = False
        RightWall.Color = 14745599
        Title.Font.Name = 'Verdana'
        Title.Text.Strings = (
          'TChart')
        BottomAxis.Axis.Color = 4210752
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
        LeftAxis.Axis.Color = 4210752
        LeftAxis.Grid.Color = 11119017
        LeftAxis.LabelsFormat.Font.Name = 'Verdana'
        LeftAxis.TicksInner.Color = 11119017
        LeftAxis.Title.Font.Name = 'Verdana'
        RightAxis.Axis.Color = 4210752
        RightAxis.Grid.Color = 11119017
        RightAxis.LabelsFormat.Font.Name = 'Verdana'
        RightAxis.TicksInner.Color = 11119017
        RightAxis.Title.Font.Name = 'Verdana'
        TopAxis.Axis.Color = 4210752
        TopAxis.Grid.Color = 11119017
        TopAxis.LabelsFormat.Font.Name = 'Verdana'
        TopAxis.TicksInner.Color = 11119017
        TopAxis.Title.Font.Name = 'Verdana'
        View3D = False
        View3DWalls = False
        TabOrder = 10
        Anchors = [akLeft, akTop, akRight, akBottom]
        DefaultCanvas = 'TGDIPlusCanvas'
        ColorPaletteIndex = 13
      end
      object btnPlayWav: TcxButton
        Left = 964
        Top = 24
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Caption = #25773#25918
        TabOrder = 11
        OnClick = btnPlayWavClick
      end
      object btnOpenBmpFile: TcxButton
        Left = 964
        Top = 64
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Caption = #25171#24320
        TabOrder = 12
        OnClick = btnOpenBmpFileClick
      end
    end
    object cxTabSheet5: TcxTabSheet
      Caption = 'PCM'
      ImageIndex = 7
      object btnPlayPCM: TcxButton
        Left = 176
        Top = 56
        Width = 75
        Height = 25
        Caption = #25773#25918
        TabOrder = 1
        OnClick = btnPlayPCMClick
      end
      object btnStopPCM: TcxButton
        Left = 496
        Top = 56
        Width = 75
        Height = 25
        Caption = #32467#26463
        Enabled = False
        TabOrder = 4
        OnClick = btnStopPCMClick
      end
      object btnInitPCM: TcxButton
        Left = 64
        Top = 56
        Width = 75
        Height = 25
        Caption = #21021#22987#21270
        TabOrder = 0
        OnClick = btnInitPCMClick
      end
      object btnPausePCM: TcxButton
        Left = 280
        Top = 56
        Width = 75
        Height = 25
        Caption = #26242#20572
        TabOrder = 2
        OnClick = btnPausePCMClick
      end
      object cxButton7: TcxButton
        Left = 384
        Top = 56
        Width = 75
        Height = 25
        Caption = #24674#22797
        TabOrder = 3
        OnClick = cxButton7Click
      end
      object mmAudioList: TcxMemo
        Left = 64
        Top = 104
        Properties.ScrollBars = ssBoth
        TabOrder = 6
        Height = 209
        Width = 489
      end
      object btnSetVolume: TcxButton
        Left = 790
        Top = 219
        Width = 75
        Height = 25
        Caption = #35774#32622#38899#37327
        TabOrder = 11
        OnClick = btnSetVolumeClick
      end
      object edtAudioVolume: TcxTextEdit
        Left = 624
        Top = 176
        TabOrder = 9
        Text = '55'
        Width = 73
      end
      object edtAudioDev: TcxTextEdit
        Left = 624
        Top = 136
        TabOrder = 7
        Width = 241
      end
      object btnGetVolume: TcxButton
        Left = 790
        Top = 175
        Width = 75
        Height = 25
        Caption = #33719#21462#38899#37327
        TabOrder = 8
        OnClick = btnGetVolumeClick
      end
      object cxButton8: TcxButton
        Left = 624
        Top = 56
        Width = 75
        Height = 25
        Caption = #38899#39057#35774#22791
        TabOrder = 5
        OnClick = cxButton8Click
      end
      object cxButton10: TcxButton
        Left = 624
        Top = 219
        Width = 105
        Height = 25
        Caption = #38899#39057#35774#22791#21015#34920
        TabOrder = 10
        OnClick = cxButton10Click
      end
    end
    object tsRealBrd: TcxTabSheet
      Caption = #23454#26102#24191#25773
      ImageIndex = 8
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 1079
        Height = 578
        Align = alClient
        TabOrder = 0
        object btnCreatSock: TcxButton
          Left = 76
          Top = 32
          Width = 108
          Height = 49
          Caption = #24191#25773#21551#21160
          TabOrder = 0
          OnClick = btnCreatSockClick
        end
        object btnRealBrdStart: TcxButton
          Left = 369
          Top = 204
          Width = 89
          Height = 33
          Caption = #24191#25773#24320#22987
          TabOrder = 1
          OnClick = btnRealBrdStartClick
        end
        object btnBrdBusy: TcxButton
          Left = 537
          Top = 204
          Width = 89
          Height = 33
          Caption = #35774#22791#24537
          TabOrder = 2
          OnClick = btnBrdBusyClick
        end
        object edtCurrRealBrdID: TcxTextEdit
          Left = 170
          Top = 209
          TabOrder = 3
          Width = 121
        end
        object cxLabel27: TcxLabel
          Left = 76
          Top = 210
          Caption = #24403#21069#24191#25773#32534#21495
          Transparent = True
        end
        object edtPARecvAudioPort: TcxTextEdit
          Left = 170
          Top = 113
          TabOrder = 5
          Text = '53000'
          Width = 121
        end
        object cxLabel31: TcxLabel
          Left = 62
          Top = 114
          Caption = 'PA'#25509#25910#35821#38899#31471#21475
          Transparent = True
        end
        object edtRealBrdCarriageID: TcxTextEdit
          Left = 170
          Top = 159
          TabOrder = 7
          Width = 121
        end
        object cxLabel32: TcxLabel
          Left = 76
          Top = 160
          Caption = #24191#25773#36710#21410#21495
          Transparent = True
        end
      end
    end
  end
  object TimerSendPCM: TTimer
    Enabled = False
    Interval = 90
    OnTimer = TimerSendPCMTimer
    Left = 880
    Top = 392
  end
  object TimerInit: TTimer
    Interval = 1
    OnTimer = TimerInitTimer
    Left = 832
    Top = 460
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Lilian'
    Left = 808
    Top = 408
  end
  object TimerRec: TTimer
    Enabled = False
    OnTimer = TimerRecTimer
    Left = 884
    Top = 452
  end
  object OpenDialog: TOpenDialog
    DefaultExt = '*.wav'
    Filter = 'WAV|*.wav'
    Left = 952
    Top = 388
  end
  object TimerPlayPCM: TTimer
    Enabled = False
    Interval = 90
    OnTimer = TimerPlayPCMTimer
    Left = 810
    Top = 355
  end
  object TimerTestMicVolume: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = TimerTestMicVolumeTimer
    Left = 876
    Top = 332
  end
end
