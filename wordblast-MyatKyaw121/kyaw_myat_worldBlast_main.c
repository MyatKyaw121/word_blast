
/**************************************************************

* Description: This assignment is to read War and Peace.txt file, and count the number of words,
* that have 6 or more characters in length. The goal of this assignment is to teach us the usage of 
* threads, and the proper usage of its methods (pthread_create, pthread_join) etc, also to teach us 
* the usage of mutex,  along with concepts such as critical section, atomicity,etc. 
*
**************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// initializing mutex lock
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

// struct to store the word from text file as well as its number of occurences
typedef struct Word
{
    char *word;
    int frequency;
} Word;

void *readFile(void *);
void AddWord(char *);
void sortArray(Word *, int);
int containsWord(char *);

// You may find this Useful
char *delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";
long indivial_textBlockSize = 0; // size of individual text chunks divied accordingly for each thread
char *fileName;                  // Name of the file that we are gonna process
Word *word_array;                // (word) array that will be holding the pointers of struct Word, keeping track of each word and its occurences
int array_index = 0;             // index of the word array
long *s_indexArray;              // array of starting indexes of text chunk that each thread is gonna process


// helper method to check if the word is already inside our word array
// if the word is already inside the array, its count (frequency ) will be increased
// since we cannot have threads processing the increment process of the word count (critical section)
// (since we want that incrementation process to be atomic)
// mutual execlusion is applied, pthread_mutex_lock and  pthread_mutex_unlock are called before and after the incrementation process

// finally, if we could find the word is already inside the array, value of 0 will be returned
// and -1 otherwise

//  I learned about pthread_mutext_lock and pthread_mutex_unlock functions from linux manaual page,
//  as well as professor's notes
//  https://linux.die.net/man/3/pthread_mutex_lock 

int containsWord(char *currentWord)
{
    int i = 0;
    while ((i < array_index))
    {
        if (strcasecmp(currentWord, word_array[i].word) == 0)
        {
            pthread_mutex_lock(&mutex_lock);
            word_array[i].frequency++;
            pthread_mutex_unlock(&mutex_lock);
            return 0;
        }
        i++;
    }
    return -1;
}

// Helper method to be called inside routine function of threads
// First, it checks (calls containsWord function), if the word is already inside the word array
// if it isn't, then currentWord will be added to array of words

// word_array[array_index].word is allocated dynamically to the size of currentWord,=>
// => since we want the exact word_size for each word
// then, we copy the currentWord into our array using strncpy

// since we dont want  multiple threads processing the word addition process on our word array (critical section) =>
// => at the same time
// (since we want that word addition process to be atomic)
// (mutual exclusion), pthread_mutex_lock and  pthread_mutex_unlock are called before and after =>
// => the whole word addition process

//  I learned about pthread_mutext_lock and pthread_mutex_unlock functions from linux manaual page,
//  as well as professor's notes
//  https://linux.die.net/man/3/pthread_mutex_lock 


void AddWord(char *currentWord)
{
    if (containsWord(currentWord) != 0)
    {

        pthread_mutex_lock(&mutex_lock);
        int word_size = strlen(currentWord);
        word_array[array_index].word = malloc((word_size));
        strncpy(word_array[array_index].word, currentWord, word_size);
        word_array[array_index].frequency = 1;
        array_index++;

        pthread_mutex_unlock(&mutex_lock);
    }
}

// helper method to help sort the array of words
// this method takes in array of words (array of the pointers of Struct Word) and indexes of array
// it uses bubble sort algorithm (but the other loop iterates only up to 10 since we only want highest 10 values)

void sortArray(Word *word_array, int array_index)
{

    for (int j = 0; j < 10; j++)
    {
        for (int k = j + 1; k < array_index; k++)
        {
            if (word_array[k].frequency > word_array[j].frequency)
            {
                Word local = word_array[j];

                word_array[j].word = word_array[k].word;
                word_array[j].frequency = word_array[k].frequency;
                word_array[k].word = local.word;
                word_array[k].frequency = local.frequency;
            }
        }
    }
}

// routine function where threads start and end
// this function takes in long* (starting index of each text chunk) as parameter
// char* buffer is dynamically allocated to the size of (individal_textBlockSize+2)

// Open the file, using READ ONLY Flag
// if sucessful, file descriptor value (non negative integer) will be returned from open function
// open function will return -1 if operation fails

// the function actual parameter is casted to long type
// First, set the file descriptor offset to long value (that is passed in)using lseek function ( SEEK_SET )
// Then, we call read function passing in file descriptor, buffer, and individual textBlockSize as arguments

// then we close the file.

// then the text chunk is broken down into tokens using strtok_r() function
// if the token has 6 or more characters, then it will be passed into AddWord() function

// then we free the buffer and assign it to NULL to prevent memory leak and dangling pointer issues

// I learned about read function from Linux man page
//  https://man7.org/linux/man-pages/man2/read.2.html

// I learned about open function and its return values from Linux man page
// https://man7.org/linux/man-pages/man2/open.2.html

// I learned about lseek, and its params from linux manaual page
// https://man7.org/linux/man-pages/man2/lseek.2.html

// I learned about close function from linux manaual page
// https://man7.org/linux/man-pages/man2/close.2.html

void *readFile(void *index)
{

    char *buffer = malloc(indivial_textBlockSize + 2);

    int fd = open(fileName, O_RDONLY);
    if (fd <= 0)
    {
        printf("Error in opening the file provided %d\n", errno);
        exit(2);
    }
    long *local = (long *)index;
    lseek(fd, *local, SEEK_SET);
    read(fd, buffer, indivial_textBlockSize);
    int closeVal = close(fd);

    // checking if we successfully close the file
    if (closeVal != 0)
    {
        printf("Error in Closing the File\n");
        exit(1);
    }

    char *localPtr;

    char *token = strtok_r(buffer, delim, &localPtr);
    while (token)
    {
        if (strlen(token) >= 6)
        {
            AddWord(token);
        }
        token = strtok_r(NULL, delim, &localPtr);
    }

    free(buffer);
    buffer = NULL;
}

int main(int argc, char *argv[])
{

    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structures

    // int variables for file descriptor and  number of threads

    int fd, num_threads;

    // array of pointers for threads (array of pthread_t* pointers)
    pthread_t *t_ptr;

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish

    

    // first, checking if we have argument 1 (file name) and argument 2 (number of threads)
    // if they are absent, error message will be printed out, and program will be ended

    if (argv[1] && argv[2])
    {
        num_threads = atoi(argv[2]);
        fileName = argv[1];
    }
    else
    {
        printf("Error! Missing File Name (and) or number of threads\n");
        exit(1);
    }

    // I learned about open function and its return values from man page
    // https://man7.org/linux/man-pages/man2/open.2.html

    // Open the file, using READ ONLY Flag
    // if successful, file descriptor value (non negative integer) will be returned from open function
    // open function will return -1 if operation fails

    fd = open(fileName, O_RDONLY);

    // checking if open function fails
    // if it fails, error message will be printed and program will be terminated

    if (fd <= 0)
    {
        printf("Error in opening the file provided\n");
        exit(2);
    }

    // I learned about lseek, and its params from linux manaual page
    // https://man7.org/linux/man-pages/man2/lseek.2.html

    // I learned about close function from linux manaual page
    // https://man7.org/linux/man-pages/man2/close.2.html

    //I also learned read, open, close, lseek functions from one YouTube video as well.
    // “System Calls | Read | Write | Open | Close | Linux” ( by iFocus Institute) (www.youtube.com)

    // First, set the file descriptor offset to 0 using lseek function ( SEEK_SET )
    // Then, we get the size of the file ( SEEK_END gives file size + offset values)
    // then we close the file.
    // if sucessful,close function returns the value of 0

    lseek(fd, 0, SEEK_SET);
    long file_size = lseek(fd, 0, SEEK_END);
    int closeVal = close(fd);

    // checking if we successfully close the file
    if (closeVal != 0)
    {
        printf("Error in Closing the File\n");
        exit(1);
    }

    // get the size of text chunk for indivial threads
    indivial_textBlockSize = file_size / num_threads;

    // allocate array of pointers for threads
    t_ptr = malloc(sizeof(pthread_t) * num_threads);

    // allocate (word array) array of pointers for struct Word
    word_array = malloc(file_size);

    // allocate array of starting indexes (of text chunks that are gonna be processed by each thread)
    s_indexArray = malloc(sizeof(s_indexArray) * num_threads);

    // creating local counter for keeping track of thread array's indexes as well as the
    // indexes for array of starting indexes of text chunk (that each thread is gonna process)

    int count = 0;

    // while the counter is less than the number of threads specified
    // starting index array's element for each thread is calculated
    // then pthread_create method is called,  passing in 
    // 1) the address of each thread (of thread array (pthread_t array)) ->
    // ->2) NULL as second (attribute param), 3) address of the function (that each thread will execute)
    // -> and 4)address of the function parameter

    // and for each pthread_create, we also need to check if pthread_create function succeeds or not,
    // pthread_create returns 0 if it is sucessful, and other numbers if it fails

    // I learned the pthread_create function from professor Bierman's lectures as well from linux man page
    // https://man7.org/linux/man-pages/man3/pthread_create.3.html

    // I also learned the pthread_create function from tutorialspoint.com 
    // https://www.tutorialspoint.com/multithreading-in-c
    while (count < num_threads)
    {

        s_indexArray[count] = (file_size / num_threads) * count;
        int local = pthread_create(&(t_ptr[count]), NULL, readFile, &(s_indexArray[count]));
        if (local != 0)
        {
            printf("pthread_create failed, exiting the program!\n");
            exit(2);
        }

        count++;
    }

    // this loop is for waiting of thread termination
    // pthread_join function is called with each thread id (of the array of pthread_t pointers)

    // And for each pthread_join() call, we have to make sure that pthread_join () works correctly
    // if successful, pthread_join() returns 0 and if it fails,
    // it returns other numbers

    // I learned the pthread_join function from professor Bierman's lectures as well from linux man page
    // https://man7.org/linux/man-pages/man3/pthread_join.3.html

    // I also learned the pthread_join function from geeksforgeeks.org as well
    // https://www.geeksforgeeks.org/multithreading-c-2/


    for (int i = 0; i < num_threads; i++)
    {
        int local = pthread_join(t_ptr[i], NULL);
        if (local != 0)
        {
            printf("pthread_join failed!, exiting the program!\n");
            exit(2);
        }
    }


    // ***TO DO *** Process TOP 10 and display


    // sortArray function is called, passing in array of words, and indexes as the param
    sortArray(word_array, array_index);

    // printing out the results of top 10 words that have 6 or more characters

    printf("\n");
    printf("\n");
    printf("Word Frequency Count on WarAndPeace.txt with %d threads\n", num_threads);
    printf("Printing top 10 words 6 characters or more.\n");
    for (int j = 0; j < 10; j++)
    {
        printf("Number %d is %s with a count of %d\n", (j + 1), word_array[j].word, word_array[j].frequency);
    }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
    {
        --sec;
        n_sec = n_sec + 1000000000L;
    }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************

    // ***TO DO *** cleanup


    // destroying the mutex_lock
    // checking if pthread_mutex_destroy function succeeds
    // if it fails, error will be printed and program will be ended

    // I learned about pthread_mutex_destroy function from professor's lectures as well as
    // from Linux man page
    // https://linux.die.net/man/3/pthread_mutex_destroy
    
    int local_num=pthread_mutex_destroy(&mutex_lock);
    if(local_num!=0){
          printf("pthread_mutex_destroy failed!, exiting the program!\n");
          exit(2);
    }

    // starting index array (s_indexArray), word_array as well as each element of word array ->
    // ->(since each element of word array is dynamically allocated as well), array of pthread_t  pointers
    // are all freed and assigned to NULL
    // to prevent memory leak and dangling pointer issues

    free(s_indexArray);
    s_indexArray = NULL;

    free(t_ptr);
    t_ptr = NULL;
    for (int i = 0; i < array_index; i++)
    {
        free(word_array[i].word);
        word_array[i].word = NULL;
    }
    free(word_array);
    word_array = NULL;
    return 0;
}
