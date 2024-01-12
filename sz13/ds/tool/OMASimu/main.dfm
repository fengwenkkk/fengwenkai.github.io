inherited FormMain: TFormMain
  Caption = 'OMA Simu'
  ClientHeight = 470
  ClientWidth = 1035
  Position = poDesktopCenter
  OnClose = FormClose
  ExplicitWidth = 1051
  ExplicitHeight = 509
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1035
    Height = 470
    ExplicitWidth = 1035
    ExplicitHeight = 470
    object Label1: TLabel
      Left = 665
      Top = 22
      Width = 65
      Height = 18
      Caption = #32593#20803#25968
    end
    object btnReply: TcxButton
      Left = 920
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #36864#20986
      TabOrder = 0
      OnClick = btnReplyClick
    end
    object tl: TcxTreeList
      Left = 13
      Top = 64
      Width = 1003
      Height = 386
      Anchors = [akLeft, akTop, akRight, akBottom]
      Bands = <
        item
        end>
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Caramel'
      Navigator.Buttons.CustomButtons = <>
      TabOrder = 1
      object tlColumn3: TcxTreeListColumn
        Caption.Text = #24207#21495
        DataBinding.ValueType = 'Integer'
        Width = 100
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object cxTreeList1Column1: TcxTreeListColumn
        Caption.Text = 'MAC'
        DataBinding.ValueType = 'String'
        Width = 192
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object cxTreeList1Column2: TcxTreeListColumn
        Caption.Text = #22312#32447
        DataBinding.ValueType = 'String'
        Width = 126
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
    object rbNoAlarm: TcxRadioButton
      Left = 25
      Top = 21
      Width = 65
      Height = 18
      Caption = #26080#21578#35686
      Checked = True
      TabOrder = 2
      TabStop = True
      OnClick = rbAlarmClick
    end
    object rbAlarmRaise: TcxRadioButton
      Left = 110
      Top = 22
      Width = 80
      Height = 18
      Caption = #20135#29983#21578#35686
      TabOrder = 3
      OnClick = rbAlarmClick
    end
    object rbAlarmClear: TcxRadioButton
      Left = 275
      Top = 21
      Width = 80
      Height = 18
      Caption = #28040#38500#21578#35686
      TabOrder = 4
      OnClick = rbAlarmClick
    end
    object edtNeNum: TcxTextEdit
      Left = 740
      Top = 20
      TabOrder = 5
      Text = '1'
      Width = 65
    end
    object btnStart: TcxButton
      Left = 825
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #24320#22987
      TabOrder = 6
      OnClick = btnStartClick
    end
    object rbNoSend: TcxRadioButton
      Left = 440
      Top = 22
      Width = 80
      Height = 18
      Caption = #20572#27490#21457#21253
      TabOrder = 7
    end
    object cxTextEdit1: TcxTextEdit
      Left = 200
      Top = 20
      Enabled = False
      TabOrder = 8
      Width = 65
    end
    object cxTextEdit2: TcxTextEdit
      Left = 365
      Top = 20
      Enabled = False
      TabOrder = 9
      Width = 65
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    SkinName = 'Lilian'
    Left = 652
    Top = 100
  end
  object TimerCheck: TTimer
    OnTimer = TimerCheckTimer
    Left = 172
    Top = 240
  end
  object cxPropertiesStore1: TcxPropertiesStore
    Components = <>
    StorageName = 'cxPropertiesStore1'
    Left = 448
    Top = 240
  end
  object dlgSaveSave: TSaveDialog
    Left = 672
    Top = 296
  end
  object TimerFlag: TTimer
    Interval = 60000
    OnTimer = TimerFlagTimer
    Left = 904
    Top = 304
  end
end
