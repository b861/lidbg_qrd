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

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/param.h>

#include <linux/kdev_t.h>

#include <cutils/properties.h>

#include <diskconfig/diskconfig.h>

#include <private/android_filesystem_config.h>

#define LOG_TAG "Vold"

#include <cutils/fs.h>
#include <cutils/log.h>

#include <string>

#include "Volume.h"
#include "VolumeManager.h"
#include "ResponseCode.h"
#include "Fat.h"
#include "Process.h"
#include "cryptfs.h"

#include "Ntfs.h"
#include "Exfat.h"
#include "../inc/lidbg_servicer.h"

extern "C" void dos_partition_dec(void const *pp, struct dos_partition *d);
extern "C" void dos_partition_enc(void *pp, struct dos_partition *d);


/*
 * Media directory - stuff that only media_rw user can see
 */
const char *Volume::MEDIA_DIR           = "/mnt/media_rw";

/*
 * Fuse directory - location where fuse wrapped filesystems go
 */
const char *Volume::FUSE_DIR           = "/storage";

/*
 * Path to external storage where *only* root can access ASEC image files
 */
const char *Volume::SEC_ASECDIR_EXT   = "/mnt/secure/asec";

/*
 * Path to internal storage where *only* root can access ASEC image files
 */
const char *Volume::SEC_ASECDIR_INT   = "/data/app-asec";

/*
 * Path to where secure containers are mounted
 */
const char *Volume::ASECDIR           = "/mnt/asec";

/*
 * Path to where OBBs are mounted
 */
const char *Volume::LOOPDIR           = "/mnt/obb";

const char *Volume::BLKID_PATH = "/system/bin/blkid";

static const char *stateToStr(int state) {
    if (state == Volume::State_Init)
        return "Initializing";
    else if (state == Volume::State_NoMedia)
        return "No-Media";
    else if (state == Volume::State_Idle)
        return "Idle-Unmounted";
    else if (state == Volume::State_Pending)
        return "Pending";
    else if (state == Volume::State_Mounted)
        return "Mounted";
    else if (state == Volume::State_Unmounting)
        return "Unmounting";
    else if (state == Volume::State_Checking)
        return "Checking";
    else if (state == Volume::State_Formatting)
        return "Formatting";
    else if (state == Volume::State_Shared)
        return "Shared-Unmounted";
    else if (state == Volume::State_SharedMnt)
        return "Shared-Mounted";
    else
        return "Unknown-Error";
}

Volume::Volume(VolumeManager *vm, const fstab_rec* rec, int flags) {
    mVm = vm;
    mDebug = false;
    mLabel = strdup(rec->label);
    mUuid = NULL;
    mUserLabel = NULL;
    mState = Volume::State_Init;
    mFlags = flags;
    mCurrentlyMountedKdev = -1;
    mPartIdx = rec->partnum;
    mRetryMount = false;
}

Volume::~Volume() {
    free(mLabel);
    free(mUuid);
    free(mUserLabel);
}

void Volume::setDebug(bool enable) {
    mDebug = enable;
}

dev_t Volume::getDiskDevice() {
    return MKDEV(0, 0);
};

dev_t Volume::getShareDevice() {
    return getDiskDevice();
}

void Volume::handleVolumeShared() {
}

void Volume::handleVolumeUnshared() {
}

int Volume::handleBlockEvent(NetlinkEvent *evt) {
    errno = ENOSYS;
    return -1;
}

void Volume::setUuid(const char* uuid) {
    char msg[256];

    if (mUuid) {
        free(mUuid);
    }

    if (uuid) {
        mUuid = strdup(uuid);
        snprintf(msg, sizeof(msg), "%s %s \"%s\"", getLabel(),
                getFuseMountpoint(), mUuid);
    } else {
        mUuid = NULL;
        snprintf(msg, sizeof(msg), "%s %s", getLabel(), getFuseMountpoint());
    }

    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeUuidChange, msg,
            false);
}

void Volume::setUserLabel(const char* userLabel) {
    char msg[256];

    if (mUserLabel) {
        free(mUserLabel);
    }

    if (userLabel) {
        mUserLabel = strdup(userLabel);
        snprintf(msg, sizeof(msg), "%s %s \"%s\"", getLabel(),
                getFuseMountpoint(), mUserLabel);
    } else {
        mUserLabel = NULL;
        snprintf(msg, sizeof(msg), "%s %s", getLabel(), getFuseMountpoint());
    }

    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeUserLabelChange,
            msg, false);
}

