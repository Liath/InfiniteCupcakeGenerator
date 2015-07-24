cl /c /O1 /GS- /Oi- /Og /Os /Oy /Gy /EHs-c- ICG.cpp
rc /r ICG.rc
link /SUBSYSTEM:WINDOWS /NODEFAULTLIB /SAFESEH:NO /ENTRY:"winMainCRTStartup" /OUT:icg.exe ICG.obj Kernel32.lib Ws2_32.lib Gdi32.lib User32.lib winmm.lib ufmod.lib ICG.res /INCREMENTAL:NO /FILEALIGN:512 /ALIGN:4096 /OPT:REF
upx --ultra-brute icg.exe