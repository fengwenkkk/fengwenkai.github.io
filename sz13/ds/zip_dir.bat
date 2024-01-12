
set ZIP_FILE = ds.rar
rem set ZIP_FILE = %1

rar.exe a -r %ZIP_FILE% -n*.c -n*.cpp -n*.h -n*.hpp -n*.dfm -n*.cbproj *.dc.groupproj dc gos gplat client_if client_plat ds_public dtp eLTE_SDK ffmpeg_api ffmpeg4.0.1 libcurl snmp

rar.exe a -r d:\ds.rar -n*.c -n*.cpp -n*.h -n*.hpp -n*.dfm -n*.cbproj *.dc.groupproj dc gos gplat client_if client_plat ds_public dtp eLTE_SDK ffmpeg_api ffmpeg4.0.1 libcurl snmp