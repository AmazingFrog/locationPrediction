#ifndef _TYPES_H_
#define _TYPES_H_

#include <ctime>

//经纬度
struct latlng{
	double lat;
	double lng;

	latlng() = default;
	latlng(double _lat,double _lng):lat(_lat),lng(_lng){}
};

//签到数据
struct checkinRecord {
	unsigned int userID;

	//签到日期的时间戳
	time_t timestamp;

	latlng latLng;

	tm date;
	//签到位置g_i
	//从0开始
	unsigned int g_i;
	unsigned int x;
	unsigned int y;

	static unsigned int xnum;
	static unsigned int ynum;

	checkinRecord() = default;
	checkinRecord(const unsigned int id, const time_t _timestamp, const latlng& _latlng) :
		userID(id),
		timestamp(_timestamp),
		latLng(_latlng),
		date(*gmtime(&_timestamp)){}
	checkinRecord(const unsigned int id, const time_t _timestamp, const double lat, const double lng) :
		userID(id),
		timestamp(_timestamp),
		latLng(latlng(lat,lng)),
		date(*gmtime(&_timestamp)){}
};

//逻辑回归训练数据
struct lrNode {
	//签到数据的小时数
	//[0,23]
	double hod;
	//签到数据的天数
	//[0,6]
	double dow;
	//两次签到的时间间隔
	double interval;
	//平均距离
	double avgDistance;
	//被访率
	double accessRatio;
	//新颖率
	double nr;
	//签到总天数
	double nod;
	lrNode() = default;
	lrNode(double _hod,double _dow,double _interval,double _avgDistance,double _accessRatio,double _nr,double _nod) :
		hod(_hod),
		dow(_dow),
		interval(_interval),
		avgDistance(_avgDistance),
		accessRatio(_accessRatio),
		nr(_nr),
		nod(_nod){}
};

#endif // _TYPES_H_
