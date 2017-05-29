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
	ptr = fopen(srcPath,"r+");

	struct FAT_Entry FAT[1];
	struct File_List_Entry FileList[1];
	struct Data_Entry Data[1];

	FAT[0].entry = 0;
	strcpy(FileList[0].FileName, "");
	FileList[0].firstBlock = 0;
	FileList[0].size = 0;
	strcpy(Data[0].Data, "");


	for (i = 0; i < 4096 ; i++)	
		fwrite(FAT,sizeof(struct FAT_Entry),1,ptr);
	for (i = 0; i < 128 ; i++)	
		fwrite(FileList,sizeof(struct File_List_Entry),1,ptr);
	for (i = 0; i < 4096 ; i++)	
		fwrite(Data,sizeof(struct Data_Entry),1,ptr);

	fclose(ptr);
}

void Write(char *srcPath,char *destFileName,char* diskName)
{

	struct FAT_Entry FAT_e[1];
	struct File_List_Entry FileList_e[1];
	struct Data_Entry Data_e[1];
	

	int ReadEntryNum = 0;

	FILE* seekAndWritePtr;
	FILE* dummyP;
	FILE* srcReader;
	FILE* dataWriter;
	FILE* fileListptr;

	srcReader = fopen(srcPath,"r+");
	seekAndWritePtr = fopen(diskName,"r+");
	dummyP = fopen(diskName,"r+");
	dataWriter = fopen(diskName,"r+");
	fileListptr = fopen(diskName,"r+");

	int writeBlockNum = 0;

	

	int empty;
	char buffer[512];
	do
	{
		empty = (fread(buffer,sizeof(buffer),1,srcReader));
		printf("%s\n",buffer );
		// FAT Search is done here

		do
		{
			fread(FAT_e,sizeof(struct FAT_Entry),1,seekAndWritePtr);
		}while(FAT_e[0].entry != 0 );


		//printf("writeBlockNum: %d\n",writeBlockNum );

	//	printf("seekAndWritePtr:%x\n",ReadEntryNum );

		fseek(dummyP,ftell(seekAndWritePtr),SEEK_SET);
		ReadEntryNum = ftell(seekAndWritePtr)/sizeof(struct FAT_Entry);

		fseek(seekAndWritePtr,-(sizeof(FAT_e)),SEEK_CUR);
		do
		{
			fread(FAT_e,sizeof(struct FAT_Entry),1,dummyP);
		}while( FAT_e[0].entry != 0 );
		//fseek(dummyP,-4,SEEK_CUR);
//		ReadEntryNum = ftell(dummyP)/sizeof(struct FAT_Entry)-1;

		FAT_e[0].entry = ReadEntryNum;
		FAT_e[0].entry  = htonl(FAT_e[0].entry );
		
		ReadEntryNum = ftell(seekAndWritePtr)/sizeof(struct FAT_Entry);
		
		if(writeBlockNum == 0)
		{		
		
			    fseek(fileListptr,4096*sizeof(struct FAT_Entry),SEEK_SET);
			    printf("%d\n",ftell(fileListptr) );
				do
				{
					fread(FileList_e,sizeof(FileList_e),1,fileListptr);
					//printf("%d\n",ftell(fileListptr) );
				}while((FileList_e[0].size != 0));

				printf("%d\n",ftell(fileListptr) );
			    fseek(fileListptr,-sizeof(FileList_e),SEEK_CUR);
			    strcpy(FileList_e[0].FileName,destFileName); 

			    printf("%s\n",FileList_e[0].FileName);

			    FileList_e[0].firstBlock = ReadEntryNum;
		}

		fwrite(FAT_e,sizeof(FAT_e),1,seekAndWritePtr);
		fseek(seekAndWritePtr,-sizeof(FAT_e),SEEK_CUR);
		fseek(dataWriter, (ReadEntryNum)*sizeof(struct Data_Entry)+sizeof(struct FAT_Entry)*4096+sizeof(struct File_List_Entry)*128, SEEK_SET);
	

		//printf("seekAndWritePtr:%ld dummyP:%ld\n",ftell(seekAndWritePtr),ftell(dummyP) );
		strcpy(Data_e[0].Data,buffer);
		fwrite(Data_e,sizeof(Data_e),1,dataWriter);
		//seekAndWritePtr = dummyP;
		writeBlockNum++;		
	}	while (empty == 1);

	FileList_e[0].size = writeBlockNum;
	fwrite(FileList_e,sizeof(FileList_e),1,fileListptr);

	FAT_e[0].entry = 0xFFFFFFFF;
	fwrite(FAT_e,sizeof(FAT_e),1,seekAndWritePtr);
	
	fclose(fileListptr);
	fclose(dataWriter);
	fclose(srcReader);
	fclose(seekAndWritePtr);
	fclose(dummyP);
}
void Read(char *srcFileName,char *destPath,char *diskName)
{
	struct FAT_Entry FAT[4096];
	struct File_List_Entry FileList[128];
	struct Data_Entry Data[4096];
	struct Data_Entry WrittenData[4096];


	FILE* fileListptr;
	FILE* fatReader;
	FILE* dataReader;
	FILE* fileWriter;

	fileListptr = fopen(diskName,"r+");
	fatReader = fopen(diskName,"r+");
	dataReader = fopen(diskName,"r+");
	fileWriter = fopen(destPath,"w");

	int i = 0;
	fseek(fileListptr,sizeof(FAT),SEEK_CUR);
	fread(FileList,sizeof(FileList),1,fileListptr);

	printf("%s\n",destPath );
	for (i = 0; i < 128; ++i)
	{
		if(!strcmp(FileList[i].FileName,srcFileName))
			break;
	}
	if(i == 128)
		return;
	int blockidx = FileList[i].firstBlock;
	fread(FAT,sizeof(FAT),4096,fatReader);
	
	fseek(dataReader,sizeof(FAT)+sizeof(FileList),SEEK_SET);
	fread(Data,sizeof(Data),1,dataReader);

	int count = 0;
	do
	{
		printf(":%x\n",blockidx );
		blockidx = ntohl(blockidx);
		printf("::%x\n",blockidx );

		WrittenData[count] = Data[blockidx];
		blockidx = FAT[blockidx].entry; 
		printf(":::%x\n",blockidx );

		count++;
	}	while(blockidx!= 0xFFFFFFFF);

	

	char buffer[512];
	i = 0;
	printf("%d\n",count );
	while(i < count)
	{	
		printf("%d\n",i);
		strcpy(buffer,WrittenData[i].Data);
		printf("%s\n",buffer );
		fwrite(buffer,sizeof(buffer),1,fileWriter);
		i++;
	}
	
	fclose(fileWriter);
	fclose(fileListptr);
	fclose(fatReader);
	fclose(dataReader);
}




void Delete(char *filename);
void List(char *diskName)
{
	FILE* fileListptr;
	fileListptr = fopen(diskName,"r+");

	struct File_List_Entry FileList[1];
	fseek(fileListptr,((4096)*(sizeof(struct FAT_Entry))) ,SEEK_SET);
	printf("file name\tfile size\tfirstblock \n");	
	do
	{
		fread(FileList,sizeof(FileList),1,fileListptr);
		if(FileList[0].size)
			printf("%-10s\t%-10d\t%-10x\n",FileList[0].FileName,FileList[0].size,FileList[0].firstBlock);
	}while((FileList[0].size != 0));

	fclose(fileListptr);
}


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
		Read(argv[3],argv[4],argv[1]);
	}
	else if(strcmp(argv[2],"-list") == 0)
	{
		List(argv[1]);
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
