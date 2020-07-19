[UI Components](../../index.html)
# Router Dest

## Description
This component has a button and optional monitor image for selecting sources to its assigned destination.

## Commands

Name               | Type      | Use
-------------------|-----------|-----------------------------------------------------------------------------
instance           | mandatory | Instance information - this will be resolved to a device number in order to load the names
dest_database      | manadatory| Which BNCS router database to load
dest_instance_database | designtime | &nbsp;
dest_index         | mandatory | The destination to be routed to when this button and a source is selected
default_source     | designtime| Used by the route_default command
Alias              | optional  | Design time alternative that overrides the router destination name put on the button
tally_database     | optional  | Comma delimited list of databases to use for the tally. Multiple entries are concatenated with a dash between them
initially_selected | optional  | Causes a host notification when true
select.&lt;instance&gt;=&lt;dest&gt; | runtime | Set the selected state if we are configured for the specified dest. Instance is optional and is checked if provided
take.&lt;instance&gt;    | runtime   | Make the route for the previously defined source/dest. Instance is optional and is checked if provided
undo               | runtime   | Undo the last route made

## Notifications

Name   | Use
-------------------|--------------
selected.&lt;instance&gt;=&lt;index&gt; | Current selection
locked=&lt;lock_state&gt; | 0 = unlocked <br>  1 = locked <br> derived from lock infodriver status
dest_index=&lt;index&gt; | Notification of index on change of dest instance (or on startup)
instance=&lt;index&gt; | Notification of instance on change of dest instance (or on startup)

## Stylesheets
This component uses the following hopefully fairly self-explanatory stylesheet entries:

|Entry|
|----------------|
| dest_selected  |
| dest_deselected|
| dest_locked    |
| dest_unlocked  |
