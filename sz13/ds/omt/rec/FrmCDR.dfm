inherited FormCDR: TFormCDR
  ClientHeight = 507
  ClientWidth = 1234
  ExplicitWidth = 1250
  ExplicitHeight = 546
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1234
    Height = 507
    ExplicitWidth = 1234
    ExplicitHeight = 507
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      PanelStyle.Active = True
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Height = 507
      Width = 1234
      object pcMain: TcxPageControl
        Left = 3
        Top = 3
        Width = 1228
        Height = 501
        Align = alClient
        TabOrder = 0
        Properties.ActivePage = tsBrd
        Properties.CustomButtons.Buttons = <>
        Properties.TabWidth = 120
        LookAndFeel.SkinName = 'Office2010Blue'
        ClientRectBottom = 495
        ClientRectLeft = 2
        ClientRectRight = 1222
        ClientRectTop = 28
        object tsP2PCall: TcxTabSheet
          Caption = #20010#21628
          ImageIndex = 0
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object tlP2PCall: TcxTreeList
            Left = 0
            Top = 0
            Width = 1220
            Height = 467
            BorderStyle = cxcbsNone
            Align = alClient
            Bands = <
              item
              end>
            LookAndFeel.NativeStyle = False
            Navigator.Buttons.CustomButtons = <>
            OptionsData.Editing = False
            OptionsView.GridLines = tlglBoth
            OptionsView.Indicator = True
            OptionsView.ShowRoot = False
            TabOrder = 0
            object cxTreeListColumn7: TcxTreeListColumn
              Caption.Text = #26102#38388
              DataBinding.ValueType = 'String'
              Width = 143
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn9: TcxTreeListColumn
              Caption.Text = #21628#21483#31867#21035
              DataBinding.ValueType = 'Integer'
              Width = 112
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn8: TcxTreeListColumn
              Caption.Text = #20027#21483
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn10: TcxTreeListColumn
              Caption.Text = #34987#21483
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlP2PColumn5: TcxTreeListColumn
              Caption.Text = #29366#24577
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
        end
        object tsGroupCall: TcxTabSheet
          Caption = #32452#21628
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object tlGroupCall: TcxTreeList
            Left = 0
            Top = 0
            Width = 1220
            Height = 467
            BorderStyle = cxcbsNone
            Align = alClient
            Bands = <
              item
              end>
            LookAndFeel.NativeStyle = False
            LookAndFeel.SkinName = 'Office2010Blue'
            Navigator.Buttons.CustomButtons = <>
            OptionsData.Editing = False
            OptionsView.GridLines = tlglBoth
            OptionsView.Indicator = True
            OptionsView.ShowRoot = False
            TabOrder = 0
            object cxTreeListColumn2: TcxTreeListColumn
              Caption.Text = #26102#38388
              DataBinding.ValueType = 'String'
              Width = 143
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn4: TcxTreeListColumn
              Caption.Text = #21628#21483#31867#21035
              DataBinding.ValueType = 'Integer'
              Width = 111
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlGroupCallColumn5: TcxTreeListColumn
              Caption.Text = #36890#35805#32452#21495#30721
              DataBinding.ValueType = 'LargeInt'
              Width = 100
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn3: TcxTreeListColumn
              Caption.Text = #36890#35805#32452#21517#31216
              DataBinding.ValueType = 'String'
              Width = 128
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object cxTreeListColumn5: TcxTreeListColumn
              Caption.Text = #35805#26435#20154
              DataBinding.ValueType = 'Integer'
              Width = 102
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
        end
        object tsEmergencyCall: TcxTabSheet
          Caption = #32039#24613#21628#21483
          ImageIndex = 3
          TabVisible = False
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
        end
        object tsBrd: TcxTabSheet
          Caption = #20154#24037#24191#25773
          ImageIndex = 4
        end
      end
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = '*.xls'
    Filter = '*.xls|*.xls|*.csv|*.csv'
    Left = 352
    Top = 176
  end
  object TimerInit: TTimer
    OnTimer = TimerInitTimer
    Left = 191
    Top = 308
  end
end
