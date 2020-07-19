[UI Components](../index.html)
# Example Component Documentation

This is a one liner to tell you concisely what it does, followed by an image to make it bleedin' obvious:
![lock](example.png)
If the image contains many items (to show the context you might use the component, then it should be highlighted as shown.

## Commands
What things you can send to this control, whether they're runtime or design-time, whether mandatory or optional

|Command|Mandatory|Notes|
|--|--|--|
|instance|Y|what to target this control at|


## Notifications
What this control can tell you

|Notification|Notes|
|--|--|
|locked|0=unlocked<br>1=locked|

## Stylesheets
What entries in the system-wide stylesheet this control needs. 

|Name|Runtime|Use|
|-|-|-|
|enum_unknown|Y|When a control is targeted but the value has not been returned from the device|
|dest_locked|Y|When the destination is locked (1)|
|dest_unlocked|Y| When the destination is not locked (0)|

## Customising the UI
what options are available to configure/modify/customise the UI (button naming conventions)

## Database
How the component uses BNCS router databases

## Object/workstation settings or other configuration
What other settings may be required to get this component configured

## Version History
This is the master copy of the version history - KEEP IT UP TO DATE.

Perhaps also note where the component originated from.

Version history should be of the format:
```
<major change>.<feature change>.<bugfix>.<branch>
```

|Version|Notes|
|--|--|
|1.0.1.0|First release|
|1.0.2.0|Important Bugfix|