void Volume::setState(int state) {
    char msg[255];
    int oldState = mState;

    if (oldState == state) {
        SLOGW("Duplicate state (%d)\n", state);
	LIDBG_PRINT("Duplicate state (%d)\n", state);
        return;
    }

    if ((oldState == Volume::State_Pending) && (state != Volume::State_Idle)) {
        mRetryMount = false;
    }

    mState = state;

    SLOGD("Volume %s state changing %d (%s) -> %d (%s)", mLabel,
         oldState, stateToStr(oldState), mState, stateToStr(mState));
    LIDBG_PRINT("Volume %s state changing %d (%s) -> %d (%s)", mLabel, oldState, stateToStr(oldState), mState, stateToStr(mState));
    snprintf(msg, sizeof(msg),
             "Volume %s %s state changed from %d (%s) to %d (%s)", getLabel(),
             getFuseMountpoint(), oldState, stateToStr(oldState), mState,
             stateToStr(mState));

    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeStateChange,
                                         msg, false);
}

int Volume::createDeviceNode(const char *path, int major, int minor) {
    mode_t mode = 0660 | S_IFBLK;
    dev_t dev = (major << 8) | minor;
    if (mknod(path, mode, dev) < 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    return 0;
}


//add by wangyihong for supporting multi partitions
/* path: partition mount path. eg: '/mnt/usbhost1/8_1' */
int Volume::deleteDeviceNode(const char *path){
#if 0
    int major = 0, minor = 0;
	char devicePath[255];

	char *temp_str1 = NULL;
	char *temp_str2 = NULL;
	char str_major[256];
	char str_path[256];
	int len = 0;

	if(!path){
		SLOGE("Volume::deleteDeviceNode: path(%s) is invalid\n", path);
		return -1;
	}

	SLOGI("Volume::deleteDeviceNode: path=%s\n", path);

	/* get device major and minor from path */
	memset(str_major, 0, 256);
	memset(str_path, 0, 256);
	strcpy(str_path, path);

	temp_str1 = strrchr(str_path, '/');
	temp_str2 = strrchr(str_path, '_');
	if(temp_str1 == NULL || temp_str2 == NULL){
		SLOGE("Volume::deleteDeviceNode: path(%s) is invalid\n", path);
		return -1;
	}

	/* delete '/' & '_' */
	temp_str1++;
	temp_str2++;
	if(temp_str1 == NULL || temp_str2 == NULL){
		SLOGE("Volume::deleteDeviceNode: path(%s) is invalid\n", path);
		return -1;
	}

	len = strcspn(temp_str1, "_");
	strncpy(str_major, temp_str1, len);

	major = strtol(str_major, NULL, 10);
	minor = strtol(temp_str2, NULL, 10);

	SLOGI("Volume::deleteDeviceNode: major=%d, minor=%d\n", major, minor);

	/* delete DeviceNode */
	memset(devicePath, 0, 255);
	sprintf(devicePath, "/dev/block/vold/%d:%d", major, minor);

	if (unlink(devicePath)) {
		SLOGE("Volume::deleteDeviceNode: Failed to remove %s (%s)", path, strerror(errno));
		return -1;
	}else{
		SLOGI("Volume::deleteDeviceNode: delete DeviceNode '%s' successful\n", path);
	}

#endif

	return 0;
}

char* Volume::createMountPoint(const char *path, int major, int minor) {
	char* mountpoint = (char*) malloc(sizeof(char)*256);

	memset(mountpoint, 0, sizeof(char)*256);
	//sprintf(mountpoint, "%s/%d_%d", path, major, minor);
	sprintf(mountpoint, "%s/disk_%d", path, minor);
	if( access(mountpoint, F_OK) ){
		SLOGI("Volume: file '%s' is not exist, create it", mountpoint);

		if(mkdir(mountpoint, 0777)){
			SLOGW("Volume: create file '%s' failed, errno is %d", mountpoint, errno);
			LIDBG_PRINT("Volume: create file '%s' failed, errno is %d", mountpoint, errno);
			return NULL;
		}
	}else{
	
		SLOGW("Volume: file '%s' is exist, can not create it", mountpoint);
		LIDBG_PRINT("Volume: file '%s' is exist, can not create it", mountpoint);
		return mountpoint;
	}

	return mountpoint;
}

int Volume::deleteMountPoint(char* mountpoint) {
	if(mountpoint){
		SLOGW("Volume::deleteMountPoint: %s exist", mountpoint); 
		LIDBG_PRINT("Volume::deleteMountPoint: %s exist", mountpoint); 
		rmdir(mountpoint);
		if( !access(mountpoint, F_OK) ){
			LIDBG_PRINT("Volume::deleteMountPoint: %s", mountpoint);
			SLOGW("Volume::deleteMountPoint: %s", mountpoint);
			if(rmdir(mountpoint)){
				SLOGW("Volume: remove file '%s' failed, errno is %d", mountpoint, errno);
				LIDBG_PRINT("Volume: remove file '%s' failed, errno is %d", mountpoint, errno);
				return -1;
			}
		}

	
		free(mountpoint);
		mountpoint = NULL;
	}

	return 0;
}

void Volume::saveUnmountPoint(char* mountpoint){
	int i = 0;

	for(i = 0; i < MAX_UNMOUNT_PARTITIONS; i++){
		if(mUnMountPart[i] == NULL){
			mUnMountPart[i] = mMountPart[i];
		}
	}

	if(i >= MAX_UNMOUNT_PARTITIONS){
		SLOGI("Volume::saveUnmountPoint: unmount point is over %d", MAX_UNMOUNT_PARTITIONS);
	}

	return;
}


void Volume::deleteUnMountPoint(int clear){
	int i = 0;

	for(i = 0; i < MAX_UNMOUNT_PARTITIONS; i++){
		if(mUnMountPart[i]){
			SLOGW("Volume::deleteUnMountPoint: %s", mUnMountPart[i]);
			LIDBG_PRINT("Volume::deleteUnMountPoint: %s", mUnMountPart[i]);
			if(deleteMountPoint(mUnMountPart[i]) == 0){
				deleteDeviceNode(mUnMountPart[i]);
				mUnMountPart[i] = NULL;
			}
		}
	}

	return;
}



int Volume::formatVol(bool wipe) {

    if (getState() == Volume::State_NoMedia) {
        errno = ENODEV;
        return -1;
    } else if (getState() != Volume::State_Idle) {
        errno = EBUSY;
        return -1;
    }

    if (isMountpointMounted(getMountpoint())) {
        SLOGW("Volume is idle but appears to be mounted - fixing");
	LIDBG_PRINT("Volume is idle but appears to be mounted - fixing");
        setState(Volume::State_Mounted);
        // mCurrentlyMountedKdev = XXX
        errno = EBUSY;
        return -1;
    }

    bool formatEntireDevice = (mPartIdx == -1);
    char devicePath[255];
    dev_t diskNode = getDiskDevice();
    dev_t partNode =
        MKDEV(MAJOR(diskNode),
              MINOR(diskNode) + (formatEntireDevice ? 1 : mPartIdx));

    setState(Volume::State_Formatting);

    int ret = -1;
    // Only initialize the MBR if we are formatting the entire device
    if (formatEntireDevice) {
        sprintf(devicePath, "/dev/block/vold/%d:%d",
                MAJOR(diskNode), MINOR(diskNode));

        if (initializeMbr(devicePath)) {
            SLOGE("Failed to initialize MBR (%s)", strerror(errno));
	LIDBG_PRINT("Failed to initialize MBR (%s)", strerror(errno));
            goto err;
        }
    }

    sprintf(devicePath, "/dev/block/vold/%d:%d",
            MAJOR(partNode), MINOR(partNode));

    if (mDebug) {
        SLOGI("Formatting volume %s (%s)", getLabel(), devicePath);
    }

    if (Fat::format(devicePath, 0, wipe)) {
        SLOGE("Failed to format (%s)", strerror(errno));
	LIDBG_PRINT("Failed to format (%s)", strerror(errno));
        goto err;
    }

    ret = 0;

err:
    setState(Volume::State_Idle);
    return ret;
}

bool Volume::isMountpointMounted(const char *path) {
    char device[256];
    char mount_path[256];
    char rest[256];
    FILE *fp;
    char line[1024];

    if (!(fp = fopen("/proc/mounts", "r"))) {
        SLOGE("Error opening /proc/mounts (%s)", strerror(errno));
	LIDBG_PRINT("Error opening /proc/mounts (%s)", strerror(errno));
        return false;
    }

    while(fgets(line, sizeof(line), fp)) {
        line[strlen(line)-1] = '\0';
        sscanf(line, "%255s %255s %255s\n", device, mount_path, rest);
        if (!strcmp(mount_path, path)) {
            fclose(fp);
            return true;
        }
    }

    fclose(fp);
    return false;
}

int Volume::mountVol() {
    dev_t deviceNodes[4];
    int n, i, rc = 0;
    char errmsg[255];
    int mounted = 0;
	
    int flags = getFlags();
    bool providesAsec = (flags & VOL_PROVIDES_ASEC) != 0;

    // TODO: handle "bind" style mounts, for emulated storage

    char decrypt_state[PROPERTY_VALUE_MAX];
    char crypto_state[PROPERTY_VALUE_MAX];
    char encrypt_progress[PROPERTY_VALUE_MAX];

    property_get("vold.decrypt", decrypt_state, "");
    property_get("vold.encrypt_progress", encrypt_progress, "");

    /* Don't try to mount the volumes if we have not yet entered the disk password
     * or are in the process of encrypting.
     */
    if ((getState() == Volume::State_NoMedia) ||
        ((!strcmp(decrypt_state, "1") || encrypt_progress[0]) && providesAsec)) {
        snprintf(errmsg, sizeof(errmsg),
                 "Volume %s %s mount failed - no media",
                 getLabel(), getFuseMountpoint());
        mVm->getBroadcaster()->sendBroadcast(
                                         ResponseCode::VolumeMountFailedNoMedia,
                                         errmsg, false);
        errno = ENODEV;
        return -1;
    } else if (getState() != Volume::State_Idle) {
        errno = EBUSY;
        if (getState() == Volume::State_Pending) {
            mRetryMount = true;
        }
        return -1;
    }

    if (isMountpointMounted(getMountpoint())) {
        SLOGW("Volume is idle but appears to be mounted - fixing");
	LIDBG_PRINT("Volume is idle but appears to be mounted - fixing");
	doUnmount(getMountpoint(), true);
      //  setState(Volume::State_Mounted);
        // mCurrentlyMountedKdev = XXX
       // return 0;
    }

    n = getDeviceNodes((dev_t *) &deviceNodes, 4);
    if (!n) {
        SLOGE("Failed to get device nodes (%s)\n", strerror(errno));
	LIDBG_PRINT("Failed to get device nodes (%s)\n", strerror(errno));
        return -1;
    }

    /* If we're running encrypted, and the volume is marked as encryptable and nonremovable,
     * and also marked as providing Asec storage, then we need to decrypt
     * that partition, and update the volume object to point to it's new decrypted
     * block device
     */
    property_get("ro.crypto.state", crypto_state, "");
    if (providesAsec &&
        ((flags & (VOL_NONREMOVABLE | VOL_ENCRYPTABLE))==(VOL_NONREMOVABLE | VOL_ENCRYPTABLE)) &&
        !strcmp(crypto_state, "encrypted") && !isDecrypted()) {
       char new_sys_path[MAXPATHLEN];
       char nodepath[256];
       int new_major, new_minor;
	SLOGE("run into step 1...\n");
	LIDBG_PRINT("run into step 1...\n");
       if (n != 1) {
           /* We only expect one device node returned when mounting encryptable volumes */
           SLOGE("Too many device nodes returned when mounting %d\n", getMountpoint());
	LIDBG_PRINT("Too many device nodes returned when mounting %s\n", getMountpoint());
           return -1;
       }

       if (cryptfs_setup_volume(getLabel(), MAJOR(deviceNodes[0]), MINOR(deviceNodes[0]),
                                new_sys_path, sizeof(new_sys_path),
                                &new_major, &new_minor)) {
           SLOGE("Cannot setup encryption mapping for %d\n", getMountpoint());
	LIDBG_PRINT("Cannot setup encryption mapping for %s\n", getMountpoint());
           return -1;
       }
       /* We now have the new sysfs path for the decrypted block device, and the
        * majore and minor numbers for it.  So, create the device, update the
        * path to the new sysfs path, and continue.
        */
        snprintf(nodepath,
                 sizeof(nodepath), "/dev/block/vold/%d:%d",
                 new_major, new_minor);
        if (createDeviceNode(nodepath, new_major, new_minor)) {
            SLOGE("Error making device node '%s' (%s)", nodepath,
                                                       strerror(errno));
	LIDBG_PRINT("Error making device node '%s' (%s)", nodepath,
                                                       strerror(errno));		
        }

        // Todo: Either create sys filename from nodepath, or pass in bogus path so
        //       vold ignores state changes on this internal device.
        updateDeviceInfo(nodepath, new_major, new_minor);

        /* Get the device nodes again, because they just changed */
        n = getDeviceNodes((dev_t *) &deviceNodes, 4);
        if (!n) {
            SLOGE("Failed to get device nodes (%s)\n", strerror(errno));
	  LIDBG_PRINT("Failed to get device nodes (%s)\n", strerror(errno));
            return -1;
        }
    }

    SLOGE("the num of devices nodes = %d\n", n);
  LIDBG_PRINT("the num of devices nodes = %d\n", n);
    for (i = 0; i < n; i++) {
        char devicePath[255];

        sprintf(devicePath, "/dev/block/vold/%d:%d", MAJOR(deviceNodes[i]),
                MINOR(deviceNodes[i]));

        SLOGI("%s being considered for volume %s\n", devicePath, getLabel());
	LIDBG_PRINT("%s being considered for volume %s\n", devicePath, getLabel());
        errno = 0;
        int gid;
        setState(Volume::State_Checking);
	mMountedPartNum = n;

if(n==1){

	if (Fat::check(devicePath)==0) {
		if (Fat::doMount(devicePath, getMountpoint(), false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007, true)) {
		 	SLOGE("%s failed to mount via VFAT (%s)\n", devicePath, strerror(errno));
			LIDBG_PRINT("%s failed to mount via VFAT (%s)\n", devicePath, strerror(errno));
				continue;
		}
	}
	else if(Ntfs::check(devicePath) == 0)
	{
		SLOGW("this is NTFS filesystem, ready to mount!\n");
		LIDBG_PRINT("this is NTFS filesystem, ready to mount!\n");
		if (Ntfs::doMount(devicePath, getMountpoint(), false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007, true))
		{
		        SLOGE("%s failed to mount via NTFS (%s)\n", devicePath, strerror(errno));
			LIDBG_PRINT("%s failed to mount via NTFS (%s)\n", devicePath, strerror(errno));
		        continue;
		}
	}
	else{
		if (errno == ENODATA) {
	                SLOGW("%s unkown filesystem\n", devicePath);
			LIDBG_PRINT("%s unkown filesystem\n", devicePath);
	                continue;
            	}
	            errno = EIO;
	            /* Badness - abort the mount */
	            SLOGE("%s failed FS checks (%s)", devicePath, strerror(errno));
		LIDBG_PRINT("%s failed FS checks (%s)", devicePath, strerror(errno));
	            setState(Volume::State_Idle);
	            return -1;
	}

        extractMetadata(devicePath);

        if (providesAsec && mountAsecExternal() != 0) {
            SLOGE("Failed to mount secure area (%s)", strerror(errno));
	  LIDBG_PRINT("Failed to mount secure area (%s)", strerror(errno));
            umount(getMountpoint());
            setState(Volume::State_Idle);
            return -1;
        }

        char service[64];
        snprintf(service, 64, "fuse_%s", getLabel());
  	 property_set("ctl.start", service);

        setState(Volume::State_Mounted);
        mCurrentlyMountedKdev = deviceNodes[i];
        return 0;
}


	if(n>1){
	            SLOGI("[WANG]: this is muti partitions disk.\n");
		    LIDBG_PRINT("[WANG]: this is muti partitions disk.\n");
		    mFuseMountPart[i] = createMountPoint( "/storage/udisk", MAJOR(deviceNodes[i]), MINOR(deviceNodes[i]) );
	            mMountPart[i] = createMountPoint( "/mnt/media_rw/udisk", MAJOR(deviceNodes[i]), MINOR(deviceNodes[i]) );
	            if(mMountPart[i] == NULL)
	            {
	                SLOGE("Part is already mount, can not mount again, (%s)\n", strerror(errno));
		      LIDBG_PRINT("Part is already mount, can not mount again, (%s)\n", strerror(errno));
	                continue;
	            }
	 	if (Fat::check(devicePath)==0){
		            if (Fat::doMount(devicePath, mMountPart[i], false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007, true))
		            {
		                SLOGE("Part(%s) failed to move mount (%s)\n", mMountPart[i], strerror(errno));
				LIDBG_PRINT("Part(%s) failed to move mount (%s)\n", mMountPart[i], strerror(errno));
		                deleteMountPoint(mMountPart[i]);
		                mMountPart[i] = NULL;
		                continue;
		            }
	 	}
		else if(Ntfs::check(devicePath) == 0)
		{
			SLOGW("this is NTFS filesystem, ready to mount!\n");
			LIDBG_PRINT("this is NTFS filesystem, ready to mount!\n");
			if (Ntfs::doMount(devicePath, mMountPart[i], false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007, true))
			{
			    SLOGE("%s failed to mount via NTFS (%s)\n", mMountPart[i], strerror(errno));
			LIDBG_PRINT("%s failed to mount via NTFS (%s)\n", mMountPart[i], strerror(errno));
			    deleteMountPoint(mMountPart[i]);
		            mMountPart[i] = NULL;
			    continue;
			}
		}
	            SLOGW("mountVol: mount %s, successful\n", mMountPart[i]);
		    LIDBG_PRINT("mountVol: mount %s, successful\n", mMountPart[i]);
	            mCurrentlyMountedKdev = deviceNodes[i];
	            mounted++;

			if(n==(i+1)){
				SLOGW("report app mounted  successful\n");
			extractMetadata(devicePath);

			if (providesAsec && mountAsecExternal() != 0) {
			    SLOGE("Failed to mount secure area (%s)", strerror(errno));
			    LIDBG_PRINT("Failed to mount secure area (%s)", strerror(errno));
			    umount(getMountpoint());
			    setState(Volume::State_Idle);
			    return -1;
			}

			char service[64];
			snprintf(service, 64, "fuse_%s", getLabel());
			SLOGE("getlabel is %s\n", getLabel());
			//property_set("ctl.start", service);
			property_set("ctl.start", "fuse_disk1");
			property_set("ctl.start", "fuse_disk2");
			property_set("ctl.start", "fuse_disk3");
			property_set("ctl.start", "fuse_disk4");

			setState(Volume::State_Mounted);
			return 0;
			}
		 
	}


 




	
		

	/*if(Exfat::check(devicePath) == 0)
	{
		SLOGW("this is exfat filesystem, no process!\n");
	}	
	else*/ 
	#if 0
	if(Ntfs::check(devicePath) == 0)
	{
		SLOGW("this is NTFS filesystem, ready to mount!\n");
		if (Ntfs::doMount(devicePath, getMountpoint(), false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007, true))
		{
		        SLOGE("%s failed to mount via NTFS (%s)\n", devicePath, strerror(errno));
		        continue;
		}
	}
        else if (Fat::check(devicePath)==0) {
		if (Fat::doMount(devicePath, getMountpoint(), false, false, false, AID_MEDIA_RW, AID_MEDIA_RW, 0007, true)) {
    		 	SLOGE("%s failed to mount via VFAT (%s)\n", devicePath, strerror(errno));
            		continue;
        	}
        }
	else{
		if (errno == ENODATA) {
	                SLOGW("%s unkown filesystem\n", devicePath);
	                continue;
            	}
	            errno = EIO;
	            /* Badness - abort the mount */
	            SLOGE("%s failed FS checks (%s)", devicePath, strerror(errno));
	            setState(Volume::State_Idle);
	            return -1;
	}

        extractMetadata(devicePath);

        if (providesAsec && mountAsecExternal() != 0) {
            SLOGE("Failed to mount secure area (%s)", strerror(errno));
            umount(getMountpoint());
            setState(Volume::State_Idle);
            return -1;
        }

        char service[64];
        snprintf(service, 64, "fuse_%s", getLabel());
        property_set("ctl.start", service);

        setState(Volume::State_Mounted);
        mCurrentlyMountedKdev = deviceNodes[i];
        return 0;
		#endif
    }

    SLOGE("Volume %s found no suitable devices for mounting :(\n", getLabel());
    setState(Volume::State_Idle);

    return -1;
}

int Volume::mountAsecExternal() {
    char legacy_path[PATH_MAX];
    char secure_path[PATH_MAX];

    snprintf(legacy_path, PATH_MAX, "%s/android_secure", getMountpoint());
    snprintf(secure_path, PATH_MAX, "%s/.android_secure", getMountpoint());

    // Recover legacy secure path
    if (!access(legacy_path, R_OK | X_OK) && access(secure_path, R_OK | X_OK)) {
        if (rename(legacy_path, secure_path)) {
            SLOGE("Failed to rename legacy asec dir (%s)", strerror(errno));
	  LIDBG_PRINT("Failed to rename legacy asec dir (%s)", strerror(errno)); 
        }
    }

    if (fs_prepare_dir(secure_path, 0770, AID_MEDIA_RW, AID_MEDIA_RW) != 0) {
        SLOGW("fs_prepare_dir failed: %s", strerror(errno));
	LIDBG_PRINT("fs_prepare_dir failed: %s", strerror(errno));
        return -1;
    }

    if (mount(secure_path, SEC_ASECDIR_EXT, "", MS_BIND, NULL)) {
        SLOGE("Failed to bind mount points %s -> %s (%s)", secure_path,
                SEC_ASECDIR_EXT, strerror(errno));
	LIDBG_PRINT("Failed to bind mount points %s -> %s (%s)", secure_path,
                SEC_ASECDIR_EXT, strerror(errno));
        return -1;
    }

    return 0;
}

int Volume::doUnmount(const char *path, bool force) {
    int retries = 5;

    if (mDebug) {
        SLOGD("Unmounting {%s}, force = %d", path, force);
	LIDBG_PRINT("Unmounting {%s}, force = %d", path, force);
    }

    while (retries--) {
        if (!umount(path) || errno == EINVAL || errno == ENOENT) {
            SLOGI("%s sucessfully unmounted", path);
	   LIDBG_PRINT("%s sucessfully unmounted", path);
            return 0;
        }

        int action = 0;

        if (force) {
            if (retries == 1) {
                action = 2; // SIGKILL
            } else if (retries == 2) {
                action = 1; // SIGHUP
            }
        }

        SLOGW("Failed to unmount %s (%s, retries %d, action %d)",
                path, strerror(errno), retries, action);
        LIDBG_PRINT("Failed to unmount %s (%s, retries %d, action %d)",
                path, strerror(errno), retries, action);
        Process::killProcessesWithOpenFiles(path, action);
        usleep(1000*1000);
    }
    errno = EBUSY;
    SLOGE("Giving up on unmount %s (%s)", path, strerror(errno));
    LIDBG_PRINT("Giving up on unmount %s (%s)", path, strerror(errno));
    return -1;
}

int Volume::unmountVol(bool force, bool revert) {
    int i, rc;

    int flags = getFlags();
    bool providesAsec = (flags & VOL_PROVIDES_ASEC) != 0;

    if (getState() != Volume::State_Mounted) {
        SLOGE("Volume %s unmount request when not mounted", getLabel());
	LIDBG_PRINT("Volume %s unmount request when not mounted", getLabel());
        errno = EINVAL;
        return UNMOUNT_NOT_MOUNTED_ERR;
    }

    setState(Volume::State_Unmounting);
    usleep(1000 * 1000); // Give the framework some time to react

    char service[64];
    snprintf(service, 64, "fuse_%s", getLabel());
    property_set("ctl.stop", service);
	property_set("ctl.stop", "fuse_disk1");
	property_set("ctl.stop", "fuse_disk2");
	property_set("ctl.stop", "fuse_disk3");
	property_set("ctl.stop", "fuse_disk4");
    /* Give it a chance to stop.  I wish we had a synchronous way to determine this... */
    sleep(1);

	//add by flyaudio 
	if(mMountedPartNum>1){
		for(i = mMountedPartNum - 1; i >= 0; i--){
			 SLOGW("fuseMountPart is %s", mFuseMountPart[i]);
			LIDBG_PRINT("fuseMountPart is %s", mFuseMountPart[i]);
		    if (doUnmount(mFuseMountPart[i], force) != 0) {
		        SLOGE("Failed to unmount %s (%s)", mFuseMountPart[i], strerror(errno));
			LIDBG_PRINT("Failed to unmount %s (%s)", mFuseMountPart[i], strerror(errno));
		    }
			 deleteMountPoint(mFuseMountPart[i]);
			if(mMountPart[i])
		            {
				if (doUnmount(mMountPart[i], force) != 0) {
					SLOGE("Failed to unmount %s (%s)", getMountpoint(), strerror(errno));
					LIDBG_PRINT("Failed to unmount %s (%s)", getMountpoint(), strerror(errno));
					deleteMountPoint(mMountPart[i]);
					goto fail_remount_secure;
				}

		                if(deleteMountPoint(mMountPart[i]))
		                	saveUnmountPoint(mMountPart[i]); 

		                mMountPart[i] = NULL;
		            }
		}
	}
    // TODO: determine failure mode if FUSE times out

    if (providesAsec && doUnmount(Volume::SEC_ASECDIR_EXT, force) != 0) {
        SLOGE("Failed to unmount secure area on %s (%s)", getMountpoint(), strerror(errno));
	LIDBG_PRINT("Failed to unmount secure area on %s (%s)", getMountpoint(), strerror(errno));
        goto out_mounted;
    }
	SLOGI("begin to unmount  the last one");

    /* Now that the fuse daemon is dead, unmount it */
    if (doUnmount(getFuseMountpoint(), force) != 0) {
        SLOGE("Failed to unmount %s (%s)", getFuseMountpoint(), strerror(errno));
	LIDBG_PRINT("Failed to unmount %s (%s)", getFuseMountpoint(), strerror(errno));
        goto fail_remount_secure;
    }

    /* Unmount the real sd card */
    if (doUnmount(getMountpoint(), force) != 0) {
        SLOGE("Failed to unmount %s (%s)", getMountpoint(), strerror(errno));
	LIDBG_PRINT("Failed to unmount %s (%s)", getMountpoint(), strerror(errno));
        goto fail_remount_secure;
    }

    SLOGI("%s unmounted successfully", getMountpoint());
    LIDBG_PRINT("%s unmounted successfully", getMountpoint());
    /* If this is an encrypted volume, and we've been asked to undo
     * the crypto mapping, then revert the dm-crypt mapping, and revert
     * the device info to the original values.
     */
    if (revert && isDecrypted()) {
        cryptfs_revert_volume(getLabel());
        revertDeviceInfo();
        SLOGI("Encrypted volume %s reverted successfully", getMountpoint());
    }


    setUuid(NULL);
    setUserLabel(NULL);
    setState(Volume::State_Idle);
    mCurrentlyMountedKdev = -1;
    return 0;

fail_remount_secure:
    if (providesAsec && mountAsecExternal() != 0) {
        SLOGE("Failed to remount secure area (%s)", strerror(errno));
        goto out_nomedia;
    }

out_mounted:
    setState(Volume::State_Mounted);
    return -1;

out_nomedia:
    setState(Volume::State_NoMedia);
    return -1;
}

int Volume::initializeMbr(const char *deviceNode) {
    struct disk_info dinfo;

    memset(&dinfo, 0, sizeof(dinfo));

    if (!(dinfo.part_lst = (struct part_info *) malloc(MAX_NUM_PARTS * sizeof(struct part_info)))) {
        SLOGE("Failed to malloc prt_lst");
        return -1;
    }

    memset(dinfo.part_lst, 0, MAX_NUM_PARTS * sizeof(struct part_info));
    dinfo.device = strdup(deviceNode);
    dinfo.scheme = PART_SCHEME_MBR;
    dinfo.sect_size = 512;
    dinfo.skip_lba = 2048;
    dinfo.num_lba = 0;
    dinfo.num_parts = 1;

    struct part_info *pinfo = &dinfo.part_lst[0];

    pinfo->name = strdup("android_sdcard");
    pinfo->flags |= PART_ACTIVE_FLAG;
    pinfo->type = PC_PART_TYPE_FAT32;
    pinfo->len_kb = -1;

    int rc = apply_disk_config(&dinfo, 0);

    if (rc) {
        SLOGE("Failed to apply disk configuration (%d)", rc);
	LIDBG_PRINT("Failed to apply disk configuration (%d)", rc);
        goto out;
    }

 out:
    free(pinfo->name);
    free(dinfo.device);
    free(dinfo.part_lst);

    return rc;
}

/*
 * Use blkid to extract UUID and label from device, since it handles many
 * obscure edge cases around partition types and formats. Always broadcasts
 * updated metadata values.
 */
int Volume::extractMetadata(const char* devicePath) {
    int res = 0;

    std::string cmd;
    cmd = BLKID_PATH;
    cmd += " -c /dev/null ";
    cmd += devicePath;

    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) {
        ALOGE("Failed to run %s: %s", cmd.c_str(), strerror(errno));
	LIDBG_PRINT("Failed to run %s: %s", cmd.c_str(), strerror(errno));
        res = -1;
        goto done;
    }

    char line[1024];
    char value[128];
    if (fgets(line, sizeof(line), fp) != NULL) {
        ALOGD("blkid identified as %s", line);

        char* start = strstr(line, "UUID=");
        if (start != NULL && sscanf(start + 5, "\"%127[^\"]\"", value) == 1) {
            setUuid(value);
        } else {
            setUuid(NULL);
        }

        start = strstr(line, "LABEL=");
        if (start != NULL && sscanf(start + 6, "\"%127[^\"]\"", value) == 1) {
            setUserLabel(value);
        } else {
            setUserLabel(NULL);
        }
    } else {
        ALOGW("blkid failed to identify %s", devicePath);
        res = -1;
    }

    pclose(fp);

done:
    if (res == -1) {
        setUuid(NULL);
        setUserLabel(NULL);
    }
    return res;
}
