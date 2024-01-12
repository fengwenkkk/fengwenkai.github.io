object ServiceMon: TServiceMon
  OldCreateOrder = False
  DisplayName = 'WatchDog'
  OnStart = ServiceStart
  OnStop = ServiceStop
  Height = 150
  Width = 215
end
