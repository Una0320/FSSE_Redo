#include <iostream>
#include <algorithm>
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

/*---------------------------------------Global Variables---------------------------------------*/
string			Dictionary[1000];		//All keyword from dataset, remove the same words
vector<string>	fingerprint_w;
int				realnum_key = 0;		//Real number of keywords
string			queryword[100];			//Query word
vector<string>	fingerprint_q;			//Query word's fingerprint
int				realnum_que = 0;		//Real number of query keywords

/*Get Dataset from csv*/
map<string, string> file_index;			//file_id、paper(file) name
string				temp_index, temp_title;
int					paper_count = 0;	//count how many paper in excel

struct paper_key {
	string keyword[4];
};
paper_key				temp_k;
map<string, paper_key>	file_keyword;	//file_id、paper(file)'s keyword*4

int *KM, *QM;
int *CKI;
/*---------------------------------------Global Variables---------------------------------------*/


/* User Input query word */
void Query()	
{
	cout << endl << "Input query word：";
	while (cin >> queryword[realnum_que])
	{
		fingerprint_q.push_back(fingerprint(queryword[realnum_que]));
		//cout << fingerprint_q[c] << endl;
		realnum_que++;
	}
	cout << "End of input query word" << endl << endl;
	//getchar();		//Get char，because of Ctrl+Z
}

/*Get Dataset from csv*/
void readDataset()
{
	ifstream inFile("paper_keyword.csv", ios::in);//建立檔案物件，由物件呼叫member func
	if (inFile.is_open())	cout << "open_successfully" << endl;
	else					cout << "fail" << endl;

	string lineStr;
	//讀進來的資料分別存在這邊
	vector<string> index;
	vector<string> title;
	vector<string> keyword1;
	vector<string> keyword2;
	vector<string> keyword3;
	vector<string> keyword4;

	while (getline(inFile, lineStr))
	{
		cout << lineStr << endl;

		stringstream ss(lineStr);
		string str;
		
		getline(ss, str, ',');
		index.push_back(str);
		temp_index = str;
		paper_count++;

		getline(ss, str, ',');
		title.push_back(str);
		temp_title = str;
		file_index.insert(pair<string, string>(temp_index, temp_title));

		getline(ss, str, ',');
		keyword1.push_back(str);
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[0] = str;


		getline(ss, str, ',');
		keyword2.push_back(str);
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[1] = str;

		getline(ss, str, ',');
		keyword3.push_back(str);
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[2] = str;

		getline(ss, str, ',');
		keyword4.push_back(str);
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[3] = str;
		file_keyword.insert(pair<string, paper_key>(temp_index, temp_k));
	}

	//for (auto it = file_index.begin(); it != file_index.end(); it++)	cout << it->first << " " << it->second << endl;
	for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)	cout << it->first << " " << it->second.keyword[0] << " " << it->second.keyword[1] << " " << it->second.keyword[2] << " " << it->second.keyword[3] << endl;
	
	for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)	//Put keyword into Dictionary
	{
		for (int j = 0; j < 4; j++)
			Dictionary[realnum_key++] = it->second.keyword[j];
	}

	for (int i = 0; i<realnum_key; i++)			//Remove Duplicates From Dictionary
	{
		for (int j = i + 1; j<realnum_key; j++)
		{
			if (!(Dictionary[i].compare(Dictionary[j])))
			{
				for (int k = j + 1; k<realnum_key; k++)
					Dictionary[k - 1] = Dictionary[k];
				realnum_key--;		//array length -1
				j--;				//repeat check again
			}
		}
	}
	cout << "Real number of keywords:" << realnum_key << endl;
	for (int k = 0; k < realnum_key; k++)			//Generate Dictionary fingerprint
	{
		fingerprint_w.push_back(fingerprint(Dictionary[k]));
		cout << Dictionary[k] << ",";
	}
	inFile.close();
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

/* Count Hamming Distance */
int HamingD(string a, string b)
{
	int HD = 0;
	for (int i = 0; i < 128; i++)
	{
		if (a[i] != b[i])HD++;
	}
	return HD;
}

/* Copy temp_KM、temp_QM into Global variables */
void setQKM(int* t_k, int* t_q)
{
	KM = (int*)malloc(sizeof(int)*realnum_key);
	memcpy(KM, t_k, sizeof(int)*realnum_key);

	QM = (int*)malloc(sizeof(int)*realnum_key);
	memcpy(QM, t_q, sizeof(int)*realnum_key);
}

/* Generate KM、QM */
void GenerateKQM()
{
	int* temp_KM = (int*)malloc(sizeof(int)*realnum_key);
	int* temp_QM = (int*)malloc(sizeof(int)*realnum_key);
	for (int i = 0; i < realnum_key; i++)			//Initialize
	{
		temp_KM[i] = 1;
		temp_QM[i] = 0;
	}

	for (int i = 0; i < realnum_key; i++)
	{
		for (int j = 0; j < realnum_que; j++)
		{
			if (!(Dictionary[i].compare(queryword[j]))) temp_QM[i] = 1;
		}
	}
	cout << endl;
	for (int i = 0; i < realnum_key; i++)
	{
		cout << temp_QM[i];
	}
	cout << endl << endl;
	
	setQKM(temp_KM, temp_QM);
	free(temp_KM);
	free(temp_QM);
}

/*Function 1 -- QKMatch*/
int* QKMatch(int* QM, int* KM, int len) {	//len is the length of KM

	int tempsize = realnum_key;
	int CKI[100] = { 0 };
	//memset(CKI,-1,sizeof(int)*len);//initialize array to -1

	int j = 0;
	for (int i = 0; i<len; i++)
	{
		if (QM[i] == KM[i] && KM[i] == 1)
		{
			CKI[j] = i;
			j++;// length of CKI
		}

	}
	int* new_arr = (int*)malloc(sizeof(int)*j);
	memcpy(new_arr, CKI, sizeof(int)*j);
	return new_arr;
}

int main(void)
{
	/* Dataset word */
	string word;
	string sub_w[1000];
	cin >> word;
	string fingerprint_w = fingerprint(word);
	//cout << fingerprint_w;

	/* Get Dataset from csv */
	readDataset();

	/* User Input query word */
	Query();

	/* Generate KM、QM */
	GenerateKQM();
	for (int i = 0; i<realnum_key; i++) {
		cout << KM[i];
	}
	cout << endl;
	for (int i = 0; i<realnum_key; i++) {
		cout << QM[i];
	}

	/* QKMatch */
	CKI = QKMatch(QM, KM, realnum_key);
	for (int i = 0; i < sizeof(CKI)-1; i++) {
		cout << CKI[i] << ",";
	}


	system("pause");
	return 0;
}
