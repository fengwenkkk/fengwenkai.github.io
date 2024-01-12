object DialogSetDCUser: TDialogSetDCUser
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = #26631#27880#32676#32452
  ClientHeight = 368
  ClientWidth = 516
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
    516
    368)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 312
    Width = 1000
    Height = 3
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
  end
  object btnOK: TcxButton
    Left = 193
    Top = 328
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    TabOrder = 3
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 313
    Top = 328
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    TabOrder = 7
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 50
    Top = 25
    Caption = #29992#25143#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 50
    Top = 64
    Caption = #29992#25143#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserID: TcxTextEdit
    Left = 120
    Top = 23
    TabOrder = 0
    Text = 'edtUserID'
    Width = 137
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 105
    Caption = #31649#29702#33539#22260
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserName: TcxTextEdit
    Left = 120
    Top = 63
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 137
  end
  object clbDCType: TcxCheckListBox
    Left = 120
    Top = 105
    Width = 273
    Height = 192
    Items = <>
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 2
    OnClickCheck = clbDCTypeClickCheck
  end
end
