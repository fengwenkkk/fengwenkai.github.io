object DialogExportRecFile: TDialogExportRecFile
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #24405#38899#24405#20687#25991#20214#23548#20986
  ClientHeight = 702
  ClientWidth = 885
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnCreate = FormCreate
  DesignSize = (
    885
    702)
  PixelsPerInch = 96
  TextHeight = 18
  object dxBevel1: TdxBevel
    Left = -556
    Top = 634
    Width = 1441
    Height = 3
    Anchors = [akLeft, akRight, akBottom]
    LookAndFeel.NativeStyle = False
    Shape = dxbsLineTop
    ExplicitTop = 500
    ExplicitWidth = 1368
  end
  object btnEnd: TcxButton
    Left = 769
    Top = 660
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #36864#20986
    TabOrder = 0
    OnClick = btnEndClick
    ExplicitLeft = 696
    ExplicitTop = 526
  end
  object pbFile: TcxProgressBar
    Left = 115
    Top = 583
    Anchors = [akLeft, akRight, akBottom]
    Properties.Max = 100.000000000000000000
    Properties.OverloadValue = 30.000000000000000000
    Properties.PeakValue = 20.000000000000000000
    TabOrder = 1
    ExplicitTop = 449
    Width = 740
  end
  object tlMain: TcxTreeList
    Left = 115
    Top = 70
    Width = 740
    Height = 497
    Anchors = [akLeft, akTop, akRight, akBottom]
    Bands = <
      item
      end>
    Navigator.Buttons.CustomButtons = <>
    OptionsData.Editing = False
    OptionsView.GridLines = tlglBoth
    OptionsView.Indicator = True
    OptionsView.ShowRoot = False
    TabOrder = 2
    ExplicitWidth = 667
    ExplicitHeight = 363
    object ColumnIndex: TcxTreeListColumn
      Caption.Text = #24207#21495
      DataBinding.ValueType = 'Integer'
      Width = 100
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnFile: TcxTreeListColumn
      Caption.Text = #25991#20214#21517
      DataBinding.ValueType = 'String'
      Width = 401
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
      OnGetDisplayText = ColumnFileGetDisplayText
    end
    object tlBrdInfoColumn7: TcxTreeListColumn
      Caption.Text = #29366#24577
      DataBinding.ValueType = 'String'
      Width = 124
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
  object cxLabel1: TcxLabel
    Left = 36
    Top = 30
    Caption = #23548#20986#30446#24405
    Transparent = True
  end
  object btnStart: TcxButton
    Left = 670
    Top = 660
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #24320#22987
    TabOrder = 4
    OnClick = btnStartClick
    ExplicitLeft = 597
    ExplicitTop = 526
  end
  object cxLabel2: TcxLabel
    Left = 36
    Top = 584
    Anchors = [akLeft, akBottom]
    Caption = #23548#20986#36827#24230
    Transparent = True
  end
  object cxLabel3: TcxLabel
    Left = 36
    Top = 70
    Caption = #25991#20214#21015#34920
    Transparent = True
  end
  object edtPath: TcxTextEdit
    Left = 115
    Top = 29
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 7
    Text = 'c:\'
    Width = 655
  end
  object btnPath: TcxButton
    Left = 780
    Top = 29
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = #36873#25321
    OptionsImage.Glyph.SourceDPI = 96
    OptionsImage.Glyph.Data = {
      89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
      61000000017352474200AECE1CE90000000467414D410000B18F0BFC61050000
      00097048597300000EC300000EC301C76FA8640000035349444154384F95927B
      50CC5114C77FDE63643C4AED96D8212B656D6D65F550CCD0A03564DA1E928A6D
      129B572A89C8A04CADE411851E4C5BCA2216430F2496A2F24B648B4CD12E59DB
      9647D89DAFDB308C69FCE13BF399B9F7DC73CEF7CC994BFDAF0EF873BD243E1C
      CEAF6B7F150985837E1D7FEB58A89DCDD115F6490703B82A59A22FF67B7332FA
      E24221F533B768A770E8E5380F9F0BD1EE725994FBD7C2F5CE7B72D63A31B2C3
      1CC499210E0FA5D15EA82ED8812E6521AAB22391B4C0AA2A71EE843BF16E962A
      4ABED523E872DC1CCD83BC38743494C2D0AB46833C1D8551F35195130B75C379
      74BF28414D9E1805E2593822B4C5F99879A84C166023DFA28CBAB8C595D6BD6E
      04F45DC0C77A183ACFC2D0968AB73569A8CB8FC185180FE484F2509EBA12CF8A
      63F0B62C1E6D67435020B245289771883AB7814F77B53F86417D066DA59150A4
      2F822C82832BDBBD886B2CDAAB4EE093B208DA6A095ECB82D07CD419F46E160E
      2C1C8F2553C68550056227FA7D8B022AC54E5424FBA2F5761E3EAB6AA1FF508F
      DED662E8AA13A1BEE887D6937C34A5D9A261CF643C8A3347ACCB7803DBD8C89A
      92AE71A2D58D37507BD21F4F2F49A0D7DC436F533A7A1411D096FA42255B8057
      B9AE683ECCC593242BD42558A25CCC40B81D5345F66F4C9D0E77A25FDECD873C
      7A2634CA5BF8DA9A8B9EFB61D0552CC7BB4B8BD19EEF8196E38E78966A8DFA5D
      2CD4C432210D3045A08D5929696044E58A78F4635932CE8573A0EF56E213BD1D
      DD7782A1B9EE838E624F32BA339E1FE480DE3B898C6E01C5E671485F680A6F2B
      937DA4C130EAD46A1E5D91B60AD71204D06BEBD1734F046D7900DE9508D076C6
      1D2D193C34EE9F4A469F808A480632979A619323E38B9BC5483E693090CA0AE6
      D1255B3DA1C85A8F6F6FAE4057198CF75797A1A3681E719F8546C9749444B090
      4AB6BED69EA911B24DB2F9CC510EA4780881A23282ECE9C2704734C925F8A2CC
      84B6CC1F9DC4BD2E8D0FA9888D84B913BF8B66306E7AB2C6849174266104E1CF
      773F126847E7AE9C0675AD149D95D1B8BDD705997E6C6C9B6DDE1CC8354D618F
      1E3183A48D260C250CE8ABF94B87FCEC6AB3836C70759737248B59BA2837F37C
      81F5584FF26442184E18D897F74F65F959B35304AC6BEB9CCDD6190FA7CC49C8
      883098D0DFAD9F28EA07B9C8D6C7D9DF27250000000049454E44AE426082}
    TabOrder = 8
    OnClick = btnPathClick
  end
  object FolderDialog: TFolderDialog
    Caption = #36873#25321#23548#20986#30446#24405
    Options = [fdoNewDialogStyle]
    DialogPosition = fdpScreenCenter
    DialogX = 0
    DialogY = 0
    Version = '1.1.4.0'
    Left = 40
    Top = 256
  end
end
