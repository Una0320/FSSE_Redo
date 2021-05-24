#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
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

/* Let program run 10 times */
string exampleset[10][10] = {
	{ "iot" },
	{ "iot", "web" },
	{ "iot", "web", "controller" },
	{ "iot", "web", "controller", "vnf" },
	{ "iot", "web", "controller", "vnf", "routing" },
	{ "iot", "web", "controller", "vnf", "routing", "network" },
	{ "iot", "web", "controller", "vnf", "routing", "network", "topology" },
	{ "iot", "web", "controller", "vnf", "routing", "network", "topology", "delay" },
	{ "iot", "web", "controller", "vnf", "routing", "network", "topology", "delay", "servers" },
	{ "iot", "web", "controller", "vnf", "routing", "network", "topology", "delay", "servers", "cloud" }
};
int example_flag = 0;
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
	realnum_que = 0;
	while (cin >> queryword[realnum_que])
	{
		fingerprint_q.push_back(fingerprint(queryword[realnum_que]));
		realnum_que++;
	}
	cout << "End of input query word" << endl << endl;
	//getchar();		//Get char，because of Ctrl+Z
}

void Query2()
{
	cout << "Input Query word by Example Set" << endl;
	realnum_que = 0;
	for (int i = 0; i < example_flag + 1; i++)
	{
		queryword[i] = exampleset[example_flag][i];
		fingerprint_q.push_back(fingerprint(queryword[i]));
		realnum_que++;
	}
	example_flag++;
}
/*Get Dataset from csv*/
void readDataset()
{
	ifstream inFile("paper_keyword.csv", ios::in);		//Build File Object，File object call member function
	if (inFile.is_open())	cout << "Open_successfully" << endl << endl;
	else					cout << "Fail" << endl << endl;

	string lineStr;

	while (getline(inFile, lineStr))
	{
		//cout << lineStr << endl;

		stringstream ss(lineStr);
		string str;
		
		getline(ss, str, ',');
		temp_index = str;
		paper_count++;

		getline(ss, str, ',');
		temp_title = str;
		file_index.insert(pair<string, string>(temp_index, temp_title));

		getline(ss, str, ',');
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[0] = str;


		getline(ss, str, ',');
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[1] = str;

		getline(ss, str, ',');
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		temp_k.keyword[2] = str;

		getline(ss, str, ',');
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

	for (int i = 0; i<realnum_key; i++)				//Remove Duplicates From Dictionary
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
	for (int i = 0; i < 128; i++)	if (a[i] != b[i])	HD++;
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

	for (int i = 0; i < realnum_key; i++)			//Compare Dictionary and Queryword；If(Keywrod == Query)，QM = 1
	{
		for (int j = 0; j < realnum_que; j++)
		{
			if (!(Dictionary[i].compare(queryword[j]))) temp_QM[i] = 1;
		}
	}
	cout << endl;
	
	setQKM(temp_KM, temp_QM);
	free(temp_KM);
	free(temp_QM);
}

/* Set SI */
void BuildSI()
{
	//<map>file_keyword：file_id、paper(file)'s keyword*4
	//SI：<Dictionary(keyword), id_sc(file_id, file's score)>
	for (auto it = file_keyword.begin(); it != file_keyword.end(); it++)	//Put keyword into Dictionary
	{
		int file_i = atoi((it->first).c_str());								//file_id(string->int)
		double   x = (5 - 1) * rand() / (RAND_MAX + 1.0) + 1;					//random score(range：5~1)
		temp_idcs.id = it->first;											//put file_id into temp_idcs(struct,id)

		for (int j = 0; j < 4; j++)
		{
			if (j == 0)
			{
				temp_idcs.score = x;										//put random score into temp_idcs(struct,score)
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));		//insert(file_id<string>, word's score<double>)
			}
			else if (j == 1)
			{
				temp_idcs.score = x;
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));
			}
			else if (j == 2)
			{
				temp_idcs.score = x;
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));
			}
			else
			{
				temp_idcs.score = 1.7;
				SI.insert(pair<string, id_sc>(it->second.keyword[j], temp_idcs));
			}
		}

	}
	/* Print map--SI */
	//for (auto it = SI.begin(); it != SI.end(); it++)	cout << it->first << " " << it->second.id << " , " << it->second.score << " " << endl;
}

