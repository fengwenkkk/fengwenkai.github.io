inherited FormIPH: TFormIPH
  Caption = #20056#23458#23545#35762
  ClientHeight = 588
  ClientWidth = 1136
  OnClose = FormClose
  OnCreate = FormCreate
  ExplicitWidth = 1152
  ExplicitHeight = 627
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 1136
    Height = 588
    ParentBackground = True
    ExplicitWidth = 1136
    ExplicitHeight = 588
    object gbIPH: TcxGroupBox
      Left = 0
      Top = 0
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alClient
      Alignment = alTopCenter
      Caption = ' '
      PanelStyle.Active = True
      Style.BorderStyle = ebsNone
      Style.Edges = []
      TabOrder = 0
      OnResize = gbIPHResize
      Height = 588
      Width = 1016
      object tlMain: TcxTreeList
        Left = 2
        Top = 2
        Width = 1012
        Height = 224
        Align = alClient
        Bands = <
          item
          end>
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        Navigator.Buttons.CustomButtons = <>
        OptionsData.Editing = False
        OptionsData.Deleting = False
        OptionsSelection.InvertSelect = False
        OptionsView.ShowEditButtons = ecsbFocused
        OptionsView.GridLines = tlglBoth
        OptionsView.Indicator = True
        OptionsView.ShowRoot = False
        PopupMenu = pmMain
        TabOrder = 0
        OnClick = tlMainClick
        OnFocusedNodeChanged = tlMainFocusedNodeChanged
        object ColumnTime: TcxTreeListColumn
          Caption.Text = #21628#21483#26102#38388
          DataBinding.ValueType = 'Integer'
          Width = 153
          Position.ColIndex = 0
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTimeGetDisplayText
        end
        object ColumnTrain: TcxTreeListColumn
          Caption.Text = #21015#36710
          DataBinding.ValueType = 'Integer'
          Width = 103
          Position.ColIndex = 1
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnTrainGetDisplayText
        end
        object ColumnCarriageID: TcxTreeListColumn
          Caption.Text = #36710#21410
          DataBinding.ValueType = 'Integer'
          Width = 103
          Position.ColIndex = 2
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnDevID: TcxTreeListColumn
          Caption.GlyphAlignHorz = taCenter
          Caption.Text = #25253#35686#22120#32534#21495
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 3
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnDevPos: TcxTreeListColumn
          Caption.Text = #25253#35686#22120#20301#32622
          DataBinding.ValueType = 'String'
          Width = 137
          Position.ColIndex = 4
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnDevPosGetDisplayText
        end
        object ColumnStatus: TcxTreeListColumn
          Caption.Text = #29366#24577
          DataBinding.ValueType = 'Integer'
          Width = 79
          Position.ColIndex = 5
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnStatusGetDisplayText
        end
        object tlMainColumn1: TcxTreeListColumn
          Visible = False
          Caption.Text = #35270#39057#27969#22320#22336
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 6
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object tlMainColumn2: TcxTreeListColumn
          Visible = False
          Caption.Text = 'IPHDevIP'
          DataBinding.ValueType = 'String'
          Width = 100
          Position.ColIndex = 7
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
        object ColumnDCUserID: TcxTreeListColumn
          Caption.Text = #22788#29702#20154
          DataBinding.ValueType = 'Integer'
          Width = 122
          Position.ColIndex = 8
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
          OnGetDisplayText = ColumnDCUserIDGetDisplayText
        end
        object ColumnUptime: TcxTreeListColumn
          Visible = False
          DataBinding.ValueType = 'Integer'
          Width = 100
          Position.ColIndex = 9
          Position.RowIndex = 0
          Position.BandIndex = 0
          Summary.FooterSummaryItems = <>
          Summary.GroupFooterSummaryItems = <>
        end
      end
      object gbVideo: TcxGroupBox
        Left = 2
        Top = 232
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alBottom
        Caption = ' '
        PanelStyle.Active = True
        Style.BorderStyle = ebs3D
        Style.Edges = []
        Style.LookAndFeel.SkinName = 'Office2010Blue'
        StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
        TabOrder = 1
        Visible = False
        Height = 354
        Width = 1012
        object PanelEmpty: TPanel
          Left = 217
          Top = 2
          Width = 578
          Height = 350
          Margins.Left = 0
          Margins.Top = 0
          Margins.Right = 0
          Margins.Bottom = 0
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 3
          Visible = False
          OnMouseDown = PanelVideoMouseDown
        end
        object PanelVideo: TPanel
          Left = 217
          Top = 2
          Width = 578
          Height = 350
          Margins.Left = 0
          Margins.Top = 0
          Margins.Right = 0
          Margins.Bottom = 0
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 0
          OnMouseDown = PanelVideoMouseDown
        end
        object PanelSide1: TPanel
          Left = 2
          Top = 2
          Width = 215
          Height = 350
          Align = alLeft
          BevelOuter = bvNone
          TabOrder = 1
          OnMouseDown = PanelVideoMouseDown
        end
        object PanelSide2: TPanel
          Left = 795
          Top = 2
          Width = 215
          Height = 350
          Align = alRight
          BevelOuter = bvNone
          TabOrder = 2
          OnMouseDown = PanelVideoMouseDown
        end
      end
      object Splitter: TcxSplitter
        Left = 2
        Top = 226
        Width = 1012
        Height = 6
        AlignSplitter = salBottom
        Visible = False
      end
    end
    object gbWorkPanel: TcxGroupBox
      Left = 1016
      Top = 0
      Align = alRight
      PanelStyle.Active = True
      Style.BorderStyle = ebs3D
      Style.Edges = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 1
      Height = 588
      Width = 120
      object btnHangoff: TcxButton
        Left = 20
        Top = 30
        Width = 85
        Height = 60
        Caption = #25509#21548
        Enabled = False
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
        OptionsImage.Layout = blGlyphTop
        ParentShowHint = False
        ShowHint = False
        SpeedButtonOptions.CanBeFocused = False
        TabOrder = 0
        OnClick = pmiHangoffClick
      end
      object btnHangup: TcxButton
        Left = 20
        Top = 120
        Width = 85
        Height = 60
        Caption = #25346#26029
        Enabled = False
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
        OptionsImage.Layout = blGlyphTop
        ParentShowHint = False
        ShowHint = False
        SpeedButtonOptions.CanBeFocused = False
        TabOrder = 1
        OnClick = pmiHangupClick
      end
      object btnCleaner: TcxButton
        Left = 20
        Top = 210
        Width = 85
        Height = 71
        Caption = #19968#38190#28165#38500
        Enabled = False
        LookAndFeel.SkinName = 'Office2010Blue'
        OptionsImage.Glyph.SourceDPI = 96
        OptionsImage.Glyph.Data = {
          89504E470D0A1A0A0000000D4948445200000020000000200806000000737A7A
          F40000001974455874536F6674776172650041646F626520496D616765526561
          647971C9653C0000000C744558745469746C650052657365743B64CF80690000
          099349444154785EC5577B7054D51DFECEBD7777936C48B29B374B9E0644A0C0
          2491C82314251615C66A5A517144E9D86A5B3B8EB5E368AD83D4513B5671D43A
          B5B50363752830AD0A3E78886124208104887941089098D76637FBC8BEF73E4F
          73CEDCEBE48F3A8E7F38FDEDFCE69C3B7BCEF9BEF3FD1EBB97504AF1FF34F23D
          EDA5DF1701628D967F03B8E9DF4E86BCF6E13A8010BE0C84B2D17CA66CE408BF
          D970D00212983FFDB7C6EABCC28C8D9224AE2502CD110859CCD6524ABB284544
          D7E867C9A8B2E7A9CD4707011800E81B9FDC42F9F9166B02FCEAA60320AFEC5B
          C7716F5BFD00082110880842043E17898092591B88092C3EFBF69A35D939F667
          8ADDB39717BB2B50985B8ABCEC7C383373C02C998E21120F6232328E89F0087C
          E1B1D6545CDDFAE4A696568B48387D844E1B76B56CC7C3EB0F40D03503BA4161
          500ACA575150CAD75AE062E3FA32E79F76AF7DBDBAB2EA50C3C2A6E5AB96ACC3
          BCF2AB903D8B40A65E0412BD08262F404300B9B919B8A66A11D62CDD80150B7E
          D45839A7AAE5E5FFDCF8FA8FB75C9D0980B8329A0805E598CCC88B7B9BF8A479
          ED031099E44484008272D74F38F8BD8F2D2E5EB8AC68CF82EA252B6BE6CC8364
          D3905482602610018220B23D7C4EC09C98A1139169CB85A250F40FF7A26FF0EC
          F18BDDE18D7FDF76C60FC078E5831FD1476F3B0CF186E62A30460BAA9682EF05
          45A57B23076FDC509E77DDBA399F2C9957DF5053568DA4E645420EC180610592
          FBD7D946C1CFA25C510DB2960088064F41254421A35C16836B9CD9B63DFD9D41
          F5D0EECB6026E9BAC18F31A80EF0300856B2D9AE6FAE7CF9EA8A45B565A525F0
          C5FA41040289881C542476D8A44C4862066C829DDD9E9F615015AAAE40373430
          D374152935017681B49CAAD39B8CEDFB76F63F64558AC008683A8BBB066A18A8
          C9DFC4A57F705B5D83A7D8B3D9535282C9F86568BA0243D7A01B3A44C18E095F
          0C1F1F69C38B7FDD8987FEF0473CF8D43378FE8D7F60DFE1568C8D87D91A0EAE
          191A1FC3A971547A2A5152E0D9F2D8F6E5CBCC4B1249D73911E84C0102EBF6F6
          C2D2AC673DC565481B01A4B5144441023500BBE444FBD9011C3BDDA90D0FFBDF
          4E27F40F863A126D0010AC4F36F45D1CBCBDA3A7F7BEB52B9649CB6AE743D192
          30A8C1135BB049282FA9C6F0C4C87300D601302456050C5933142C2CF935BFFD
          E6C717D715E617ADCACD732098F4727921020E7B16CE750DE2404B9BCF3B18BB
          F3ECFE5007000D800E00A161E5530047E55B8D7F46A29FED3560145FBB642E14
          3D01567AB1741845F9A5287217AF7EE8D9FABA379FEE38253005781874CDCA2A
          29AF20B3D995E74224ED83AAAB300C1D94124C4EC6D0DE7941F30D26364E83B7
          039001A82601DD9CCBE7F687DB2747123F3DD1DEA98DF942A0143C0CAAA620A1
          8650985B849C3C7B330051D2CC24AC2B7BC46A38926023F5CE2C07524A888786
          128A0CE2C4E0F004BC13A1773BF6073A2DE0AFA6F6500258E587B2DC3B0C46E4
          CCBE60675E91E39D8B9747B65CE79ECB087015E24A049283C200E57920BCF4F0
          49F2E7874F4A002C67297D9560D320AB692B914009C1B837806450DB6F76354E
          B822EF4EC2487237740C857753F37B4D891BEF0D8FF1107225934A1457FC3DD0
          05198661D4001079B901C80290032017C02CAA53775C0F22920E41D554689AC6
          D987A722F8F248B00F80D3F40C46BACAB589E8546325C8EB1F0027D1D73A7526
          100A433354F8A323180CF4212E4F818A6968AA910F409096DDE8715FDB347B94
          0022D39200607921AB0924E528924A0C99362772B2DC28CA77E1FEDF2FBD4000
          F0B514FA6BBF3BCD881BBAA1E9B042C18DCF89240A18090D20101F07A5E000B2
          9A84AAF2E5443AFDE958B2B0C6BEFB969B57DCB364D1D500170CE8F51E876E70
          A59130E208C4BC68BEF56638A40C488284CEDE3E7C74F0E82E331454D56506C8
          4BCE4AE6C5AB0BEA8A0AF259236292735924D8904CC9482B5A0866CCD5BE2F82
          2F64669FBA35ED189A251B71AE2021C424C34784E27E1CBFF86F088420CBE6C6
          C9566FB4E744E039AB04A3A930DF735DC513BC940138F20A339B4B8A5D482B01
          E8FCFA001145C4A32AE4B476850B69E64076E3DDA58F2C6A28DC5AB3D009594B
          82839B241819CB32EDD9183A9F444FC7E4D6C33B865E0590344930040B3CF386
          8D550D8B1A667FB2B46E8E14542EF124649635BD3F34EA40F7D9D1D7DF7F75E0
          49C96C24E9D3FBFD3B3272A4FBCBAAB22B44870D06B8FC3363CAA54FC709AEF4
          87FA8FED1DDD6966BB64B55573EEF8E1ED15F5731717BC337F5EB994267E289A
          02005C216A08F04FC430399A62D5A49319ACB36BD717DCB9B0A1E0CDBA9585BC
          FD0A163821E6ED67E14CAB1FBD6DFEBBDB3FF6B52C5BE729751566541E7AF772
          3B23D1B4A97AD5F4F32D9EF2FCBBAF995F21D95C118C4F0D0214DCECB64C4426
          047C797AB2FDA3BF5CB909408C5CFFF3521C7DCBCBFB3F0057E3BDC57B1B6F2A
          5F955F64072B2DAE0111587742C0ABA2AD65E4C0911D230F02C05DBF5DF441C9
          6C576D2CCE4306B73B17B34B0B50549A0D59F4612C3408CB044104951DB8DC2D
          A3FB946F7DC747BECF01A424969D66FC540089D1F3C96DDD0593079B36548BF4
          EB9F7DA6851DBD9DDEF4A5B391E701A0E99EEABBE6CDAFAC5DB97A01ECA203CC
          12720491B41F81D825C42221EB3F023711360C0DCAF8EA4A70EF34F829000AC3
          950C4AB1FA6745F4D80E3F63220F76C4BA0A2AECEF0C9C0FDD3F77818B77379B
          68C7404F18BEE1E85B439DD191E272A7ABC833EBF1854BE6A067F4042B31F317
          4F07059F9B7F6C05080220100943FD295CEC89F44C37B2276626AE40298561D2
          3455480EB4C55EEEE99C08AA692E3D5499A0AFDB17387730F02600B2F4FA9247
          EBEB171404E5619E7F0261CE43C59F099B438024DAA024450C7425D0D7193A73
          E144785370243D094069FA85874B2F5113BCE5D2567A43CD360090A7C69489C0
          68EAA52FCF7A5F58DE5889CE7363088EA5B6A7A25A6C6EADBBBCD0937B8FA73A
          07FE6800A220C23043253222FC3C1B54CDC0F857718C0FA7303E147FBFFB50E8
          B1784863E069F0CEC9F12150F0CFCC72D3990A3D0723BBFAFB26BAA68232067A
          7D5DE73E0EEC629BF3AEC21DD174D4D6D2DA06EF581CE9B8000919904806E4A4
          80C97119FD5D5338F5F9043A4EF8BBCF9F0A6E3EF92FFF2FA7C1FD26B87EB8FF
          516AA94E96DFE7C64C3BF976C82A4B67CD2A67E3FC1F78DEBBD4EB6FBE706CEA
          B819A28CAA06676D6E89FD364796B08208C8166DA4863357E92550125392FA17
          E131F5C3C1F6780F80B809AC01301AB7145198D6BAD3FF8DAF665653C9343BA5
          6AB1B7DAACE976AB1159EA9940CA0CD74D6746BFCBBB210120986E986E9960F9
          FF38C398E1DFFA7E48BEC3CB2BFD8E7B29BEDDF05FA803D10321B41163000000
          0049454E44AE426082}
        OptionsImage.Layout = blGlyphTop
        ParentShowHint = False
        ShowHint = False
        SpeedButtonOptions.CanBeFocused = False
        TabOrder = 2
        OnClick = btnCleanerClick
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
    Left = 80
    Top = 96
    PixelsPerInch = 96
    object dxBarSeparator1: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
    object pmiHangoff: TdxBarButton
      Caption = #25509#21548
      Category = 0
      Hint = #25509#21548
      Visible = ivAlways
      OnClick = pmiHangoffClick
    end
    object pmiHangup: TdxBarButton
      Caption = #32467#26463
      Category = 0
      Hint = #32467#26463
      Visible = ivAlways
      OnClick = pmiHangupClick
    end
    object dxBarSeparator2: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivAlways
      ShowCaption = False
    end
    object pmiAECMode: TdxBarButton
      Caption = #22768#38899#20248#21270
      Category = 0
      Hint = #22768#38899#20248#21270
      Visible = ivAlways
      ButtonStyle = bsChecked
      OnClick = pmiAECModeClick
    end
    object pmiPlayVideo: TdxBarButton
      Caption = #25773#25918#35270#39057
      Category = 0
      Hint = #25773#25918#35270#39057
      Visible = ivNever
      OnClick = pmiPlayVideoClick
    end
    object pmiStopVideo: TdxBarButton
      Caption = #20572#27490#35270#39057
      Category = 0
      Hint = #20572#27490#35270#39057
      Visible = ivAlways
      OnClick = pmiStopVideoClick
    end
    object btnIPHPause: TdxBarButton
      Caption = #25346#36215
      Category = 0
      Hint = #25346#36215
      Visible = ivAlways
      OnClick = btnIPHPauseClick
    end
    object btnRenewIPH: TdxBarButton
      Caption = #25346#36215#24674#22797
      Category = 0
      Hint = #25346#36215#24674#22797
      Visible = ivAlways
      OnClick = btnRenewIPHClick
    end
  end
  object pmMain: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiHangoff'
      end
      item
        Visible = True
        ItemName = 'btnIPHPause'
      end
      item
        Visible = True
        ItemName = 'btnRenewIPH'
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
        ItemName = 'dxBarSeparator2'
      end
      item
        Visible = True
        ItemName = 'pmiAECMode'
      end>
    UseOwnFont = False
    OnPopup = pmMainPopup
    Left = 240
    Top = 56
    PixelsPerInch = 96
  end
  object TimerCheck: TTimer
    OnTimer = TimerCheckTimer
    Left = 212
    Top = 132
  end
  object TimerCheckRTP: TTimer
    Enabled = False
    OnTimer = TimerCheckRTPTimer
    Left = 144
    Top = 52
  end
  object pmVideo: TdxBarPopupMenu
    BarManager = BarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiPlayVideo'
      end
      item
        Visible = True
        ItemName = 'pmiStopVideo'
      end>
    UseOwnFont = False
    Left = 337
    Top = 218
    PixelsPerInch = 96
  end
  object TimerGetIPH: TTimer
    Interval = 10000
    OnTimer = TimerGetIPHTimer
    Left = 92
    Top = 204
  end
  object TimerCheckIPHAck: TTimer
    OnTimer = TimerCheckIPHAckTimer
    Left = 196
    Top = 220
  end
end
