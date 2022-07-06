/**
 * @file app1.c
 * @author Vishwajit Tiwari (tvishwajit@cdac.in)
 * @brief  An user appllication for charDriver
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define mem_size 1024
int8_t wrBuff[mem_size];
int8_t rdBuff[mem_size];

int main(int argc, char const *argv[])
{
    int fd_open, fd_close;
    ssize_t fd_wr, fd_rd;
    char option;

    fd_open = open("/dev/charDev_deviceWQ", O_RDWR);
    
    if(fd_open < 0) {
        perror("Error in : open !!!!\n");
        exit(EXIT_FAILURE);
    }

    while(1) 
    {
        printf("\nPlease choose the option:\n");
        printf("1. write\n");
        printf("2. read\n");
        printf("3. exit\n");

        printf("Your choice: ");
        scanf(" %c", &option);

        switch (option)
        {
        case '1':
            printf("Enter the string to write into driver: ");
            scanf(" %[^\t\n]s", wrBuff);
            printf("Data writing....");
            fd_wr = write(fd_open,wrBuff,strlen(wrBuff)+1);
            if(fd_wr < 0) {
                perror("Error in : Write!!!\n");
                exit(EXIT_FAILURE);
            }
            printf("Done!\n\n");
            break;

        case '2':
            printf("Data reading....");
            fd_rd = read(fd_open,rdBuff,mem_size);
            if(fd_rd < 0) {
                perror("Error in : Reading!!!\n");
                exit(EXIT_FAILURE);
            }
            printf("Done!\n\n");
            printf("Data from : Driver = %s\n\n", rdBuff);
            break;

        case '3':
            fd_close = close(fd_open);
            if(fd_close < 0) {
                perror("Error in : close!!!\n");
                exit(EXIT_FAILURE);
            }
            printf("Successfully closed : open file\n");
            exit(1);
            break;
        
        default:
            break;
        }
    }

    close(fd_open);
    
    return 0;
}
