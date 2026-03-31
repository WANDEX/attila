#!/bin/sh
## include/source guard
if [ -z "$QT_PATH" ]; then
	## backup unmodified PATH
	export PATH0="$PATH"
    export QT_PATH="/c/wndx/downloads/aqtinstaller/6.5.2/msvc2019_64"
    export PATH="$PATH:$QT_PATH/bin"
    export QT_PLUGIN_PATH="$QT_PATH/plugins"
    export QML_IMPORT_PATH="$QT_PATH/qml"
    export QML2_IMPORT_PATH="$QT_PATH/qml"
fi