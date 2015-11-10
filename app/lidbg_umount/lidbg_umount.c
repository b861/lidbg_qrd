
#include "lidbg_servicer.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/mount.h>

#undef   LOG_TAG
#define  LOG_TAG "ProcessKiller"
#include <cutils/log.h>


bool mDebug = true;
bool force = true;
int checkFileMaps(int pid, const char *mountPoint, char *openFilename, size_t max);
int checkSymLink(int pid, const char *mountPoint, const char *name);

int pathMatchesMountPoint(const char *path, const char *mountPoint)
{
    int length = strlen(mountPoint);
    if (length > 1 && strncmp(path, mountPoint, length) == 0)
    {
        // we need to do extra checking if mountPoint does not end in a '/'
        if (mountPoint[length - 1] == '/')
            return 1;
        // if mountPoint does not have a trailing slash, we need to make sure
        // there is one in the path to avoid partial matches.
        return (path[length] == 0 || path[length] == '/');
    }

    return 0;
}
int readSymLink(const char *path, char *link, size_t max)
{
    struct stat s;
    int length;

    if (lstat(path, &s) < 0)
        return 0;
    if ((s.st_mode & S_IFMT) != S_IFLNK)
        return 0;

    // we have a symlink
    length = readlink(path, link, max - 1);
    if (length <= 0)
        return 0;
    link[length] = 0;
    return 1;
}
int checkFileDescriptorSymLinks(int pid, const char *mountPoint, char *openFilename, size_t max)
{
    // compute path to process's directory of open files
    char    path[PATH_MAX];
    sprintf(path, "/proc/%d/fd", pid);
    DIR *dir = opendir(path);
    if (!dir)
        return 0;

    // remember length of the path
    int parent_length = strlen(path);
    // append a trailing '/'
    path[parent_length++] = '/';

    struct dirent *de;
    while ((de = readdir(dir)))
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")
                || strlen(de->d_name) + parent_length + 1 >= PATH_MAX)
            continue;

        // append the file name, after truncating to parent directory
        path[parent_length] = 0;
        strcat(path, de->d_name);

        char link[PATH_MAX];

        if (readSymLink(path, link, sizeof(link)) && pathMatchesMountPoint(link, mountPoint))
        {
            if (openFilename)
            {
                memset(openFilename, 0, max);
                strncpy(openFilename, link, max - 1);
            }
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);
    return 0;
}
void getProcessName(int pid, char *buffer, size_t max)
{
    int fd;
    snprintf(buffer, max, "/proc/%d/cmdline", pid);
    fd = open(buffer, O_RDONLY);
    if (fd < 0)
    {
        strcpy(buffer, "???");
    }
    else
    {
        int length = read(fd, buffer, max - 1);
        buffer[length] = 0;
        close(fd);
    }
}
int getPid(const char *s)
{
    int result = 0;
    while (*s)
    {
        if (!isdigit(*s)) return -1;
        result = 10 * result + (*s++ - '0');
    }
    return result;
}
void killProcessesWithOpenFiles(const char *path, int action)
{
    DIR    *dir;
    struct dirent *de;

    if (!(dir = opendir("/proc")))
    {
        SLOGE("opendir failed (%s)", strerror(errno));
        return;
    }

    while ((de = readdir(dir)))
    {
        int killed = 0;
        int pid = getPid(de->d_name);
        char name[PATH_MAX];

        if (pid == -1)
            continue;
        getProcessName(pid, name, sizeof(name));

        char openfile[PATH_MAX];

        if (checkFileDescriptorSymLinks(pid, path, openfile, sizeof(openfile)))
        {
            lidbg("Process %s (%d) has open file %s", name, pid, openfile);
        }
        else if (checkFileMaps(pid, path, openfile, sizeof(openfile)))
        {
            lidbg("Process %s (%d) has open filemap for %s", name, pid, openfile);
        }
        else if (checkSymLink(pid, path, "cwd"))
        {
            lidbg("Process %s (%d) has cwd within %s", name, pid, path);
        }
        else if (checkSymLink(pid, path, "root"))
        {
            lidbg("Process %s (%d) has chroot within %s", name, pid, path);
        }
        else if (checkSymLink(pid, path, "exe"))
        {
            lidbg("Process %s (%d) has executable path within %s", name, pid, path);
        }
        else
        {
            continue;
        }
        if (action == 1)
        {
            SLOGW("Sending SIGHUP to process %d", pid);
            lidbg("Sending SIGHUP to process %d,%s", pid, name);
            kill(pid, SIGTERM);
        }
        else if (action == 2)
        {
            SLOGE("Sending SIGKILL to process %d", pid);
            lidbg("Sending SIGKILL to process %d,%s", pid, name);
            kill(pid, SIGKILL);
        }
    }
    closedir(dir);
}


int checkSymLink(int pid, const char *mountPoint, const char *name)
{
    char    path[PATH_MAX];
    char    link[PATH_MAX];

    sprintf(path, "/proc/%d/%s", pid, name);
    if (readSymLink(path, link, sizeof(link)) && pathMatchesMountPoint(link, mountPoint))
        return 1;
    return 0;
}


int checkFileMaps(int pid, const char *mountPoint, char *openFilename, size_t max)
{
    FILE *file;
    char buffer[PATH_MAX + 100];

    sprintf(buffer, "/proc/%d/maps", pid);
    file = fopen(buffer, "r");
    if (!file)
        return 0;

    while (fgets(buffer, sizeof(buffer), file))
    {
        // skip to the path
        const char *path = strchr(buffer, '/');
        if (path && pathMatchesMountPoint(path, mountPoint))
        {
            if (openFilename)
            {
                memset(openFilename, 0, max);
                strncpy(openFilename, path, max - 1);
            }
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}


int main(int argc, char **argv)
{
    argc = argc;
    argv = argv;
    int retries = 5;
    int action = 2;//1.SIGHUP 2.SIGKILL
    const char *path = "/storage/udisk";
    if (mDebug)
    {
        SLOGD("Unmounting {%s}, force = %d", path, force);
        lidbg("Unmounting {%s}, force = %d", path, force);
    }

    while (retries--)
    {
        if ((!umount(path)) || (errno == EINVAL) || (errno == ENOENT))
        {
            SLOGI("%s sucessfully unmounted!!", path);
            lidbg("%s sucessfully unmounted!!", path);
            return 0;
        }

        if (retries < 3)
        {
            action = 2; // SIGKILL
        }

        SLOGW("Failed to unmount %s (%s, retries %d, action %d)",
              path, strerror(errno), retries, action);
        lidbg("Failed to unmount %s (%s, retries %d, action %d)",
              path, strerror(errno), retries, action);
        killProcessesWithOpenFiles(path, action);
        usleep(500 * 1000);
    }
    errno = EBUSY;
    SLOGE("Giving up on unmount %s (%s)", path, strerror(errno));
    lidbg("Giving up on unmount %s (%s)", path, strerror(errno));
    return -1;
}
