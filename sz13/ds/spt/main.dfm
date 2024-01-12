object FormMain: TFormMain
  Left = 0
  Top = 0
  ClientHeight = 744
  ClientWidth = 1250
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = #23435#20307
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  WindowState = wsMaximized
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 14
  object PanelLockWin: TPanel
    Left = 0
    Top = 0
    Width = 1250
    Height = 696
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    Visible = False
  end
  object PanelBottom: TPanel
    Left = 0
    Top = 696
    Width = 1250
    Height = 48
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alBottom
    BevelOuter = bvNone
    Color = 3153944
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = #23435#20307
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 0
    object PanelStatusBar: TAdvSmoothPanel
      Left = 0
      Top = 0
      Width = 1250
      Height = 48
      Cursor = crDefault
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Caption.HTMLFont.Charset = DEFAULT_CHARSET
      Caption.HTMLFont.Color = clWindowText
      Caption.HTMLFont.Height = -11
      Caption.HTMLFont.Name = 'Tahoma'
      Caption.HTMLFont.Style = []
      Caption.Font.Charset = DEFAULT_CHARSET
      Caption.Font.Color = clWindowText
      Caption.Font.Height = -12
      Caption.Font.Name = 'Tahoma'
      Caption.Font.Style = []
      Caption.ColorStart = clWhite
      Caption.ColorEnd = clCream
      Caption.TextRendering = tAntiAlias
      Fill.Color = 8938576
      Fill.ColorTo = clNone
      Fill.ColorMirror = clNone
      Fill.ColorMirrorTo = 16773863
      Fill.GradientType = gtVertical
      Fill.GradientMirrorType = gtVertical
      Fill.BorderColor = 14922381
      Fill.BorderOpacity = 165
      Fill.BorderWidth = 0
      Fill.Rounding = 0
      Fill.ShadowColor = clNone
      Fill.ShadowOffset = 10
      Fill.Glow = gmNone
      Fill.GlowGradientColor = 10053222
      Fill.GlowRadialColor = 10053222
      Version = '1.6.0.1'
      Align = alClient
      OnMouseUp = PanelStatusBarMouseUp
      TabOrder = 0
      DesignSize = (
        1250
        48)
      TMSStyle = 0
      object btnNTPStatus: TAdvSmoothPanel
        Left = 110
        Top = 6
        Width = 48
        Height = 24
        Cursor = crDefault
        Caption.Text = 'NTP'
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Left = 20
        Caption.Top = 3
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.Picture.Data = {
          89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
          61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
          00097048597300000EC300000EC301C76FA864000002EB49444154384F6D9379
          48D36118C7B754A2C2ACFE294AA8209A27A6D2F24A2B21F398DB3CE635CD299E
          B9999A986487B6CCA384A0084AF3BEE62CCF16A678649E9BF38804BB88FA2735
          FCBBFEF9F6BCBF0D49EA810FCFEFE5FD7EBFCFFB8E77BCBF8A4F5864DDF194A8
          2ABC9F5EBE775A9F53E5FB8B75B6BEA4F690D2BEA559F74FF123725C0EA9CA7D
          7AD4CF24D0E9D5307E7D8295B566AEEBF425B85D2346D65DAF5EA9D2D996F4DB
          4C3653F115854287DCFB67561B07B231FFFD31463F5E856E3915BDEF2EE2E5FB
          140C7F2880F1DB4334BC524255E6F3439CE672847CA6105B0FDB1DCA32CF79ED
          580126BFA8D1B794889EC578F42CC5A39B7AF7A21C5D0B72EE7BFC733134C357
          90522CEC27EB7682CF4B2A122695D68663FCD375CED4B518676281118B17F3B1
          783E1FC3D1B9108391954294544B1097EF9A440116BCD412E180F64D1E4D4DDC
          2236110DAD310A9D44C79C8CA3D3188BB6916C28AEB90F72A7482DF658D72DE6
          992618A339316720B47391666324348608B41BC2D13E17811EA31249D7DD5729
          60278FEE83FEA5F4CD091D063211CCA021439B9E114648D13A4BCC48D1655420
          A1C8F53705ECE22516B9FD6C9F96434353D88436031387A19541869619095A66
          C5D4C56866CC4AD03C2183BCC0658D0B88CD3F31F4A82F8CCC323230B1D42462
          E26946289A88C629111182260A7CD02542A4CA6998BB8228CD212BB7CA8F8E1B
          459B62128B381A196460344C05A37E3288EB2D740555A50F2E241CCBA0002B9E
          8D8DCD1E598EE3728526804E10CE4DAA371B18751381A825D877E3B404A52DFE
          10A7DBBD26B335C13D6B4BCF10DB93D1794E6BC5F5E768BA942689504786DAB7
          015C40FD243B8518376AFD1091EDB4EEE2BDDF91F99899154BB172F63A28084D
          170C26DF7243A5F63C6A4643E97790A27A5484F20E7F286EBA22385930E478EA
          803DD39B7D9BC5162CD1FA6CD4D18CA014C15868A6DD8654690F51A6602330E5
          F898AFEC7026EDEF36EBFEFB8F64C536D81B67F7DB4BEC3377B636BDFD2DC5E3
          FD017A12E1BD9A3AEF380000000049454E44AE426082}
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Version = '1.6.0.1'
        TabOrder = 0
        TMSStyle = 0
      end
      object btnServerStatus: TAdvSmoothPanel
        Left = 25
        Top = 14
        Width = 79
        Height = 24
        Cursor = crDefault
        Caption.Text = #26381#21153#22120
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Left = 20
        Caption.Top = 3
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.Picture.Data = {
          89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
          61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
          00097048597300000EC300000EC301C76FA864000002EB49444154384F6D9379
          48D36118C7B754A2C2ACFE294AA8209A27A6D2F24A2B21F398DB3CE635CD299E
          B9999A986487B6CCA384A0084AF3BEE62CCF16A678649E9BF38804BB88FA2735
          FCBBFEF9F6BCBF0D49EA810FCFEFE5FD7EBFCFFB8E77BCBF8A4F5864DDF194A8
          2ABC9F5EBE775A9F53E5FB8B75B6BEA4F690D2BEA559F74FF123725C0EA9CA7D
          7AD4CF24D0E9D5307E7D8295B566AEEBF425B85D2346D65DAF5EA9D2D996F4DB
          4C3653F115854287DCFB67561B07B231FFFD31463F5E856E3915BDEF2EE2E5FB
          140C7F2880F1DB4334BC524255E6F3439CE672847CA6105B0FDB1DCA32CF79ED
          580126BFA8D1B794889EC578F42CC5A39B7AF7A21C5D0B72EE7BFC733134C357
          90522CEC27EB7682CF4B2A122695D68663FCD375CED4B518676281118B17F3B1
          783E1FC3D1B9108391954294544B1097EF9A440116BCD412E180F64D1E4D4DDC
          2236110DAD310A9D44C79C8CA3D3188BB6916C28AEB90F72A7482DF658D72DE6
          992618A339316720B47391666324348608B41BC2D13E17811EA31249D7DD5729
          60278FEE83FEA5F4CD091D063211CCA021439B9E114648D13A4BCC48D1655420
          A1C8F53705ECE22516B9FD6C9F96434353D88436031387A19541869619095A66
          C5D4C56866CC4AD03C2183BCC0658D0B88CD3F31F4A82F8CCC323230B1D42462
          E26946289A88C629111182260A7CD02542A4CA6998BB8228CD212BB7CA8F8E1B
          459B62128B381A196460344C05A37E3288EB2D740555A50F2E241CCBA0002B9E
          8D8DCD1E598EE3728526804E10CE4DAA371B18751381A825D877E3B404A52DFE
          10A7DBBD26B335C13D6B4BCF10DB93D1794E6BC5F5E768BA942689504786DAB7
          015C40FD243B8518376AFD1091EDB4EEE2BDDF91F99899154BB172F63A28084D
          170C26DF7243A5F63C6A4643E97790A27A5484F20E7F286EBA22385930E478EA
          803DD39B7D9BC5162CD1FA6CD4D18CA014C15868A6DD8654690F51A6602330E5
          F898AFEC7026EDEF36EBFEFB8F64C536D81B67F7DB4BEC3377B636BDFD2DC5E3
          FD017A12E1BD9A3AEF380000000049454E44AE426082}
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Version = '1.6.0.1'
        TabOrder = 1
        TMSStyle = 0
      end
      object btnVer: TAdvSmoothPanel
        Left = 924
        Top = 6
        Width = 110
        Height = 24
        Cursor = crDefault
        Caption.Text = #29256#26412#65306'V2.0.4a'
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Top = 3
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Version = '1.6.0.1'
        Anchors = [akTop, akRight]
        TabOrder = 2
        TMSStyle = 0
      end
      object btnUser: TAdvSmoothPanel
        Left = 764
        Top = 6
        Width = 135
        Height = 24
        Cursor = crDefault
        Caption.Text = #35843#24230#21592#65306#19968#20108#19977#22235
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Top = 3
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Version = '1.6.0.1'
        Anchors = [akTop, akRight]
        TabOrder = 3
        TMSStyle = 0
      end
      object btnCurrTime: TAdvSmoothPanel
        Left = 1040
        Top = 6
        Width = 184
        Height = 24
        Cursor = crDefault
        Caption.Text = '2020-03-05 11:23:00'
        Caption.Location = plCenterRight
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Top = 3
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Version = '1.6.0.1'
        Anchors = [akTop, akRight]
        TabOrder = 4
        TMSStyle = 0
      end
    end
  end
  object PanelMain: TPanel
    Left = 0
    Top = 0
    Width = 1250
    Height = 696
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object PanelWork: TAdvSmoothPanel
      Left = 0
      Top = 0
      Width = 1250
      Height = 696
      Cursor = crDefault
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Caption.HTMLFont.Charset = DEFAULT_CHARSET
      Caption.HTMLFont.Color = clWindowText
      Caption.HTMLFont.Height = -11
      Caption.HTMLFont.Name = 'Tahoma'
      Caption.HTMLFont.Style = []
      Caption.Font.Charset = DEFAULT_CHARSET
      Caption.Font.Color = clWindowText
      Caption.Font.Height = -16
      Caption.Font.Name = 'Tahoma'
      Caption.Font.Style = []
      Fill.Color = 10053222
      Fill.ColorTo = 10053222
      Fill.ColorMirror = 10053222
      Fill.ColorMirrorTo = 10053222
      Fill.GradientType = gtVertical
      Fill.GradientMirrorType = gtVertical
      Fill.Opacity = 0
      Fill.OpacityTo = 0
      Fill.OpacityMirror = 0
      Fill.OpacityMirrorTo = 0
      Fill.BorderColor = clSilver
      Fill.Rounding = 0
      Fill.RoundingType = rtNone
      Fill.ShadowColor = clNone
      Fill.ShadowOffset = 4
      Fill.ShadowType = stSurround
      Fill.Glow = gmNone
      Version = '1.6.0.1'
      Align = alClient
      OnMouseUp = PanelWorkMouseUp
      TabOrder = 0
      Padding.Left = 1
      Padding.Top = 1
      Padding.Right = 1
      Padding.Bottom = 1
      TMSStyle = 4
      object ImageRecCli: TImage
        Left = 110
        Top = 368
        Width = 32
        Height = 32
        AutoSize = True
        Picture.Data = {
          055449636F6E0000010001002020000001000800A80800001600000028000000
          2000000040000000010008000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          2F1E180000000000BF986B00795F5400634A4100000000000000000000000000
          0000000000000000000000000000000000000000DFB8870032221B007C615700
          E1BE8D00654C4100674F4500E0BB8B00765D5300513D3300D6A87000DFBA8A00
          D7AA740037261F005841370046322900775D5100E0BA8800D9AC7600765C5100
          674F44005A42390032211A00785E53007B6155007A605600533A3100553E3600
          E8CCA400654D43006D544A00DEB7850034231C00D8AE7800DAB17D00ECD4B000
          694F4400D8AD7700573F3600E3C49800543C3200E9CDA7007C625800D7AB7400
          E3C39800E0BC8A00D7AC7600D7AC7400664D4300442F270047322A00755B5000
          7B615700553E3500533D3400E2C09100ECD4B200624A4000EDD8B500D5A87000
          ECD2AF005B43380069504600644C4300E6CAA200553D3500513A2F0079605500
          7A615500785D53003020190059423800654C4300755C50004A352D006E554A00
          EAD1AC00301F1900EED9B800D6A97100E1BD8D00E5C79E0069504500DFBB8A00
          72594E00573F35007C62560070574C00563E3300D7AA73003C282100D9AE7900
          6C534900E0BC8C00543D340045312800DFBA8B00D8AE77005F463C006B514700
          ECD5B300563E3300E4C69B00553C3300745B5000E1BE8E00442E2600664D4200
          49332B00E0BC8E00DFB98800E3C29700D7AA72007B625700D8AB7500533D3300
          E9CEA800E4C59A0058403600D8AC76006A504600EDD5B300DCB58200D9AF7A00
          59403700DEB8850047322A00E5C69A00573E3400563D33006D53490061493F00
          5C433900EDD7B50037251E00D8AD7600DBB38000E2BF8F00765D520033221C00
          684E44004F372E0072594E00E2C095005D4439005F473D0051393000422E2600
          5D443A006A51460061493F0061493F003E2B2400EBD2AD0070564B00E1BE9000
          6A51470071584D00EBD3AF00E0BB8A00E6C8A000E1BE8E00EDD7B500D6AA7300
          EAD0AA0034221C006F554A004D362D007A615600DEB98700E2C09300583F3500
          E0BD8E00EDD7B600ECD4B100412D2500D6A97100634A4000E3C39800795F5500
          6B524700553D3300543D3500EFD9BA004D372E0060473D00523C3200523C3300
          61483D005E453C00644B4000E3C3950073594F004F3A3100E5C79D00765C5100
          33211B00E4C69C005A4137005B443A0032211B00D9AD7700634B4100DEB88600
          EAD0AA00DFBB8900E4C69C00775E53005A423800D8AC7500E2C09300DFBA8800
          7C625700E0BC8B00563E3400E1BD8C007D635800D7AB74007A605500684F4500
          7B61560000000000000000000000000000000000000000000000000000000000
          00000000000017EBE7EB39391700000000000000000000000000001769706969
          69170000000065A4A4D8D8B5BB0000000000000000000000000000CA3D65B53D
          CA7D00000000A3A3000000F9544343AEC8C8AE25E7AEE7EBE7EBD24B4B000000
          3DD2000000009982000000CE7878CEA378F9F9F9F9F9A4D87BD88A8AB6000000
          4BD200000000E955A9A9329F99954E4E7CC6C672937272725E5E2DF98DC8C88D
          D88D00000000603860F3F3F3F3F4F44E954E4E4EC6C6C6C6727272CE78CEA3F9
          A35400000000B7B7000000B3B348ECAAAAF4F495524E4E2F7CC67C9F9F000000
          998200000000856D000000E0B79E7E7EECECEC35AA5151F44E4E4EF355000000
          E93200000000DFDB7D7DDBB4859E9E9E9E7E7E7E44EC84489A9AF4A78F30A955
          F35500000000A6A6A6A6DFDFDFAB42242E80CFCF90802EEE9DAB9EE0B7B7B7B3
          B78F00000000E1ED0000005CA6E2E2E2B2B2CDCDCDBEBEBECFCF9085E0000000
          E06D000000006BDD000000EDE19898984A4A4F929292B2CDCDCDCDDFDB000000
          B4CA00000000AF96D2D2DE538E7474E5E5E8E898A298984F4F4F92D4E4BBBBE4
          A6DB0000000061FE47FEFEFEAF63C3C3C3C3C37474E5E5E58989A2E1EDE1EDED
          D4E400000000BFBF000000B87558586481D9D98181969659DEDEDE8E8E000000
          28E400000000A531000000D7D7D786BFBFB89B9B6175FEFEFEFEFEAF59000000
          53DE000000006E385555F3A5A5F0F034F6F6CCCCEEEEEE424242429B3E54B664
          47D800000000BDBDBDBDC9C96EF8502AC2C2F0F6F6F691CCEEEEA0D786D7D7B8
          9B3E00000000C0C00000007A7A2773FA8080F8F8C27676F0F6F6CCA57F000000
          D79F00000000E3B40000007777ACAC8C8CCFFA73FA80F8F850C2766E38000000
          A56A000000008BDCDBDBDCE3E35A5AACACACAC8C8C8C73FAFAFAF87AEA6D8FEA
          BDEA00000000AD363656566C564AA2C33F3F4C97973F6374A2E25A7777C0C0C0
          7AE000000000ADF2000000AD365FC1C1BCBCBC6F6F6FC7C7C79797E3E3000000
          77B4000000003AED000000F2F29C9C5B5BD1D146C1C1C1BCBCBC6F56DF000000
          8BB40000000066622CDE62D6D6D0D0D0D05DA89C9C9C8787D1D146335CE4E4A6
          6CA600000000FFFFCB67FDFDFDDADADADADA71717171D0A8A8A8A868F2F2AD2B
          AD5C00000000F79400000057FFFF3CFFCBCB3BFDFDFD66D6D6D6D6D63A000000
          3AED000000004D290000004DF7F794FFF779FFFFFFFFFFFDCBFF3CFD40000000
          D66200000000FBFED9D9FEFBFB0000000000000000000000000000FF532CDE53
          6740000000001AFBFBFBFBFB1B00000000000000000000000000001AF7269479
          571B000000000000000000000000000000000000000000000000000000000000
          00000000FFFFFFFFC07FFE03C07FFE03CE000073CE000073C0000003C0000003
          CE000073CE000073C0000003C0000003CE000073CE000073C0000003C0000003
          CE000073CE000073C0000003C0000003CE000073CE000073C0000003C0000003
          CE000073CE000073C0000003C0000003CE000073CE000073C07FFE03C07FFE03
          FFFFFFFF}
        Visible = False
      end
      object PanelStatusWarn: TAdvSmoothPanel
        Left = 59
        Top = 469
        Width = 23
        Height = 29
        Cursor = crDefault
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Left = 20
        Caption.Top = 7
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.Picture.Data = {
          89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
          61000000097048597300000EC300000EC301C76FA8640000028049444154388D
          A593CD4B546114C67FF7BE736746476DFCC41CCD302C4CCC3E040377412D8AC2
          1691B6AC8D6EFA0BB2458B68D722AC45B48D682344B84943A240C2349B4A3219
          93B4A1D43B33CED79DFBF1DE1677CC9C961D389CC3E13CCF79CE7979154A6CE6
          765FBFDFAF9C43F88F298AD2E5BA6E14C79CCBE7DDF1DE913763A5FDCA76327E
          F368A4A922FCA0AAAEF9FCDE8E3EA8A885CA1A48EB90FAC9EA9769B6D6579EAF
          E5D2C3176ECDAFEE22981CE93B5C5F5731B5BFE364BDD6DC098965286C402E0B
          BE00EC6984CA168CD802CB0BAF7F7D4DA47B2FDF99F906A0F60CB5965587787C
          A0F354BD56D300B129D097219B0621C1CC406209BEBF2218A9E750CFE9867D95
          62745B817A371219AC6E6CEF56431AC4A3202D701D100E9816B8D28B0503E2F3
          A8E582BA968367276E9CB80AA0FA853BD0D8D4069B4B1E78DB4D0B5407A4E911
          DA1618066C2ED21C6943D3C415009F837A9C320772391080EDEC9CB820BDA801
          4AB16E67C13110C823003EC5B56BC96E7913EC623378537DC55CCA1D522941A6
          51027618C0276D5B279FA8412B825CBCBD2901BB45058A802D1D33E3A600D4BC
          E9BC97960116E0D81E584A702CCFA5B9730769795BA40B644DF911404D5885B1
          585C07BF5A3C96B11BE0CA22A9F547542C95444F659E0288C905B978A6DDEE6F
          0A05EA4428F417C8065C701C4F91AA8008904D66985B5C9D181C5DBA0E20F2F9
          BCD1DDCA5B69DB17AB34AD3C182A078A536D0B14157C1A8800C9F524EF56E29B
          53B3F181E9E5C23A780FC78B68EE47B84A3CF3FBD21D9994D1565B16440435D0
          2A00B032793EC5D6985DD35F4E7CD8B8746F22F1F99FCFB46D0FAF350F876BCB
          06FD8AD225841B963849C352A2BA9E7B32F4287EBFB4FFBFED3741283A8C2C2E
          BD520000000049454E44AE426082}
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Images = ImageListStatus
        Version = '1.6.0.1'
        Visible = False
        TabOrder = 0
        TMSStyle = 0
        object cxLabel4: TcxLabel
          Left = 600
          Top = 24
          AutoSize = False
          ParentFont = False
          Style.Font.Charset = DEFAULT_CHARSET
          Style.Font.Color = clWindowText
          Style.Font.Height = -13
          Style.Font.Name = 'Tahoma'
          Style.Font.Style = []
          Style.IsFontAssigned = True
          Properties.Alignment.Horz = taCenter
          Properties.Alignment.Vert = taVCenter
          Transparent = True
          Visible = False
          Height = 22
          Width = 145
          AnchorX = 673
          AnchorY = 35
        end
      end
      object PanelStatusNormal: TAdvSmoothPanel
        Left = 30
        Top = 504
        Width = 23
        Height = 29
        Cursor = crDefault
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Left = 20
        Caption.Top = 7
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.Picture.Data = {
          89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
          61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
          00097048597300000EC300000EC301C76FA864000002EB49444154384F6D9379
          48D36118C7B754A2C2ACFE294AA8209A27A6D2F24A2B21F398DB3CE635CD299E
          B9999A986487B6CCA384A0084AF3BEE62CCF16A678649E9BF38804BB88FA2735
          FCBBFEF9F6BCBF0D49EA810FCFEFE5FD7EBFCFFB8E77BCBF8A4F5864DDF194A8
          2ABC9F5EBE775A9F53E5FB8B75B6BEA4F690D2BEA559F74FF123725C0EA9CA7D
          7AD4CF24D0E9D5307E7D8295B566AEEBF425B85D2346D65DAF5EA9D2D996F4DB
          4C3653F115854287DCFB67561B07B231FFFD31463F5E856E3915BDEF2EE2E5FB
          140C7F2880F1DB4334BC524255E6F3439CE672847CA6105B0FDB1DCA32CF79ED
          580126BFA8D1B794889EC578F42CC5A39B7AF7A21C5D0B72EE7BFC733134C357
          90522CEC27EB7682CF4B2A122695D68663FCD375CED4B518676281118B17F3B1
          783E1FC3D1B9108391954294544B1097EF9A440116BCD412E180F64D1E4D4DDC
          2236110DAD310A9D44C79C8CA3D3188BB6916C28AEB90F72A7482DF658D72DE6
          992618A339316720B47391666324348608B41BC2D13E17811EA31249D7DD5729
          60278FEE83FEA5F4CD091D063211CCA021439B9E114648D13A4BCC48D1655420
          A1C8F53705ECE22516B9FD6C9F96434353D88436031387A19541869619095A66
          C5D4C56866CC4AD03C2183BCC0658D0B88CD3F31F4A82F8CCC323230B1D42462
          E26946289A88C629111182260A7CD02542A4CA6998BB8228CD212BB7CA8F8E1B
          459B62128B381A196460344C05A37E3288EB2D740555A50F2E241CCBA0002B9E
          8D8DCD1E598EE3728526804E10CE4DAA371B18751381A825D877E3B404A52DFE
          10A7DBBD26B335C13D6B4BCF10DB93D1794E6BC5F5E768BA942689504786DAB7
          015C40FD243B8518376AFD1091EDB4EEE2BDDF91F99899154BB172F63A28084D
          170C26DF7243A5F63C6A4643E97790A27A5484F20E7F286EBA22385930E478EA
          803DD39B7D9BC5162CD1FA6CD4D18CA014C15868A6DD8654690F51A6602330E5
          F898AFEC7026EDEF36EBFEFB8F64C536D81B67F7DB4BEC3377B636BDFD2DC5E3
          FD017A12E1BD9A3AEF380000000049454E44AE426082}
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Images = ImageListStatus
        Version = '1.6.0.1'
        Visible = False
        TabOrder = 1
        TMSStyle = 0
        object cxLabel1: TcxLabel
          Left = 600
          Top = 24
          AutoSize = False
          ParentFont = False
          Style.Font.Charset = DEFAULT_CHARSET
          Style.Font.Color = clWindowText
          Style.Font.Height = -13
          Style.Font.Name = 'Tahoma'
          Style.Font.Style = []
          Style.IsFontAssigned = True
          Properties.Alignment.Horz = taCenter
          Properties.Alignment.Vert = taVCenter
          Transparent = True
          Visible = False
          Height = 22
          Width = 145
          AnchorX = 673
          AnchorY = 35
        end
      end
      object PanelStatusDisable: TAdvSmoothPanel
        Left = 66
        Top = 504
        Width = 23
        Height = 29
        Cursor = crDefault
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Left = 20
        Caption.Top = 7
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.Picture.Data = {
          89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
          61000000097048597300000EC300000EC301C76FA8640000026549444154388D
          A593BB6B545110C67FE7ECBD7BF7918D4625A25888A0B1121F887F84F8406211
          5F58090A762A686150040511441014042D8440C4420B898508828D607C800404
          4550D44D88BB66B38F64EFCC589CEC82B1749A39CCF9BE99EF7CC3712C8A7397
          AFEFCE98DBE99CDBEABDDF2422EF456D5C357D72E5FCA9478BF1AE733871E6D2
          AABEA5A55B8524BBBB542C924DB278E75155E6E6DBD4666BCC365A8F2BB5DAF1
          DB5787BFFFD5E0ECF0B5813817BF58522AF5F7140B38E7C0C030CC0C0CD4945A
          BD4E65A6566ED5ABDBAF5FB9F815C0AF39309825F2234B97F4F6F7140B1886AA
          222A8808AA4AAA8A99D15328D0D75B5A99C4F99B1D05FEE0861D43B924BB259F
          2481204A2A8A6898AEAA98856622422EC91125B95D274F5F3802E04D6DA898CB
          07B0D125D882025143957067606614F239BCE72040A4AADBB2D90811C1CC301C
          8E90CD00043540173CC1887C842A9B0122D015C1AC0EC130B43BCD994355C181
          A98103E71DA96A1F40D4569B6EA7B23CF20B52C3A8EE162034320BDA440D9514
          D5B40AE0E75BADD7AD668B5414B5400E7E68378B1AA928ED3498DC68B648E7D3
          7700BED19A7F58A95650D5EEFA54155D20A469A8D331D784996A954ABD3E0290
          F9F69989751B56ED89E36C7F1C4541A2051F8227DA790F0033B559267F4E8F8D
          DEBB710A20D36C7E6DAF5EBBF125A68338574C92982EDA0C3143D53083CAEF2A
          E5C9A9A9A76FDFEC6FFCF8F20B2003F0F1C378F95BDCF3A0BF981F6836E7D67B
          E7F1DE61E61011E69A739427CBFC989C1E7B3E31B16FEAD5B34FFF7CA64EEC3D
          74EC6892490EE3D99CC9C4CB44DABF30DED61ACDFB4F46EFDC5D8CFFEFF803D7
          60A15C802BFFD10000000049454E44AE426082}
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Images = ImageListStatus
        Version = '1.6.0.1'
        Visible = False
        TabOrder = 2
        TMSStyle = 0
        object cxLabel2: TcxLabel
          Left = 600
          Top = 24
          AutoSize = False
          ParentFont = False
          Style.Font.Charset = DEFAULT_CHARSET
          Style.Font.Color = clWindowText
          Style.Font.Height = -13
          Style.Font.Name = 'Tahoma'
          Style.Font.Style = []
          Style.IsFontAssigned = True
          Properties.Alignment.Horz = taCenter
          Properties.Alignment.Vert = taVCenter
          Transparent = True
          Visible = False
          Height = 22
          Width = 145
          AnchorX = 673
          AnchorY = 35
        end
      end
      object PanelStatusError: TAdvSmoothPanel
        Left = 95
        Top = 504
        Width = 23
        Height = 29
        Cursor = crDefault
        Caption.Location = plCustom
        Caption.HTMLFont.Charset = DEFAULT_CHARSET
        Caption.HTMLFont.Color = clWindowText
        Caption.HTMLFont.Height = -11
        Caption.HTMLFont.Name = 'Tahoma'
        Caption.HTMLFont.Style = []
        Caption.Font.Charset = DEFAULT_CHARSET
        Caption.Font.Color = clWindowText
        Caption.Font.Height = -15
        Caption.Font.Name = #23435#20307
        Caption.Font.Style = []
        Caption.Left = 20
        Caption.Top = 7
        Caption.ColorStart = 15461355
        Caption.ColorEnd = 15461355
        Caption.Line = False
        Caption.TextWordWrapping = False
        Fill.Color = clNone
        Fill.ColorTo = clNone
        Fill.ColorMirror = clNone
        Fill.ColorMirrorTo = clNone
        Fill.GradientType = gtVertical
        Fill.GradientMirrorType = gtVertical
        Fill.BackGroundPictureMode = pmInsideFill
        Fill.BackGroundPicturePosition = ppCenterCenter
        Fill.BackGroundPictureAspectMode = pmNormal
        Fill.Picture.Data = {
          89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
          61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
          00097048597300000EC300000EC301C76FA864000002F049444154384F6D9359
          4854511CC6AFA9855BA5669433EAB8A5E39AE6B88CFB98E3B88FA3E3B88D8E3A
          2D6E2DDA4285E14B4F06D1B3AF8941D8831A5169A14181F9A0BD1419A949894B
          A92056B3DCAFFF993B89517FF871389CDFF7DD7BE15C6ED738108EEF2B35EA39
          5D79DFE79ACAA9AFB5BA5F0B55DAA94F3A4DDFDBB292323A77B27BFF8CC3B042
          219AD3550C2D1BF5D8EE6987E5F665E0EE555ABBB07DB30D4B866ACC6A4A8707
          3333C5E4EF1162C2384CA85411F3559A958DCE66F0BD17806E0370A51AE8D402
          5D55C0753DF85B6DF8DE61C0ACBA78792029494239A1A4422C769955974C6F5E
          32023D446705705E039CFB4319D0510AB411D7F4D8E8D0635AA97C44D17D8403
          37A954367DA9A327DD6820512DC8AD25C059A2A588D642A20038CD500117B598
          D794602C35A3890A1CB999BCDCA79B1D35142A2691026728C0E453F980310F68
          CE051A09430ED0A000EAB3B06E50E3B53C63D4F61633D98A35533BBDAA514932
          C1E44692EBB305F459406D0650930E54A7013A394CF539984C90AF50812B3793
          9A099E856C0192EBEC728D2033F8CA14409B0CBE3C11568D8CD624BC8A9599A8
          C08D9B8C977F3369994C54A580D72609908C7219509600A84F00A5F1E08B8F83
          2F8AC50F552CC6C363576D05CFA3E2C656F3536CCDB600C9FC2ED95A18036B41
          342C2A4614ACB991584A8DC68844FAC2F60903C111ED33F114627231C914B0E4
          4709B22A02D6BC08589452587242615610D92178132E459F4F600B15387301DC
          81838FFDA5EF1693236DB2F564182C8A633BB2253B18E6AC20983302614E0DC4
          4274201E1C0A7A46610FC276AD9DBABD8FC89E8842573FC684C09C19044BBA04
          E6B40098E50C7F98E47EF899E8870F61FE18F196AC19DD3C23598E85D9B016E7
          260FEFB0FB5E01A32F7D25588CF2C35682086699085B71BE580C1361FCB018F7
          3CC463FABDEE52E6DB733BC336ACD1E38EBB4F4BBF9B68E2A1BBEFFA90BB1F06
          DD8EAEF7BBFA4EF4BAF8B4D2F97EBBF7DF3F920D3B60779C7D9F27E1655FD95E
          B8FB7F0DC7FD0634FDC7E0E725325B0000000049454E44AE426082}
        Fill.PicturePosition = ppCenterLeft
        Fill.PictureLeft = 5
        Fill.PictureAspectMode = pmNormal
        Fill.BorderColor = clNone
        Fill.BorderOpacity = 0
        Fill.BorderWidth = 0
        Fill.Rounding = 0
        Fill.ShadowOffset = 0
        Fill.Glow = gmNone
        Fill.GlowGradientColor = 6307888
        Fill.GlowRadialColor = 15587527
        Images = ImageListStatus
        Version = '1.6.0.1'
        Visible = False
        TabOrder = 3
        TMSStyle = 0
        object cxLabel3: TcxLabel
          Left = 600
          Top = 24
          AutoSize = False
          ParentFont = False
          Style.Font.Charset = DEFAULT_CHARSET
          Style.Font.Color = clWindowText
          Style.Font.Height = -13
          Style.Font.Name = 'Tahoma'
          Style.Font.Style = []
          Style.IsFontAssigned = True
          Properties.Alignment.Horz = taCenter
          Properties.Alignment.Vert = taVCenter
          Transparent = True
          Visible = False
          Height = 22
          Width = 145
          AnchorX = 673
          AnchorY = 35
        end
      end
    end
  end
  object ImageListStatus: TcxImageList
    SourceDPI = 96
    FormatVersion = 1
    DesignInfo = 26345861
    ImageInfo = <
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000001000000070000000D0000001100000012000000110000
          000D000000070000000100000000000000000000000000000000000000000000
          0001000000040000000D0E241B691E4D39C226644AEF296D51FF256449EF1D4D
          38C20D241A6A0000000E00000004000000010000000000000000000000010000
          0005040907271D4D3ABC328063FF46AC8BFF4EC19DFF51CAA5FF4DC19CFF43AB
          89FF317F61FF1D4A38BD04090729000000050000000100000000000000040610
          0C35296A52EB42A181FF51C9A4FF4DC8A2FF4AC7A0FF47C59FFF49C6A0FF4CC7
          A1FF4FC8A2FF3F9F7EFF29674EEC040A082B0000000400000000010101102560
          49D747AA8AFF53CBA7FF4DC8A3FF4BC8A1FF4AC7A1FF4AC7A1FF49C6A0FF49C6
          9FFF4AC7A1FF50C9A3FF43A686FF1E4D3ABB0000000D00000001122C216C3F8E
          72FF57CDAAFF4FCAA4FF4DC9A4FF4DC9A4FF4DC9A3FF4CC8A3FF4CC8A3FF4BC8
          A2FF4AC7A1FF4BC8A2FF51CAA6FF3C8B6EFF0F271E6900000005245743BD52B4
          95FF56CDA9FF50CBA7FF51CBA6FF50CBA5FF4FCAA5FF4FC9A5FF4EC9A5FF4EC9
          A4FF4DC8A3FF4CC8A3FF50CAA4FF4DB091FF215340BF0000000A2F745AED5CC9
          A9FF55CEAAFF53CDA9FF52CDA8FF52CCA8FF52CCA7FF51CBA7FF50CAA6FF4FCB
          A6FF4FCAA6FF4FCAA5FF50CBA5FF56C6A5FF2C6E55EB0000000B368165FF68D3
          B5FF56CEABFF56CEABFF55CDABFF55CDABFF54CDAAFF53CDAAFF53CCA8FF52CC
          A8FF51CCA8FF51CBA7FF51CAA7FF61CFB0FF317A5DFA0000000B34785FEC73D2
          B7FF59D0AEFF58CFADFF57CFADFF57CFADFF57CFABFF55CEACFF56CEABFF55CE
          ABFF54CDAAFF53CDA9FF54CEA9FF6DCFB2FF2F735AEA0000000A285F4BBA73C3
          ACFF60D2B2FF5AD0AFFF59D0AEFF59D0AEFF59D0AEFF58D0AEFF57CFACFF57CF
          ADFF56CEABFF56CEABFF5ACFADFF6FC0A8FF265B48BC0000000715302662539F
          86FF7DDDC4FF5DD3B2FF5BD2B0FF5BD2B0FF5BD1B0FF5AD1B0FF5AD0AFFF59D0
          AEFF59D0AEFF59D1ADFF79DAC0FF509C82FF132D236200000003010201092F6F
          59D077C2ACFF84DFC7FF60D3B4FF5DD2B2FF5DD3B2FF5DD2B1FF5CD2B1FF5CD1
          B0FF5DD2B1FF81DEC4FF74BFAAFF275C48B50000000700000001000000010914
          102A3B8269EA6FBAA3FF97E4D0FF78DCC1FF66D7B7FF5FD4B3FF65D6B6FF77DB
          BFFF94E3CFFF6DB8A2FF377D65E9060D0A200000000200000000000000000000
          0001050C091B2A604DB2509E84FF84CCB7FF9AE1CFFFA5EBDAFF9AE1CFFF82CC
          B7FF4F9D83FF295F4CB3050C091C000000020000000000000000000000000000
          00000000000100000003122A22502454439B31705ACC3D8B6FF931705ACC2454
          439B122A22510000000400000001000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000001000000070000000D0000001100000012000000110000
          000D000000070000000100000000000000000000000000000000000000000000
          0001000000040000000D09053B6915097EC2190CA4EF1D0CB2FF180BA3EF1408
          7EC209043A6A0000000E00000004000000010000000000000000000000010000
          000503020F27170E7CBC201AC3FF1D2CE3FF1B34F4FF1B35FBFF1A31F4FF1B27
          E3FF1C16C1FF150A7ABD02010F29000000050000000100000000000000040504
          1A352319ABEB242EDCFF223FFBFF203DFCFF1F3CFBFF1F3AFBFF1E3AFBFF1D3A
          FAFF1D36F8FF1E27DAFF1E12A7EC0302112B000000040000000001000210231A
          9BD72B38E3FF2A47FDFF2744FDFF2642FCFF2542FBFF2441FBFF2341FCFF2240
          FBFF213EFBFF213BFBFF212DE1FF190F7EBB0000000D00000001120F476C3332
          D1FF2F4CFEFF2E4DFDFF2E4BFDFF2C49FCFF2C48FCFF2A48FDFF2A47FDFF2946
          FCFF2543FCFF2542FBFF2441FBFF2928CBFF0E0940690000000525218DBD3949
          EBFF3552FDFF3553FFFF3350FFFF324FFEFF304DFEFF2F4CFEFF2E4BFDFF2D4A
          FDFF2D48FCFF2C48FCFF2B45FDFF2D3BE7FF1F1887BF0000000A362FBCED3C55
          FAFF3C57FFFF3958FFFF3755FEFF3755FEFF3653FEFF3553FFFF3451FFFF3350
          FFFF324FFFFF304DFEFF2F4CFEFF2F48F7FF2C25B2EB0000000B3F3BD1FF4561
          FEFF415EFFFF405EFFFF3E5CFFFF3D5AFFFF3C5AFFFF3A57FFFF3A57FFFF3956
          FEFF3855FFFF3653FEFF3552FDFF3753FEFF342FC3FA0000000B3E3DC2EC526C
          FAFF4664FFFF4663FFFF4561FFFF4361FFFF415EFFFF415EFFFF405DFFFF3E5B
          FFFF3D5AFFFF3C59FFFF3A57FFFF435EFAFF3630BAEA0000000A333499BA5A6D
          F1FF4D6AFFFF4C68FFFF4B67FFFF4A66FFFF4965FFFF4764FFFF4563FFFF4561
          FFFF4360FFFF425FFFFF405EFFFF5060EFFF2E2B93BC000000071B1B4E62555C
          E5FF5F7BFFFF506DFFFF4F6DFFFF4E6AFFFF4E68FFFF4C69FFFF4B68FFFF4A68
          FFFF4A66FFFF4866FFFF5571FFFF4E52E0FF1817496200000003010103094042
          B3D06474F1FF6480FFFF5472FFFF526FFFFF526EFFFF526EFFFF516EFFFF4F6D
          FFFF4F6BFFFF5D79FFFF5D6DF0FF323294B50000000700000001000000010C0C
          202A4C51CEEA6372F0FF708BFEFF607DFFFF5976FFFF5572FFFF5873FFFF5D7A
          FFFF6B85FEFF606DEEFF474AC9E9070715200000000200000000000000000000
          00010707131B3B3D9CB25D64E9FF6C80F6FF758CFCFF7791FFFF738CFCFF6B7E
          F6FF5960E7FF383A9AB30707131C000000020000000000000000000000000000
          000000000001000000031A1B44503335889B4548B5CC5559E1F94447B4CC3235
          879B191A43510000000400000001000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000026231E60514640BD655A51ED72645AFF655A51ED5147
          40BD26211C610000000000000000000000000000000000000000000000000000
          000009070320514A42B67D7067FF8B7F77FF91877FFF948880FF91867DFF8B7E
          77FF796D65FF4E433BB709080322000000000000000000000000000000001210
          0E2E6E635CEA8A7E76FF968C84FF968C84FF968C84FF968B84FF958A81FF9489
          80FF958981FF857B71FF695E54EA0907032500000000000000000201010A645A
          54D390847CFF9A8E88FF998D85FF998D85FF978D85FF988C84FF968C84FF968C
          84FF968B84FF968B84FF8A7E76FF514641B70000000000000000302A2666857B
          73FF9C918AFF9C918AFF9B9189FF9B9089FF9A8E88FF998D85FF998D85FF998D
          85FF978C85FF968C84FF968C84FF7F746DFF2A252062000000005B534CB9968B
          84FF9E938CFF9E938CFF9E938CFF9D928BFF9C918AFF9C918AFF9B9189FF9B8F
          87FF998E85FF998D85FF998D85FF8F847BFF574E49BB000000007B7167EC9E94
          8CFFA09690FFA0948EFF9E948CFF9E938CFF9E928CFF9D928BFF9D928BFF9C91
          8AFF9B9189FF9C918AFF9B9087FF978D85FF71685FEA00000000867C74FFA198
          91FFA39891FFA29790FFA29790FFA09790FFA0968EFF9F958DFFA0948EFF9E92
          8CFF9E938CFF9D918BFF9D928BFF9C918AFF7D7167FA000000007D726BEBA298
          92FFA39993FFA29892FFA39891FFA29892FFA39891FFA29790FFA09790FFA096
          8EFFA0948EFF9F948DFF9E948CFF9B8F89FF766B64E900000000645B55B89E92
          8CFFA69C96FFA69B94FFA49A94FFA39A93FFA39993FFA29892FFA39891FFA398
          91FFA29790FFA29790FFA29790FF978D85FF5E5651B90000000034302C5F958A
          81FFA79D97FFA69C96FFA69C96FFA69B94FFA69B94FFA59B95FFA49A94FFA39A
          93FFA29892FFA29892FFA39891FF8F847BFF302B285E0000000002020106746A
          62CEA0968EFFA99F99FFA89F98FFA89E98FFA79D97FFA69C96FFA69C96FFA69B
          94FFA59B95FFA59B95FF9B9189FF5E5753B20000000000000000000000001613
          1327857B73E99F948DFFA99F99FFAAA09AFFAA9F9AFFA99F99FFA89E98FFA89D
          98FFA69C96FF9B9189FF81776DE80E0E0D1D0000000000000000000000000000
          00000D0C0A19655D55B1978D85FFA29892FFA89F98FFAAA09AFFA89E98FFA398
          91FF968C84FF635A53B10D0C0B1A000000000000000000000000000000000000
          000000000000000000002C29254E57504B99746B64CB91867DF9736B64CB564F
          49992B28254F0000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000010000
          000300000006000000080000000B0000000D0000000E0000000F0000000F0000
          00100000000F0000000E0000000C00000009000000050000000100000003081A
          1341133F2E921B543BBD1F5E42D3257553FF247353FF247251FF247151FF2370
          51FF23704FFF1C593FD6174E36C1113A29990717104900000006000000072A82
          60FF36A780FF34B186FF33B589FF34C192FF33BF90FF31BD8DFF30BC8CFF2FBA
          8AFF2EB989FF2CAA7DFF2AA378FF2A976FFF257353FF0000000D00000006297D
          5EE973D9BDFF5AD4B0FF43CCA1FF40C99DFF3EC79AFF3BC598FF38C295FF36C0
          92FF34BE90FF32BD8EFF3ABF93FF42BE95FF237152EE0000000C000000031845
          357B48A787FF74CEB4FF83E5CBFF57CEABFF2F9773FF207D5DFF1B7859FF2189
          66FF42BF98FF5AD1ADFF4BB996FF349674FF1540308800000006000000000000
          00030C221A3C256B53B345A987FF53AC90FF63A9B1FF71A7CCFF5D8CB7FF3674
          89FF2C8E73FF3A9D7BFF22654DBA0B2019460000000700000001000000000000
          00000000000100000003091B15322D7A61CC5588ABFF325994FF2C538FFF3460
          8EFF27735CCD091A143600000007000000030000000100000000000000000000
          00000000000000000000000000071221316B4572ACFF659FD7FF629CD6FF3968
          A5FF0C1B2C6F0000000800000000000000000000000000000000000000000000
          000000000000000000010000000E2B466DC379AFDAFF90CCF5FF77B4E8FF5991
          CBFF1D3A65CA0000000E00000001000000000000000000000000000000000000
          0000000000000101010315253F8A35598CF8BAE1F6FFBDE6FCFF8CC9F2FF69A5
          DBFF214479F70D19339501010103000000000000000000000000000000000000
          000000000000010101052A4A7DE2385F95FFD5F0FBFFD1EDFBFF94CFF3FF6DA7
          DDFF24467DFF162B59E201010105000000000000000000000000000000000000
          0000000000000101010538639EFA3C6CA8FFBAD7E9FF698EB7FF325A91FF2B50
          86FF28518EFE1C3869FA01010105000000000000000000000000000000000000
          00000000000001010104376398DC539CE0FF497BB7FF5390CDFF4E8FD3FF3C76
          C1FF396CB1FF223F72FF01010105000000000000000000000000000000000000
          000000000000010101021525374D4B83C2F17FB9E7FF86BDE9FF8DC4EEFF75A8
          DAFF5683B8FF1C3358BF01010104000000000000000000000000000000000000
          00000000000000000000010101020E16202B37618DB03D6A9FD24276B3FF3054
          85D51C3251930305071200000001000000000000000000000000000000000000
          0000000000000000000000000000000000010101010301010105010101050101
          0104010101030000000100000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000005656
          56FF5D5D5DFF5F5F5FFF5F5F5FFF636363FF616161FF606060FF5F5F5FFF5F5F
          5FFF5E5E5EFF595959FF565656FF515151FF434343FF00000000000000006F6F
          6FFF8F8F8FFF979797FF9A9A9AFFA4A4A4FFA2A2A2FFA0A0A0FF9F9F9FFF9D9D
          9DFF9C9C9CFF8F8F8FFF898989FF7F7F7FFF616161FF00000000000000007575
          75FFC5C5C5FFBCBCBCFFB0B0B0FFADADADFFABABABFFA9A9A9FFA6A6A6FFA3A3
          A3FFA1A1A1FFA0A0A0FFA3A3A3FFA4A4A4FF666666FF00000000000000007B7B
          7BFF939393FFBCBCBCFFD2D2D2FFB6B6B6FF818181FF696969FF656565FF7373
          73FFA6A6A6FFB9B9B9FFA3A3A3FF818181FF666666FF00000000000000000000
          00007C7C7CFF838383FF949494FF9A9A9AFFA2A2A2FFAAAAAAFF919191FF7272
          72FF7B7B7BFF888888FF777777FF646464FF0000000000000000000000000000
          00000000000000000000767676FF848484FF8A8A8AFF636363FF5D5D5DFF6666
          66FF7C7C7CFF696969FF00000000000000000000000000000000000000000000
          0000000000000000000000000000535353FF7B7B7BFFA6A6A6FFA3A3A3FF7171
          71FF434343FF0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000646464FFB3B3B3FFCFCFCFFFB9B9B9FF9999
          99FF525252FF0000000000000000000000000000000000000000000000000000
          000000000000555555FF4C4C4CFF646464FFE1E1E1FFE6E6E6FFCCCCCCFFABAB
          ABFF4F4F4FFF333333FF555555FF000000000000000000000000000000000000
          000000000000333333FF5E5E5EFF686868FFEFEFEFFFEDEDEDFFD1D1D1FFADAD
          ADFF505050FF3B3B3BFF333333FF000000000000000000000000000000000000
          000000000000333333FF6F6F6FFF757575FFD8D8D8FF949494FF636363FF5959
          59FF5B5B5BFF424242FF333333FF000000000000000000000000000000000000
          0000000000003F3F3FFF7C7C7CFFA4A4A4FF848484FF989898FF989898FF8282
          82FF777777FF484848FF333333FF000000000000000000000000000000000000
          0000000000007F7F7FFF828282FF949494FFBDBDBDFFC1C1C1FFC8C8C8FFAEAE
          AEFF8B8B8BFF4D4D4DFF3F3F3FFF000000000000000000000000000000000000
          000000000000000000007F7F7FFF8B8B8BFF949494FF8A8A8AFF7F7F7FFF6D6D
          6DFF5F5F5FFF4A4A4AFF00000000000000000000000000000000000000000000
          000000000000000000000000000000000000555555FF333333FF333333FF3F3F
          3FFF555555FF0000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000000000000090A0
          71F497A776FF97A776FF97A776FF97A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF97A776FF97A776FF97A776FF90A071F40000000000000000828F
          64DB97A776FF97A776FF97A776FF97A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF97A776FF97A776FF97A776FF828F64DB00000000000000002D32
          234C91A171F997A776FF97A776FF97A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF97A776FF97A776FF91A171F92D32234C00000000000000000000
          00001E2117326A7754B595A272FB97A776FF5F694BA11B1D152D1B1D152D5F69
          4BA197A776FF95A272FB6A7754B51E2117320000000000000000000000000000
          000000000000000000000A0B0811535C418D0404030600000000000000000404
          0306535C418D0A0B081100000000000000000000000000000000000000000000
          000000000000000000000000000000000000383E2C5F75845BC975845BC9383E
          2C5F000000000000000000000000000000000000000000000000000000000000
          000000000000000000000000000025291D3F92A474FA97A776FF97A776FF92A4
          74FA25291D3F0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000738059C397A776FF97A776FF97A776FF97A7
          76FF738059C30000000000000000000000000000000000000000000000000000
          0000000000000000000022251A3996A673FD97A776FF97A776FF97A776FF97A7
          76FF96A673FD21251A3800000000000000000000000000000000000000000000
          0000000000000000000051593F8897A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF51593F8800000000000000000000000000000000000000000000
          00000000000000000000262A1E4097A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF262B1E4100000000000000000000000000000000000000000000
          00000000000000000000181B132997A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF191C132A00000000000000000000000000000000000000000000
          00000000000000000000020302048A996CE997A776FF97A776FF97A776FF97A7
          76FF8B976AEA0404030600000000000000000000000000000000000000000000
          000000000000000000000000000021251A38757F5AC58F9F6FF58F9F6FF57683
          5DC825291D3F0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000009999
          99FF999999FF999999FF999999FF999999FF999999FF999999FF999999FF9999
          99FF999999FF999999FF999999FF999999FF999999FF00000000000000009898
          98FF999999FF999999FF999999FF999999FF999999FF999999FF999999FF9999
          99FF999999FF999999FF999999FF999999FF989898FF00000000000000009898
          98FF979797FF999999FF999999FF999999FF999999FF999999FF999999FF9999
          99FF999999FF999999FF999999FF979797FF989898FF00000000000000000000
          0000999999FF999999FF979797FF999999FF989898FF979797FF979797FF9898
          98FF999999FF979797FF999999FF999999FF0000000000000000000000000000
          00000000000000000000989898FF989898FF9F9F9FFF00000000000000009F9F
          9FFF989898FF989898FF00000000000000000000000000000000000000000000
          000000000000000000000000000000000000989898FF989898FF989898FF9898
          98FF000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000979797FF999999FF999999FF999999FF9999
          99FF979797FF0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000989898FF999999FF999999FF999999FF9999
          99FF989898FF0000000000000000000000000000000000000000000000000000
          00000000000000000000979797FF989898FF999999FF999999FF999999FF9999
          99FF989898FF999999FF00000000000000000000000000000000000000000000
          00000000000000000000989898FF999999FF999999FF999999FF999999FF9999
          99FF999999FF989898FF00000000000000000000000000000000000000000000
          00000000000000000000999999FF999999FF999999FF999999FF999999FF9999
          99FF999999FF999999FF00000000000000000000000000000000000000000000
          00000000000000000000999999FF999999FF999999FF999999FF999999FF9999
          99FF999999FF999999FF00000000000000000000000000000000000000000000
          00000000000000000000A7A7A7FF999999FF999999FF999999FF999999FF9999
          99FF979797FF9F9F9FFF00000000000000000000000000000000000000000000
          0000000000000000000000000000999999FF979797FF979797FF979797FF9999
          99FF979797FF0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000001000000050000000A0000000D0000000E0000000D0000
          000A000000050000000100000000000000000000000000000000000000000000
          0000000000030000000A0B0C3065191A67BF1F2285EE222492FF1F2085EE1719
          66BF0B0B30650000000B00000003000000010000000000000000000000000000
          000403040D241A1C67B83439A5FF515CC7FF606DD6FF6574DEFF5E6CD6FF4E5A
          C6FF3034A2FF181A65BA03030C25000000040000000100000000000000030606
          1632292B8FEA525BC1FF6574DBFF535DCBFF484FC0FF444CBEFF4750C1FF535E
          CBFF6371DAFF4851BDFF23278BEB04040E2700000003000000000101020D2629
          84D5636ECBFF6472D7FF464DBEFF999CD9FFDBDCF1FFFEFDFDFFDBDBF1FF999C
          DAFF484EC0FF626ED7FF515CC4FF1C1E69B80000000A0000000112143D694A52
          B8FF7887E0FF4A50BEFFD2D3EBFFFCFBFAFFFDFBF9FFFCFBFAFF73584EFFE2E1
          E0FFD5D5ECFF4B52C0FF6B7ADDFF3E46AFFF0F10366500000004262A79BB7681
          D5FF5964CBFFA1A2D8FFFBF8F5FFFCF8F6FFFBF8F6FFFBF8F6FF755950FFE1DF
          DDFFFBF8F6FFA2A3D9FF5761CBFF606CCEFF222574BD00000008363AA2EC92A0
          E7FF454CBCFFE1DEEAFFFAF5F2FFF9F5F2FFFAF5F2FFFAF5F2FF775C51FFE0DC
          D9FFFAF5F2FFE2DFEAFF464DBEFF7485DFFF2E3398EA000000093E45B4FFA1AF
          EEFF3F43B7FFF4EEEDFFF8F2EDFFE0DCD8FFE3E2E1FFE5E5E5FF785D53FFE0DC
          D8FFF7F2EDFFF4EEECFF4146B8FF8193E7FF363BA9FA000000093B42AAECA1AE
          EBFF454BBAFFDED8E2FF4945B2FF433CACFF3B33A5FF352B9EFF5B4770FFF8F7
          F6FFF6EFE9FFDED9E2FF464BBCFF8595E5FF343BA2EA00000008313685B9919C
          E1FF6770CCFF9492CBFFF5ECE6FFFEFDFCFFFFFFFFFFFFFFFFFFFFFFFFFFFEFD
          FCFFF4ECE5FF9593CDFF5D68CCFF7D8ADCFF2C3280BA000000061A1C44607982
          D6FFA5B3EBFF4246B7FFC5C0D7FFF5EEE7FFFBF8F5FFFEFDFCFFFBF8F6FFF5EE
          E7FFC6C0D7FF4247B7FF8E9FE7FF6B73CFFF17193F6000000003010102085A61
          AACF9FAAE8FF96A1E3FF4044B5FF8C89C6FFCDC6D7FFF0E6DFFFCDC6D7FF8C8A
          C7FF3F44B6FF8593DFFF949EE3FF464A8CB40000000600000001000000011112
          1E296F75C4E99AA5E6FFADBAEDFF6A74CCFF444AB8FF383CB0FF454AB8FF636C
          CBFFA1B0EBFF959EE3FF6269BDE90A0B141F0000000100000000000000000000
          00010A0B121A535894B18690DEFFACB7EDFFBDC9F5FFC5D2F8FFBDC8F5FFAAB6
          ECFF828BDCFF4E5491B2090A111B000000020000000000000000000000000000
          000000000001000000032527404F494D7F9A6168ABCC7780D4F96066ABCC474C
          7F9B23263F500000000400000001000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000B0A
          0A241A19195A00000001010101090303031504040420050505290505052A0505
          0524030303190101010B000000020D0D0C5A0505052400000000000000001B1A
          1A5A494747FF302E2EB41817176B282727AD343231DF353434F7302F2FF82525
          25E21B1B1BB7101010771C1C1CB6252525FF0C0C0C5A00000000000000000000
          0003323030B4464343FF5B5858FE8A8786FFADAAA8FFC5C2BEFFC0BDBAFF9E9C
          99FF716E6CFF393938FE252525FF1B1B1BB80101010500000000000000010202
          020F28272795737170FEB9B7B6FFCFC6BAFFBEB19EFFB7A68EFFB9AA91FFBFB3
          A2FFC4BCB1FF979491FF4A4949FF171717AB0303031B00000002010101051413
          13505A5757E7C1BFBFFFC9BEAEFFBFAB8BFFDECAA8FFF5DBB2FFF6DCB0FFE2CA
          A3FFCAB797FFC2B8AAFF979491FF363635F10E0E0E6B0101010B0101010A2C2A
          2A9A9A9998FFD7CEC0FFBAA37FFFEDE2CFFFFFE9C5FFFFE8C2FFFFE7BEFFFFE5
          BAFFF0D6ACFFC9B696FFC4BCB1FF716E6DFF1B1B1BB7030303190202020E3F3D
          3DCFCBC9C9FFBBAA91FFD9C39FFFFFFDF9FFFFF0D9FFFFEBCCFFFFEAC7FFFFE7
          C1FFFFE5BAFFE1CAA2FFBEB3A0FF9F9C99FF262525E2050505240202020F4C49
          49F1EFEFEFFFA48E6BFFF3E0C1FFFFFDFAFFFFF8ECFF535353FF464646FF3B3A
          3AFF2F2F30FFF6DCB0FFB7A790FFC1BEBAFF302F2FF80505052A0202020D4C49
          49F1F1F0F0FFA28A67FFF3E2C6FFFFF9F1FFFFFDFBFF605F60FFFFEED5FFFFEC
          CCFFFFE8C2FFF5DCB2FFB5A48BFFC6C3C0FF353434F705050529010101093E3C
          3CCBCECDCDFFBAA88EFFD6C2A3FFFFF5E7FFFFFDFAFF6B6B6BFFFFF7ECFFFFEF
          D8FFFFE9C5FFDDC8A6FFBDAF9AFFAEABA9FF343232DF04040420010101062D2B
          2B9A9D9C9CFFDAD0C2FFB29A76FFEADAC1FFFFF5E7FF767676FFFFFDFAFFFFFD
          F9FFEDE1CEFFBDA887FFCEC6B9FF8A8887FF292828B503030315000000022725
          259F686666FFC9C8C8FFCBBEAAFFB29B76FFD6C2A3FFF3E1C6FFF3DFC1FFD8C2
          9EFFB8A07CFFC9BDABFFBAB8B7FF5F5C5CFF202020AF01010109000000003533
          33D6939191FF7D7C7CFFC9C8C8FFDAD0C2FFBAA88EFFA28B66FFA28C68FFBAA9
          8FFFD7CEC0FFC2C1C1FF787575FF807F7FFF242423D800000001000000003B39
          39D5CCCBCBFFADACACFF797777FF9D9C9CFFCECDCDFFF1F0F0FFF0F0F0FFCCCB
          CBFF9B9A99FF726F6FFF9A9999FFC1C1C1FF2A2929D600000000000000002725
          2580848382FFCDCCCCFFADACACFF5E5C5CFF424141E54C4949F14C4949F14744
          44E7636161FFA6A5A5FFC9C8C8FF81807FFF1D1C1C8000000000000000000808
          081A272525803B3A39D5353534D51D1C1C82060606230202020D0202020F0A09
          092628272785434141D6424040D5252424800706061A00000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0002000000070000000B0000000C0000000C0000000C0000000C0000000D0000
          000D0000000D0000000D0000000C000000080000000200000000000000000000
          00062D257ABE3F32A9FF3E32A9FF3D31A8FF3C2FA7FF3B2FA6FF3A2DA6FF392D
          A5FF392CA5FF382BA3FF3729A3FF271D75C00000000800000000000000000000
          00094439ADFF6373DEFF5566DAFF5464D9FF5A6CDCFF6271DDFF6373DEFF5E6F
          DCFF5566DAFF4B5CD7FF5060D8FF3A2DA6FF0000000C00000000000000000000
          0009483FB1FF6D7DE1FF5A6BDBFF6572DAFF5655C2FF4E48B8FF4F48B8FF5554
          C2FF6270DAFF5F6FDDFF5869DBFF3F33A9FF0000000C00000000000000000000
          00084D44B5FF7787E3FF636FD9FF5149B7FFA9A1D0FFE6DFE4FFE5DFE3FFA9A1
          D1FF5049B7FF636FD9FF6979E0FF4338ADFF0000000B00000000000000000000
          00075149B8FF8192E6FF5655C1FFA39BCDFFF3EDE4FFF3EDE5FF755A51FFF3EC
          E4FFA59DCDFF5452C0FF7786E3FF473EB1FF0000000A00000000000000000000
          0007564FBDFF8C9BE9FF4941B2FFE4DBDDFFF6F0E9FFFEFDFCFF785D53FFF6F0
          E9FFE4DCDCFF483FB2FF7F90E6FF4B42B4FF0000000900000000000000000000
          00065A54C0FF95A3ECFF473FB2FFE3D9D9FF3F38A8FF3B32A4FF564377FFFDFC
          FBFFE2D9D8FF463EB1FF8594E8FF5048B8FF0000000900000000000000000000
          00055F5AC4FFA0AFEFFF5857C1FF9E94C4FFFDFCFBFFFFFFFFFFFFFFFFFFFDFD
          FBFF9E93C4FF5755C0FF8798E8FF544EBBFF0000000800000000000000000000
          00056561C8FFB0BEF2FF7787DFFF4B41B1FFB4ABD1FFF8F5F5FFF8F5F5FFB4AB
          D1FF4B41B1FF7A8ADFFF90A2EBFF5B55C1FF0000000700000000000000000000
          00046D6BCFFFB8C7F4FF879CEAFF7F90E3FF5A59C1FF4841B1FF4941B2FF5B5A
          C1FF8191E3FF8094E8FF97AAEDFF6360C6FF0000000700000000000000000000
          00047271D1FFBFCCF5FFBECCF6FFBCCBF5FFBCCBF5FFBBCAF5FFBACAF5FFBAC8
          F5FFB9C8F4FFB8C6F4FFB7C5F4FF6A66CBFF0000000600000000000000000000
          00037777D6FF7576D7FF7474D5FF7171D4FF6F6ED2FF6D6CD0FF6B68CEFF6966
          CCFF6663CAFF6360C8FF615DC7FF6F6CCEFF0000000500000000000000000000
          00028282DBFFC5C5EFFFFEFEFEFFFDFCFCFFFCFAF9FFFAF8F6FFF8F5F3FFF7F2
          F0FFF6F0ECFFF4EDE9FFF2ECE7FF7979D4FF0000000400000000000000000000
          00016264A6BF8587E0FF8486DFFF8485DFFF8385DDFF8284DEFF8284DCFF8283
          DCFF8083DCFF8082DCFF8081DBFF5E5FA2C00000000300000000000000000000
          0000000000010000000200000002000000020000000200000003000000030000
          0003000000030000000300000003000000020000000100000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000100000004000000070000000A0000000B0000000A0000
          0008000000040000000100000000000000000000000000000000000000000000
          000000000002000000080F0B0960211612BD291D17ED2E1F19FF291D17ED2016
          12BD0F0A08610000000900000003000000000000000000000000000000010000
          000304030220211813B635251EFF3F2E26FF45322AFF47332BFF453229FF3F2C
          25FF33231DFF201511B704030222000000030000000000000000000000020705
          042E2E211AEA3E2D25FF49362DFF4A362DFF48362DFF48352DFF48352CFF4735
          2BFF47342CFF3B2B22FF2B1E18EA0403022500000002000000000100000A2B1E
          18D3423129FF4C3830FF4B382FFF4B372FFF4A372EFF4B362DFF49362DFF4936
          2DFF48352DFF48352DFF3E2D25FF211613B70000000800000001140E0B663C2C
          23FF4E3A32FF4E3A32FF4D3A31FF4C3931FF4C3830FF4B382FFF4B382FFF4B37
          2EFF4A362EFF4A362DFF49362DFF372720FF110C096200000003271D17B94835
          2DFF503C33FF503C33FF503B33FF4F3A32FF4E3A32FF4D3A32FF4C3A31FF4D39
          30FF4B392FFF4B382FFF4B372FFF433128FF241A15BB0000000637271FEC503D
          34FF523E36FF513D35FF503D34FF503C33FF503C34FF4F3B32FF4F3A32FF4E3A
          32FF4E3A31FF4D3A32FF4D3A30FF4A372EFF30221CEA000000073D2C24FF5340
          37FF544037FF543F36FF533F36FF523F36FF523E35FF513E34FF513D35FF503C
          34FF503C33FF4F3B33FF4F3A32FF4D3A32FF36271FFA000000083A2922EB5440
          38FF554139FF554138FF544037FF544038FF543F37FF533F36FF523F36FF523E
          35FF513D35FF513D34FF503D34FF4E3931FF34251EE9000000062D211BB8503C
          34FF58433BFF58433AFF56423AFF564239FF564139FF554138FF544037FF5440
          37FF543F36FF533F36FF533E36FF4A372FFF291E18B90000000518120E5F4835
          2CFF59443CFF58443CFF58433BFF58433AFF58433AFF57433AFF56423AFF5642
          39FF554138FF554038FF544037FF433128FF150F0C5E00000002010100063628
          20CE513E35FF5B473EFF5A463DFF5A453DFF59453CFF58443CFF58443BFF5843
          3AFF57433AFF57423AFF4D3A31FF2B1F1AB20000000400000000000000010A07
          0627403026E9513C34FF5B473EFF5C473FFF5C463EFF5B463EFF5A453DFF5A44
          3DFF58443CFF4D3A31FF3D2D23E80605041D0000000100000000000000000000
          00010604031930231CB14A372EFF554138FF5B463DFF5D473FFF5A453DFF5440
          37FF49362DFF2F221BB10604031A000000010000000000000000000000000000
          0000000000010000000215100C4E2A1F1999382921CB45332AF9372921CB291E
          1899140F0C4F0000000300000001000000000000000000000000}
      end>
  end
  object MainBarManager: TdxBarManager
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
    ImageOptions.Images = ImageListBarManager
    ImageOptions.StretchGlyphs = False
    LookAndFeel.NativeStyle = False
    LookAndFeel.SkinName = 'Office2010Blue'
    PopupMenuLinks = <>
    Style = bmsOffice11
    UseSystemFont = True
    Left = 520
    Top = 192
    PixelsPerInch = 96
    object MainToolbar: TdxBar
      AllowClose = False
      AllowCustomizing = False
      AllowQuickCustomizing = False
      AllowReset = False
      BorderStyle = bbsNone
      CaptionButtons = <>
      Color = 6373424
      DockedDockingStyle = dsTop
      DockedLeft = 0
      DockedTop = 0
      DockingStyle = dsTop
      FloatLeft = 886
      FloatTop = 2
      FloatClientWidth = 64
      FloatClientHeight = 648
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -15
      Font.Name = 'Microsoft YaHei UI'
      Font.Style = []
      ItemLinks = <>
      NotDocking = [dsNone, dsLeft, dsTop, dsRight, dsBottom]
      OneOnRow = True
      Row = 0
      ShowMark = False
      UseOwnFont = True
      UseRecentItems = False
      Visible = False
      WholeRow = True
    end
    object pmPTTDial: TdxBarLargeButton
      Caption = #32452#21628
      Category = 0
      Hint = #32452#21628
      Visible = ivNever
    end
    object pmStartRealPlay: TdxBarLargeButton
      Caption = #24320#22987#30417#25511
      Category = 0
      Hint = #24320#22987#30417#25511
      Visible = ivAlways
    end
    object pmStopRealPlay: TdxBarLargeButton
      Caption = #32467#26463#35270#39057
      Category = 0
      Hint = #32467#26463#35270#39057
      Visible = ivAlways
    end
    object pmRecvVideoPlay: TdxBarLargeButton
      Caption = #25509#25910#35270#39057
      Category = 0
      Hint = #25509#25910#35270#39057
      Visible = ivAlways
    end
    object pmP2PDial: TdxBarLargeButton
      Caption = #21628#21483
      Category = 0
      Hint = #21628#21483
      Visible = ivAlways
    end
    object pmiLockWindow: TdxBarButton
      Caption = #38145#23631
      Category = 0
      Hint = #38145#23631
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000010000000100803000000282D0F
        530000000467414D410000B18F0BFC6105000000206348524D00007A26000080
        840000FA00000080E8000075300000EA6000003A98000017709CBA513C000002
        31504C5445999B97B1B2AEB6B8B4B6B7B4B2B3AF9C9D98B2B3ADFFFFFFA7AAA5
        000000949592A1A29EA6A8A4A2A39F989893A3A5A1D9DAD8D9DAD8A4A5A1A9AB
        A7A8AAA6A9AAA7A9AAA6A9ABA6A8A9A5A7A9A59E9F9BC7C8C5D1D2CFD2D3D0D0
        D0CDC5C6C3C6C7C5CFD0CDD1D2CFD0D1CFC6C7C59D9F9B8485807F817C838581
        8385807577738686849899967D7F7A82847F84868180827D8587833D3D3BA3A4
        A1BABBB9BCBCBAA3A4A03C3D3A000000000000464644A4A5A2C2C3C1C1C2BFC0
        C0BEBFC0BEBFC0BEC0C1BFC2C3C0A4A6A24547430000002F302E60615D525450
        494A4743444140413ED1D1D0CCCCCBCCCDCBCBCBCACBCCCACCCCCAD0D0CED2D2
        D04C4C4C4F4F4E5050505151505252525E5E5D5D5D5D5252515051504E4E4E4B
        4B4BCDCDCC4A4A49626261646464656665959595919190646564636362444444
        565656585958606161BABBBB9D9F9EA1A2A1B3B5B45C5D5D5152514545443939
        383F3F3E4B4C4A50514E7A7454C1B788AAA174ACA376BDB27E67603A2C2D2A21
        222028292840413F4A4A41AEA15AF2E693F0E48DEBDE7FE7D8759D8F41323229
        2728262C2C2B333332353634414137A89731F4E24FEBD736E3CE249D8C273A3A
        322D2E2D2F2F2ECDCDCB29292825262430302A7E6E17BBA525BCA72FB8A323B5
        9E117B6B1430312BCBCBC9DDDDDB8989887E7E7D7F807E86847B8B87768A8776
        85847A7D7D7B878786DBDBD9C6C7C5C6C7C6C7C8C6C8C9C79E9E9CA1A1A0A6A6
        A4ACACAAD5D5D4DEDEDCDEDEDDDFDFDED8D8D7FFFFFFB79A3A530000004C7452
        4E530000000000000000000014657F68166EF8FA718C8C8B8C8C8D8D64DDE3E2
        E4FCFCE4E2E3DE68081D212B69F0EF6A2C211E090D93EBEC930D01082ACAE8E7
        E7E8E7E7E8CA2A020A262D33373A9A28293700000001624B474407166188EB00
        00000774494D4507E5010F06230AB5AFFBBA000001064944415418D363606064
        628603261656062E6E1E24C0CBC7C02FE0E3EBE7EBEBEF1F1018E01F2428C420
        1C1C121A161E1119151D13161B1724C2201A9F9098949CE2EB979A9C969EE02F
        C6201C9F9199959D939B975F5058541C081228292D2BAFA8ACAAAEA9ADABF705
        091437343635B7B4B6B5777476015588C777F7F4F6F5F74F983869F294A90140
        8169D367CC9C357BCEDC79F317CC98BE508C4162D1E2254B972D5FBE62F9CA25
        AB56AF91649092969195935FBB6EFD060545256515550635750D4D2DED8D9B36
        6FD1D1D5D33730646063373236D9BA6DDBF61D3B4DCDCCD939182C2CADAC6D6C
        EDECED1D1C9D9C5D2C2D18385DDDDC3D3CBDBCBDBD3C3DDCDD5C39019B3F52BE
        A4D5020B0000002574455874646174653A63726561746500323032302D30372D
        31395430333A33393A31392B30303A30309D704C6C0000002574455874646174
        653A6D6F6469667900323031392D30312D30385431363A33363A32352B30303A
        3030B8B5BA930000002074455874736F6674776172650068747470733A2F2F69
        6D6167656D616769636B2E6F7267BCCF1D9D00000018744558745468756D623A
        3A446F63756D656E743A3A50616765730031A7FFBB2F00000017744558745468
        756D623A3A496D6167653A3A48656967687400333228F4F8F400000016744558
        745468756D623A3A496D6167653A3A5769647468003332D05B38790000001974
        4558745468756D623A3A4D696D657479706500696D6167652F706E673FB2564E
        00000017744558745468756D623A3A4D54696D650031353436393635333835F9
        7CECE200000011744558745468756D623A3A53697A65003133383342A94F6155
        00000056744558745468756D623A3A5552490066696C653A2F2F2F646174612F
        777777726F6F742F7777772E6561737969636F6E2E6E65742F63646E2D696D67
        2E6561737969636F6E2E636E2F66696C65732F342F34313039382E706E675863
        8A490000000049454E44AE426082}
      OnClick = pmiLockWindowClick
    end
    object dxBarSeparator1: TdxBarSeparator
      Caption = 'New Separator'
      Category = 0
      Hint = 'New Separator'
      Visible = ivNever
      ShowCaption = False
    end
    object pmiLogout: TdxBarButton
      Caption = #27880#38144
      Category = 0
      Hint = #27880#38144
      Visible = ivNever
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000010000000100803000000282D0F
        530000000467414D410000B18F0BFC6105000000206348524D00007A26000080
        840000FA00000080E8000075300000EA6000003A98000017709CBA513C000002
        BB504C544500000055BEE359C4E656BFE353BBE34CB2DB59C2E95BC4EA5DC8EC
        56BEE55BC5EA54BCE455BEE551B8E44EB5DF5CC7EF53BAE453BBE54FB6E05CC6
        EE51B8E34FB4E452B8E65FCCF44EB4E267D8FF51B5E650B0FF52B9EC4CADE8FF
        FFFF4FB6EF49A9E73991E73E99E64FB7F44DB3F046A8ED419BE33F9BE6429FE5
        4BAFEE3F9EEC3D97E63E9AE844B5FF47A6EA44A4EB3D96E5409DE9419EE73E99
        E73D97E744A7F03486DF3E9AE93C95E743A6EF388DE33C95E63A93E83A91E63E
        9CEC44AEF63B95E8388CE43A92E758C2E55CC7E95AC5E754BDE15AC3E962CFF2
        4FB5DE56BFE558C0E653BADD55BDEA5DC9F44AADDE57BFE558C0E651B8DF54BC
        E660CCF25FCBF153B9DF4EB3EB54BEF443A0DD58C2E962CFF459C3EB4DB2DE4F
        B4E25BC6F060CCF451B6DF46A8EB4AB2F43D95DE56BFEA61CFF755BCE954BBEB
        5FCEFA60C8F24EB1DC409FEC42A9F5388DDF57BCE965D0FA59C4F24DAFE145A5
        DD55BEF15FCCFA57BBEB3D9AEA42A9F340A2F0368AE34FB2E35DC4F35AC7F84F
        B4E847A7E152BCF455BFF64DAEE5378DE6368AE3378BE4348BEB51B6ED4CB1EB
        46A4E24FB8F44FB7F547A5E34AAEEC4AADEB419CE04DB4F34BB5F845A5E93F96
        DB48AEF04FBAFA48AAEA3687D449ACEF44A8F03F97E042A1E946AFF74BB3F645
        A3E742A0E848B0F643AEFB40A3F03E9BE93D94E23E97E53F9EEB41A8F646A8EE
        3689D93687DC42A2EC45B0F93EA6F73DA3F43DA4F53EA8FA45AAF23F99E7398D
        E2409FED43ABF642AFFA42A6F13D98E8337FDE3C97E93FA1EF40A7F441A9F541
        A9F440A5F23E9CEC398FE52A64D03687E23789E33788E23482DF69D9F964D4FC
        59C8FC66D7FD4DBAFC66D5FC65D5FF44B1FD63D3FE5DCDFE5FCFFF59C9FF58C6
        FD57C4FC54C3FF52C0FC53C0FC4EBDFF4DBBFE4BB8FD48B5FD49B8FF45B4FF44
        B1FE48B4FC41B0FF3FABFD40AEFF44B2FD41B0FD3FAEFF3EADFF40AFFEFFFFFF
        9F3662F6000000C774524E530000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000003388690977DB192F6B0A7ADF1927560A34
        CAE31B78DE1670F27D0919BEE21B78DE157AF25F67FACD127AE01665FCC4160E
        B3F9765DDFB41121CBF24B1ED9E52C081912018C7223DFE1217F7817C9EE4508
        A4F9610490A41047E8E22D3AE7F4A1411B256CD29505057AF4ECDEE2F8C62B0A
        7AE7F9BB33053A8DC4DCD3AB631603151D1A0BB4D9CBC300000001624B47441E
        720A202B0000000774494D4507E5010508342B9966A21C0000011B4944415418
        D3011001EFFE00000001010102434445460301010000000000040506070847C7
        4849090A0B0C0000000D0E0F4A4B4C4DC84E4F50515210110000121353545556
        57C958595A5B5C5D141500165E5FCA606162CB636465CC6667171800196869CD
        6A6B6CCE6D6E6F70CF71721A007374D075761B7778797A7B7CD17D7E1C007F80
        D281821D838485861E87D3D4881F00898AD58B8C2021212122238DD6D78E2400
        8F90D8919225262727289394D9959629009798DADB999A2A2B2C2D9B9CDC9D9E
        2E002F9FA0DDA1A2A3A4A5A6A7DEDFA8A9270030AAABACE0E1ADAEAFB0E2E3B1
        B23132003334B3B4B5E4E5E6E6E7B6B7B835362200003738B9BABBBCBDBEBFC0
        C1393A3B000000003C3D3EC2C3C4C5C63F40414200002C7B6A492B37A1350000
        002574455874646174653A63726561746500323032302D30372D31395430333A
        33393A31392B30303A30309D704C6C0000002574455874646174653A6D6F6469
        667900323031392D30312D30385431363A34353A35342B30303A3030D60B16BE
        0000002074455874736F6674776172650068747470733A2F2F696D6167656D61
        6769636B2E6F7267BCCF1D9D00000018744558745468756D623A3A446F63756D
        656E743A3A50616765730031A7FFBB2F00000017744558745468756D623A3A49
        6D6167653A3A48656967687400333228F4F8F400000016744558745468756D62
        3A3A496D6167653A3A5769647468003332D05B38790000001974455874546875
        6D623A3A4D696D657479706500696D6167652F706E673FB2564E000000177445
        58745468756D623A3A4D54696D650031353436393635393534364227EF000000
        11744558745468756D623A3A53697A6500313437324226DF2F90000000587445
        58745468756D623A3A5552490066696C653A2F2F2F646174612F777777726F6F
        742F7777772E6561737969636F6E2E6E65742F63646E2D696D672E6561737969
        636F6E2E636E2F66696C65732F35302F3530353033372E706E67D1270D3D0000
        000049454E44AE426082}
      UnclickAfterDoing = False
      OnClick = pmiLogoutClick
    end
    object pmiSetPwd: TdxBarButton
      Caption = #20462#25913#23494#30721
      Category = 0
      Hint = #20462#25913#23494#30721
      Visible = ivAlways
      Glyph.SourceDPI = 96
      Glyph.Data = {
        89504E470D0A1A0A0000000D4948445200000010000000100803000000282D0F
        530000000467414D410000B18F0BFC6105000000206348524D00007A26000080
        840000FA00000080E8000075300000EA6000003A98000017709CBA513C000001
        59504C54450000000061800C69876EA8BB6FA9BC066583086784086785036382
        1B738E18718C18718D00607F026381005D7D005B7B277A94267A94005A7A0162
        8104648231819A1D749024799436859D000939056483005F7E1C749000567700
        001600476B004F713E89A272AABD85B6C773AABD3F8AA20042665597AC99C1CF
        8DB9C879ADBD5698AD19728D89B7C56EA5B6106C880052738AB7C61A728E146F
        8B7DAFBE7DAFBE3B87A019728E19718E3A879F7DAFBE156F8B002A54418AA18B
        B8C69CC3D0408AA00029531F7590619EB11E759000244F005C7C86B5C3005879
        8CB9C68BB8C600537500597A91BCC997C0CD21779202638205658300597993BE
        CB66A3B54C92A822789298C1CE67A3B5448DA418718D0058799DC5D16AA5B748
        90A61D7590005778A1C7D36DA7B95799AD267A94004A6D67A4B66BA6B8005F7E
        00000000375E002751A2C7D3ACCCD6B0D0D9B5D3DCBCD8E0BEDAE2FFFFFF13DC
        BF9B0000006C74524E5300000000000000000000000000000000000000000000
        000000000000000005120C5EACCDAC5E0893FDF6DB9333F2D24014F2332DECDE
        5F2D2D5FEB2D0479F2ED780440B7400425E116D9D91517D8F14D110517D7E1C5
        48D7EFD73C17D7F3DE4017D9CAAA3E0996A11D0208088A8BC27100000001624B
        474472360E0D580000000774494D4507E5010F06272AEAAD1E76000000C44944
        415418D36360606064626496939797630132184080954D41514959594555819D
        152CC0A1A6AEA1A9A5A5A9A1ADC60116E0D4D1D5D33730D0D73334E2040B7019
        9B989A995B589A5A59738305786C6CED72ECED73EC1C1C79C002BC7C8E4ECEB9
        B9CE2EAE7CBC0C1053F905DCDCDDDD048520660201238787A79737A3B0304C80
        41C4C7D7CF3F2090152110149C17121A268A24109E1F1119258610888E29888D
        8B1747082424162625A748C0057853D3D233321924E10252D259D932B28C20DF
        030017CF1E99D5CA46D00000002574455874646174653A637265617465003230
        32302D30372D31395430333A33393A32302B30303A303086670E5C0000002574
        455874646174653A6D6F6469667900323031392D30312D30385431373A31353A
        33342B30303A3030BCA56AFE0000002074455874736F66747761726500687474
        70733A2F2F696D6167656D616769636B2E6F7267BCCF1D9D0000001874455874
        5468756D623A3A446F63756D656E743A3A50616765730031A7FFBB2F00000018
        744558745468756D623A3A496D6167653A3A48656967687400313238437C4180
        00000017744558745468756D623A3A496D6167653A3A576964746800313238D0
        8D11DD00000019744558745468756D623A3A4D696D657479706500696D616765
        2F706E673FB2564E00000017744558745468756D623A3A4D54696D6500313534
        36393637373334C08F65E800000011744558745468756D623A3A53697A650034
        38383342B6AE692400000058744558745468756D623A3A5552490066696C653A
        2F2F2F646174612F777777726F6F742F7777772E6561737969636F6E2E6E6574
        2F63646E2D696D672E6561737969636F6E2E636E2F66696C65732F35342F3534
        373030332E706E67BEFCB45D0000000049454E44AE426082}
      OnClick = pmiSetPwdClick
    end
  end
  object dxSkinController1: TdxSkinController
    Kind = lfStandard
    NativeStyle = False
    ScrollbarMode = sbmClassic
    SkinName = 'Office2010Blue'
    Left = 524
    Top = 264
  end
  object ImageListBarManager: TcxImageList
    SourceDPI = 96
    FormatVersion = 1
    DesignInfo = 12452181
    ImageInfo = <
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000000000000000000000907030E7D6226BBA88334F9A78333F9795F
          25B50806020C0000000000000000000000000000000000000000000000000000
          000000000000000000000000000001010001251D0D32977839C580662CB5211A
          0B2E010100010000000000000000000000000000000000000000000000000000
          0000000000000000000000000000110D051B392C0F59957530D5846727C9362A
          0E55100C04190000000000000000000000000000000000000000000000000000
          00000000000000000000372B1053806325C370571FAE54421785564218867258
          20B07F6325C234280F4E00000000000000000000000000000000000000000000
          000000000000322710488B6D2CCC3D30135A362B15455846227353421E722F25
          0F433F31135E8B6D2CCC2E240F43000000000000000000000000000000000000
          00000907030C81662DB54A3A196858462569C0994FEFC49B4BFFB99141FFA27F
          33ED423313644D3D1A6D7E632CB10706030A0000000000000000000000000000
          0000261E0E33907234C13127143CBC9650DFCFA556FFC49C4BFFB99241FFAD88
          36FF8E6E28DB29200D3C917336C3221B0D2E0000000000000000000000000000
          0000372C15478A6D35B240331C4BD1A85BF7D0A556FFC49C4BFFB99241FFAD88
          36FF9D7A2BF42F250E488E7037B6322814410000000000000000000000000000
          00003A2F18488E723BB042351D4DD1A759F7CFA454FFC49A49FFB8913FFFAD87
          34FF9D792AF530260E4992753DB4352B16420000000000000000000000000000
          000016120A1A372D1A423D311C47DEC28EF8E5CEA3FFDEC79BFFD7C092FFCFB9
          8AFFB59C62F52B210C423A2F1B44141009180000000000000000000000000000
          000000000000000000003B301C44E9D9BAF8F8F1E5FFF4EDE0FFF0E8D7FFECE3
          D0FFC8B78EF5281F0C3E00000000000000000000000000000000000000000000
          000000000000000000003C311D45E6D0A9F8F1E2C6FFECE0C7FFE6D8BBFFDED0
          B0FFBDA876F529210E3F00000000000000000000000000000000000000000000
          000000000000000000003C301C45EADABDF8F8F2E7FFF5EEE0FFF0E9D8FFEDE5
          D2FFCBBA94F629200C3F00000000000000000000000000000000000000000000
          0000000000000000000030271538E4D3B3F3F1E4CDFFEDDFC4FFE2D4B5FFE4D7
          BBFFC4B38BF02019083300000000000000000000000000000000000000000000
          000000000000000000000C09040EA18D66B5F2E7D3FFF5EEE1FFF1E9D9FFE4D8
          BCFF847148B00705000C00000000000000000000000000000000000000000000
          0000000000000000000000000000261E102EA18B61BCDCC69CFBD5BF93FA8E7A
          4EB91C16082A0000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000030302052A36
          1B485D783BA18AB256ED8DB758F5668240AD222C153B00000000000000000000
          000000000000000000000000000000000000010101023F50276B7FA34FD59CC9
          62FF9CC962FF95BF5DFF94BE5DFF9CC962FF9CC861FF3544225C000000000000
          000000000000000000000000000013180C226E8E45BD9FCD64FF9AC661FF99C5
          60FF96C05EFF92BC5CFF93BD5CFF93BC5CFF99C560FF83A951E4000000000000
          000000000000000000001D26133485AA53E09ECB63FF98C35FFF9BC761FF7B9F
          4DD68EB559F697C35FFF92BC5CFF97C15EFF9ECC63FF7FA450DD000000000000
          0000000000001C2411318CB458EC9BC861FF9BC761FF8EB658EF4153286F0304
          02062F3D1E5199C460FFA1CF65FF90B95AF35068328A0B0F0713000000000000
          000012170B1E86AC53E39CC861FF9CC962FF7B9D4DD2181F0F2A000000000000
          0000000000003D4E266A678440A81A21102C0000000000000000000000000304
          02066F8E45BF9ECB63FF9BC761FF7B9F4ED30D10081700000000000000000000
          0000000000000000000000000000000000000000000000000000000000003D4E
          26699FCD64FF98C35FFF8EB659F1171D0E280000000000000000000000000000
          0000000000000000000000000000000000000000000000000000030402057DA1
          4EDB9BC761FF9BC861FF3F51276E000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000273219449DC9
          62FF99C560FF7B9E4CD800000000000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000005D793AA29CC9
          62FF96C05EFF8EB658EF2F3C1D51000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000008AB156EF95BF
          5DFF92BC5CFF97C35EFF97C360FE3E4F276A0000000000000000000000000000
          00000000000000000000000000000000000000000000000000008EB659F694BE
          5DFF93BD5CFF92BC5CFFA1CF65FF678440B10000000000000000000000000000
          000000000000000000000000000000000000000000000000000065823FB09CC9
          62FF92BC5CFF97C25EFF90B95AF6181F0F290000000000000000000000000000
          00000000000000000000000000000000000000000000000000001F2814379CC9
          62FF99C560FF9FCC63FF5066328A000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000003443
          215C84A852D980A44FDC080B050E000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          00000000000091A271F797A776FF97A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF97A776FF91A271F7000000000000000000000000000000000000
          000000000000636F4EA997A776FF97A776FF97A776FF97A776FF97A776FF97A7
          76FF97A776FF97A776FF697452B1000000000000000000000000000000000000
          000000000000040503074C543B80869369E286946AE42F34254F2F34254F8694
          6AE4869369E24C543B800506040900000000000000000000000091A271F797A7
          76FF97A776FF88986AE84B533B7F020302041B1D152D25291D3E25291D3E1B1E
          152E020302044B533B7F88986AE897A776FF97A776FF91A271F7636F4EA997A7
          76FF97A776FF97A776FF97A776FF444B3572454C367497A776FF97A776FF454C
          3674444B357297A776FF97A776FF97A776FF97A776FF697452B1040503074C54
          3B80869369E286946AE42C30224A090A070F92A072F697A776FF97A776FF92A0
          72F6090A070F2C30224A86946AE4869369E24C543B8005060409000000000000
          0000020302041B1D152D14160F21464E377797A776FF97A776FF97A776FF97A7
          76FF464E377714160F211B1E152E020302040000000000000000000000000000
          000000000000454C3674636D4EA83439285797A776FF97A776FF97A776FF97A7
          76FF34392857636D4EA8454C3674000000000000000000000000000000000000
          0000090A070F92A072F678835DCA1F23193597A776FF97A776FF97A776FF97A7
          76FF1F23193578835DCA92A072F6090A070F0000000000000000000000000000
          0000464E377797A776FF95A576FE010100015C66489B909E6FF3909E6FF35E68
          4A9F0101010296A674FD97A776FF464E37770000000000000000000000000000
          00003439285797A776FF97A776FF79855FCD3B412E640203020402030204393F
          2C6076845CC997A776FF97A776FF343928570000000000000000000000000000
          00001F23193597A776FF97A776FF97A776FF97A776FF1F2319351F23193597A7
          76FF97A776FF97A776FF97A776FF1F2319350000000000000000000000000000
          0000010100015C66489B909E6FF3909E6FF35E684A9F01010102010100015C66
          489B909E6FF3909E6FF35E684A9F010101020000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000004646467E494949CC434343CC444444CC444444CC444444CC4545
          45CC424242CC3131318A00000000000000000000000000000000000000000000
          000000000000A5A5A5F3CACACAFFCACACAFFCACACAFFCACACAFFCBCBCBFFCACA
          CAFFC9C9C9FF707070FE00000000000000000000000000000000000000000000
          000000000000ACACACFFD3D3D3FFAAAAAAFFC8C8C8FFAAAAAAFFC5C5C5FFAAAA
          AAFFCECECEFF7D7D7DFF00000000000000000000000000000000000000000000
          000000000000AAAAAAFFD5D5D5FFC3C3C3FFC2C2C2FFC0C0C0FFBDBDBDFFBDBD
          BDFFCFCFCFFF7B7B7BFF00000000000000000000000000000000000000000000
          000000000000A7A7A7FFD6D6D6FFA3A3A3FFC2C2C2FFA0A0A0FFBFBFBFFFA0A0
          A0FFCFCFCFFF777777FF00000000000000000000000000000000000000000000
          000000000000A3A3A3FFD8D8D8FFC6C6C6FFC4C4C4FFC1C1C1FFC0C0C0FFBEBE
          BEFFCFCFCFFF747474FF00000000000000000000000000000000000000000000
          000000000000A0A0A0FFDADADAFF6FBE78FF76C57FFFA0A0A0FFC2C2C2FF5E52
          D4FFCFCFCFFF707070FF00000000000000000000000000000000000000000000
          0000000000009D9D9DFFDCDCDCFFCBCBCBFFC9C9C9FFC6C6C6FFC4C4C4FFC1C1
          C1FFD0D0D0FF6D6D6DFF00000000000000000000000000000000000000000000
          0000000000009A9A9AFFDEDEDEFFF79D21FFF79E21FFF69F20FFF6A021FFF5A1
          21FFD6CDBDFF696969FF00000000000000000000000000000000000000000000
          000000000000969696FFE0E0E0FFF79A20FFF7DBA3FFF6DAA3FFF6DAA5FFF5AF
          30FFD9CEBEFF656565FF00000000000000000000000000000000000000000000
          000000000000949494FFE5E5E5FFF8981FFFF8D89DFFF7DAA1FFF7DAA2FFF6AC
          2FFFD9CFBEFF616161FF00000000000000000000000000000000000000000000
          000000000000909090FFE8E8E8FFF9951EFFF79927FFF8971EFFF7971EFFF29D
          2BFFDBD5CCFF5D5D5DFF00000000000000000000000000000000000000000000
          000000000000929292FFD9D9D9FFE0E0E0FFE4E4E4FFE4E4E4FFE1E1E1FFDEDE
          DEFFDCDCDCFF595959FF00000000000000000000000000000000000000000000
          0000000000005454548D797979CD898989E6959595FA989898FF999999FF9797
          97FFB3B3B3FF565656FF00000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000939393FF525252FF00000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000959595FF888888FF00000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000020000
          00090000000D0000000C00000003000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000075E52
          4CBF826F67FF7F6B64FF03020211000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000000000000A8E7B
          74FFD2C5BEFF7D6A63FF09080722000000010000000000000000000000090000
          000D000000080000000200000000000000000000000000000000000000099686
          7EFFD9CEC8FF92827BFF1A1615420000000200000000000000005A4B47C07661
          58FF2F2421900000000C0000000200000000000000000000000000000007A08F
          88FFE2D9D4FFAC9D97FF332C296A0000000300000000000000008A776FFFD6C9
          C4FF806A62FF322824900000000B00000001000000000000000000000005A999
          92FFEAE4E0FFD1C7C2FF655954B600000004000000000000000095837CFFDDD1
          CEFFA78F8BFF88736CFF342A2690000000110000000D0000000D000000129789
          7EFFC0BAB1FFBEB7AFFF86796EFA0000000B00000002000000009F8F88FFE4DD
          DAFFB19E97FFB09D96FF87736CFF85726BFF847169FF836F67FFA36C3BFFA771
          3FFFA76F3DFFA56E3CFFA36F41FF5B3B1F9C0402011200000002A99A92FFECE7
          E4FFBBACA4FFBBACA7FFD2C6C1FFD2C6C1FFD2C6C1FFD2C6C1FFBC8E61FFC294
          67FFC29467FFC19467FFC09164FFB07C4CFF5B3E229700000006B1A39CFFF2EF
          EEFFC7B7B3FFC5B9B5FFD0C4C0FFD1C4C2FFD1C3C2FFD1C3C2FFB8895AFFBD8F
          60FFBC8F60FFBD8F60FFBE9162FFC39A6FFFA27044ED0000000AB8ABA3FFF8F5
          F5FFD2C7C3FFD2C7C3FFECE7E5FFECE7E5FFECE7E5FFECE7E5FFDDC095FFDFC5
          9BFFDFC49BFFDFC59BFFE1C79FFFE2CAA7FFA77A4CED00000009BCB0A9FFFBFA
          FAFFE0D4D1FFDDD4D2FFF6F3F2FFF6F3F2FFF6F3F2FFF6F3F2FFF1E2C3FFF1E4
          C6FFF1E4C6FFF1E3C6FFE7D2AFFFCCA579FF654C329000000005BFB3ACFFFDFD
          FDFFE9E0E0FFEFECECFFAFA199FFAE9F99FFAC9E97FFAC9D95FFC39464FFC79A
          6AFFC79A6AFFC69969FFB2895DE7664E358C0504020D00000001C1B5AEFFFFFE
          FEFFF3EEEEFFD7CECAFF5A535084000000060000000500000005000000060000
          0006000000060000000600000005000000030000000100000000C2B6AFFFFFFF
          FFFFDDD6D2FF5E57548200000003000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000908782BFC2B6
          AFFF605A57810000000200000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000080000
          000C0000000E0000000E0000000E0000000E0000000F0000000F0000000F0000
          001000000010000000100000001000000011000000100000000A7C5B52BFAD7F
          73FFAC7F73FFAC7F72FFAC7F72FFAC7F72FFAC7F72FFAC7E72FFAB7E71FFAB7E
          71FFAB7E71FFAB7E70FFAB7D70FFAA7D70FFAA7D70FF7A594FC1AF8276FFF9F4
          F0FFF6EDE8FFF6EDE8FFF6EDE8FFCCAEA5FFF6EDE8FFF6EDE8FFF6EDE8FFF6ED
          E8FFCBABA1FFF6EDE8FFF6EDE8FFF6EDE8FFF6EDE8FFAD8174FFAF8377FFF9F4
          F0FFF9F4F0FFF9F4F0FFF9F4F0FFDFCBC4FFF9F4F0FFF9F4F0FFF9F4F0FFF9F4
          F0FFDEC9C2FFF9F4F0FFF9F4F0FFF9F4F0FFF9F4F0FFAF8176FF937A72C1C6A4
          9AFFC5A398FFC4A298FFC4A196FFC3A095FFC29F94FFC19E93FFC19C92FFC09B
          90FFBF9A8FFFBE998EFFBD978DFFBC968BFFBC958AFF8A6D65C30000000B0000
          0011000000130000001300000014000000140000001500000015000000160000
          001700000017000000170000001700000018000000170000000F81573ABFB479
          50FFB47950FFB37950FFB37850FFB4784FFFB3784FFFB3784FFFB3784FFFB377
          4EFFB3774EFFB3774EFFB3774EFFB3774EFFB2774EFF805637C1BA835AFFEBD1
          A7FFE6C598FFE5C397FFE5C396FFC7976CFFE4C293FFE5C192FFE4C091FFE4C0
          91FFC7956BFFE3BF8EFFE3BE8DFFE3BD8DFFE2BD8CFFBA835AFFBD8860FFEDD7
          ADFFEDD5ACFFECD5ABFFECD4A9FFCC9F76FFEBD2A7FFEBD1A6FFEAD0A5FFEBCF
          A3FFCB9E74FFEACDA1FFE9CCA0FFE9CB9EFFE8CA9DFFBD895FFF9C7B59C1D2A6
          79FFD2A67AFFD2A679FFD2A679FFD2A67AFFD2A679FFD2A67AFFD2A67AFFD2A6
          79FFD2A679FFD2A679FFD2A679FFD2A679FFD2A679FF9A7A59C30000000B0000
          0011000000130000001300000014000000140000001500000015000000160000
          001700000017000000170000001700000018000000170000000F7F615ABFB388
          7CFFB3887BFFB2877BFFB2877AFFB2867AFFB18679FFB18679FFB08578FFB185
          78FFB08577FFB08477FFAF8477FFAF8376FFAF8376FF7E5E55C1B78D81FFFDFA
          F9FFFBF8F5FFFBF7F5FFFBF7F5FFCCAEA5FFFBF7F5FFFBF6F4FFFBF7F3FFFAF6
          F4FFCBABA1FFFBF6F3FFFBF5F3FFFAF5F3FFFBF6F3FFB58C7FFFB78E82FFFDFB
          FAFFFDFBFAFFFDFBFAFFFDFAFAFFDFCBC4FFFDFAF9FFFDFAF9FFFDFAF9FFFDFA
          F9FFDEC9C2FFFCFAF9FFFCFAF9FFFDFAF9FFFCFAF9FFB78C81FF947B72C1C6A5
          9BFFC6A49AFFC6A399FFC5A398FFC5A297FFC4A197FFC4A096FFC3A095FFC29F
          94FFC29E93FFC29D92FFC19C91FFC09B90FFC09A90FF8D726AC3000000030000
          0005000000050000000500000006000000060000000600000006000000070000
          0007000000070000000700000008000000080000000800000005}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000070000
          000B0000000B0000000B0000000C0000000C0000000C0000000C0000000C0000
          000C0000000C0000000C0000000C0000000C00000008000000027F5347BEB074
          62FFAF7461FFAF7460FFAE735FFFAE725FFFAD715EFFAC705CFFAC705CFFAC70
          5CFFAC6F5BFFAC6F5BFFAB6F5BFFAB6E5AFF7B4F40BF00000008B67E6CFFFBF6
          F3FFFAF5F2FFFAF5F2FFFAF5F2FFFAF5F1FFFAF4F1FFF9F4F0FFF9F4F0FFF9F4
          F0FFF9F3F0FFF8F3EFFFF9F3EFFFF8F2EEFFB37968FF0000000BB77F6FFFFBF7
          F4FFF7EFE9FFF6EEE9FFF6EEE9FFF6EEE9FFF6EEE9FFF6EEE8FFF6EEE8FFF6EE
          E8FFF6EEE8FFF6EDE7FFF5EDE7FFF9F4F0FFB47B6AFF0000000BB88171FFFBF8
          F5FFF7F0EBFFC6A092FFC6A093FFC69F91FFC59F92FFC59E90FFC59E90FFC59E
          90FFC59E91FFC49E8FFFF6EEE9FFFAF4F2FFB47C6BFF0000000BB98472FFFCF9
          F7FFF8F0ECFFF8F1ECFFF7F0ECFFF7F0EBFFF8F0EBFFF7EFEAFFF7F0EAFFF7EF
          EAFFF7EFE9FFF7EFE9FFF6EFEAFFFAF6F3FFB67F6DFF0000000ABB8674FFFDFA
          F8FFF8F1EDFFC9A697FFC9A497FFC9A497FFC9A396FFC8A395FFC8A396FFC8A2
          95FFC7A295FFC7A394FFF7F0EAFFFAF7F5FFB7806FFF00000009BB8674FFFDFA
          F8FFF8F1EDFFF9F3EFFFF9F2EFFFF8F2EEFFF8F2EDFFF8F1ECFFF8F1ECFFF8F1
          ECFFF8F0ECFFF7F0ECFFF7F0EAFFFAF7F5FFB7806FFF00000009BC8877FFFDFB
          F9FFF9F3EFFFCCA99CFFCCA89CFFCBA89BFFCBA89BFFCBA79AFFCAA79AFFCAA6
          9AFFCAA699FFCAA699FFF7F0ECFFFBF8F6FFB98271FF00000008C08D7DFFFEFD
          FCFFFAF5F2FFFAF6F3FFFAF6F2FFFAF5F2FFFAF5F1FFF9F4F1FFF9F4F0FFF9F3
          F0FFF8F3EFFFF8F3EFFFF9F3F0FFFDFBFAFFBC8877FF00000008C28F7EFFFEFD
          FDFFFBF6F3FFCFADA2FFCFAFA0FFCFAEA1FFCFADA1FFCEAC9FFFCFAC9FFFCEAC
          9EFFCDAB9FFFCEAA9EFFFAF4F0FFFDFCFAFFBE8A79FF00000007C39281FFFEFE
          FDFFFBF7F4FFFBF7F4FFFAF7F5FFFBF7F4FFFAF6F4FFFAF5F3FFFAF6F3FFFAF6
          F2FFF9F5F2FFF9F5F2FFF9F6F2FFFDFCFCFFBF8D7BFF00000007C49483FFFFFE
          FEFFFBF8F5FFD1B2A6FFD1B2A6FFD2B1A5FFD2B1A5FFD1B0A4FFD1B1A4FFD1B0
          A3FFD1AFA3FFD0B0A3FFFAF6F3FFFEFDFCFFC08F7EFF00000006C59584FFFFFE
          FEFFFCF8F7FFFBF8F6FFFCF8F6FFFBF8F5FFFBF8F6FFFCF8F5FFFCF8F5FFFBF8
          F5FFFBF7F4FFFBF7F4FFFBF7F4FFFEFDFDFFC2907FFF00000005C69786FFFFFF
          FEFFFFFFFEFFFFFEFEFFFFFFFEFFFFFEFEFFFFFEFEFFFFFEFEFFFFFEFEFFFEFE
          FEFFFEFEFEFFFFFEFEFFFFFEFDFFFEFEFDFFC39281FF000000059A796DBFCFA4
          94FFCFA393FFCEA393FFCEA393FFCEA292FFCEA292FFCDA191FFCDA191FFCDA0
          90FFCCA090FFCC9F8FFFCB9E8EFFCB9E8EFF967569C000000003}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000010205080C10
          38451C247A962735B3E82936B5EC1D26809E090D2A3700000000000000000000
          00000000000000000000000000000000000000000102131A566C2633ABD72D3C
          CCFF2C3CCAFF2B39C0FF2B39C0FF2D3CCCFF2D3CCAFE0E134052000000000000
          000000000000000000000000000007081C24202B93BB2E3DCFFF2C3AC7FF2F3E
          CDFF3343D4FF3646DAFF3746DBFF3241D0FF2F3DCDFF2530A4D5000000000000
          00000000000000000000090C28322835B1DC2D3CCCFF303FCDFF3A4AE9FF3544
          CEE03C4DE7F83F4FF0FF3D4DE8FF3F50F0FF4051F8FF2935AEDD000000000000
          000000000000080B26302935B7E62D3CCAFF3747E0FF3C4DEAF421297B7E0405
          0E0F171D575D4051F7FF4253FCFF3C4CE9F5263090950708191B000000000000
          00000507181D2532ADDD2E3CCCFF3B4BEAFF3846D4D80F123739000000000000
          0000000000001F27767C2E3CAFB30D1032350000000000000000000000000101
          04051F2A90BB2F3ED0FF3C4CEAFF3845D2DA080A202200000000000000000000
          0000000000000000000000000000000000000000000000000000000000001218
          50682E3ED0FF3949E3FF3E4EEDF40D1033370000000000000000000000000000
          0000000000000000000000000000000000000000000000000000010105062430
          A5DA3747DFFF4253FBFF1E27767F000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000000A0E2E3E3241
          D5FF3F50F3FF3645D1E101010305000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000019216E903B4B
          ECFF3F4EEDFF3B4CE6F2181E5E64000000000000000000000000000000000000
          00000000000000000000000000000000000000000000000000002734AFDE3E4E
          EDFF3C4CE6FF3D4EECFF4152F9FF21297D830000010100000000000000000000
          0000000000000000000000000000000000000000000000000000303CC3E63E4F
          EFFF3C4CE7FF3C4CE6FF4253FDFF2F3BB5C00101030300000000000000000000
          0000000000000000000000000000000000000000000000000000252E8E984153
          FBFF3C4CE5FF3D4DEBFF3C4EEAF80D1032360000000000000000000000000000
          00000000000000000000000000000000000000000000000000000A0D272A3D4F
          EEFB4051F5FF4253FEFF252E8E98000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000001217
          474C3341C2C73340C5D005061417000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          20000000000000040000000000000000000000000000000000000000000A0000
          000F000000110000001100000011000000100000000B000000060000000B0000
          0011000000130000001300000013000000120000000C0000000378462DC1A763
          3EFFA7633EFFA7633EFFA7633EFFA7633EFF77482DC20000001577462CC3A763
          3DFFA7633DFFA7623DFFA7623DFFA7623DFF77462CC30000000BB8764CFFECC1
          85FFECC184FFECC084FFECC084FFEBC084FFB7734AFF0000001EB67349FFECBF
          81FFEBBE82FFEBBF80FFEABE81FFEBBE80FFB77047FF00000011BA7950FFF1D1
          9FFFF2D09FFFF1D19FFFF1D09FFFF1D09FFFB8764CFF0000001CB8754CFFF1CE
          9DFFF0CE9DFFF1CE9BFFF1CE9BFFF0CE9CFFB87349FF00000010906749C5C289
          62FFC28962FFC28861FFC28861FFC18860FF8F6547C6000000118F6347C6C087
          5FFFC0865FFFC0865EFFC0865FFFC0865EFF8D6245C70000000A0000000E0000
          0016000000170000001800000018000000170000001000000008000000110000
          00190000001B0000001B0000001C0000001B0000001200000005794A30BEA967
          42FFA96742FFA96742FFA96641FFA96641FF79492EBF0000000F79492EBFA966
          41FFA96641FFA96640FFA96640FFA86540FF78482EC000000008C1855EFFEDC5
          8CFFEDC68BFFECC68BFFECC58AFFEDC58AFFBF825AFF00000015BF8059FFEDC5
          88FFEDC488FFEDC487FFECC487FFEDC486FFBE7E56FF0000000CC28861FFF2D4
          A5FFF2D4A5FFF2D4A5FFF2D4A5FFF2D4A4FFC1845CFF00000013C0845DFFF1D3
          A2FFF1D2A2FFF1D2A2FFF1D1A2FFF2D2A1FFBF8159FF0000000B946E51C2C894
          6EFFC8936DFFC7936DFFC7936DFFC7936DFF946D50C30000000C946C50C3C791
          6BFFC7916BFFC6916AFFC79069FFC6916AFF93694EC400000007000000090000
          000D0000000E0000000F0000000F0000000E0000000A000000050000000B0000
          0010000000110000001200000012000000120000000C000000037A4C33BBAB6B
          46FFAB6B46FFAB6B46FFAB6B46FFAB6A46FF7A4C31BC000000097A4C31BCAB6A
          45FFAB6A45FFAB6A45FFAB6A45FFAB6945FF7A4C32BD00000006C9946DFFEFCC
          93FFEFCC93FFEFCC92FFEFCB92FFEFCB92FFC7916AFF0000000CC8916AFFEECA
          90FFEEC990FFEEC990FFEEC88EFFEEC88EFFC68E67FF00000008C9966FFFF4D9
          ABFFF3D8ABFFF3D9ABFFF3D8ABFFF3D8ABFFCA946DFF0000000BC9946DFFF2D8
          AAFFF3D8A9FFF3D7A9FFF3D7A9FFF2D6A8FFC7906AFF00000007977558BFCD9E
          77FFCD9D77FFCC9D77FFCC9C77FFCC9C76FF977457C000000006977257C0CC9B
          75FFCB9B75FFCC9A75FFCC9A75FFCB9A74FF977256C100000004000000020000
          0003000000030000000300000004000000030000000200000001000000030000
          0004000000050000000500000005000000050000000300000001}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000AF7B
          4AF4B8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFFB8824DFFB8824DFFB8824DFFAF7B4AF400000000000000009D70
          41DBB8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFFB8824DFFB8824DFFB8824DFF9D7041DB00000000000000003727
          174CB47F4AF9B8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFFB8824DFFB8824DFFB47F4AF93727174C00000000000000000000
          000024190F32835C37B5B4804CFBB8824DFF745131A120170E2D20170E2D7451
          31A1B8824DFFB4804CFB835C37B524190F320000000000000000000000000000
          000000000000000000000C09051166482B8D0403020600000000000000000403
          020666482B8D0C09051100000000000000000000000000000000000000000000
          00000000000000000000000000000000000045301D5F90663DC990663DC94530
          1D5F000000000000000000000000000000000000000000000000000000000000
          00000000000000000000000000002D20133FB37E4BFAB8824DFFB8824DFFB37E
          4BFA2D20133F0000000000000000000000000000000000000000000000000000
          00000000000000000000000000008D633BC3B8824DFFB8824DFFB8824DFFB882
          4DFF8D633BC30000000000000000000000000000000000000000000000000000
          00000000000000000000291D1139B7814BFDB8824DFFB8824DFFB8824DFFB882
          4DFFB7814BFD281D113800000000000000000000000000000000000000000000
          0000000000000000000062452988B8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFF6245298800000000000000000000000000000000000000000000
          000000000000000000002E211340B8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFF2F21144100000000000000000000000000000000000000000000
          000000000000000000001E150C29B8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFF1E150D2A00000000000000000000000000000000000000000000
          0000000000000000000003020104A77745E9B8824DFFB8824DFFB8824DFFB882
          4DFFA97647EA0403020600000000000000000000000000000000000000000000
          0000000000000000000000000000281D11388D643BC5B17D4AF5B17D4AF59066
          3CC82D20133F0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          20000000000000040000000000000000000000000000000000000000000A0000
          0010000000110000001100000011000000110000001100000011000000110000
          0011000000120000001200000012000000110000000B0000000374472BC2A361
          3CFFA3603CFFA2603AFFA2603AFFA25E39FFA05E3AFFA05C38FFA05C38FF9F5C
          38FF9F5B38FF9F5B37FF9F5B37FF9E5B37FF703F28C30000000AAB6943FFE5C1
          91FFE1B981FFE2B881FFE1B982FFE1B881FFE1B881FFE1B881FFE1B881FFE1B8
          81FFE1B881FFE1B881FFE1B881FFE4BE8CFFA8653FFF0000000FAC6B45FFE6C3
          94FFE3BC88FFE3BC89FFE3BC88FFE3BC88FFE3BC88FFE2BC88FFE2BC88FFE3BC
          87FFE2BC87FFE2BC87FFE3BC87FFE5BF8EFFA96741FF00000010AD6E48FFE7C5
          99FFCD9967FFC68F5FFFC58F60FFC58E5FFFC58E5EFFC48E5FFFC58E5EFFC892
          60FFC48D5DFFC58D5EFFCB9764FFE5C292FFAA6943FF0000000FB0714BFFE9C8
          9CFFC79263FFF2E9E4FFF9F3EDFFF9F2EDFFF9F2EDFFF9F2EDFFFCF8F6FFC690
          60FFF2E9E3FFFCF8F5FFC58F60FFE7C596FFAC6B45FF0000000EB1744EFFEACB
          A1FFCB9868FFC99466FFC89365FFC89465FFC79364FFC79364FFC89364FFC893
          64FFC79263FFC79263FFC99464FFE8C89AFFAD6D47FF0000000EB37651FFECCF
          A6FFCA9769FFF2EAE5FFF9F4F0FFF9F4F0FFF9F4F0FFF9F4F0FFFCF9F7FFC995
          67FFF2E9E5FFFCF9F6FFC89465FFEACB9FFFAE7049FF0000000DB57A55FFECD1
          AAFFCE9D6EFFCC9A6CFFCC996BFFCC996BFFCB996BFFCB986AFFCB986AFFCB98
          6AFFCA9869FFCA9769FFCC996BFFECCDA3FFB1724DFF0000000CB87D58FFEED5
          AFFFCD9D71FFF3EAE7FFFBF6F2FFFAF5F2FFFBF5F2FFFAF5F1FFFCFAF8FFCFA0
          76FFF2EAE6FFFCF9F8FFCB9A6CFFECD0A9FFB3754FFF0000000BBA815BFFF2DD
          BCFFD5AC83FFD3A980FFD3A980FFD3A880FFD3A87FFFD2A87FFFD2A77EFFD2A8
          7EFFD1A67DFFD1A57CFFD1A479FFEDD4ADFFB57952FF0000000BC18E6BFFF5E7
          CBFFD5AD85FFF3ECE8FFFCF8F4FFFCF7F4FFFBF7F4FFFBF7F4FFFDFAF9FFD3AA
          82FFF3EBE7FFFDFAF9FFD3A981FFF3DFBFFFB67C56FF0000000AC59878FFF7E9
          D0FFDEBC93FFD7B089FFD7B088FFD6B088FFD6AF88FFD6AE88FFD6AE87FFD9B3
          8BFFD5AD86FFD5AD85FFDCB890FFF5E6CBFFBC8662FF00000009C89C7DFFF7EC
          D4FFF2DEB7FFF2DEB7FFF2DDB6FFF2DDB6FFF2DDB6FFF2DDB6FFF2DDB5FFF2DD
          B5FFF2DCB5FFF2DCB5FFF2DCB4FFF7E9CEFFC29170FF00000008CBA081FFF8ED
          D6FFF8EDD6FFF8EDD6FFF8ECD5FFF8ECD5FFF8ECD5FFF8ECD5FFF8ECD4FFF8EC
          D4FFF8ECD4FFF8ECD4FFF8ECD3FFF8ECD3FFC59878FF00000007977A62C1CDA4
          86FFCCA384FFCCA384FFCBA284FFCBA283FFCAA183FFCAA082FFCAA082FFCAA0
          81FFC99F80FFC99F80FFC99E7FFFC89D7EFF947259C100000005}
      end>
  end
  object TimerCheck: TTimer
    OnTimer = TimerCheckTimer
    Left = 320
    Top = 304
  end
  object TimerExit: TTimer
    Enabled = False
    Interval = 1
    OnTimer = TimerExitTimer
    Left = 552
    Top = 368
  end
  object ImageListStatusBar: TImageList
    Height = 24
    Width = 24
    Left = 560
    Top = 456
    Bitmap = {
      494C010102002400040018001800FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000600000001800000001002000000000000024
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000007000B003B001400660019
      007F0019007F00140066000B003B000000070000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000B27000019590000
      237C0000237C00001F6D000014460000010B0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000007002E0039009B016A03F107830AFF098F0EFF0B97
      11FF0B9711FF098F0EFF07830AFF016A03F10039009B0007002E000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000010B00002F7000007ED800009CFB0000A2FD0000
      A6FF0000A7FF0000A3FE00009EFC000093F600005EA400000F37000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000090034008B037206F70E9915FF12AB1CFF13AC1CFF13AC1CFF13AC
      1CFF13AC1CFF13AC1CFF13AC1CFF12AB1CFF0E9915FF037206F70034008B0000
      0009000000000000000000000000000000000000000000000000000000000000
      00000000000000001A4000007FD80000A4FD0000B2FF0000B2FF0000B2FF0000
      B1FF0000B0FF0000AFFF0000AEFF0000ADFF0000A8FF000099F9000053930000
      010A000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000001
      0014004B00C10B8E12FE16AE21FF17B022FF17B022FF17B022FF17B022FF17B0
      22FF17B022FF17B022FF17B022FF17B022FF17B022FF16AE21FF0B8E12FE004B
      00C1000100140000000000000000000000000000000000000000000000000000
      000000002763000097F30000B5FF0000B8FF0000B7FF0000B7FF0000B6FF0000
      B5FF0000B4FF0000B3FF0000B2FF0000B2FF0000B1FF0000B0FF0000A4FE0000
      70C3000002140000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000A024E
      03C3129B1CFF1AB327FF1AB327FF1AB327FF1AB327FF1AB327FF1AB327FF1AB3
      27FF1AB327FF1AB327FF1AB327FF1AB327FF1AB327FF1AB327FF1AB327FF129B
      1CFF024E03C30000000A00000000000000000000000000000000000000000000
      1F5200019FF80000BCFF0000BEFF0000BDFF0000BCFF0000BBFF0000BAFF0000
      B9FF0000B8FF0000B7FF0000B7FF0000B6FF0000B5FF0000B4FF0000B3FF0000
      AAFF00016FC10000010900000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000003700931B9F
      2AFE1EB62DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB7
      2DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB72DFF1EB6
      2DFF1B9E2AFD00370093000000000000000000000000000000000000081F0608
      95EA0203C2FF0000C3FF0000C2FF0000C1FF0000C0FF0000C0FF0000BFFF0000
      BEFF0000BDFF0000BCFF0000BBFF0000BAFF0000B9FF0000B8FF0000B8FF0000
      B6FF080BB1FE00004D8B00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000A0037178723F927BF
      3BFF22BB33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB
      33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB33FF22BB
      33FF27BF3BFF178723F9000A003700000000000000000000000000005CA7181E
      CCFF0000C9FF0000C8FF0000C7FF0000C6FF0000C5FF0000C4FF0000C3FF0000
      C2FF0000C2FF0000C1FF0000C0FF0000BFFF0000BEFF0000BDFF0000BCFF0000
      BBFF0709C0FF1014A7F700000B2E000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000003F00A439C656FF27C0
      3AFF26BF38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF
      38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF38FF26BF
      38FF27C03AFF39C656FF003F00A3000000000000000000000820161DACFA1015
      D8FF0000CEFF0000CDFF0000CCFF0000CBFF0000CAFF0000C9FF0000C8FF0000
      C7FF0000C6FF0000C5FF0000C4FF0000C3FF0000C2FF0000C1FF0000C0FF0000
      BFFF0000BFFF2733D0FF0000549B000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000001000B147F1DF641D861FF2AC3
      3EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC3
      3EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC33EFF2AC3
      3EFF2AC33EFF41D861FF147E1DF50001000B0000000000002E793B4CD7FF0101
      D5FF0000D4FF0000D3FF0000D2FF0000D1FF0000D0FF0000CFFF0000CEFF0000
      CCFF0000CBFF0000CAFF0000C9FF0000C8FF0000C7FF0000C6FF0000C5FF0000
      C4FF0000C3FF2A36DBFF10159EF1000000070000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000D00462DA844FC3DD65BFF2DC6
      44FF2DC644FF2DC644FF2DC644FF2DC644FF2DC644FF2DC644FF2DC644FF2DC6
      44FF2DC644FF2DC644FF2DC644FF2DC644FF2DC644FF2DC644FF2DC644FF2DC6
      44FF2DC644FF3DD65BFF2DA844FC000D00460000000000006EBA5168EFFF0000
      DBFF0000D9FF0000D8FF0000D7FF0000D6FF0000D5FF0000D4FF0000D3FF0000
      D2FF0000D1FF0000D0FF0000CFFF0000CEFF0000CDFF0000CCFF0000CBFF0000
      CAFF0000C9FF1D26D6FF3444C5FF0000113B0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000014006D40C761FE3AD357FF31CA
      49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA
      49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA49FF31CA
      49FF31CA49FF3AD357FF3FC45EFE0014006D000000000F1393E6546DF4FF0000
      E0FF0000DFFF0000DEFF0000DDFF0000DCFF0000DBFF0000DAFF0000D8FF0000
      D7FF0000D6FF0000D5FF0000D4FF0000D3FF0000D2FF0000D1FF0000D0FF0000
      CFFF0000CEFF1318D4FF526AD7FF00001E660000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000017007C49D46EFF41DA61FF35CE
      4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE
      4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE4FFF35CE
      4FFF35CE4FFF41DA61FF49D46EFF0017007C00000000222CB0FF607CF7FF0000
      E7FF0000E5FF0000E4FF0000E3FF0000E2FF0000E1FF0000E0FF0000DEFF0000
      DDFF0000DCFF0000DBFF0000DAFF0000D9FF0000D7FF0000D6FF0000D5FF0000
      D4FF0000D3FF1820DAFF6988E3FF0000257F0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000017007C48D26CFF49E26DFF39D2
      55FF39D255FF39D255FF39D255FF39D255FF39D255FF39D255FF39D255FF39D2
      55FF39D255FF39D255FF39D255FF39D255FF39D255FF39D255FF39D255FF39D2
      55FF39D255FF49E26DFF46CF69FF0017007C00000000232DB0FF7192FBFF0000
      EDFF0000ECFF0000EAFF0000E9FF0000E8FF0000E7FF0000E6FF0000E4FF0000
      E3FF0000E2FF0000E1FF0000E0FF0000DFFF0000DDFF0000DCFF0000DBFF0000
      DAFF0000D9FF2834E2FF6A8AE3FF0000257F0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000011005936B752FD56EF81FF3DD6
      5BFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD6
      5AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD65AFF3DD6
      5AFF3DD65BFF56EF81FF36B752FD0011005900000000101593E68CB5FEFF0406
      F3FF0000F2FF0000F1FF0000EFFF0000EEFF0000EDFF0000ECFF0000EAFF0000
      E9FF0000E8FF0000E7FF0000E5FF0000E4FF0000E3FF0000E2FF0000E1FF0000
      E0FF0000DEFF485DEEFF5A75D7FF00001E660000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000070027229834FB61F991FF42DB
      64FF40D960FF40D960FF40D960FF40D960FF40D960FF40D960FF40D960FF40D9
      60FF40D960FF40D960FF40D960FF40D960FF40D960FF40D960FF40D960FF40D9
      60FF42DB64FF61F991FF229834FB000700270000000000006EBA87AFF7FF2B38
      FBFF0000F9FF0000F7FF0000F6FF0000F5FF0000F3FF0000F2FF0000F1FF0000
      F0FF0000EEFF0000EDFF0000ECFF0000EBFF0000E9FF0000E8FF0000E7FF0000
      E6FF0001E4FF7395F9FF4053C4FF0000113B0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000055B06D860F490FF54EC
      7EFF44DD66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD
      66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD66FF44DD
      66FF54EC7EFF60F490FF045906D4000000000000000000002E795E7ADBFF6685
      FEFF0102FEFF0000FDFF0000FCFF0000FBFF0000FAFF0000F9FF0000F7FF0000
      F6FF0000F5FF0000F4FF0000F2FF0000F1FF0000F0FF0000EEFF0000EDFF0000
      ECFF212AEFFF8EB8FDFF151B9CED000000070000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000020007038B252FD6DFC
      9EFF4EE574FF48E16BFF48E16BFF48E16BFF48E16BFF48E16BFF48E16BFF48E1
      6BFF48E16BFF48E16BFF48E16BFF48E16BFF48E16BFF48E16BFF48E16BFF4EE5
      74FF6CFC9EFF38B252FD00200070000000000000000000000820222DACFA8EB8
      FEFF2B38FFFF0000FFFF0000FFFF0000FFFF0000FEFF0000FEFF0000FDFF0000
      FCFF0000FBFF0000FAFF0000F9FF0000F7FF0000F6FF0000F5FF0000F3FF0405
      F2FF6E8FFBFF6A8AE3FF0000549B000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000B065D09D868EA
      94FF70FAA1FF50E777FF4CE571FF4CE571FF4CE571FF4CE571FF4CE571FF4CE5
      71FF4CE571FF4CE571FF4CE571FF4CE571FF4CE571FF4CE571FF50E777FF70FA
      A1FF68EA94FF065D09D80000000B00000000000000000000000000005CA76887
      E1FF80A6FFFF1116FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
      FFFF0000FFFF0000FEFF0000FEFF0000FDFF0000FCFF0000FBFF0102FAFF475D
      FBFF8EB8FCFF202AA9F700000B2E000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000001100401B82
      26F379F5A9FF79FBABFF59EC84FF4FE877FF4FE877FF4FE877FF4FE877FF4FE8
      77FF4FE877FF4FE877FF4FE877FF4FE877FF4FE877FF59EC84FF79FBABFF79F5
      A9FF1B8226F300110040000000000000000000000000000000000000081F1318
      98EB83AAF5FF7699FFFF171EFFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
      FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0305FFFF4256FEFF8EB9
      FEFF4F67D0FE00004D8B00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000001A
      0063238C31F87BF0AAFF87FEBAFF72F6A2FF5BEE86FF53EC7CFF53EC7CFF53EC
      7CFF53EC7CFF53EC7CFF53EC7CFF5BEF86FF72F6A2FF87FEBAFF7BF0AAFF238C
      31F8001A00630000000000000000000000000000000000000000000000000000
      1F522531AEF888B0F9FF82A9FFFF3343FFFF0609FFFF0000FFFF0000FFFF0000
      FFFF0000FFFF0000FFFF0000FFFF0203FFFF141AFFFF5E7AFFFF8FBAFEFF6380
      DEFF030570C10000010900000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000150052137119EA67D48CFF90FDC3FF91FEC4FF87FCB9FF7BF8ACFF75F7
      A4FF75F7A5FF7BF8ACFF87FCB9FF91FEC4FF90FDC3FF67D38CFF137219EB0015
      0052000000000000000000000000000000000000000000000000000000000000
      0000000027631F28A6F37DA2F1FF90BBFEFF7599FFFF4357FFFF222CFFFF0B0F
      FFFF090CFFFF141BFFFF303FFFFF5C78FFFF89B2FFFF8DB7FCFF526BD1FD0305
      72C3000002140000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000005001F003E00A7238830FA63C985FF8FF4BFFF9AFFCEFF9AFF
      CEFF9AFFCEFF9AFFCEFF8FF4BFFF63C985FF238830FA003E00A70005001F0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000001A40080B83D84960CAFD87AFF7FF8FBAFEFF91BCFFFF91BC
      FFFF91BCFFFF91BCFFFF90BBFEFF8DB7FCFF6F90E6FF2530ADF9000053930000
      010A000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000050020001F0079004B00BA116C16E6258A
      31FF268B33FF126E18E6004C00BE001F00790005002000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000010B00002F70060881D83140B8FB4E66CEFD6482
      DEFF6987E2FF5C77D9FE4054C3FC1B24A3F500005DA300000F37000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000B27000019590000
      237C0000237C00001F6D0000154A0000010B0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000060000000180000000100010000000000200100000000000000000000
      000000000000000000000000FFFFFF00FF00FFFF80FF000000000000FC003FFC
      003F000000000000F0000FF8000F000000000000E00007F00007000000000000
      C00003E00003000000000000C00003C00003000000000000800001C000010000
      0000000080000180000100000000000000000080000000000000000000000080
      0000000000000000000000800000000000000000000000800000000000000000
      0000008000000000000000000000008000000000000000000000008000000000
      00000000800001800000000000000000800001800001000000000000800001C0
      0001000000000000C00003C00003000000000000E00007E00003000000000000
      F0000FF00007000000000000F8001FF8000F000000000000FE007FFC003F0000
      00000000FFFFFFFF80FF00000000000000000000000000000000000000000000
      000000000000}
  end
  object TimerInit: TTimer
    Enabled = False
    Interval = 10
    OnTimer = TimerInitTimer
    Left = 413
    Top = 314
  end
  object TimerInitAllForm: TTimer
    Interval = 10
    OnTimer = TimerInitAllFormTimer
    Left = 421
    Top = 154
  end
  object TimerAutoLock: TTimer
    Enabled = False
    OnTimer = TimerAutoLockTimer
    Left = 288
    Top = 424
  end
  object pmSys: TdxBarPopupMenu
    BarManager = MainBarManager
    ItemLinks = <
      item
        Visible = True
        ItemName = 'pmiLockWindow'
      end
      item
        BeginGroup = True
        Visible = True
        ItemName = 'pmiLogout'
      end
      item
        Visible = True
        ItemName = 'dxBarSeparator1'
      end
      item
        Visible = True
        ItemName = 'pmiSetPwd'
      end>
    UseOwnFont = False
    Left = 736
    Top = 480
    PixelsPerInch = 96
  end
  object TrayIcon: TCTrayIcon
    Hide = True
    RestoreOn = imDoubleClick
    PopupMenuOn = imNone
    Left = 768
    Top = 400
  end
end
