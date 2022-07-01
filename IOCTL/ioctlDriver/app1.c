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
#include<sys/ioctl.h>

// Creating IOCTL Command In User Space Application
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int main(int argc, char const *argv[])
{
    int open_fd;
    char option;
    int32_t value, number;   // to read & write to kernel space

    printf("\n****Opening Driver****\n");

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
            printf("Enter the Value to send: ");
            scanf("%d",&number);
            printf("Writing Value to Driver....");
            ioctl(open_fd, WR_VALUE, (int32_t*) &number); 
            printf("Done!\n\n");
            break;

        case '2':   
            printf("Reading Value from Driver....");
            ioctl(open_fd, RD_VALUE, (int32_t*) &value);
            printf("Done!\n");
            printf("Value is %d\n\n", value);
            break;

        case '3':
            printf("Closing Driver\n");
            close(open_fd);
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
