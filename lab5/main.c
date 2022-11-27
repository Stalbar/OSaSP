#include <stdio.h>
#include <windows.h>
#include <string.h>

#define WORD_SIZE 128
#define BUFFER_SIZE 1024 * 1024
#define THREAD_COUNT 3

HANDLE hMutex;
HANDLE* hThreads;

int CountWords(char*);
char** BreakBufferInWords(char*);
char*** BreakWordsInChunks(char**, char*);

int main()
{
	HANDLE hFile = CreateFile("text.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	char buffer[BUFFER_SIZE];
	DWORD dwBytesRead = 0;
	ReadFile(hFile, buffer, BUFFER_SIZE - 1, &dwBytesRead, NULL);
	buffer[dwBytesRead] = '\0';
	char** words = BreakBufferInWords(buffer);
	char*** chunks = BreakWordsInChunks(words, buffer);
	int wordCountPerChunk = CountWords(buffer) / THREAD_COUNT;
	int lastChunksSize = CountWords(buffer) - (THREAD_COUNT - 1) * wordCountPerChunk;
	for (int i = 0; i < THREAD_COUNT - 1; i++)
	{
		printf("Chunk %d:\n", i + 1);
		for (int j = 0; j < wordCountPerChunk; j++)
			printf("%s\n", chunks[i][j]);
		printf("\n");
	}
	printf("Chunk %d\n", THREAD_COUNT);
	for (int j = 0; j < lastChunksSize; j++)
		printf("%s\n", chunks[THREAD_COUNT - 1][j]);
	printf("\n");
	printf("Success");
	return 0;
}

int CountWords(char* buffer)
{
	int count = 0;
	char* tmp = buffer;
	while ((*tmp))
	{
		if ((*tmp) == '\n')
			count++;
		tmp++;
	}
	return count;
}

char** BreakBufferInWords(char* buffer)
{
	char** result = (char**) malloc (sizeof(char*) * CountWords(buffer));
	for (int i = 0; i < CountWords(buffer); i++)
		result[i] = (char*) malloc (sizeof(char) * WORD_SIZE);
	char* tmp = buffer;
	char* wordBeginning = buffer;
	int count = 0;
	while ((*tmp))
	{
		if ((*tmp) == '\n')
		{
			memcpy(result[count], wordBeginning, tmp - wordBeginning);
			result[count][tmp - wordBeginning] = '\0';
			tmp++;
			wordBeginning = tmp;
			count++;
		}
		tmp++;
	}
	return result;
}

char*** BreakWordsInChunks(char** words, char* buffer)
{
	char*** result = (char***) malloc (sizeof(char**) * THREAD_COUNT);
	int count = 0;
	int totalWordCount = CountWords(buffer);
	int wordCountPerChunk = totalWordCount / THREAD_COUNT;
	for (int i = 0; i < THREAD_COUNT - 1; i++)
	{
		result[i] = (char**) malloc(sizeof(char*) * wordCountPerChunk);
		for (int j = 0; j < wordCountPerChunk; j++)
		{
			result[i][j] = (char*) malloc (sizeof(char) * WORD_SIZE);
			memcpy(result[i][j], words[count], strlen(words[count]));
			result[i][j][strlen(words[count])] = '\0';
			count++;
		}
	}
	int lastChunksSize = totalWordCount - (THREAD_COUNT - 1) * wordCountPerChunk;
	result[THREAD_COUNT - 1] = (char**) malloc(sizeof(char*) * lastChunksSize);
	for (int j = 0; j < lastChunksSize; j++)
	{
		result[THREAD_COUNT - 1][j] = (char*) malloc (sizeof(char) * WORD_SIZE);
		memcpy(result[THREAD_COUNT - 1][j], words[count], strlen(words[count]));
		result[THREAD_COUNT - 1][j][strlen(words[count])] = '\0';
		count++;
	}
	return result;
}