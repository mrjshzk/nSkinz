## Users coming from the original [namazso / nSkinz](https://github.com/namazso/nSkinz):

This fork is originally meant for demos / movie makers and can replace multiple players by XUID, so you will need to enter your XUID in the XUID field!

### Getting your XUID:
You can get your own XUID after starting the game from the game's console, there should be a message like this
```
SDR RelayNetworkStatus:  avail=Attempting  config=OK  anyrelay=Attempting   (Performing ping measurement)
AuthStatus (steamid:12345678901234567):  OK  (OK)
```
12345678901234567 would be the XUID.

----

![Screenshot](http://i.imgur.com/KfnFQrX.jpg)

### nSkinz

nSkinz is a fully featured internal skin, knife, glove and sticker changer for Counter-Strike: Global Offensive.
Although written from scratch, nSkinz was heavily influenced by [chameleon-ng](https://github.com/emskye96/chameleon-ng).

* Use any skin on any weapon, including ones not available normally
* Add any sticker to your weapons
* Use any glove model and skin combination
* Special knife animations are supported
* Knife kill feed icons are automatically replaced
* In-game configuration with [ImGui](https://github.com/ocornut/imgui)
* Always up-to-date dynamic skins, with localized names
* Uses signature scanning and NetVar manager for dynamic offsets
* [JSON](https://github.com/nlohmann/json) based configuration saving & loading support

## Requirements for compiling

* Tested under: [Microsoft Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/community/)
* Latest [Git](https://git-scm.com/downloads)

## Usage

Currently only Windows is supported, however this may change ~~in the future~~ if you submit a PR because I'm lazy.

* Clone the source with:
```git clone --recurse-submodules https://github.com/advancedfx/nSkinz.git```
* Compile into a x86 library using your preferred compiler
* Inject the resulting library into the game
* Press <kbd>Insert</kbd> to bring up the menu.
* Setup weapon configuration(s)
* To override multiple weapons press the "Add" button. When an inventory update occurs always the first enabled entry applicable for that weapon will be used.
* To save your configuration press the "Save" button. To load it later press the "Load" button.

## License

**This project is licensed under the MIT license**

See the "[LICENSE](https://github.com/advancedfx/nSkinz/blob/advancedfx/LICENSE)" file for more details.
