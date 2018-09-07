/*
  developed by:   Sepand  Seradj   9313013
      Taher  Akbari  93311705
*/



#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
 
void insertionSort(int arr[], int n);
void merge(int a[], int l1, int h1, int h2);
 
void mergeSort(int a[], int l, int h)
{
    int i, len=(h-l+1);
 
    // Using insertion sort for small sized array


    if (len<=5000)
    {
        insertionSort(a+l, len);
        return;
    }
 
    pid_t lpid,rpid;
    lpid = fork();
    if (lpid<0)
    {
        // Lchild proc not created
        perror("Left Child Proc. not created\n");
        _exit(-1);
    }
    else if (lpid==0)
    {
        mergeSort(a,l,l+len/2-1);
        _exit(0);
    }
    else
    {
        rpid = fork();
        if (rpid<0)
        {
            // Rchild proc not created
            perror("Right Child Proc. not created\n");
            _exit(-1);
        }
        else if(rpid==0)
        {
            mergeSort(a,l+len/2,h);
            _exit(0);
        }
    }
 
    int status;
 
    // Wait for child processes to finish
    waitpid(lpid, &status, 0);
    waitpid(rpid, &status, 0);
 
    // Merge the sorted subarrays
    merge(a, l, l+len/2-1, h);
}
 
/* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
  { int i, key, j;
   for (i = 1; i < n; i++)
   {
       key = arr[i];
       j = i-1;

       while (j >= 0 && arr[j] > key)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
   }
}
 
// Method to merge sorted subarrays
void merge(int a[], int l1, int h1, int h2)
{
  
    int count=h2-l1+1;
    int sorted[count];
    int i=l1, k=h1+1, m=0;
    while (i<=h1 && k<=h2)
    {
        if (a[i]<a[k])
            sorted[m++]=a[i++];
        else if (a[k]<a[i])
            sorted[m++]=a[k++];
        else if (a[i]==a[k])
        {
            sorted[m++]=a[i++];
            sorted[m++]=a[k++];
        }
    }
 
    while (i<=h1)
        sorted[m++]=a[i++];
 
    while (k<=h2)
        sorted[m++]=a[k++];
 
    int arr_count = l1;
    for (i=0; i<count; i++,l1++)
        a[l1] = sorted[i];
}
 
// To fill randome values in array for testing
void fillData(int a[], int len)
{
    // Create random arrays
    int i;
    for (i=0; i<len; i++)
        a[i] = rand();
    return;

}
 
// main function
int main()
{   clock_t begin=clock();
    int shmid;
    key_t key = IPC_PRIVATE;
    int *shm_array; 
    int length =10000 ;
 
    // Calculate segment length
    size_t SHM_SIZE = sizeof(int)*length;
 
    // Create the segment.
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        _exit(1);
    }
 
    // Now we attach the segment to our data space.
    if ((shm_array = shmat(shmid, NULL, 0)) == (int *) -1)
    {
        perror("shmat");
        _exit(1);
    }
 
    // Create a random array of given length
    srand(time(NULL));
    fillData(shm_array, length);
 
    // Sort the created array
    mergeSort(shm_array, 0, length-1);
 
    // print array after sorting 
	for (int i=0;i<10000;i++)
	{
	printf("%d\n  ",shm_array[i]);
	}
	 	
    // Detach from the shared memory now that we aredone using it. 
      
    if (shmdt(shm_array) == -1)
    {
        perror("shmdt");
        _exit(1);
    }
 
    // Delete the shared memory segment. 
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        _exit(1);
    }
	clock_t end=clock();
	double time_spent=(double)(end-begin);
	printf("procces time: %f ",time_spent);
 
    return 0;
}




