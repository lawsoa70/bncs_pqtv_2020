[UI Components](../../index.html)
# Monitor Follow Component

This is a simple control to optionally follow the selected source to a monitoring destination.

## Configuration ##
This device requires the following configuration:

## workstation_settings.xml ##
We need to know which what operational position this workstation is and in which area it is used so that we can tie this up with the monitoring destination.  
This is demonstrated in the following example.

``` xml
<?xml version="1.0" encoding="utf-8"?>
<workstation_settings>
      <workstation id="10">

            <setting id="ops_area" value="gallery_a"/>

            <setting id="ops_position" value="gallery_a_tm"/>

      </workstation>

</workstation_settings>
```

## ops_positions.xml ##

Once we've determined where this operational position is then we can work out what destinations it uses.

``` xml
<?xml version="1.0"?>
<ops_positions>
      <position id="gallery_a_tm">
      <!-- This is the ops_position value from workstation_settings -->

            <setting id="name" value="Gallery A TM" />

            <setting id="mon_dest_packager" instance="packager_gallery_router" dest_index="614" />
            <!-- id="mon_dest_packager" is used by the component to find the instance and dest_index. -->
            <!-- It checks for all entries starting with "mon_dest" and matches its instance with the instance in the setting line -->
            <!-- instance="packager_gallery_router" identifies the packager infodriver (or a router GRD) -->
            <!-- dest_index="614" is the destination package of the monitor that will be routed to -->

      </position>
</ops_positions>
```

## Modifications and updates to monitorFollow component ##
These modifications allows packager (infodriver) and router (GRD) instances to be used to target the component and will ensure that the monitoring destination is not locked before making monitoring routes

The component is now much safer because you give it the instance name it is acting on and it uses *that* to discover the appropriate config and to get the monitoring destination in ops_positions.xml.

It can't act on the wrong routers like it could before (so long as your instance is set right!!!)

The component auto-detects the type by using the **"type=nnn"** entry in the instances line  
Routers have **"type=Router"** and packagers have **"type=PackagerAuto"**  
**Routers** will use **RC** crosspoints whilst **packagers** will use **IW** crosspoints

### Composite Instance ###
If the monFollow is targeted against a composite instance it will pick the correct element from the composite group.  
It will choose group **id="router"** for the router element and group **id="lock"** for the lock infodriver

### Normal Instance ###
If it is targetted at a normal instance it will construct the instance names as it did previously - that is if the instance begins with **"packager_"** and ends with **"_router"** it creates the lock instance name by replacing **"_router"** with **"_lock"**  
...or, failing that it adds **"_lock"** to the instance name to produce the lock infodriver name

### Destination ###
Once the component has deciphered the instance names it rattles through the relivant section of ops_positions.xml in order to find the monitoring destination


it looks for all setting ids in the section beginning with **"mon_dest"** and checks if the **"instance="** name matches its given instance in the component targeting.  
If it does, it uses *that* entry's **"dest_index="** setting for the destination number.

For example, targeting the component to **"packager_gallery_router"** would find the **"package_gallery_router"** destination and targeting to **"packager_core_router"** would find the **"package_core_router"** destination

### Connections ###
In the panel, the only two connections needed now are **"source=nnn"** and **"dest=mmm"** from the source and dest grids respectively.  
The code keeps track of the last source or dest selected and if the last request was a source or a dest

Two other commands can be used, **label** and **status**

Label can be **"enable"** or **"disable"** and will show or hide the tally label at the bottom of the mon follow button.

*Status* is a feature added for debug purposes, it too can be **"enable"** or **"disable"**.  
If it is *enabled*, a push-and-hold action on the mon follow button makes a popup diagnostics window appear.  
This lists all the current states ie instance names, dests, logic, selections etc


bncs_style.xml requires an added state of **"mon_notvalid"** which greys the button (similar to it looking disabled) so that it can remain active to allow the push-and-hold status operation.  
If the state is notvalid then the monfollow function cannot be activated so it fails safe.

## Targetting ##
The component can now take a composite instance as well as a regular instance in order to automatically calculate its settings and checks for the **composite="yes"** command in the instance entry

Consider the following composite instance/instances example...

```xml
    <instance composite="yes" id="c_rtr_returns">
        <group id="router" instance="rtr_returns" />
        <group id="lock"   instance="rtr_returns_lock" />
    </instance>
    <instance composite="no" id="rtr_returns" type="Router" location="TBA" ref="device=48,offset=0" alt_id="Returns Router" />
    <instance composite="no" id="rtr_returns_lock" type="Router" location="TBA" ref="device=49,offset=0" alt_id="Returns Router Lock" />
```
Giving the component an instance of **"c_rtr_returns"** detects the **composite="yes"** entry

