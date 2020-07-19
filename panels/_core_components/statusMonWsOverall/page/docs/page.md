# Workstation Status Monitoring - Page Component

## Description
This is a component that aggregates workstation status as returned in the internal CSI workstation status infodrivers.

## Commands
Name     | Type       | Use
---------|------------|--------------------
instance | mandatory  | Standard Instance information
add      | mandatory write-only	| workstation number to monitor and aggregate
text     | write-only | text to display on the button

## Notifications
This component passes on any notifications from the children on its UI.

Typically this is the following table.

Name            | Use
----------------|--------------------
button=released | This is the usual button notification

## Stylesheets
Name            | Use
----------------|--------------------
enum_ok         | OK status (all workstations present)
enum_alarm      | Fail status (one or more workstations not present)
