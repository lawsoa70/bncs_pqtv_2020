[UI Components](../../../index.html)

# Audio_Lock Component

## Description
This is a simple control to set and show the audio lock state.

The control takes base instance, the base lock instance and global index and displays the current audio_lock state (using the stylesheet entries below). On pressing the button on this control the audio_lock state is toggled.

The control understands audio levels and will display locked if any of the audio slots is locked on sending unlock will set all audio slots to unlocked

## Commands

Name          | Use
--------------|-------------------------------------
instance	  | Base Audio Instance
lock_instance | Base Audio Lock Instance
index         | The global index of on the audio_lock

## Notifications

None

## Stylesheets

Name          | Use
--------------|-------------------------------------
enum_unknown  | When a control is targeted but the value has not been returned from the device
dest_locked	  | When the destination is locked (1)
dest_unlocked | When the destination is not locked (0)
 
