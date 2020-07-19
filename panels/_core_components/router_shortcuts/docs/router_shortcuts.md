[UI Components](../../index.html)
# Router Shortcuts
A layout configurable grid of source buttons  
for use as preset shortcuts for example "silence" or "black" or "test"  

The component is self learning on the layout chosen  
Type the layout name (the .bncs_ui extension will be removed if used) and the component will detect the number of source buttons on the layout  
You can then enter an alias name and source number for each button

## Commands
|Name|Type|Use|
|------|------|------|
|instance | Mandatory |Sets the instance of the router
|source_index_ | Mandatory <br> Multiple generated via choice of layout | Sets the index of each source button <br> Command ends in the button number so there are as many of these as there are buttons
|source_alias_ | Mandatory <br> Multiple generated via choice of layout | Sets the label of each source button <br> Command ends in the button number so there are as many of these as there are buttons <br>  if no alias is entered then the source name from the #define DATABASE_SOURCE is used
|source | Mandatory <br> Nominally at runtime | Sets the index of the currently selected source <br> If that source is one of the shortcut buttons then that button is highlighted
|layout | Mandatory | Sets the .bncs_ui layout of the shortcuts <br> Extension is automatically removed if added <br> Layouts should be in a subdirectory as #defined in LAYOUTS_SUB_DIR
|background_colour | Optional | Sets the background colour of the .bncs_ui layout <br> Will default to the #define of DEFAULT_BACKGROUND_COLOUR
|deselect | Unused | Not Used

## Editor _Commands

|Name|Use|
|------|------|
|source=[value] | Currently selected source|

## Editor _Events 

|Name|Use|
|------|------|
|index=<index> |Index of selected source|


## Developer Notes
New layouts can be created by subsequently adding buttons named source_n  
where n is an incrementing number starting at 1  
If you miss an increment or don't start at 1 then the buttons will not work properly  

If you want the background colour to be configurable ensure the control is called "grpBackground"
 