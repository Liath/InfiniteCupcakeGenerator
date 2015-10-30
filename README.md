####Introduction
Need a swanky keygen template? I didn't!(and no I don't need one) But I made it anyways so feel free to use it anywhere you so choose.

Ignore the current crypto codes, they are crippled versions from some research elsewhere that are here as examples.

I have no idea how uFMOD is licensed so it's not included here. [You can grab it here though.](http://ufmod.sourceforge.net/) You'll need to place ufmod.lib and winmm.lib in this folder to compile.
####How I compile with the VS-13 cli tools
```
cl /c /O1 /GS- /Oi- /Og /Os /Oy /Gy /EHs-c- ICG.cpp

rc /r ICG.rc

link /SUBSYSTEM:WINDOWS /NODEFAULTLIB /SAFESEH:NO /ENTRY:"winMainCRTStartup" /OUT:icg.exe ICG.obj Kernel32.lib Ws2_32.lib Gdi32.lib User32.lib winmm.lib ufmod.lib ICG.res /INCREMENTAL:NO /FILEALIGN:512 /ALIGN:4096 /OPT:REF

upx --ultra-brute icg.exe
```

####References (As I remember the particularly useful ones):
* [CodeProject: How to Use a Font Without Installing it by Shao Voon Wong](http://www.codeproject.com/Articles/42041/How-to-Use-a-Font-Without-Installing-it)
* [StackOverflow: Transparency in GDI DCs answer by Remy Lebeau](http://stackoverflow.com/questions/28846219/transparency-in-gdi-dcs)
* [DaFont: Dimitri Swank by Fontalicious](http://www.dafont.com/dimitri.font)
