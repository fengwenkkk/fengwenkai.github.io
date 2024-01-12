object FormMain: TFormMain
  Left = 0
  Top = 0
  ClientHeight = 673
  ClientWidth = 952
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsMDIForm
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object tlSourceFile: TcxTreeList
    AlignWithMargins = True
    Left = 1
    Top = 1
    Width = 144
    Height = 671
    Margins.Left = 1
    Margins.Top = 1
    Margins.Right = 1
    Margins.Bottom = 1
    BorderStyle = cxcbsNone
    Align = alClient
    Bands = <
      item
      end>
    Navigator.Buttons.CustomButtons = <>
    OptionsData.Editing = False
    OptionsData.Deleting = False
    TabOrder = 0
    OnCustomDrawDataCell = tlSourceFileCustomDrawDataCell
    OnFocusedColumnChanged = tlSourceFileFocusedColumnChanged
    OnFocusedNodeChanged = tlSourceFileFocusedNodeChanged
    object ColumnFile: TcxTreeListColumn
      Caption.Text = #25991#20214
      DataBinding.ValueType = 'String'
      Width = 180
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnState: TcxTreeListColumn
      PropertiesClassName = 'TcxLabelProperties'
      Properties.Alignment.Horz = taCenter
      Properties.Alignment.Vert = taVCenter
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'Integer'
      Width = 70
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnStateGetDisplayText
    end
    object ColumnFile2: TcxTreeListColumn
      Caption.Text = #25991#20214
      DataBinding.ValueType = 'String'
      Width = 180
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnState2: TcxTreeListColumn
      PropertiesClassName = 'TcxLabelProperties'
      Properties.Alignment.Horz = taCenter
      Properties.Alignment.Vert = taVCenter
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'Integer'
      Width = 70
      Position.ColIndex = 3
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnStateGetDisplayText
    end
    object tlSourceFileColumn5: TcxTreeListColumn
      Caption.Text = #25991#20214
      DataBinding.ValueType = 'String'
      Width = 180
      Position.ColIndex = 4
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object tlSourceFileColumn6: TcxTreeListColumn
      PropertiesClassName = 'TcxLabelProperties'
      Properties.Alignment.Horz = taCenter
      Properties.Alignment.Vert = taVCenter
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'Integer'
      Width = 70
      Position.ColIndex = 5
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnStateGetDisplayText
    end
  end
  object Panel1: TPanel
    Left = 152
    Top = 0
    Width = 800
    Height = 673
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object PanelTop: TPanel
      AlignWithMargins = True
      Left = 1
      Top = 1
      Width = 798
      Height = 152
      Margins.Left = 1
      Margins.Top = 1
      Margins.Right = 1
      Margins.Bottom = 1
      Align = alTop
      BevelInner = bvRaised
      BevelOuter = bvNone
      BiDiMode = bdLeftToRight
      Ctl3D = False
      ParentBiDiMode = False
      ParentCtl3D = False
      TabOrder = 0
      DesignSize = (
        798
        152)
      object Label1: TcxLabel
        Left = 28
        Top = 35
        Caption = #24037#31243
      end
      object btnStop: TcxButton
        Left = 404
        Top = 110
        Width = 75
        Height = 25
        Caption = #20013#27490
        Enabled = False
        TabOrder = 6
        OnClick = btnStopClick
      end
      object btnOpen: TcxButton
        Left = 70
        Top = 110
        Width = 75
        Height = 25
        Caption = #25171#24320
        TabOrder = 2
        OnClick = btnOpenClick
      end
      object btnClear: TcxButton
        Left = 518
        Top = 110
        Width = 75
        Height = 25
        Caption = #28165#38500
        TabOrder = 3
        OnClick = btnClearClick
      end
      object cxLabel2: TcxLabel
        Left = 28
        Top = 61
        Caption = #36827#24230
      end
      object pbMake: TcxProgressBar
        Left = 70
        Top = 60
        Anchors = [akLeft, akTop, akRight]
        Properties.BorderWidth = 1
        Properties.ShowTextStyle = cxtsText
        TabOrder = 1
        Width = 714
      end
      object btnRebuild: TcxButton
        Left = 290
        Top = 110
        Width = 75
        Height = 25
        Caption = #37325#26032#32534#35793
        Enabled = False
        TabOrder = 7
        OnClick = btnRebuildClick
      end
      object btnStart: TcxButton
        Left = 180
        Top = 110
        Width = 75
        Height = 25
        Caption = #32534#35793
        TabOrder = 8
        OnClick = btnStartClick
      end
      object edtProject: TcxButtonEdit
        Left = 70
        Top = 33
        Properties.Buttons = <
          item
            Default = True
            Kind = bkEllipsis
          end>
        Properties.ReadOnly = True
        Properties.OnButtonClick = edtProjectPropertiesButtonClick
        TabOrder = 0
        Width = 714
      end
    end
    object mmInfo: TcxMemo
      Left = 0
      Top = 154
      Align = alClient
      Properties.ReadOnly = True
      Properties.ScrollBars = ssBoth
      Style.BorderStyle = ebs3D
      TabOrder = 1
      Height = 519
      Width = 800
    end
  end
  object cxSplitter1: TcxSplitter
    Left = 146
    Top = 0
    Width = 6
    Height = 673
    AlignSplitter = salRight
    Control = Panel1
  end
  object TimerCheckTask: TTimer
    OnTimer = TimerCheckTaskTimer
    Left = 368
    Top = 184
  end
  object OpenDialog: TFileOpenDialog
    FavoriteLinks = <>
    FileTypes = <
      item
        DisplayName = 'make'
        FileMask = '*.make'
      end>
    Options = []
    Left = 256
    Top = 184
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    SkinName = 'MoneyTwins'
    Left = 272
    Top = 336
  end
  object TimerQueryMake: TTimer
    OnTimer = TimerQueryMakeTimer
    Left = 152
    Top = 464
  end
  object TimerInit: TTimer
    OnTimer = TimerInitTimer
    Left = 280
    Top = 456
  end
  object TimerCheckObj: TTimer
    Interval = 2000
    OnTimer = TimerCheckObjTimer
    Left = 272
    Top = 536
  end
end
