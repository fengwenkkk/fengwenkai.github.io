inherited FormTrainCfg: TFormTrainCfg
  ClientHeight = 390
  ClientWidth = 821
  OnCreate = FormCreate
  ExplicitWidth = 837
  ExplicitHeight = 429
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 821
    Height = 390
    ExplicitWidth = 821
    ExplicitHeight = 390
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #21015#36710
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      ExplicitWidth = 304
      ExplicitHeight = 201
      Height = 390
      Width = 821
      object tlMain: TcxTreeList
        Left = 3
        Top = 16
        Width = 479
        Height = 315
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
        ExplicitTop = 19
        ExplicitWidth = 481
        ExplicitHeight = 317
        object Column1: TcxTreeListColumn
          Caption.Text = #36710#20307#21495
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object Column2: TcxTreeListColumn
          Caption.Text = #31867#22411
          DataBinding.ValueType = 'Integer'
          Width = 135
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = Column2GetDisplayText
        end
        object tlMainColumn7: TcxTreeListColumn
          Caption.Text = #36710#36733#21488'1'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn8: TcxTreeListColumn
          Caption.Text = #36710#36733#21488'2'
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn3: TcxTreeListColumn
          Caption.Text = #21015#36710#36890#35805#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn4: TcxTreeListColumn
          Visible = False
          Caption.Text = #36710#36742#27573#21015#36710#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn5: TcxTreeListColumn
          Visible = False
          Caption.Text = #21015#36710#24191#25773#32452
          DataBinding.ValueType = 'LargeInt'
          Width = 100
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn9: TcxTreeListColumn
          Caption.Text = #21019#24314#32773
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn6: TcxTreeListColumn
          Caption.Text = #29366#24577
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
      object gbCtrl: TcxGroupBox
        Left = 3
        Top = 331
        Align = alBottom
        Caption = ' '
        Ctl3D = False
        PanelStyle.Active = True
        ParentCtl3D = False
        Style.BorderColor = clRed
        Style.BorderStyle = ebs3D
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Black'
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Black'
        TabOrder = 1
        ExplicitLeft = 2
        ExplicitTop = 336
        ExplicitWidth = 817
        DesignSize = (
          815
          52)
        Height = 52
        Width = 815
        object btnDel: TcxButton
          Left = 105
          Top = 13
          Width = 75
          Height = 25
          Caption = #21024#38500
          LookAndFeel.NativeStyle = False
          TabOrder = 1
          OnClick = btnDelClick
        end
        object btnAdd: TcxButton
          Left = 15
          Top = 13
          Width = 75
          Height = 25
          Caption = #22686#21152
          LookAndFeel.NativeStyle = False
          TabOrder = 0
          OnClick = btnAddClick
        end
        object btnRefresh: TcxButton
          Left = 285
          Top = 13
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #21047#26032
          LookAndFeel.NativeStyle = False
          TabOrder = 3
          OnClick = btnRefreshClick
        end
        object btnSet: TcxButton
          Left = 195
          Top = 13
          Width = 75
          Height = 25
          Caption = #20462#25913
          LookAndFeel.NativeStyle = False
          TabOrder = 2
          OnClick = btnSetClick
        end
        object btnImport: TcxButton
          Left = 375
          Top = 13
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #23548#20837
          LookAndFeel.NativeStyle = False
          TabOrder = 4
          Visible = False
          OnClick = btnImportClick
        end
        object btnCheck: TcxButton
          Left = 375
          Top = 13
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #26816#26597
          LookAndFeel.NativeStyle = False
          OptionsImage.Glyph.SourceDPI = 96
          OptionsImage.Glyph.Data = {
            89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
            61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
            00097048597300000EC300000EC301C76FA8640000029749444154384F6D52F9
            4B9451149524CBB5F54FB10C228984849428090C524A32318429332735452CD4
            D469B41CB54C5B702983A0991035CD155331378ACA9A6A5A4C719919C7719CF9
            6639DDFB3EE5FB210F9CF77EB9E7DC7BEE7B7EEBF0AF6E9CE8AD793605C1A753
            A86E9E14AC62363127A06B9C40C5A3A17EAA0F90650AB672F1067C4C3A7C7428
            B70F5EE29D2763A0FA4059A620A0B2615C143B25AFE09A8BE8F4C034FB155775
            5168EA2885C7E3C3EDFA51360896650AB6553C7E270C58D8D05E08B52E1ADF67
            8C50571D41CF7B0D322A622079BC287D30BCB981E6E1A8187175CD0395260A3D
            1FCA70491B89BAAE53485047C0F8DB0497DB8BA29AB76C1022CB146C2FB93F0C
            AFD707BBC34DC5DF70B1F810B486A388BF1C8E2FBF4CB0AE4A22DA8DCA013608
            95650A020BAB07E121031B192CAFBAF1F9A711275323306DFA01B35D82654512
            F1F2B47D6C1026CB140415DC1D809B966425B1D5EE1602E6D28A0B4B36171696
            5DB053BCECB26E36D821CB1404E796F742A28CDC6DF25E1DBA12CEC1D8378CCE
            336731A6ABC5BCD589159A2EB3A4737383ACD26EB124EEF63C2E1E7F9BEBD07A
            3A1E338DB5683A1E87398B93A249482F7CCD063B65998290CCE24E91913B4DF7
            8DA0233616F345E9683F16838F3D23985D72C242D3A90ADAD860972C53109A7E
            B31D0EFA3873E635B4449F80F14A12EAC3F7E1535A22CAC30F6266D101B34D42
            5A5E2B1BEC96650AC254F96DB424B7E8A4CFBF05ED81C3187F3304CDFE48BCC8
            2DC29F0507166991A939AFD8608F2C53109696DB2A9ED046AFC0592DF412667A
            0133ED64913A2F50B479AB0B2959FA4D27084CCA68E94FB96E404A8E01C95906
            5CC8D6CBBCA6C77962B2FAA560A2AA6190EAFFFB895B884144DE2E2F883B3079
            D40DEE5DBFF917FA13097E7EFF007E9D15F9A25E890C0000000049454E44AE42
            6082}
          TabOrder = 5
          OnClick = btnCheckClick
        end
      end
      object mmESDKGroup: TcxMemo
        Left = 482
        Top = 16
        Align = alRight
        Lines.Strings = (
          'mmESDKGroup')
        Properties.ScrollBars = ssBoth
        TabOrder = 2
        Visible = False
        ExplicitLeft = 483
        ExplicitTop = 19
        ExplicitHeight = 314
        Height = 315
        Width = 336
      end
    end
  end
  object OpenDialog: TOpenDialog
    Left = 200
    Top = 184
  end
  object TimerCheck: TTimer
    Interval = 10000
    OnTimer = TimerCheckTimer
    Left = 416
    Top = 192
  end
end
