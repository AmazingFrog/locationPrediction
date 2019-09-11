#include <list>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "bpr.h"
#include "user.h"
#include "types.h"
#include "logisticsRegression.h"
#include "MarkovTransferMatrix.h"

#include <crtdbg.h>

using namespace std;
using namespace shochuAlgorithm::BPR;
using namespace shochuAlgorithm::LogisticsRegression;

unsigned int gsum = 0;
double latMin = 181, latMax = -181, lngMin = 181, lngMax = -181, dis = 0.03;
unsigned int CheckinRecord::xnum = 0;
unsigned int CheckinRecord::ynum = 0;

void getCheckinRecords(list<CheckinRecord>& checkinReocrds, const char* fileName);
void initUsers(list<User>& users, list<CheckinRecord>& checkinRecords,const char* friendsInputFile,double friendIntimacyThreshold = 0.7,double userSimilarityThreshold = 0.7);
void getLRNodesFromUsers(const list<User>& users, list<TrainNode>& lrTrainSet);
void getBPRNodesFromUsers(const list<User>& users, list<shochuAlgorithm::BPR::Triad>& bprTrainSet);

int main(int argc, const char* argv[]) {
	//设置必需参数
    shochuAlgorithm::LogisticsRegression::featureNum = 7;
    ios_base::sync_with_stdio(false);

	list<User> users;
	list<CheckinRecord> checkinRecords;
	list<TrainNode> lrTrainSet;
    list<shochuAlgorithm::BPR::Triad> bprTrainSet;
    LogisticsRegression_impl lr;
	shochuAlgorithm::BPR::Bpr_impl bpr;
    vector<double> model;

	getCheckinRecords(checkinRecords,"first_training.csv");
	initUsers(users, checkinRecords,"edges_NY.csv");
	//getLRNodesFromUsers(users, lrTrainSet);
    cout << "get bpr" << endl;
    system("pause");
    //getBPRNodesFromUsers(users, bprTrainSet);
    
    /*
    int* data = new int[users.rbegin()->getID() * User::getPlaceNum()];
    for (auto i = bprTrainSet.begin(); i != bprTrainSet.end(); ++i) {
        data[i->u*User::getPlaceNum() + i->i] = BPR_CKICK_IN_YES;
        data[i->u*User::getPlaceNum() + i->j] = BPR_CHICK_IN_NO;
    }
    */

    /*
    bpr.setTrainSetSize(20);
    bpr.add(data, users.rbegin()->getID(), User::getPlaceNum());
    delete[] data;
    bpr.train(0.01);
    bpr.saveModel("bpr.txt");
    */

    /*
    lr.add(lrTrainSet);
    model = lr.train();
    for (auto i = model.begin(); i != model.end(); ++i) {
        cout << *i << endl;
    }
    lr.save("model.txt");
    */

	system("pause");
	return 0;
}

void getCheckinRecords(list<CheckinRecord>& checkinRecords, const char* fileName) {
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

        checkinRecords.emplace_back(id, timestamp, lat, lng);
	}
	checkinRecords.pop_back();

	xnum = static_cast<unsigned int>((latMax - latMin) / dis);
	ynum = static_cast<unsigned int>((lngMax - lngMin) / dis);
	gsum = xnum * ynum;
	CheckinRecord::xnum = xnum;
	CheckinRecord::ynum = ynum;
	User::setPlaceNum(gsum);

	//计算每个签到数据的g_i
	for (auto i = checkinRecords.begin(); i != checkinRecords.end(); ++i) {
		i->x = static_cast<unsigned int>(abs(i->latLng.lat - latMin) / dis);
		i->y = static_cast<unsigned int>(abs(i->latLng.lng - lngMin) / dis);
		i->g_i = i->x * ynum + i->y;
	}

	//按照用户名从小到大排序
	//sort(checkinRecords.begin(), checkinRecords.end(), [](const CheckinRecord& a, const CheckinRecord& b)->bool {return a.userID < b.userID; }); 
	checkinRecords.sort([](const CheckinRecord& a, const CheckinRecord& b)->bool {return a.userID < b.userID; });
}

