object DialogNewDynamicGroup: TDialogNewDynamicGroup
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #26032#24314#21160#24577#32452
  ClientHeight = 616
  ClientWidth = 884
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  DesignSize = (
    884
    616)
  PixelsPerInch = 96
  TextHeight = 18
  object cxLabel6: TcxLabel
    Left = 30
    Top = 75
    Caption = #32452#21517#31216
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 79
  end
  object edtName: TcxTextEdit
    Left = 100
    Top = 73
    Properties.MaxLength = 31
    TabOrder = 1
    Width = 175
  end
  object PanelCfgCtrl: TPanel
    Left = 0
    Top = 560
    Width = 884
    Height = 56
    Align = alBottom
    BevelOuter = bvNone
    ParentBackground = False
    TabOrder = 4
    DesignSize = (
      884
      56)
    object dxBevel1: TdxBevel
      Left = 0
      Top = 0
      Width = 884
      Height = 3
      Align = alTop
      LookAndFeel.NativeStyle = False
      Shape = dxbsLineTop
      ExplicitLeft = -261
      ExplicitTop = 47
      ExplicitWidth = 1145
    end
    object btnOK: TcxButton
      Left = 610
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #30830#35748
      Default = True
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TcxButton
      Left = 710
      Top = 18
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCancelClick
    end
  end
  object cxLabel1: TcxLabel
    Left = 30
    Top = 30
    Caption = #32452#32534#21495
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 79
  end
  object edtGroupID: TcxTextEdit
    Left = 100
    Top = 28
    Properties.MaxLength = 31
    TabOrder = 0
    Width = 175
  end
  object tlMain: TcxTreeList
    Left = 100
    Top = 120
    Width = 745
    Height = 422
    Anchors = [akLeft, akTop, akRight, akBottom]
    Bands = <
      item
      end>
    LookAndFeel.NativeStyle = False
    LookAndFeel.ScrollbarMode = sbmClassic
    Navigator.Buttons.CustomButtons = <>
    OptionsBehavior.Sorting = False
    OptionsCustomizing.BandCustomizing = False
    OptionsData.Editing = False
    OptionsSelection.HideSelection = True
    OptionsSelection.InvertSelect = False
    OptionsView.ColumnAutoWidth = True
    OptionsView.CheckGroups = True
    OptionsView.GridLines = tlglBoth
    OptionsView.Headers = False
    OptionsView.TreeLineStyle = tllsNone
    Styles.UseOddEvenStyles = bFalse
    TabOrder = 2
    object ColumnUserName: TcxTreeListColumn
      Caption.Text = 'UserName'
      DataBinding.ValueType = 'String'
      Width = 200
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnUserID: TcxTreeListColumn
      Visible = False
      Caption.Text = 'UserID'
      DataBinding.ValueType = 'Integer'
      Width = 100
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnUserType: TcxTreeListColumn
      Visible = False
      Caption.Text = 'UserType'
      DataBinding.ValueType = 'Integer'
      Width = 100
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
  object cxLabel2: TcxLabel
    Left = 30
    Top = 120
    Caption = #32452#25104#21592
    Style.BorderColor = clActiveCaption
    Properties.Alignment.Horz = taRightJustify
    Transparent = True
    AnchorX = 79
  end
end
