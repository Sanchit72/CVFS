#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<io.h>

//user define micro
#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 2048

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
}superblock,*Psuperblock;

typedef struct inode
{
    char FileName[50];
    int inodeNumber;
    int Filesize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int permission;
    struct inode *next;
} inode,*PINODE,**PPINODE;

typedef struct FileTable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode ; //1 2 3
    PINODE ptrinode;
}FileTable,*PFileTable;

typedef struct ufdt
{
    PFileTable ptrFileTable;
}ufdt;
ufdt ufdtArr[50];

superblock superblockobj;
PINODE head = NULL;

void man(char *name)
{
    if(name == NULL) return;

     if(strcmp(name,"create") == 0)
     {
        printf("Description : Used to create new regular file \n");
        printf("Uages : create File_Name\n");
     }
     else if(strcmp(name,"read") == 0)
     {
        printf("Description : Used to read data from regular File \n");
        printf("Uages : read File_nmae No_Of_Bytes_To_Read \n");
     }
     else if(strcmp(name,"write") == 0)
     {
        printf("Description :Used to write into regular file \n");
        printf("Uages : write File_name\n After this enter the data that we want to write \n");
     }
     else if(strcmp(name,"Is") == 0)
     {
        printf("Description : Used to list all information of files \n");
        printf("Uages : Is \n");
     }
     else if(strcmp(name,"start") == 0)
     {
        printf("Description :Used to Dispaly information of File \n");
        printf("Uages :start File_name \n");
     }
     else if(strcmp(name,"Fstart") == 0)
     {
        printf("Description : Used to display informatin of file \n");
        printf("Uages : start file_Description\n");
     }
     else if(strcmp(name,"truncate") == 0)
     {
        printf("Description : Used to Remove data from file \n");
        printf("Uages : Truncate File_name\n");
     }
     else if(strcmp(name,"open")== 0)
     {
        printf("Description :Used to open existing file \n");
        printf(" Uages : open file_name mode \n");
     }
     else if(strcmp(name,"close") == 0)
     {
        printf("Description : Used to close opened file \n");
        printf("Uages : close File_name \n");
     }
     else if(strcmp(name,"closeall") == 0)
     {
        printf("Description : Used to close all opened fiel \n");
        printf("Uages : closeall\n");
     }
     else if(strcmp(name,"Iseek") == 0)
     {
        printf("Description : Used to change file offset \n");
        printf("Uages : Iseek File_Name ChangeInOffset StartPoint \n");
     }
     else if(strcmp(name,"rm") == 0)
     {
        printf("Description : Used to delete th efile\n");
        printf("Uages : rm File_Nmae\n");
     }
     else
     {
        printf("ERROR : No manual entry available.\n");
     }
}

void DisplayHelp()
{
    printf("create : To create a new file\n");
    printf("Is : To List Out all Files \n");
    printf("Clear : To clear console\n");
    printf("Open : To open the file\n");
    printf("Close : To close  file\n");
    printf("Closeall : To close all opened file\n ");
    printf("Read : To read the content from file \n");
    printf("Write : To write content into file \n");
    printf("Exit : To Terminate File System\n");
    printf("Stat : To Dispaly information of file using name \n");
    printf("Fstat : To Dispaly information of file using description \n");
    printf("Truncate : To remove all data from file\n");
    printf("rm : To Delet the file \n");
}

int GetFDFromName(char *name)
{
    int i = 0;

    while(i<50)
    {
        if(ufdtArr[i].ptrFileTable != NULL)
           if(strcmp((ufdtArr[i].ptrFileTable->ptrinode->FileName),name) == 0)
           break;
           i++;
    }

    if(i == 50)  return -1;
    else        return i;
}

PINODE Get_Inode(char * name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL)
        return NULL;

        while (temp != NULL)
        {
            if(strcmp(name,temp->FileName) == 0)
                break;
            temp = temp->next;
        }
        return temp;
}

void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while (i <= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(inode));

        newn->LinkCount = 0;
        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->Filesize = 0;

        newn->Buffer = NULL;
        newn->next = NULL;

        newn->inodeNumber = i;

        if(temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    printf("DILB created successfully \n");
}

void InitialiseSuperBlock()
{
    int i = 0;
    while (i<MAXINODE)
    {
        ufdtArr[i].ptrFileTable = NULL;
        i++;
    }

    superblockobj.TotalInodes = MAXINODE;
    superblockobj.FreeInode = MAXINODE;  
}

