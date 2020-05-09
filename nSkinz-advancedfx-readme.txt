You need to replace the ones of latest HLAE with the files in this ZIP.

Then load the game with HLAE.

Then use
mirv_loadlibrary "C:\Path\To\nSkinz.dll"

Then it should say it's loaded in console.
(You can do in game menu, no need to load map yet.)

Then when you press Insert it should show the nSkinz GUI.
The XUID (bellow Enabled check box)
you can get from
mirv_listentities isPlayer=1 sort=distance
it's unique per Steam account
when you are done setting it up, click update and jump _before_ a round start (or demo begin) and hit play.

The default paintkit for gloves / weapons is "workshop_default" (at start of list).

Known problems:
- New skins only work in new demos.

All credit goes to
https://github.com/namazso/nSkinz

Please understand the original author probably won't support this modification and we don't plan to provide support at the moment either.

MIT License

Copyright (c) namazso 2018 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

