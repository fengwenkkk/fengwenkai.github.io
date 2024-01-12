object DialogSetSw: TDialogSetSw
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  ClientHeight = 209
  ClientWidth = 496
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
    496
    209)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -1
    Top = 153
    Width = 1029
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 312
    ExplicitWidth = 1000
  end
  object btnOK: TcxButton
    Left = 248
    Top = 171
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #30830#35748
    Default = True
    LookAndFeel.NativeStyle = False
    TabOrder = 4
    OnClick = btnOKClick
    ExplicitTop = 169
  end
  object btnCancel: TcxButton
    Left = 360
    Top = 171
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #21462#28040
    LookAndFeel.NativeStyle = False
    TabOrder = 8
    OnClick = btnCancelClick
    ExplicitTop = 169
  end
  object cxLabel6: TcxLabel
    Left = 50
    Top = 25
    Caption = #29256#26412#21517#31216
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cxLabel5: TcxLabel
    Left = 50
    Top = 65
    Caption = #29256#26412#25991#20214
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object edtVer: TcxTextEdit
    Left = 120
    Top = 23
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 0
    Width = 337
  end
  object cxLabel1: TcxLabel
    Left = 50
    Top = 105
    Caption = #29256#26412#31867#22411
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 114
  end
  object cbType: TcxComboBox
    Left = 120
    Top = 103
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleFocused.LookAndFeel.NativeStyle = False
    StyleHot.LookAndFeel.NativeStyle = False
    TabOrder = 2
    Width = 337
  end
  object edtFile: TcxButtonEdit
    Left = 120
    Top = 63
    Properties.Buttons = <
      item
        Default = True
        Kind = bkEllipsis
      end>
    Properties.MaxLength = 250
    Properties.OnButtonClick = edtFilePropertiesButtonClick
    Properties.OnEditValueChanged = edtFilePropertiesEditValueChanged
    TabOrder = 1
    Text = 'edtFile'
    Width = 337
  end
  object cxLabel2: TcxLabel
    Left = 50
    Top = 145
    Caption = #19978#20256#36827#24230
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    Visible = False
    AnchorX = 114
  end
  object cbUpload: TcxProgressBar
    Left = 120
    Top = 143
    TabOrder = 10
    Visible = False
    Width = 337
  end
  object btnUpload: TcxButton
    Left = 123
    Top = 171
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #19978#20256
    LookAndFeel.NativeStyle = False
    OptionsImage.Glyph.SourceDPI = 96
    OptionsImage.Glyph.Data = {
      89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
      61000000097048597300000EC400000EC401952B0E1B000001DB49444154388D
      A5923B6814511486FF73CE9D97937DCC8CB3EEC46C7683C6C655518C82928D6E
      D00521A0481250C4229D20829560AD859D20AC8258D828561629240A9A8045B0
      48E71A0D6CAA10DDC2477CE0223B2992C02C4E34901F6E71CF3DFF77CEBDF700
      9B1445378317BA2795A94ACC000B4398C004B01044082C044238F9F8D6ECB135
      8F8A02529E59AA0CBB1056106108AF2CC50C250237D98989E7B581A8A70DD055
      E8989A7FDB2AB1FC8152B4D2C56A7511C6BCAAC3F38CA90DDF6F772973A5EF54
      70F95F39B2DE41EFA1F4BEA067CB23376B0CDA096D7CB1FEE3635C9E8A0B7A1E
      12E9ACF9A0FF74CAD23541F337EEFBFEA772A381EF1BEAA0580E6EF4959DB3BD
      7B6CD84981A91BDBC536656EE6CB8BFF020E54824A6157C79D81219F88420833
      B29D092C7DA3235B73D6EBB999CFF57501C5C3996D7EC11A1F3A974BE92656BF
      51818590CF67A8B1F8F3B8E6C8C385774BBF62DFC0DF61564746FBBB7676A710
      A209534B002058E28258039FC8E4C0D3D5374F174662017B0F06676AB31F507B
      0F5CBA388666EB2B0840DA2CE2DE939B0011F23DCE70D4D306B87D759A00E0DA
      DDA3A1ADBBD05A0222C0520E745D70FDFCCBB6D1FF0BB0265D57708CFDED8946
      FCC8C402921E26AACFC64E4663A285AF62099BD532AE9C5E23F8809D8F000000
      0049454E44AE426082}
    TabOrder = 3
    OnClick = btnUploadClick
    ExplicitTop = 169
  end
  object OpenDialog: TOpenDialog
    FileName = '*.bin'
    Filter = '*.bin'
    Left = 8
    Top = 8
  end
end
