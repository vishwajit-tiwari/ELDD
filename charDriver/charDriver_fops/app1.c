/**
 * @file app1.c
 * @author Vishwajit Tiwari (tvishwajit@cdac.in)
 * @brief   User Space Applicayion to test Char Device Driver
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>

#define MAX_SIZE 1024

int8_t Wbuff[MAX_SIZE];
int8_t Rbuff[MAX_SIZE];

int main(int argc, char const *argv[])
{
    int open_fd, close_fd;
    ssize_t write_fd, read_fd;
    char option;


    // Open System Call
    if((open_fd = open("/dev/charDev_device", O_RDWR)) < 0) {
        perror("Error in opening Device file\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        printf("Please Choose the Option:\n");
        printf("1. Write\n");
        printf("2. Read\n");
        printf("3. Exit\n");

        scanf(" %c", &option);
        printf("Your Option = %c\n", option);

        switch (option)
        {
        case '1':
            printf("Enter the string to write into driver :");
            scanf("  %[^\t\n]s", Wbuff);
            printf("Data Writing...");
            if((write_fd = write(open_fd, Wbuff, strlen(Wbuff)+1)) < 0) 
            {
                perror("Error in write system call\n");
                exit(EXIT_FAILURE);
            }
            printf("Done!\n\n");
            break;

        case '2':
            printf("Data Reading...");
            if((read_fd = read(open_fd,Rbuff,/*sizeof(Rbuff)*/MAX_SIZE)) < 0) 
            {
                perror("Erro in read system call\n");
                exit(EXIT_FAILURE);
            }
            printf("Done!\n\n");
            printf("Data from Kernel Driver = %s\n\n", Rbuff);
            break;

        case '3':
            if((close_fd = close(open_fd)) < 0) 
            {
                perror("error in close system call\n");
                exit(EXIT_FAILURE);
            }
            printf("Successfully closed the open file\n");
            exit(1);
            break;
        
        default:
            printf("Enter valid option = %c\n",option);
            break;
        }
    }

    close(open_fd);

    return 0;
}