void initUsers(list<User>& users, list<CheckinRecord>& checkinRecords, const char* friendsInputFile, double friendIntimacyThreshold,double userSimilarityThreshold) {
	//对每个用户中的签到数据按照时间排序
	for (auto beg = checkinRecords.begin(); beg != checkinRecords.end();) {
		auto end = std::next(beg, 1);
		
		//找到某用户所有的签到数据,先前已经按照用户排序过了
		while (end != checkinRecords.end() && beg->userID == end->userID) {
			++end;
		}

		//添加用户轨迹
		User newUser;
		newUser.setID(beg->userID);
		newUser.addTrace(beg, end);

		users.push_back(newUser);
		beg = end;
	}
    checkinRecords.clear();

	//计算每个用户自己的已到过的地点
	for (auto i = users.begin(); i != users.end(); ++i) {
		i->calcUserHasArrivalsPlane();
	}
	
	//输入好友
	fstream friendsIn(friendsInputFile);
	if (!friendsIn.is_open()) {
		cout << "open firends file error" << endl;
		return;
	}

	list<pair<unsigned int, unsigned int> > inputDataTemp;
	while (!friendsIn.eof()) {
		unsigned int u = 0;
		unsigned int f = 0;
		char t = 0;
		friendsIn >> u >> t >> f;
		inputDataTemp.emplace_back(u, f);
	}
	friendsIn.close();
	inputDataTemp.pop_back();

	//输入的好友数据按照第一个数据排序
	//sort(inputDataTemp.begin(), inputDataTemp.end(), [](const pair<unsigned int, unsigned int>& a, const pair<unsigned int, unsigned int>& b)->bool {return a.first < b.first; }); 
	inputDataTemp.sort([](const pair<unsigned int, unsigned int>& a, const pair<unsigned int, unsigned int>& b)->bool {return a.first < b.first; });
	
	auto user = users.begin();
	auto userBackup = user;
	for (auto i = inputDataTemp.begin(); i != inputDataTemp.end(); ++i) {\
        // u f 为一对好友关系
		unsigned int u = i->first;
		unsigned int f = i->second;
		//从users中找到用户u
		if (u != user->getID()) {
			userBackup = user;
			do {
				++user;
			} while (user != users.end() && u != user->getID());
		}
		//跳过签到数据中没有的用户 的好友关系
		if (user == users.end()) {
			do {
				++i;
			} while (i != inputDataTemp.end() && i->first == u);
			--i;
			user = userBackup;
			continue;
		}
		//找到好友f的迭代器
		auto k = users.begin();
		while (k != users.end() && k->getID() != f) {
			++k;
		}
		if (k != users.end()) {
			user->addFriend(&(*k));
		}
	}
    inputDataTemp.clear();

    //计算亲密好友
	for (auto i = users.begin(); i != users.end(); ++i) {
		for (auto j = users.begin(); j != users.end(); ++j) {
			if (i == j) {
				continue;
			}
			unsigned int iFriendsNum = i->getFirends().size();
			unsigned int jFriendsNum = j->getFirends().size();
			unsigned int minFriendsNum = iFriendsNum < jFriendsNum ? iFriendsNum : jFriendsNum;
            if (minFriendsNum == 0) {
                continue;
            }

			vector<const User*> commonFriends(minFriendsNum);
            vector<const User*>::iterator it;
			//求两人的好友交集
            it = set_intersection(i->getFirends().begin(), i->getFirends().end(), j->getFirends().begin(), j->getFirends().end(), commonFriends.begin());
			//it - commonFirends.begin() 共同好友个数
			double friendIntimacy = static_cast<double>(it - commonFriends.begin()) / minFriendsNum;
			if (friendIntimacy >= friendIntimacyThreshold) {
                i->addFriend(&(*j));
			}
		}
	}
	
	//计算相似用户
	for (auto i = users.begin(); i != users.end(); ++i) {
		for (auto j = users.begin(); j != users.end(); ++j) {
			if (i == j) {
				continue;
			}
			unsigned int iHasArrivalNum = i->getHasArrivals().size();
			unsigned int jHasArrivalNum = j->getHasArrivals().size();
			unsigned int minArrivalNum = iHasArrivalNum < jHasArrivalNum ? iHasArrivalNum : jHasArrivalNum;

            if (minArrivalNum == 0) {
                continue;
            }
			vector<unsigned int> commonPlace(minArrivalNum);
			vector<unsigned int>::iterator it;
			it = set_intersection(i->getHasArrivals().begin(), i->getHasArrivals().end(), j->getHasArrivals().begin(), j->getHasArrivals().end(), commonPlace.begin());
			commonPlace.resize(it - commonPlace.begin());
			double a = 0;
			double b = 0;
			double c = 0;
			//$Usimi(i, j) = \frac{ \sum_{g \in U_{i,j}} {P_{i \rightarrow g} \cdot P_{j \rightarrow g}} }{\sqrt{ \sum_{g \in U_{i,j}{(P_{i \rightarrow g}) ^ 2}} } \sqrt{ \sum_{g \in U_{i,j}{(P_{j \rightarrow g}) ^ 2}} }} $
			for (auto k = commonPlace.begin(); k != commonPlace.end(); ++k) {
				//分子
				a += (i->getPlaceProbability().at(*k)) * (j->getPlaceProbability().at(*k));
				//分母
				b += (i->getPlaceProbability().at(*k)) * (i->getPlaceProbability().at(*k));
				c += (j->getPlaceProbability().at(*k)) * (j->getPlaceProbability().at(*k));
			}
			double UserSimilarity = a / sqrt(b*c);
			if (UserSimilarity >= userSimilarityThreshold) {
				i->addFriend(&(*j));
			}
		}
	}
}

void getLRNodesFromUsers(const list<User>& users, list<TrainNode>& lrTrainSet) {
	for (auto i = users.begin(); i != users.end(); ++i) {
		std::list<TrainNode> add(i->getLRNode());
		lrTrainSet.insert(lrTrainSet.end(), add.begin(), add.end());
	}
}

void getBPRNodesFromUsers(const list<User>& users, list<shochuAlgorithm::BPR::Triad>& bprTrainSet) {
    for (auto currentUser = users.begin(); currentUser != users.end(); ++currentUser) {
        auto add = currentUser->getBPRNode();
        bprTrainSet.insert(bprTrainSet.end(), add.begin(), add.end());
    }
}
