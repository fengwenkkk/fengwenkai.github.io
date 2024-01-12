object DialogSetDCUser: TDialogSetDCUser
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  ClientHeight = 510
  ClientWidth = 614
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
    614
    510)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 454
    Width = 1000
    Height = 3
    Anchors = [akLeft, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
  end
  object clbStationList: TcxCheckListBox
    Left = 120
    Top = 133
    Width = 433
    Height = 303
    Anchors = [akLeft, akTop, akRight, akBottom]
    Items = <>
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 3
  end
  object btnOK: TcxButton
    Left = 241
    Top = 472
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #30830#35748
    Default = True
    TabOrder = 4
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 361
    Top = 472
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #21462#28040
    TabOrder = 8
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 35
    Top = 25
    Caption = #35843#24230#21488#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 35
    Top = 64
    Caption = #35843#24230#21488#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserID: TcxTextEdit
    Left = 120
    Top = 23
    TabOrder = 0
    Text = 'edtUserID'
    Width = 217
  end
  object LabelStationList: TcxLabel
    Left = 50
    Top = 105
    Caption = #31649#29702#21306#22495
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserName: TcxTextEdit
    Left = 120
    Top = 63
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 217
  end
  object cbAllStation: TcxCheckBox
    Left = 120
    Top = 105
    Caption = #20840#37096#27491#32447#21306#38388
    Properties.OnChange = cbAllStationPropertiesChange
    TabOrder = 2
  end
end
