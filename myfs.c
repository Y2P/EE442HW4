#include <stdio.h>
#include <arpa/inet.h>

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

	struct FAT_Entry FAT[1];
	struct File_List_Entry FileList[1];
	struct Data_Entry Data[1];

	FAT[0].entry = 0;
	strcpy(FileList[0].FileName, "");
	FileList[0].firstBlock = 0;
	FileList[0].size = 0;
	strcpy(Data[0].Data, (""));

	
	for (i = 0; i < 4096; i++)
		fwrite(FAT,sizeof(struct FAT_Entry),1,ptr);
	for (i = 0; i < 128; i++)
		fwrite(FileList,sizeof(struct File_List_Entry),1,ptr);
	for (i = 0; i < 4096; i++)
		fwrite(Data,sizeof(struct Data_Entry),1,ptr);

	fclose(ptr);
}

void Write(char *srcPath,char *destFileName,const char* diskName)
{

	struct FAT_Entry FAT_e[1];
	struct File_List_Entry FileList_e;
	struct Data_Entry Data_e[1];
	

	int ReadEntryNum = 0;
	FILE *seekAndWritePtr;
	FILE *dummyP;
	FILE *srcReader;
	FILE *dataWriter;

	FILE* fptr;
	fptr = fopen(diskName,"r+");

	srcReader = fopen(srcPath,"r");
	seekAndWritePtr = fopen(diskName,"r+");
	dummyP = fopen(diskName,"r+");
	dataWriter = fopen(diskName,"w");
	int writeBlockNum = 0;

	printf("%s\n",diskName );
	printf("HEY" );

	int empty;
	char buffer[512];
	while ((fread(buffer,sizeof(buffer),1,srcReader)) == 1)
	{
		// FAT Search is done here
		printf("HEY" );

		do
		{

			fread(FAT_e,sizeof(struct FAT_Entry),1,seekAndWritePtr);
			//printf("%d\n",FAT_e[0].entry );
			
		}while(FAT_e[0].entry != 0 );

		ReadEntryNum = ftell(seekAndWritePtr);

		//printf("writeBlockNum: %d\n",writeBlockNum );

	//	printf("seekAndWritePtr:%x\n",ReadEntryNum );
		
		fseek(dummyP,ftell(seekAndWritePtr),SEEK_SET);
		fseek(seekAndWritePtr,-(sizeof(FAT_e)),SEEK_CUR);
		do
		{
			fread(FAT_e,sizeof(struct FAT_Entry),1,dummyP);
		}while( FAT_e[0].entry != 0 );
		//fseek(dummyP,-4,SEEK_CUR);

		FAT_e[0].entry = ReadEntryNum;
		//FAT_e[0].entry  = htonl(FAT_e[0].entry );
		//fwrite(FAT_e,sizeof(FAT_e),1,seekAndWritePtr);
		fseek(dataWriter, (writeBlockNum)*sizeof(struct Data_Entry)+ sizeof(struct FAT_Entry)*4096 + sizeof(struct File_List_Entry)*128, SEEK_SET);
	
		printf("seekAndWritePtr:%d dummyP:%d\n",ftell(seekAndWritePtr),ftell(dummyP) );
		strcmp(Data_e[0].Data,buffer);
		fwrite(Data_e,sizeof(Data_e),1,dataWriter);
		//seekAndWritePtr = dummyP;
		writeBlockNum++;		

	}
	FAT_e[0].entry = 0xFFFFFFFF;
	//fwrite(FAT_e,sizeof(FAT_e),1,dummyP);

	fclose(srcReader);
	fclose(seekAndWritePtr);
}
void Read(char *srcFileName,char *destPath,char *diskName)
{
	struct FAT_Entry FAT[1];
	struct File_List_Entry FileList[1];
	struct Data_Entry Data[1];
	FILE* fptr;
	fptr = fopen(diskName,"r+");

	int i;
	int PrintList = 100;
	for (i = 0; i < PrintList; ++i)
	{
		fread(FAT,sizeof(struct FAT_Entry),1,fptr);
		printf("Fat Entry %d : %d \n",i ,FAT[0].entry );
	}
	fseek(fptr,(128*sizeof(struct File_List_Entry)+(4096-PrintList)*(sizeof(struct FAT_Entry))) ,SEEK_CUR);
		
		//fread(FileList,sizeof(struct File_List_Entry),1,fptr);
	for (i = 0; i < PrintList; ++i)	
	{	
		fread(Data,sizeof(struct Data_Entry),1,fptr);
		printf("Data Entry %d : %s \n",i ,Data[0].Data);
	}
}


/*
	FileList[0].FileName[0] = 'h';
	fwrite(FAT,sizeof(struct FAT_Entry),4096,fptr);
	fwrite(FileList,sizeof(struct File_List_Entry),128,fptr);
	fwrite(Data,sizeof(struct Data_Entry),4096,fptr);
*/



void Delete(char *filename);
void List();


int main(int argc, char const *argv[])
{


	if(strcmp(argv[2],"-format") == 0)
	{
		Format(argv[1]);
	}
	else if(strcmp(argv[2],"-write") == 0)
	{
		printf("%s\n",argv[2] );
		Write(argv[3],argv[4],argv[1]);
	}
	else if(strcmp(argv[2],"-read") == 0)
	{
		Read(NULL,NULL,argv[1]);
	}

/*
	struct FAT_Entry FAT[4096];
	struct File_List_Entry FileList[128];
	struct Data_Entry Data[4096];
	FILE* fptr;
	fptr = fopen(argv[1],"r+");
	fread(FAT,sizeof(struct FAT_Entry),4096,fptr);
	fread(FileList,sizeof(struct File_List_Entry),128,fptr);
	fread(Data,sizeof(struct Data_Entry),4096,fptr);
*/


/*
	FileList[0].FileName[0] = 'h';
	fwrite(FAT,sizeof(struct FAT_Entry),4096,fptr);
	fwrite(FileList,sizeof(struct File_List_Entry),128,fptr);
	fwrite(Data,sizeof(struct Data_Entry),4096,fptr);
*/

	//printf("%d\n",FAT[4096].entry );
	

	//fclose(fptr);
	return 0;
}