## What is MAME 2003-Plus?

MAME 2003-Plus (also referred to as MAME 2003+ and mame2003-plus) is a libretro arcade system emulator core with an emphasis on high performance and broad compatibility with mobile devices, single board computers, embedded systems, and similar platforms.

In order to take advantage of the performance and lower hardware requirements of an earlier MAME architecture, MAME 2003-Plus began with the MAME 2003 codebase which is itself derived from xmame 0.78. Upon that base, MAME 2003-Plus contributors have backported support for an additional 350 games, as well as other functionality not originally present in the underlying codebase.

**Authors:** MAMEdev, MAME 2003-Plus team, et al (see [LICENSE.md](https://raw.githubusercontent.com/libretro/mame2003-plus-libretro/master/LICENSE.md) and [CHANGELOG.md](https://raw.githubusercontent.com/libretro/mame2003-plus-libretro/master/CHANGELOG.md))

# Documentation
User documentation for MAME 2003-Plus can be found in the **[libretro core documentation library](https://docs.libretro.com/)**.

Developer documentation can be found in **[the MAME 2003-Plus wiki](https://github.com/libretro/mame2003-plus-libretro/wiki)**.

## Development chat
#programming channel of the [libretro discord chat server](https://discordapp.com/invite/C4amCeV).

## What Arcade romsets work with MAME 2003-Plus and how to build them

**mame2003-plus was originally built from the MAME 0.78 codebase, meaning that 95% or more of MAME 0.78 romsets will work as-is in mame2003-plus, where they immediately benefit from its bugfixes and other improvements.** In order to play the new games and games which received ROM updates in mame2003-plus, you will need to find or build the correct romsets.

**[Read more about rebuilding romsets in the libretro core documentation for mame2003-plus](https://docs.libretro.com/library/mame2003_plus/#Building-romsets-for-MAME-2003-Plus)**.

## Why is this port here

** So I can actually use it on my barcade and have some auto mapping to gamepads and panels.

** Im also trying to bring back fully user flexibility of the way mame was originally set up as well and restore the functionaltiy libretro removed and never replaced with anything functional.

** examples are the on screen display  to set volues brightness and gamma also when loading a non working rom it brings up an informations screen that was removed both have been put back in this core.

** retropad remapping from the libretro frontend has been remove as well its not felxible enough to cater for mame is very basic cant handle dynamic mappping across controllers.

** On a personal note. I never wanted these libretro changes maybe mark will finish his vision one day. I cant see libretro being able to hande what the mame ui does at all. 

** Plus is very difficult for new users but i had to do my best to keep the old usage active thats why the dual input system was added thats also been removed from this port it back to basics use mame menu for your inputa the way it should be.
