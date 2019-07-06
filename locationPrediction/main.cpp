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
#include "bpr.h"
#include "logisticsRegression.h"
#include "markovTransferMatrix.h"


using namespace std;
using namespace shochuAlgorithm::LogisticsRegression;
using namespace shochuAlgorithm::BPR;


unsigned int gsum = 0;
double latMin = 181, latMax = -181, lngMin = 181, lngMax = -181, dis = 0.03;
unsigned int checkinRecord::xnum = 0;
unsigned int checkinRecord::ynum = 0;

void getCheckinRecords(vector<checkinRecord>& checkinReocrds, const string& fileName);
void getCheckinRecords(vector<checkinRecord>& checkinReocrds, const char* fileName);
void initUsers(vector<user>& users, vector<checkinRecord>& checkinRecords);
void getLRNodesFromUsers(const vector<user>& users, vector<TrainNode>& lrTrainSet);

int main(int argc, const char* argv[]) {
	user::setPlaceNum(10);
	vector<user> users;
	vector<checkinRecord> checkinRecords;
	vector<TrainNode> lrTrainSet;
    LogisticsRegression_impl lr;

	getCheckinRecords(checkinRecords,"first_training.csv");
	initUsers(users, checkinRecords);
	getLRNodesFromUsers(users, lrTrainSet);

    lr.add(lrTrainSet);
    lr.train();

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

	xnum = static_cast<unsigned int>((latMax - latMin) / dis);
	ynum = static_cast<unsigned int>((lngMax - lngMin) / dis);
	gsum = xnum * ynum;
	checkinRecord::xnum = xnum;
	checkinRecord::ynum = ynum;

	//计算每个签到数据的g_i
	for (auto i = checkinRecords.begin(); i != checkinRecords.end(); ++i) {
		i->x = static_cast<unsigned int>(abs(i->latLng.lat - latMin) / dis);
		i->y = static_cast<unsigned int>(abs(i->latLng.lng - lngMin) / dis);
		i->g_i = i->x * ynum + i->y;
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

		//添加用户轨迹
		//[i,j)为同一天
		user newUser;
		newUser.setID(beg->userID);
		newUser.addTrace(beg, end);

		users.push_back(newUser);
		beg = end;
	}

	//计算每个用户自己的已到过的地点
	for (auto i = users.begin(); i != users.end(); ++i) {
		i->calcUserHasArrivalsPlane();
	}

}

void getLRNodesFromUsers(const vector<user>& users, vector<TrainNode>& lrTrainSet) {
	for (auto i = users.begin(); i != users.end(); ++i) {
		std::vector<TrainNode> add(i->getLRNode());
		lrTrainSet.insert(lrTrainSet.end(), add.begin(), add.end());
	}
}


