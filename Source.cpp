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

struct paper_key {						//keyword score(random,1.7);
	string keyword[4];
};
paper_key				temp_k;
map<string, paper_key>	file_keyword;	//file_id、paper(file)'s keyword*4

struct id_score_ {						//Build SI(Security Inverted Index)
	int id;
	double score;
	struct id_score_* next;
};
typedef struct SI_word_ {
	string w;
	id_score_* next;
} SI_word;

struct id_sc {
	string id;
	double score;						//keyword score(4.5,3.2,2.6,1.7);
};
multimap<string, id_sc>  SI;			//<Dictionary(keyword), id_sc(file_id, file's score)>
id_sc temp_idcs;
int *KM, *QM;							//KM：keyword mark vector 、 QM：Query keyword mark vector
int *CKI;								//CKI：Candidate Keyword Index
int *CFS;								//CFS：Candidate File Set

map<string, double> RList;				//RList：Remember file_id and total score
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
	if (inFile.is_open())	cout << "Open_successfully" << endl;
	else					cout << "Fail" << endl;

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
		//cout << lineStr << endl;

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

	/* Print file_id & keyword[4] */
	//for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)	cout << it->first << " " << it->second.keyword[0] << " " << it->second.keyword[1] << " " << it->second.keyword[2] << " " << it->second.keyword[3] << endl;
	
	/* Put keyword into Dictionary */
	for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)
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

/* Set SI */
void BuildSI()
{
	// Build first node
	id_score_ *head = new id_score_();
	head->next = NULL;
	id_score_ *current = head;

	//file_keyword：file_id、paper(file)'s keyword*4
	int temp_realkey = 0;
	for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)	//Put keyword into Dictionary
	{
		int file_i = atoi((it->first).c_str());								//file_id(string->int)

		//SI：<Dictionary(keyword), id_sc(file_id, file's score)>
		double x = (5 - 1) * rand() / (RAND_MAX + 1.0) + 1;					//random score(range：5~1)
		temp_idcs.id = it->first;											//put file_id into temp_idcs(struct,id)

		for (int j = 0; j < 4; j++)
		{
			if (j == 0)
			{
				//Una method
				temp_idcs.score = x;										//put random score into temp_idcs(struct,score)
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));		//insert(file_id<string>, word's score<double>)

																						//Cliff method
				current->id = file_i;
				current->score = 4.5;
				current->next = new id_score_();
				current = current->next;  //現在指到哪
				current->next = NULL;
			}
			else if (j == 1)
			{
				temp_idcs.score = x;
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));

				current->id = file_i;
				current->score = 3.2;
				current->next = new id_score_();
				current = current->next;  //現在指到哪
				current->next = NULL;
			}
			else if (j == 2)
			{
				temp_idcs.score = x;
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));

				current->id = file_i;
				current->score = 2.6;
				current->next = new id_score_();
				current = current->next;  //現在指到哪
				current->next = NULL;
			}
			else
			{
				temp_idcs.score = 1.7;
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));

				current->id = file_i;
				current->score = 1.7;
				current->next = new id_score_();
				current = current->next;  //現在指到哪
				current->next = NULL;
			}
		}

	}
	/* Print linked list */
	/*current = head;
	while (true) {
	cout << "File id：" << current->id << "\t score：" << current->score << endl;  //印目前的節點
	current = current->next;	//印完後要印下一個節點的內容
	if (current == NULL) {		//current為NULL的時候停下來
	break;
	}
	}*/

	/* Clear linked list */
	while (head != 0) {            // Traversal
		id_score_ *current = head;
		head = head->next;
		delete current;
		current = 0;
	}

	/* Print map--SI */
	for (auto it = SI.begin(); it != SI.end(); it++)	cout << it->first << " " << it->second.id << " , " << it->second.score << " " << endl;
}

