object DialogSetGroupInfo: TDialogSetGroupInfo
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #26631#27880#32676#32452
  ClientHeight = 259
  ClientWidth = 422
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
    422
    259)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 194
    Width = 904
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
    ExplicitWidth = 1000
  end
  object btnOK: TcxButton
    Left = 120
    Top = 219
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    TabOrder = 4
    OnClick = btnOKClick
  end
  object btnCancel: TcxButton
    Left = 240
    Top = 219
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    TabOrder = 9
    OnClick = btnCancelClick
  end
  object cxLabel6: TcxLabel
    Left = 50
    Top = 25
    Caption = #32676#32452#21495#30721
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 50
    Top = 65
    Caption = #32676#32452#21517#31216
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtGroupID: TcxTextEdit
    Left = 120
    Top = 23
    Properties.ReadOnly = True
    TabOrder = 0
    Text = 'edtGroupID'
    Width = 193
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 105
    Caption = #32676#32452#21035#21517
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtOriGroupName: TcxTextEdit
    Left = 120
    Top = 63
    Properties.ReadOnly = True
    TabOrder = 1
    Text = 'cxTextEdit1'
    Width = 193
  end
  object edtGroupName: TcxTextEdit
    Left = 120
    Top = 103
    TabOrder = 2
    Text = 'edtGroupName'
    Width = 193
  end
  object cxLabel2: TcxLabel
    Left = 50
    Top = 145
    Caption = #32676#32452#20998#31867
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cbFuncType: TcxComboBox
    Left = 120
    Top = 143
    TabOrder = 3
    Text = 'cbFuncType'
    Width = 193
  end
end
