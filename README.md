AmbientBeatsKitchenCabinets - Reactive accent lighting
====================

## Environment

You may use any IDE of choice, but Particle provides an easy to use, Atom based IDE that includes everything you need to get going quickly. 
[You can find the setup instructions here.](https://docs.particle.io/tutorials/developer-tools/dev/). 

If you'd like to use your own IDE would rather use the command line,
 you may [install the cli](https://docs.particle.io/tutorials/developer-tools/cli/) as it provides all the functionality you need on the command line.

You'll also need to set up a Particle account, and [register your first device](https://docs.particle.io/quickstart/photon/#connect-your-photon)
if you haven't already.

#### Device Firmware
AmbientBeats requires the device be flashed with [v1.2.1](https://github.com/particle-iot/device-os/releases/tag/v1.2.1) or greater.
The flashing process is quite simple, but requires that you have the [cli installed.](https://docs.particle.io/tutorials/developer-tools/cli/).
The instructions on the GitHub firmware page should suffice, but more information about manually flashing can be found [here.](https://docs.particle.io/support/troubleshooting/firmware-upgrades/photon/)


## Submodules setup

Currently the AmbientBeats projects all make use of Git submodules for their dependencies. 
After cloning the repository for the first time, you'll have to init the submodules. 

`git submodule update --init --recursive`

When any of the submodules have been modified in the repo, you'll have to pull the changes separately from the usual `git pull`.

`git submodule update --recursive --remote`