/* Function 1 -- QKMatch */
int* QKMatch(int* QM, int* KM, int len) {	//len is the length of KM

	int tempsize = realnum_key;
	int CKI[100] = { 0 };
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

/* Function 2 -- DMatch */
int* DMatch(int *CKI, int cki_len) {
	//SI是一個陣列，每一格都是SI_word的結構，每一格存放一個word，和一個指到id_score的指標
	//SI陣列中，如果由0~n-1格，0對應word0，word有多少就有幾格

	int CKI_len = sizeof(CKI) / sizeof(int);
	int CFS[200] = { 0 };
	int j = 0;

	for (int i = 0; i < cki_len; i++)
	{
		//針對每個word去找
		multimap<string, id_sc>::iterator m;
		m = SI.find(Dictionary[CKI[i]]);
		for (int k = 0; k != SI.count(Dictionary[CKI[i]]); k++, m++)
		{
			cout << m->first << "--" << m->second.id << endl;
			CFS[j++] = atoi((m->second.id).c_str());
			//j++;
		}
		/*if (SI[CKI[i]].next != NULL)//表示這個word是有出現在某個document中的
		{
			id_score *ptr;
			ptr = SI[CKI[i]].next;
			while (ptr != NULL) {
				CFS[j] = ptr->id;
				ptr = ptr->next;
				j++;
			}

		}
		else//表示這個word沒有出現在任何document中
		{
			continue;
		}*/
	}

	//j：means how many id in CFS
	int *CFS_return = (int*)malloc(sizeof(int)*j);
	memcpy(CFS_return, CFS, sizeof(int)*j);
	return CFS_return;
}

/* Function 3 -- Cscore */
void Cscore()
{
	/* use CKI、CFS */
	for (int i = 0; i < _msize(CFS) / sizeof(CFS[0]); i++)
	{
		double total_score = 0;
		string current_fileid = to_string(CFS[i]);
		//先找到當前的paper
		for (int j = 0; j < _msize(CKI) / sizeof(CKI[0]); j++)
		{
			//SI：<Dictionary(keyword), id_sc(file_id, file's score)>
			//針對每個word去找
			
			//file_keyword -- file_id、paper(file)'s keyword*4
			multimap<string, id_sc>::iterator m;
			m = SI.find(Dictionary[CKI[j]]);
			//cout << "KEY\tELEMENT\n";
			for (int k = 0; k != SI.count(Dictionary[CKI[j]]); k++, m++)
			{
				if (m->second.id == current_fileid) total_score += m->second.score;
				//cout << m->first << "--" << m->second.id << endl;
				//CFS[j++] = atoi((m->second.id).c_str());
				//j++;
			}
		}
		RList.insert(pair<string, double>(current_fileid, total_score));
	}
	cout << endl << "RList：" << endl;
	for (auto it = RList.begin(); it != RList.end(); it++)	cout << it->first << " " << it->second << endl;
}

/* Rank，cannot use map<value> sort, so need other function */
void printVec(vector< pair<string, double> > &vec) {
	vector<pair<string, double> >::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		cout << it->first << " : " << it->second << endl;
	}
}
bool comp_by_value(pair<string, double> &p1, pair<string, double> &p2) {
	return p1.second > p2.second;
}
struct CompByValue {
	bool operator()(pair<string, double> &p1, pair<string, double> &p2) {
		return p1.second > p2.second;
	}
};
void Rank()
{
	// Put <map>RList into vector for sorting 
	vector<pair<string, double> > vec(RList.begin(), RList.end());
	sort(vec.begin(), vec.end(), comp_by_value);
	printVec(vec);
}
int main(void)
{
	/* Get Dataset from csv */
	readDataset();

	/* User Input query word */
	Query();

	/* Generate KM、QM */
	GenerateKQM();
	for (int i = 0; i<realnum_key; i++) cout << KM[i];
	cout << endl;
	for (int i = 0; i<realnum_key; i++)	cout << QM[i];
	cout << endl;

	/* Build SI */
	BuildSI();

	/* Search(QKMatch -> DMatch -> CScore -> Rank) */
	/* QKMatch */
	CKI = QKMatch(QM, KM, realnum_key);
	for (int i = 0; i < (_msize(CKI) / sizeof(CKI[0])); i++)	cout << "CKI：" << CKI[i] << ",";
	cout << endl;

	/* DMatch */
	CFS = DMatch(CKI, _msize(CKI) / sizeof(CKI[0]));
	for (int i = 0; i < (_msize(CFS) / sizeof(CFS[0])); i++)	cout << "CFS：" << CFS[i] << ",";
	cout << endl;

	/* CSore */
	Cscore();

	/* Rank */
	Rank();

	/* Free Memory */
	SI.clear();
	file_index.clear();
	file_keyword.clear();
	RList.clear();

	system("pause");
	return 0;
}
