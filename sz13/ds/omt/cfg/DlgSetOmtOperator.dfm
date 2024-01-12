object DialogSetOmtOperator: TDialogSetOmtOperator
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  ClientHeight = 228
  ClientWidth = 465
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
    465
    228)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 166
    Width = 946
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
    ExplicitWidth = 1000
  end
  object btnOK: TcxButton
    Left = 155
    Top = 188
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    TabOrder = 3
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 286
    Top = 188
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    TabOrder = 4
    OnClick = btnCancelClick
  end
  object lbUserName: TcxLabel
    Left = 90
    Top = 32
    Caption = #29992#25143#22995#21517
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 154
  end
  object lbPwd: TcxLabel
    Left = 90
    Top = 82
    Caption = #29992#25143#23494#30721
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 154
  end
  object edtUserName: TcxTextEdit
    Left = 160
    Top = 31
    Properties.MaxLength = 31
    TabOrder = 0
    Text = 'cxTextEdit1'
    Width = 201
  end
  object cxLabel2: TcxLabel
    Left = 90
    Top = 132
    Caption = #29992#25143#31867#22411
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 154
  end
  object edtPwd: TcxTextEdit
    Left = 160
    Top = 81
    Properties.MaxLength = 31
    TabOrder = 1
    Text = 'edtPwd'
    Width = 201
  end
  object cbOperatorType: TcxComboBox
    Left = 160
    Top = 131
    TabOrder = 2
    Width = 201
  end
end
