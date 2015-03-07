
#include "Compare.hh"
#include "Cleanup.hh"
#include "algorithms/product_rule.hh"
#include "properties/Derivative.hh"

product_rule::product_rule(Kernel& k, exptree& tr)
	: Algorithm(k, tr), number_of_indices(0)
	{
	}

//
//  A(b*c*d)     -> A(b)*c*d + b*A(c)*d + b*c*A(d)
//  A(b*c)(e*f) 1-> A(b)(e*f)*c + b*A(c)(e*f)
//  A(b*c)(e*f) 2-> A(b*c)(e)*f + e*A(b*c)(f)

bool product_rule::can_apply(iterator it)
	{
	const Derivative *der=kernel.properties.get<Derivative>(it);
	if(der || *it->name=="\\cdb_Derivative") {
		prodnode=tr.end();
		number_of_indices=0;
		if(tr.number_of_children(it)>0) {
			sibling_iterator ch=tr.begin(it);
			while(ch!=tr.end(it)) {
				 if(prodnode==tr.end() && ( *ch->name=="\\prod" || *ch->name=="\\pow") )
					prodnode=ch; // prodnode contains the first product node, there may be more
				else {
					if(ch->is_index()) ++number_of_indices;
					}
				++ch;
				}
			if(prodnode!=tr.end()) return true;
			}
		}
	return false;
	}

