#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

int *QKMatch(int *QM, int *KM, int len)
{ //len is the length of KM

    int CKI[len];
    //memset(CKI,-1,sizeof(int)*len);//initialize array to -1

    int j = 0;
    for (int i = 0; i < len; i++)
    {
        if (QM[i] == KM[i] && KM[i] == 1)
        {
            CKI[j] = i;
            j++; // length of CKI
        }
    }
    int *new_arr = (int *)malloc(sizeof(int) * j);
    memcpy(new_arr, CKI, sizeof(int) * j);
    return new_arr;
}

//以下是驗證此function的主程式
int main()
{
    int QM[5] = {1, 2, 1, 4, 5};
    int KM[5] = {1, 2, 1, 4, 3};

    int *ans = QKMatch(QM, KM, 5);

    int len = sizeof(ans) / sizeof(int);
    for (int i = 0; i < len; i++)
    {
        cout << ans[i] << " " << endl;
    }
    return 0;
}
