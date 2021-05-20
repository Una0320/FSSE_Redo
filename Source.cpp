#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include "KFPA.h"
//#include "KFPA.h"
using namespace std;

/*Global Variables*/
string queryword[100];
vector<string>fingerprint_q;

/* User Input query word */
void Query()	
{
	cout << endl << "Input query word：";
	int c = 0;
	while (cin >> queryword[c])
	{
		fingerprint_q.push_back(fingerprint(queryword[c]));
		//cout << fingerprint_q[c] << endl;
		c++;
	}
	cout << "End of input query word" << endl << endl;
	getchar();		//Get char，because of Ctrl+Z
}

void convertUnCharToStr(char* str, unsigned char* UnChar, int ucLen)
{
	int i = 0;
	for (i = 0; i < ucLen; i++)
	{
		//格式化輸str,每unsigned char 轉換字元佔兩位置%x寫輸%X寫輸  
		printf(str + i * 2, "%02x", UnChar[i]);
	}
}
/*
string hextotwo(string str)		//16進制轉二進制
{
	string two = "";
	for (int f = 0; f < str.length(); f++)
	{
		char e = str[f];
		if (e >= 'a' && e <= 'f')
		{
			int a = e - 'a' + 10;
			switch (a)
			{
			case 10:two += "1010"; break;
			case 11:two += "1011"; break;
			case 12:two += "1100"; break;
			case 13:two += "1101"; break;
			case 14:two += "1110"; break;
			case 15:two += "1111"; break;
			}
		}
		else
		{
			int b = e - '0';
			switch (b)
			{
			case 0:two += "0000"; break;
			case 1:two += "0001"; break;
			case 2:two += "0010"; break;
			case 3:two += "0011"; break;
			case 4:two += "0100"; break;
			case 5:two += "0101"; break;
			case 6:two += "0110"; break;
			case 7:two += "0111"; break;
			case 8:two += "1000"; break;
			case 9:two += "1001"; break;
			}
		}
	}
	return two;
}

string fingerprint(string str)
{
	int M[128] = { 0 };
	string Y;
	string word = str;
	string sub_w[1000];
	for (int i = 0; i < word.length() - 1; i++)
	{
		sub_w[i] = word.substr(i, 2);
		cout << i << " = " << sub_w[i] << endl;
	}
	for (int j = 0; j < word.length() - 1; j++)
	{
		string temp = hextotwo(getMD5(sub_w[j]));
		cout << temp << "," << temp.length() << endl;
		for (int k = 0; k < 128; k++)
		{
			if (temp[k] == '1')M[k]++;
			else M[k]--;
		}
	}
	for (int i = 0; i < 128; i++)
	{
		if (M[i] >= 0)Y += "1";
		else Y += "0";
	}
	cout << Y << endl;
	return Y;
}
*/
int HamingD(string a, string b)
{
	int HD = 0;
	for (int i = 0; i < 128; i++)
	{
		if (a[i] != b[i])HD++;
	}
	return HD;
}

int main(void)
{
	/* Dataset word */
	string word;
	string sub_w[1000];
	cin >> word;
	string fingerprint_w = fingerprint(word);
	//cout << fingerprint_w;

	/*Get Dataset from csv*/
	map<string, string> file_index;
	string temp_index, temp_title;
	struct paper_key {
		string keyword[4];
	};
	paper_key temp_k;
	map<string, paper_key> file_keyword;
	int i = 0;
		ifstream inFile("paper_keyword.csv", ios::in);//建立檔案物件，由物件呼叫member func
	if (inFile.is_open()) {
		cout << "open_successfully" << endl;
	}
	else {
		cout << "fail" << endl;
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
	/*char arr_index[4 * 30];      memset(arr_index, '\0', sizeof(char) * 4 * 30);
	char arr_title[121 * 30];    memset(arr_title, '\0', sizeof(char) * 121 * 30);
	char arr_keyword1[20 * 30];  memset(arr_keyword1, '\0', sizeof(char) * 20 * 30);
	char arr_keyword2[20 * 30];  memset(arr_keyword2, '\0', sizeof(char) * 20 * 30);
	char arr_keyword3[20 * 30];  memset(arr_keyword3, '\0', sizeof(char) * 20 * 30);
	char arr_keyword4[20 * 30];  memset(arr_keyword4, '\0', sizeof(char) * 20 * 30);
	*/
	while (getline(inFile, lineStr))
	{
		cout << lineStr << endl;
		//cout<<sizeof(lineStr)<<endl;

		stringstream ss(lineStr);
		string str;
		//vector<string> lineArray;
		getline(ss, str, ',');
		index.push_back(str);
		temp_index = str;
		

		getline(ss, str, ',');
		title.push_back(str);
		temp_title = str;
		file_index.insert(pair<string, string>(temp_index, temp_title));

		getline(ss, str, ',');
		keyword1.push_back(str);
		temp_k.keyword[0] = str;

		getline(ss, str, ',');
		keyword2.push_back(str);
		temp_k.keyword[1] = str;

		getline(ss, str, ',');
		keyword3.push_back(str);
		temp_k.keyword[2] = str;

		getline(ss, str, ',');
		keyword4.push_back(str);
		temp_k.keyword[3] = str;
		file_keyword.insert(pair<string, paper_key>(temp_index, temp_k));
	}
	for (auto it = file_index.begin(); it != file_index.end(); it++)	cout << it->first << " " << it->second << endl;
	for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)	cout << it->first << " " << it->second.keyword[0] << " " << it->second.keyword[1] << " " << it->second.keyword[2] << " " << it->second.keyword[3] << endl;

	/* User Input query word */
	Query();

	system("pause");
	return 0;
}
