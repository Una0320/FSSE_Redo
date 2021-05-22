#include <iostream>

using namespace std;

void merge_func(int *a, int *result,int *index,int *index_p, int low, int mid, int high){
    //merge sort，把a由小到大排序好，放入result中，index隨著a的排序一起移動排好
    int l1, l2, i;

    for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
        if(a[l1] <= a[l2]){
            //printf("l1:%d\n",l1);
            index_p[i]=index[l1];
            result[i] = a[l1++];
        }
        else{
            index_p[i]=index[l2];
            result[i] = a[l2++];
        }
    }
    while(l1 <= mid){
        index_p[i]=index[l1];
        result[i++] = a[l1++];
    }

    while(l2 <= high){
        index_p[i]=index[l2];
        result[i++] = a[l2++];
    }

    for(i = low; i <= high; i++){
        a[i] = result[i];
        index[i]=index_p[i];

    }
}

void sort_func(int *arr, int *result,int *index, int *index_p, int low, int high) {
    //arr和index是要sort的目標，以arr為主，result和index_p是中轉用的空間，可以開全域變數
   int mid;
   if(low < high) {
        mid = (low + high) / 2;
        sort_func(arr,result,index,index_p,low, mid);
        sort_func(arr,result,index,index_p,mid+1, high);
        //printf("low:%d mid:%d high:%d\n",low, mid, high);
        merge_func(arr, result,index,index_p, low, mid, high);

   } else {
      return;
   }
}

/* //sort_func是主體，中間用到merge_func
int main()
{
    //2個整數陣列，a是數值，index是a對應的index
    int a[10]={9,8,7,6,5,4,3,2,1,0};
    int index[10]={0,1,2,3,4,5,6,7,8,9};
    int re[10];
    int in[10];
    sort_func(a,re,index,in,0,9);//merge sort，把a由小到大排序好，放入re中，index隨著a的排序一起移動
    //0和9是a的index開頭跟結尾

    cout<<"sorted a : ";
    for(int i=0;i<10;i++){
        cout<<re[i];
    }
    cout<<endl;
    cout<<"index : ";
    for(int i=0;i<10;i++){
        cout<<in[i];
    }

    return 0;
}
*/