This in turn causes the component to seek out the group instance of **"router"** for the instance it uses as a router and the group instance of **"lock"** for the lock infodriver

In the *real* router entry **id="rtr_returns"**, it then reads the **type="Router"** - from this it can tell that the monitoring uses a real GRD and so should use RC commands for monitoring routes

With this example..

```xml
    <instance composite="yes" id="c_packager_core_router">
        <group id="router" instance="packager_core_router" />
        <group id="mm_stack" instance="packager_core_mm_stack" />
        <group id="pti" instance="packager_core_pti" />
        <group id="dest_comms" instance="packager_core_dest_comms" />
        <group id="lock" instance="packager_core_lock" />
        <group id="ifbpti" instance="packager_core_ifbpti" />
        <group id="eventsend" instance="packager_core_eventsend" />
        <group id="miscdata" instance="packager_core_miscdata" />
    </instance>    
    
    <instance ref="device=401,offset=0" id="packager_core_router" type="PackagerAuto" alt_id="CORE Package Manager" composite="no" location="CTA" />
    <instance ref="device=402,offset=0" id="packager_core_mm_stack" type="PackagerAuto" alt_id="Packager MM Stack" composite="no" location="CTA" />
    <instance ref="device=403,offset=0" id="packager_core_pti" type="PackagerAuto" alt_id="Packager Source PTI" composite="no" location="CTA" />
    <instance ref="device=404,offset=0" id="packager_core_dest_comms" type="PackagerAuto" alt_id="Packager Dest Comms" composite="no" location="CTA" />
    <instance ref="device=405,offset=0" id="packager_core_lock" type="PackagerAuto" alt_id="Packager Lock" composite="no" location="CTA" />
    <instance ref="device=406,offset=0" id="packager_core_ifbpti" type="PackagerAuto" alt_id="Packager IFB pti" composite="no" location="CTA" />
    <instance ref="device=407,offset=0" id="packager_core_eventsend" type="PackagerAuto" alt_id="Packager Event Send" composite="no" location="CTA" />
    <instance ref="device=408,offset=0" id="packager_core_miscdata" type="PackagerAuto" alt_id="Packager Misc Data" composite="no" location="CTA" />
```
the group ids of **"router"** and **"lock"** point to real instances **"packaging_core_router"** and **"packaging_core_lock"**

This time the router type is **"PackagerAuto"** which ensures the component uses IW commands to the packager for monitoring routes

For legacy uses, if a non-composite instance is used then the previous technique of checking for the instance beginning with **"packager_"** and ending in **"_router"** is used.  
The lock infodriver name in this case is derived from the router instance name.

## Status ##
As a help for debugging and setup, the "Mon Follow" button is now equipped with a push-and-hold feature.

This causes a popup window to appear that lists all the parameters and values used in the component

Parameter|Information
---------|-----------
Composite instance|YES if the component instance is composite
Script instance|The instance name issued to the script
Derived instance|The instance name derived from the script instance name (router/packager instance)
Instance type|Router or PackagerAuto
Valid|YES if the config generates a valid monitor destination
Instance device|ROUTER for a router, PACKAGER for a package automatic
Instance dev id|Device number for the router/packager
Instance mon dest|Destination number for the monitoring destination
Derived lock instance|The lock instance name derived from the script instance name (router/packager instance)
Lock instance dev id|Device number for the lock infodriver
Locked|YES if the monitoring destination is locked
Following|YES if the monitor follow has been activated for the component
Monitoring|DEST if the component is monitoring a destination, SCE if the component is monitoring a source
Selected source|The selcted source index
Selected dest|The selected destination index
Monitored source|The source index routed to the selected destination

# Commands #
Name|Use
----|---
instance|Instance of the device to monitor follow
source|The source index - if we're monitor following then this is routed to our monitoring destination. If we're not monitor following then the value is stored in case the user hits the button to monitor follow. Also used to light yellow ‘led’ top left.
dest|Tells the component to monitor follow the source routed to the selected destination. Also used to light the red ‘led’ top right
label|enabled - the tally label below the mon follow button is shown. disabled - the tally label below the mon follow button is hidden
status
enabled - the status popup can be instigated with a push-and-hold action of the mon follow button
disabled - the status popup cannot be instigated with a push-and-hold action of the mon follow button

# Notifications #

None

# Stylesheets #
Name|Use
----|---
mon_notfollowing|Shown if the monitor follow function is inactive
mon_following|Shown if the monitor follow function is active
mon_notvalid|Shown if the monitor follow configuration is not valid or incomplete
