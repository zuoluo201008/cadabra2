
#pragma once

#include "Props.hh"

class CommutingBehaviour : virtual public list_property {
	public:
		virtual int sign() const=0;
		virtual match_t equals(const property *) const;
};
