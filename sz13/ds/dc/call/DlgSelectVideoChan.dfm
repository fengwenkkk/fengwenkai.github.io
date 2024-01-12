object DialogSelectVideoChan: TDialogSelectVideoChan
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #35270#39057#19978#22681#35774#32622
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
    TabOrder = 1
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
    Left = 35
    Top = 24
    Caption = #35270#39057#28304#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 20
    Top = 64
    Caption = #35270#39057#31383#21475#32534#21495
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtUserID: TcxTextEdit
    Left = 120
    Top = 22
    Enabled = False
    Properties.ReadOnly = False
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 0
    Text = 'edtUserID'
    Width = 137
  end
  object cbVideoChanID: TcxComboBox
    Left = 120
    Top = 62
    TabOrder = 3
    Text = 'cbVideoChanID'
    Width = 137
  end
end