/* Function 1 -- QKMatch */
int* QKMatch(int* QM, int* KM, int len) {	//len is the length of KM

	int tempsize = realnum_key;
	int CKI[100] = { 0 };
	int j = 0;

	for (int i = 0; i<len; i++)
		if (QM[i] == KM[i] && KM[i] == 1)	CKI[j++] = i;

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
		multimap<string, id_sc>::iterator m;
		m = SI.find(Dictionary[CKI[i]]);
		for (int k = 0; k != SI.count(Dictionary[CKI[i]]); k++, m++)
		{
			cout << m->first << "\t-- " << m->second.id << endl;
			CFS[j++] = atoi((m->second.id).c_str());
		}
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
		for (int j = 0; j < _msize(CKI) / sizeof(CKI[0]); j++)
		{
			//SI：<Dictionary(keyword), id_sc(file_id, file's score)>
			//file_keyword -- file_id、paper(file)'s keyword*4
			multimap<string, id_sc>::iterator m;
			m = SI.find(Dictionary[CKI[j]]);
			for (int k = 0; k != SI.count(Dictionary[CKI[j]]); k++, m++)
			{
				if (m->second.id == current_fileid) total_score += m->second.score;
			}
		}
		RList.insert(pair<string, double>(current_fileid, total_score));
	}
	cout << endl << "RList：" << endl;
	for (auto it = RList.begin(); it != RList.end(); it++)	cout << it->first << "\t" << it->second << endl << endl;
}

/* Rank，cannot use map<value> sort, so need other function */
void printVec(vector< pair<string, double> > &vec) {
	vector<pair<string, double> >::iterator it;
	for (it = vec.begin(); it != vec.end(); ++it) {
		//cout << it->first << " : " << it->second << endl;
		cout << "Paper：" << file_index.at(it->first) << endl;
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
	clock_t query_time, builddata_time;
	
	/* Get Dataset from csv */
	readDataset();

	/* Build SI */
	BuildSI();
	builddata_time = clock();
	cout << "Build SI Time：" << double(builddata_time) / CLOCKS_PER_SEC << "s" << endl;

	for (int times = 1; times <= 10; times++)
	{
		clock_t start, finish;
		start = clock();
		Query2();

		/* Generate KM、QM */
		GenerateKQM();
		/*for (int i = 0; i<realnum_key; i++) cout << KM[i];
		cout << endl;*/
		//for (int i = 0; i<realnum_key; i++)	cout << QM[i];
		cout << endl;



		/* Search(QKMatch -> DMatch -> CScore -> Rank) */
		/* QKMatch */
		CKI = QKMatch(QM, KM, realnum_key);
		//for (int i = 0; i < (_msize(CKI) / sizeof(CKI[0])); i++)	cout << "\nCKI：" << CKI[i] << ",";
		//cout << endl;

		/* DMatch */
		CFS = DMatch(CKI, _msize(CKI) / sizeof(CKI[0]));
		//for (int i = 0; i < (_msize(CFS) / sizeof(CFS[0])); i++)	cout << "\nCFS：" << CFS[i] << ",";
		//cout << endl;

		/* CSore */
		Cscore();

		/* Rank */
		Rank();

		finish = clock();
		cout << "Query Time " << times << "：" << double(finish-start) / CLOCKS_PER_SEC << "s" << endl;
	}
	/* User Input query word */
	
	
	/* Free Memory */
	SI.clear();
	file_index.clear();
	file_keyword.clear();
	RList.clear();

	
	system("pause");
	return 0;
}