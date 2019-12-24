/*
*  实例化该类前请务必调用uesr::setPlaceNum来设置地点个数
*
*
*
*
*/
#ifndef _USER_H_
#define _USER_H_

#include <set>
#include <unordered_map>
#include <list>
#include <cmath>
#include <cassert>
#include <iterator>
#include <algorithm>

#include "bpr.h"
#include "types.h"
#include "logisticsRegression.h"
#include "MarkovTransferMatrix.h"

constexpr unsigned int INTERVAL_TIME_S = 1;
constexpr unsigned int INTERVAL_TIME_M = 60;
constexpr unsigned int INTERVAL_TIME_H = 60 * 60;
constexpr unsigned int INTERVAL_TIME_D = 60 * 60 * 24;

class User {
public:
    User();
    User(const User& a);
    ~User();

    /**
	* @brief      设置用户id
	* @param      i 用户id
	*/
    void setID(const unsigned int i);

    /**
	* @brief       设置数据集大小
	* @param  size 每个数据集的大小
	*/
    void setDataSize(const unsigned int size);

    /**
	* @brief      获取用户id
	* @return     用户id
	*/
    unsigned int getID();

    /**
    * @brief 实例化user前先调用该函数设置地点个数
    */
    static void setPlaceNum(const unsigned int _placeNum);

    /**
    * @brief 获取用户曾去过的地点
    */
    const std::list<unsigned int>& getHasArrivals() const;

    /**
    * @brief 获取用户每个点在历史签到轨迹中的概率的map
    */
    const std::unordered_map<unsigned int, double>& getPlaceProbability() const;

    /**
    * @brief 添加用户的亲密用户或者是相似用户
    */
    void addFriend(const User* fri);

    /**
    * @brief 获取用户的亲密用户或者是相似用户
    */
    const std::list<const User*>& getFirends() const;

    /**
    * @brief  添加签到轨迹
    */
    void addTrace(const std::list<CheckinRecord>::const_iterator& beg, const std::list<CheckinRecord>::const_iterator& end);
    void addTrace(const std::list<CheckinRecord>& a);

    /**
    * @brief 计算用户去过的地点,存在user::hasArrivals中
    *        在用户的数据输入完后调用
    */
    void calcUserHasArrivalsPlane();

    /**
	* @brief 计算用户去过的地点,存在user::hasArrivals中
	*        在用户的数据输入完后调用
	*/
    std::list<shochuAlgorithm::LogisticsRegression::TrainNode> getLRNode() const;

    /**
    * @brief 获取用户所有的lrnode
    */
    std::list<shochuAlgorithm::BPR::Triad> getBPRNode() const;

    static unsigned int getPlaceNum() {
        return placeNum;
    }

private:
    //用户id
    unsigned int id;

    //数据集大小
    unsigned int dataSetSize;

    //马尔可夫转移矩阵
	//transMatrix[i][j] 表示从i转移到j的概率
	shochuAlgorithm::MarkovTransferMatrix* transMatrix;

	//相似用户和亲密用户
	std::list<const User*> friends;

	//用户去过的地点
	std::list<unsigned int> hasArrivals;
	bool ifHasArrivalsIsCalc;

	//签到轨迹
	std::list<CheckinRecord> trace;

	//用户在历史签到轨迹里去过pair->first的概率为pair->second
	std::unordered_map<unsigned int, double> placeProbability;

	//地点个数
	static unsigned int placeNum;
	static bool placeNumSet;
};

#include "user.inl.hpp"

#endif // _USER_H_
