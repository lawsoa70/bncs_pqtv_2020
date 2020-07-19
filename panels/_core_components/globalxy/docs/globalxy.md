[UI Components](../../index.html)
# Global XY
A list box style X/Y panel  
* Sources and destinations can be sorted alpha or index  
* Sources and destinations can be addressed by entry of their index number  
* Route is actioned by a double press of the take button  
* There is also an undo facility  

## Modifications
This panel has been slimmed down from its original incarnation. 
It was originally designed to handle up to 3 routers and switch between them 'in panel'. This feature has been deprecated by removing the switching buttons but the config arrangement and code remains.  
The rename source/dest buttons have been dragged to one side as this feature is also not relevant. Subsequent popup panels triggered by this (keyboards/rename) are therefore also not used.

## Commands
|Name|Type|Use|
|------|------|------|
|instance | Mandatory | Sets the instance of the router as the index within "router_list" to maintain compatability set to the instance of the router
|router_list | Mandatory | List of router instances to be used With this component, only the first instance is valid to maintain compatability set to the instance of the router
|return | Mandatory | Persistant settings return


## How to use
To use this component create a blank bncs_ui file, slap a "script" control on it and point it at this globalxy component. Then set the router_list to the instance of the router to use
