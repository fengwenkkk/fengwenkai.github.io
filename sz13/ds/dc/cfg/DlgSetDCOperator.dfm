object DialogSetDCOperator: TDialogSetDCOperator
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 423
  ClientWidth = 518
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
    518
    423)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 361
    Width = 999
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
    ExplicitWidth = 1000
  end
  object btnOK: TcxButton
    Left = 195
    Top = 383
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    TabOrder = 4
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 315
    Top = 383
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    TabOrder = 8
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 35
    Top = 25
    Caption = #35843#24230#21592#36134#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 35
    Top = 65
    Caption = #35843#24230#21592#22995#21517
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserID: TcxTextEdit
    Left = 120
    Top = 23
    Properties.MaxLength = 31
    Properties.OnChange = edtUserIDPropertiesChange
    TabOrder = 0
    Text = 'edtUserID'
    Width = 273
  end
  object lbPwd: TcxLabel
    Left = 35
    Top = 105
    Caption = #35843#24230#21592#23494#30721
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserName: TcxTextEdit
    Left = 120
    Top = 63
    Properties.MaxLength = 31
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 273
  end
  object clbDCType: TcxCheckListBox
    Left = 120
    Top = 145
    Width = 273
    Height = 197
    Items = <>
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 3
  end
  object cxLabel2: TcxLabel
    Left = 50
    Top = 145
    Caption = #31649#29702#33539#22260
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtPwd: TcxTextEdit
    Left = 120
    Top = 103
    Properties.MaxLength = 31
    TabOrder = 2
    Text = 'edtPwd'
    Width = 273
  end
end
