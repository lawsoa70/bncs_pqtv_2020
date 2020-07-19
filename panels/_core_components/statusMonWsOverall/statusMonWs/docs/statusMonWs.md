# Status Mon Workstation Component

## Description
This is a simple component to show the state of a single workstation status slot as derived from CSI's internal workstation status infodriver.

It only shows presence of "some" status data, it doesn't otherwise display any detail (but could easily be made to)

## Commands

Command     | Use
------------|----------------------
workstation | Workstation whose information we want to display
instance    | Instance of the workstation status infodriver

## Notifications
None

## Stylesheets

Entry               | Use
--------------------|----------------------
enum_ok             | Awake, alive
enum_alarm          | Broken
enum_warning        | Haven't heard of for a while...
background_discrete | default background style for the "last heard from" display
