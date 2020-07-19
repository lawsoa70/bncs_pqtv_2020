set toolFolder=%~dp0
 
For /F %%A in ("%1") do (
    Set Folder=%%~dA%%~pA
)
For %%A in ("%1") do (
    Set Name=%%~nA
)
 
echo Folder is: %Folder%
echo Name is: %Name%
 
pandoc --from markdown_github --to html -o "%Folder%\%Name%.html" --css "%toolFolder%bncs.css" -V path="%toolFolder:~0,-1%" --template "%toolFolder%html_template.txt" --self-contained "%1"
 
if %CC_DEVELOPER% equ true (
	start chrome "%Folder%\%Name%.html"
	timeout 60
)
