object DialogSubGroup: TDialogSubGroup
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #32676#32452#36873#25321
  ClientHeight = 571
  ClientWidth = 634
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 18
  object tlMain: TcxTreeList
    Left = 0
    Top = 0
    Width = 634
    Height = 500
    Align = alClient
    Bands = <
      item
      end>
    LookAndFeel.SkinName = 'Office2010Blue'
    Navigator.Buttons.CustomButtons = <>
    OptionsCustomizing.DynamicSizing = True
    OptionsView.CheckGroups = True
    TabOrder = 0
    object tlUserColumn1: TcxTreeListColumn
      BestFitMaxWidth = 200
      DataBinding.ValueType = 'String'
      Options.Editing = False
      Width = 342
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object tlUserColumn2: TcxTreeListColumn
      Visible = False
      DataBinding.ValueType = 'LargeInt'
      Width = 100
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object tlUserColumn3: TcxTreeListColumn
      Visible = False
      DataBinding.ValueType = 'Integer'
      Width = 100
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
  object gbCtrl: TcxGroupBox
    Left = 0
    Top = 500
    Align = alBottom
    PanelStyle.Active = True
    Style.BorderStyle = ebs3D
    Style.Edges = []
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Office2010Blue'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
    TabOrder = 1
    DesignSize = (
      634
      71)
    Height = 71
    Width = 634
    object btnOK: TcxButton
      Left = 414
      Top = 24
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #30830#35748
      Default = True
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 510
      Top = 24
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
end
