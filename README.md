[![CI/CD cmake-multi-platform][cicd_cmake_mp_bdg]][cicd_cmake_mp_url]

# wndx::attila
ATTILA -- Advanced Task Text Interactive Analyzer

https://user-images.githubusercontent.com/15724752/208170719-cc99b9ba-e5d1-47f4-959a-ef9a60caea55.mp4

Demo briefly showcases an early version of the interaction and minimalistic UI.\
Each element of which is accessible using hotkeys, not only via the mouse.

### Pomodoro - the whole gist of this project
- read about [pomodoro technique](https://en.wikipedia.org/wiki/Pomodoro_Technique)
- [wndx::pomodoro](https://github.com/WANDEX/pomodoro) creates tasks log files in plain text
- gather all files from **POMODORO_DIR** which must be set as the ENV variable!\
e.g. `export POMODORO_DIR="$HOME/Documents/Pomodoro"`
- perform non blocking processing of the selected dates span

### Implemented Features
- date range selection: from -> to
- find/filter by regex
- calculate time spent
- merge the same tasks
- brief statistics on the sample

## Requirements
- git submodule: [wndx::sane](https://github.com/WANDEX/wndx_sane)
- lib Qt >= 6.3: comes with amazing IDE [qtcreator](https://github.com/qt-creator/qt-creator)
- tests require: [GoogleTest (gtest)](https://github.com/google/googletest) (CMake fetch content at configure time)

## Build
```sh
$ git clone --recurse-submodules git@github.com:WANDEX/attila.git && cd attila
$ cmake -E make_directory build
$ cmake -S . -B build
$ cmake --build build
```

<details><summary>
How to install specific Qt version using aqtinstall
</summary>

#### Instruction for systems origin from UNIX Win OS w/ **MINGW64:git-bash** shell
Install pip with python from the official website or using platform default package manager\
https://www.python.org/downloads/

##### Install [aqtinstall](https://github.com/miurahr/aqtinstall)
```sh
$ pip install -U pip
$ pip install aqtinstall
```

##### See available architectures & aqt ref
local [./.aqt/settings.ini](./.aqt/settings.ini) &
local [./.aqt/set_aqt_env.sh](./.aqt/set_aqt_env.sh) &
online [settings.ini](https://aqtinstall.readthedocs.io/en/latest/configuration.html)

###### May be needed to add under group blocks: aqt, requests
`INSECURE_NOT_FOR_PRODUCTION_ignore_hash: True` [-> look into local settings.ini](./.aqt/settings.ini)

```sh
$ aqt -c ./.aqt/settings.ini list-qt windows desktop --arch 6.5.2
```
`>>> win64_mingw win64_msvc2019_64 win64_msvc2019_arm64 wasm_singlethread wasm_multithread`
##### cd into dir which will be new installation root of newly installed Qt and required modules
```sh
$ aqt -c ./.aqt/settings.ini install-qt windows desktop 6.5.2 win64_msvc2019_64 -m qtcharts
```
</details>

## Style
For the unified visual style between Qt & GTK applications you can use the default GTK theme, dark variant\
which was used in the demo video, or something similar. `export QT_STYLE_OVERRIDE=Adwaita-Dark`
[[style]](https://wiki.archlinux.org/title/Dark_mode_switching)

## License
[LGPL-3.0-or-later](https://choosealicense.com/licenses/lgpl-3.0/)

[cicd_cmake_mp_url]: https://github.com/WANDEX/attila/actions/workflows/cicd_cmake_multi_platform.yml
[cicd_cmake_mp_bdg]: https://github.com/WANDEX/attila/actions/workflows/cicd_cmake_multi_platform.yml/badge.svg?event=push
