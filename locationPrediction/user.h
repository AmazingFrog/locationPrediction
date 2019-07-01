#ifndef _USER_H_
#define _USER_H_

#include <set>
#include <vector>

#include "types.h"
#include "markovTransferMatrix.h"

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

	//轨迹
	//pair<vector<checkinRecord>::iterator, vector<checkinRecord>::iterator>
	//相当于指针，直接存储当前轨迹第一个签到点和最后一个签到点的地址,通过checkinRecords(main.cpp:32)给出
	//[first,second)
	std::vector<std::pair<std::vector<checkinRecord>::iterator, std::vector<checkinRecord>::iterator> > trace;

public:
	user() = default;
	user(const user& a);

	void setID(const unsigned int& i);

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
	void addTrace(const std::pair<std::vector<checkinRecord>::iterator, std::vector<checkinRecord>::iterator>& a);

};



#endif // _USER_H_