int CreateFile(char *name,int permission)
{
    int i =0;
    PINODE temp = head;

    if((name == NULL) || (permission == 0) || (permission > 3))
    return -1;

    if(superblockobj.FreeInode == 0)
    return -2;

    (superblockobj.FreeInode)--;

    if(Get_Inode(name)!=NULL)
        return -3;

        while (temp != NULL)
        {
            if(temp->FileType == 0)
                break;
            temp = temp->next;
        }

        while (i<50)
        {
            if(ufdtArr[i].ptrFileTable == NULL)
                break;
            i++;
        }

        ufdtArr[i].ptrFileTable = (PFileTable)malloc(sizeof(FileTable));

        ufdtArr[i].ptrFileTable->count = 1;
        ufdtArr[i].ptrFileTable->mode = permission;
        ufdtArr[i].ptrFileTable->readoffset = 0;
        ufdtArr[i].ptrFileTable->writeoffset = 0;

        ufdtArr->ptrFileTable->ptrinode = temp;

        strcpy(ufdtArr[i].ptrFileTable->ptrinode->FileName,name);
        ufdtArr[i].ptrFileTable->ptrinode->FileType = REGULAR;
        ufdtArr[i].ptrFileTable->ptrinode->ReferenceCount = 1;
        ufdtArr[i].ptrFileTable->ptrinode->LinkCount = 1;
        ufdtArr[i].ptrFileTable->ptrinode->Filesize = MAXFILESIZE;
        ufdtArr[i].ptrFileTable->ptrinode->FileActualSize = 0;
        ufdtArr[i].ptrFileTable->ptrinode->permission = permission;
        ufdtArr[i].ptrFileTable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);

        return i;       
}

// rm_file("Demo.txt")
int rm_file(char * name)
{
    int fd = 0;

    fd = GetFDFromName(name);
    if(fd == -1)
        return -1;

        (ufdtArr[fd].ptrFileTable->ptrinode->LinkCount)--;

        if(ufdtArr[fd].ptrFileTable->ptrinode->LinkCount == 0)
        {
            ufdtArr[fd].ptrFileTable->ptrinode->FileType = 0;
            //free(ufdtArr[fd].ptrfiletable->ptrinode->Buffer);
            free(ufdtArr[fd].ptrFileTable);
        }

        ufdtArr[fd].ptrFileTable = NULL;
        (superblockobj.FreeInode)++;
}

int ReadFile(int fd,char *arr,int isize)
{
    int read_size = 0;

    if(ufdtArr[fd].ptrFileTable == NULL)   return -1;

    if(ufdtArr[fd].ptrFileTable->mode != READ && ufdtArr[fd].ptrFileTable->mode != READ + WRITE) return -2;

if(ufdtArr[fd].ptrFileTable->ptrinode->permission != READ && ufdtArr[fd].ptrFileTable->ptrinode->permission != READ+WRITE) return -2;

if(ufdtArr[fd].ptrFileTable->readoffset == ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) return -3;

if(ufdtArr[fd].ptrFileTable->ptrinode->FileType != REGULAR) return -4;

read_size = (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) - (ufdtArr[fd].ptrFileTable->readoffset);
if(read_size < isize)
{
    strncpy(arr,(ufdtArr[fd].ptrFileTable->ptrinode->Buffer) + (ufdtArr[fd].ptrFileTable->readoffset),read_size);

    ufdtArr[fd].ptrFileTable->readoffset = ufdtArr[fd].ptrFileTable->readoffset + read_size;
}
else
{
    strncpy(arr,(ufdtArr[fd].ptrFileTable->ptrinode->Buffer) +(ufdtArr[fd].ptrFileTable->readoffset),isize);

    (ufdtArr[fd].ptrFileTable->readoffset) = (ufdtArr[fd].ptrFileTable->readoffset) + isize;
}

return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
    if(((ufdtArr[fd].ptrFileTable->mode) != WRITE) && ((ufdtArr[fd].ptrFileTable->mode) != READ + WRITE)) return -1;

    if(((ufdtArr[fd].ptrFileTable->ptrinode->permission) != WRITE) && ((ufdtArr[fd].ptrFileTable->ptrinode->permission)!=READ+WRITE)) return -1;

    if((ufdtArr[fd].ptrFileTable->writeoffset) == MAXFILESIZE) return -2;

    if((ufdtArr[fd].ptrFileTable->ptrinode->FileType)!= REGULAR) return -3;

    strncpy((ufdtArr[fd].ptrFileTable->ptrinode->Buffer)+(ufdtArr[fd].ptrFileTable->writeoffset),arr,isize);

    (ufdtArr[fd].ptrFileTable->writeoffset) = (ufdtArr[fd].ptrFileTable->writeoffset) + isize;

    (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) = (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) + isize;

    return isize;
}

 int OpenFile(char *name,int mode)
{
    int i =0;i ;
    PINODE temp = NULL;

    if(name == NULL || mode <= 0)
    return -1;

    temp = Get_Inode(name);
    if(temp == NULL)
     return -2;

     if(temp->permission < mode)
        return -3;
    
    while (i < 50)
    {
        if(ufdtArr[i].ptrFileTable == NULL)
        break;
        i++;
    }
    
    ufdtArr[i].ptrFileTable = (PFileTable)malloc(sizeof(FileTable));
    if(ufdtArr[i].ptrFileTable == NULL) return -1;
    ufdtArr[i].ptrFileTable->count = 1;
    ufdtArr[i].ptrFileTable->mode = mode;
    if(mode == READ + WRITE)
    {
        ufdtArr[i].ptrFileTable->readoffset = 0;
        ufdtArr[i].ptrFileTable->writeoffset = 0;
    }
    else if(mode == READ)
    {
        ufdtArr[i].ptrFileTable->readoffset = 0;
    }
    else if(mode == WRITE)
    {
        ufdtArr[i].ptrFileTable->writeoffset = 0;
    }
    ufdtArr[i].ptrFileTable->ptrinode = temp;
    (ufdtArr[i].ptrFileTable->ptrinode->ReferenceCount)++;

    return i;
}

