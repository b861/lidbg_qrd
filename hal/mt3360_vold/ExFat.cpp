/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/mount.h>

#include <linux/kdev_t.h>
#include <linux/fs.h>

#define LOG_TAG "Vold"

#include <cutils/log.h>
#include <cutils/properties.h>

#include "ExFat.h"

static char MOUNT_EXFAT_PATH[] = "/system/bin/mount.exfat-fuse";
extern "C" int logwrap(int argc, const char **argv, int background);
extern "C" int mount(const char *, const char *, const char *, unsigned long, const void *);
int ExFat::doMount(const char *fsPath, const char *mountPoint,
                 bool ro, bool remount, bool executable,
                 int ownerUid, int ownerGid, int permMask, bool createLost) 
{
    int rc;
    unsigned long flags;
    char mountData[255];
	char options[255] = {};
	const char *args[6];

    flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);

    sprintf(mountData,
            "utf8,uid=%d,gid=%d,fmask=%o,dmask=%o,shortname=mixed",
            ownerUid, ownerGid, permMask, permMask);

	if(!remount){
		SLOGI("Trying to use exfat program to mount %s\n", fsPath);

		if(ro)
			snprintf(options, sizeof(options), "ro,%s", mountData);
		else
			snprintf(options, sizeof(options), "%s", mountData);

		args[0] = MOUNT_EXFAT_PATH;
		args[1] = "-o";
		args[2] = options;
		args[3] = fsPath;
		args[4] = mountPoint;
		args[5] = NULL;

		rc = logwrap(6, args, 1);

		if(rc == 0){
			SLOGE("exfat executed success.\n");
		}else{
			SLOGE("Failed to execute exfat.\n");
		}
	}else{
		rc = mount(fsPath, mountPoint, "fuseblk", flags, mountData);
	}

    return rc;
}



