object FormMain: TFormMain
  Left = 0
  Top = 0
  Caption = 'Media Player - 2022.05.20'
  ClientHeight = 572
  ClientWidth = 974
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
    974
    572)
  PixelsPerInch = 96
  TextHeight = 17
  object btnStart: TcxButton
    Left = 746
    Top = 19
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = #24320#22987
    TabOrder = 0
    OnClick = btnStartClick
  end
  object cxLabel1: TcxLabel
    Left = 43
    Top = 21
    Caption = 'URL'
    Transparent = True
  end
  object edtURL: TcxButtonEdit
    Left = 82
    Top = 19
    Anchors = [akLeft, akTop, akRight]
    Properties.Buttons = <
      item
        Default = True
        Kind = bkEllipsis
      end>
    Properties.OnButtonClick = edtURLPropertiesButtonClick
    TabOrder = 1
    Text = 'rtsp://192.168.90.223:55554/h264/ch1/sub/av_stream'
    Width = 631
  end
  object btnStop: TcxButton
    Left = 850
    Top = 19
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = #32467#26463
    TabOrder = 2
    OnClick = btnStopClick
  end
  object cbRec: TcxCheckBox
    Left = 112
    Top = 58
    Caption = #24405#20687
    TabOrder = 3
  end
  object cbTcpMode: TcxCheckBox
    Left = 256
    Top = 58
    Caption = 'TCP'#20256#36755
    TabOrder = 4
  end
  object cbKeepPlay: TcxCheckBox
    Left = 412
    Top = 58
    Caption = #20445#25345#25773#25918
    TabOrder = 5
  end
  object cxGroupBox1: TcxGroupBox
    Left = 8
    Top = 91
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 6
    Height = 473
    Width = 958
    object gbVideo: TcxGroupBox
      Left = 3
      Top = 23
      Align = alClient
      PanelStyle.Active = True
      TabOrder = 0
      Visible = False
      Height = 447
      Width = 952
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Office2010Blue'
    Left = 808
    Top = 272
  end
  object OpenDialog: TOpenDialog
    DefaultExt = '*.mp4'
    Filter = '*.mp4|*.mp4|*.avi|*.avi|*.mp3|*.mp3|*.*|*.*'
    Options = [ofEnableSizing]
    OptionsEx = [ofExNoPlacesBar]
    Title = #36873#25321#23186#20307#25991#20214
    Left = 696
    Top = 272
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 148
    Top = 300
  end
end
