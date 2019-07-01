#include "user.h"

user::user(const user& a) {
	this->friends = a.friends;
	this->id = a.id;
	this->trace = a.trace;
	this->transMatrix = a.transMatrix;
}

void user::setID(const unsigned int& i) {
	this->id = i;
}

const std::set<unsigned int>& user::getHasArrivals() const {
	return this->hasArrivals;
}

void user::addFriend(const user* fri) {
	this->friends.push_back(fri);
}

void user::addTrace(const std::pair<std::vector<checkinRecord>::iterator, std::vector<checkinRecord>::iterator>& a) {
	this->trace.push_back(a);
}
