#include "memory.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

int main()
{
    int i=0;

	ifstream inFile("paper_keyword.csv", ios::in);//建立檔案物件，由物件呼叫member func
	if(inFile.is_open()){
        cout<<"open_successfully"<<endl;
	}
	else{
        cout<<"fail"<<endl;
	}
	string lineStr;
	//讀進來的資料分別存在這邊
    vector<string> index;
	vector<string> title;
	vector<string> keyword1;
	vector<string> keyword2;
	vector<string> keyword3;
	vector<string> keyword4;
    
    //這邊想做把vec轉成arr，好像用memcpy就可以了，但記憶體大小調整不好
	char arr_index[4*30];      memset(arr_index, '\0', sizeof(char)*4*30);
	char arr_title[121*30];    memset(arr_title, '\0', sizeof(char)*121*30);
	char arr_keyword1[20*30];  memset(arr_keyword1, '\0', sizeof(char)*20*30);
	char arr_keyword2[20*30];  memset(arr_keyword2, '\0', sizeof(char)*20*30);
	char arr_keyword3[20*30];  memset(arr_keyword3, '\0', sizeof(char)*20*30);
	char arr_keyword4[20*30];  memset(arr_keyword4, '\0', sizeof(char)*20*30);

	while (getline(inFile, lineStr))
	{

		cout << lineStr << endl;
        //cout<<sizeof(lineStr)<<endl;

		stringstream ss(lineStr);
        string str;
		//vector<string> lineArray;
        getline(ss, str, ',');
        index.push_back(str);

        getline(ss, str, ',');
        title.push_back(str);

        getline(ss, str, ',');
        keyword1.push_back(str);

        getline(ss, str, ',');
        keyword2.push_back(str);

        getline(ss, str, ',');
        keyword3.push_back(str);

        getline(ss, str, ',');
        keyword4.push_back(str);
	}

	return 0;
}
