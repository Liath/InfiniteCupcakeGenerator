Need a swanky keygen skin? I didn't! But I made one anyways. Feel free to use it anywhere you so choose.

Ignore the current crypto codes, they are crippled versions from some research elsewhere that are here as examples.

######How I compile with the VS-13 cli tools
```
cl /c /O1 /GS- /Oi- /Og /Os /Oy /Gy /EHs-c- ICG.cpp

rc /r ICG.rc

link /SUBSYSTEM:WINDOWS /NODEFAULTLIB /SAFESEH:NO /ENTRY:"winMainCRTStartup" /OUT:icg.exe ICG.obj Kernel32.lib Ws2_32.lib Gdi32.lib User32.lib winmm.lib ufmod.lib ICG.res /INCREMENTAL:NO /FILEALIGN:512 /ALIGN:4096 /OPT:REF

upx --ultra-brute icg.exe
```