inherited FormMulticastGroupCfg: TFormMulticastGroupCfg
  Caption = #36710#31449
  ClientHeight = 429
  ClientWidth = 931
  OnCreate = FormCreate
  ExplicitWidth = 947
  ExplicitHeight = 468
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 931
    Height = 429
    ExplicitWidth = 931
    ExplicitHeight = 429
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = #36890#25773#32452
      Style.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      TabOrder = 0
      Height = 429
      Width = 931
      object gbCtrl: TcxGroupBox
        Left = 3
        Top = 370
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
        TabOrder = 0
        DesignSize = (
          925
          52)
        Height = 52
        Width = 925
        object btnRefresh: TcxButton
          Left = 15
          Top = 13
          Width = 75
          Height = 25
          Anchors = [akLeft, akBottom]
          Caption = #21047#26032
          LookAndFeel.NativeStyle = False
          TabOrder = 0
          OnClick = btnRefreshClick
        end
        object btnCheck: TcxButton
          Left = 105
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
          TabOrder = 1
          Visible = False
        end
      end
      object Panel1: TPanel
        Left = 653
        Top = 16
        Width = 3
        Height = 354
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 1
        Visible = False
      end
      object gbMember: TcxGroupBox
        Left = 656
        Top = 16
        Align = alRight
        PanelStyle.Active = True
        TabOrder = 2
        Height = 354
        Width = 272
        object tlUser: TcxTreeList
          Left = -3
          Top = 3
          Width = 272
          Height = 348
          BorderStyle = cxcbsNone
          Align = alRight
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
          object cxTreeListColumn1: TcxTreeListColumn
            Caption.Text = #25104#21592
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 0
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object cxTreeList1Column2: TcxTreeListColumn
            Caption.Text = #29366#24577
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 1
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
        end
      end
      object gbGroup: TcxGroupBox
        Left = 3
        Top = 16
        Align = alClient
        PanelStyle.Active = True
        TabOrder = 3
        Height = 354
        Width = 650
        object tlMain: TcxTreeList
          Left = 3
          Top = 3
          Width = 644
          Height = 348
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
          OnFocusedNodeChanged = tlMainFocusedNodeChanged
          object Column1: TcxTreeListColumn
            Caption.Text = #36890#25773#32452
            DataBinding.ValueType = 'LargeInt'
            Width = 100
            Position.ColIndex = 0
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object Column2: TcxTreeListColumn
            Caption.Text = #36890#25773#32452#21517#31216
            DataBinding.ValueType = 'String'
            Width = 188
            Position.ColIndex = 1
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn4: TcxTreeListColumn
            Caption.Text = #30446#26631#25104#21592#25968
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 2
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn5: TcxTreeListColumn
            Caption.Text = #23454#38469#25104#21592#25968
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 3
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object Column4: TcxTreeListColumn
            Visible = False
            Caption.Text = #29366#24577
            DataBinding.ValueType = 'String'
            Width = 112
            Position.ColIndex = 4
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
        end
      end
    end
  end
end
