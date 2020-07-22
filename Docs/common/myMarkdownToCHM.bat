set toolFolder=%~dp0
 
For /F %%A in ("%1") do (
    Set Folder=%%~dA%%~pA
)
For %%A in ("%1") do (
    Set Name=%%~nA
)
 
echo ToolFolder is: %toolFolder%
echo Folder is:     %Folder%
echo Name is:       %Name%
set file=%Folder%%name%.hhp

pushd %Folder%
 
pandoc --from=markdown_github --to=html --output="%Folder%%Name%.html" --css="%toolFolder%bncs.css" --variable=path:"%toolFolder:~0,-1%" --template="%toolFolder%html_template.txt" --self-contained "%1"
 
if not exist "%file%" (
>%file%  echo [OPTIONS]
>>%file% echo Compatibility=1.1 or later
>>%file% echo Compiled file=%Folder%..\%Name%.chm
>>%file% echo Default Window=main
>>%file% echo Default topic=%Name%.html
>>%file% echo Display compile notes=No
>>%file% echo Display compile progress=No
>>%file% echo Full-text search=Yes
>>%file% echo Language=0x809 English (United Kingdom)
>>%file% echo:
>>%file% echo [WINDOWS]
>>%file% echo main="%Name%",,,"%Name%.html","%Name%.html",,,,,0x22420,,0x204e,[0,0,700,800],,,,1,,,0
>>%file% echo:
>>%file% echo:
>>%file% echo [FILES]
>>%file% echo *.html
>>%file% echo:
>>%file% echo [INFOTYPES]

set __hhpcreated=true
)
 
@echo off
 
if exist "C:\Program Files (x86)\HTML Help Workshop\hhc.exe" (
    "C:\Program Files (x86)\HTML Help Workshop\hhc.exe" "%Folder%%Name%.hhp"
) else (
   if exist "C:\Program Files\HTML Help Workshop\hhc.exe" (
    "C:\Program Files\HTML Help Workshop\hhc.exe" "%Folder%%Name%.hhp"
    )
)

if %__hhpcreated% equ true (
del %file%
)

popd
 
if %CC_DEVELOPER% equ true (
	start chrome "%Folder%%Name%.html"
	timeout 60
)
