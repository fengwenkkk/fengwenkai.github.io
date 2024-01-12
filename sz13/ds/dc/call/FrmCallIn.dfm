inherited FormCallIn: TFormCallIn
  Caption = #21628#20837
  ClientHeight = 635
  ClientWidth = 587
  OnCreate = FormCreate
  OnShow = FormShow
  ExplicitWidth = 603
  ExplicitHeight = 674
  PixelsPerInch = 96
  TextHeight = 14
  object MediaPlayer: TMediaPlayer [0]
    Left = 365
    Top = 104
    Width = 253
    Height = 30
    DoubleBuffered = True
    Visible = False
    ParentDoubleBuffered = False
    TabOrder = 1
  end
  object WindowsMediaPlayer: TWindowsMediaPlayer [1]
    Left = 188
    Top = 65
    Width = 245
    Height = 240
    TabOrder = 2
    Visible = False
    ControlData = {
      000300000800000000000500000000000000F03F030000000000050000000000
      0000000008000200000000000300010000000B00FFFF0300000000000B00FFFF
      08000200000000000300320000000B00000008000A000000660075006C006C00
      00000B0000000B0000000B00FFFF0B00FFFF0B00000008000200000000000800
      020000000000080002000000000008000200000000000B00000052190000CE18
      0000}
  end
  inherited MainPanel: TPanel
    Width = 587
    Height = 635
    OnResize = MainPanelResize
    ExplicitWidth = 587
    ExplicitHeight = 635
    object SplitterTop: TAdvSplitter
      Left = 0
      Top = 240
      Width = 587
      Height = 8
      Cursor = crVSplit
      Align = alTop
      Appearance.BorderColor = clNone
      Appearance.BorderColorHot = clNone
      Appearance.Color = clBackground
      Appearance.ColorTo = clBackground
      Appearance.ColorHot = clBackground
      Appearance.ColorHotTo = clBackground
      GripStyle = sgNone
      ExplicitTop = 206
      ExplicitWidth = 638
    end
    object PanelMain: TPanel
      Left = 0
      Top = 248
      Width = 587
      Height = 387
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      object sbStation: TcxScrollBox
        Left = 0
        Top = 0
        Width = 587
        Height = 387
        Align = alClient
        TabOrder = 0
        object gbCallInfo: TcxGroupBox
          Left = 0
          Top = 0
          Margins.Left = 0
          Margins.Top = 0
          Margins.Right = 0
          Margins.Bottom = 0
          Align = alClient
          Alignment = alTopCenter
          Caption = #21628#21483#35760#24405
          ParentFont = False
          Style.BorderStyle = ebs3D
          Style.Edges = []
          Style.Font.Charset = ANSI_CHARSET
          Style.Font.Color = clWindowText
          Style.Font.Height = -24
          Style.Font.Name = 'Arial'
          Style.Font.Style = []
          Style.LookAndFeel.NativeStyle = False
          Style.LookAndFeel.SkinName = 'Office2010Blue'
          Style.IsFontAssigned = True
          StyleDisabled.LookAndFeel.NativeStyle = False
          StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
          TabOrder = 0
          Height = 385
          Width = 585
          object tlCallInfo: TcxTreeList
            AlignWithMargins = True
            Left = 1
            Top = 27
            Width = 583
            Height = 297
            Margins.Left = 0
            Margins.Top = 0
            Margins.Right = 0
            Margins.Bottom = 0
            BorderStyle = cxcbsNone
            Align = alClient
            Bands = <
              item
              end>
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -14
            Font.Name = #23435#20307
            Font.Style = []
            LookAndFeel.NativeStyle = False
            LookAndFeel.SkinName = 'Office2010Blue'
            Navigator.Buttons.CustomButtons = <>
            OptionsBehavior.MultiSort = False
            OptionsData.Editing = False
            OptionsData.AnsiSort = True
            OptionsView.GridLines = tlglBoth
            OptionsView.Indicator = True
            OptionsView.ShowRoot = False
            ParentFont = False
            PopupMenu = pmCallInfo
            TabOrder = 0
            OnCustomDrawDataCell = tlCallInfoCustomDrawDataCell
            OnDblClick = tlCallInfoDblClick
            object cxTreeListColumn35: TcxTreeListColumn
              Caption.Text = #26102#38388
              DataBinding.ValueType = 'Integer'
              Width = 100
              Position.ColIndex = 0
              Position.RowIndex = 0
              Position.BandIndex = 0
              SortOrder = soDescending
              SortIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = cxTreeListColumn10GetDisplayText
            end
            object cxTreeListColumn36: TcxTreeListColumn
              Visible = False
              Caption.AlignVert = vaTop
              Caption.Text = #21628#21483#31867#22411
              DataBinding.ValueType = 'Integer'
              Width = 104
              Position.ColIndex = 1
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
            end
            object tlCallInfoColumn5: TcxTreeListColumn
              Caption.Text = #32676#32452
              DataBinding.ValueType = 'LargeInt'
              Width = 209
              Position.ColIndex = 2
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = tlCallInfoColumn5GetDisplayText
            end
            object ColumnCurrSpeaker: TcxTreeListColumn
              Caption.Text = #35805#26435#20154
              DataBinding.ValueType = 'LargeInt'
              Width = 127
              Position.ColIndex = 3
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = ColumnCurrSpeakerGetDisplayText
            end
            object cxTreeListColumn39: TcxTreeListColumn
              Caption.Text = #29366#24577
              DataBinding.ValueType = 'Integer'
              Width = 78
              Position.ColIndex = 4
              Position.RowIndex = 0
              Position.BandIndex = 0
              Summary.FooterSummaryItems = <>
              Summary.GroupFooterSummaryItems = <>
              OnGetDisplayText = cxTreeListColumn13GetDisplayText
            end
          end
          object Panel4: TPanel
            Left = 1
            Top = 324
            Width = 583
            Height = 56
            Align = alBottom
            BevelOuter = bvNone
            TabOrder = 1
            Visible = False
            object dxBevel4: TdxBevel
              Left = 0
              Top = 0
              Width = 583
              Height = 3
              Align = alTop
              LookAndFeel.NativeStyle = False
              Shape = dxbsLineTop
              ExplicitLeft = -1
              ExplicitWidth = 650
            end
            object btnJoinGroup: TcxButton
              Left = 50
              Top = 15
              Width = 90
              Height = 30
              Caption = #36890#35805
              LookAndFeel.SkinName = 'Office2010Blue'
              OptionsImage.Glyph.SourceDPI = 96
              OptionsImage.Glyph.Data = {
                89504E470D0A1A0A0000000D4948445200000016000000160806000001B3B35C
                AD000000017352474200AECE1CE900000044655849664D4D002A000000080001
                87690004000000010000001A000000000003A00100030000000100010000A002
                00040000000100000016A00300040000000100000016000000001C21335A0000
                02D3494441543811CD554F481461147FEF33DB2CDD4A096637FC134411417468
                660F29D5A12EDE3A0811D141C2DC20B02E51878228EA605D62C74D4228A143E7
                2E0575A82057BA04061A9251B91245B8A9359AF37A6FC66F66675D69BDF51DE6
                7BEFF77EDF6FBFEF37EF9B05E091B0AD6B3247064A66D8E61010585E25619B6F
                22141FCD58171376EAEEF24A11E289F1FA8F44D024F8547AD8C3944FC2979A6C
                D8D61F1D7B732263FED8796F7F5D04FC6782869D1A5040E8129DD46CFF481993
                14E00517E886EC626907000244983A617BEE13D1099DCBACF75F8C551EB3627F
                32DBEA39516E9591B1DE0BBE461EF50DCD67BE7F9B709AB36D49879C1E3ED259
                4218ABAD53AD33051A45A085803CD2F1689E1314C79C79DC424031248CCD15A8
                632A9D4B08518667851F026C1F4CC5670BF49A001A1542EF6477EE8AAEC91C58
                24C94CC19D66D55D0054CBE65E36FACC3B82FF8F43FB586E6FE2BFBC07A9793E
                F381B64B627083C8AC8742B4C975DA09D433C13C3710F133ABE7D7D56CD88C08
                FC12C061576F56ABD855E9F18686A62E21073E6B5544D58F44E32ED251B95908
                78299FCE5D0F9425F0FB124F01D101BF2DD5BB8DB1B5714D144EA86C9BCC0973
                29CA60C2603E3DECB56D40F64BE1B365E0E026E7D7EC133EB119A261C4DB1B8B
                C7AAF78D76BEFA19A261B4E45C08E8C8F93D37CEA2F53A2F9DB985764CCF2FBC
                657C5B694DF248CF1513F826AE281AF0885A82B8245851B884B7EA746561C4AF
                AB562B5A10782C0DC75D6AF04BB9C5AD709EFB0823AD5AB428081127244E64AC
                5EF6FC1CF7C594FE10843B56D829242124B3A923B11AD8C3C40F82951D08E3EB
                557CAF707D51662D69083FD26EFCCB6DFCBD7ACECD5CC5A511A5B067F2F4D0D3
                72C2C9BED461D7A5DBBC95DD80B8C8C73BC4277DA1B911610D6ECD5AEDEE22D9
                6C45A3C6CACDBCF893AAC2EE2F5DB9C7E5EA1561728FF55DAE64C1B21DF3FFD9
                03423CE6DB5181047FDFF9853FCC77E78E17B3FF02EBDCF923E17920B0000000
                0049454E44AE426082}
              TabOrder = 0
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -15
              Font.Name = 'Arial'
              Font.Style = []
              ParentFont = False
              OnClick = btnJoinGroupClick
            end
            object btnExitGroup: TcxButton
              Left = 180
              Top = 15
              Width = 90
              Height = 30
              Caption = #36864#20986
              LookAndFeel.SkinName = 'Office2010Blue'
              OptionsImage.Glyph.SourceDPI = 96
              OptionsImage.Glyph.Data = {
                89504E470D0A1A0A0000000D4948445200000016000000160806000001B3B35C
                AD000000017352474200AECE1CE900000044655849664D4D002A000000080001
                87690004000000010000001A000000000003A00100030000000100010000A002
                00040000000100000016A00300040000000100000016000000001C21335A0000
                02A2494441543811CD554B689341109ED93F89053D78D35A29A2B58828A90916
                BCF4241AC126BEE25110AA05853639285E7CA007F114E2ABF8C093277B489356
                058FEA4DAD097A8A0ABE12AFD25392E6DF756675E2DF24C514115CD87F2633DF
                CECE7303E059C87C70386598FAF883A84649AA98FFBDC2F174AFC0945B8588A8
                16186061613A6965F6138ADEE8734DED9DA0032AD02B7C8774209ABA22370B45
                61C406227E6FF218C018B3D20A3924714BA89CFC331D184E7D8DC71F385EA435
                DBEC0403288F3DC54AA91E8CA65C39D04891CF71FAEAAEFB5E146DE86C238A57
                53631F10E1B66C2FD851CE100512B660C7079B2949B70490CF254799A79CDCE1
                6867B363CF44F7777470FFF57EBAE97EB315A4F4048AD5F2674AF92A5692CF0F
                11D48C367A02102E1572C9737248152BE54FA0D411F68D37F9BB9712B7DC020C
                9C0D455307046C0BCDA076B91690DFA7B6BECC8CBF6DA44E14EDE87C5DBF6179
                4760AA5C89C176309841A54E81D11B99A75A0F50B9072D0F58C94F27D632EFAC
                DEB427D6DD1F19227891F637BA2A4301BBD48F3B1940F1F899F252855C220468
                68427498B746DC62C0CCB1920367FA7FAD25B9148CA58F82D6F738043A78373F
                9D3CB6583836CB3CEEC6C0E9C54064E40919D9AD34ACD0BF405485116A901185
                70E2752E39D17CD67A2C1D2449A58B2E10B0BB19BCB0AC5E2D561CA57679C7A9
                D11702DB7EF0E6FA5AAD7A5E7E77464D97ABDDA7F4CE94FC2AB0E345F6E49796
                EE74E74D8BAC33E33FDFA49AAE4D31BEC5888350EDD49017C72F2E0F1EA593FA
                D61697FE48F8D533A0D6AC73BA1E5F1B5BD4F0B658FA8CD6FAB2D720B587A6F9
                3A54C88E67BC729B63048C183433E58F6E850AE9D55B9EBC29E773899E1605E0
                459AB0B6F558521F87F75DDD5077F5737ADA1EF52FEB3E3E3979B8F1BEB75EFA
                8F243F009A66F35A7EB494E00000000049454E44AE426082}
              SpeedButtonOptions.CanBeFocused = False
              TabOrder = 1
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -15
              Font.Name = 'Arial'
              Font.Style = []
              ParentFont = False
              OnClick = btnExitGroupClick
            end
          end
        end
      end
    end
    object sbMain: TcxScrollBox
      Left = 0
      Top = 0
      Width = 587
      Height = 240
      Align = alTop
      TabOrder = 1
      object gbMain: TcxGroupBox
        Left = 0
        Top = 0
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alClient
        Alignment = alTopCenter
        Caption = #21628#21483#35831#27714
        ParentFont = False
        Style.BorderStyle = ebs3D
        Style.Edges = []
        Style.Font.Charset = ANSI_CHARSET
        Style.Font.Color = clWindowText
        Style.Font.Height = -24
        Style.Font.Name = 'Arial'
        Style.Font.Style = []
        Style.LookAndFeel.NativeStyle = False
        Style.LookAndFeel.SkinName = 'Office2010Blue'
        Style.IsFontAssigned = True
        StyleDisabled.LookAndFeel.NativeStyle = False
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 0
        Height = 238
        Width = 585
        object tlCallIn: TcxTreeList
          AlignWithMargins = True
          Left = 1
          Top = 27
          Width = 583
          Height = 150
          Margins.Left = 0
          Margins.Top = 0
          Margins.Right = 0
          Margins.Bottom = 0
          BorderStyle = cxcbsNone
          Align = alClient
          Bands = <
            item
            end>
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -14
          Font.Name = #23435#20307
          Font.Style = []
          LookAndFeel.NativeStyle = False
          LookAndFeel.SkinName = 'Office2010Blue'
          Navigator.Buttons.CustomButtons = <>
          OptionsBehavior.MultiSort = False
          OptionsBehavior.Sorting = False
          OptionsData.Editing = False
          OptionsData.AnsiSort = True
          OptionsView.GridLines = tlglBoth
          OptionsView.Indicator = True
          OptionsView.ShowRoot = False
          ParentFont = False
          PopupMenu = pmMain
          TabOrder = 0
          OnCustomDrawDataCell = tlCallInCustomDrawDataCell
          OnDataChanged = tlCallInDataChanged
          OnFocusedNodeChanged = tlCallInFocusedNodeChanged
          object cxTreeListColumn10: TcxTreeListColumn
            Caption.AlignVert = vaTop
            Caption.Text = #26102#38388
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 0
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = cxTreeListColumn10GetDisplayText
          end
          object tlCallInColumnCallType: TcxTreeListColumn
            Caption.AlignVert = vaTop
            Caption.Text = #21628#21483#31867#22411
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 1
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = tlCallInColumnCallTypeGetDisplayText
          end
          object ColumnCaller: TcxTreeListColumn
            Caption.AlignVert = vaTop
            Caption.Text = #21628#21483#26041
            DataBinding.ValueType = 'LargeInt'
            Width = 100
            Position.ColIndex = 2
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnCallerGetDisplayText
          end
          object ColumnTrainOrStation: TcxTreeListColumn
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = #21015#36710'/'#36710#31449
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 3
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnTrainOrStationGetDisplayText
          end
          object cxTreeListColumn13: TcxTreeListColumn
            Caption.AlignVert = vaTop
            Caption.Text = #29366#24577
            DataBinding.ValueType = 'Integer'
            Width = 78
            Position.ColIndex = 4
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = cxTreeListColumn13GetDisplayText
          end
          object tlMainColumn6: TcxTreeListColumn
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = 'ReqCallType'
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 5
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn7: TcxTreeListColumn
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = 'SortOrder'
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 6
            Position.RowIndex = 0
            Position.BandIndex = 0
            SortOrder = soDescending
            SortIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn8: TcxTreeListColumn
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = 'VideoFormat'
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 7
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn9: TcxTreeListColumn
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = 'CameraType'
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 8
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnAccept: TcxTreeListColumn
            PropertiesClassName = 'TcxImageComboBoxProperties'
            Properties.Images = ImageListCall
            Properties.Items = <
              item
                Description = #25509#21548
                ImageIndex = 0
                Value = 0
              end>
            Properties.ShowDescriptions = False
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = #25509#21548
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 9
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnClose: TcxTreeListColumn
            PropertiesClassName = 'TcxImageComboBoxProperties'
            Properties.Images = ImageListCall
            Properties.Items = <
              item
                Description = #25346#26029
                ImageIndex = 1
                Value = 0
              end>
            Properties.ShowDescriptions = False
            Visible = False
            Caption.AlignVert = vaTop
            Caption.Text = #25346#26029
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 10
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnDCUser: TcxTreeListColumn
            Caption.AlignVert = vaTop
            Caption.Text = #22788#29702#20154
            DataBinding.ValueType = 'Integer'
            Width = 65
            Position.ColIndex = 11
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
            OnGetDisplayText = ColumnDCUserGetDisplayText
          end
          object tlMainColumn13: TcxTreeListColumn
            Visible = False
            DataBinding.ValueType = 'LargeInt'
            Width = 100
            Position.ColIndex = 12
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlMainColumn14: TcxTreeListColumn
            Visible = False
            Caption.Text = 'PrevCallStatus'
            DataBinding.ValueType = 'String'
            Width = 100
            Position.ColIndex = 13
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnUEType: TcxTreeListColumn
            Visible = False
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 14
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object ColumnReqTime: TcxTreeListColumn
            Visible = False
            DataBinding.ValueType = 'Integer'
            Width = 100
            Position.ColIndex = 15
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
          object tlCallInColumn17: TcxTreeListColumn
            Caption.Text = #20301#32622#20449#24687
            DataBinding.ValueType = 'String'
            Width = 240
            Position.ColIndex = 16
            Position.RowIndex = 0
            Position.BandIndex = 0
            Summary.FooterSummaryItems = <>
            Summary.GroupFooterSummaryItems = <>
          end
        end
        object Panel1: TPanel
          Left = 1
          Top = 177
          Width = 583
          Height = 56
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 1
          object dxBevel1: TdxBevel
            Left = 0
            Top = 0
            Width = 583
            Height = 3
            Align = alTop
            LookAndFeel.NativeStyle = False
            Shape = dxbsLineTop
            ExplicitTop = 8
            ExplicitWidth = 621
          end
          object btnCall: TcxButton
            Left = 546
            Top = 15
            Width = 80
            Height = 60
            Caption = #36890#35805
            LookAndFeel.SkinName = 'Office2010Blue'
            OptionsImage.Glyph.SourceDPI = 96
            OptionsImage.Glyph.Data = {
              89504E470D0A1A0A0000000D4948445200000016000000160806000001B3B35C
              AD000000017352474200AECE1CE900000044655849664D4D002A000000080001
              87690004000000010000001A000000000003A00100030000000100010000A002
              00040000000100000016A00300040000000100000016000000001C21335A0000
              02D3494441543811CD554F481461147FEF33DB2CDD4A096637FC134411417468
              660F29D5A12EDE3A0811D141C2DC20B02E51878228EA605D62C74D4228A143E7
              2E0575A82057BA04061A9251B91245B8A9359AF37A6FC66F66675D69BDF51DE6
              7BEFF77EDF6FBFEF37EF9B05E091B0AD6B3247064A66D8E61010585E25619B6F
              22141FCD58171376EAEEF24A11E289F1FA8F44D024F8547AD8C3944FC2979A6C
              D8D61F1D7B732263FED8796F7F5D04FC6782869D1A5040E8129DD46CFF481993
              14E00517E886EC626907000244983A617BEE13D1099DCBACF75F8C551EB3627F
              32DBEA39516E9591B1DE0BBE461EF50DCD67BE7F9B709AB36D49879C1E3ED259
              4218ABAD53AD33051A45A085803CD2F1689E1314C79C79DC424031248CCD15A8
              632A9D4B08518667851F026C1F4CC5670BF49A001A1542EF6477EE8AAEC91C58
              24C94CC19D66D55D0054CBE65E36FACC3B82FF8F43FB586E6FE2BFBC07A9793E
              F381B64B627083C8AC8742B4C975DA09D433C13C3710F133ABE7D7D56CD88C08
              FC12C061576F56ABD855E9F18686A62E21073E6B5544D58F44E32ED251B95908
              78299FCE5D0F9425F0FB124F01D101BF2DD5BB8DB1B5714D144EA86C9BCC0973
              29CA60C2603E3DECB56D40F64BE1B365E0E026E7D7EC133EB119A261C4DB1B8B
              C7AAF78D76BEFA19A261B4E45C08E8C8F93D37CEA2F53A2F9DB985764CCF2FBC
              657C5B694DF248CF1513F826AE281AF0885A82B8245851B884B7EA746561C4AF
              AB562B5A10782C0DC75D6AF04BB9C5AD709EFB0823AD5AB428081127244E64AC
              5EF6FC1CF7C594FE10843B56D829242124B3A923B11AD8C3C40F82951D08E3EB
              557CAF707D51662D69083FD26EFCCB6DFCBD7ACECD5CC5A511A5B067F2F4D0D3
              72C2C9BED461D7A5DBBC95DD80B8C8C73BC4277DA1B911610D6ECD5AEDEE22D9
              6C45A3C6CACDBCF893AAC2EE2F5DB9C7E5EA1561728FF55DAE64C1B21DF3FFD9
              03423CE6DB5181047FDFF9853FCC77E78E17B3FF02EBDCF923E17920B0000000
              0049454E44AE426082}
            OptionsImage.Layout = blGlyphTop
            SpeedButtonOptions.CanBeFocused = False
            TabOrder = 1
            Visible = False
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'Arial'
            Font.Style = []
            ParentFont = False
            OnClick = btnCallClick
          end
          object btnHangup: TcxButton
            Left = 180
            Top = 15
            Width = 90
            Height = 30
            Caption = #25346#26029
            LookAndFeel.SkinName = 'Office2010Blue'
            OptionsImage.Glyph.SourceDPI = 96
            OptionsImage.Glyph.Data = {
              89504E470D0A1A0A0000000D4948445200000016000000160806000000C4B46C
              3B000000097048597300000EC400000EC401952B0E1B0000019249444154388D
              B5D5BF6BD3411CC6F1D7A509F49738283A3808C23749171D623707DD44715017
              377115FF004510175D1C9D8B525C1CBBE820EAA0994444C1A1594454285A08A5
              55A9D67C1C349AE65743421FB8E5F9DCBDEFF9DCC11DDBA404517258C335C931
              6152521796244B788D070A9EA5B77E0C0C8EA2593C11A6B788B086FB921B69D1
              BB41C00F85E3832691FCC45D53AEA657BEF4032F0BBB0606FFDFE013CEA59AE7
              DDCA39AC6E01D8E8EA877D781A45977A81EFF440AE4B4EA49A8209D3F22A92EB
              586981E785DB5172A173E3A3F291B9179968192B5176B66BD083F644662E328D
              96F91B51766A73A3CD054587302B7C953C4A35CB3D3A69CEBF22DC6C21D58D2B
              A7373EF75B37903A3A2D9AEF483C14B862D2AAF7C2EE7F66CE91B4A89A1B059C
              5EFA86B94D66C3C53FFC5135E6719B733A2A768E0E6ED8DBE64C5873666C1466
              644E0AF328B4953E0E0D8E19FB852AC6BB94BF0F7F14BF9C171D499B9A191E9C
              FA3C5CC9D4289757ED59092F86071FB020757D32D77179682E44C98E28B91599
              0F91A94766E1EF8FB47DFA0DC9C187019F6906860000000049454E44AE426082}
            SpeedButtonOptions.CanBeFocused = False
            TabOrder = 2
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'Arial'
            Font.Style = []
            ParentFont = False
            OnClick = btnHangupClick
          end
          object btnAccept: TcxButton
            Left = 50
            Top = 15
            Width = 90
            Height = 30
            Caption = #25509#21548
            LookAndFeel.SkinName = 'Office2010Blue'
            OptionsImage.Glyph.SourceDPI = 96
            OptionsImage.Glyph.Data = {
              89504E470D0A1A0A0000000D4948445200000016000000160806000001B3B35C
              AD000000017352474200AECE1CE900000044655849664D4D002A000000080001
              87690004000000010000001A000000000003A00100030000000100010000A002
              00040000000100000016A00300040000000100000016000000001C21335A0000
              0290494441543811AD55CF6B1351109E79495A6D6A4588B2D90A2A5E5AF59AA4
              142B4529FEC08A20D29382546952FC0BAC20F452F4A007213F2AE4A2965A0541
              298A14BC68B1E9494F1654CC21A488168A696C6D76C779B16FBB9B646923EE65
              DE7CDF375FDEDB99B701B03F7A327249E528175A3C440A588F76D42963A66067
              D76B5CCCCAE54AD59A0C1D334D7C04024E28CC19A577957F15C0350211EF0713
              E107F6FAB5BD865701C8AB08C70EB444780A0822B83BD5112919C63BA942C037
              F9C14C97AAA88CC2308C9B0A24A4885ABBC69AC7A8A1B6F6C6FB2A01910710D3
              F3B14CBFD26AF1F0221FA6854FFBD0122B3298888C12995754FECF1183F1D032
              CF41A37468DE1BD8F2E9D48B153737A18452B0945DB8E52694B870903ECF6D47
              5E91586214E242FEF274B68277A47200C62442261D72306E495D4DD1931DBDA6
              693CE3E1203FF8B4CFB1E96FD298DFF9397EE74FE4BAADDDEFB39AA227C2574D
              A2BB92F8FB60C93E86DCD9AF96580AB474D74E5859CEF138FAD62AACC013F9D2
              2156CCC189F30D0BDFB323DC83E3ECDECEF8E4FCE0EC99B2581BED0C81B17A1D
              098EF0CF1590687C5B63C3F05CFFDB9FCAA0DE885A22F49AB7D85DABD083D097
              8BCD4ED4E236C2B8D978C04D64A06873E336C205203DAE142142D1E3C1D3F3D1
              99E14A6EB3B968C2ED43DC995FF602447136379099B463F5AEC59781A94536EA
              B117F2908DE9A9DE263B56EFDA1AA1FD89CE5D452A7D24A01DCA842FD9B57C74
              6644E5B5A29EEA0E00157BC0043F0A98CD4533EFA5CE325645F28FC534CDB4CA
              CB1181BF5D784708F13CB0B565EEC3C5574B5ABCE32482F1D4FE0DB16A84B851
              65AC483D1539CC377D9C885A15B6D9C83DFBE16A6C37D9732FBCEFF72A0CF1EE
              FAF87635DBB9EA3516C0EB3D5A8DFF27E40FF570D1882025960E000000004945
              4E44AE426082}
            TabOrder = 0
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'Arial'
            Font.Style = []
            ParentFont = False
            OnClick = btnAcceptClick
          end
          object btnConfirm: TcxButton
            Left = 310
            Top = 15
            Width = 90
            Height = 30
            Caption = #30830#35748
            LookAndFeel.SkinName = 'Office2010Blue'
            OptionsImage.Glyph.SourceDPI = 96
            OptionsImage.Glyph.Data = {
              89504E470D0A1A0A0000000D494844520000001800000018080600000197700D
              6E000000017352474200AECE1CE900000044655849664D4D002A000000080001
              87690004000000010000001A000000000003A00100030000000100010000A002
              00040000000100000018A0030004000000010000001800000000E23575AD0000
              029E49444154480DE5544D681351109ED94D1A430F22826E922A0A3D587BF1D0
              6C22C4837A901E0583782A414A4DAEE2C583F4D0A3E76CDA80E8C543F0073C94
              1E4403A2262922880AE2C1086D5225454113B5D9EC73DE0B6F93DD240DD28BE2
              1E76DE7CF3CDCC7B6F661E40BF4F4B8799C03523BCD58FD0C682867E133443FF
              64F3FB716DA30C46B21848EB0D0799C23C73008314741B42B7F4BDAD3A186E1C
              3A89E571886283B416A124706462D49B3F99371D61B44CE49C03F853A567B7EE
              008C310C18BAC57144A59D6D32171F7113B91E4887BFF0FDD2EDBD95F6F621A8
              08C0D83E3A527323B93A123422D72D665DE6A48DD4AA63170E85574F381251F5
              2A07D6678B6B32F2BF2EB5B4FE5E5657197618AAC16D00362E79DB3A04339104
              15EE0227CBEB150EF15C5C9511A4A41E3A6A59D60DAEEFF28FEE91B842FBAB3E
              A9954D4A7D568263B9E37EB0AC375CF7A86AB49CC87F953605143CCD154A7D4F
              3A9935B33D488A72656DAE5094642E45A50F2E8527B74C782D0C882DF2561170
              A59A2A4D77936D07BE70380154E8902137B9470F64A313D44F1F7A0C7F35E018
              879DEE34948D8C31935D65C87E297E5C589F296D7A761A94FB8F2F4FFBEAE5CD
              87ADA61513F1E87DB6EA7082D653DBB6AA200FF9512F2D7C2FD77E3260EDE09C
              8F708D5A64AABD1C1260903964E8A7A8E1568081B7C3C1C721F5D89917734B4D
              89891AD038CC204286C00632255149151E48825B1ECE46F6FF3059819AF9906D
              43FCECF340F4E36CA9A70545027A3D4B746DE18E03346912CE5793A5FB129B67
              F3CA6266F90E8D983D9374150C41895793C5BB92E796761789C969E123F9A80A
              22BDCA3C11DDAF465791EE7646C445DAC0A56EACDFDA4E208D7D1349A390F86A
              B7CF1B7B77F1E937073C40E9492079EE4454A306822756493E7F2939FF87FC0D
              4A63F3BD176507AB0000000049454E44AE426082}
            SpeedButtonOptions.CanBeFocused = False
            TabOrder = 3
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'Arial'
            Font.Style = []
            ParentFont = False
            OnClick = btnConfirmClick
          end
        end
      end
    end
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
    Left = 536
    Top = 64
    PixelsPerInch = 96
    object dxBarSeparator1: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
    object pmiHangup: TdxBarButton
      Caption = #25346#26029
      Category = 0
      Hint = #25346#26029
      Visible = ivAlways
      DropDownEnabled = False
      OnClick = pmiHangupClick
    end
    object pmiConfirm: TdxBarButton
      Caption = #30830#35748
      Category = 0
      Hint = #30830#35748
      Visible = ivAlways
      OnClick = pmiConfirmClick
    end
    object pmiCallTransfer: TdxBarButton
      Caption = #36716#25509
      Category = 0
      Hint = #36716#25509
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
        61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
        00097048597300000EC300000EC301C76FA8640000012149444154384F63A01D
        F8FF9FD1A769C71CDFA61DFFA1F87140F33665A82C9100CD109FC6ED8F7C9B77
        2A4265890420431AB7CF85BBA471C703AFE62DF2505922019A2140F63D9FFADD
        72505904F0ADDFA1E2D3B8A30968CB61207E05B7150B06AABBE351BF4D06ACD1
        AF73032F50C36CA07FFF62538C13032D62F0A9DFC4E5DBB8FD2456054DDB7F03
        6DFA835D6EC773AFA65D9A0C4045B5C8827E4D3B92E04E430260AFA16B0601A0
        EDB76012DECDDB1DC0826800A8A60159B34FFD0E0DA8145012E84C9004C8FFA1
        AB56314385E100285E0FD30C74C53314CD200034E02D4C8177FB1641A83018F8
        36EFC80506D43B1006853A86661000C6E941980120FF438589073E0D3BC2C106
        346EBFE6DBBA471C2A4C0200A732600893A59962C0C00000AFC109A0ABBB1891
        0000000049454E44AE426082}
      OnClick = pmiCallTransferClick
    end
    object dxBarSeparator2: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivNotInCustomizing
      ShowCaption = False
    end
    object pmiAccept: TdxBarButton
      Caption = #25509#21548
      Category = 0
      Hint = #25509#21548
      Visible = ivAlways
      OnClick = pmiAcceptClick
    end
    object pmiCall: TdxBarButton
      Caption = #36890#35805
      Category = 0
      Hint = #36890#35805
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
        61000000097048597300000EC400000EC401952B0E1B0000023449444154388D
        8593BD6B545110C5CFCCBD6FDFD34D882EAC9AB08DC1CA22047531D8B9012DC4
        2682E42FF03B8D160A360163A9A0F891A0E09F202216295C41A3B806643160A5
        5899102382B85F79EFDE198B17757713CC85DB9C7BEE8F33CC0C6183333073E0
        902A4D89C741222898E688686AE97465AEDB4BDDC28EE9E2A8F17842BD415663
        9F9A3206BE96B4D8D291A553EF5EB5FB6D37C028ED4788CFBE16C7D03531F6A0
        8CB1E26418C0FF01AAB26002FB08B174E60B0888F553B77F1D40944306F56A57
        750C82570E3705B020506846553B74858289CCA600585F57CF8EDA0308400A80
        443604F4DF2B1E55A263D6F264124BD358FAEE3BF34301F2905AE1C148CE3999
        24D5674BE7E6676D1A0F45DE6A275CC3BD64C62E8DE53211C5903640A201C0D7
        34916D9CB513524FBE01986500F06ACBD27442A2E314C86B5995887B8241CE9A
        F4F60483487C109ACC1B818E4BD389575B5E63032BF942051E658ACC094D68AF
        1A53927AFC501AFE8334A4AABF9219843CEA7C3244911983D7F24ABE5001DA3A
        3D70A7382C166530F7C0E97532F49CBC5F0100159357D65158BA0A911A3B9416
        2FCC573B0000B0F3EEBEC36CED4D8ACCB0AFBB16802F6B4FBB4DD646DAF25571
        EEE2F2F9F72FFEFC59B70B0030303D725CD4975470160088719FC99417CFBC7D
        DAEDFD07B8B527EC37B93184949755272026102E216DD30DA8288796C9FBE5C5
        5CE3314E7E8CFFCE01006C8FFA22885EE1DECC108569E3A5994E036F31B741E9
        56CA0F59E8FB1ACCFE046200F80D9176FC8A454740770000000049454E44AE42
        6082}
      OnClick = btnJoinGroupClick
    end
    object pmiExitGroup: TdxBarButton
      Caption = #36864#20986#32676#32452
      Category = 0
      Hint = #36864#20986#32676#32452
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
        61000000097048597300000EC400000EC401952B0E1B0000022849444154388D
        A593316854411086FF99DDB77731A68820A2161656A6883167A7016B23210816
        16161662216872208869121291201E3120183BB112BBE390148110C452BDA4B2
        B0112C02113D0CDEBBF77677C622E64E0372883F6CB1ECECC7CCFFEF02FF29EA
        56303CFEA814A38C586D3C7D5B9B69FE157072F4E151E5C4EDEE23E7A178A86F
        336C36A713D777D7A78DC5F5D3DB93989991DF01B6545A4AE4C88F49213B6D6D
        A1473582D8027968C4CDD61928BC4804D9C2CDC177FC650398FD03901DDE3E6E
        C9CD12D38B90355700019B22983875393E67097A550200C01837353856D9DAA8
        969FEC021828F41BB7CF4996CDC1856563ED0941360048294B641EAAA32A012A
        1E000A0453191A5BB8D4EE80485455C06C55838C9BE2FE3BF069DB2095085501
        4090E8C1C6F540E5F1D058E55BBD5A5EE11D8EC2101BC040628044DF5E3B973B
        929803640EAAE0F9A9F30F8ED9F6814402778F75AFEC6E9AC142580036162A49
        BBA033C22FD38C03346E11E3CAFBDAED4F5655888820EC43817A5F059F2E20FA
        04A40A9053D1B3C4664035828D834848097AA35E2DAF00008B4A84089268CEB5
        C4E779DE9C5729DE77B999AA5727AF13D12AB105710200992296EBD58997ED11
        F2683FD8BCF9C69842C5889F302651456632A35F4B1717AF852029B381468104
        3FB751EBBC0100E08FCBB7BE3B132F0BC91C485F13E91A11D618584D4DAB0168
        4E6C81E817D66B13F7F69AD8FD335DA80C7BC6C881FEFEA5B567575BFF9A5257
        FD04F29014C64F7F14550000000049454E44AE426082}
      OnClick = btnExitGroupClick
    end
    object dxBarSeparator3: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
    object btnListen: TdxBarButton
      Caption = #37325#28857#30417#21548
      Category = 0
      Hint = #37325#28857#30417#21548
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000010000000100803000000282D0F
        530000000467414D410000B18F0BFC6105000000206348524D00007A26000080
        840000FA00000080E8000075300000EA6000003A98000017709CBA513C000001
        DA504C5445000000C6D77EC5D67CC7D87EC2D378C5D57CC4D57CC5D57DC0D279
        C3D47BBCCD74C0D278C0D177B0C45CC4D479BFD078C5D67DBFD079BFD06DB9CA
        71BDCE77BFD077C2D37AC0D078B1C55EABC057ABBC64B5C66EB5C56FAFC068AD
        BE679DB2499FB34BA5B850A6BB529CB048A7B762AEBF69AEBF679EB24A9EB34A
        94A941A9BA63AABB649AAF4698AD44A6B761A0B357A1B25C8FA43D8EA33B9DAF
        58899E368A9E3799AB55859A33869B349AAC5699AA539BAD5784993280952E87
        9C3591A448C3CF9A849933839831899E37C7D87EC7D87FC5D67CC5D67DC6D67E
        C6D77FC1D279C2D37BC4D47CC5D57DC5D67DC3D47DBECF77BFD078C0D179C0D2
        79BCCD72B8C971BACB73BBCC74B3C664AFC25CA9BE53B5C66EB6C76FB5C76FA7
        BB51A7BB52A7BB52B0C169B0C16AAFC169AFC069ACBD659CB1489FB44B9FB44B
        A0B54CA0B54CA9BA63AABC65ABBC66AABB649AAF469AAF4799AE4698AC44A5B7
        60A4B65EA0B25694A94194A94194A840A1B25C94A8468CA2388EA33B8EA33B8E
        A33B9DAF589DAE59899E36899E36899E378A9F3799AB569AAB558EA142859A33
        859A33869B34859A33869B34859A347F942A839831839831829730ABBC669AAF
        47A6B76194A941A1B25CA1B35D9EB0578EA33B9AAC548EA240899E36FFFFFFFD
        0403620000009274524E53000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        00000000000000000000000000000009210653ABC1067BC4692B0651C5333351
        09AB6B69A80825C12C2ABE2444DAB76F02026DB5D84219BAC70E0DC6B81864C7
        0F0EC66377C50E0EC5763CEAC70EE93B0255B8F3AB08B753020A3A200AA2574F
        BA00000001624B47449D06BBF2B10000000774494D4507E403110D260EC2BA84
        6E000000F64944415418D363600002462646666620C100012CAC6CEC2EAEAE2E
        EC6CAC2C60010E4E37770F4F4F0F77374E0E109F8BDBCBDBC7D7CFCFD7C7DB9F
        871728C0C71F1018C42E20C01E1C1822280414100E0D0B17111513139788888C
        92646090928E8E899591959357508C8B4F50526650514D4C4A4E4955534F4BCF
        C8CCD2D064D0CACE993429374F5B27BF60F2E4C2225D06BDE22953A69494EA6B
        96954F9D3AB54293C1A072DAF41955D5864635B53367CEAC336230AE6F9835BB
        B1C9C4A4A971CE9CE6165306B3D6B6F68ECE2E73F3AECE8EEE9E5E0B064B2BEB
        BEFE0936B6B63613FA27DA59D83130D83B383AC180B30D030600009384409085
        EB4CBC0000002574455874646174653A63726561746500323032302D30322D32
        365430393A31313A30372B30303A303093FB19B8000000257445587464617465
        3A6D6F6469667900323031392D30312D30385431363A30323A33382B30303A30
        3085C11DB60000002074455874736F6674776172650068747470733A2F2F696D
        6167656D616769636B2E6F7267BCCF1D9D00000018744558745468756D623A3A
        446F63756D656E743A3A50616765730031A7FFBB2F0000001774455874546875
        6D623A3A496D6167653A3A486569676874003634BCE0A9840000001674455874
        5468756D623A3A496D6167653A3A5769647468003634444F6909000000197445
        58745468756D623A3A4D696D657479706500696D6167652F706E673FB2564E00
        000017744558745468756D623A3A4D54696D6500313534363936333335381708
        B1CE00000011744558745468756D623A3A53697A650032313335421EE99B6900
        000058744558745468756D623A3A5552490066696C653A2F2F2F646174612F77
        7777726F6F742F7777772E6561737969636F6E2E6E65742F63646E2D696D672E
        6561737969636F6E2E636E2F66696C65732F35302F3530363233342E706E678F
        9761270000000049454E44AE426082}
      OnClick = btnListenClick
    end
    object dxBarSeparator4: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
    object pmiBreakoff: TdxBarButton
      Caption = #24378#25286
      Category = 0
      Hint = #24378#25286
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D494844520000001000000010080600000168F4CF
        F7000000017352474200AECE1CE900000044655849664D4D002A000000080001
        87690004000000010000001A000000000003A00100030000000100010000A002
        00040000000100000010A0030004000000010000001000000000345571F20000
        02164944415438119D52BF6B1441149E379BBDF51057D8402C020652A860B009
        04050928CAEE912D0251B1B0901052A6B25044BC42D2F90F58D8984272C80929
        6E8F34694E032902492069F20788DE5D0C31973DEF76FCDEECCD721262E1839D
        F7BEEFFD9E5921CE9686EF27EC253EEABEAF48CAE742A96D4D1892F569E17066
        8D168D20F8D21FA66B803C514A3952CA05D3E51BA23A83D5EA30528FCE39CE95
        9338DE04BE34D04B1FB284B8DFB3CFC771FC02F61063C90722A92BC42AB25B6C
        0BA25D4974877DD9DC0C580E0A85BB49929414D1DA6014CDA4EC3F4E3A989A1A
        ED763AFB9EE3B887EDF635B47A86452FDBF97CE896CB7509A7DEB3D16EAF5F8C
        A20DACF7087776939D5C5862A01A1BE8771DFD1F00B730983A0C82ABCC6BE16B
        6B160AD306F76BBDA6B4AC594C5EC60D3E6627DEEAA309CAD684F309FA7F300E
        D67C27BA02032F8A969840C667C05FF8C698CFE4380C879BBEFF0A956E64240C
        353F6FFF858B45F33C9AD623D4836019AB3F3481205F7AD5EA22F85BA4540DF7
        C6CFD3424C9E758E68E442A5F29DE3D311959AEC4BFE2DA4FCC1D8CBE57690B0
        AD7D9C0C41F15593CC581720CB9A63A085E8BD57A9BCD3B6E32824940688267A
        FB87C06B69607A02A782271F4FBADD1AFFA8E87A0CF61E1EEFABF19FA5B30226
        A01904AF13A58A06A3D81BD85BD87F39E384F8E4B9EE532A958E4E1530413FC3
        7002FFD95B4C74DB7099C684B66D8FBB2B2B7B19F7BFC61F8487CE735F7942F7
        0000000049454E44AE426082}
      OnClick = pmiBreakoffClick
    end
    object dxBarSeparator5: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
  end
  object pmMain: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiAccept'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator2'
      end
      item
        Visible = True
        ItemName = 'pmiConfirm'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator1'
      end
      item
        Visible = True
        ItemName = 'pmiHangup'
      end
      item
        Visible = True
        ItemName = 'pmiCallTransfer'
      end>
    UseOwnFont = False
    Left = 96
    Top = 64
    PixelsPerInch = 96
  end
  object ImageListCall: TcxImageList
    SourceDPI = 96
    FormatVersion = 1
    DesignInfo = 2097736
    ImageInfo = <
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000000F001D007000DA007400DD002A005400000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000004B0095008A00FF008800FF007E00F60017002C000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000001D0039007E00F6008500FF008200FF008900FF005500AC000000000000
          0000000000000000000000000000000000000000000000000000000000000002
          0004006100BE008C00FF008100FF008200FF008500FF006B00EF000000000000
          0000000000000000000000000000000000000000000000000000000000000001
          000100430084008600FF008500FF008200FF008300FF005B00EB000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          00000000000000320065007F00F3008600FF007C00FF003C00A5000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000005000B007200E2008800FF006A00FF00170041000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000040007D008900FF007700FF004C00D600030007000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000001D003A008100F3007B00FF006300FF0026006700000000000000000000
          0000000000000000000000000000000000000000000000000000000000000011
          0020007100D7008000FF006600FF004300BB0003000700000000000000000000
          0000001B0036005F00B6003F007C000000000000000000000000001F003C0070
          00D6007E00FF006500FF005000DB000C00200000000000000000000F001F004C
          0092007D00F4008D00FF008500FF0030005E000B00130042007C007D00F30078
          00FF006300FF005500E600140034000000000000000000000000005700DF0084
          00FF008800FF008400FF008700FF008000F7006E00DF007A00FF006A00FF0063
          00FF005100DB0014003600000000000000000000000000000000005300E70066
          00FF006E00FF007600FF007600FF007000FF006800FF006100FF006100FF0045
          00BA000E002400000000000000000000000000000000000000000021005A005D
          00FA006100FF005E00FF005E00FF006000FF006000FF004F00D50028006A0002
          0006000000000000000000000000000000000000000000000000000000000014
          0037004400B6005A00F4005800F2004000AD001A00460004000A000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000608181B3240C5D93544CFDD13194B5400000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000222B84924051F6FF3F50F2FF3B4AE3F60A0E292C000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          00000D1031363A4AE1F53E4EEDFF3C4CE6FF3F51F4FF283299AB000000000000
          0000000000000000000000000000000000000000000000000000000000000101
          03032B37AABC4152F9FF3C4CE6FF3C4CE7FF3D4EEBFF313FC9EF000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          01011F2778843E4EF0FF3D4DECFF3C4CE7FF3D4CE9FF2A37B7EC000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000171D5A663949E0F33E4EEDFF3949E6FF1B257CA7000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000020308093542CBE13E4FF1FF3140D0FF0A0E3143000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000001D246E7B3F50F2FF3747E0FF242FA1D801020609000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          00000D1032383B49E2F23949E4FF2E3CCCFF11174F6A00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000709
          1B1E3340C3D53B4CEAFF2F3ECFFF1E2A8BBD0102060800000000000000000000
          00000C0F2F342C36A7B41D24707C0000000000000000000000000D1135393341
          C4D43A4AE7FF2E3DCEFF2431A6DD06081922000000000000000007091B1E222B
          84903A48DEF24152FAFF3E4EEDFF161C555F050611111E2570793949DDF13747
          E1FF2E3DCCFF2634B0E80A0D29360000000000000000000000002833ACDD3D4E
          EFFF3F4FF0FF3D4DE9FF3F4FEFFF3A4BE2F73240C6DD3848E2FF3140D1FF2D3C
          CBFF2431A7DD0A0D2B38000000000000000000000000000000002733ACE72E3D
          CCFF3342D2FF3746DBFF3645D9FF3343D5FF303FCFFF2D3BC9FF2C3BC9FF202A
          8EBD06091D2600000000000000000000000000000000000000000F14445A2939
          BFFA2C3BC7FF2B39C0FF2B39C0FF2C3AC6FF2C3BC5FF2331A4D71219536D0102
          050700000000000000000000000000000000000000000000000000000000090D
          2A381E298AB62836B9F42936B8F31E2785B00C1137480203080B000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000000004000A001A
          0046004000AD005800F2005A00F4004400B60014003700000000000000000000
          000000000000000000000000000000000000000200060028006A004F00D50060
          00FF006000FF005E00FF005E00FF006100FF005D00FA0021005A000000000000
          0000000000000000000000000000000E0024004500BA006100FF006100FF0068
          00FF007000FF007600FF007600FF006E00FF006600FF005300E7000000000000
          0000000000000000000000140036005100DB006300FF006A00FF007A00FF006E
          00DF008000F7008700FF008400FF008800FF008400FF005700DF000000000000
          00000000000000140034005500E6006300FF007800FF007D00F30042007C000B
          00130030005E008500FF008D00FF007D00F4004C0092000F001F000000000000
          0000000C0020005000DB006500FF007E00FF007000D6001F003C000000000000
          000000000000003F007C005F00B6001B00360000000000000000000000000003
          0007004300BB006600FF008000FF007100D70011002000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000026
          0067006300FF007B00FF008100F3001D003A0000000000000000000000000000
          000000000000000000000000000000000000000000000000000000030007004C
          00D6007700FF008900FF0040007D000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000000170041006A
          00FF008800FF007200E20005000B000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000003C00A5007C
          00FF008600FF007F00F300320065000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000005B00EB0083
          00FF008200FF008500FF008600FF004300840001000100000000000000000000
          0000000000000000000000000000000000000000000000000000006B00EF0085
          00FF008200FF008100FF008C00FF006100BE0002000400000000000000000000
          0000000000000000000000000000000000000000000000000000005500AC0089
          00FF008200FF008500FF007E00F6001D00390000000000000000000000000000
          00000000000000000000000000000000000000000000000000000017002C007E
          00F6008800FF008A00FF004B0095000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000002A
          0054007400DD007000DA000F001D000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end>
  end
  object TimerCheck: TTimer
    Interval = 5000
    OnTimer = TimerCheckTimer
    Left = 556
    Top = 312
  end
  object TimerCheckCall: TTimer
    OnTimer = TimerCheckCallTimer
    Left = 556
    Top = 168
  end
  object pmCallInfo: TdxBarPopupMenu
    BarManager = BarManager
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = #23435#20307
    Font.Style = []
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiCall'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator4'
      end
      item
        Visible = True
        ItemName = 'pmiBreakoff'
      end
      item
        BeginGroup = True
        Visible = True
        ItemName = 'pmiExitGroup'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator3'
      end
      item
        Visible = True
        ItemName = 'btnListen'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator5'
      end>
    UseOwnFont = True
    Left = 152
    Top = 376
    PixelsPerInch = 96
  end
end