int CloseFileByName(int fd)
{
    ufdtArr[fd].ptrFileTable->readoffset = 0;
    ufdtArr[fd].ptrFileTable->writeoffset = 0;
    (ufdtArr[fd].ptrFileTable->ptrinode->ReferenceCount)--;
}

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if(i==-1)
    return -1;

    ufdtArr[i].ptrFileTable->readoffset = 0;
    ufdtArr[i].ptrFileTable->writeoffset = 0;
    (ufdtArr[i].ptrFileTable->ptrinode->ReferenceCount)--;

    return 0;
}

void CloseAllFile()
{
    int i =0 ;
    while ( i <50)
    {
        if(ufdtArr[i].ptrFileTable != NULL)
        {
            ufdtArr[i].ptrFileTable->readoffset = 0;
            ufdtArr[i].ptrFileTable->writeoffset = 0;
            (ufdtArr[i].ptrFileTable->ptrinode->ReferenceCount)--;
            break;
        }
        i++;
    }    
}

int LseekFile(int fd,int size,int from)
{
    if((fd < 0) || (from > 2)) return -1;
    if(ufdtArr[fd].ptrFileTable == NULL) return -1;

    if((ufdtArr[fd].ptrFileTable->mode == READ) || (ufdtArr[fd].ptrFileTable->mode == READ + WRITE))
    {
        if(from == CURRENT)
        {
            if(((ufdtArr[fd].ptrFileTable->readoffset) + size) > ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) return -1;
            if(((ufdtArr[fd].ptrFileTable->readoffset) + size) < 0) return -1;
            (ufdtArr[fd].ptrFileTable->readoffset) = (ufdtArr[fd].ptrFileTable->readoffset) + size;
        }
        else if(from == START)
        {
            if(size >(ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize)) return -1;
            if(size < 0) return -1;
            (ufdtArr[fd].ptrFileTable->readoffset) = size;
        }
        else if(from == END)
        {
            if((ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) + size >MAXFILESIZE) return -1;
            if(((ufdtArr[fd].ptrFileTable->readoffset) + size) < 0) return -1;
            (ufdtArr[fd].ptrFileTable->readoffset) = (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) + size;
        }
    }
    else if(ufdtArr[fd].ptrFileTable->mode == WRITE)
    {
        if(from == CURRENT)
        {
            if(((ufdtArr[fd].ptrFileTable->writeoffset)+size)>MAXFILESIZE) return -1;
            if(((ufdtArr[fd].ptrFileTable->writeoffset)+size)<0) return -1;
            if(((ufdtArr[fd].ptrFileTable->writeoffset)+size)>(ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize))
            (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) = (ufdtArr[fd].ptrFileTable->writeoffset) + size;
            (ufdtArr[fd].ptrFileTable->writeoffset) = (ufdtArr[fd].ptrFileTable->writeoffset) + size;
        }
        else if(from == START)
        {
            if(size > MAXFILESIZE) return -1;
            if(size < 0) return -1;
            if(size > (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize))
            (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize) = size;
            (ufdtArr[fd].ptrFileTable->writeoffset) = size;
        }
        else if(from == END)
        {
            if((ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize)+size > MAXFILESIZE) return -1;
            if(((ufdtArr[fd].ptrFileTable->writeoffset)+size)<0) return -1;
            (ufdtArr[fd].ptrFileTable->writeoffset) = (ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize)+ size;
        }
    }
}

