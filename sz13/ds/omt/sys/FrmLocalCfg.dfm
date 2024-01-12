inherited FormLocalCfg: TFormLocalCfg
  ClientHeight = 608
  ClientWidth = 976
  Font.Name = 'Arial'
  OnCreate = FormCreate
  ExplicitWidth = 992
  ExplicitHeight = 647
  PixelsPerInch = 96
  TextHeight = 16
  inherited MainPanel: TPanel
    Width = 976
    Height = 608
    ExplicitWidth = 976
    ExplicitHeight = 608
    object sbMain: TcxScrollBox
      Left = 0
      Top = 0
      Width = 976
      Height = 608
      Align = alClient
      LookAndFeel.NativeStyle = False
      LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 0
      Transparent = True
    end
    object gbMain: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      PanelStyle.Active = True
      Style.BorderStyle = ebs3D
      Style.Edges = []
      Style.LookAndFeel.NativeStyle = False
      Style.LookAndFeel.SkinName = 'Office2010Blue'
      StyleDisabled.LookAndFeel.NativeStyle = False
      StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
      TabOrder = 1
      Height = 608
      Width = 976
      object pcMain: TcxPageControl
        Left = 1
        Top = 1
        Width = 974
        Height = 606
        Align = alClient
        TabOrder = 0
        Properties.ActivePage = tsSMS
        Properties.CustomButtons.Buttons = <>
        Properties.TabWidth = 120
        LookAndFeel.NativeStyle = False
        LookAndFeel.SkinName = 'Office2010Blue'
        OnPageChanging = pcMainPageChanging
        ClientRectBottom = 600
        ClientRectLeft = 2
        ClientRectRight = 968
        ClientRectTop = 30
        object cxTabSheet1: TcxTabSheet
          Caption = #22522#26412#37197#32622
          ImageIndex = 1
          object gbBasicCfgMain: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            PanelStyle.Active = True
            Style.BorderStyle = ebs3D
            Style.Edges = []
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 0
            OnResize = gbBasicCfgMainResize
            Height = 570
            Width = 966
          end
        end
        object tsParam: TcxTabSheet
          Caption = #19994#21153#21442#25968
          ImageIndex = 3
          object gbServiceCfgMain: TcxGroupBox
            Left = 0
            Top = 0
            Align = alClient
            PanelStyle.Active = True
            Style.BorderStyle = ebs3D
            Style.Edges = []
            Style.LookAndFeel.NativeStyle = False
            Style.LookAndFeel.SkinName = 'Office2010Blue'
            StyleDisabled.LookAndFeel.NativeStyle = False
            StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
            TabOrder = 0
            OnResize = gbServiceCfgMainResize
            Height = 570
            Width = 966
            object gbServiceCfg: TcxGroupBox
              Left = 220
              Top = 92
              PanelStyle.Active = True
              ParentFont = False
              Style.BorderStyle = ebs3D
              Style.Edges = []
              Style.Font.Charset = ANSI_CHARSET
              Style.Font.Color = clWindowText
              Style.Font.Height = -14
              Style.Font.Name = 'Arial'
              Style.Font.Style = []
              Style.LookAndFeel.NativeStyle = False
              Style.LookAndFeel.SkinName = 'Office2010Blue'
              Style.IsFontAssigned = True
              StyleDisabled.LookAndFeel.NativeStyle = False
              StyleDisabled.LookAndFeel.SkinName = 'Office2010Blue'
              TabOrder = 0
              DesignSize = (
                453
                253)
              Height = 253
              Width = 453
              object btnCancelParam: TcxButton
                Left = 304
                Top = 201
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #25918#24323
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 12
                OnClick = btnCancelParamClick
              end
              object btnSetParam: TcxButton
                Left = 188
                Top = 201
                Width = 75
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #35774#32622
                LookAndFeel.NativeStyle = False
                LookAndFeel.SkinName = 'Office2010Blue'
                TabOrder = 6
                OnClick = btnSetParamClick
              end
              object cxLabel20: TcxLabel
                Left = 82
                Top = 29
                Caption = #26174#31034#21628#21483#24377#31383
                ParentFont = False
                Style.Font.Charset = ANSI_CHARSET
                Style.Font.Color = clWindowText
                Style.Font.Height = -14
                Style.Font.Name = 'Arial'
                Style.Font.Style = []
                Style.IsFontAssigned = True
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 39
              end
              object cxLabel21: TcxLabel
                Left = 82
                Top = 59
                Caption = #35270#39057#36716#21457#26684#24335
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 69
              end
              object cxLabel22: TcxLabel
                Left = 26
                Top = 89
                Caption = #20056#23458#23545#35762#35270#39057#20256#36755#21327#35758
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 99
              end
              object cxLabel23: TcxLabel
                Left = 54
                Top = 119
                Caption = #20056#23458#23545#35762#24405#38899#27169#24335
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                AnchorX = 170
                AnchorY = 129
              end
              object cbShowCallWin: TcxCheckBox
                Left = 188
                Top = 29
                Caption = #26159
                State = cbsChecked
                TabOrder = 0
                Transparent = True
              end
              object cbOriginalVideoFormat: TcxCheckBox
                Left = 188
                Top = 59
                Caption = #21407#22987#26684#24335
                TabOrder = 1
                Transparent = True
              end
              object cbIPHRecMergeMode: TcxCheckBox
                Left = 188
                Top = 119
                Caption = #24038#21491#22768#36947#21512#24182
                State = cbsChecked
                TabOrder = 4
                Transparent = True
              end
              object rbIPHVideoTCPMode: TcxRadioButton
                Left = 188
                Top = 91
                Width = 80
                Height = 17
                Caption = 'TCP'
                Checked = True
                TabOrder = 2
                TabStop = True
                Transparent = True
              end
              object rbIPHVideoUDPMode: TcxRadioButton
                Left = 307
                Top = 91
                Width = 80
                Height = 17
                Caption = 'UDP'
                TabOrder = 3
                Transparent = True
              end
              object cxLabel24: TcxLabel
                Left = 64
                Top = 149
                Caption = 'OMC'#26381#21153#22120#22320#22336
                Properties.Alignment.Horz = taRightJustify
                Properties.Alignment.Vert = taVCenter
                Transparent = True
                Visible = False
                AnchorX = 170
                AnchorY = 159
              end
              object btnDefaultParam: TcxButton
                Left = 52
                Top = 201
                Width = 99
                Height = 25
                Anchors = [akLeft, akBottom]
                Caption = #24674#22797#32570#30465#20540
                TabOrder = 5
                OnClick = btnDefaultParamClick
              end
            end
          end
        end
        object tsSMS: TcxTabSheet
          Caption = #32456#31471#30701#28040#24687#27169#26495
          ImageIndex = 2
        end
      end
    end
  end
end
