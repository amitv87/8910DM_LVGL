#!/bin/bash
# Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

set -e

build_target() {
    source tools/launch.sh $1
    cout
    cmake ../.. -G Ninja
    ninja
    croot
}

for TARGET in $(
    cd target
    /bin/ls
); do
    if test -f target/$TARGET/target.config; then
        case $TARGET in
        8915DM_nbiot | EM610V2)
            echo "$TARGET in known not work"
            ;;
        *)
            build_target $TARGET
            ;;
        esac
    fi
done