void Is_file()
{
    int i = 0;
    PINODE temp = head;

    if(superblockobj.FreeInode == MAXINODE)
    {
        printf("Error : There are no file\n");
        return;
    }

    printf("\n File Name\tInode number\tFile size\tLink count\n");
    printf("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
    while (temp != NULL)
    {
        if(temp->FileType!=0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->inodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ ");
}

int Fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;

    if(fd < 0) return -1;

    if(ufdtArr[fd].ptrFileTable == NULL) return -2;

    temp = ufdtArr[fd].ptrFileTable->ptrinode;

    printf("\n_ _ _ _ _ _ _Statistical Information about file_ _ _ _ _ _ _\n");
    printf("File name : %s\n",temp->FileName);
    printf("Inode Number :%d\n",temp->inodeNumber);
    printf("File size :%d\n",temp->Filesize);
    printf("Actual File size : %d\n",temp->FileActualSize);
    printf("Link count : %d\n",temp->LinkCount);
    printf("Reference count :%d\n",temp->ReferenceCount);

    if(temp->permission == 1)
        printf("File Permission : Read only \n");
    else if(temp->permission == 2)
         printf("File Permission : Write\n");
    else if(temp->permission == 3)
        printf("File Permission : Read & Write\n");
    printf("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ ");

    return 0;
}

int Stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    if(name == NULL) return -1;

    while (temp != NULL)
    {
        if(strcmp(name,temp->FileName)==0)
         break;
        temp = temp->next;
    }
    
    if(temp == NULL) return -2;

    printf("\n_ _ _ _ _ _ _ _ _Statistical Information about file_ _ _ _ _ _ _ _ _\n");
    printf("File name : %s\n",temp->FileName);
    printf("Inode Number %d\n",temp->inodeNumber);
    printf("File size : %d\n",temp->Filesize);
    printf("Actual file size : %d\n",temp->FileActualSize);
    printf("Link count : %d\n",temp->LinkCount);
    printf("Reference count : %d\n",temp->ReferenceCount);

    if(temp->permission == 1)
        printf("File Permission : Read only\n");
    else if(temp->permission == 2)
        printf("File permission : write\n");
    else if(temp->permission == 3)
        printf("File Permission : Read & Write\n");
    printf("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _ _ _ _ \n\n");

    return 0;
}

int truncate_file(char *name)
{
    int fd = GetFDFromName(name);
    if(fd == -1)
        return -1;

memset(ufdtArr[fd].ptrFileTable->ptrinode->Buffer,0,1024);
ufdtArr[fd].ptrFileTable->readoffset = 0;
ufdtArr[fd].ptrFileTable->writeoffset = 0;
ufdtArr[fd].ptrFileTable->ptrinode->FileActualSize = 0;
}

int main()
{
    char *ptr = NULL;
    int ret = 0,fd = 0,count = 0;
    char command[4][80],str[80],arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while (1)
    {
        fflush(stdin);
        strcpy(str,"");

        printf("\n Marvellous VFS : >");

    fgets(str,80,stdin);  //scanf("%[^\n]s",str);

count = sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);

    if(count == 1)
    {
        if(strcmp(command[0],"Is") == 0)
        {
            Is_file();
        }
        else if(strcmp(command[0],"Closeal") == 0)
        {
            CloseAllFile();
            printf("All File Close Successfully\n");
            continue;
        }
        else if(strcmp(command[0],"clear") == 0)
        {
            system("cls");
            continue;
        }
        else if(strcmp(command[0],"help") == 0)
        {
           DisplayHelp();
           continue;
        }
        else if(strcmp(command[0],"exit") == 0)
        {
            printf("Terminating the Marvellous Virtual File System\n");
            break;
        }
        else
        {
            printf("\nERROR : Command not found !!!\n");
            continue;
        }
    }
    else if(count == 2)
    {
        if(strcmp(command[0],"stat") == 0)
        {
            ret = Stat_file(command[1]);
            if(ret == -1)
            printf("ERROR : Incorrect Parameters\n");
            if(ret == -2)
            printf("ERROR : There is no such file\n");
            continue;
        }
        else if(strcmp(command[0],"fstat") == 0)
        {
            ret = Fstat_file(atoi(command[1]));
            if(ret == -1)
            printf("ERROR : Incorrect parameters\n");
            if(ret == -2)
            printf("ERROR : There is no such file\n");
            continue;
        }
        else if(strcmp(command[0],"close") == 0)
        {
            ret = CloseFileByName(command[1]);
            if(ret == -1)
            printf("ERROR : There is no such file\n");
            continue;
        }
        else if(strcmp(command[0],"rm") == 0)
        {
            ret = rm_file(command[1]);
            if(ret == -1)
            printf("ERROR : There is no such file\n");
            continue;
        }
        else if(strcmp(command[0],"man") == 0)
        {
            man(command[1]);
        }
        else if(strcmp(command[0],"write") == 0)
        {
            fd = GetFDFromName(command[1]);
            if(fd == -1)
            {
                printf("Error : Incorrect parameters\n");
                continue;
            }
            printf("Enter the data :\n");
            scanf("%[^\n]",arr);

            ret = strlen(arr);
            if(ret == 0)
            {
                printf("Error : Incorrect parameters \n");
                continue;
            }
            ret = WriteFile(fd,arr,ret);
            if(ret == -1)
            printf("ERROR : Permission denied\n");
            if(ret == -2)
            printf("ERROR : There is no sufficient memory to write\n");
            if(ret == -3)
            printf("ERROR : It is not regular file\n");
        }
        else if(strcmp(command[0],"truncate") == 0)
        {
            ret = truncate_file(command[1]);
            if(ret == -1)
            printf("Error : Incorrect parameters\n");
        }
        else
        {
            printf("\nERROR : Command not found !!!\n");
            continue;
        }
    }
    else if(count == 3)
    {
        if(strcmp(command[0],"create") == 0)
        {
            ret = CreateFile(command[1],atoi(command[2]));
            if(ret>=0)
                printf("File is successfuly create with file descriptor :%d\n",ret);
            if(ret == -1)
                printf("ERROR : Incorrect Parameters\n");
            if(ret == -2)
                printf("ERROR : There is no inodes\n");
            if(ret == -3)
                printf("ERROR : File already exists\n");
            if(ret == -4)
                printf("ERROR : Memory allocation failure\n");
            continue;
        }
        else if(strcmp(command[0],"open") == 0)
        {
            ret = OpenFile(command[1],atoi(command[2]));
            if(ret >= 0)
                printf("File Successfuly opened with file descriptor : %d\n",ret);
            if(ret == -1)
                printf("ERROR : Incorrect parameters\n");
            if(ret == -2)
                printf("ERROR : File not present\n");
            if(ret == -3)
                printf("ERROR : Permission denied\n");
            continue;
        }
        else if(strcmp(command[0],"read") == 0)
        {
            fd = GetFDFromName(command[1]);
            if(fd == -1)
            {
                printf("Error : Incorrect parameters\n");
                continue;
            }
            ptr = (char *)malloc(sizeof(atoi(command[2]))+1);
            if(ptr == NULL)
            {
                printf("ERROR : Memory allocation failure\n");
                continue;
            }
            ret = ReadFile(fd,ptr,atoi(command[2]));
            if(ret == -1)
                printf("ERROR : File not existing\n");
            if(ret == -2)
                printf("ERROR : Permission denied\n");
            if(ret == -3)
                printf("ERROR : Reached at end of file\n");
            if(ret == -4)
                printf("ERROR : It is not regular file\n");
            if(ret == 0)
                printf("ERROR :File Empty\n");
            if(ret > 0)
            {
                write(2,ptr,ret);
            }
            continue;
        }
        else
        {
            printf("\nERROR : Command not found !!!\n");
            continue;
        }
    }
    else if(count == 4)
    {
        if(strcmp(command[0],"Iseek") == 0)
        {
            fd = GetFDFromName(command[1]);
            if(fd == -1)
            {
                printf("ERROR : Incorrect Parameters\n");
                continue;
            }
            ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
            if(ret == -1)
            {
                printf("ERROR : Unable to perform Iseek\n");
            }
        }
        else
        {
            printf("\nERROR : Command not found !!!\n");
            continue;
        }
    }
        else
        {
            printf("\nERROR : Command not found !!!\n");
            continue;
        }
    }
    return 0;
}