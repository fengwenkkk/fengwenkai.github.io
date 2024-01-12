inherited FormGroupCfg: TFormGroupCfg
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
    OnResize = MainPanelResize
    ExplicitWidth = 976
    ExplicitHeight = 608
    object gbCfg: TcxGroupBox
      Left = 216
      Top = 32
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
      Height = 441
      Width = 497
      object btnCancel: TcxButton
        Left = 296
        Top = 381
        Width = 75
        Height = 25
        Caption = #25918#24323
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 23
        OnClick = btnCancelClick
      end
      object btnSet: TcxButton
        Left = 188
        Top = 381
        Width = 75
        Height = 25
        Caption = #35774#32622
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 20
        OnClick = btnSetClick
      end
      object cxLabel10: TcxLabel
        Left = 68
        Top = 179
        Caption = #27491#32447#21015#36710#24191#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 189
      end
      object cxLabel5: TcxLabel
        Left = 68
        Top = 29
        Caption = #19978#34892#21015#36710#36890#25773#32452
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
        Caption = #19979#34892#21015#36710#36890#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 69
      end
      object cxLabel7: TcxLabel
        Left = 68
        Top = 89
        Caption = #27491#32447#21015#36710#36890#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 99
      end
      object cxLabel8: TcxLabel
        Left = 68
        Top = 119
        Caption = #19978#34892#21015#36710#24191#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 129
      end
      object cxLabel9: TcxLabel
        Left = 68
        Top = 149
        Caption = #19979#34892#21015#36710#24191#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 159
      end
      object edtMainLineTrainGroupID: TcxTextEdit
        Left = 188
        Top = 87
        Properties.ReadOnly = False
        TabOrder = 2
        Width = 221
      end
      object edtUpLineTrainBrdGroupID: TcxTextEdit
        Left = 188
        Top = 117
        Properties.ReadOnly = False
        TabOrder = 3
        Width = 221
      end
      object edtUpLineTrainGroupID: TcxTextEdit
        Left = 188
        Top = 27
        ParentFont = False
        Properties.ReadOnly = False
        Style.Font.Charset = ANSI_CHARSET
        Style.Font.Color = clWindowText
        Style.Font.Height = -14
        Style.Font.Name = 'Arial'
        Style.Font.Style = []
        Style.IsFontAssigned = True
        TabOrder = 0
        Width = 221
      end
      object edtDownLineTrainGroupID: TcxTextEdit
        Left = 188
        Top = 57
        Properties.ReadOnly = False
        TabOrder = 1
        Width = 221
      end
      object edtMainLineTrainBrdGroupID: TcxTextEdit
        Left = 188
        Top = 177
        Properties.ReadOnly = False
        TabOrder = 5
        Width = 221
      end
      object edtDownLineTrainBrdGroupID: TcxTextEdit
        Left = 188
        Top = 147
        Properties.ReadOnly = False
        TabOrder = 4
        Width = 221
      end
      object cxLabel25: TcxLabel
        Left = 68
        Top = 209
        Caption = #20840#37096#21015#36710#36890#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 219
      end
      object edtAllStationGroupID: TcxTextEdit
        Left = 188
        Top = 207
        Properties.ReadOnly = False
        TabOrder = 8
        Width = 221
      end
      object cxLabel1: TcxLabel
        Left = 68
        Top = 239
        Caption = #27491#32447#21496#26426#36890#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 249
      end
      object edtDriverGroupID: TcxTextEdit
        Left = 188
        Top = 237
        Properties.ReadOnly = False
        TabOrder = 10
        Width = 221
      end
      object cxLabel2: TcxLabel
        Left = 54
        Top = 269
        Caption = #36710#36742#27573#21496#26426#36890#25773#32452
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 279
      end
      object edtDepotDriverGroupID: TcxTextEdit
        Left = 188
        Top = 267
        Properties.ReadOnly = False
        TabOrder = 12
        Width = 221
      end
      object cxLabel3: TcxLabel
        Left = 40
        Top = 299
        Caption = #20020#26102#36890#35805#32452#36215#22987#21495#30721
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 309
      end
      object edtStartTempDynamicGroupID: TcxTextEdit
        Left = 188
        Top = 297
        Properties.ReadOnly = False
        TabOrder = 14
        Width = 221
      end
      object cxLabel4: TcxLabel
        Left = 40
        Top = 329
        Caption = #20020#26102#36890#35805#32452#32467#26463#21495#30721
        Properties.Alignment.Horz = taRightJustify
        Properties.Alignment.Vert = taVCenter
        Transparent = True
        AnchorX = 170
        AnchorY = 339
      end
      object edtEndTempDynamicGroupID: TcxTextEdit
        Left = 188
        Top = 327
        Properties.ReadOnly = False
        TabOrder = 16
        Width = 221
      end
    end
  end
end
