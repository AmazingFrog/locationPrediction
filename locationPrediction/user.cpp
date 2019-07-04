#include "user.h"

unsigned int user::placeNum = 0;
bool user::placeNumSet = false;

user::user() {
	assert(user::placeNumSet && "placeNum not set.");
	this->transMatrix.create(user::placeNum);
	this->ifHasArrivalsIsCalc = false;
}

user::user(const user& a) {
	assert(user::placeNumSet && "placeNum not set.");
	this->friends = a.friends;
	this->id = a.id;
	this->trace = a.trace;
	a.transMatrix.copyTo(this->transMatrix);
	this->ifHasArrivalsIsCalc = false;
}

void user::setID(const unsigned int& i) {
	this->id = i;
}

void user::setPlaceNum(const unsigned int _placeNum) {
	user::placeNum = _placeNum;
	user::placeNumSet = true;
}

const std::set<unsigned int>& user::getHasArrivals() const {
	return this->hasArrivals;
}

void user::addFriend(const user* fri) {
	this->friends.push_back(fri);
}

void user::addTrace(const std::vector<checkinRecord>::iterator& beg, const std::vector<checkinRecord>::iterator& end) {
	this->trace.insert(this->trace.cend(), beg, end);
	std::sort(this->trace.begin(), this->trace.end(), [](const checkinRecord& a, const checkinRecord& b)->bool {return a.timestamp < b.timestamp; });
}
void user::addTrace(const std::vector<checkinRecord>& a) {
	for (auto i = a.begin(); i != a.end(); ++i) {
		this->trace.push_back(*i);
	}
	std::sort(this->trace.begin(), this->trace.end(), [](const checkinRecord& a, const checkinRecord& b)->bool {return a.timestamp < b.timestamp; });
}

void user::calcUserHasArrivalsPlane() {
	for (auto i = this->trace.begin(); i != this->trace.end(); ++i) {
		this->hasArrivals.insert(i->g_i);
	}
	this->ifHasArrivalsIsCalc = true;
}

std::vector<lrNode> user::getLRNode() const {
	double hod;
	double dow;
	double interval;
	double avgDistance;
	double accessRatio;
	double nr;
	double nod;
	unsigned int newPlace = 0;
	unsigned int alreadCalcFinishNum = 0;
	std::vector<lrNode> ret;
	std::set<unsigned int> arrivals;
	int dirt[8][2] = { {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1} };

	alreadCalcFinishNum = 1;
	newPlace = 1;
	arrivals.insert(this->trace.begin()->g_i);
	for (auto i = this->trace.begin() + 1; i != this->trace.end(); ++i) {
			hod = static_cast<double>(i->date.tm_hour)/23;
			dow = static_cast<double>(i->date.tm_wday)/6;

			//两次签到时间差,单位秒
			interval = static_cast<double>(i->timestamp - (i - 1)->timestamp) / INTERVAL_TIME_S;

			//avgDistance
			double sumDistance = 0;
			for (auto j = this->trace.begin(); j != i; ++j) {
				double temp = (i->latLng.lat - j->latLng.lat)*(i->latLng.lat - j->latLng.lat) + (i->latLng.lng - j->latLng.lng)*(i->latLng.lng - j->latLng.lng);
				sumDistance += sqrt(temp);
			}
			avgDistance = sumDistance / alreadCalcFinishNum;

			//accessRatio
			unsigned int accessPlaceNum = 0;
			//周围地点总数,比如g_i为0时,周围应该只有3个点可以访问的到,其余5个点超出范围
			unsigned int placeNum = 8;
			for (int j = 0; j < 8; ++j) {
				unsigned int nextX = i->x + dirt[j][0];
				unsigned int nextY = i->y + dirt[j][1];
				unsigned int nextG_I = nextX * checkinRecord::ynum + nextY;
				//检查g_i是否超出范围
				if (nextG_I < 0 || nextG_I >= checkinRecord::xnum*checkinRecord::ynum) {
					--placeNum;
				}
				//不超出范围的话才检查是否去过
				else if (arrivals.find(nextG_I) != arrivals.end()) {
					++accessPlaceNum;
				}
			}
			accessRatio = accessPlaceNum * 1.0 / placeNum;

			//nr
			if (arrivals.find(i->g_i) != arrivals.end()) {
				++newPlace;
			}
			nr = newPlace * 1.0 / (alreadCalcFinishNum + 1);

			//nod
			nod = static_cast<double>(i->timestamp - this->trace.begin()->timestamp) / INTERVAL_TIME_D;

			arrivals.insert(i->g_i);
			++alreadCalcFinishNum;

			ret.emplace_back(hod, dow, interval, avgDistance, accessRatio, nr, nod);
	}

	return ret;
}