inherited FormOperLog: TFormOperLog
  BorderIcons = [biSystemMenu]
  Caption = #25805#20316#26085#24535
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
    Width = 868
    Height = 538
    ExplicitWidth = 900
    ExplicitHeight = 601
    object gbBasicInfo: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #22522#26412#20449#24687
      PanelStyle.Active = True
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      Visible = False
      ExplicitWidth = 900
      ExplicitHeight = 549
      Height = 486
      Width = 868
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
      TabOrder = 1
      ExplicitTop = 549
      ExplicitWidth = 900
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
        ExplicitLeft = 665
      end
      object btnClose: TcxButton
        Left = 743
        Top = 12
        Width = 75
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = #20851#38381
        LookAndFeel.NativeStyle = False
        TabOrder = 1
        OnClick = btnCloseClick
        ExplicitLeft = 775
      end
    end
    object tlInfo: TcxTreeList
      Left = 0
      Top = 0
      Width = 868
      Height = 486
      BorderStyle = cxcbsNone
      Align = alClient
      Bands = <
        item
        end>
      LookAndFeel.NativeStyle = False
      Navigator.Buttons.CustomButtons = <>
      OptionsData.Editing = False
      OptionsView.GridLines = tlglBoth
      OptionsView.ShowRoot = False
      TabOrder = 2
      ExplicitWidth = 884
      ExplicitHeight = 472
      object Column1: TcxTreeListColumn
        Caption.Text = #26102#38388
        DataBinding.ValueType = 'String'
        Width = 163
        Position.ColIndex = 0
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
      object Column2: TcxTreeListColumn
        Caption.Text = #25805#20316#26085#24535
        DataBinding.ValueType = 'String'
        Width = 645
        Position.ColIndex = 1
        Position.RowIndex = 0
        Position.BandIndex = 0
        Summary.FooterSummaryItems = <>
        Summary.GroupFooterSummaryItems = <>
      end
    end
  end
end
