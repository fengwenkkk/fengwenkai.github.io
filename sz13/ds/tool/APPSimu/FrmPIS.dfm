inherited FormPIS: TFormPIS
  Caption = 'PIS'
  ClientHeight = 426
  ClientWidth = 813
  WindowState = wsMaximized
  ExplicitLeft = -40
  ExplicitTop = -22
  ExplicitWidth = 829
  ExplicitHeight = 465
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 813
    Height = 426
    ExplicitWidth = 813
    ExplicitHeight = 426
    object btnIPHApply: TcxButton
      Left = 420
      Top = 372
      Width = 75
      Height = 25
      Caption = #23545#35762#30003#35831
      TabOrder = 0
      OnClick = btnIPHApplyClick
    end
    object cxGroupBox1: TcxGroupBox
      Left = 16
      Top = 37
      Caption = #21015#36710'1'
      TabOrder = 1
      Height = 300
      Width = 297
      object Label1: TLabel
        Left = 28
        Top = 40
        Width = 42
        Height = 14
        Caption = #36710#20307#21495
      end
      object Label2: TLabel
        Left = 28
        Top = 84
        Width = 42
        Height = 14
        Caption = #36710#31665#21495
      end
      object Label3: TLabel
        Left = 28
        Top = 128
        Width = 42
        Height = 14
        Caption = #25253#35686#22120
      end
      object Label7: TLabel
        Left = 28
        Top = 216
        Width = 42
        Height = 14
        Caption = #35843#24230#21488
      end
      object edtTrainUnitID1: TcxTextEdit
        Left = 92
        Top = 37
        TabOrder = 0
        Text = '1'
        Width = 121
      end
      object edtCarriageID1: TcxTextEdit
        Left = 92
        Top = 81
        TabOrder = 1
        Text = '3'
        Width = 121
      end
      object edtIPHDevID1: TcxTextEdit
        Left = 92
        Top = 125
        TabOrder = 2
        Text = '5'
        Width = 121
      end
      object btnIPHAlarm1: TcxButton
        Left = 92
        Top = 170
        Width = 75
        Height = 25
        Caption = #25253#35686
        TabOrder = 3
      end
      object btnIPHhandup1: TcxButton
        Left = 180
        Top = 170
        Width = 75
        Height = 25
        Caption = #25346#26029
        Enabled = False
        TabOrder = 4
        OnClick = btnIPHhandup1Click
      end
      object edtDCAddr1: TcxTextEdit
        Left = 92
        Top = 213
        TabOrder = 5
        Width = 163
      end
    end
    object cxGroupBox2: TcxGroupBox
      Left = 340
      Top = 37
      Caption = #21015#36710'2'
      TabOrder = 2
      Height = 300
      Width = 301
      object Label4: TLabel
        Left = 28
        Top = 40
        Width = 42
        Height = 14
        Caption = #36710#20307#21495
      end
      object Label5: TLabel
        Left = 28
        Top = 84
        Width = 42
        Height = 14
        Caption = #36710#31665#21495
      end
      object Label6: TLabel
        Left = 28
        Top = 128
        Width = 42
        Height = 14
        Caption = #25253#35686#22120
      end
      object Label8: TLabel
        Left = 28
        Top = 216
        Width = 42
        Height = 14
        Caption = #35843#24230#21488
      end
      object edtTrainUnitID2: TcxTextEdit
        Left = 92
        Top = 37
        TabOrder = 0
        Text = '2'
        Width = 121
      end
      object edtCarriageID2: TcxTextEdit
        Left = 92
        Top = 81
        TabOrder = 1
        Text = '4'
        Width = 121
      end
      object edtIPHDevID2: TcxTextEdit
        Left = 92
        Top = 125
        TabOrder = 2
        Text = '6'
        Width = 121
      end
      object btnIPHhandup2: TcxButton
        Left = 180
        Top = 170
        Width = 75
        Height = 25
        Caption = #25346#26029
        Enabled = False
        TabOrder = 3
        OnClick = btnIPHhandup2Click
      end
      object btnIPHAlarm2: TcxButton
        Left = 92
        Top = 170
        Width = 75
        Height = 25
        Caption = #25253#35686
        TabOrder = 4
      end
      object edtDCAddr2: TcxTextEdit
        Left = 92
        Top = 213
        TabOrder = 5
        Width = 165
      end
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    SkinName = 'Darkroom'
    Left = 728
    Top = 148
  end
  object Timer: TTimer
    OnTimer = TimerTimer
    Left = 728
    Top = 76
  end
  object TimerSendPCM: TTimer
    Enabled = False
    Interval = 100
    OnTimer = TimerSendPCMTimer
    Left = 660
    Top = 60
  end
end
