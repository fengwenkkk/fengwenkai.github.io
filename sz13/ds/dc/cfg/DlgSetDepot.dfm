object DialogSetDepot: TDialogSetDepot
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 181
  ClientWidth = 442
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
    442
    181)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 120
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object btnOK: TcxButton
    Left = 119
    Top = 141
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
    Left = 239
    Top = 141
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 5
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 80
    Top = 24
    Caption = #32534#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 80
    Top = 64
    Caption = #21517#31216
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
end
