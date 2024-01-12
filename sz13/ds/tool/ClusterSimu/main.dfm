inherited FormMain: TFormMain
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Cluster'
  ClientHeight = 352
  ClientWidth = 628
  Position = poScreenCenter
  ExplicitWidth = 644
  ExplicitHeight = 391
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 628
    Height = 352
    ExplicitWidth = 628
    ExplicitHeight = 352
    object Panel2: TPanel
      Left = 0
      Top = 0
      Width = 628
      Height = 352
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      object gb1: TcxGroupBox
        Left = 20
        Top = 65
        Caption = #20027#26426
        Enabled = False
        TabOrder = 0
        Height = 264
        Width = 277
        object edtStatus1: TcxTextEdit
          Left = 104
          Top = 37
          TabOrder = 0
          Width = 121
        end
        object edtPeerStatus1: TcxTextEdit
          Left = 104
          Top = 77
          TabOrder = 1
          Width = 121
        end
        object cxLabel2: TcxLabel
          Left = 32
          Top = 38
          Caption = #24403#21069#29366#24577
          Transparent = True
        end
        object cxLabel15: TcxLabel
          Left = 32
          Top = 78
          Caption = #23545#31471#29366#24577
          Transparent = True
        end
        object cbOffline1: TCheckBox
          Left = 104
          Top = 128
          Width = 105
          Height = 17
          Caption = #31163#32447
          TabOrder = 4
        end
        object cbDisableSend1: TCheckBox
          Left = 104
          Top = 161
          Width = 105
          Height = 17
          Caption = #19981#21457#21253
          TabOrder = 5
          OnClick = cbDisableSend1Click
        end
        object cbDisableRecv1: TCheckBox
          Left = 104
          Top = 192
          Width = 105
          Height = 17
          Caption = #19981#25910#21253
          TabOrder = 6
          OnClick = cbDisableRecv1Click
        end
      end
      object cxLabel1: TcxLabel
        Left = 52
        Top = 26
        Caption = #20027#26426#22320#22336
        Transparent = True
      end
      object edtMasterAddr: TcxTextEdit
        Left = 118
        Top = 25
        TabOrder = 2
        Text = '192.168.2.1'
        Width = 121
      end
      object gb2: TcxGroupBox
        Left = 324
        Top = 65
        Caption = #22791#26426
        Enabled = False
        TabOrder = 3
        Height = 264
        Width = 277
        object edtStatus2: TcxTextEdit
          Left = 104
          Top = 37
          TabOrder = 0
          Width = 121
        end
        object edtPeerStatus2: TcxTextEdit
          Left = 104
          Top = 77
          TabOrder = 1
          Width = 121
        end
        object cxLabel3: TcxLabel
          Left = 32
          Top = 38
          Caption = #24403#21069#29366#24577
          Transparent = True
        end
        object cxLabel4: TcxLabel
          Left = 32
          Top = 78
          Caption = #23545#31471#29366#24577
          Transparent = True
        end
        object cbOffline2: TCheckBox
          Left = 104
          Top = 128
          Width = 105
          Height = 17
          Caption = #31163#32447
          TabOrder = 4
        end
        object cbDisableRecv2: TCheckBox
          Left = 104
          Top = 192
          Width = 105
          Height = 17
          Caption = #19981#25910#21253
          TabOrder = 5
          OnClick = cbDisableRecv2Click
        end
        object cbDisableSend2: TCheckBox
          Left = 104
          Top = 161
          Width = 105
          Height = 17
          Caption = #19981#21457#21253
          TabOrder = 6
          OnClick = cbDisableSend2Click
        end
      end
    end
  end
  object TimerCheck: TTimer
    Interval = 1
    OnTimer = TimerCheckTimer
    Left = 392
    Top = 4
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Blue'
    Left = 504
    Top = 8
  end
end
