#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cmath>
#include <algorithm>



#include "user.h"
#include "types.h"
#include "markovTransferMatrix.h"




using namespace std;

unsigned int gsum = 0;
double latMin = 181, latMax = -181, lngMin = 181, lngMax = -181, dis = 0.03;

void getCheckinRecords(vector<checkinRecord>& checkinReocrds, const string& fileName);
void getCheckinRecords(vector<checkinRecord>& checkinReocrds, const char* fileName);
void initUsers(vector<user>& users, vector<checkinRecord>& checkinRecords);


int main(int argc, const char* argv[]) {
	vector<user> users;
	vector<checkinRecord> checkinRecords;

	getCheckinRecords(checkinRecords,"first_training.csv");
	initUsers(users, checkinRecords);

	system("pause");
	return 0;
}


void getCheckinRecords(vector<checkinRecord>& checkinReocrds, const string& fileName) {
	getCheckinRecords(checkinReocrds, fileName.c_str());
}
void getCheckinRecords(vector<checkinRecord>& checkinRecords, const char* fileName) {
	unsigned int xnum = 0;
	unsigned int ynum = 0;
	fstream fin;
	fin.open(fileName);
	assert(fin.is_open() && "open file error,check fileName is right.");
	
	//输入所有数据
	while (!fin.eof()) {
		unsigned int id;
		time_t timestamp;
		double lat;
		double lng;

		fin >> id >> timestamp >> lat >> lng;

		latMin = lat < latMin ? lat : latMin;
		latMax = lat > latMax ? lat : latMax;
		lngMin = lng < lngMin ? lng : lngMin;
		lngMax = lng > lngMax ? lng : lngMax;

		checkinRecords.push_back(checkinRecord(id, timestamp, lat, lng));
	}
	checkinRecords.pop_back();

	xnum = (latMax - latMin) / dis;
	ynum = (lngMax - lngMin) / dis;
	gsum = xnum * ynum;

	//计算每个签到数据的g_i
	for (auto i = checkinRecords.begin(); i != checkinRecords.end(); ++i) {
		i->g_i = abs(i->latLng.lat - latMin) / dis * ynum + abs(i->latLng.lng - lngMin);
	}

	//按照用户名从小到大排序
	sort(checkinRecords.begin(), checkinRecords.end(), [](const checkinRecord& a, const checkinRecord& b)->bool {return a.userID < b.userID; });
}

void initUsers(vector<user>& users, vector<checkinRecord>& checkinRecords) {
	//对每个用户中的签到数据按照时间排序
	for (auto beg = checkinRecords.begin(); beg != checkinRecords.end();) {
		auto end = beg + 1;
		
		//找到某用户所有的签到数据,先前已经按照用户排序过了
		while (end != checkinRecords.end() && beg->userID == end->userID) {
			++end;
		}
		sort(beg, end, [](const checkinRecord& a, const checkinRecord& b)->bool {return a.timestamp < b.timestamp; });

		//添加用户轨迹
		//[i,j)为同一天
		user newUser;
		newUser.setID(beg->userID);
		for (auto i = beg; i != end;) {
			auto j = i + 1;
			while(j != end) {
				if (j->date.tm_year == i->date.tm_year && j->date.tm_yday == i->date.tm_yday) {
					++j;
				}
				else {
					break;
				}
			}
			newUser.addTrace(make_pair(i, j));
			i = j;
		}

		users.push_back(newUser);
		beg = end;
	}
}