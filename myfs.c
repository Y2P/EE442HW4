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
//	FileList[0].FileName[0] = '\0';
	memset(&FileList[0].FileName[0], 0, sizeof(FileList[0].FileName));

	FileList[0].firstBlock = 0;
	FileList[0].size = 0;
//	Data[0].Data[0] = '\0';
	memset(&Data[0].Data[0], 0, sizeof(Data[0].Data));

	for (i = 0; i < 4096 ; i++)	
		fwrite(FAT,sizeof(FAT),1,ptr);
	for (i = 0; i < 128 ; i++)	
		fwrite(FileList,sizeof(FileList),1,ptr);
	for (i = 0; i < 4096 ; i++)	
		fwrite(Data,sizeof(Data),1,ptr);

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

	
	int byteCounter; 
	int empty;
	char buffer[512];
	do
	{	
		memset(&buffer[0], 0, sizeof(buffer));
		empty = (fread(buffer,sizeof(buffer),1,srcReader));

		byteCounter = sizeof(buffer);
		// FAT Search is done here

		do
		{
			fread(FAT_e,sizeof(struct FAT_Entry),1,seekAndWritePtr);
		}while(FAT_e[0].entry != 0 );

		fseek(dummyP,ftell(seekAndWritePtr),SEEK_SET);
		ReadEntryNum = ftell(seekAndWritePtr)/sizeof(struct FAT_Entry);

		fseek(seekAndWritePtr,-(sizeof(FAT_e)),SEEK_CUR);
		do
		{
			fread(FAT_e,sizeof(struct FAT_Entry),1,dummyP);
		}while( FAT_e[0].entry != 0 );

		FAT_e[0].entry = ReadEntryNum;
		FAT_e[0].entry  = htonl(FAT_e[0].entry );
		
		ReadEntryNum = ftell(seekAndWritePtr)/sizeof(struct FAT_Entry);
		
		if(writeBlockNum == 0)
		{		
		
			    fseek(fileListptr,4096*sizeof(struct FAT_Entry),SEEK_SET);
				do
				{
					fread(FileList_e,sizeof(FileList_e),1,fileListptr);
				}while((FileList_e[0].size != 0));

			    fseek(fileListptr,-sizeof(FileList_e),SEEK_CUR);
			    strcpy(FileList_e[0].FileName,destFileName); 


			    FileList_e[0].firstBlock = ReadEntryNum;
		}

		fwrite(FAT_e,sizeof(FAT_e),1,seekAndWritePtr);
		fseek(seekAndWritePtr,-sizeof(FAT_e),SEEK_CUR);
		fseek(dataWriter, (ReadEntryNum)*sizeof(struct Data_Entry)+sizeof(struct FAT_Entry)*4096+sizeof(struct File_List_Entry)*128, SEEK_SET);
	

		memset(&Data_e[0].Data[0], 0, sizeof(Data_e[0].Data));
		
		fwrite(buffer,byteCounter*sizeof(char),1,dataWriter);
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

	int i = 0;
	fseek(fileListptr,sizeof(FAT),SEEK_CUR);
	fread(FileList,sizeof(FileList),1,fileListptr);

	for (i = 0; i < 128; ++i)
	{
		if(!strcmp(FileList[i].FileName,srcFileName))
			break;
	}
	if(i == 128)
		return;

	fileWriter = fopen(destPath,"w+");

	int blockidx = FileList[i].firstBlock;
	blockidx = htonl(blockidx);
	fread(FAT,sizeof(FAT),4096,fatReader);
	
	fseek(dataReader,sizeof(FAT)+sizeof(FileList),SEEK_SET);
	fread(Data,sizeof(Data),1,dataReader);

	int count = 0;
	do
	{
		blockidx = ntohl(blockidx);

		WrittenData[count] = Data[blockidx];
		blockidx = FAT[blockidx].entry; 

		count++;
	}	while(blockidx!= 0xFFFFFFFF);

	
	int byteCounter;
	char buffer[512];
	i = 0;
	int cp;
	while(i < count)
	{	
		memset(&buffer[0], 0, sizeof(buffer));
		
		for (cp = 0; cp < sizeof(buffer) ; cp++)
			buffer[cp] = WrittenData[i].Data[cp];
		if (i == count -1)
		{
			for (byteCounter = 0; byteCounter < sizeof(buffer); byteCounter++)
				if(buffer[byteCounter] == 0)
					break;
		}
		else
			byteCounter = 512;

		fwrite(buffer,byteCounter*sizeof(char),1,fileWriter);
		i++;
	}

	fclose(fileWriter);
	fclose(fileListptr);
	fclose(fatReader);
	fclose(dataReader); 
}

