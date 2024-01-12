object DialogSetTrainDepotID: TDialogSetTrainDepotID
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #21015#36710#25386#36710
  ClientHeight = 443
  ClientWidth = 508
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
  object cxLabel6: TcxLabel
    Left = 63
    Top = 30
    Caption = #30446#30340#21306#22495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 383
    Width = 508
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 6
    DesignSize = (
      508
      60)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 508
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -650
      ExplicitTop = 47
      ExplicitWidth = 1000
    end
    object btnOK: TcxButton
      Left = 270
      Top = 22
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #30830#35748
      Default = True
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 370
      Top = 22
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
  object cxLabel1: TcxLabel
    Left = 48
    Top = 70
    Caption = #36816#34892#30446#30340#22320
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object cbDest: TcxComboBox
    Left = 144
    Top = 69
    TabOrder = 2
    Width = 173
  end
  object tlMain: TcxTreeList
    Left = 144
    Top = 110
    Width = 321
    Height = 259
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
    TabOrder = 3
    object cxTreeListColumn1: TcxTreeListColumn
      Caption.Text = #24207#21495
      DataBinding.ValueType = 'Integer'
      Options.Editing = False
      Width = 69
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object cxTreeList2Column1: TcxTreeListColumn
      Caption.Text = #36710#20307#21495
      DataBinding.ValueType = 'Integer'
      Width = 139
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
  object cxLabel2: TcxLabel
    Left = 33
    Top = 110
    Caption = #25386#20986#21015#36710#21015#34920
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 127
  end
  object cbDestDepotID: TcxComboBox
    Left = 144
    Top = 29
    TabOrder = 0
    Width = 173
  end
end
