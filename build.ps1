if(![System.IO.File]::Exists("C:\build\ufmod.h")){
  echo "ufmod not found, fetching..."

  bitsadmin /transfer ufmod /download /dynamic `
    https://master.dl.sourceforge.net/project/ufmod/uFMOD%20library%20for%20Win32/1.25.2/ufmod-1.25.2a-win32.7z `
    C:\TEMP\ufmod-1.25.2a-win32.7z
  7z.exe e -aos C:\temp\ufmod-1.25.2a-win32.7z "Examples\C\WinMM\src\ufmod.*"
}

echo "Compiling resource files..."
# Generate resource file
rc /r ICG.rc

echo "Compiling source code..."
# https://www.catch22.net/tuts/win32/reducing-executable-size
# https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-alphabetically?view=msvc-160
# https://forum.osdev.org/viewtopic.php?f=1&t=32980
cl /c /O1 /Os /Oy /Zl /Gy /GS- /EHs-c- /Zc:threadSafeInit- ICG.cpp

# Link it all together. In a loop because link.exe occasionally likes to output
# an empty file of the right size but filled with 0s. :(
rm icg.exe
while (!(Get-Content icg.exe -Encoding byte -TotalCount 1 -ErrorAction SilentlyContinue)) {
echo "Attempting Link... (occasionally fails, no worries though because we'll retry)"
link /SUBSYSTEM:WINDOWS /NODEFAULTLIB /SAFESEH:NO /ENTRY:"winMainCRTStartup" `
  /OUT:icg.exe ICG.obj Kernel32.lib Ws2_32.lib Gdi32.lib User32.lib winmm.lib `
  ufmod.obj ICG.res /INCREMENTAL:NO /FILEALIGN:512 /ALIGN:4096 /OPT:REF
}

echo "Compressing exe, please wait (can take like 30 minutes)..."
# Takes forever but gets our 1912kb exe down to 43kb!
upx --ultra-brute icg.exe
