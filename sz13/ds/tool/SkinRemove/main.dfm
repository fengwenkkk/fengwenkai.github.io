object FormMain: TFormMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Skin Remove'
  ClientHeight = 310
  ClientWidth = 447
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Arial'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  DesignSize = (
    447
    310)
  PixelsPerInch = 96
  TextHeight = 17
  object mmFile: TMemo
    Left = 346
    Top = 112
    Width = 33
    Height = 25
    TabOrder = 5
    Visible = False
    WordWrap = False
  end
  object btnStart: TcxButton
    Left = 346
    Top = 159
    Width = 75
    Height = 25
    Caption = #24320#22987
    TabOrder = 1
    OnClick = btnStartClick
  end
  object cxLabel1: TcxLabel
    Left = 8
    Top = 10
    Caption = #24037#31243#25991#20214
    Transparent = True
  end
  object pbMain: TcxProgressBar
    Left = 8
    Top = 279
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 2
    Width = 431
  end
  object cxLabel2: TcxLabel
    Left = 8
    Top = 40
    Caption = #20445#30041#30382#32932
    Transparent = True
  end
  object mmExpSkin: TcxMemo
    Left = 82
    Top = 40
    Lines.Strings = (
      'Blue'
      'Blueprint'
      'DarkSide'
      'DevExpressDarkStyle'
      'GlassOceans'
      'McSkin'
      'MoneyTwins'
      'Office2010Blue'
      'Office2007Pink'
      'Office2010Silver'
      'Office2013DarkGray'
      'Sharp'
      'VisualStudio2013Dark'
      'VisualStudio2013Light')
    Properties.ScrollBars = ssVertical
    TabOrder = 0
    Height = 225
    Width = 239
  end
  object edtProject: TcxButtonEdit
    Left = 82
    Top = 9
    Properties.Buttons = <
      item
        Default = True
        Kind = bkEllipsis
      end>
    Properties.OnChange = edtProjectPropertiesChange
    TabOrder = 6
    OnClick = edtProjectClick
    Width = 339
  end
  object btnExit: TcxButton
    Left = 346
    Top = 207
    Width = 75
    Height = 25
    Caption = #32467#26463
    TabOrder = 7
    OnClick = btnExitClick
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Lilian'
    Left = 368
    Top = 56
  end
  object OpenDialog: TOpenDialog
    DefaultExt = '*.cbproj'
    Filter = '*.cbproj|*.cbproj'
    Options = [ofEnableSizing]
    OptionsEx = [ofExNoPlacesBar]
    Title = #36873#25321#24037#31243#25991#20214
    Left = 392
    Top = 112
  end
  object cxImageList1: TcxImageList
    SourceDPI = 96
    FormatVersion = 1
    DesignInfo = 6815760
  end
end
