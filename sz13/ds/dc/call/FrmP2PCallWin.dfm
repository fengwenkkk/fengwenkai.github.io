object FormP2PCallWin: TFormP2PCallWin
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = #20010#21628#21628#20837
  ClientHeight = 175
  ClientWidth = 311
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  StyleElements = []
  OnClose = FormClose
  OnDeactivate = FormDeactivate
  OnHide = FormDeactivate
  PixelsPerInch = 96
  TextHeight = 18
  object gbMain: TcxGroupBox
    Left = 0
    Top = 0
    Align = alClient
    Alignment = alTopCenter
    PanelStyle.Active = True
    Style.BorderColor = clRed
    Style.BorderStyle = ebsNone
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Office2010Blue'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
    TabOrder = 0
    DesignSize = (
      311
      175)
    Height = 175
    Width = 311
    object dxBevel1: TdxBevel
      Left = 199
      Top = 0
      Width = 7
      Height = 175
      ParentCustomHint = False
      Anchors = [akLeft, akRight, akBottom]
      LookAndFeel.SkinName = 'Office2010Blue'
      Shape = dxbsLineRight
      Visible = False
      ExplicitWidth = 5
    end
    object btnClose: TcxButton
      Left = 214
      Top = 78
      Width = 75
      Height = 25
      Caption = #32467#26463
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Office2010Blue'
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
        89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
        61000000097048597300000EC400000EC401952B0E1B0000020849444154388D
        95923F6B536114C69FF75F6EEEBD2626D5066B21830E4A8B8850424C6CA18522
        365DBB083AA8830EFD021644B08B8BA083882E7E004107B182985ADAA2B33AD4
        A2018B6D0489A6496C929BF77D8F43C85095923C70E070861FCF79CE6100B098
        3D9EE696DF00610C8C11115E73490FEAAE5C3EFBEAFD6FEC21F6263D745E703C
        968CABA6250040883110030CD93500F76BD1E0E1D4CBCFCDFF02F2E9E1625489
        4321A5E04A094B046D0D02DD2E80503794E7D25C1B5B595BFF1BC05D21768EC6
        F72319D987A81342C27371D0731BFDBE571888F8082B055FF209327C61397B22
        FD0F20198F8C0796EE06C656E3AAEDA065E9EA602C3EE229E7FA40C4AF247C0F
        BE9447AC35CF96464FA676ADD069B6CE65464252DE0C8C7977F8C5EA7C675E9A
        CE9C262E1EB5B4192E566BD8D6BAA085989C5CF950D805D84B9BD3D9A4627C01
        6487362B359482202F9C9DDCF8D2D706EF0630F87C75A3D9B2972513D57EDF43
        4CAA096AB897BA76D0D1B7A93373D19098FF596FA050AE7EFAC12AA7BA72D091
        F5F49D5ACB6CC4C30EFA5CE758DC78999E0054631714479F2620E1862185B8D2
        D50A04B0EFB9EC2D5788394D8416113818BEFC2A97BA733033C30948BB5220A0
        F6BB730EC4C28EE83AC4626E3425382D0AC6BDA6B5882A8172533FE9E90A5BB9
        ECBD5848CD6A22348DAD73A9533D85A8616E6F07AD75636DA161F5C5034FDF7E
        FC03F3E1D41204D007EC0000000049454E44AE426082}
      SpeedButtonOptions.CanBeFocused = False
      TabOrder = 2
      OnClick = btnCloseClick
    end
    object cxLabel26: TcxLabel
      Left = 30
      Top = 40
      Caption = #20027#21483#65306
      Transparent = True
    end
    object cxLabel27: TcxLabel
      Left = 30
      Top = 80
      Caption = #29366#24577#65306
      Transparent = True
    end
    object cxLabel28: TcxLabel
      Left = 30
      Top = 120
      Caption = #36716#25509#65306
      Transparent = True
    end
    object tbVolume: TcxTrackBar
      Left = -22
      Top = 180
      ParentShowHint = False
      Position = 50
      Properties.AutoSize = False
      Properties.Max = 100
      Properties.ShowTicks = False
      Properties.ThumbHeight = 24
      Properties.ThumbWidth = 30
      Properties.TickSize = 25
      Properties.TickType = tbttValueNumber
      Properties.TrackSize = 12
      ShowHint = True
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Silver'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Silver'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Office2010Silver'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Office2010Silver'
      TabOrder = 10
      Transparent = True
      Visible = False
      Height = 40
      Width = 111
    end
    object LabelCallingNumber: TcxLabel
      Left = 76
      Top = 40
      Transparent = True
    end
    object LabelCallTime: TcxLabel
      Left = 180
      Top = 117
      Transparent = True
    end
    object btnHangoff: TcxButton
      Left = 214
      Top = 38
      Width = 75
      Height = 25
      Caption = #25509#21548
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Office2010Blue'
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
        89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
        61000000097048597300000EC400000EC401952B0E1B0000018349444154388D
        95D13D68530114C5F1DF7B2F2F3E1B5012456C07F163B1E026469062EBD041E8
        2EC4DA452777171D225850105CC4C15241671717E9500503E2264510072D5268
        6927C1A2AD2F49F39E83F1AB484DCF72B907EE1FCEB901187752A42E775A2E13
        98C57D8997A6ACDB42810B6A420F85629DAE1B7667EE9DC03D6BA63DD6FA3760
        DC8A3EFB2548BA6E0B299ADDBD6356ECB207E637034289D4010C6017CAD82BB5
        CF827EEC446C54DB8C09D5CD80C8594FC4221DC7F4296A2373D11E5704BE28A9
        8A15A52A32638E6B98B3FC3BC24FD555155DB7E195BAC95FFE0D432253528356
        909A171AF5C8C2DF80AD74D341B9192D472D63CD331563EE6A463D019EFB6CC8
        9CC4B96E9CC3D62D79E3756F00685834ACA0E48C00AB8E18311DFEF7F04FADBA
        23B5A88CDD067D726A7B80929A82B20C15842EF5162117084DDAE1368A3A28E0
        ABFE5E3B28085D9538A4E9C7EF22645ABD011A3223DEEBA889C472141178DAFB
        175E58326C40E284106DDFC4CE6FAFC4C82D4D1F643EDA30E19AB7DF018C1E68
        4A961114E80000000049454E44AE426082}
      SpeedButtonOptions.CanBeFocused = False
      TabOrder = 1
      OnClick = btnHangoffClick
    end
    object btnCallTransfer: TcxButton
      Left = 214
      Top = 118
      Width = 75
      Height = 25
      Caption = #36716#25509
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Office2010Blue'
      OptionsImage.Glyph.SourceDPI = 96
      OptionsImage.Glyph.Data = {
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
      TabOrder = 3
      OnClick = btnCallTransferClick
    end
    object edtTransferNumber: TcxTextEdit
      Left = 76
      Top = 118
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Office2010Blue'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Width = 114
    end
    object LabelStatus: TcxLabel
      Left = 76
      Top = 80
      Transparent = True
    end
  end
  object TimerCheck: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = TimerCheckTimer
    Left = 140
    Top = 67
  end
end
