# word_blast
C program that utilizes multithreads ,parses through text file and displays top 10 most frequent words (with length of 6 or more).

Program opens the text file ( WarAndPeace.txt) and counts the words with length of 6 or more characters and displays the top number words.
This program focuses on data parallelism, threads are created, critical sections are guarded with mutex locks.
The efficiency of threads is directly proportionate to number of cores available.


Since I run this program on VM which is only one core, the results of 1 thread vs 2 threads is significant, 50% reduction in processing time (from 1 thread to
2 threads).
But as the threads are increased (from 2 to 4) the difference between very minimal (since the number of available cores limits parallel processing).


**********************************************************************
**********************************************************************

RESULTS OF MY PROGRAM OUTPUT ARE PROVIDED IN A SEPARATE FILE

**********************************************************************
**********************************************************************



HOW TO CONFIGURE AND RUN THE PROGRAM

1) cd to folder (where makefile and main.c are located)

2) change make file ( details are mentioned in makefile)

3) type in command ( make run ) in linux terminal ( which will create default 4 threads as it is specified in make file)

4) or provide command make run  main  WarAndPeace.txt #threads 
   for example, if we want 2 threads it would be ( make run main WarAndPeace.txt 2)
   