void Rename(char* diskName,char* filename,char* newname)
{
	struct FAT_Entry FAT[4096];
	struct File_List_Entry FileList[128];

	int i ; 
	FILE* fileListptr;

	fileListptr = fopen(diskName,"r+");

	fseek(fileListptr,sizeof(FAT),SEEK_CUR);
	fread(FileList,sizeof(FileList),1,fileListptr);

	for (i = 0; i < 128; ++i)
	{
		if(!strcmp(FileList[i].FileName,filename))
			break;
	}
	if(i == 128)
		return;
	strcpy(FileList[i].FileName,newname);
	fseek(fileListptr,sizeof(FAT),SEEK_SET);

	fwrite(FileList,sizeof(FileList),1,fileListptr);

}


void Delete(char* diskName,char *filename)
{

	// Deletes given file from disk image 
	// All the table entries also are deleted

	// Necessary data structures
	struct FAT_Entry FAT[4096];
	struct File_List_Entry FileList[128];
	struct Data_Entry Data[4096];

	// File pointers
	FILE* fileListptr;
	FILE* fatReader;
	FILE* dataReader;
	FILE* fileDeleter;


	// Open file for read and update purposes 
	fileListptr = fopen(diskName,"r+");
	fatReader = fopen(diskName,"r+");
	dataReader = fopen(diskName,"r+");
	fileDeleter = fopen(diskName,"r+");

	// Adjust the location of pointer for correct reading
	int i = 0;
	fseek(fileListptr,sizeof(FAT),SEEK_CUR);
	fread(FileList,sizeof(FileList),1,fileListptr);

	// Find the given file from the file list 
	for (i = 0; i < 128; ++i)
	{
		if(!strcmp(FileList[i].FileName,filename))
			break;
	}
	// If file does not exist,return
	if(i == 128)
		return;

	// 

	int blockidx = FileList[i].firstBlock;
	FileList[i].firstBlock = 0;
	FileList[i].size = 0;
	memset(&FileList[i].FileName[0],0,sizeof(FileList[i].FileName));



	blockidx = htonl(blockidx);
	
	fread(FAT,sizeof(FAT),1,fatReader);
	
	fseek(dataReader,sizeof(FAT)+sizeof(FileList),SEEK_SET);
	fread(Data,sizeof(Data),1,dataReader);

	int deleteidx = 0;
	do
	{

		blockidx = ntohl(blockidx);

		memset(&Data[blockidx].Data[0],0,sizeof(Data[blockidx].Data));
	
		deleteidx = blockidx;
		blockidx = FAT[blockidx].entry; 
		FAT[deleteidx].entry = 0;


	}	while(blockidx!= 0xFFFFFFFF);


	fwrite(FAT,sizeof(FAT),1,fileDeleter);
	fwrite(FileList,sizeof(FileList),1,fileDeleter);
	fwrite(Data,sizeof(Data),1,fileDeleter);
	fclose(fileDeleter);
	fclose(fileListptr);
	fclose(fatReader);
	fclose(dataReader); 
}
void List(char *diskName)
{
	// Lists all the files with nonzero sizes
	FILE* fileListptr;
	fileListptr = fopen(diskName,"r+");
	int i;
	struct File_List_Entry FileList[1];
	fseek(fileListptr,((4096)*(sizeof(struct FAT_Entry))) ,SEEK_SET);
	printf("file name\tfile size\tfirstblock \n");	
	for ( i = 0 ; i < 128 ; i++)
	{
		fread(FileList,sizeof(FileList),1,fileListptr);
		if(FileList[0].size)
			printf("%-10s\t%-10d\t%-10x\n",FileList[0].FileName,FileList[0].size,FileList[0].firstBlock);
	}

	fclose(fileListptr);
}


int main(int argc, char const *argv[])
{

// Function selection is done here
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
		
		printf("%s\n",argv[2] );
		Read(argv[3],argv[4],argv[1]);
	}
	else if(strcmp(argv[2],"-list") == 0)
	{
		printf("%s\n",argv[2] );
		List(argv[1]);
	}
	else if(strcmp(argv[2],"-delete") == 0)
	{
		printf("%s\n",argv[2] );
		Delete(argv[1],argv[3]);
	}
	else if(strcmp(argv[2],"-rename") == 0)
	{
		Rename(argv[1],argv[3],argv[4]);
	}
	return 0;
}
