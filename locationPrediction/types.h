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

#endif // _TYPES_H_
