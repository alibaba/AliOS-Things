gpu_demo application
====================

GPU demonstration displayed on a Hannstar LVDS panel on the LVDS0.


Description
-----------

The gpu_demo application demonstrates GPU output via a pre-recorded stream of low-level GPU operations and 
runs on the i.MX6DQ and i.MX6SDL devices. The gpu_demo does not run on the i.MX6SL because it does not have a GPU.

After the application starts running, it sets up the IPU peripheral to work with the HannStar XGA LVDS display
device. Then it loads a sphere with a mirrored texture against a background that resembles the interior of an
intricate building. The program then begins to rotate the camera, panning around the building's interior
with the building's details reflected in the mirrored texture of the floating sphere. To further illustrate
the GPU performance, the mirrored sphere ripples occasionally distorting the reflections of the building.

The demo runs continuously until the user stops it with a key press at the terminal.


Requirements
------------

A Hannstar LVDS panel is required on the LVDS0 port.


Build options
-------------

None.


Code organization
-----------------

All code for gpu_demo resides in gpu_demo.c, texture.c and roatate_texture.c in the src directory. It makes use
of the ipu drivers from the SDK and some apps/common code for initialization. 
To speed up the build, the very large texture files are precompiled to .o files and included in the src directory,
such that the .c files are not compiled during the build.
