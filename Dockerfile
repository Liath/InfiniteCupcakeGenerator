FROM mcr.microsoft.com/dotnet/framework/sdk:4.8-windowsservercore-ltsc2019

# Restore the default Windows shell for correct batch processing.
SHELL ["cmd", "/S", "/C"]

# As good a place to put temp files as any
RUN mkdir C:\\TEMP

# Download the Build Tools bootstrapper.
RUN bitsadmin /transfer vs_buildtools /download /dynamic https://aka.ms/vs/16/release/vs_buildtools.exe C:\\TEMP\\vs_buildtools.exe

# https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2019#c-build-tools
# https://stackoverflow.com/questions/56419556/how-do-you-get-cl-exe-if-msbuild-must-be-installed-via-command-line
RUN C:\\TEMP\\vs_buildtools.exe --quiet --wait --norestart --nocache \
  --installPath C:\BuildTools \
  --add Microsoft.VisualStudio.Workload.MSBuildTools \
  --add Microsoft.VisualStudio.Workload.VCTools \
  --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 \
  --add Microsoft.VisualStudio.Component.Windows10SDK \
  --add Microsoft.VisualStudio.Component.Windows10SDK.18362 \
  --remove Microsoft.VisualStudio.Component.Windows10SDK.10240 \
  --remove Microsoft.VisualStudio.Component.Windows10SDK.10586 \
  --remove Microsoft.VisualStudio.Component.Windows10SDK.14393 \
  --remove Microsoft.VisualStudio.Component.Windows81SDK \
  || IF "%ERRORLEVEL%"=="3010" EXIT 0

# grab 7zip
RUN bitsadmin /transfer 7zip /download /dynamic \
  https://www.7-zip.org/a/7z1900-x64.exe \
  C:\\TEMP\\7z1900-x64.exe

# trailing slash on C:\Windows is needed as the 7z installer chops off the end.
RUN powershell -Command "saps -Wait C:\\TEMP\\7z1900-x64.exe -ArgumentList '/S /D=C:\\Windows\\'"

# grab UPX
RUN bitsadmin /transfer upx /download /dynamic \
  https://github.com/upx/upx/releases/download/v3.96/upx-3.96-win64.zip \
  C:\\TEMP\\upx-3.96-win64.zip && \
  7z.exe e C:\\temp\\upx-3.96-win64.zip upx-3.96-win64\upx.exe -oC:\Windows

VOLUME C:\\build
WORKDIR C:\\build
ENTRYPOINT ["C:\\BuildTools\\Common7\\Tools\\VsDevCmd.bat", "&&", "powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass", "C:\\Build\\build.ps1"]
