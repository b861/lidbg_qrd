/* notify:
 add one feature,note its item down below as the example.	2013.3.15

1:add:dev node test
2:add:test_opendir

*/


#include "dev_node_test.h"


#define mdbg(msg...) printf("[futengfei]"msg);
typedef struct
{
    int fd;
    unsigned char err;
    char *dev_node;
} sdev_node_test;
sdev_node_test test_dev1 = { -1, -1, "/dev/uboxserver"};

void test_dev_nod()
{
    char *gps_data[255];
    int count = 5;
    test_dev1.fd = open(test_dev1.dev_node, O_RDWR);
    mdbg("%d\n", test_dev1.fd);

    while(test_dev1.fd > 0 && count > 0)
    {
        test_dev1.err = read(test_dev1.fd, gps_data, sizeof(gps_data));
        mdbg("%d %s\n", sizeof(gps_data), gps_data);
        sleep(1);
        count--;
    }
}

static struct utsname host_msg;
void get_host_msg()
{
    char hostname[32];
    uname(&host_msg);
    printf("\n%s,%s,%s,%s,%s\n", host_msg.sysname, host_msg.machine, host_msg.nodename, host_msg.release, host_msg.version);

    if( gethostname(hostname, sizeof(hostname)) )
    {
        printf("gethostname err.\n");
        return ;
    }
    else
        printf("localhost name:[%s]\n", hostname);

}
void test_opendir(const char *dir_path)
{
    struct dirent *entry;
    DIR *dir;
    int file_count = 0;
    //S1
    if (access(dir_path, R_OK) == 0)
    {
        printf("dir exist:[%s]\n", dir_path);
    }
    else
    {
        printf("dir miss:[%s]  return;\n", dir_path);
        return ;
    }

    //S2
    dir = opendir(dir_path);
    if (dir == NULL)
    {
        printf("Could not open %s\n", dir_path);
        return -1;
    }
    //S3
    while ((entry = readdir(dir)))
    {
        const char *name = entry->d_name;
        file_count++;
        printf("F.LIST:%d = [%s]------[%c,%c,%c]\n", file_count, entry->d_name, name[0], name[1], name[2]);
    }
    //S4
    closedir(dir);
}
#define SERVPORT 3333
#define BACKLOG 10
void test_socket_smthing()
{
    int sockfd, client_fd;
    struct sockaddr_in my_addr;//local addr msg
    struct sockaddr_in remote_addr;//client addr msg
    printf("socket ====================test_socket_smthing\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket err\n");
        return ;
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(SERVPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    printf("my_addr.sin_addr: %s\n", inet_ntoa(my_addr.sin_addr));
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("bind err\n");
        return ;
    }
    if (listen(sockfd, BACKLOG) == -1)
    {
        printf("while(1)\n");
        return ;
    }

    while(1)
    {
        int sin_size = sizeof(struct sockaddr_in);
        printf("while(1)---------1\n");
		
	//accept will block to wait client ; we are server;
        if ((client_fd = accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size)) == -1)
        {
            printf("accept err\n");
            continue;
        }
        printf("while(1)---------2\n");
        printf("received a connection from %s\n", inet_ntoa(remote_addr.sin_addr));
        if (!fork())
        {
            if (send(client_fd, "Hello, you are connected!\n", 26, 0) == -1)
                printf("send er\n");
            printf("while(1)---------3\n");

            close(client_fd);
            return ;
        }
        printf("while(1)---------4\n");

        close(client_fd);
    }


}

int main (int argc, char **argv)
{
    mdbg("=========test start1=============\n\n\n");
    usleep(1000 * 1000); //1000ms or sleep(1);
    get_host_msg();
    test_dev_nod();
    test_opendir("/mnt/sdcard");
    test_socket_smthing();


    mdbg("=========test stop1=============\n\n\n");
    return 0;
}
