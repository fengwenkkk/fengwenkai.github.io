object FormSystem: TFormSystem
  Left = 0
  Top = 0
  ClientHeight = 631
  ClientWidth = 1086
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object PaneSystem: TPanel
    Left = 0
    Top = 0
    Width = 1086
    Height = 631
    Align = alClient
    TabOrder = 0
    object pcMain: TcxPageControl
      Left = 1
      Top = 1
      Width = 1084
      Height = 629
      Align = alClient
      ParentShowHint = False
      ShowHint = False
      TabOrder = 0
      Properties.ActivePage = tsAlarmCfg
      Properties.CustomButtons.Buttons = <>
      Properties.TabWidth = 120
      ClientRectBottom = 623
      ClientRectLeft = 2
      ClientRectRight = 1078
      ClientRectTop = 27
      object tsUserCfg: TcxTabSheet
        Caption = #29992#25143#31649#29702
        ImageIndex = 0
        ExplicitLeft = 4
        ExplicitTop = 24
        ExplicitHeight = 601
      end
      object tsSoftwareCfg: TcxTabSheet
        Caption = #29256#26412#31649#29702
        ImageIndex = 2
        ExplicitLeft = 4
        ExplicitTop = 24
        ExplicitHeight = 601
      end
      object tsUpdateCfg: TcxTabSheet
        Caption = #21319#32423#31649#29702
        ImageIndex = 3
        ExplicitLeft = 4
        ExplicitTop = 24
        ExplicitHeight = 601
      end
      object tsAlarmCfg: TcxTabSheet
        Caption = #21578#35686#37197#32622
        ImageIndex = 4
        ExplicitLeft = 3
        ExplicitTop = 25
        ExplicitHeight = 601
      end
      object tsLog: TcxTabSheet
        Caption = #25805#20316#26085#24535
        ImageIndex = 4
        ExplicitLeft = 4
        ExplicitTop = 24
        ExplicitHeight = 601
      end
    end
  end
end
