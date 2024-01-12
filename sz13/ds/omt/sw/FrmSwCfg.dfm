inherited FormSwCfg: TFormSwCfg
  ClientHeight = 623
  ClientWidth = 1117
  OnCreate = FormCreate
  OnShow = FormShow
  ExplicitWidth = 1133
  ExplicitHeight = 662
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1117
    Height = 623
    ExplicitWidth = 1117
    ExplicitHeight = 623
    object pcMain: TcxPageControl
      Left = 0
      Top = 0
      Width = 1117
      Height = 623
      Align = alClient
      TabOrder = 0
      Properties.ActivePage = tsSwCfg
      Properties.CustomButtons.Buttons = <>
      Properties.TabWidth = 100
      ClientRectBottom = 617
      ClientRectLeft = 2
      ClientRectRight = 1111
      ClientRectTop = 28
      object tsSwUpdate: TcxTabSheet
        Caption = #29256#26412#21319#32423
        ImageIndex = 0
        ExplicitLeft = 4
        ExplicitTop = 25
        ExplicitHeight = 594
      end
      object tsSwCfg: TcxTabSheet
        Caption = #29256#26412#24211
        ImageIndex = 1
        ExplicitLeft = 4
        ExplicitTop = 25
        ExplicitHeight = 594
        object gbMain: TcxGroupBox
          Left = 0
          Top = 0
          Align = alClient
          Alignment = alTopCenter
          PanelStyle.Active = True
          Style.LookAndFeel.NativeStyle = False
          StyleDisabled.LookAndFeel.NativeStyle = False
          TabOrder = 0
          ExplicitHeight = 594
          Height = 589
          Width = 1109
          object tlMain: TcxTreeList
            Left = 3
            Top = 83
            Width = 1103
            Height = 503
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
            ExplicitLeft = 2
            ExplicitTop = 82
            ExplicitWidth = 1105
            ExplicitHeight = 510
            object ColumnVer: TcxTreeListColumn
              Caption.Text = #29256#26412#21517#31216
              DataBinding.ValueType = 'String'
              Width = 195
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object ColumnFile: TcxTreeListColumn
              Caption.Text = #29256#26412#25991#20214
              DataBinding.ValueType = 'String'
              Width = 446
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object ColumnType: TcxTreeListColumn
              Caption.Text = #29256#26412#31867#22411
              DataBinding.ValueType = 'Integer'
              Width = 112
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = ColumnTypeGetDisplayText
            end
            object ColumnTime: TcxTreeListColumn
              Caption.Text = #29256#26412#26102#38388
              DataBinding.ValueType = 'String'
              Width = 156
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
          end
          object gbCtrl: TcxGroupBox
            Left = 3
            Top = 3
            Align = alTop
            Caption = ' '
            Ctl3D = False
            PanelStyle.Active = True
            ParentCtl3D = False
            Style.BorderColor = clRed
            Style.BorderStyle = ebs3D
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 1
            ExplicitLeft = 2
            ExplicitTop = 2
            ExplicitWidth = 1105
            Height = 80
            Width = 1103
            object btnDel: TcxButton
              Left = 620
              Top = 30
              Width = 75
              Height = 25
              Caption = #21024#38500
              LookAndFeel.NativeStyle = False
              TabOrder = 1
              OnClick = btnDelClick
            end
            object btnAdd: TcxButton
              Left = 420
              Top = 30
              Width = 75
              Height = 25
              Caption = #22686#21152
              LookAndFeel.NativeStyle = False
              TabOrder = 0
              OnClick = btnAddClick
            end
            object btnRefresh: TcxButton
              Left = 820
              Top = 30
              Width = 75
              Height = 25
              Caption = #21047#26032
              LookAndFeel.NativeStyle = False
              TabOrder = 2
              OnClick = btnRefreshClick
            end
          end
        end
      end
      object tsUserCfg: TcxTabSheet
        Caption = #29992#25143#31649#29702
        ImageIndex = 2
      end
    end
  end
end
