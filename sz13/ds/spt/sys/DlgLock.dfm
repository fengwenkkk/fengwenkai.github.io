object DialogLock: TDialogLock
  Left = 0
  Top = 0
  BorderIcons = []
  Caption = #35299#38145
  ClientHeight = 181
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 18
  object PanelBack: TPanel
    Left = 0
    Top = 0
    Width = 360
    Height = 181
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitLeft = 256
    ExplicitTop = 40
    ExplicitWidth = 96
    ExplicitHeight = 41
  end
  object PanelFront: TPanel
    Left = 0
    Top = 0
    Width = 360
    Height = 181
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    ExplicitHeight = 121
    object lbPwd: TLabel
      Left = 52
      Top = 68
      Width = 34
      Height = 23
      Caption = #23494#30721
      Font.Charset = DEFAULT_CHARSET
      Font.Color = 6373424
      Font.Height = -17
      Font.Name = #24494#36719#38597#40657
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object lbUser: TLabel
      Left = 52
      Top = 24
      Width = 34
      Height = 23
      Caption = #29992#25143
      Font.Charset = ANSI_CHARSET
      Font.Color = 6373424
      Font.Height = -17
      Font.Name = #24494#36719#38597#40657
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object PanelCfgCtrl: TPanel
      Left = 0
      Top = 121
      Width = 360
      Height = 60
      Align = alBottom
      BevelOuter = bvNone
      ParentBackground = False
      TabOrder = 0
      DesignSize = (
        360
        60)
      object dxBevel1: TdxBevel
        Left = 0
        Top = 0
        Width = 360
        Height = 3
        Align = alTop
        LookAndFeel.NativeStyle = False
        Shape = dxbsLineTop
        ExplicitLeft = -650
        ExplicitTop = 47
        ExplicitWidth = 1000
      end
      object btnOK: TcxButton
        Left = 132
        Top = 18
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Caption = #30830#35748
        Default = True
        LookAndFeel.Kind = lfUltraFlat
        LookAndFeel.NativeStyle = False
        TabOrder = 0
        OnClick = btnOKClick
      end
      object btnCancel: TcxButton
        Left = -15
        Top = 18
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Cancel = True
        Caption = #25918#24323
        LookAndFeel.Kind = lfUltraFlat
        LookAndFeel.NativeStyle = False
        TabOrder = 1
        Visible = False
        OnClick = btnCancelClick
      end
      object btnLogout: TcxButton
        Left = 241
        Top = 18
        Width = 75
        Height = 25
        Anchors = [akTop, akRight]
        Cancel = True
        Caption = #27880#38144
        LookAndFeel.Kind = lfUltraFlat
        LookAndFeel.NativeStyle = False
        OptionsImage.Glyph.SourceDPI = 96
        OptionsImage.Glyph.Data = {
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
        TabOrder = 2
        OnClick = btnLogoutClick
      end
    end
    object edtPwd: TcxTextEdit
      Left = 100
      Top = 67
      ParentFont = False
      Properties.AutoSelect = False
      Properties.EchoMode = eemPassword
      Properties.HideSelection = False
      Properties.MaxLength = 15
      Properties.PasswordChar = #9679
      Style.BorderStyle = ebs3D
      Style.Font.Charset = DEFAULT_CHARSET
      Style.Font.Color = clWindowText
      Style.Font.Height = -15
      Style.Font.Name = 'Tahoma'
      Style.Font.Style = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Blue'
      Style.TextColor = 6307888
      Style.IsFontAssigned = True
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Blue'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Blue'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Blue'
      TabOrder = 1
      Text = 'admin'
      Width = 216
    end
    object edtUser: TcxTextEdit
      Left = 100
      Top = 23
      AutoSize = False
      Enabled = False
      ParentFont = False
      Properties.AutoSelect = False
      Properties.MaxLength = 15
      Style.Font.Charset = DEFAULT_CHARSET
      Style.Font.Color = clWindowText
      Style.Font.Height = -15
      Style.Font.Name = 'Arial'
      Style.Font.Style = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Blue'
      Style.TextColor = 6307888
      Style.IsFontAssigned = True
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Blue'
      StyleFocused.LookAndFeel.NativeStyle = False
      StyleFocused.LookAndFeel.SkinName = 'Blue'
      StyleHot.LookAndFeel.NativeStyle = False
      StyleHot.LookAndFeel.SkinName = 'Blue'
      TabOrder = 2
      Text = 'leijie'
      Height = 26
      Width = 216
    end
  end
  object TimerCheck: TTimer
    OnTimer = TimerCheckTimer
    Left = 64
    Top = 104
  end
end
