inherited FormTest: TFormTest
  Caption = 'Test'
  ClientHeight = 619
  ClientWidth = 1130
  ExplicitWidth = 1146
  ExplicitHeight = 658
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 705
    Height = 619
    Align = alLeft
    ExplicitWidth = 705
    ExplicitHeight = 619
    object mmInfo: TcxMemo
      Left = 0
      Top = 401
      Align = alClient
      Properties.ScrollBars = ssVertical
      TabOrder = 0
      Height = 218
      Width = 705
    end
    object Panel3: TPanel
      Left = 0
      Top = 0
      Width = 705
      Height = 401
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 705
        Height = 57
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object edtUser: TcxTextEdit
          Left = 54
          Top = 18
          TabOrder = 0
          Text = '7008'
          Width = 91
        end
        object cxLabel1: TcxLabel
          Left = 16
          Top = 19
          Caption = #29992#25143
        end
        object cxLabel2: TcxLabel
          Left = 179
          Top = 19
          Caption = #32676#32452
        end
        object edtGroup: TcxTextEdit
          Left = 224
          Top = 18
          TabOrder = 3
          Text = '9999999999991'
          Width = 120
        end
        object cbAllowLog: TcxCheckBox
          Left = 362
          Top = 19
          Caption = #20801#35768#26085#24535
          TabOrder = 4
        end
        object btnClearLog: TcxButton
          Left = 463
          Top = 17
          Width = 114
          Height = 25
          Caption = #28165#31354'SDK'#26085#24535
          TabOrder = 5
          OnClick = btnClearLogClick
        end
        object cbTriggerEventReport: TcxCheckBox
          Left = 600
          Top = 18
          Caption = #20107#20214#35302#21457
          Properties.OnChange = cbTriggerEventReportPropertiesChange
          TabOrder = 6
          Transparent = True
        end
      end
      object pcMain: TcxPageControl
        Left = 0
        Top = 57
        Width = 705
        Height = 344
        Align = alClient
        TabOrder = 1
        Properties.ActivePage = tsSMS
        Properties.CustomButtons.Buttons = <>
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        ClientRectBottom = 338
        ClientRectLeft = 2
        ClientRectRight = 699
        ClientRectTop = 28
        object cxTabSheet1: TcxTabSheet
          Caption = #26597#35810
          ImageIndex = 0
          object btnGetGroup: TcxButton
            Left = 49
            Top = 88
            Width = 98
            Height = 25
            Caption = #26597#35810#32452
            TabOrder = 0
            OnClick = btnGetGroupClick
          end
          object btnGetUser: TcxButton
            Left = 49
            Top = 130
            Width = 98
            Height = 25
            Caption = #26597#35810#29992#25143
            TabOrder = 1
            OnClick = btnGetUserClick
          end
          object btnGeMIC: TcxButton
            Left = 24
            Top = 177
            Width = 123
            Height = 25
            Caption = #26597#35810#25196#22768#22120'/MIC'
            TabOrder = 2
            OnClick = btnGeMICClick
          end
          object btnLogin: TcxButton
            Left = 206
            Top = 21
            Width = 98
            Height = 25
            Caption = #30331#24405
            TabOrder = 3
            OnClick = btnLoginClick
          end
          object btnSkinCfg: TcxButton
            Left = 197
            Top = 172
            Width = 107
            Height = 25
            Caption = #30382#32932
            TabOrder = 4
            OnClick = btnSkinCfgClick
          end
          object btnGetGroupMember: TcxButton
            Left = 197
            Top = 130
            Width = 107
            Height = 25
            Caption = #26597#35810#32452#25104#21592
            TabOrder = 5
            OnClick = btnGetGroupMemberClick
          end
          object cxButton17: TcxButton
            Left = 197
            Top = 229
            Width = 107
            Height = 25
            Caption = 'SDK'#29256#26412
            TabOrder = 6
            OnClick = cxButton17Click
          end
          object cxButton12: TcxButton
            Left = 49
            Top = 224
            Width = 98
            Height = 25
            Caption = #26597#35810#37096#38376
            TabOrder = 7
            OnClick = cxButton12Click
          end
          object cxLabel8: TcxLabel
            Left = 46
            Top = 23
            Caption = #29992#25143
            Transparent = True
          end
          object edtLoginUser: TcxTextEdit
            Left = 98
            Top = 22
            TabOrder = 9
            Text = '13002'
            Width = 63
          end
          object cxLabel9: TcxLabel
            Left = 49
            Top = 47
            Caption = #23494#30721
            Transparent = True
          end
          object edtLoginPwd: TcxTextEdit
            Left = 98
            Top = 50
            TabOrder = 11
            Text = '13002'
            Width = 63
          end
          object btnLogout: TcxButton
            Left = 206
            Top = 57
            Width = 98
            Height = 25
            Caption = #30331#20986
            TabOrder = 12
            OnClick = btnLogoutClick
          end
          object cxButton16: TcxButton
            Left = 206
            Top = 88
            Width = 98
            Height = 25
            Caption = #26597#35810#38745#24577#32452
            TabOrder = 13
            OnClick = cxButton16Click
          end
        end
        object cxTabSheet2: TcxTabSheet
          Caption = #28857#21628
          ImageIndex = 1
          object btnP2PHangup: TcxButton
            Left = 77
            Top = 134
            Width = 75
            Height = 25
            Caption = #25346#26029
            TabOrder = 0
            OnClick = btnP2PHangupClick
          end
          object btnP2PReject: TcxButton
            Left = 77
            Top = 93
            Width = 75
            Height = 25
            Caption = #25298#32477
            TabOrder = 1
            OnClick = btnP2PRejectClick
          end
          object btnP2PRecv: TcxButton
            Left = 77
            Top = 54
            Width = 75
            Height = 25
            Caption = #25509#21548
            TabOrder = 2
            OnClick = btnP2PRecvClick
          end
          object btnP2PDial: TcxButton
            Left = 77
            Top = 15
            Width = 75
            Height = 25
            Caption = #28857#21628
            TabOrder = 3
            OnClick = btnP2PDialClick
          end
          object btnStartUserDiscreetListen: TcxButton
            Left = 77
            Top = 208
            Width = 100
            Height = 25
            Caption = #24320#22987#32540#23494#20390#21548
            TabOrder = 4
            OnClick = btnStartUserDiscreetListenClick
          end
          object btnStopUserDiscreetListen: TcxButton
            Left = 77
            Top = 264
            Width = 100
            Height = 25
            Caption = #32467#26463#32540#23494#20390#21548
            TabOrder = 5
            OnClick = btnStopUserDiscreetListenClick
          end
          object btnP2PTransfer: TcxButton
            Left = 447
            Top = 46
            Width = 75
            Height = 25
            Caption = #36716#25509
            TabOrder = 6
            OnClick = btnP2PTransferClick
          end
          object cxLabel3: TcxLabel
            Left = 236
            Top = 48
            Caption = #36716#25509#29992#25143
          end
          object edtTransferUser: TcxTextEdit
            Left = 302
            Top = 47
            TabOrder = 8
            Text = '7007'
            Width = 139
          end
        end
        object cxTabSheet3: TcxTabSheet
          Caption = #32452#21628
          ImageIndex = 2
          object btnJoinGroup: TcxButton
            Left = 61
            Top = 72
            Width = 100
            Height = 25
            Caption = #21152#20837#32452
            TabOrder = 0
            OnClick = btnJoinGroupClick
          end
          object btnSubGroup: TcxButton
            Left = 61
            Top = 23
            Width = 100
            Height = 25
            Caption = #35746#38405
            TabOrder = 1
            OnClick = btnSubGroupClick
          end
          object btnPTT: TcxButton
            Left = 61
            Top = 118
            Width = 100
            Height = 25
            Caption = #32452#21628'('#25250#26435#65289
            TabOrder = 2
            OnClick = btnPTTClick
          end
          object btnPTTRelease: TcxButton
            Left = 205
            Top = 118
            Width = 100
            Height = 25
            Caption = #37322#25918#35805#26435
            TabOrder = 3
            OnClick = btnPTTReleaseClick
          end
          object btnPTTHangup: TcxButton
            Left = 205
            Top = 170
            Width = 100
            Height = 25
            Caption = #36864#20986#32452#21628
            TabOrder = 4
            OnClick = btnPTTHangupClick
          end
          object btnGroupBreakoff: TcxButton
            Left = 61
            Top = 170
            Width = 100
            Height = 25
            Caption = #24378#25286
            TabOrder = 5
            OnClick = btnGroupBreakoffClick
          end
          object btnUnsubGroup: TcxButton
            Left = 205
            Top = 23
            Width = 100
            Height = 25
            Caption = #21435#35746#38405
            TabOrder = 6
            OnClick = btnUnsubGroupClick
          end
          object btnStartGroupDiscreetListen: TcxButton
            Left = 61
            Top = 216
            Width = 100
            Height = 25
            Caption = #24320#22987#32540#23494#20390#21548
            TabOrder = 7
            OnClick = btnStartGroupDiscreetListenClick
          end
          object btnStopGroupDiscreetListen: TcxButton
            Left = 205
            Top = 216
            Width = 100
            Height = 25
            Caption = #32467#26463#32540#23494#20390#21548
            TabOrder = 8
            OnClick = btnStopGroupDiscreetListenClick
          end
          object btnPTTEmergency: TcxButton
            Left = 205
            Top = 72
            Width = 100
            Height = 25
            Caption = #32039#24613#32452#21628
            TabOrder = 9
            OnClick = btnPTTEmergencyClick
          end
        end
        object cxTabSheet4: TcxTabSheet
          Caption = #35270#39057
          ImageIndex = 3
          object cxButton43: TcxButton
            Left = 22
            Top = 15
            Width = 75
            Height = 25
            Caption = #35270#39057#19978#25289
            TabOrder = 0
            OnClick = cxButton43Click
          end
          object cxButton44: TcxButton
            Left = 114
            Top = 15
            Width = 75
            Height = 25
            Caption = #20572#27490#35270#39057
            TabOrder = 1
            OnClick = cxButton44Click
          end
          object Panel7: TPanel
            Left = 22
            Top = 183
            Width = 220
            Height = 121
            Caption = 'Panel7'
            TabOrder = 2
          end
          object cxButton3: TcxButton
            Left = 234
            Top = 15
            Width = 75
            Height = 25
            Caption = #25509#25910#22238#20256
            TabOrder = 3
            OnClick = cxButton3Click
          end
          object cxButton5: TcxButton
            Left = 333
            Top = 15
            Width = 75
            Height = 25
            Caption = #25773#25918
            TabOrder = 4
            OnClick = cxButton5Click
          end
          object cxButton6: TcxButton
            Left = 252
            Top = 228
            Width = 75
            Height = 25
            Caption = #20999#25442
            TabOrder = 5
            OnClick = cxButton6Click
          end
          object cxButton7: TcxButton
            Left = 580
            Top = 236
            Width = 75
            Height = 25
            Caption = #20999#25442
            TabOrder = 6
            OnClick = cxButton7Click
          end
          object Panel2: TPanel
            Left = 333
            Top = 183
            Width = 220
            Height = 121
            Caption = 'Panel7'
            TabOrder = 7
          end
          object cxButton11: TcxButton
            Left = 348
            Top = 361
            Width = 75
            Height = 25
            Caption = 'URL'#19979#36733
            TabOrder = 8
          end
          object edtURL: TcxTextEdit
            Left = 36
            Top = 336
            AutoSize = False
            TabOrder = 9
            Text = 
              'https://dlc2.pconline.com.cn/filedown_5826_13676217/CXjo21ii/Fil' +
              'eZilla_3.46.0_win32_sponsored-setup.exe'
            Height = 25
            Width = 269
          end
          object cxButton31: TcxButton
            Left = 428
            Top = 15
            Width = 75
            Height = 25
            Caption = #36716#21457
            TabOrder = 10
            OnClick = cxButton31Click
          end
          object cxButton18: TcxButton
            Left = 22
            Top = 71
            Width = 75
            Height = 25
            Caption = #35270#39057#21628#21483
            TabOrder = 11
            OnClick = cxButton18Click
          end
        end
        object cxTabSheet5: TcxTabSheet
          Caption = #27966#25509
          ImageIndex = 4
          object btnDeletePatchMember: TcxButton
            Left = 243
            Top = 103
            Width = 99
            Height = 25
            Caption = #21024#38500#27966#25509#32452
            TabOrder = 0
            OnClick = btnDeletePatchMemberClick
          end
          object btnCreatePatchGroup: TcxButton
            Left = 243
            Top = 63
            Width = 99
            Height = 25
            Caption = #21019#24314#27966#25509#32452
            TabOrder = 1
            OnClick = btnCreatePatchGroupClick
          end
          object cxButton4: TcxButton
            Left = 243
            Top = 145
            Width = 99
            Height = 25
            Caption = #28155#21152#25104#21592
            TabOrder = 2
            OnClick = cxButton4Click
          end
          object edtPatchGroupMember: TcxTextEdit
            Left = 59
            Top = 162
            TabOrder = 3
            Text = '12001'
            Width = 102
          end
          object btnGetPatchGroup: TcxButton
            Left = 126
            Top = 16
            Width = 99
            Height = 25
            Caption = #26597#35810#27966#25509#32452
            TabOrder = 4
            OnClick = btnGetPatchGroupClick
          end
          object btnGetPatchMember: TcxButton
            Left = 243
            Top = 192
            Width = 99
            Height = 25
            Caption = #26597#35810#25104#21592
            TabOrder = 5
            OnClick = btnGetPatchMemberClick
          end
          object edtPatchGroupName: TcxTextEdit
            Left = 27
            Top = 64
            TabOrder = 6
            Text = '7008'
            Width = 182
          end
          object edtPatchGroupMember2: TcxTextEdit
            Left = 59
            Top = 202
            TabOrder = 7
            Text = '12004'
            Width = 102
          end
          object cxButton24: TcxButton
            Left = 383
            Top = 145
            Width = 99
            Height = 25
            Caption = #28165#31354#25104#21592
            TabOrder = 8
            OnClick = cxButton24Click
          end
          object btnConfirmGroupValid: TcxButton
            Left = 243
            Top = 248
            Width = 166
            Height = 25
            Caption = #26816#26597#25104#21592#32452#24050#32463#34987#27966#25509
            TabOrder = 9
            OnClick = btnConfirmGroupValidClick
          end
          object btnQueryPatchGroupInfo: TcxButton
            Left = 243
            Top = 288
            Width = 166
            Height = 25
            Caption = #26597#35810#27966#25509#32452#35814#32454#20449#24687
            TabOrder = 10
            OnClick = btnQueryPatchGroupInfoClick
          end
          object edtCheckGroup: TcxTextEdit
            Left = 59
            Top = 245
            TabOrder = 11
            Width = 102
          end
        end
        object cxTabSheet6: TcxTabSheet
          Caption = #21160#24577#32452
          ImageIndex = 5
          object btnGetGroups: TcxButton
            Left = 259
            Top = 35
            Width = 105
            Height = 25
            Caption = #26597#35810#21160#24577#32452
            TabOrder = 0
            OnClick = btnGetGroupsClick
          end
          object edtDnaUser: TcxTextEdit
            Left = 109
            Top = 36
            TabOrder = 1
            Text = '7007'
            Width = 86
          end
          object edtDnaGroup: TcxTextEdit
            Left = 109
            Top = 100
            TabOrder = 2
            Text = '7008'
            Width = 86
          end
          object btnCancelDynamicGroup: TcxButton
            Left = 259
            Top = 127
            Width = 105
            Height = 25
            Caption = #21024#38500#21160#24577#32452
            TabOrder = 3
            OnClick = btnCancelDynamicGroupClick
          end
          object btnGetGroupUsers: TcxButton
            Left = 397
            Top = 35
            Width = 100
            Height = 25
            Caption = #26597#35810#32452#29992#25143
            TabOrder = 4
            OnClick = btnGetGroupMemberClick
          end
          object edtDynamicGroup: TcxTextEdit
            Left = 105
            Top = 212
            TabOrder = 5
            Width = 86
          end
          object btnAddDynamicGroupMember: TcxButton
            Left = 259
            Top = 169
            Width = 105
            Height = 25
            Caption = #22686#21152#32452#25104#21592
            TabOrder = 6
            OnClick = btnAddDynamicGroupMemberClick
          end
          object btnDelDynamicGroupMember: TcxButton
            Left = 259
            Top = 215
            Width = 105
            Height = 25
            Caption = #21024#38500#32452#25104#21592
            TabOrder = 7
            OnClick = btnDelDynamicGroupMemberClick
          end
          object btnCreateDynamicGroup: TcxButton
            Left = 259
            Top = 83
            Width = 105
            Height = 25
            Caption = #21019#24314#21160#24577#32452
            TabOrder = 8
            OnClick = btnCreateDynamicGroupClick
          end
          object cxLabel5: TcxLabel
            Left = 40
            Top = 37
            Caption = #32456#31471#25104#21592
            Transparent = True
          end
          object cxLabel6: TcxLabel
            Left = 48
            Top = 213
            Caption = #32452'ID'
            Transparent = True
          end
          object cxLabel7: TcxLabel
            Left = 36
            Top = 171
            Caption = #32452#21517#31216
            Transparent = True
          end
          object edtGroupName: TcxTextEdit
            Left = 105
            Top = 170
            TabOrder = 12
            Width = 86
          end
          object cxButton13: TcxButton
            Left = 397
            Top = 127
            Width = 116
            Height = 25
            Caption = #25209#37327#21024#38500#21160#24577#32452
            TabOrder = 13
            OnClick = cxButton13Click
          end
          object cxLabel10: TcxLabel
            Left = 43
            Top = 100
            Caption = #32676#32452#25104#21592
            Transparent = True
          end
          object edtDnaUser2: TcxTextEdit
            Left = 109
            Top = 64
            TabOrder = 15
            Text = '7007'
            Width = 86
          end
          object edtDnaGroup2: TcxTextEdit
            Left = 109
            Top = 128
            TabOrder = 16
            Text = '7008'
            Width = 86
          end
          object edtDeletePeriod: TcxTextEdit
            Left = 397
            Top = 84
            TabOrder = 17
            Text = '10'
            Width = 86
          end
        end
        object cxTabSheet11: TcxTabSheet
          Caption = #22810#36873#32452
          ImageIndex = 10
          object cxButton14: TcxButton
            Left = 189
            Top = 46
            Width = 98
            Height = 25
            Caption = #21019#24314#22810#36873#32452
            TabOrder = 0
            OnClick = cxButton14Click
          end
          object cxButton15: TcxButton
            Left = 189
            Top = 87
            Width = 98
            Height = 25
            Caption = #21024#38500#22810#36873#32452
            TabOrder = 1
            OnClick = cxButton15Click
          end
          object edtMultiGroup1: TcxTextEdit
            Left = 76
            Top = 47
            TabOrder = 2
            Text = '11000'
            Width = 86
          end
          object edtMultiGroup2: TcxTextEdit
            Left = 76
            Top = 88
            TabOrder = 3
            Text = '13000'
            Width = 86
          end
          object cxLabel4: TcxLabel
            Left = 8
            Top = 48
            Caption = #32452#25104#21592
          end
          object cxButton32: TcxButton
            Left = 189
            Top = 142
            Width = 98
            Height = 25
            Caption = #26597#35810#22810#36873#32452
            TabOrder = 5
            OnClick = cxButton32Click
          end
        end
        object cxTabSheet7: TcxTabSheet
          Caption = #24405#38899
          ImageIndex = 6
          DesignSize = (
            697
            310)
          object cxButton9: TcxButton
            Left = 243
            Top = 29
            Width = 75
            Height = 25
            Caption = #21021#22987#21270
            TabOrder = 0
            OnClick = cxButton9Click
          end
          object cxButton10: TcxButton
            Left = 309
            Top = 161
            Width = 75
            Height = 25
            Caption = #26597#35810
            TabOrder = 1
            OnClick = cxButton10Click
          end
          object edtCallType: TcxTextEdit
            Left = 80
            Top = 77
            Hint = '0:'#28857#21628'/1:'#35270#39057#28857#21628'/2'#65306#35270#39057#22238#20256'/3'#65306#32452#21628
            ParentShowHint = False
            ShowHint = True
            TabOrder = 2
            Text = '3'
            TextHint = '0:'#28857#21628'/1:'#35270#39057#28857#21628'/2'#65306#35270#39057#22238#20256'/3'#65306#32452#21628
            Width = 129
          end
          object edtCaller: TcxTextEdit
            Left = 80
            Top = 105
            TabOrder = 3
            Text = '-1'
            Width = 129
          end
          object edtCallee: TcxTextEdit
            Left = 80
            Top = 133
            TabOrder = 4
            Text = '-1'
            Width = 129
          end
          object edtResourceID: TcxTextEdit
            Left = 80
            Top = 161
            TabOrder = 5
            Text = '61820000'
            Width = 129
          end
          object cxButton19: TcxButton
            Left = 387
            Top = 29
            Width = 75
            Height = 25
            Caption = #24320#22987#24405#38899
            TabOrder = 6
            OnClick = cxButton19Click
          end
          object cxButton20: TcxButton
            Left = 483
            Top = 29
            Width = 75
            Height = 25
            Caption = #32467#26463#24405#38899
            TabOrder = 7
            OnClick = cxButton20Click
          end
          object edtMRSAddr: TcxTextEdit
            Left = 94
            Top = 30
            TabOrder = 8
            Text = '172.19.139.4'
            Width = 124
          end
          object cxLabel11: TcxLabel
            Left = 14
            Top = 31
            Caption = #24405#38899#26381#21153#22120
            Transparent = True
          end
          object cxLabel15: TcxLabel
            Left = 3
            Top = 222
            Caption = #25991#20214'HTTP URL'
            Transparent = True
          end
          object edtHttpUrl: TcxTextEdit
            Left = 97
            Top = 222
            Anchors = [akLeft, akTop, akRight]
            TabOrder = 11
            Text = 
              'http://172.19.139.4:8000/ubp/rec/2020/10/30/17/61830509_20201030' +
              '173336/61830509_20201030173336_865733.mp4'
            Width = 461
          end
          object cxButton30: TcxButton
            Left = 579
            Top = 222
            Width = 75
            Height = 25
            Hint = #19979#36733#21040'C'#30424#26681#30446#24405
            Anchors = [akTop, akRight]
            Caption = #19979#36733
            TabOrder = 12
          end
          object cxLabel16: TcxLabel
            Left = 14
            Top = 78
            Caption = #21628#21483#31867#21035
            Transparent = True
          end
          object cxLabel17: TcxLabel
            Left = 14
            Top = 105
            Caption = #20027#21483
            Transparent = True
          end
          object cxLabel18: TcxLabel
            Left = 14
            Top = 133
            Caption = #34987#21483
            Transparent = True
          end
          object cxLabel19: TcxLabel
            Left = 14
            Top = 161
            Caption = #36164#28304#21495
            Transparent = True
          end
          object cxLabel20: TcxLabel
            Left = 243
            Top = 132
            Caption = #32467#26463#26102#38388
            Transparent = True
          end
          object cxLabel21: TcxLabel
            Left = 243
            Top = 104
            Caption = #24320#22987#26102#38388
            Transparent = True
          end
          object edtRecStartTime: TcxTextEdit
            Left = 309
            Top = 104
            TabOrder = 19
            Text = '2020-11-01 00:00:00'
            Width = 153
          end
          object edtRecEndTime: TcxTextEdit
            Left = 309
            Top = 132
            TabOrder = 20
            Text = '2020-11-02 00:00:00'
            Width = 153
          end
        end
        object cxTabSheet9: TcxTabSheet
          Caption = 'RTP'
          ImageIndex = 8
          object Label1: TLabel
            Left = 11
            Top = 15
            Width = 49
            Height = 14
            Caption = 'RTP URL'
          end
          object edtRTPUrl: TcxTextEdit
            Left = 66
            Top = 12
            TabOrder = 0
            Text = 'rtsp://192.168.30.50:8554/'
            Width = 226
          end
          object btnStartPreview: TcxButton
            Left = 298
            Top = 11
            Width = 75
            Height = 25
            Caption = #25773#25918
            TabOrder = 1
            OnClick = btnStartPreviewClick
          end
          object btnStopPreview: TcxButton
            Left = 379
            Top = 11
            Width = 75
            Height = 25
            Caption = #20572#27490
            TabOrder = 2
            OnClick = btnStopPreviewClick
          end
          object PanelRTP: TPanel
            Left = 37
            Top = 64
            Width = 372
            Height = 281
            BevelInner = bvLowered
            TabOrder = 3
          end
        end
        object cxTabSheet10: TcxTabSheet
          Caption = 'PCM'
          ImageIndex = 9
          object btnPlayPCM: TcxButton
            Left = 315
            Top = 83
            Width = 75
            Height = 25
            Caption = 'Sleep'#25773#25918
            TabOrder = 0
            OnClick = btnPlayPCMClick
          end
          object cxButton1: TcxButton
            Left = 138
            Top = 83
            Width = 75
            Height = 25
            Caption = '+'
            TabOrder = 1
          end
          object cxButton2: TcxButton
            Left = 218
            Top = 83
            Width = 75
            Height = 25
            Caption = '-'
            TabOrder = 2
          end
          object btnInitPCM: TcxButton
            Left = 27
            Top = 128
            Width = 75
            Height = 25
            Caption = #21021#22987#21270
            TabOrder = 3
            Visible = False
            OnClick = btnInitPCMClick
          end
          object cxButton28: TcxButton
            Left = 138
            Top = 128
            Width = 75
            Height = 25
            Caption = #23450#26102#22120#25773#25918
            TabOrder = 4
            OnClick = cxButton28Click
          end
          object btnPausePCM: TcxButton
            Left = 315
            Top = 128
            Width = 75
            Height = 25
            Caption = #26242#20572
            TabOrder = 5
            OnClick = btnPausePCMClick
          end
          object cxButton29: TcxButton
            Left = 396
            Top = 128
            Width = 75
            Height = 25
            Caption = #24674#22797
            TabOrder = 6
            OnClick = cxButton29Click
          end
          object btnStopPCM: TcxButton
            Left = 220
            Top = 128
            Width = 75
            Height = 25
            Caption = #32467#26463
            Enabled = False
            TabOrder = 7
            OnClick = btnStopPCMClick
          end
          object cxLabel28: TcxLabel
            Left = 78
            Top = 27
            Caption = #25991#20214
          end
          object edtWavFile: TcxTextEdit
            Left = 116
            Top = 26
            TabOrder = 9
            Text = 'd:\code_sz\bin\all.wav'
            Width = 418
          end
        end
        object tsSMS: TcxTabSheet
          Caption = #24425#20449
          ImageIndex = 11
          object cxGroupBox1: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            TabOrder = 0
            Height = 310
            Width = 697
            object btnSendSMS: TcxButton
              Left = 132
              Top = 183
              Width = 75
              Height = 25
              Caption = #21457#36865
              TabOrder = 0
              OnClick = btnSendSMSClick
            end
            object cxButton36: TcxButton
              Left = 285
              Top = 183
              Width = 75
              Height = 25
              Caption = #21457#36865#30701#20449
              TabOrder = 1
              Visible = False
              OnClick = cxButton36Click
            end
            object cxLabel22: TcxLabel
              Left = 81
              Top = 19
              Caption = #20869#23481
              Transparent = True
            end
            object cxLabel23: TcxLabel
              Left = 81
              Top = 52
              Caption = #38468#20214
              Transparent = True
            end
            object cxLabel24: TcxLabel
              Left = 67
              Top = 88
              Caption = #25509#25910#32456#31471
              Transparent = True
            end
            object cxLabel25: TcxLabel
              Left = 67
              Top = 128
              Caption = #25509#25910#32676#32452
              ParentColor = False
              ParentFont = False
              Style.Font.Charset = ANSI_CHARSET
              Style.Font.Color = clRed
              Style.Font.Height = -14
              Style.Font.Name = #23435#20307
              Style.Font.Style = []
              Style.LookAndFeel.NativeStyle = False
              Style.Shadow = False
              Style.TransparentBorder = True
              Style.IsFontAssigned = True
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.NativeStyle = False
              Transparent = True
            end
            object edtSMSFile: TcxButtonEdit
              Left = 132
              Top = 51
              ParentFont = False
              Properties.Buttons = <
                item
                  Default = True
                  Kind = bkEllipsis
                end>
              Properties.OnButtonClick = edtSMSFilePropertiesButtonClick
              Style.Font.Charset = ANSI_CHARSET
              Style.Font.Color = clHighlight
              Style.Font.Height = -14
              Style.Font.Name = #23435#20307
              Style.Font.Style = []
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Sharp'
              Style.IsFontAssigned = True
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Sharp'
              StyleFocused.LookAndFeel.NativeStyle = False
              StyleFocused.LookAndFeel.SkinName = 'Sharp'
              StyleHot.LookAndFeel.NativeStyle = False
              StyleHot.LookAndFeel.SkinName = 'Sharp'
              TabOrder = 6
              Text = 'd:\size.png'
              Width = 330
            end
            object edtSMSGroup1: TcxTextEdit
              Left = 133
              Top = 127
              ParentFont = False
              Style.Font.Charset = DEFAULT_CHARSET
              Style.Font.Color = clRed
              Style.Font.Height = -12
              Style.Font.Name = 'Tahoma'
              Style.Font.Style = []
              Style.HotTrack = True
              Style.IsFontAssigned = True
              TabOrder = 7
              Text = '12001'
              Width = 124
            end
            object edtSMSGroup2: TcxTextEdit
              Left = 285
              Top = 127
              TabOrder = 8
              Text = '12002'
              Width = 124
            end
            object edtSMSText: TcxTextEdit
              Left = 132
              Top = 18
              TabOrder = 9
              Text = 'hello 123'
              Width = 330
            end
            object edtSMSUser1: TcxTextEdit
              Left = 133
              Top = 87
              TabOrder = 10
              Text = '11001'
              Width = 124
            end
            object edtSMSUser2: TcxTextEdit
              Left = 285
              Top = 87
              TabOrder = 11
              Text = '11002'
              Width = 124
            end
          end
        end
        object cxTabSheet8: TcxTabSheet
          Caption = #24191#25773
          ImageIndex = 11
          object LabelBrd: TcxLabel
            Left = 288
            Top = 88
            Transparent = True
          end
          object cxLabel12: TcxLabel
            Left = 99
            Top = 28
            Caption = #24191#25773#30446#30340#21015#36710
            Transparent = True
          end
          object cbBrd: TcxCheckBox
            Left = 358
            Top = 30
            Caption = #24320#22987#24191#25773
            Properties.OnChange = cbBrdPropertiesChange
            TabOrder = 2
            Transparent = True
          end
          object edtBrdTrain: TcxTextEdit
            Left = 213
            Top = 27
            TabOrder = 3
            Text = '10101'
            Width = 121
          end
          object cxButton25: TcxButton
            Left = 312
            Top = 125
            Width = 145
            Height = 44
            Caption = #20174#21015#36710#24191#25773#31995#32479#33719#21462#39044#24405#38899#24191#25773#37197#32622
            TabOrder = 4
            WordWrap = True
          end
        end
        object cxTabSheet12: TcxTabSheet
          Caption = #25196#22768#22120
          ImageIndex = 12
          object cxButton21: TcxButton
            Left = 132
            Top = 86
            Width = 113
            Height = 25
            Caption = #35774#32622#32676#32452#25196#22768#22120
            TabOrder = 0
            OnClick = cxButton21Click
          end
          object cxButton22: TcxButton
            Left = 297
            Top = 86
            Width = 113
            Height = 25
            Caption = #35774#32622#32570#30465#25196#22768#22120
            TabOrder = 1
            OnClick = cxButton22Click
          end
          object cxLabel13: TcxLabel
            Left = 36
            Top = 35
            Caption = #30417#21548#25196#22768#22120
            Properties.Alignment.Horz = taRightJustify
            Properties.Alignment.Vert = taVCenter
            Transparent = True
            AnchorX = 110
            AnchorY = 44
          end
          object cbMonSpeaker: TcxComboBox
            Left = 128
            Top = 32
            Properties.DropDownListStyle = lsFixedList
            TabOrder = 3
            Width = 400
          end
          object cxButton23: TcxButton
            Left = 453
            Top = 86
            Width = 75
            Height = 25
            Caption = #21047#26032
            TabOrder = 4
            OnClick = cxButton23Click
          end
          object cxLabel14: TcxLabel
            Left = 18
            Top = 163
            Caption = #40614#20811#39118#38899#37327
            Properties.Alignment.Horz = taRightJustify
            Properties.Alignment.Vert = taVCenter
            Transparent = True
            AnchorX = 92
            AnchorY = 172
          end
          object cxButton26: TcxButton
            Left = 261
            Top = 209
            Width = 75
            Height = 25
            Caption = #33719#21462
            TabOrder = 6
            OnClick = cxButton26Click
          end
          object cbMic: TcxComboBox
            Left = 128
            Top = 161
            Properties.DropDownListStyle = lsFixedList
            TabOrder = 7
            Width = 400
          end
          object cxButton27: TcxButton
            Left = 357
            Top = 209
            Width = 75
            Height = 25
            Caption = #35774#32622
            TabOrder = 8
            OnClick = cxButton27Click
          end
          object edtMicVolume: TcxSpinEdit
            Left = 128
            Top = 215
            Properties.MaxValue = 100.000000000000000000
            TabOrder = 9
            Width = 101
          end
        end
        object cxTabSheet13: TcxTabSheet
          Caption = #25668#20687#22836
          ImageIndex = 13
          object cxButton8: TcxButton
            Left = 220
            Top = 96
            Width = 98
            Height = 25
            Caption = #26597#35810#25668#20687#22836
            TabOrder = 0
            OnClick = cxButton8Click
          end
          object cxButton37: TcxButton
            Left = 220
            Top = 144
            Width = 98
            Height = 25
            Caption = #25668#20687#22836#29366#24577
            TabOrder = 1
            OnClick = cxButton37Click
          end
        end
        object cxTabSheet14: TcxTabSheet
          Caption = #35270#39057#19978#22681
          ImageIndex = 14
          object cxLabel26: TcxLabel
            Left = 44
            Top = 33
            Caption = #35270#39057#22681#32534#21495
          end
          object edtVideoChanID: TcxTextEdit
            Left = 136
            Top = 32
            TabOrder = 1
            Text = '1'
            Width = 73
          end
          object cxButton33: TcxButton
            Left = 237
            Top = 31
            Width = 75
            Height = 25
            Caption = #35270#39057#19978#22681
            TabOrder = 2
            OnClick = cxButton33Click
          end
          object cxButton34: TcxButton
            Left = 237
            Top = 67
            Width = 75
            Height = 25
            Caption = #32467#26463#19978#22681
            TabOrder = 3
            OnClick = cxButton34Click
          end
          object cxLabel27: TcxLabel
            Left = 44
            Top = 69
            Caption = #35270#39057#28304#21495#30721
          end
          object edtVWallUserID: TcxTextEdit
            Left = 136
            Top = 68
            TabOrder = 5
            Width = 73
          end
          object cxButton35: TcxButton
            Left = 237
            Top = 123
            Width = 75
            Height = 25
            Caption = #26597#35810
            TabOrder = 6
            OnClick = cxButton35Click
          end
        end
      end
    end
  end
  object mmSDKEvent: TcxMemo
    Left = 711
    Top = 0
    Align = alClient
    Properties.ScrollBars = ssVertical
    TabOrder = 1
    Height = 619
    Width = 419
  end
  object cxSplitter1: TcxSplitter
    Left = 705
    Top = 0
    Width = 6
    Height = 619
  end
  object TimerDynamicGroup: TTimer
    Enabled = False
    Interval = 200
    Left = 564
    Top = 192
  end
  object dxBarManager1: TdxBarManager
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
    PopupMenuLinks = <>
    UseSystemFont = True
    Left = 616
    Top = 164
    PixelsPerInch = 96
  end
  object TimerCheckRTP: TTimer
    Interval = 500
    Left = 604
    Top = 256
  end
  object cxPropertiesStore1: TcxPropertiesStore
    Components = <>
    StorageName = 'cxPropertiesStore1'
    Left = 536
    Top = 152
  end
  object TimerStartBrd: TTimer
    Enabled = False
    Interval = 20000
    OnTimer = TimerStartBrdTimer
    Left = 546
    Top = 109
  end
  object TimerStopBrd: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = TimerStopBrdTimer
    Left = 630
    Top = 105
  end
  object TimerPlayPCM: TTimer
    Enabled = False
    Interval = 90
    OnTimer = TimerPlayPCMTimer
    Left = 658
    Top = 219
  end
  object OpenDialog: TOpenDialog
    Left = 536
    Top = 288
  end
end
