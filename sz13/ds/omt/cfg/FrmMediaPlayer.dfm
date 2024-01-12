inherited FormMediaPlayer: TFormMediaPlayer
  BorderIcons = [biSystemMenu]
  ClientHeight = 161
  ClientWidth = 483
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnShow = FormShow
  ExplicitWidth = 499
  ExplicitHeight = 200
  PixelsPerInch = 96
  TextHeight = 14
  inherited MainPanel: TPanel
    Width = 483
    Height = 161
    ExplicitWidth = 875
    ExplicitHeight = 538
    object PanelPlay: TPanel
      Left = 3
      Top = 3
      Width = 429
      Height = 27
      BevelInner = bvLowered
      BevelOuter = bvNone
      TabOrder = 0
      Visible = False
      object PanelMediaPlayer: TPanel
        Left = 1
        Top = 1
        Width = 427
        Height = 25
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 0
        object WindowsMediaPlayer: TWindowsMediaPlayer
          Left = 0
          Top = 0
          Width = 427
          Height = 25
          Align = alClient
          TabOrder = 0
          OnPlayStateChange = WindowsMediaPlayerPlayStateChange
          ExplicitWidth = 245
          ExplicitHeight = 240
          ControlData = {
            000300000800000000000500000000000000F03F030000000000050000000000
            0000000008000200000000000300010000000B00FFFF0300000000000B00FFFF
            08000200000000000300320000000B00000008000A000000660075006C006C00
            00000B0000000B0000000B00FFFF0B00FFFF0B00000008000200000000000800
            020000000000080002000000000008000200000000000B000000222C00009502
            0000}
        end
        object PanelSide1: TPanel
          Left = 0
          Top = 0
          Width = 427
          Height = 25
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 1
          ExplicitLeft = 65
          ExplicitTop = -5
          ExplicitWidth = 433
          ExplicitHeight = 55
        end
        object PanelSide2: TPanel
          Left = 224
          Top = 89
          Width = 185
          Height = 60
          BevelOuter = bvNone
          TabOrder = 2
        end
      end
    end
  end
  object gbPlay: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    Alignment = alTopCenter
    PanelStyle.Active = True
    Style.LookAndFeel.SkinName = 'Office2010Blue'
    StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
    TabOrder = 1
    ExplicitTop = 5
    ExplicitWidth = 435
    ExplicitHeight = 93
    DesignSize = (
      483
      161)
    Height = 161
    Width = 483
    object btnPause: TcxButton
      Left = 180
      Top = 90
      Width = 40
      Height = 36
      Hint = #26242#20572
      Anchors = [akTop, akRight]
      Caption = #26242#20572
      LookAndFeel.SkinName = 'Office2010Blue'
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000020000000200806000000737A7A
        F4000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
        00097048597300000EC300000EC301C76FA864000001DD494441545847ED96CD
        4B945118C527C4BB712DF83788892212216225228ADAD2360689189442483084
        5A62E5571A315A7EA0828490101248D684A895C91846C92CD2D09A2475404210
        0B43E4E879182244785A3CCBF707877BCFF9C10BEFEEFA3C3C3CFE93133DC1A2
        CE9E6049B4F7550978B273579C1971DD2F8BF16B7FE36FD8B92BCE8CF887E345
        D8DE8F606EB545C2CE5D7166B8C05821B6F61611FADE2461E7AE3833DC836705
        D8DCFB84D9C81D093B77C599E1DA9FE623FA278499AF8D1276EE8A33C3B58EE4
        E1C7EE14DEAC3448D8B92BCE0CD7349C8BC8EF714C2FD74BD8B92BCE0CD73874
        16CB3BA398FC522761E7AE3833DCAD811C2C6E0F6362E986849DBBE2CC70B57D
        D9086F0D20F8D92F61E7AE38339CFF51163EFEECC2E84239C6C257C1CEFDA87B
        F2A1F45F6786BB1E388DF9CD0E3C7E7F5EC2CE5D7166B86BF74F2114BD8BA150
        B1849DBBE2CC70D56D999859BB89C1D942093B77C599E1AE3467607AD58FFE77
        F91276EE8A33C355DE4EC7C4B71AF4BECD93B073579C19AEA2210DC1952A74BF
        3E2761E7AE3833DCA5FA543C5FBA8CC0640EEEBD380376EE8A3323FE424D727F
        993F65FD62EDC95D9EECDC1567069F570987493C4C52EC64972759EC7E9C3383
        0F4C7E50FE3816767994C6EEC7390F0F059FEF00082F23FC31CBD07900000000
        49454E44AE426082}
      OptionsImage.ImageIndex = 1
      PaintStyle = bpsGlyph
      TabOrder = 1
      OnClick = btnPauseClick
    end
    object btnPlay: TcxButton
      Left = 180
      Top = 90
      Width = 40
      Height = 36
      Hint = #25773#25918
      Anchors = [akTop, akRight]
      Caption = #25773#25918
      LookAndFeel.SkinName = 'Office2010Blue'
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000020000000200806000000737A7A
        F4000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
        00097048597300000EC300000EC301C76FA8640000034F494441545847ED967B
        4853511CC7D7C3D33B2A307AFD57605086568C59A1F630E72B0D472582959569
        14DA032BCBAE6539676E3935D1594E6DAD876DB3C7328466E6239DAF569166BE
        5A6526D1932082FA76AE5D2269FDE3E620F2031FB8F73C7EE79CDFB9F79ECB1B
        62887F8161D9B702B4B212EFE9DCBDDD19412700EADB0CBDCF5686E10DE7CAED
        86C3E99BFEA8EE504171671B32F4FE06B956389BABB30B24E3BA2F7A3F99F0EC
        5D1D8A8C87917EDDE7B35C27DCC7301E23B936830A492BF641CFC70698BACFA2
        FD8D1E75CF3450182221D309EB532E7BB970ED060D22D578A3FB432DEACDE97D
        3EEE51C3FCEE1E8AEB1391AAF1FE7AB2C8EB38A3F418CDB5B73924E5F26A3C7F
        5F09A359DECFE6D757607A790339B77720F9A2578B58E5E5CEF5B12944AC5E85
        8E37A5A8ED3AF5878DCF156879ADC5B50609C4EA95DF8E9F5B91C5C8F913B9BE
        3681D0A0B8FFA200551D1254774A2DCAD63FEE29419E61178E152E371F557AFA
        71FDAD8624283D61ECCAC4D507E1A86817A3AA3DC5A2F73A6530BD2884BE498A
        93DAF560F2DCD54C96DB542ECE8021F1B9EE7480545C6A58074D53180C4F1854
        B449FE6A6D57169A5FE99055128E78857B2F8D61D5C78B1CCC5E86B2D644A8EB
        833945287EB01D86D663287F2AFEE5DDB664FA8AE6D0ED50A1A03C0607B2977D
        DB2B7753D218237E861A18243673094A9BE3A03206F5F3BC712D8A1AC370E361
        349D40321ACC4AE84D4934FD1BB05B2A78141C39D79FF61F4FB56E027BD20474
        90DD28A809F8C3F34611DD962D28BE1F85844BAB1123E37FDC7CC8E5C8B8713C
        76EFC750D9AFE53036D04021D1523E744D915056FBF6F3425D08B48D11905CF3
        47742AFFFBB6A3AE3A81CF2C67DA875DB503D5260717D929594C1FC04D385325
        ECB3A026A8EF3EBB2C18B1F91E883AB1B075CD5627116D3B893A8ACAA6DCAA55
        FF0E894A5A0455ED7AE4567A23BF3A10853522C49DF344E489859F43F6CC4B72
        741C3B8DB61B4BB53ADD96201189AE505605510391A859899DE90284C53997B8
        0967B8D2FA09549BA5DB12644B820BD24A572146B1141B0F2FE8F20B9F134ACB
        2753D903C8A6E9B604D91CBF801DF88B6897936CCAAC313369D9A0A5DB120EA1
        FBE7DFE20B670AE8357BC810AA5D7FCBD814B3EFB35DD26D09764076C5761F78
        88FF011EEF079F080E201EEE75750000000049454E44AE426082}
      OptionsImage.ImageIndex = 0
      PaintStyle = bpsGlyph
      TabOrder = 2
      OnClick = btnPlayClick
    end
    object btnClose: TcxButton
      Left = 394
      Top = 112
      Width = 75
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = #20851#38381
      LookAndFeel.NativeStyle = False
      TabOrder = 0
      Visible = False
      OnClick = btnCloseClick
    end
    object btnStop: TcxButton
      Left = 240
      Top = 90
      Width = 40
      Height = 36
      Hint = #25773#25918
      Anchors = [akTop, akRight]
      Caption = #20572#27490
      LookAndFeel.SkinName = 'Office2010Blue'
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000020000000200806000000737A7A
        F4000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
        00097048597300000EC300000EC301C76FA864000001CF494441545847ED96CB
        2B84511C86477276F6CA9FA0484A4A4A2244946425260B85B2409471CB25C442
        24974452A284C284426E8D7129933B434388486281D5EB7B9D6C9973ECF43DF5
        6E667ECF53DF6CBEB198989898FC824FF75CCA98B1B79EF914A88C0E5D36644A
        0FDF4E7BF29BFB7106772F5BB87FDDF16ABCA543970D99D2C3AF633A09B7CF0E
        6C5E35C379D9E8D5784B872E1B32A587689B4CC4F5F31A1C9E06A5D1A1CB864C
        E9215AC7E271F9B488F58B5AA5D1A1CB864CE9219A4662E17EB063E5BC466974
        E8B221537A88FAA118B86E06B1786AC3D259A557E32D1DBA6CC8941EA2BA3F1A
        4E4F3B265C59983B2AC6C249F98FE30D6FE9D06543A6F410B6DE28ACBA9B31BA
        93F1B5F1DD6C4CED1560F6B018F34765C64A317B50F4F519BFFBBEA343970D99
        D24394754662E1B80AC35B694AA343970D99D24314B545C0BE5F822167AAD2E8
        D06543A6F410852DE198D8CDC38023496974E8B221537A88FCC6308C6C67A16F
        3D416974E8B221537A88DCBA500C6EA4A367354E6974E8B221537A889CEA90F7
        AE65E3A9D6929546872E1B32A5875F6669D0B4D516FC61AD0886D20C872E1B32
        A5075FA5FEC6028C052A8E0EDD3FBD8EF96782013E057F4A95D1A1FBA73F2426
        26FF1D8BE51377B1D6DEEB55113D0000000049454E44AE426082}
      OptionsImage.ImageIndex = 2
      PaintStyle = bpsGlyph
      TabOrder = 3
      OnClick = btnStopClick
    end
    object LabelMediaLength: TcxLabel
      Left = 422
      Top = 37
      Anchors = [akTop, akRight]
      Caption = '0:00'
      Transparent = True
      ExplicitLeft = 374
    end
    object tbPlay: TcxTrackBar
      Left = 21
      Top = 30
      Anchors = [akLeft, akTop, akRight]
      Properties.Max = 100
      Properties.ShowPositionHint = True
      Properties.ShowTicks = False
      Properties.OnGetPositionHint = tbPlayPropertiesGetPositionHint
      Style.BorderStyle = ebs3D
      Style.Edges = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      Style.Shadow = False
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Office2010Blue'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 5
      Transparent = True
      OnMouseDown = tbPlayMouseDown
      OnMouseUp = tbPlayMouseUp
      ExplicitWidth = 324
      Height = 33
      Width = 372
    end
  end
  object TimerPlay: TTimer
    Enabled = False
    OnTimer = TimerPlayTimer
    Left = 55
    Top = 90
  end
end
