/**
 * Copyright 2021 Larthia, University of Pisa. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <nethuns/nethuns.h>
#include <stdio.h>
#include <unistd.h>




int
main(int argc, char *argv[])
{
    printf("version_full: %s\n", nethuns_version_full());
    printf("version: %s\n", nethuns_version());
}
