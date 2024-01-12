object FormActiveAlarm: TFormActiveAlarm
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'FormActiveAlarm'
  ClientHeight = 740
  ClientWidth = 1800
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object gbWorkPanel: TcxGroupBox
    Left = 0
    Top = 0
    Align = alTop
    PanelStyle.Active = True
    Style.BorderStyle = ebs3D
    Style.Edges = []
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Office2010Blue'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
    TabOrder = 0
    Height = 60
    Width = 1800
    object btnRefresh: TcxButton
      Left = 1680
      Top = 17
      Width = 75
      Height = 25
      Caption = #21047#26032
      LookAndFeel.NativeStyle = False
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
        89504E470D0A1A0A0000000D494844520000001000000010080600000168F4CF
        F7000000017352474200AECE1CE900000044655849664D4D002A000000080001
        87690004000000010000001A000000000003A00100030000000100010000A002
        00040000000100000010A0030004000000010000001000000000345571F20000
        021E4944415438119D534168D44014FD93DD6D33C96C96BA82A016A4AB14F760
        11948A27A1E041BC7A10048F0AE24548D282488A6261172AD5BB089E3C2878B6
        28F5D68B78944A45B4450B2DAC996C926ED97CFF44B2CD966E0FCE65FE7FF3DF
        9F97373F00072FDFD157FB2AA4A36F6B0AA12052BB30CA55B5F72FF40E597944
        0BC27023037C5BC7222B1626D1BB546CB797EF8866C4D8EE29FF0E0C04002E03
        C215AD54382B1EB73FB3D8B64E75A0B36235E35EB122C5F72BB59DCEF60D4D6F
        FA5FB32EF95D7FF4671581EDB294BAA01D2D2183510DE121DDBF90270C8CD37B
        D11DA9C8246A31C69648E0380246A4692C65A94FDA4B0F67F8A4B4F50F29AE7C
        0BA6CDCB7B8B94290A2373D95B53B717F305E89DD0554EE4F710BAFCA274F8B3
        7C818AE9AD9EF7304AA4748CDB0A506C69F337D961FA7E56232E032455121606
        E1C63B317AEC7A56D0676FDAE1E9C9E1606DFD16C5D748E569066802B22FA8C1
        ABF291EA02BBB7960E43D620BD214BD03B6A043FD75F02633BC2189BB21AD1E1
        7223E6E2FCD57390B04DF96BF3B76FF32D3923EA1927DD7D974FFB8EF1A20F1C
        9090214F9485A1C32FA8122D353CC139C0E4261D7CC3F9E37C00378545ADEED0
        10865D84BB88F86F1EA56BCC62923C182A96C6F539B9725083C031A712EC2E92
        7951A93074A6574BD23E929A4E26AD77B04FA026331BA1BE57085C732249BAAF
        89532323B7E8053E914A417F4A1D102B348C3F84313CC1BC566B9FBEFF07FD05
        976ED54D26A48EAE0000000049454E44AE426082}
      TabOrder = 7
      OnClick = btnRefreshClick
    end
    object cbDevType: TcxComboBox
      Left = 88
      Top = 19
      Properties.DropDownRows = 30
      Properties.OnChange = cbDevTypePropertiesChange
      TabOrder = 0
      Width = 165
    end
    object deStartTime: TcxDateEdit
      Left = 904
      Top = 19
      Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
      Properties.EditFormat = 'YYYY-MM-DD HH:mm'
      Properties.InputKind = ikRegExpr
      Properties.Kind = ckDateTime
      Properties.OnChange = deStartTimePropertiesChange
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Office2010Blue'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 3
      Width = 165
    end
    object deEndTime: TcxDateEdit
      Left = 1176
      Top = 19
      Properties.DisplayFormat = 'YYYY-MM-DD HH:mm'
      Properties.EditFormat = 'YYYY-MM-DD HH:mm'
      Properties.InputKind = ikRegExpr
      Properties.Kind = ckDateTime
      Properties.OnChange = deEndTimePropertiesChange
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Office2010Blue'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 4
      Width = 165
    end
    object edtSearch: TcxTextEdit
      Left = 1448
      Top = 19
      Properties.OnChange = edtSearchPropertiesChange
      TabOrder = 5
      Width = 165
    end
    object cbAlarmID: TcxComboBox
      Left = 632
      Top = 19
      Properties.OnChange = cbAlarmIDPropertiesChange
      TabOrder = 2
      Width = 165
    end
    object cbAlarmLevel: TcxComboBox
      Left = 360
      Top = 19
      Properties.OnChange = cbAlarmLevelPropertiesChange
      TabOrder = 1
      Width = 165
    end
    object cxLabel1: TcxLabel
      Left = 832
      Top = 21
      Caption = #24320#22987#26102#38388
    end
    object cxLabel3: TcxLabel
      Left = 1100
      Top = 21
      Caption = #32467#26463#26102#38388
    end
    object cxLabel2: TcxLabel
      Left = 284
      Top = 21
      Caption = #21578#35686#31561#32423
    end
    object cxLabel4: TcxLabel
      Left = 560
      Top = 21
      Caption = #21578#35686#31867#21035
    end
    object cxLabel5: TcxLabel
      Left = 1376
      Top = 21
      Caption = #25628#32034#20869#23481
    end
    object cxLabel6: TcxLabel
      Left = 16
      Top = 21
      Caption = #35774#22791#31867#22411
    end
  end
  object tlActiveAlarm: TcxTreeList
    Left = 0
    Top = 60
    Width = 1800
    Height = 680
    BorderStyle = cxcbsNone
    Align = alClient
    Bands = <
      item
      end>
    LookAndFeel.NativeStyle = False
    LookAndFeel.SkinName = 'Office2010Blue'
    Navigator.Buttons.CustomButtons = <>
    OptionsData.Editing = False
    OptionsData.Deleting = False
    OptionsSelection.InvertSelect = False
    OptionsView.CheckGroups = True
    OptionsView.GridLines = tlglBoth
    OptionsView.Indicator = True
    PopupMenu = pmActiveAlarm
    TabOrder = 1
    OnCustomDrawDataCell = tlActiveAlarmCustomDrawDataCell
    ExplicitTop = 105
    ExplicitWidth = 1350
    ExplicitHeight = 635
    object ColumnNeID: TcxTreeListColumn
      Visible = False
      Caption.Text = #35774#22791#21517#31216
      DataBinding.ValueType = 'String'
      Options.Editing = False
      Width = 102
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnDevName: TcxTreeListColumn
      Caption.Text = #35774#22791#21517#31216
      DataBinding.ValueType = 'String'
      Width = 120
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnDevType: TcxTreeListColumn
      Caption.Text = #35774#22791#31867#22411
      DataBinding.ValueType = 'String'
      Width = 165
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnDevNameGetDisplayText
    end
    object ColumnAlarmID: TcxTreeListColumn
      Caption.Text = #21578#35686#31867#21035
      DataBinding.ValueType = 'Integer'
      Width = 125
      Position.ColIndex = 3
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmIDGetDisplayText
    end
    object ColumnAlarmLevel: TcxTreeListColumn
      Caption.Text = #21578#35686#31561#32423
      DataBinding.ValueType = 'Integer'
      Width = 100
      Position.ColIndex = 4
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmLevelGetDisplayText
    end
    object ColumnAlarmType: TcxTreeListColumn
      PropertiesClassName = 'TcxImageComboBoxProperties'
      Properties.Items = <
        item
          Description = #19978#34892
          ImageIndex = 0
          Value = 1
        end
        item
          Description = #19979#34892
          ImageIndex = 1
          Value = 2
        end>
      Properties.ShowDescriptions = False
      Visible = False
      Caption.Text = #21578#35686#31867#22411
      DataBinding.ValueType = 'Integer'
      Width = 70
      Position.ColIndex = 5
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnAlarmRaiseTime: TcxTreeListColumn
      Caption.Text = #21578#35686#20135#29983#26102#38388
      DataBinding.ValueType = 'Integer'
      Width = 143
      Position.ColIndex = 6
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnAlarmRaiseTimeGetDisplayText
    end
    object ColumnAlarmInfo: TcxTreeListColumn
      Caption.Text = #21578#35686#20869#23481
      DataBinding.ValueType = 'String'
      Width = 277
      Position.ColIndex = 7
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnAlarmReason: TcxTreeListColumn
      Visible = False
      Caption.Text = #21578#35686#21407#22240
      DataBinding.ValueType = 'String'
      Width = 135
      Position.ColIndex = 8
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnAlarmDealAdvise: TcxTreeListColumn
      Visible = False
      Caption.Text = #22788#29702#24314#35758
      DataBinding.ValueType = 'String'
      Width = 100
      Position.ColIndex = 9
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
  object TimerRefresh: TTimer
    Interval = 10000
    OnTimer = TimerRefreshTimer
    Left = 962
    Top = 330
  end
  object BarManager: TdxBarManager
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Microsoft YaHei UI'
    Font.Style = []
    Categories.Strings = (
      'Default')
    Categories.ItemsVisibles = (
      2)
    Categories.Visibles = (
      True)
    PopupMenuLinks = <>
    UseSystemFont = True
    Left = 72
    Top = 424
    PixelsPerInch = 96
    object pmiAlarmClear: TdxBarButton
      Caption = #21578#35686#28165#38500
      Category = 0
      Hint = #21578#35686#28165#38500
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
        610000001974455874536F6674776172650041646F626520496D616765526561
        647971C9653C0000000C744558745469746C650054726173683B649A356D0000
        024249444154785E75924F6BD44018C69FD9CC36E976975D28A52DDA562DB1C5
        2FE021C52FE04128C52A28BD7810912E5814113D78F0E4A907BD0852C4434FF5
        50B4071145DB83081E2C45BBBBAD8AF51F8B6CDBD5CD6EE69F99A181B0A40F09
        79923CEFF07BDF19AA94429BC8D9A96B6FFA7ABB3DC7B10128B45A0C3F7E5657
        E61FDD1D439B28122484F06AB5DDD9B907776E025053176EDC9652CE2041D475
        5D63CAE532C6CF14970921DED0E14184CFE2E973578A1A7060E8204252FD5FE3
        AE2CCCCF8EE9BA38013935397DFFD8E8B037E21E427F5F0F0AF91CD21D142CE0
        A8FFFD87ED9D3A46DD419437BE7961F61E80CB9A8EEA62002925E5C542EF01EC
        0629A4EB01489AC39104CD264775BB893FB526FC264177FF004AA5CD4B00A601
        C888C0924262F9C54B8D0E4A2DA42905491128A9C03807E7C2B4111BBA151118
        ADAF3ECF1F3F31BE3331711241AB85ADEFBF313A321C12F82855BEE2A87B04BE
        DFC0D2D22BBC7DBD5068DF05012050522293C982520A100BD95C0E969582440A
        99AE2C0801A410D0598D1F23302F4C2A820EDB41486E2663DBB62E306D394E67
        E83974265A2022507BDB2826CFCF98FE95A4900A86C40A6F2194FEAE7D442012
        0F92101C003197E01ACA583D40E3A24C5C541FA04882F3A8084CA3EF2DC6431F
        CF542A15E3955271021D94918D500D03E73C2193D402E7D50FAB6B3D9D8E6D82
        1B9B9FC1183733F8F8691D8D860FC1D9D6BE0B1025E79E3E59BCDA95CDA090CF
        E3D7970A1817085A0C6BEFDF210818A8957EBC2FC1B3C587D701DC8A9D0D2BC1
        33C4F41F3E8F1596C01C0CF00000000049454E44AE426082}
      OnClick = pmiAlarmClearClick
    end
  end
  object pmActiveAlarm: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiAlarmClear'
      end>
    UseOwnFont = False
    Left = 352
    Top = 456
    PixelsPerInch = 96
  end
end
