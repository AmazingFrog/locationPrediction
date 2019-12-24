#include "user.h"

unsigned int User::placeNum = 0;
bool User::placeNumSet = false;

inline double sigmoid(double v) {
    return 1.0 / (1 + exp(-v));
}

User::User() {
	assert(User::placeNumSet && "placeNum not set.");
	this->transMatrix = new shochuAlgorithm::MarkovTransferMatrix_spzrseMatrix(User::placeNum);
	this->ifHasArrivalsIsCalc = false;
}

User::User(const User& a) {
	assert(User::placeNumSet && "placeNum not set.");
	this->friends = a.friends;
	this->id = a.id;
	this->trace = a.trace;
	this->placeProbability = a.placeProbability;
    this->transMatrix = new shochuAlgorithm::MarkovTransferMatrix_spzrseMatrix(User::placeNum);
	a.transMatrix->copyTo(*(this->transMatrix));
	this->ifHasArrivalsIsCalc = false;
}

User::~User() {
    this->transMatrix->clear();
    delete this->transMatrix;
}

void User::setID(const unsigned int i) {
	this->id = i;
}

void User::setDataSize(const unsigned int size) {
	this->dataSetSize = size;
	//this->
}

void User::setPlaceNum(const unsigned int _placeNum) {
	User::placeNum = _placeNum;
	User::placeNumSet = true;
}

void User::addFriend(const User* fri) {
    auto beg = this->friends.begin();
    for (; beg != this->friends.end(); ++beg) {
        if (*beg >= fri) {
            if (*beg == fri) {
                return;
            }
            this->friends.insert(beg, fri);
            return;
        }
    }
	this->friends.push_back(fri);
}

void User::addTrace(const std::list<CheckinRecord>::const_iterator& beg, const std::list<CheckinRecord>::const_iterator& end) {
	this->trace.insert(this->trace.cend(), beg, end);
	//std::sort(this->trace.begin(), this->trace.end(), [](const CheckinRecord& a, const CheckinRecord& b)->bool {return a.timestamp < b.timestamp; });
	this->trace.sort([](const CheckinRecord& a, const CheckinRecord& b)->bool {return a.timestamp < b.timestamp; });
    this->trace.unique([](const CheckinRecord& a, const CheckinRecord& b) {
        return a.g_i == b.g_i;
    });
	//计算每个地点在历史签到轨迹里的概率
	std::unordered_map<unsigned int, unsigned int> placeAppearNum;
	unsigned int checkinTotalNum = this->trace.size();
	for (auto i = this->trace.begin(); i != this->trace.end(); ++i) {
		++(placeAppearNum[i->g_i]);
	}
	for (auto i = placeAppearNum.begin(); i != placeAppearNum.end(); ++i) {
		this->placeProbability[i->first] = static_cast<double>(i->second) / checkinTotalNum;
	}
}
void User::addTrace(const std::list<CheckinRecord>& a) {
	this->addTrace(a.begin(), a.end());
}

void User::calcUserHasArrivalsPlane() {
	for (auto i = this->trace.begin(); i != this->trace.end(); ++i) {
		this->hasArrivals.push_back(i->g_i);
	}
    this->hasArrivals.sort([](unsigned int a, unsigned int b) {
        return a < b;
    });
    this->hasArrivals.unique();
	this->ifHasArrivalsIsCalc = true;
}

std::list<shochuAlgorithm::LogisticsRegression::TrainNode> User::getLRNode() const {
	double hod;
	double dow;
	double interval;
	double avgDistance;
	double accessRatio;
	double nr;
	double nod;
    double trueVal;
	unsigned int newPlace = 0;
	unsigned int alreadCalcFinishNum = 0;
	std::list<shochuAlgorithm::LogisticsRegression::TrainNode> ret;
	std::set<unsigned int> arrivals;
	int dirt[8][2] = { {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1} };

	alreadCalcFinishNum = 1;
	newPlace = 1;
	arrivals.insert(this->trace.begin()->g_i);
	for (auto i = std::next(this->trace.begin(), 1); i != this->trace.end(); ++i) {
			hod = static_cast<double>(i->date.tm_hour)/23;
			dow = static_cast<double>(i->date.tm_wday)/6;

			//两次签到时间差,单位秒
			interval = static_cast<double>(i->timestamp - (std::prev(i, 1))->timestamp) / INTERVAL_TIME_S;
            interval = sigmoid(interval);

			//avgDistance
			double sumDistance = 0;
			for (auto j = this->trace.begin(); j != i; ++j) {
				double temp = (i->latLng.lat - j->latLng.lat)*(i->latLng.lat - j->latLng.lat) + (i->latLng.lng - j->latLng.lng)*(i->latLng.lng - j->latLng.lng);
				sumDistance += sqrt(temp);
			}
			avgDistance = sumDistance / alreadCalcFinishNum;
            avgDistance = sigmoid(avgDistance);

			//accessRatio
			unsigned int accessPlaceNum = 0;
			//周围地点总数,比如g_i为0时,周围应该只有3个点可以访问的到,其余5个点超出范围
			unsigned int placeNum = 8;
			for (int j = 0; j < 8; ++j) {
				unsigned int nextX = i->x + dirt[j][0];
				unsigned int nextY = i->y + dirt[j][1];
				unsigned int nextG_I = nextX * CheckinRecord::ynum + nextY;
				//检查g_i是否超出范围
				if (nextG_I < 0 || nextG_I >= CheckinRecord::xnum*CheckinRecord::ynum) {
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
            nod = sigmoid(nod);

            //trueVal
            trueVal = arrivals.find(i->g_i) == arrivals.end() ? 0.0 : 1.0;

			arrivals.insert(i->g_i);
			++alreadCalcFinishNum;

            ret.push_back(shochuAlgorithm::LogisticsRegression::TrainNode(trueVal, { hod, dow, interval, avgDistance, accessRatio, nr, nod }));
	}

	return ret;
}

std::list<shochuAlgorithm::BPR::Triad> User::getBPRNode() const {
    std::list<shochuAlgorithm::BPR::Triad> ret;

    try {
        int* data = new int[User::placeNum];
        memset(data, 0, User::placeNum * sizeof(int));
        for (auto i = this->hasArrivals.begin(); i != this->hasArrivals.end(); ++i) {
            if (*i < User::placeNum) {
                data[*i] = BPR_CKICK_IN_YES;
            }
        }

        for (int i = 0; i < User::placeNum; ++i) {
            for (int j = i + 1; j < User::placeNum; ++j) {
                if (data[i] == BPR_CKICK_IN_YES && data[j] == BPR_CHICK_IN_NO) {
                    ret.emplace_back(this->id, i, j);
                }
            }
        }

        delete[] data;
    }
    catch (std::bad_alloc e) {
        std::cout << "don't have memory" << std::endl;
    }
    
    return ret;
}