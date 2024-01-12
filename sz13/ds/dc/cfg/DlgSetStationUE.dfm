object DialogSetStationUE: TDialogSetStationUE
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 293
  ClientWidth = 444
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnShow = FormShow
  DesignSize = (
    444
    293)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 232
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object btnOK: TcxButton
    Left = 121
    Top = 253
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 2
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 241
    Top = 253
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 6
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 35
    Top = 24
    Caption = #22266#23450#21488#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 35
    Top = 64
    Caption = #22266#23450#21488#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtID: TcxTextEdit
    Left = 120
    Top = 22
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 0
    Text = 'edtID'
    Width = 137
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 105
    Caption = #20027#26426#22320#22336
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtName: TcxTextEdit
    Left = 120
    Top = 62
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 137
  end
  object edtIPAddr: TcxTextEdit
    Left = 120
    Top = 105
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 7
    Text = 'edtIPAddr'
    Width = 137
  end
  object edtDefaultGroup: TcxTextEdit
    Left = 120
    Top = 183
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 8
    Text = 'cxTextEdit1'
    Width = 137
  end
  object cbStation: TcxComboBox
    Left = 120
    Top = 143
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 9
    Text = 'cbStation'
    Width = 137
  end
  object label4: TcxLabel
    Left = 50
    Top = 144
    Caption = #25152#23646#36710#31449
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel3: TcxLabel
    Left = 65
    Top = 185
    Caption = #36890#35805#32452
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
end
