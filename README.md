# HideUI

SKSE plugin that hides the whole interface with one key, F8 by default. Menus, loading screens and HUDs added by other mods are hidden with the rest until the key is pressed again.

Needs Skyrim SE, AE or VR, [SKSE64](https://skse.silverlock.org/) and [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444). On VR, the [VR Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/58101) is required.

## Settings

`Data/SKSE/Plugins/HideUI.ini`

```ini
[General]
Hotkey=66
```

A DirectX scan code. `66` is F8.

With [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352) installed, the key can also be changed in game: click it, then press the new one.

## Building

Needs Visual Studio 2022, CMake, Ninja and vcpkg (`VCPKG_ROOT`).

```
git clone --recurse-submodules <repo>
cmake --preset release
cmake --build build/release
```

Set `SKYRIM_MODS_FOLDER` (Mod Organizer 2 or Vortex) or `SKYRIM_FOLDER` to copy the plugin there after each build.

## License

GPL-3.0 with the modding exceptions in [EXCEPTIONS.md](EXCEPTIONS.md). See [NOTICE](NOTICE) for third-party code.
