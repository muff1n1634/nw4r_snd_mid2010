# NW4R SND library decompilation (c. mid-2010)

This repository contains a (mostly) matching decompilation of the SND library in the NintendoWare for Revolution SDK, timestamped mid-2010[^1].
[^1]: Instead of being given pre-compiled library archives, developers were provided with the source code and compiled it themselves. Therefore, one specific date cannot alone determine a specific release of the library.

Symbol names come from symbol maps in [`[R89JEL]`](https://wiki.dolphin-emu.org/index.php?title=R89JEL) *Tokyo Friend Park II Ketteiban: Minna de Chousen! Taikan Attraction*. Struct definitions and other names come from DWARF info from the debug ELF binary from *Tokyo Friend Park II*, as well as other sources listed where referenced.

Your decomp may have some of the extra functions that are not in this repository; you will have to do those yourself, but hopefully the surrounding source can help you with that.

## Building

### Prerequisites
- [`mkdir`](https://en.wikipedia.org/wiki/mkdir) that supports the `-p` flag
- [Make](https://en.wikipedia.org/wiki/Make_(software))
- [Metrowerks CodeWarrior](https://en.wikipedia.org/wiki/CodeWarrior) for MPC55xx/MPC56xx v2.10 ([Windows installer, direct link](https://www.nxp.com/lgfiles/devsuites/PowerPC/CW55xx_v2_10_SE.exe))
- [Wine](https://wiki.winehq.org/Download) or equivalent (if not compiling natively under Windows)

### Instructions

In the makefile, set the values of the following variables:
- `MKDIR`: `mkdir` that supports the `-p` flag
- `WINE`: path of `wine`, if applicable
- `MWERKS`: path of `mwcceppc.exe`

Then configure
- the `NW4R_ASSERT` macro (see [`include/nw4r/NW4RAssert.hpp`](include/nw4r/NW4RAssert.hpp))
- the `BUILDSTAMP` macros in [`source/snd_SoundSystem.cpp`](source/snd_SoundSystem.cpp)

Then run
- `make libnw4r_snd` to create `lib/libnw4r_snd.a`,
- `make libnw4r_sndD` to create `lib/libnw4r_sndD.a`, or
- `make build/`(`release`/`debug`)`/<file>.o` to compile a specific file, if you're playing around with the source. For example, to compile the release version of `source/snd_SoundSystem.cpp`, run `make build/release/snd_SoundSystem.o`.
- `make clean` comes included.

## Porting to a decompilation

No guarantees about similarity can be made with any other versions, since developers were also able to change code to fit their specific games, but this source could be a good starting point if it's only a little different.

`source/` should be mostly drag-and-drop into wherever you keep your NW4R source, though you may have problems with include directives not finding local headers if you use `-cwd explicit` or `-cwd project`.

The only headers in `include/` you need to take are the NW4R SND headers in `nw4r/snd/`; perhaps `context_rvl.h` as well as some of `macros.h` and `types.h`, if you don't have some of those declarations. The rest (primarily `stdlib/`) is context; you should use your decomp's own versions of those headers.

Integrating this library into your decomp's build system is going to be specific to your decomp, and so is not covered here.

> [!NOTE]
> The `import` branch *does* build correctly, but everything is a mess, especially the SND headers, and not everything is checked and referenced against `[R89JEL]` DWARF.
> Commits to `main` will be cleaned up and verified to match DWARF as closely as possible.

## Contribution

By its nature, matching decompilation *has* a finish line, and this source is almost there. The few remaining unmatched functions in this repository are

- [x] ~~`snd_SeqFileReader.cpp`: `SeqFileReader::ReadOffsetByLabel`~~
- [x] ~~`snd_SeqTrack.cpp`: `SeqTrack::SetSilence`~~
- [ ] `snd_StrmPlayer.cpp`: `StrmHeader::LoadHeader`
- [ ] `snd_StrmPlayer.cpp`: `StrmHeader::LoadStreamData`
- [ ] `snd_Voice.cpp`: `Voice::Acquire`

as well as some fakematches scattered around.