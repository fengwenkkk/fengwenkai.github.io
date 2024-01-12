object DialogSetStation: TDialogSetStation
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 335
  ClientWidth = 467
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
    467
    335)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 279
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object btnOK: TcxButton
    Left = 120
    Top = 297
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 6
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 240
    Top = 297
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 10
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 50
    Top = 25
    Caption = #36710#31449#32534#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 50
    Top = 65
    Caption = #36710#31449#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtID: TcxTextEdit
    Left = 120
    Top = 23
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 0
    Width = 181
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 105
    Caption = #36710#31449#31867#22411
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtName: TcxTextEdit
    Left = 120
    Top = 63
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 181
  end
  object cbType: TcxComboBox
    Left = 120
    Top = 103
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 2
    Width = 181
  end
  object cxLabel2: TcxLabel
    Left = 35
    Top = 145
    Caption = #26159#21542#38598#20013#31449
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cbInterLockStation: TcxCheckBox
    Left = 120
    Top = 143
    Caption = #26159
    Properties.OnChange = cbInterLockStationPropertiesChange
    TabOrder = 3
  end
  object cxLabel3: TcxLabel
    Left = 65
    Top = 185
    Caption = #38598#20013#31449
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cbInterLockStationID: TcxComboBox
    Left = 120
    Top = 183
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 4
    Width = 181
  end
  object cxLabel4: TcxLabel
    Left = 35
    Top = 225
    Caption = #36710#31449#36890#35805#32452
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtStationGroupID: TcxTextEdit
    Left = 120
    Top = 223
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 5
    Width = 181
  end
end