Algorithm::result_t product_rule::apply(iterator& it)
	{
	exptree rep; // the subtree storing the result
	iterator sm; // the sum node inside 'rep'

	// If we are distributing a multiple derivative, take out all
	// indices except the last one, and wrap things in a new derivative
	// node which has these indices as child nodes.
	bool indices_at_front=true;
	if(number_of_indices>1) {
		rep.set_head(str_node(it->name));
		sm=rep.append_child(rep.begin(),str_node("\\sum"));
		sibling_iterator sib=tr.begin(it);
		while(sib->is_index()==false) {
			indices_at_front=false;
			++sib;  // find first index
			}
		++sib;

	HERE: should find _last_ index to act with, not the first.

		while(sib!=tr.end(it)) { // move all other indices away FIXME: wrong order
			if(sib->is_index()) {
				sibling_iterator nxt=sib;
				++nxt;
				if(indices_at_front) rep.move_before(sm, (iterator)sib);
				else                 rep.move_after(sm, (iterator)sib);
				sib=nxt;
				}
			else ++sib;
			}
		}
	else {
		sm=rep.set_head(str_node("\\sum"));
		if(tr.begin(it)->is_index()==false)
			indices_at_front=false;
		}


	// Two cases to handle now: D(A**n) -> n D(A**(n-1)) and
	//                          D(A*B)  -> D(A)*B + A*D(B) 
	// both suitably generalised to anti-commuting derivatives.

	if(*prodnode->name=="\\pow") {
		 sibling_iterator ar=tr.begin(prodnode);
		 sibling_iterator pw=ar;
		 ++pw;
		 sm->name=name_set.insert("\\prod").first;
		 if(pw->is_integer()) 
			  multiply(sm->multiplier, *pw->multiplier);
		 else rep.append_child(sm, (iterator)pw);

		 // \partial(A**n)
		 iterator pref=rep.append_child(sm, iterator(prodnode));  // add A**n
		 iterator theD=rep.append_child(sm, it);                  // add \partial_{m}(A**n)
		 sibling_iterator repch=tr.begin(theD);                   // convert to \partial_{m}(A)
		 while(*repch->name!="\\pow") 
			  ++repch;
		 sibling_iterator pw2=tr.begin(repch);
		 rep.move_before(repch, pw2);
//		 txtout << "after rep.move_before" << std::endl;
//		 tr.print_recursive_treeform(txtout, rep.begin());
		 rep.erase(repch);
//		 txtout << "after rep.erase" << std::endl;
//		 tr.print_recursive_treeform(txtout, rep.begin());

		 if(pw->is_integer()) {                                   // A**n -> A**(n-1)
			  if(*pw->multiplier==2) {
					iterator nn=rep.move_after(pref, iterator(tr.begin(pref)));
					multiply(nn->multiplier, *pref->multiplier);
					rep.erase(pref);
					}
			  else {
					pw2=tr.begin(pref);
					++pw2;
					add(pw2->multiplier, -1);
					}
//			  txtout << "after all done " << std::endl;
//			  tr.print_recursive_treeform(txtout, rep.begin());
			  }
		 else {
			  pw2=tr.begin(pref);
			  ++pw2;
			  if(*pw2->name=="\\sum") {
					iterator tmp=rep.append_child(pw2, str_node("1"));
					tmp->fl.bracket=rep.begin(pw2)->fl.bracket;
					multiply(tmp->multiplier, -1);
					}
			  else {
					iterator sumn=rep.wrap(pw2, str_node("\\sum"));	
					iterator tmp=rep.append_child(sumn, str_node("1"));
					multiply(tmp->multiplier, -1);
					}
			  }

		 iterator top=rep.begin();
		 cleanup_dispatch(kernel, tr, top);
		 }
	else {
		 // replace the '\diff' with a '\sum' of diffs.
		 unsigned int num=0;
		 sibling_iterator chl=tr.begin(prodnode); // pointer to current factor in the product
		 int sign=1; // keep track of a sign for anti-commuting derivatives

		 // In order to figure out whether a derivative is anti-commuting with
		 // a given object in the product on which it acts, we need to consider
		 // a number of cases:
		 //
		 //    D_{a}{\theta^{b}}                    with \theta^{a} Coordinate & SelfAntiCommuting
       //    D_{\theta^{a}}{\theta^{b}}           with \theta^{a} Coordinate and a,b,c AntiCommuting
		 //    D_{a}{D_{b}{G}}                      handled by making indices AntiCommuting.
		 //    D_{a}{D_{\dot{b}}{G}}                handled by making indices AntiCommuting.
		 //    D_{a}{T^{a b}}                       handled by making indices AntiCommuting.
		 //    D_{a}{\theta}                        with \theta having an ImplicitIndex of type 'a' 
		 //    D{ A B }                             not yet handled (problem is to give scalar anti-commutativity
       //                                            property to D, A, B).

		 exptree_comparator comp(kernel.properties);

		 while(chl!=tr.end(prodnode)) { // iterate over all factors in the product
			  // Add the whole product node to the replacement sum.
			  iterator dummy=rep.append_child(sm);
			  dummy=rep.replace(dummy, prodnode);
			  if(*tr.parent(it)->name=="\\expression") 
					dummy->fl.bracket=str_node::b_none;
			  else dummy->fl.bracket=str_node::b_round;

			  sibling_iterator wrap=rep.begin(dummy); 
			  // Go to the 'num'th factor in the product.
			  wrap+=num;    
			  // Replace this num'th factor with the original expression.
			  // We will then remove all that has to be removed.
			  iterator theD=rep.insert_subtree(wrap, it);  // iterator to the Derivative node
			  theD->fl.bracket=wrap->fl.bracket;
			  // Go to the 'prod' child of the \diff.
			  sibling_iterator repch=tr.begin(theD);
			  while(*repch->name!="\\prod")
					++repch;
			  // Replace this 'prod' child with 'just' the factor to be replaced, i.e.
			  // remove all the other factors which have been taken out of the derivative.
			  wrap->fl.bracket=prodnode->fl.bracket;
			  repch=tr.replace(repch,wrap);
			  // Erase the factor which we replaced with the \diff.
			  tr.erase(wrap);

			  // Handle signs for anti-commuting derivatives.
			  multiply(dummy->multiplier, sign);
			  // Update the sign. First create an exptree containing the derivative _without_
			  // the object on which it acts.
			  exptree emptyD(theD);
			  sibling_iterator theDargs=emptyD.begin(emptyD.begin());
			  while(theDargs!=emptyD.end(emptyD.begin())) {
				  if(theDargs->is_index()==false) 
					  theDargs=tr.erase(theDargs);
				  else ++theDargs;
				  }
			  
			  int stc=subtree_compare(&kernel.properties, emptyD.begin(), repch);
//			  txtout << "trying to move " << *emptyD.begin()->name << " through " << *repch->name 
//						<< " " << stc << std::endl;
			  int ret=comp.can_swap(emptyD.begin(), repch, stc);
//			  txtout << ret << std::endl;
			  if(ret==0)
				  return result_t::l_no_action;
			  sign*=ret;

			  
			  // Avoid \partial_{a}{\partial_{b} ...} constructions in 
			  // case this child is a \partial-like too.
			  iterator repchi=repch;

			  // The 'dummy' iterator points to the \prod node.
			  cleanup_dispatch(kernel, tr, theD);
			  cleanup_dispatch(kernel, tr, dummy);
			  
			  ++chl;
			  ++num;
			  }
		 }
	it=tr.replace(it,rep.begin()); 
//	cleanup_expression(tr, it);
//	cleanup_nests_below(tr, it);
	return result_t::l_applied;
	}
