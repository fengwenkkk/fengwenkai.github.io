object FramePlayAndRec: TFramePlayAndRec
  Left = 0
  Top = 0
  Width = 471
  Height = 303
  TabOrder = 0
  OnResize = FrameResize
  object PanelVideo: TPanel
    Left = 248
    Top = 152
    Width = 121
    Height = 97
    BevelOuter = bvNone
    TabOrder = 0
    object Image: TImage
      Left = 0
      Top = 0
      Width = 121
      Height = 97
      Align = alClient
      Stretch = True
      ExplicitLeft = 3
      ExplicitTop = 29
      ExplicitWidth = 243
      ExplicitHeight = 169
    end
    object gbVideo: TcxGroupBox
      Left = 0
      Top = 0
      Align = alClient
      PanelStyle.Active = True
      Style.Edges = []
      TabOrder = 0
      Visible = False
      Height = 97
      Width = 121
    end
  end
end
