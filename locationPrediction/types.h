#ifndef _TYPES_H_
#define _TYPES_H_

#include <ctime>

//经纬度
struct LatLng{
	double lat;
	double lng;

	LatLng() = default;
	LatLng(double _lat,double _lng):lat(_lat),lng(_lng){}
};

//签到数据
struct CheckinRecord {
	unsigned int userID;

	//签到日期的时间戳
	time_t timestamp;

	LatLng latLng;

	tm date;
	//签到位置g_i
	//从0开始
	unsigned int g_i;
	unsigned int x;
	unsigned int y;

	static unsigned int xnum;
	static unsigned int ynum;

	CheckinRecord() = default;
	CheckinRecord(const unsigned int id, const time_t _timestamp, const LatLng& _latlng) :
		userID(id),
		timestamp(_timestamp),
		latLng(_latlng),
		date(*gmtime(&_timestamp)){}
	CheckinRecord(const unsigned int id, const time_t _timestamp, const double lat, const double lng) :
		userID(id),
		timestamp(_timestamp),
		latLng(LatLng(lat,lng)),
		date(*gmtime(&_timestamp)){}
};

#endif // _TYPES_H_
