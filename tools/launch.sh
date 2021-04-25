# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

export PROJECT_ROOT=$PWD

function remove_from_var
{
    local VAR=$1
    local VAL="$2"
    if test -n "$VAL"; then
        export $VAR="$(echo "${!VAR}" | tr ':' '\n' | grep -v "$VAL" | paste -d: -s)"
    fi
}

function prepend_var
{
    local VAR=$1
    local VAL="$2"
    if test -n "$VAL"; then
        remove_from_var $VAR "$VAL"
        if test -n "${!VAR}"; then
            export $VAR="$VAL:${!VAR}"
        else
            export $VAR="$VAL"
        fi
    fi
}

case `uname` in
Linux)
    prepend_var PATH $PROJECT_ROOT/prebuilts/linux/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/linux/cmake/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/linux/gcc-arm-none-eabi/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/linux/gcc-mips-rda-elf/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/linux/gcc-rv32-elf/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/linux/nanopb
    prepend_var PATH $PROJECT_ROOT/tools
    prepend_var PATH $PROJECT_ROOT/tools/linux
    ;;
*_NT*)
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/cmake/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/python3
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/gcc-arm-none-eabi/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/gcc-mips-rda-elf/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/gcc-rv32-elf/bin
    prepend_var PATH $PROJECT_ROOT/prebuilts/win32/nanopb
    prepend_var PATH $PROJECT_ROOT/tools
    prepend_var PATH $PROJECT_ROOT/tools/win32
    ;;
esac

function project_select_from
{
    local BAR="$1"
    local ITEMS="$2"
    local DEF=$3
    local VAR=$4

    echo
    echo "$BAR"

    local MENUS
    local i=0
    local item
    for item in $ITEMS; do
        echo "   $i. $item"
        i=$(($i+1))
        MENUS=(${MENUS[@]} $item)
    done

    echo -n "which would you like? [$DEF] "
    local answer
    read answer

    local choice=$DEF
    if test -n "$answer"; then
        if (echo -n $answer | grep -q -e "^[0-9][0-9]*$"); then
            if [ $answer -lt ${#MENUS[@]} ]; then
                choice=${MENUS[$(($answer))]}
            fi
        fi
    fi

    export $VAR=$choice
}

function project_select
{
    local target
    local TARGETS=
    for target in $(cd target; /bin/ls); do
        if test -f target/$target/target.config; then
            TARGETS="$TARGETS $target"
        fi
    done

    project_select_from "build target:" "$TARGETS" "" BUILD_TARGET
}

function release_type_select
{
    local release_type
    local RELEASE_TYPES="debug release"

    project_select_from "build release type:" "$RELEASE_TYPES" "debug" BUILD_RELEASE_TYPE
}

function project_show
{
    echo
    echo "=========================="
    echo "PATH=$PATH"
    echo "PROJECT_ROOT=$PROJECT_ROOT"
    echo "PROJECT_OUT=$PROJECT_OUT"
    echo "BUILD_TARGET=$BUILD_TARGET"
    echo "BUILD_RELEASE_TYPE"="$BUILD_RELEASE_TYPE"
    echo "KCONFIG_CONFIG=$KCONFIG_CONFIG"
}

if [ -z "$1" ]; then
    project_select
    release_type_select
else
    export BUILD_TARGET=$1
    if [ -z "$2" ]; then
        export BUILD_RELEASE_TYPE=debug
    else
        export BUILD_RELEASE_TYPE=$2
    fi
fi

export KCONFIG_CONFIG=$PROJECT_ROOT/target/$BUILD_TARGET/target.config
export PROJECT_OUT=$PROJECT_ROOT/out/${BUILD_TARGET}_${BUILD_RELEASE_TYPE}

project_show
unset -f project_select
unset -f project_select_from
unset -f release_type_select

cout() {
    mkdir -p $PROJECT_OUT && cd $PROJECT_OUT
}

croot() {
    cd $PROJECT_ROOT
}
