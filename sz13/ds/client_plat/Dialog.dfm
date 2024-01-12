object DialogForm: TDialogForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'DialogForm'
  ClientHeight = 215
  ClientWidth = 457
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object MDIFormMainPanel: TPanel
    Left = 0
    Top = 0
    Width = 457
    Height = 215
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object PanelButton: TPanel
      Left = 0
      Top = 162
      Width = 457
      Height = 53
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
    end
  end
end
