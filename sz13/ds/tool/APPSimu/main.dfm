object FormMain: TFormMain
  Left = 0
  Top = 0
  Caption = 'APP(SZ14) - 2022.05.23'
  ClientHeight = 636
  ClientWidth = 1119
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Arial Narrow'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  WindowState = wsMaximized
  PixelsPerInch = 96
  TextHeight = 20
  object mmMsg: TcxMemo
    Left = 0
    Top = 0
    Align = alLeft
    Properties.ScrollBars = ssVertical
    TabOrder = 0
    Height = 636
    Width = 461
  end
  object PanelMain: TPanel
    Left = 461
    Top = 0
    Width = 658
    Height = 636
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object pcMain: TcxPageControl
      Left = 0
      Top = 197
      Width = 658
      Height = 439
      Align = alClient
      TabOrder = 0
      Properties.ActivePage = cxTabSheet4
      Properties.CustomButtons.Buttons = <>
      Properties.TabWidth = 75
      LookAndFeel.SkinName = 'iMaginary'
      ClientRectBottom = 432
      ClientRectLeft = 3
      ClientRectRight = 651
      ClientRectTop = 34
      object cxTabSheet6: TcxTabSheet
        Caption = #30331#24405
        ImageIndex = 6
        TabVisible = False
        object btnLogin: TcxButton
          Left = 240
          Top = 71
          Width = 75
          Height = 25
          Caption = #27169#25311#30331#24405
          TabOrder = 0
          OnClick = btnLoginClick
        end
        object cxLabel13: TcxLabel
          Left = 52
          Top = 70
          Caption = #35843#24230#21488#21495#30721
          Transparent = True
        end
        object edtDCUser: TcxTextEdit
          Left = 150
          Top = 69
          TabOrder = 2
          Text = '1001'
          Width = 67
        end
      end
      object tsDISCfg: TcxTabSheet
        Caption = #33719#21462#37197#32622
        ImageIndex = 4
        DesignSize = (
          648
          398)
        object mmCfg: TcxMemo
          Left = 21
          Top = 52
          Anchors = [akLeft, akTop, akRight, akBottom]
          Properties.ScrollBars = ssVertical
          TabOrder = 0
          Height = 337
          Width = 607
        end
        object cxButton1: TcxButton
          Left = 427
          Top = 11
          Width = 75
          Height = 25
          Caption = #21047#26032
          TabOrder = 1
          OnClick = cxButton1Click
        end
      end
      object 广播: TcxTabSheet
        Caption = #24191#25773
        ImageIndex = 1
        object btnStartBrd: TcxButton
          Left = 185
          Top = 56
          Width = 75
          Height = 25
          Caption = #24320#22987#24191#25773
          TabOrder = 0
          OnClick = btnStartBrdClick
        end
      end
      object cxTabSheet2: TcxTabSheet
        Caption = #35831#21628
        ImageIndex = 2
        object btnReqCall: TcxButton
          Left = 134
          Top = 114
          Width = 75
          Height = 25
          Caption = #35831#21628
          TabOrder = 0
          OnClick = btnReqCallClick
        end
        object cxLabel4: TcxLabel
          Left = 44
          Top = 41
          Caption = #35831#21628#31867#22411
          Transparent = True
        end
        object rbCallDC: TcxRadioButton
          Left = 130
          Top = 45
          Width = 75
          Height = 17
          Caption = #35843#24230#21488
          Checked = True
          TabOrder = 2
          TabStop = True
          Transparent = True
        end
        object rbCallStation: TcxRadioButton
          Left = 229
          Top = 45
          Width = 113
          Height = 17
          Caption = #36710#31449
          TabOrder = 3
          Transparent = True
        end
      end
      object cxTabSheet3: TcxTabSheet
        Caption = #36710#27425#21495
        ImageIndex = 3
        object cxLabel2: TcxLabel
          Left = 80
          Top = 70
          Caption = #36710#27425#21495
          Transparent = True
        end
        object edtNewTrainID: TcxTextEdit
          Left = 138
          Top = 69
          TabOrder = 1
          Text = '10100'
          Width = 88
        end
        object btnSetTrainID: TcxButton
          Left = 348
          Top = 71
          Width = 97
          Height = 25
          Caption = #35774#32622#36710#27425#21495
          TabOrder = 2
          OnClick = btnSetTrainIDClick
        end
        object cxLabel16: TcxLabel
          Left = 69
          Top = 118
          Caption = #32456#28857#31449
          Style.BorderColor = clActiveCaption
          Properties.Alignment.Horz = taRightJustify
          Transparent = True
          AnchorX = 121
        end
        object cxLabel17: TcxLabel
          Left = 53
          Top = 158
          Caption = #36816#34892#26041#21521
          Style.BorderColor = clActiveCaption
          Properties.Alignment.Horz = taRightJustify
          Transparent = True
          AnchorX = 121
        end
        object rbUp: TcxRadioButton
          Left = 138
          Top = 161
          Width = 73
          Height = 17
          Caption = #19978#34892
          TabOrder = 5
          GroupIndex = 1
          LookAndFeel.SkinName = 'Lilian'
          Transparent = True
        end
        object rbDown: TcxRadioButton
          Left = 238
          Top = 161
          Width = 73
          Height = 17
          Caption = #19979#34892
          Checked = True
          TabOrder = 6
          TabStop = True
          GroupIndex = 1
          LookAndFeel.SkinName = 'Lilian'
          Transparent = True
        end
        object edtNewDestID: TcxTextEdit
          Left = 138
          Top = 117
          TabOrder = 7
          Text = '0'
          Width = 103
        end
      end
      object cxTabSheet5: TcxTabSheet
        Caption = #30005#23376#24037#21333
        ImageIndex = 5
        DesignSize = (
          648
          398)
        object btnSendSDSReply: TcxButton
          Left = 420
          Top = 14
          Width = 75
          Height = 25
          Caption = #21457#36865#22238#25191
          TabOrder = 0
          OnClick = btnSendSDSReplyClick
        end
        object btnGetSDSInfo: TcxButton
          Left = 308
          Top = 14
          Width = 75
          Height = 25
          Caption = #21047#26032
          TabOrder = 1
          OnClick = btnGetSDSInfoClick
        end
        object tlSDS: TcxTreeList
          Left = 6
          Top = 60
          Width = 622
          Height = 329
          Anchors = [akLeft, akTop, akRight, akBottom]
          Bands = <
            item
            end>
          LookAndFeel.NativeStyle = False
          Navigator.Buttons.CustomButtons = <>
          OptionsData.Editing = False
          OptionsView.GridLines = tlglBoth
          OptionsView.Indicator = True
          OptionsView.ShowRoot = False
          TabOrder = 2
          object Column1: TcxTreeListColumn
            Caption.Text = #32534#21495
            DataBinding.ValueType = 'Integer'
            Width = 60
            Position.ColIndex = 0
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object Column2: TcxTreeListColumn
            Caption.Text = #21457#36865#26102#38388
            DataBinding.ValueType = 'String'
            Width = 135
            Position.ColIndex = 1
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn3: TcxTreeListColumn
            Caption.Text = #31867#21035
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 2
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn4: TcxTreeListColumn
            Caption.Text = #26684#24335
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 3
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn5: TcxTreeListColumn
            Caption.Text = #24207#21495
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 4
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn6: TcxTreeListColumn
            Caption.Text = #21457#36865#32773
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 5
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn8: TcxTreeListColumn
            Caption.Text = #22238#25191#29366#24577
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 6
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn7: TcxTreeListColumn
            Caption.Text = #20869#23481
            DataBinding.ValueType = 'String'
            Width = 227
            Position.ColIndex = 7
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
        end
        object cbCoverClosed: TcxCheckBox
          Left = 168
          Top = 15
          Caption = #35774#22791#30422#20851#38381
          Properties.OnChange = cbLocalRTSPPropertiesChange
          State = cbsChecked
          TabOrder = 3
        end
      end
      object cxTabSheet4: TcxTabSheet
        Caption = #20027#26426#29366#24577
        ImageIndex = 6
        DesignSize = (
          648
          398)
        object btnGetMasterStatus: TcxButton
          Left = 300
          Top = 46
          Width = 75
          Height = 25
          Caption = #21047#26032
          TabOrder = 0
          OnClick = btnGetMasterStatusClick
        end
        object cxLabel14: TcxLabel
          Left = 88
          Top = 45
          Caption = 'DBA'#22320#22336
          Transparent = True
        end
        object edtDBAAddr: TcxTextEdit
          Left = 154
          Top = 44
          TabOrder = 2
          Text = '192.168.30.150'
          Width = 115
        end
        object mmDBAInfo: TcxMemo
          Left = 36
          Top = 96
          Anchors = [akLeft, akTop, akRight, akBottom]
          Properties.ScrollBars = ssVertical
          TabOrder = 3
          Height = 253
          Width = 470
        end
      end
      object cxTabSheet7: TcxTabSheet
        Caption = #21015#36710#36716#32452
        ImageIndex = 7
        object cxLabel15: TcxLabel
          Left = 84
          Top = 74
          Caption = #21015#36710#20301#32622
          Transparent = True
        end
        object cxButton3: TcxButton
          Left = 388
          Top = 75
          Width = 97
          Height = 25
          Caption = #36716#32452
          TabOrder = 1
          OnClick = cxButton3Click
        end
        object cbTrainPos: TcxComboBox
          Left = 166
          Top = 73
          TabOrder = 2
          Text = 'cbDCType'
          Width = 179
        end
        object btnRegister: TcxButton
          Left = 388
          Top = 16
          Width = 75
          Height = 25
          Caption = #27880#20876
          TabOrder = 3
          OnClick = btnRegisterClick
        end
        object btnUnregister: TcxButton
          Left = 488
          Top = 16
          Width = 75
          Height = 25
          Caption = #27880#38144
          TabOrder = 4
          OnClick = btnUnregisterClick
        end
        object cbDCType: TcxComboBox
          Left = 145
          Top = 14
          TabOrder = 5
          Text = 'cbDCType'
          Visible = False
          Width = 204
        end
      end
      object tsIPH: TcxTabSheet
        Caption = 'IPH'
        ImageIndex = 7
        object Panel2: TPanel
          Left = 0
          Top = 0
          Width = 648
          Height = 398
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          object btnIPHApply: TcxButton
            Left = 443
            Top = 276
            Width = 75
            Height = 25
            Caption = #23545#35762#30003#35831
            TabOrder = 0
            OnClick = btnIPHApplyClick
          end
          object cxGroupBox1: TcxGroupBox
            Left = 20
            Top = 16
            Caption = #35774#22791'1'
            TabOrder = 1
            Height = 209
            Width = 385
            object edtCarriageID1: TcxTextEdit
              Left = 100
              Top = 77
              TabOrder = 0
              Text = '1'
              Width = 121
            end
            object edtIPHDevID1: TcxTextEdit
              Left = 100
              Top = 109
              TabOrder = 1
              Text = '1'
              Width = 121
            end
            object cxLabel19: TcxLabel
              Left = 36
              Top = 78
              Caption = #36710#31665#21495
              Transparent = True
            end
            object cxLabel20: TcxLabel
              Left = 36
              Top = 110
              Caption = #25253#35686#22120
              Transparent = True
            end
            object cxLabel22: TcxLabel
              Left = 36
              Top = 142
              Caption = #35270#39057#27969
              Transparent = True
            end
            object edtIPHUrl1: TcxTextEdit
              Left = 100
              Top = 141
              TabOrder = 5
              Text = 'http://ivi.bupt.edu.cn/hls/cctv1.m3u8'
              Width = 265
            end
            object cxLabel31: TcxLabel
              Left = 36
              Top = 42
              Caption = #36710#20307#21495
              Transparent = True
            end
            object edtTrainUnitID1: TcxTextEdit
              Left = 100
              Top = 40
              TabOrder = 7
              Text = '18001'
              Width = 121
            end
          end
          object cxGroupBox2: TcxGroupBox
            Left = 428
            Top = 16
            Caption = #35774#22791'2'
            TabOrder = 2
            Height = 209
            Width = 385
            object edtCarriageID2: TcxTextEdit
              Left = 88
              Top = 77
              TabOrder = 0
              Text = '3'
              Width = 121
            end
            object edtIPHDevID2: TcxTextEdit
              Left = 88
              Top = 110
              TabOrder = 1
              Text = '5'
              Width = 121
            end
            object cxLabel25: TcxLabel
              Left = 28
              Top = 78
              Caption = #36710#31665#21495
              Transparent = True
            end
            object cxLabel26: TcxLabel
              Left = 28
              Top = 110
              Caption = #25253#35686#22120
              Transparent = True
            end
            object cxLabel28: TcxLabel
              Left = 28
              Top = 142
              Caption = #35270#39057#27969
              Transparent = True
            end
            object edtIPHUrl2: TcxTextEdit
              Left = 88
              Top = 141
              TabOrder = 5
              Text = 'http://ivi.bupt.edu.cn/hls/cctv3.m3u8'
              Width = 265
            end
            object cxLabel21: TcxLabel
              Left = 30
              Top = 47
              Caption = #36710#20307#21495
              Transparent = True
            end
            object edtTrainUnitID2: TcxTextEdit
              Left = 88
              Top = 43
              TabOrder = 7
              Text = '18002'
              Width = 121
            end
          end
          object cbLocalRTSP: TcxCheckBox
            Left = 96
            Top = 272
            Caption = #26412#22320#35270#39057#27969
            Properties.OnChange = cbLocalRTSPPropertiesChange
            TabOrder = 3
          end
          object cbBatchIPH: TcxCheckBox
            Left = 248
            Top = 272
            Caption = #25209#37327'IPH'
            Properties.OnChange = cbBatchIPHPropertiesChange
            TabOrder = 4
          end
        end
      end
    end
    object Panel1: TPanel
      Left = 0
      Top = 0
      Width = 658
      Height = 197
      Align = alTop
      TabOrder = 1
      object cxLabel1: TcxLabel
        Left = 24
        Top = 17
        Caption = #36710#20307#21495
        Transparent = True
      end
      object edtTrainUnitID: TcxTextEdit
        Left = 82
        Top = 16
        TabOrder = 1
        Text = '0'
        Width = 88
      end
      object cxLabel5: TcxLabel
        Left = 216
        Top = 18
        Caption = #32456#31471#21495#30721
        Transparent = True
      end
      object edtUser: TcxTextEdit
        Left = 290
        Top = 16
        TabOrder = 3
        Text = '7008'
        Width = 103
      end
      object cxLabel3: TcxLabel
        Left = 24
        Top = 51
        Caption = #36710#27425#21495
        Transparent = True
      end
      object edtTrainID: TcxTextEdit
        Left = 82
        Top = 50
        TabOrder = 5
        Text = '0'
        Width = 88
      end
      object cxLabel6: TcxLabel
        Left = 232
        Top = 51
        Caption = #24403#21069#32452
        Transparent = True
      end
      object edtCurrGroupID: TcxTextEdit
        Left = 290
        Top = 50
        TabOrder = 7
        Text = '0'
        Width = 103
      end
      object cxLabel11: TcxLabel
        Left = 428
        Top = 17
        Caption = #27880#20876#35843#24230#21488
        Transparent = True
      end
      object edtRegisteredDCType: TcxTextEdit
        Left = 529
        Top = 16
        TabOrder = 9
        Text = '0'
        Width = 88
      end
      object cxLabel7: TcxLabel
        Left = 25
        Top = 86
        Caption = #24403#21069#31449
        Transparent = True
      end
      object edtStationID: TcxTextEdit
        Left = 82
        Top = 84
        TabOrder = 11
        Text = '0'
        Width = 88
      end
      object edtCellID: TcxTextEdit
        Left = 528
        Top = 50
        TabOrder = 12
        Text = '0'
        Width = 105
      end
      object cxLabel8: TcxLabel
        Left = 446
        Top = 55
        Caption = #24403#21069#23567#21306
        Transparent = True
      end
      object cxLabel9: TcxLabel
        Left = 9
        Top = 120
        Caption = #21628#21483#31867#22411
        Transparent = True
      end
      object edtCallType: TcxTextEdit
        Left = 82
        Top = 119
        TabOrder = 15
        Text = '0'
        Width = 88
      end
      object cxLabel10: TcxLabel
        Left = 217
        Top = 120
        Caption = #21628#21483#21495#30721
        Transparent = True
      end
      object edtCallID: TcxTextEdit
        Left = 290
        Top = 119
        TabOrder = 17
        Text = '0'
        Width = 103
      end
      object cxLabel12: TcxLabel
        Left = 214
        Top = 86
        Caption = #36816#34892#26041#21521
        Transparent = True
      end
      object edtDirection: TcxTextEdit
        Left = 290
        Top = 84
        TabOrder = 19
        Text = '0'
        Width = 103
      end
      object cxLabel18: TcxLabel
        Left = 462
        Top = 85
        Caption = #32456#28857#31449
        Transparent = True
      end
      object edtDestID: TcxTextEdit
        Left = 530
        Top = 84
        TabOrder = 21
        Text = '0'
        Width = 103
      end
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Lilian'
    Left = 88
    Top = 224
  end
  object TimerCfg: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = TimerCfgTimer
    Left = 232
    Top = 216
  end
  object TimerATS: TTimer
    Interval = 3000
    OnTimer = TimerATSTimer
    Left = 228
    Top = 296
  end
  object TimerIPH: TTimer
    Enabled = False
    Interval = 2000
    OnTimer = TimerIPHTimer
    Left = 324
    Top = 304
  end
end
