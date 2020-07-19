[UI Components](../../index.html)
# Router List

## Description
This is a simple component that fills a listbox with database names for the given instance name and database number

## Commands

|Name|Type|Use|
|------|------|------|
|instance | mandatory | Instance information - this will be resolved to a device number in order to load the names
|database | manadatory | Which BNCS router database to load
|deselect | runtime | Will cause it to deselect and un highlight any row that is currently selected
|remove_begins=nn | runtime <br> designtime | Will cause it to remove all entries that start with nn. (Default for designtime is -1)
|remove_exact=yy | runtime | Will cause it to remove all entries that match exactly with yy. (Not case sensitive)
|select=[text] | runtime | Will cause it to highlight and select the row that matches the given text
|size=1 | runtime | Relative size of the font used in the listbox. <br> I'd expect this to be set using a stylesheet rather than fiddling with the font-size directly.. <br> Note size=0 doesn't work - use a positive or negative integer
|sort=0 <br> sort=1 | runtime | Whether the contents of the listbox should be alpha-sorted

## Notifications

|Name|Use|
|------|------|
|index=<router index> | Listbox selection has changed to this router index

## Stylesheets
This component should use them but doesn't. They have been omitted to minimise the number of external dependencies on this example.
 