object DialogSetStation: TDialogSetStation
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  ClientHeight = 236
  ClientWidth = 445
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
    445
    236)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 180
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object btnOK: TcxButton
    Left = 120
    Top = 203
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 3
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 245
    Top = 203
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 7
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 50
    Top = 49
    Caption = #36710#31449#32534#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 50
    Top = 88
    Caption = #36710#31449#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtID: TcxTextEdit
    Left = 120
    Top = 47
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 0
    Width = 200
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 129
    Caption = #36710#31449#31867#22411
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtName: TcxTextEdit
    Left = 120
    Top = 87
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 200
  end
  object cbType: TcxComboBox
    Left = 120
    Top = 127
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 2
    Width = 200
  end
end
