
#pragma once

#include "properties/WeightBase.hh"

class Weight : virtual public WeightBase {
	public: 
		virtual multiplier_t  value(const Properties&, Ex::iterator, const std::string& forcedlabel) const override;
		virtual bool          parse(const Properties&, keyval_t&) override;
		virtual std::string   unnamed_argument() const  override{ return "value"; };
		virtual std::string   name() const override;
	private:
		multiplier_t value_;
};

