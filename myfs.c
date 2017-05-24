#include <stdio.h>

#define CHUNK_SIZE 512

char buffer[CHUNK_SIZE];

struct  FAT_Entry
{
 	int entry;
}; 
struct  File_List_Entry
{
	char FileName[128];
	int firstBlock;
	int size;
};
struct  Data_Entry
{
	char Data[512];
};
/*
struct FAT_Entry FAT[4096];
struct File_List_Entry FileList[128];
struct Data_Entry Data[4096];
*/
FILE *fptr;
void Format(const char* srcPath)
{
	int i ;
	FILE *ptr;
	ptr = fopen(srcPath,"w");

	struct FAT_Entry FAT[4096];
	struct File_List_Entry FileList[128];
	struct Data_Entry Data[4096];

	for (i  = 0; i < 4096 ; i++)
	{
		FAT[i].entry = 0;
		if(i%32 == 0)
		{
			strcpy(FileList[i/32].FileName, "");
			FileList[i/32].firstBlock = 0;
			FileList[i/32].size = 0;
		}
		char buffer[512];
		sprintf(buffer,"%d",i);
		strcpy(Data[i].Data, (""));

	}

	fwrite(FAT,sizeof(struct FAT_Entry),4096,ptr);
	fwrite(FileList,sizeof(struct File_List_Entry),128,ptr);
	fwrite(Data,sizeof(struct Data_Entry),4096,ptr);

	fclose(ptr);
}

void Write(char *srcPath,char *destFileName,char* diskName)
{

	struct FAT_Entry FAT_e;
	struct File_List_Entry FileList_e;
	struct Data_Entry Data_e;
	
	int ReadEntryNum = 0;
	FILE *seekAndWritePtr;
	FILE *srcReader;
	srcReader = fopen(srcPath,"r");
	seekAndWritePtr = fopen(diskName,"w");
	char* empty;

	char buffer[512];

	// FAT Search is done here
	do
	{
		ReadEntryNum++;
		fread(empty,sizeof(char),1,seekAndWritePtr);
	}while(*empty == 0 || ReadEntryNum < 4096 );
	fseek(seekAndWritePtr,-4,SEEK_CUR);

	if(	fread(buffer,sizeof(buffer),1,seekAndWritePtr) != 0 )
	{
		FAT_e 
	}

	fclose(srcReader);
	fclose(seekAndWritePtr);
}
void Read(char *srcFileName,char *destPath)
{
	struct FAT_Entry FAT[4096];
	struct File_List_Entry FileList[128];
	struct Data_Entry Data[4096];

/*	fptr = fopen(argv[1],"r+");
	fread(FAT,sizeof(struct FAT_Entry),4096,fptr);
	fread(FileList,sizeof(struct File_List_Entry),128,fptr);
	fread(Data,sizeof(struct Data_Entry),4096,fptr);
*/

}
void Delete(char *filename);
void List();


int main(int argc, char const *argv[])
{


	printf("%s\n",argv[2] );
	if(strcmp(argv[2],"-format") == 0)
	{
		Format(argv[1]);
	}


/*
	FileList[0].FileName[0] = 'h';
	fwrite(FAT,sizeof(struct FAT_Entry),4096,fptr);
	fwrite(FileList,sizeof(struct File_List_Entry),128,fptr);
	fwrite(Data,sizeof(struct Data_Entry),4096,fptr);
*/

//	printf("%c\n",FileList[0].FileName[0] );
	

	//fclose(fptr);
	return 0;
}