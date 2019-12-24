#ifndef _USER_ILN_HPP_
#define _USER_INL_HPP_

inline unsigned int User::getID() {
	return this->id;
}

inline const std::unordered_map<unsigned int, double>& User::getPlaceProbability() const {
	return this->placeProbability;
}

inline const std::list<unsigned int>& User::getHasArrivals() const {
	return this->hasArrivals;
}

inline const std::list<const User*>& User::getFirends() const {
	return this->friends;
}

#endif // !_USER_LNI_HPP_
