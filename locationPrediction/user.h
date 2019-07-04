#ifndef _USER_H_
#define _USER_H_

#include <cmath>
#include <set>
#include <cassert>
#include <vector>
#include <algorithm>

#include "types.h"
#include "markovTransferMatrix.h"

constexpr unsigned int INTERVAL_TIME_S = 1;
constexpr unsigned int INTERVAL_TIME_M = 60;
constexpr unsigned int INTERVAL_TIME_H = 60 * 60;
constexpr unsigned int INTERVAL_TIME_D = 60 * 60 * 24;

class user {
private:
	//用户id
	unsigned int id;

	//马尔可夫转移矩阵
	//transMatrix[i][j] 表示从i转移到j的概率
	markovTransferMatrix transMatrix;

	//相似用户和亲密用户
	std::vector<const user*> friends;

	//用户去过的地点
	std::set<unsigned int> hasArrivals;
	bool ifHasArrivalsIsCalc;

	//轨迹
	//pair<vector<checkinRecord>::iterator, vector<checkinRecord>::iterator>
	//相当于指针，直接存储当前轨迹第一个签到点和最后一个签到点的地址,通过checkinRecords(main.cpp:32)给出
	//[first,second)
	std::vector<checkinRecord> trace;

	//地点个数
	static unsigned int placeNum;
	static bool placeNumSet;
public:
	user();
	user(const user& a);

	/**
	* @brief      设置用户id
	* @parameter  i 用户id
	*/
	void setID(const unsigned int& i);

	/**
	* @brief 实例化user前先调用该函数设置地点个数
	*/
	static void setPlaceNum(const unsigned int _placeNum);

	/**
	* @brief 获取用户曾去过的地点
	*/
	const std::set<unsigned int>& getHasArrivals() const;

	/**
	* @brief 添加用户的亲密用户或者是相似用户
	*/
	void addFriend(const user* fri);

	/**
	* @brief  添加签到轨迹
	*/
	void addTrace(const std::vector<checkinRecord>::iterator& beg, const std::vector<checkinRecord>::iterator& end);
	void addTrace(const std::vector<checkinRecord>& a);

	/**
	* @brief 计算用户去过的地点,存在user::hasArrivals中
	*        在用户的数据输入完后调用
	*/
	void calcUserHasArrivalsPlane();

	/**
	* @brief 获取用户所有的lrnode
	*/
	std::vector<lrNode> getLRNode() const;
};



#endif // _USER_H_
