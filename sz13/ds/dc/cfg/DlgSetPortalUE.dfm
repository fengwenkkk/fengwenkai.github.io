object DialogSetPortalUE: TDialogSetPortalUE
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 290
  ClientWidth = 464
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
    464
    290)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 229
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object btnOK: TcxButton
    Left = 141
    Top = 250
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 5
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 261
    Top = 250
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
    Top = 30
    Caption = #32456#31471#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 50
    Top = 70
    Caption = #32456#31471#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtID: TcxTextEdit
    Left = 120
    Top = 28
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 0
    Text = 'edtID'
    Width = 193
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 110
    Caption = #32456#31471#31867#22411
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtName: TcxTextEdit
    Left = 120
    Top = 68
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 193
  end
  object cbStation: TcxComboBox
    Left = 120
    Top = 188
    Properties.DropDownListStyle = lsFixedList
    Properties.DropDownRows = 16
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 4
    Width = 193
  end
  object label4: TcxLabel
    Left = 50
    Top = 150
    Caption = #21151#33021#20998#31867
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cbType: TcxComboBox
    Left = 120
    Top = 108
    Properties.DropDownListStyle = lsFixedList
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 2
    Width = 193
  end
  object cxLabel2: TcxLabel
    Left = 50
    Top = 190
    Caption = #25152#23646#36710#31449
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cbFuncType: TcxComboBox
    Left = 120
    Top = 148
    Properties.DropDownListStyle = lsFixedList
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 3
    Width = 193
  end
end
