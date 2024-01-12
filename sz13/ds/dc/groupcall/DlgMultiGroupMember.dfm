object DialogMultiGroupMember: TDialogMultiGroupMember
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #32676#32452#25104#21592
  ClientHeight = 671
  ClientWidth = 894
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnDeactivate = FormDeactivate
  PixelsPerInch = 96
  TextHeight = 18
  object gbCtrl: TcxGroupBox
    Left = 0
    Top = 600
    Align = alBottom
    Style.BorderStyle = ebs3D
    Style.Edges = []
    Style.LookAndFeel.NativeStyle = False
    Style.LookAndFeel.SkinName = 'Office2010Blue'
    StyleDisabled.LookAndFeel.NativeStyle = False
    StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
    TabOrder = 0
    DesignSize = (
      894
      71)
    Height = 71
    Width = 894
    object btnOK: TcxButton
      Left = 688
      Top = 23
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #30830#35748
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnClose: TcxButton
      Left = 782
      Top = 23
      Width = 75
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = #21462#28040
      TabOrder = 1
      OnClick = btnCloseClick
    end
    object cbShowAll: TcxCheckBox
      Left = 12
      Top = 25
      Caption = #26174#31034#20840#37096
      Properties.OnChange = cbShowAllPropertiesChange
      TabOrder = 2
      Transparent = True
    end
  end
  object tlMain: TcxTreeList
    Left = 0
    Top = 0
    Width = 894
    Height = 600
    Align = alClient
    Bands = <
      item
      end>
    LookAndFeel.NativeStyle = False
    LookAndFeel.ScrollbarMode = sbmClassic
    Navigator.Buttons.CustomButtons = <>
    OptionsBehavior.Sorting = False
    OptionsCustomizing.BandCustomizing = False
    OptionsData.Editing = False
    OptionsSelection.HideSelection = True
    OptionsSelection.InvertSelect = False
    OptionsView.ColumnAutoWidth = True
    OptionsView.CheckGroups = True
    OptionsView.GridLines = tlglBoth
    OptionsView.TreeLineStyle = tllsNone
    Styles.UseOddEvenStyles = bFalse
    TabOrder = 1
    object ColumnName: TcxTreeListColumn
      Caption.Text = #25104#21592#32676#32452#21517#31216
      DataBinding.ValueType = 'String'
      Width = 220
      Position.ColIndex = 0
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnGroupID: TcxTreeListColumn
      Caption.Text = #25104#21592#32676#32452#21495#30721
      DataBinding.ValueType = 'Integer'
      Width = 154
      Position.ColIndex = 1
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnMultiGroupID: TcxTreeListColumn
      Caption.Text = #25152#23646#22810#36873#32452
      DataBinding.ValueType = 'String'
      Width = 184
      Position.ColIndex = 2
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
    object ColumnMultiGroupCreator: TcxTreeListColumn
      Caption.Text = #25152#23646#22810#36873#32452#21019#24314#32773
      DataBinding.ValueType = 'String'
      Width = 185
      Position.ColIndex = 3
      Position.RowIndex = 0
      Position.BandIndex = 0
      Summary.FooterSummaryItems = <>
      Summary.GroupFooterSummaryItems = <>
    end
  end
  object ImageListUser: TcxImageList
    SourceDPI = 96
    FormatVersion = 1
    DesignInfo = 8388800
    ImageInfo = <
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000B17E4BF7B8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFFB8824DFFB17E4BF7000000000000000000000000000000000000
          0000000000007A5532A9B8824DFFB8824DFFB8824DFFB8824DFFB8824DFFB882
          4DFFB8824DFFB8824DFF805A35B1000000000000000000000000000000000000
          000000000000050402075C412780A27243E2A57345E43928184F3928184FA573
          45E4A27243E25C41278006050309000000000000000000000000B17E4BF7B882
          4DFFB8824DFFA67545E85C41267F0302010420170E2D2D20133E2D20133E2117
          0E2E030201045C41267FA67545E8B8824DFFB8824DFFB17E4BF77A5532A9B882
          4DFFB8824DFFB8824DFFB8824DFF523A2272543B2374B8824DFFB8824DFF543B
          2374523A2272B8824DFFB8824DFFB8824DFFB8824DFF805A35B1050402075C41
          2780A27243E2A57345E43526164A0B08050FB27C49F6B8824DFFB8824DFFB27C
          49F60B08050F3526164AA57345E4A27243E25C41278006050309000000000000
          00000302010420170E2D18110A21563D2477B8824DFFB8824DFFB8824DFFB882
          4DFF563D247718110A2121170E2E030201040000000000000000000000000000
          000000000000543B2374795633A83F2C1A57B8824DFFB8824DFFB8824DFFB882
          4DFF3F2C1A57795633A8543B2374000000000000000000000000000000000000
          00000B08050FB27C49F692673DCA261B1035B8824DFFB8824DFFB8824DFFB882
          4DFF261B103592673DCAB27C49F60B08050F0000000000000000000000000000
          0000563D2477B8824DFFB6804DFE01010001704E2F9BAE7C48F3AE7C48F37350
          2F9F01010102B7814BFDB8824DFF563D24770000000000000000000000000000
          00003F2C1A57B8824DFFB8824DFF94693ECD48331E6403020104030201044531
          1D6090663DC9B8824DFFB8824DFF3F2C1A570000000000000000000000000000
          0000261B1035B8824DFFB8824DFFB8824DFFB8824DFF261B1035261B1035B882
          4DFFB8824DFFB8824DFFB8824DFF261B10350000000000000000000000000000
          000001010001704E2F9BAE7C48F3AE7C48F373502F9F0101010201010001704E
          2F9BAE7C48F3AE7C48F373502F9F010101020000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          00000000000C2B1E19D136251FFF36251FFF35241EFF32221DFF32221DFF3524
          1EFF36251FFF36251FFF2B1E19D10000000C0000000000000000000000000000
          00000000000D372620FF46342DFF45322BFF423029FF867C78FF867C78FF4230
          29FF45322BFF46342DFF372620FF0000000D0000000000000000000000000000
          00000000000D392821FF4A372FFF4A372FFF48362EFF47352DFF47352DFF4836
          2EFF4A372FFF4A372FFF392821FF0000000D0000000000000000000000000000
          00000000000D3B2A23FF926041FFA56B45FFA56B45FFA46B45FFA46B45FFA56B
          45FFA56B45FF926041FF3B2A23FF0000000D0000000000000000000000000000
          00000000000D3E2B24FFA9714BFFDDAF77FFDDAF77FFDDAF77FFDDAF77FFDDAF
          77FFDDAF77FFA9714BFF3E2B24FF0000000D0000000000000000000000000000
          00000000000D402D26FFAD7752FFDFB57FFFDFB57FFFDFB57FFFDFB57FFFDFB5
          7FFFDFB57FFFAD7752FF402D26FF0000000D0000000000000000000000000000
          00000000000D432F28FFB17E59FFE2BB87FFE2BB87FFE2BB87FFE2BB87FFE2BB
          87FFE2BB87FFB17E59FF432F28FF0000000D0000000000000000000000000000
          00000000000D45312AFFB68560FFE5C292FFE7C597FFE7C699FFE7C698FFE6C4
          96FFE5C292FFB68560FF45312AFF0000000D0000000000000000000000000000
          00000000000D513D35FFC59D7DFFEDD3ADFFEDD3ADFFEDD3ADFFEDD3ADFFEDD3
          ADFFEDD3ADFFC59D7DFF513D35FF0000000D0000000000000000000000000000
          00000201010D6E5D57FFCCA98BFFF0D8B3FFF0D8B3FFF0D8B3FFF0D8B3FFF0D8
          B3FFF0D8B3FFCCA98BFF6E5D57FF0201010D0000000000000000000000000000
          00000303030D705F59FFCFAD91FFF1DDB9FFF1DDB9FFF1DDB9FFF1DDB9FFF1DD
          B9FFF1DDB9FFCFAD91FF705F59FF0303030D0000000000000000000000000000
          00000303030D726059FFD2B295FFF3E1BEFFF3E1BFFFF3E1BFFFF3E1BFFFF3E1
          BFFFF3E1BEFFD2B295FF726059FF0303030D0000000000000000000000000000
          00000303030D74615BFFCAAD94FFD5B699FFD5B79AFFD5B89BFFD5B89BFFD5B7
          9AFFD5B699FFCAAD94FF74615BFF0303030D0000000000000000000000000000
          00000202020C75635CFFAA9389FFAA9389FF8D7971FF6D5D57FF6D5D57FF8D79
          71FFAA9389FFAA9389FF75635CFF0202020C0000000000000000000000000000
          00000202020876645DFFAE978DFFAD968CFFAD968CFFAD968CFFAD968CFFAD96
          8CFFAD968CFFAE978DFF76645DFF020202080000000000000000000000000000
          0000000000025E4F49C978655DFF78655DFF78655DFF78655DFF78655DFF7865
          5DFF78655DFF78655DFF5D4E4ACA010101030000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          000000000000252525FF262626FF262626FF252525FF232323FF232323FF2525
          25FF262626FF262626FF252525FF000000000000000000000000000000000000
          000000000000272727FF353535FF333333FF313131FF7C7C7CFF7C7C7CFF3131
          31FF333333FF353535FF272727FF000000000000000000000000000000000000
          000000000000282828FF373737FF373737FF363636FF353535FF353535FF3636
          36FF373737FF373737FF282828FF000000000000000000000000000000000000
          0000000000002A2A2AFF5F5F5FFF696969FF696969FF696969FF696969FF6969
          69FF696969FF5F5F5FFF2A2A2AFF000000000000000000000000000000000000
          0000000000002C2C2CFF6F6F6FFFA7A7A7FFA7A7A7FFA7A7A7FFA7A7A7FFA7A7
          A7FFA7A7A7FF6F6F6FFF2C2C2CFF000000000000000000000000000000000000
          0000000000002E2E2EFF757575FFADADADFFADADADFFADADADFFADADADFFADAD
          ADFFADADADFF757575FF2E2E2EFF000000000000000000000000000000000000
          000000000000303030FF7B7B7BFFB3B3B3FFB3B3B3FFB3B3B3FFB3B3B3FFB3B3
          B3FFB3B3B3FF7B7B7BFF303030FF000000000000000000000000000000000000
          000000000000323232FF828282FFBABABAFFBEBEBEFFBFBFBFFFBFBFBFFFBDBD
          BDFFBABABAFF828282FF323232FF000000000000000000000000000000000000
          0000000000003E3E3EFF9A9A9AFFCDCDCDFFCDCDCDFFCDCDCDFFCDCDCDFFCDCD
          CDFFCDCDCDFF9A9A9AFF3E3E3EFF000000000000000000000000000000000000
          0000000000005E5E5EFFA6A6A6FFD2D2D2FFD2D2D2FFD2D2D2FFD2D2D2FFD2D2
          D2FFD2D2D2FFA6A6A6FF5E5E5EFF000000000000000000000000000000000000
          000000000000606060FFAAAAAAFFD7D7D7FFD7D7D7FFD7D7D7FFD7D7D7FFD7D7
          D7FFD7D7D7FFAAAAAAFF606060FF000000000000000000000000000000000000
          000000000000616161FFAFAFAFFFDBDBDBFFDBDBDBFFDBDBDBFFDBDBDBFFDBDB
          DBFFDBDBDBFFAFAFAFFF616161FF000000000000000000000000000000000000
          000000000000626262FFAAAAAAFFB3B3B3FFB4B4B4FFB4B4B4FFB4B4B4FFB4B4
          B4FFB3B3B3FFAAAAAAFF626262FF000000000000000000000000000000000000
          000000000000646464FF939393FF939393FF7A7A7AFF5E5E5EFF5E5E5EFF7A7A
          7AFF939393FF939393FF646464FF000000000000000000000000000000000000
          000000000000656565FF979797FF969696FF969696FF969696FF969696FF9696
          96FF969696FF979797FF656565FF000000000000000000000000000000000000
          000000000000646464FF656565FF656565FF656565FF656565FF656565FF6565
          65FF656565FF656565FF646464FF000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000C297
          5CFFBF8838FFBF8430FFBF8430FFBF8430FFBF8430FFBF8430FFBF8430FFBF84
          30FFBF8430FFBF8430FFBF8430FFC28C3FFFC6B49AFF0000000000000000BF84
          30FFFEFAF3FFFFEACEFFFFEACEFFFFEACEFFFFEACEFFFFEACEFFFFEACEFFFFEA
          CEFFFFEACEFFFFEACEFFFFEACEFFFFEACEFFC28938FF0000000000000000BF84
          30FF00000000FFE6C5FFC18A35FFBF8A3BFFBF8A3BFFBF8A3BFFBF8A3BFFBF8A
          3BFFBF8A3BFFC18A35FFFFE6C5FFFCE4C4FFBF8430FF0000000000000000BF84
          30FF00000000C18A38FFF2CD71FFF2CD71FFF2CD71FFF2CD71FFF2CD71FFF2CD
          71FFF2CD71FFF2CD71FFC28B35FFF8DCB6FFBF8430FF0000000000000000BF84
          30FF00000000BF8A3BFFEDC25FFFEDC25FFFEDC25FFFEDC25FFFEDC25FFFEDC2
          5FFFEDC25FFFEDC25FFFBF8A3BFFF8DBB4FFBF8430FF0000000000000000BF84
          30FF00000000BF8A3BFFE6B449FFE6B449FFE6B449FFE6B449FFE6B449FFE6B4
          49FFE6B449FFE6B449FFBF8A3BFFF8DBB4FFBF8430FF0000000000000000BF84
          30FF00000000BF8A3BFFDFA633FFDFA633FFDFA633FFDFA633FFDFA633FFDFA6
          33FFDFA633FFDFA633FFBF8A3BFFF8DBB4FFBF8430FF0000000000000000BF84
          30FF00000000BF8A3BFFD79619FFD79619FFD79619FFD79619FFD79619FFD796
          19FFD79619FFD79619FFBF8A3BFFF8DBB4FFBF8430FF0000000000000000BF84
          30FF00000000C08937FFEFC786FFF0C989FFF0C989FFF0C989FFF0C989FFF0C9
          89FFF0C989FFF0C989FFC08835FFF9DDB7FFBF8430FF0000000000000000BF84
          30FFFFF9F2FFF0CFA2FFD5A563FFC28C3DFFBF8A3BFFBF8A3BFFBF8A3BFFBF8A
          3BFFBF8A3BFFBF8A3BFFFFE6C5FFFFE6C5FFBF8430FF0000000000000000C99A
          56FFF9F4EEFF0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000FBF8F3FFBF8A3BFF0000000000000000FEFE
          FEFFD0A66BFFBF8430FFBF8430FFBF8430FFBF8430FFBF8430FFBF8430FFBF84
          30FFBF8430FFBF8430FFBF8430FFD0A76CFFFBFAF8FF00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000008E8E
          8EFF7B7B7BFF767676FF767676FF767676FF767676FF767676FF767676FF7676
          76FF767676FF767676FF767676FF808080FFB0B0B0FF00000000000000007676
          76FFF9F9F9FFE6E6E6FFE6E6E6FFE6E6E6FFE6E6E6FFE6E6E6FFE6E6E6FFE6E6
          E6FFE6E6E6FFE6E6E6FFE6E6E6FFE6E6E6FF7C7C7CFF00000000000000007676
          76FF00000000E1E1E1FF7C7C7CFF7D7D7DFF7D7D7DFF7D7D7DFF7D7D7DFF7D7D
          7DFF7D7D7DFF7C7C7CFFE1E1E1FFDFDFDFFF767676FF00000000000000007676
          76FF000000007C7C7CFFBBBBBBFFBBBBBBFFBBBBBBFFBBBBBBFFBBBBBBFFBBBB
          BBFFBBBBBBFFBBBBBBFF7C7C7CFFD6D6D6FF767676FF00000000000000007676
          76FF000000007D7D7DFFAFAFAFFFAFAFAFFFAFAFAFFFAFAFAFFFAFAFAFFFAFAF
          AFFFAFAFAFFFAFAFAFFF7D7D7DFFD5D5D5FF767676FF00000000000000007676
          76FF000000007D7D7DFFA0A0A0FFA0A0A0FFA0A0A0FFA0A0A0FFA0A0A0FFA0A0
          A0FFA0A0A0FFA0A0A0FF7D7D7DFFD5D5D5FF767676FF00000000000000007676
          76FF000000007D7D7DFF909090FF909090FF909090FF909090FF909090FF9090
          90FF909090FF909090FF7D7D7DFFD5D5D5FF767676FF00000000000000007676
          76FF000000007D7D7DFF7F7F7FFF7F7F7FFF7F7F7FFF7F7F7FFF7F7F7FFF7F7F
          7FFF7F7F7FFF7F7F7FFF7D7D7DFFD5D5D5FF767676FF00000000000000007676
          76FF000000007B7B7BFFBCBCBCFFBEBEBEFFBEBEBEFFBEBEBEFFBEBEBEFFBEBE
          BEFFBEBEBEFFBEBEBEFF7A7A7AFFD7D7D7FF767676FF00000000000000007676
          76FFF8F8F8FFC8C8C8FF9B9B9BFF7F7F7FFF7D7D7DFF7D7D7DFF7D7D7DFF7D7D
          7DFF7D7D7DFF7D7D7DFFE1E1E1FFE1E1E1FF767676FF00000000000000008F8F
          8FFFF3F3F3FF0000000000000000000000000000000000000000000000000000
          0000000000000000000000000000F7F7F7FF7D7D7DFF0000000000000000FEFE
          FEFF9D9D9DFF767676FF767676FF767676FF767676FF767676FF767676FF7676
          76FF767676FF767676FF767676FF9D9D9DFFFAFAFAFF00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000473E
          1B4DA18C3DACAE8E3FBAAC8D3EB7AC8D3EB7AC8D3EB7AC8D3EB7AC8D3EB7AC8D
          3EB7AC8D3EB7AC8D3EB7AB8C3EB7B09943BD3E3417420000000041361741BA9E
          46BB39331739201B0C20231E0D23231E0D23231E0D23231E0D23231E0D23231E
          0D23231E0D23231E0D231D190B1D7F6B2F7F78622D780000000081692E826A55
          266B000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000614E2362745E2B740000000043371843B99B
          45BA2D26112D1D170A1D261F0E26261F0D26261F0E26201C0C20070502070000
          000006050206070603070000000065522566745E2B7400000000000000006F5C
          296FF3D05CF5B29040B3A2843BA3AA893EABA6883DA7C4A549C675602B767363
          2C73B29342B3AF8F3FB08D77358E6E58276E705B297000000000000000000000
          000073612B74B39844B414110715000000000000000052441E53957E3896AA8B
          3EAB382E1538221C0C22B29040B37E662D7F6E59286E00000000000000000000
          00000000000078662D78B19542B23C33173C392F153985713386937C3794A384
          3BA4322912321B170A1BAC8A3EAD7B632C7B6E59286E00000000000000000000
          0000000000000000000075632C75F4D35FF6B09041B1A08A3EA158451F588974
          338ABF9D46C1BD9945BE9C813A9E7F652D7F755F2B7500000000000000000000
          00000000000000000000000000005C4F235CAF9843B0A0863CA1A58E3FA8A990
          3FAAAD933FAEAD9341AFA58D3EA8C4A84CC54C411D4C00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000B4B4
          B4FFB6B6B6FFADADADFFAEAEAEFFAEAEAEFFAEAEAEFFAEAEAEFFAEAEAEFFAEAE
          AEFFAEAEAEFFAEAEAEFFADADADFFB5B5B5FFB1B1B1FF00000000BABABAFFBEBE
          BEFFC8C8C8FFBEBEBEFFC0C0C0FFC0C0C0FFC0C0C0FFC0C0C0FFC0C0C0FFC0C0
          C0FFC0C0C0FFC0C0C0FFC1C1C1FFBDBDBDFFBABABAFF00000000B6B6B6FFB4B4
          B4FF000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000B5B5B5FFB8B8B8FF00000000B9B9B9FFBCBC
          BCFFBEBEBEFFB4B4B4FFB9B9B9FFB8B8B8FFB9B9B9FFC3C3C3FFA4A4A4FF0000
          0000BABABAFFC3C3C3FF00000000B7B7B7FFB8B8B8FF0000000000000000BBBB
          BBFFBFBFBFFFB7B7B7FFB7B7B7FFB7B7B7FFB8B8B8FFBCBCBCFFB8B8B8FFC1C1
          C1FFBABABAFFB8B8B8FFBDBDBDFFB5B5B5FFB8B8B8FF00000000000000000000
          0000BCBCBCFFBFBFBFFFB4B4B4FF0000000000000000B8B8B8FFBDBDBDFFB8B8
          B8FFBABABAFFBABABAFFB7B7B7FFB6B6B6FFB8B8B8FF00000000000000000000
          000000000000BFBFBFFFBDBDBDFFBFBFBFFFBABABAFFBEBEBEFFBCBCBCFFB7B7
          B7FFB9B9B9FFBFBFBFFFB6B6B6FFB7B7B7FFB8B8B8FF00000000000000000000
          00000000000000000000BEBEBEFFC1C1C1FFB9B9B9FFC1C1C1FFB2B2B2FFBDBD
          BDFFB9B9B9FFB7B7B7FFB9B9B9FFB5B5B5FFB8B8B8FF00000000000000000000
          0000000000000000000000000000C0C0C0FFC1C1C1FFBDBDBDFFBEBEBEFFBEBE
          BEFFBDBDBDFFBDBDBDFFBDBDBDFFC1C1C1FFC0C0C0FF00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000F59D14FFF79908FFF79908FFF79908FFF79908FFF79908FFF79908FFF799
          08FFF6A01BFF0000000000000000000000000000000000000000000000000000
          0000EFBB66FFF89D07FFF89D07FFF89D07FFF89D07FFF89D07FFF89D07FFF89D
          07FFEFBB66FF0000000000000000000000000000000000000000000000000000
          000000000000F1B956FFF9A207FFF9A207FFF7A616FFF9A207FFF9A207FFF1BA
          56FF000000000000000000000000000000000000000000000000000000000000
          00000000000000000000E7D8BBFFEBCF97FFECCB8AFFEBCF98FFE7D8BBFF0000
          0000000000000000000000000000000000000000000000000000000000000000
          00000000000000000000F4BB3DFFFAAE06FFFAAE06FFFAAE06FFF4BB3DFF0000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000EECE81FFFBB306FFFBB306FFFBB306FFFBB306FFFBB306FFEDCE
          82FF000000000000000000000000000000000000000000000000000000000000
          000000000000F1CB66FFFCB805FFFCB805FFFCB805FFFCB805FFFCB805FFF1CB
          66FF000000000000000000000000000000000000000000000000000000000000
          000000000000E9DAB0FFFDBE08FFFDBD05FFFDBD05FFFDBD05FFFDBE08FFE9DB
          B2FF000000000000000000000000000000000000000000000000000000000000
          00000000000000000000EAD8A1FFF8C62AFFFDC109FFF8C62BFFEAD8A1FF0000
          0000E8D1ACFFF89B07FFF79D0DFFE7D2B0FF0000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000F2B7
          4DFFF4B442FFF4B442FFF4B442FFF4B442FFF1B956FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000F4B8
          3AFFFAAB06FFFAAB06FFFAAB06FFFAAB06FFF3BC47FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000F5BE
          3AFFFBB306FFFBB306FFFBB306FFFBB306FFF4C147FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000F6C4
          39FFFCBB05FFFCBB05FFFCBB05FFFCBB05FFF5C747FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000F4CD
          56FFF5CC4FFFF5CC4FFFF5CC4FFFF5CC4FFFF3CE5FFF00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end
      item
        Image.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000868686FF818181FF818181FF818181FF818181FF818181FF818181FF8181
          81FF8A8A8AFF0000000000000000000000000000000000000000000000000000
          0000ACACACFF838383FF838383FF838383FF838383FF838383FF838383FF8383
          83FFACACACFF0000000000000000000000000000000000000000000000000000
          000000000000A7A7A7FF868686FF868686FF8C8C8CFF868686FF868686FFA8A8
          A8FF000000000000000000000000000000000000000000000000000000000000
          00000000000000000000D3D3D3FFC5C5C5FFBFBFBFFFC5C5C5FFD3D3D3FF0000
          0000000000000000000000000000000000000000000000000000000000000000
          00000000000000000000A3A3A3FF8E8E8EFF8E8E8EFF8E8E8EFFA3A3A3FF0000
          0000000000000000000000000000000000000000000000000000000000000000
          000000000000BFBFBFFF919191FF919191FF919191FF919191FF919191FFBFBF
          BFFF000000000000000000000000000000000000000000000000000000000000
          000000000000B7B7B7FF949494FF949494FF949494FF949494FF949494FFB7B7
          B7FF000000000000000000000000000000000000000000000000000000000000
          000000000000D1D1D1FF989898FF979797FF979797FF979797FF989898FFD3D3
          D3FF000000000000000000000000000000000000000000000000000000000000
          00000000000000000000CDCDCDFFA5A5A5FF9B9B9BFFA6A6A6FFCDCDCDFF0000
          0000CBCBCBFF828282FF848484FFCCCCCCFF0000000000000000000000000000
          000000000000000000000000000000000000000000000000000000000000A4A4
          A4FFA0A0A0FFA0A0A0FFA0A0A0FFA0A0A0FFA7A7A7FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000A0A0
          A0FF8C8C8CFF8C8C8CFF8C8C8CFF8C8C8CFFA6A6A6FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000A4A4
          A4FF919191FF919191FF919191FF919191FFA9A9A9FF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000A8A8
          A8FF969696FF969696FF969696FF969696FFADADADFF00000000000000000000
          000000000000000000000000000000000000000000000000000000000000B4B4
          B4FFB2B2B2FFB2B2B2FFB2B2B2FFB2B2B2FFB7B7B7FF00000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000}
      end>
  end
end
