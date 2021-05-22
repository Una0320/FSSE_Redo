#include <iostream>

using namespace std;

typedef struct id_score_{
    int id;
    double score;
    struct id_score_* next;
} id_score;

typedef struct SI_word_{
    string w;
    id_score* next;
} SI_word;


//
int* DMatch(int *CKI, SI_word *SI){
    //SI是一個陣列，每一格都是SI_word的結構，每一格存放一個word，和一個指到id_score的指標
    //SI陣列中，如果由0~n-1格，0對應word0，word有多少就有幾格


    //這邊應該能知道CKI的大小
    int CKI_len = sizeof(CKI)/sizeof(int);
    int CFS[CKI_len];//輸出目標

    int j=0;
    for(int i=0;i<CKI_len;i++)
    {
        //針對每個word去找
        if( SI[ CKI[i] ].next != NULL )//表示這個word是有出現在某個document中的
        {
            id_score *ptr;
            ptr = SI[ CKI[i] ].next;
            while(ptr != NULL){
                CFS[j]=ptr->id;
                ptr = ptr->next;
                j++;
            }

        }
        else//表示這個word沒有出現在任何document中
        {
            continue;
        }
    }
    // j表示CFS中有多少個id
    int *CFS_return = (int*)malloc(sizeof(int)*j);
    memcpy(CFS_return, CFS, sizeof(int)*j);
    return CFS_return;

}

/*
int main()
{
    //SI是一個陣列，每一格都是SI_word的結構，每一格存放一個word，和一個指到id_score的指標

    return 0;
}
*/
