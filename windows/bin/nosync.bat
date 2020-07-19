echo off


rem ***** DO NOT CHANGE ANYTHING ABOVE THIS LINE!!! *****


echo Copying WORKSTATION specific ini files...

xcopy "%CC_ROOT%\%CC_SYSTEM%\CONFIG\WS\%CC_WORKSTATION%\bncs_system.ini" "%CC_ROOT%\%CC_SYSTEM%\CONFIG\system" /c/i/f/y


desktopbmp.exe
