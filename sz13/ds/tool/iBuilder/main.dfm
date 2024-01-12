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
  WindowState = wsMaximized
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object tlSourceFile: TcxTreeList
    AlignWithMargins = True
    Left = 1
    Top = 1
    Width = 544
    Height = 671
    Margins.Left = 1
    Margins.Top = 1
    Margins.Right = 1
    Margins.Bottom = 1
    BorderStyle = cxcbsNone
    Align = alLeft
    Bands = <
      item
      end>
    Navigator.Buttons.CustomButtons = <>
    OptionsData.Editing = False
    OptionsData.Deleting = False
    TabOrder = 0
    OnClick = tlSourceFileClick
    OnCustomDrawDataCell = tlSourceFileCustomDrawDataCell
    OnDblClick = tlSourceFileDblClick
    OnKeyDown = tlSourceFileKeyDown
    OnKeyUp = tlSourceFileKeyUp
    object ColumnFile: TcxTreeListColumn
      Caption.Text = #25991#20214
      DataBinding.ValueType = 'String'
      Width = 186
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnState: TcxTreeListColumn
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'Integer'
      Width = 59
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
      Width = 100
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnState2: TcxTreeListColumn
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'Integer'
      Width = 100
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
      Width = 100
      Position.ColIndex = 4
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object tlSourceFileColumn6: TcxTreeListColumn
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'Integer'
      Width = 100
      Position.ColIndex = 5
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnStateGetDisplayText
    end
  end
  object Panel1: TPanel
    Left = 552
    Top = 0
    Width = 400
    Height = 673
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    ExplicitLeft = 553
    ExplicitWidth = 399
    object PanelTop: TPanel
      AlignWithMargins = True
      Left = 1
      Top = 1
      Width = 398
      Height = 192
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
      ExplicitWidth = 397
      DesignSize = (
        398
        192)
      object btnStart: TcxButton
        Left = 64
        Top = 150
        Width = 75
        Height = 25
        Caption = #32534#35793
        Enabled = False
        TabOrder = 6
        OnClick = btnStartClick
      end
      object Label1: TcxLabel
        Left = 28
        Top = 48
        Caption = #24037#31243
      end
      object edtProject: TcxTextEdit
        Left = 64
        Top = 46
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 1
        ExplicitWidth = 319
        Width = 320
      end
      object btnStop: TcxButton
        Left = 196
        Top = 110
        Width = 75
        Height = 25
        Caption = #20013#27490
        Enabled = False
        TabOrder = 9
        OnClick = btnStopClick
      end
      object btnOpen: TcxButton
        Left = 64
        Top = 110
        Width = 75
        Height = 25
        Caption = #25171#24320
        TabOrder = 3
        OnClick = btnOpenClick
      end
      object btnClear: TcxButton
        Left = 316
        Top = 110
        Width = 75
        Height = 25
        Caption = #28165#38500
        TabOrder = 4
        Visible = False
        OnClick = btnClearClick
      end
      object cxLabel1: TcxLabel
        Left = 20
        Top = 18
        Caption = #24182#21457#25968
      end
      object cxLabel2: TcxLabel
        Left = 28
        Top = 78
        Caption = #36827#24230
      end
      object pbBuild: TcxProgressBar
        Left = 64
        Top = 77
        Anchors = [akLeft, akTop, akRight]
        Properties.BorderWidth = 1
        TabOrder = 2
        ExplicitWidth = 319
        Width = 320
      end
      object btnRebuild: TcxButton
        Left = 196
        Top = 150
        Width = 75
        Height = 25
        Caption = #37325#26032#32534#35793
        Enabled = False
        TabOrder = 10
        OnClick = btnRebuildClick
      end
      object cbCpuNum: TcxComboBox
        Left = 64
        Top = 16
        Properties.DropDownListStyle = lsFixedList
        Properties.DropDownRows = 24
        TabOrder = 0
        Width = 73
      end
    end
    object mmInfo: TcxMemo
      Left = 0
      Top = 194
      Align = alClient
      Properties.ReadOnly = True
      Properties.ScrollBars = ssBoth
      Style.BorderStyle = ebs3D
      TabOrder = 1
      ExplicitWidth = 399
      Height = 479
      Width = 400
    end
  end
  object cxSplitter1: TcxSplitter
    Left = 546
    Top = 0
    Width = 6
    Height = 673
    Control = tlSourceFile
  end
  object tmrCheck: TTimer
    Interval = 1
    OnTimer = tmrCheckTimer
    Left = 352
    Top = 184
  end
  object dlgOpen: TOpenDialog
    DefaultExt = '*.cbproj'
    FileName = '*.cbproj'
    Filter = 'cbproj|*.cbproj'
    Left = 408
    Top = 104
  end
  object OpenDialog: TFileOpenDialog
    FavoriteLinks = <>
    FileTypes = <
      item
        DisplayName = 'cbproj'
        FileMask = '*.cbproj'
      end>
    Options = []
    Left = 240
    Top = 176
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    SkinName = 'MoneyTwins'
    Left = 272
    Top = 336
  end
  object tpBuild: TdxTaskbarProgress
    Active = True
    Position = 0
    Left = 465
    Top = 240
  end
end
