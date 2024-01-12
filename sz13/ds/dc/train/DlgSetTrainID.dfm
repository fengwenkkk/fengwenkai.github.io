object DialogSetTrainID: TDialogSetTrainID
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #35774#32622#21015#36710#20301#32622
  ClientHeight = 366
  ClientWidth = 480
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 18
  object edtDestID: TcxTextEdit
    Left = 144
    Top = 218
    TabOrder = 7
    Width = 173
  end
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 306
    Width = 480
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 15
    DesignSize = (
      480
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 480
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 278
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #30830#35748
      Default = True
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 378
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
  object cxLabel6: TcxLabel
    Left = 78
    Top = 100
    Caption = #36710#27425#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object edtTrainID: TcxTextEdit
    Left = 144
    Top = 99
    Properties.MaxLength = 31
    Properties.ReadOnly = False
    TabOrder = 2
    Width = 173
  end
  object cxLabel1: TcxLabel
    Left = 63
    Top = 140
    Caption = #36816#34892#26041#21521
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object cbDestID: TcxComboBox
    Left = 144
    Top = 218
    Properties.Sorted = True
    TabOrder = 6
    Width = 173
  end
  object cxLabel2: TcxLabel
    Left = 63
    Top = 180
    Caption = #24403#21069#20301#32622
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object rbUp: TcxRadioButton
    Left = 144
    Top = 141
    Width = 73
    Height = 21
    Caption = #19978#34892
    Checked = True
    TabOrder = 3
    TabStop = True
    LookAndFeel.SkinName = 'Office2010Blue'
    Transparent = True
  end
  object rbDown: TcxRadioButton
    Left = 244
    Top = 141
    Width = 73
    Height = 21
    Caption = #19979#34892
    TabOrder = 4
    LookAndFeel.SkinName = 'Office2010Blue'
    Transparent = True
  end
  object cxLabel3: TcxLabel
    Left = 78
    Top = 220
    Caption = #30446#30340#22320
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object cbStationID: TcxComboBox
    Left = 144
    Top = 178
    TabOrder = 5
    Width = 173
  end
  object cxLabel4: TcxLabel
    Left = 63
    Top = 60
    Caption = #24402#23646#21306#22495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object cbDepotID: TcxComboBox
    Left = 144
    Top = 58
    Properties.OnChange = cbTrainPosPropertiesChange
    TabOrder = 1
    Width = 173
  end
  object cxLabel5: TcxLabel
    Left = 63
    Top = 260
    Caption = #21496#26426#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object edtDriverID: TcxTextEdit
    Left = 144
    Top = 258
    TabOrder = 8
    Width = 173
  end
  object cxLabel7: TcxLabel
    Left = 78
    Top = 20
    Caption = #36710#20307#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object edtTrainUnitID: TcxTextEdit
    Left = 144
    Top = 18
    Properties.ReadOnly = True
    TabOrder = 0
    Width = 173
  end
end
