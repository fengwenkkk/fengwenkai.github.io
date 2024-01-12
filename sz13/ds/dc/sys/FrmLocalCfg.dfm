inherited FormLocalCfg: TFormLocalCfg
  ClientHeight = 608
  ClientWidth = 976
  Font.Name = 'Arial'
  OnCreate = FormCreate
  ExplicitWidth = 992
  ExplicitHeight = 647
  PixelsPerInch = 96
  TextHeight = 16
  inherited MainPanel: TPanel
    Width = 976
    Height = 608
    ExplicitWidth = 976
    ExplicitHeight = 608
    object sbMain: TcxScrollBox
      Left = 0
      Top = 0
      Width = 976
      Height = 608
      Align = alClient
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Transparent = True
    end
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      PanelStyle.Active = True
      Style.BorderStyle = ebs3D
      Style.Edges = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 1
      Height = 608
      Width = 976
      object pcMain: TcxPageControl
        Left = 1
        Top = 1
        Width = 974
        Height = 606
        Align = alClient
        TabOrder = 0
        Properties.ActivePage = tsParam
        Properties.CustomButtons.Buttons = <>
        Properties.TabWidth = 120
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        OnPageChanging = pcMainPageChanging
        ClientRectBottom = 600
        ClientRectLeft = 2
        ClientRectRight = 968
        ClientRectTop = 30
        object cxTabSheet1: TcxTabSheet
          Caption = #22522#26412#37197#32622
          ImageIndex = 1
          object gbBasicCfgMain: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            PanelStyle.Active = True
            Style.BorderStyle = ebs3D
            Style.Edges = []
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 0
            OnResize = gbBasicCfgMainResize
            Height = 570
            Width = 966
            object gbBasicCfg: TcxGroupBox
              Left = 216
              Top = 88
              PanelStyle.Active = True
              ParentFont = False
              Style.BorderStyle = ebs3D
              Style.Edges = []
              Style.Font.Charset = ANSI_CHARSET
              Style.Font.Color = clWindowText
              Style.Font.Height = -14
              Style.Font.Name = 'Arial'
              Style.Font.Style = []
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Office2010Blue'
              Style.IsFontAssigned = True
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
              TabOrder = 0
              DesignSize = (
                489
                313)
              Height = 313
              Width = 489
              object btnCancelBasic: TcxButton
                Left = 296
                Top = 285
                Width = 75
                Height = 25
                Caption = #25918#24323
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 14
                Visible = False
                OnClick = btnCancelBasicClick
              end
              object btnRefresh: TcxButton
                Left = 188
                Top = 261
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #21047#26032
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 6
                OnClick = btnRefreshClick
              end
              object btnSetBasic: TcxButton
                Left = 403
                Top = 285
                Width = 75
                Height = 25
                Caption = #35774#32622
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 7
                Visible = False
              end
              object cxLabel10: TcxLabel
                Left = 69
                Top = 179
                Caption = 'NTP'#26381#21153#22120#22320#22336
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 189
              end
              object cxLabel5: TcxLabel
                Left = 67
                Top = 29
                Caption = 'CAD'#26381#21153#22120#22320#22336
                ParentFont = False
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -14
                Style.Font.Name = 'Arial'
                Style.Font.Style = []
                Style.IsFontAssigned = True
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 39
              end
              object cxLabel6: TcxLabel
                Left = 68
                Top = 59
                Caption = #38598#32676#26381#21153#22120#22320#22336
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 69
              end
              object cxLabel7: TcxLabel
                Left = 62
                Top = 89
                Caption = 'ATS'#26381#21153#22120'1'#22320#22336
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 99
              end
              object cxLabel8: TcxLabel
                Left = 62
                Top = 119
                Caption = 'ATS'#26381#21153#22120'2'#22320#22336
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 129
              end
              object cxLabel9: TcxLabel
                Left = 64
                Top = 149
                Caption = 'OMC'#26381#21153#22120#22320#22336
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 159
              end
              object edtATSAddr1: TcxTextEdit
                Left = 188
                Top = 87
                Properties.ReadOnly = True
                TabOrder = 2
                Width = 221
              end
              object edtATSAddr2: TcxTextEdit
                Left = 188
                Top = 117
                Properties.ReadOnly = True
                TabOrder = 3
                Width = 221
              end
              object edtDISAddr: TcxTextEdit
                Left = 188
                Top = 27
                ParentFont = False
                Properties.ReadOnly = True
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -14
                Style.Font.Name = 'Arial'
                Style.Font.Style = []
                Style.IsFontAssigned = True
                TabOrder = 0
                Width = 221
              end
              object edtMDCAddr: TcxTextEdit
                Left = 188
                Top = 57
                Properties.ReadOnly = True
                TabOrder = 1
                Width = 221
              end
              object edtNTPAddr: TcxTextEdit
                Left = 188
                Top = 177
                Properties.ReadOnly = True
                TabOrder = 5
                Width = 221
              end
              object edtOMCAddr: TcxTextEdit
                Left = 188
                Top = 147
                Properties.ReadOnly = True
                TabOrder = 4
                Width = 221
              end
              object cxLabel25: TcxLabel
                Left = 66
                Top = 209
                Caption = 'NTP'#21516#27493#21608#26399'(s)'
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 219
              end
              object edtNTPSyncPeriod: TcxTextEdit
                Left = 188
                Top = 207
                Properties.ReadOnly = True
                TabOrder = 16
                Width = 221
              end
            end
          end
        end
        object tsHistory: TcxTabSheet
          Caption = #22768#38899#37197#32622
          ImageIndex = 2
          object gbAudioCfgMain: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            PanelStyle.Active = True
            Style.BorderStyle = ebs3D
            Style.Edges = []
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 0
            OnResize = gbAudioCfgMainResize
            Height = 570
            Width = 966
            object gbAudioCfg: TcxGroupBox
              Left = 46
              Top = 56
              PanelStyle.Active = True
              Style.BorderStyle = ebs3D
              Style.Edges = []
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Office2010Blue'
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
              TabOrder = 0
              DesignSize = (
                731
                353)
              Height = 353
              Width = 731
              object btnCancelAudio: TcxButton
                Left = 433
                Top = 298
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #25918#24323
                TabOrder = 15
                OnClick = btnCancelAudioClick
              end
              object btnSetAudio: TcxButton
                Left = 333
                Top = 298
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #35774#32622
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 14
                OnClick = btnSetAudioClick
              end
              object cbMainMic: TcxComboBox
                Left = 136
                Top = 58
                Properties.DropDownListStyle = lsFixedList
                TabOrder = 2
                Width = 400
              end
              object cbSlaveMic: TcxComboBox
                Left = 136
                Top = 118
                Properties.DropDownListStyle = lsFixedList
                TabOrder = 6
                Width = 400
              end
              object cbSlaveSpeaker: TcxComboBox
                Left = 136
                Top = 88
                Properties.DropDownListStyle = lsFixedList
                TabOrder = 4
                Width = 400
              end
              object cbMainSpeaker: TcxComboBox
                Left = 136
                Top = 28
                Properties.DropDownListStyle = lsFixedList
                TabOrder = 0
                Width = 400
              end
              object cxLabel1: TcxLabel
                Left = 69
                Top = 30
                Caption = #20027#25196#22768#22120
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 40
              end
              object cxLabel2: TcxLabel
                Left = 575
                Top = 30
                Anchors = [akTop, akRight]
                Caption = #38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 607
                AnchorY = 40
              end
              object cxLabel3: TcxLabel
                Left = 69
                Top = 60
                Caption = #20027#40614#20811#39118
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 70
              end
              object btnRefreshSpeaker: TcxButton
                Left = 224
                Top = 298
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #21047#26032
                TabOrder = 13
                OnClick = btnRefreshSpeakerClick
              end
              object cxLabel11: TcxLabel
                Left = 69
                Top = 90
                Caption = #21103#25196#22768#22120
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 100
              end
              object cbMonSpeaker: TcxComboBox
                Left = 136
                Top = 148
                Properties.DropDownListStyle = lsFixedList
                TabOrder = 8
                Width = 400
              end
              object cxLabel13: TcxLabel
                Left = 69
                Top = 120
                Caption = #21103#40614#20811#39118
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 130
              end
              object cxLabel15: TcxLabel
                Left = 55
                Top = 150
                Caption = #30417#21548#25196#22768#22120
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 160
              end
              object edtMainSpeakerVolume: TcxSpinEdit
                Left = 621
                Top = 28
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 1
                Value = 100
                Width = 61
              end
              object cxLabel4: TcxLabel
                Left = 575
                Top = 60
                Anchors = [akTop, akRight]
                Caption = #38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 607
                AnchorY = 70
              end
              object edtMainMicVolume: TcxSpinEdit
                Left = 621
                Top = 58
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 3
                Value = 100
                Width = 61
              end
              object cxLabel12: TcxLabel
                Left = 69
                Top = 210
                Anchors = [akTop, akRight]
                Caption = #24191#25773#38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 220
              end
              object edtSlaveSpeakerVolume: TcxSpinEdit
                Left = 621
                Top = 88
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 5
                Value = 100
                Width = 61
              end
              object cxLabel17: TcxLabel
                Left = 575
                Top = 90
                Anchors = [akTop, akRight]
                Caption = #38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 607
                AnchorY = 100
              end
              object edtSlaveMicVolume: TcxSpinEdit
                Left = 621
                Top = 118
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 7
                Value = 100
                Width = 61
              end
              object cxLabel18: TcxLabel
                Left = 575
                Top = 120
                Anchors = [akTop, akRight]
                Caption = #38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 607
                AnchorY = 130
              end
              object edtMonSpeakerVolume: TcxSpinEdit
                Left = 621
                Top = 148
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 9
                Value = 100
                Width = 61
              end
              object cxLabel14: TcxLabel
                Left = 575
                Top = 150
                Anchors = [akTop, akRight]
                Caption = #38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 607
                AnchorY = 160
              end
              object edtRealBrdVolume: TcxSpinEdit
                Left = 135
                Top = 208
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 11
                Value = 80
                Width = 61
              end
              object cxLabel16: TcxLabel
                Left = 55
                Top = 180
                Anchors = [akTop, akRight]
                Caption = #25552#31034#38899#38899#37327
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 190
              end
              object edtVoiceAlarmVolume: TcxSpinEdit
                Left = 136
                Top = 178
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 10
                Value = 100
                Width = 61
              end
              object cxLabel19: TcxLabel
                Left = 35
                Top = 240
                Caption = #20056#23458#23545#35762#22686#30410
                ParentFont = False
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -15
                Style.Font.Name = #23435#20307
                Style.Font.Style = []
                Style.IsFontAssigned = True
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 129
                AnchorY = 250
              end
              object edtIPHMicVolumeRate: TcxSpinEdit
                Left = 136
                Top = 237
                Anchors = [akTop, akRight]
                Properties.LargeIncrement = 5.000000000000000000
                Properties.MaxValue = 100.000000000000000000
                Properties.MinValue = 1.000000000000000000
                TabOrder = 12
                Value = 1
                Width = 61
              end
              object cxLabel24: TcxLabel
                Left = 205
                Top = 240
                Caption = 'dB'
                ParentFont = False
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -15
                Style.Font.Name = #23435#20307
                Style.Font.Style = []
                Style.IsFontAssigned = True
                Properties.Alignment.Horz = taLeftJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorY = 250
              end
            end
          end
        end
        object tsParam: TcxTabSheet
          Caption = #19994#21153#21442#25968
          ImageIndex = 3
          object gbServiceCfgMain: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            PanelStyle.Active = True
            Style.BorderStyle = ebs3D
            Style.Edges = []
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 0
            OnResize = gbServiceCfgMainResize
            Height = 570
            Width = 966
            object gbServiceCfg: TcxGroupBox
              Left = 220
              Top = 92
              PanelStyle.Active = True
              ParentFont = False
              Style.BorderStyle = ebs3D
              Style.Edges = []
              Style.Font.Charset = ANSI_CHARSET
              Style.Font.Color = clWindowText
              Style.Font.Height = -14
              Style.Font.Name = 'Arial'
              Style.Font.Style = []
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Office2010Blue'
              Style.IsFontAssigned = True
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
              TabOrder = 0
              DesignSize = (
                453
                215)
              Height = 215
              Width = 453
              object btnCancelParam: TcxButton
                Left = 304
                Top = 163
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #25918#24323
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 11
                OnClick = btnCancelParamClick
              end
              object btnSetParam: TcxButton
                Left = 188
                Top = 163
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #35774#32622
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 6
                OnClick = btnSetParamClick
              end
              object cxLabel20: TcxLabel
                Left = 82
                Top = 29
                Caption = #26174#31034#21628#21483#24377#31383
                ParentFont = False
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -14
                Style.Font.Name = 'Arial'
                Style.Font.Style = []
                Style.IsFontAssigned = True
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 39
              end
              object cxLabel21: TcxLabel
                Left = 82
                Top = 59
                Caption = #35270#39057#36716#21457#26684#24335
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 69
              end
              object cxLabel22: TcxLabel
                Left = 26
                Top = 89
                Caption = #20056#23458#23545#35762#35270#39057#20256#36755#21327#35758
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 99
              end
              object cxLabel23: TcxLabel
                Left = 54
                Top = 119
                Caption = #20056#23458#23545#35762#24405#38899#27169#24335
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 129
              end
              object cbShowCallWin: TcxCheckBox
                Left = 188
                Top = 29
                Caption = #26159
                State = cbsChecked
                TabOrder = 0
                Transparent = True
              end
              object cbOriginalVideoFormat: TcxCheckBox
                Left = 188
                Top = 59
                Caption = #21407#22987#26684#24335
                TabOrder = 1
                Transparent = True
              end
              object cbIPHRecMergeMode: TcxCheckBox
                Left = 188
                Top = 119
                Caption = #24038#21491#22768#36947#21512#24182
                Enabled = False
                State = cbsChecked
                TabOrder = 4
                Transparent = True
              end
              object rbIPHVideoTCPMode: TcxRadioButton
                Left = 188
                Top = 91
                Width = 80
                Height = 21
                Caption = 'TCP'
                Checked = True
                Enabled = False
                TabOrder = 2
                TabStop = True
                Transparent = True
              end
              object rbIPHVideoUDPMode: TcxRadioButton
                Left = 307
                Top = 91
                Width = 80
                Height = 21
                Caption = 'UDP'
                Enabled = False
                TabOrder = 3
                Transparent = True
              end
              object btnDefaultParam: TcxButton
                Left = 40
                Top = 163
                Width = 111
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #24674#22797#32570#30465#20540
                TabOrder = 5
                OnClick = btnDefaultParamClick
              end
            end
          end
        end
        object tsSMS: TcxTabSheet
          Caption = #32456#31471#30701#28040#24687#27169#26495
          ImageIndex = 2
        end
        object tsSetPwd: TcxTabSheet
          Caption = #35774#32622#30331#24405#23494#30721
          ImageIndex = 4
          object cxGroupBox2: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            PanelStyle.Active = True
            ParentFont = False
            Style.BorderStyle = ebs3D
            Style.Edges = []
            Style.Font.Charset = ANSI_CHARSET
            Style.Font.Color = clWindowText
            Style.Font.Height = -14
            Style.Font.Name = 'Arial'
            Style.Font.Style = []
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            Style.IsFontAssigned = True
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 0
            Height = 570
            Width = 966
            object gbSetPwd: TcxGroupBox
              Left = 121
              Top = 105
              PanelStyle.Active = True
              Style.BorderStyle = ebs3D
              Style.Edges = []
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Office2010Blue'
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
              TabOrder = 0
              OnResize = gbSetPwdResize
              DesignSize = (
                487
                215)
              Height = 215
              Width = 487
              object btnSetPwd: TcxButton
                Left = 188
                Top = 167
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #35774#32622
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 3
                OnClick = btnSetPwdClick
              end
              object cxLabel26: TcxLabel
                Left = 138
                Top = 29
                Caption = #29992#25143
                ParentFont = False
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -14
                Style.Font.Name = 'Arial'
                Style.Font.Style = []
                Style.IsFontAssigned = True
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 39
              end
              object cxLabel27: TcxLabel
                Left = 110
                Top = 59
                Caption = #24403#21069#23494#30721
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 69
              end
              object cxLabel28: TcxLabel
                Left = 124
                Top = 89
                Caption = #26032#23494#30721
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 99
              end
              object cxLabel29: TcxLabel
                Left = 96
                Top = 119
                Caption = #30830#35748#26032#23494#30721
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 129
              end
              object edtNewPwd: TcxTextEdit
                Left = 188
                Top = 86
                ParentFont = False
                Properties.AutoSelect = False
                Properties.EchoMode = eemPassword
                Properties.HideSelection = False
                Properties.MaxLength = 31
                Properties.PasswordChar = #9679
                Style.BorderStyle = ebs3D
                Style.Font.Charset = DEFAULT_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -15
                Style.Font.Name = 'Tahoma'
                Style.Font.Style = []
                Style.LookAndFeel.NativeStyle = False
                Style.LookAndFeel.SkinName = 'Blue'
                Style.TextColor = 6307888
                Style.IsFontAssigned = True
                StyleDisabled.LookAndFeel.NativeStyle = False
                StyleDisabled.LookAndFeel.SkinName = 'Blue'
                StyleFocused.LookAndFeel.NativeStyle = False
                StyleFocused.LookAndFeel.SkinName = 'Blue'
                StyleHot.LookAndFeel.NativeStyle = False
                StyleHot.LookAndFeel.SkinName = 'Blue'
                TabOrder = 1
                Width = 221
              end
              object edtNewPwd2: TcxTextEdit
                Left = 188
                Top = 116
                ParentFont = False
                Properties.AutoSelect = False
                Properties.EchoMode = eemPassword
                Properties.HideSelection = False
                Properties.MaxLength = 31
                Properties.PasswordChar = #9679
                Style.BorderStyle = ebs3D
                Style.Font.Charset = DEFAULT_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -15
                Style.Font.Name = 'Tahoma'
                Style.Font.Style = []
                Style.LookAndFeel.NativeStyle = False
                Style.LookAndFeel.SkinName = 'Blue'
                Style.TextColor = 6307888
                Style.IsFontAssigned = True
                StyleDisabled.LookAndFeel.NativeStyle = False
                StyleDisabled.LookAndFeel.SkinName = 'Blue'
                StyleFocused.LookAndFeel.NativeStyle = False
                StyleFocused.LookAndFeel.SkinName = 'Blue'
                StyleHot.LookAndFeel.NativeStyle = False
                StyleHot.LookAndFeel.SkinName = 'Blue'
                TabOrder = 2
                Width = 221
              end
              object edtOldPwd: TcxTextEdit
                Left = 188
                Top = 56
                ParentFont = False
                Properties.AutoSelect = False
                Properties.EchoMode = eemPassword
                Properties.HideSelection = False
                Properties.MaxLength = 31
                Properties.PasswordChar = #9679
                Style.BorderStyle = ebs3D
                Style.Font.Charset = DEFAULT_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -15
                Style.Font.Name = 'Tahoma'
                Style.Font.Style = []
                Style.LookAndFeel.NativeStyle = False
                Style.LookAndFeel.SkinName = 'Blue'
                Style.TextColor = 6307888
                Style.IsFontAssigned = True
                StyleDisabled.LookAndFeel.NativeStyle = False
                StyleDisabled.LookAndFeel.SkinName = 'Blue'
                StyleFocused.LookAndFeel.NativeStyle = False
                StyleFocused.LookAndFeel.SkinName = 'Blue'
                StyleHot.LookAndFeel.NativeStyle = False
                StyleHot.LookAndFeel.SkinName = 'Blue'
                TabOrder = 0
                Width = 221
              end
              object edtUser: TcxTextEdit
                Left = 188
                Top = 27
                ParentFont = False
                Properties.ReadOnly = True
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -14
                Style.Font.Name = 'Arial'
                Style.Font.Style = []
                Style.IsFontAssigned = True
                TabOrder = 8
                Text = '192.168.30.177'
                Width = 221
              end
              object btnCancelPwd: TcxButton
                Left = 304
                Top = 167
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #25918#24323
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 9
                OnClick = btnCancelPwdClick
              end
            end
          end
        end
      end
    end
  end
end
