# rugburn :fire:
_This project is not endorsed by or related to Ntreev Soft Corporation. See [Trademark Notice](#trademark-notice) for more information._

`rugburn` is an unobtrusive and small shim for `ijl15.dll` that allows you to
run unmodified PangYa™ without GameGuard.

This also allows you to run PangYa under Wine :)

To patch your installation (new way), use [the web patcher](https://rugburn.gg).

To install without patching (old way), grab a binary from the [Releases](https://github.com/pangbox/rugburn/releases) page and follow the [installation instructions](#install).

See [Configuration](#configuration) for information on how to use it.

Features:

  * Redirects network traffic to localhost, configurable via a configuration
    file. Supports basic regular expressions for rewriting URLs.

  * Patches GameGuard's check routines to allow it to run indefinitely without
    GameGuard.

  * Prevents PangYa from creating annoying topmost windows.

  * Sets `PANGYA_ARG` when possible to avoid the updater check.

  * Can be used as a relay for `ijl15.dll` or as a very simple patch for it.
    No need to modify or remove protection on `ProjectG.exe`.

  * **This program is not suitable for cheating. It does not support the only
    active region of PangYa and does not offer GameGuard emulation that would
    be needed to stay connected to an official server.** This program is
    designed for personal and educational use.

## Configuration
When running PangYa with Rugburn for the first time, a sample configuration
file is created at `rugburn.json`. It looks like this:

```json
{
  "UrlRewrites": {
    "http://[a-zA-Z0-9:.]+/(.*)": "http://localhost:8080/$0"
  },
  "PortRewrites": [
    {
      "FromPort": 10803,
      "ToPort": 10101,
      "ToAddr": "localhost"
    },
    {
      "FromPort": 10103,
      "ToPort": 10101,
      "ToAddr": "localhost"
    }
  ]
}
```

You can add `PortRewrites` to override and redirect Winsock2 connections,
whereas you can add `UrlRewrites` to rewrite WinHTTP requests.

### Regular Expressions
Regular expression support in Rugburn is somewhat limited. The following
features are supported:

  * Capture groups (special characters `(` and `)`)
  * Character classes (special characters `[` and `]`)
  * Preset character classes (special escapes `\d`, `\D`, `\w` `\W`, `\s`, `\S`)
  * Dot match (special character `.`)
  * Zero or more match (special character `*`)
  * One or more match (special character `+`)
  * Optional match (special character `?`)

The replacement engine supports `$0` through `$9` in the replacement text to
refer to capture groups in the regular expression. Note that `$0` does not
refer to the entire match, since all regular expressions must fully match
anyways. `$$` can be used to escape a `$` in the replacement.

## Compiling
`rugburn` is typically compiled with MinGW32. A Visual Studio solution is
provided for convenience and should work in most recent versions of Visual
Studio.

Prerequisites:
  * MinGW32
  * GNU Make

And all you have to do is run `make`.
```sh
make
```

And you should find an `ijl15.dll` in your `out/` directory.

By default, the Makefile will assume that `i686-w64-mingw32-gcc` is the appropriate compiler. You can override this by passing in the `CC` variable:

```
make CC=gcc
```
## Install

 1. Back up the ijl15.dll file in your PangYa directory.

 2. Copy out/ijl15.dll from your Rugburn directory to your PangYa directory.

## Usage
Once installed, you can run ProjectG directly. Enjoy!

## Troubleshooting
If you have any issues, I can **not** guarantee that I can help you. However, please feel free to create a GitHub issue. Please describe your problem and if applicable, attach a copy of the `gglog.txt` file.

## Contributing
I would be overjoyed if anyone wanted to contribute to this project! However, the project is considered _nearly_ feature complete and therefore new features may not always be accepted. Well-tested, well-written improvements to the patching routines would definitely be welcome.

Please note that I may take a while to get to your pull request. This project is not my fulltime job. Sorry!

## Why the name?
"Rugburn" was chosen arbitrarily, inspired by an anagram of GameGuard. Before release it was known as ijlshim or ggtfo, and you may see occasional references to this in the code or Git history.

## License
Most of the code of rugburn is licensed under the ISC license. Some portions are licensed differently. See [LICENSE.md](./LICENSE.md) for complete licensing information.

The long and short of Rugburn's license is that you are free to do with it what you please, although you should maintain the necessary copyright license disclaimers if you are reproducing a substantial portion of the program.

A copy of the Intel JPEG Library is included in the `third_party/ijl` directory. It is an unfree redistributable. For more information, see its [LICENSE.md](./third_party/ijl/LICENSE.md) file.

When compiled together with the Intel JPEG Library, which is what you get by default, the resulting binary is unfree but redistributable.

> **Note**: This section is not legal advice. The `LICENSE.md` file contains the canonical text covering the rights granted to you and your legal obligations.

## Special Thanks
I would like to give special thanks to:

- The entire [Retreev](https://github.com/retreev) community for motivation and advice.

- Acrisio Filho for their many efforts; Rugburn contributions, many reverse engineering efforts, the SuperSS project, and more.

- pixeldesu for supporting and funding PangYa preservation efforts, and for a variety of PangYa reverse engineering efforts.

- You! I originally created Rugburn for myself only, but I am nonetheless pleased whenever I do see anybody interested in it. I hope that you will find it useful, whether for playing PangYa or for your own preservation endeavors, or perhaps even just as a curiosity.

## Trademark Notice
PangYa is a registered trademark of Ntreev Soft Co., Ltd. Corporation. Pangbox is not endorsed or related to Ntreev Soft Co., Ltd. Corporation in any way. "PangYa" and related trademarks are used strictly for purposes of identification.
