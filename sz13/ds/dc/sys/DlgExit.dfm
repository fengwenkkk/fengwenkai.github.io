object DialogExit: TDialogExit
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  Caption = #27880#38144#36864#20986
  ClientHeight = 181
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 18
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 121
    Width = 360
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 1
    DesignSize = (
      360
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 360
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 157
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #30830#35748
      Default = True
      LookAndFeel.Kind = lfUltraFlat
      LookAndFeel.NativeStyle = False
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 257
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Caption = #25918#24323
      LookAndFeel.Kind = lfUltraFlat
      LookAndFeel.NativeStyle = False
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
  object cxLabel29: TcxLabel
    Left = 70
    Top = 21
    Caption = #35831#36873#25321#65306
    Transparent = True
  end
  object cxImage1: TcxImage
    Left = 20
    Top = 60
    AutoSize = True
    Picture.Data = {
      0B546478504E47496D61676589504E470D0A1A0A0000000D4948445200000020
      000000200806000000737A7AF4000000017352474200AECE1CE9000000046741
      4D410000B18F0BFC6105000000097048597300000EC300000EC301C76FA86400
      00022E494441545847D557314EC340100CBF4054F4343C813AD8404949C92F8C
      4D8B92968E27D020CEA048BC208A7801FC803265D8596FACCB796DEFD902C448
      23C27976F77C7BBB779EFC1B9C65AF0767C5F3659ABB595A948B342F3FD3C2AD
      99FC1B636E060DB462361E14E0881CDF2445F94E413616B2966C602B6E0660B3
      D94B72774D0E976180082EE103BEC4AB0DD3EC699F0CEF148783085FF029EEBB
      0121E5F45E73348AE4B37F12D5B277BE39E5F72329DC9CF27B757AEB4E401ABB
      C0189E69365BC277673A902FCDB0266D2C91B622C9CB07D556881822DD05EFF6
      9E0D87B715792768A28F9ABD906228D551958D6A50D39FC079E60EF13FFECA50
      8D69F672ACD9D70C57128DC352E70888DC87B9C61E105735FCE721116BA75971
      8753844DBA953A9E976FE2AA86AAF3889822E5E5477B55851662E3892B466F0A
      408A29729EEDA2213092CAEF2BDC0796FD445C881C067C9868A24E2238DE56DC
      304C6F0F524C31C10AE05453441D6C0B8E714DDFA45B8BD9B009843D01D5A1E9
      DAE94F2032052843316554E5A96B5BB99B82C84D18941D6DBAAECED7467F13C6
      96A15B892963D004FC32B437228FB40A5B869DD1C29D46646DC54D5267C424D4
      67ED6CB462C0D83C986109C6951F513BD6E96D7A8FE32D29D85CCC6A604CD32A
      D48F63A0F742221C3381D60B09C3702503B1DC435200DFBD37E4B84B69C42634
      5D4A05105A56C24AF83207AF51A5E36F3E4C7C707550D9C4F409D672598FF934
      0B80C6C11DB36ADBBFF771FAB3984CBE01649D8ADE2A939E4E0000000049454E
      44AE426082}
    TabOrder = 2
    Transparent = True
    Visible = False
  end
  object rbExit: TcxRadioButton
    Left = 157
    Top = 55
    Width = 113
    Height = 21
    Caption = #36864#20986
    TabOrder = 4
  end
  object rbLogout: TcxRadioButton
    Left = 157
    Top = 25
    Width = 113
    Height = 21
    Caption = #27880#38144
    Checked = True
    TabOrder = 3
    TabStop = True
  end
  object rbLock: TcxRadioButton
    Left = 157
    Top = 85
    Width = 113
    Height = 21
    Caption = #38145#23631
    TabOrder = 5
  end
end
