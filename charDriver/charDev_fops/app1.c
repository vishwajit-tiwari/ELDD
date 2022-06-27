#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int open_fd, close_fd;
    ssize_t write_fd, read_fd;

    char Wbuff[] = "Hello from user app1";
    char Rbuff[132];

    // Open System Call
    if((open_fd = open("/dev/charDev_device", O_RDWR | O_CREAT, 777)) < 0) {
        perror("Error in opening file\n");
        exit(EXIT_FAILURE);
    }

    // Write System call
    if((write_fd = write(open_fd, Wbuff, sizeof(Wbuff))) < 0) {
        perror("Error in write system call\n");
        exit(EXIT_FAILURE);
    }
    printf("%ld bytes is written successfully\n", write_fd);

    // Read System Call
    if((read_fd = read(open_fd,Rbuff,sizeof(Rbuff))) < 0) {
        perror("Erro in read system call\n");
        exit(EXIT_FAILURE);
    }
    printf("Data from Kernel Driver >>>>%s<<<<\n", Rbuff);
    printf("%ld bytes read successfully\n", read_fd);

    // Close System Call
    if((close_fd = close(open_fd)) < 0) {
        perror("error in close system call\n");
        exit(EXIT_FAILURE);
    }
    printf("Successfully closed the open file\n");

    return 0;
}
