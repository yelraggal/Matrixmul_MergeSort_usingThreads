#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// in libarary p thread lzm nb3tlha el arguments f struct
struct data{
	int left;
	int right;
};
// dynamic array in C
int * arr_nums;

void merge(int left, int middle, int right)
{
	int size1 = middle-left+1;
	int size2 = right-middle;
	int left_arr[size1];
	int right_arr[size2];
    	int i;
    	int j = 0;
    	int k = left;  
    	for (i=0; i<size1; i++){
        	left_arr[i] = arr_nums[left+i];
    	}
    	for (i=0; i<size2; i++){
        	right_arr[i] = arr_nums[middle+1+i];
    	}
    	i = 0;
    	while(i<size1 && j<size2){
        	if(left_arr[i] <= right_arr[j]){
        		arr_nums[k] = left_arr[i];
            		i++;
        	}else{
            		arr_nums[k] = right_arr[j];
            		j++;
        	}
        	k++;
    	} 
    	for(i=i; i<size1; i++){
        	arr_nums[k] = left_arr[i];
        	k++;
    	}
    	for(j=j; j<size2; j++){
    		arr_nums[k] = right_arr[j];
    		k++;
    	}
}


void * mergeSort(void * args)
{
	struct data *data2 = args;
	int left = (int)data2->left;
	int right = (int)data2->right;
	if (left < right){
        	int middle = left+(right-left)/2;
        	struct data data3;
        	pthread_t th1;
        	data3.left = left;
        	data3.right = middle;
        	pthread_create(&th1,NULL,&mergeSort,&data3);
        	
        	struct data data4;
        	pthread_t th2;
        	data4.left = middle + 1;
        	data4.right = right;
        	pthread_create(&th2,NULL,&mergeSort,&data4);
        	
        	pthread_join(th1,NULL);// mainthread tstna el sub threads
        	pthread_join(th2,NULL);
        	merge(left, middle, right);
        }
}

int main(){
	FILE *inputs;
	char filename[50];
	//take name of file from user 
	printf("Enter the file name: ");
	scanf("%s",filename);
	//read unsorted input array from file
	inputs = fopen(filename,"r");
	if(inputs==NULL){
		printf("Error in file\n");
		exit(0);
	}
	int size, i;
	struct data data1;
	fscanf(inputs,"%d",&size);
	//allocate memory for inputs numbers   
	arr_nums = (int *)malloc(size*sizeof(int));
	for(i=0; i<size; i++){
		fscanf(inputs,"%d",&arr_nums[i]);
	}
	fclose(inputs);
	
	data1.left = 0;
	data1.right = size - 1;
	
	mergeSort(&data1);
	
	//write sorted output array to file
	inputs = fopen("output.txt","w");
	if(inputs == NULL){
        	printf("Error in file\n");
        }else{
        	fprintf(inputs,"Sorted array: ");
        	for(i=0; i<size; i++){
        		fprintf(inputs,"%d ",arr_nums[i]);
        	}
        }
        fclose(inputs);
        free(arr_nums);	
	return 0;
}
