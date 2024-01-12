inherited FormNeInfo: TFormNeInfo
  BorderIcons = [biSystemMenu]
  ClientHeight = 538
  ClientWidth = 868
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  ExplicitWidth = 884
  ExplicitHeight = 577
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Top = 200
    Width = 868
    Height = 286
    Visible = False
    ExplicitTop = 200
    ExplicitWidth = 868
    ExplicitHeight = 286
  end
  object gbBasicInfo: TcxGroupBox
    Left = 0
    Top = 0
    Align = alTop
    Alignment = alTopCenter
    Caption = #22522#26412#20449#24687
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    TabOrder = 1
    Height = 200
    Width = 868
    object tlInfo: TcxTreeList
      Left = 2
      Top = 19
      Width = 864
      Height = 179
      Align = alClient
      Bands = <
        item
        end>
      LookAndFeel.NativeStyle = False
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsView.GridLines = tlglBoth
      OptionsView.Headers = False
      OptionsView.ShowRoot = False
      TabOrder = 0
      ExplicitLeft = 3
      ExplicitTop = 16
      ExplicitWidth = 862
      ExplicitHeight = 177
      object Column1: TcxTreeListColumn
        Styles.Content = StyleTitle
        Caption.Text = #32456#31471#21495#30721
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object Column2: TcxTreeListColumn
        Caption.Text = #32456#31471#21517#31216
        DataBinding.ValueType = 'String'
        Width = 180
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object Column3: TcxTreeListColumn
        Styles.Content = StyleTitle
        Caption.Text = #20027#26426#22320#22336
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object Column4: TcxTreeListColumn
        Caption.Text = #36710#20307#21495
        DataBinding.ValueType = 'String'
        Width = 180
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
  end
  object gbStatus: TcxGroupBox
    Left = 0
    Top = 200
    Align = alClient
    Alignment = alTopCenter
    Caption = #35774#22791#29366#24577
    Style.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.NativeStyle = False
    TabOrder = 2
    Height = 286
    Width = 868
    object tlStatus: TcxTreeList
      Left = 2
      Top = 19
      Width = 864
      Height = 265
      Align = alClient
      Bands = <
        item
        end>
      LookAndFeel.NativeStyle = False
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsView.GridLines = tlglBoth
      OptionsView.Headers = False
      OptionsView.ShowRoot = False
      TabOrder = 0
      ExplicitLeft = 3
      ExplicitTop = 16
      ExplicitWidth = 862
      ExplicitHeight = 263
      object cxTreeListColumn1: TcxTreeListColumn
        Styles.Content = StyleTitle
        Caption.Text = #32456#31471#21495#30721
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object cxTreeListColumn2: TcxTreeListColumn
        Caption.Text = #32456#31471#21517#31216
        DataBinding.ValueType = 'String'
        Width = 180
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object cxTreeListColumn3: TcxTreeListColumn
        Styles.Content = StyleTitle
        Caption.Text = #20027#26426#22320#22336
        DataBinding.ValueType = 'String'
        Width = 150
        Position.ColIndex = 2
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object cxTreeListColumn4: TcxTreeListColumn
        Caption.Text = #36710#20307#21495
        DataBinding.ValueType = 'String'
        Width = 180
        Position.ColIndex = 3
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
  end
  object gbCtrl: TcxGroupBox
    Left = 0
    Top = 486
    Align = alBottom
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
    TabOrder = 3
    DesignSize = (
      868
      52)
    Height = 52
    Width = 868
    object btnRefresh: TcxButton
      Left = 633
      Top = 12
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21047#26032
      LookAndFeel.NativeStyle = False
      TabOrder = 0
      OnClick = btnRefreshClick
    end
    object btnClose: TcxButton
      Left = 750
      Top = 12
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #20851#38381
      LookAndFeel.NativeStyle = False
      TabOrder = 1
      OnClick = btnCloseClick
    end
  end
  object cxStyleRepository1: TcxStyleRepository
    Left = 16
    Top = 48
    PixelsPerInch = 96
    object StyleTitle: TcxStyle
      AssignedValues = [svColor]
      Color = clInactiveCaption
    end
  end
end
