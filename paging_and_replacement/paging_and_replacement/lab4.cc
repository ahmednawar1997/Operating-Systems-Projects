#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inputArray[10000];
int sizeOfBuffer;
char type[5];
int sizeOfArray;


struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

int contains(struct Queue* queue, int input)
{
    int i;

    if(isEmpty(queue))
    {
        return 0;
    }
    for(i = 0; i <= queue->size; i++)
    {
        if(queue->array[i] == input)
        {
            return 1;
        }
    }

    return 0;
}

void printQueue(struct Queue* queue, int input, int pageFault)
{
    if(pageFault)
        printf("%02d F   ",input);
    else
        printf("%02d     ",input);

    int i = 0;
    for(i = 0; i < queue->size; i++)
    {
        printf("%02d ",queue->array[i]);
    }
    printf("\n");
}



void printBuffer(int buffer[], int input, int pageFault)
{
    if(pageFault)
        printf("%02d F   ",input);
    else
        printf("%02d     ",input);

    int i = 0;
    for(i = 0; i < sizeOfBuffer; i++)
    {
        if(buffer[i]==-1)
            break;
        printf("%02d ",buffer[i]);
    }
    printf("\n");
}



void printHeader(char *policy)
{
    printf("Replacement Policy = %s\n", policy);
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");

}

void printFooter(int pageFaults)
{
    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n", pageFaults);
}


void executeFIFO()
{

    printHeader("FIFO");
    struct Queue* queue = createQueue(sizeOfBuffer);

    int i =0;
    int pageFault = 0;
    int numberOfPageFaults = 0;
    for(i = 0; i < sizeOfArray; i++)
    {
        pageFault = 0;
        if(!isFull(queue)&&!contains(queue,inputArray[i] ))
        {
            enqueue(queue, inputArray[i]);
        }
        else if(isFull(queue)&&!contains(queue,inputArray[i] ))
        {
            numberOfPageFaults++;
            pageFault = 1;
            dequeue(queue);
            enqueue(queue, inputArray[i]);
        }
        else if(contains(queue, inputArray[i]))
        {
            //do nothing
        }
        printQueue(queue, inputArray[i], pageFault);
    }

    printFooter(numberOfPageFaults);
}


int containsInBuffer(int buffer[], int input )
{

    int i = 0 ;
    for(i = 0; i < sizeOfBuffer; i++)
    {
        if(buffer[i]==input)
        {
            return i;
        }
    }
    return -1;


}

int searchMinCount(int count[])
{

    int i = 0 ;
    int min = 999999;
    int index = 0;
    for(i = 0; i<sizeOfBuffer; i++)
    {
        if(count[i]<min)
        {
            min = count[i];
            index = i;
        }
    }
    return index;


}

void initializeArray(int count[])
{
    int i = 0;

    for(i = 0; i<sizeOfBuffer; i++)
    {
        count[i] = -1;
    }

}


void executeLRU()
{

    printHeader("LRU");
    int buffer[sizeOfBuffer];
    int bufferIndex = 0;
    int count[sizeOfBuffer];
    int LRU = 0;
    int pageFault = 0;
    int numberOfPageFaults = 0;
    int i = 0;

    initializeArray(buffer);
    initializeArray(count);
    for(i = 0; i < sizeOfArray; i++)
    {
        pageFault = 0;
        int index = containsInBuffer(buffer, inputArray[i]);
        if(index>=0)
        {
            count[index] = ++LRU;
        }
        else if(bufferIndex<sizeOfBuffer)
        {
            buffer[bufferIndex++] = inputArray[i];
        }
        else
        {
            pageFault = 1;
            numberOfPageFaults++;
            int index = searchMinCount(count);

            buffer[index] = inputArray[i];
            count[index] = ++LRU;
        }

        printBuffer(buffer, inputArray[i], pageFault);
    }
    printFooter(numberOfPageFaults);
}

int getIndexOfFirstZeroUseBit(int useBit[], int clockPointer)
{
    while(1)
    {
        clockPointer = clockPointer%sizeOfBuffer;
        if(useBit[clockPointer] == 0)
        {
            return clockPointer;
        }

        useBit[clockPointer] = 0;
        clockPointer++;
    }
}


void executeCLOCK()
{

    printHeader("CLOCK");
    int buffer[sizeOfBuffer];
    int bufferIndex = 0;
    int clockPointer = 0;
    int useBit[sizeOfBuffer];
    int pageFault = 0;
    int numberOfPageFaults = 0;

    int i = 0;
    initializeArray(buffer);
    initializeArray(useBit);
    int index;
    for(i = 0; i < sizeOfArray; i++)
    {
        pageFault = 0;
        index = containsInBuffer(buffer, inputArray[i]);

        //if contains in buffer
        if(index>=0)
        {
            useBit[index] = 1;
        }
        else if(bufferIndex<sizeOfBuffer)
        {
            bufferIndex++;
            int indexOfFirstZero = getIndexOfFirstZeroUseBit(useBit, clockPointer);

            buffer[indexOfFirstZero] = inputArray[i];
            useBit[indexOfFirstZero] = 1;
            clockPointer++;

        }
        else
        {
            pageFault = 1;
            numberOfPageFaults++;
            int indexOfFirstZero = getIndexOfFirstZeroUseBit(useBit, clockPointer);

            buffer[indexOfFirstZero] = inputArray[i];
            useBit[indexOfFirstZero] = 1;
            clockPointer++;
        }

        printBuffer(buffer, inputArray[i], pageFault);
    }
    printFooter(numberOfPageFaults);

}



void takeInput()
{

    scanf("%d", &sizeOfBuffer);
    scanf("%s", type);

    int i = 0;

    scanf("%d", &inputArray[i++]);
    while(1)
    {
        if(inputArray[i-1] == -1)
            break;

        scanf("%d", &inputArray[i++]);

    }
    sizeOfArray = i - 1;
}

int main()
{
    takeInput();

    if(!strcmp("FIFO", type))
    {
        executeFIFO();
    }
    else if(!strcmp("LRU", type))
    {
        executeLRU();

    }
    else if(!strcmp("CLOCK", type))
    {
        executeCLOCK();
    }

    return 0;
}
