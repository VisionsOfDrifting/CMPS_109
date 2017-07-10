**************
*nhpappas
*kbcrum
*CMPS 109 Summer 2017 
*Asg1
*************

#ifndef __ITERSTACK_H__
#define __ITERSTACK_H__

#include <vector>
using namespace std;

template <typename value_type>
class iterstack: private vector<value_type> {
   private:
      using stack_t = vector<value_type>;
      using stack_t::crbegin;
      using stack_t::crend;
      using stack_t::push_back;
      using stack_t::pop_back;
      using stack_t::back;
      using const_iterator = typename stack_t::const_reverse_iterator;
   public:
      using stack_t::clear;
      using stack_t::empty;
      using stack_t::size;
      inline const_iterator begin() {return crbegin();}
      inline const_iterator end() {return crend();}
      inline void push (const value_type& value) {push_back (value);}
      inline void pop() {pop_back();}
      inline const value_type& top() const {return back();}
};

#endif

