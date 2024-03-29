**************
*nhpappas
*kbcrum
*CMPS 109 Summer 2017 
*Asg1
*************

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <iostream>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   if(is_negative and that.is_negative){
      ubigint result = uvalue + that.uvalue;
     // cout << "in _ - _" << endl;
      return {result, true};}
   if(is_negative and !that.is_negative){
      if(uvalue > that.uvalue){
         ubigint result = that.uvalue - uvalue;
         //cout << "in _ - +" << endl;
         return {result, true};
   }else if(uvalue < that.uvalue){
         ubigint result = uvalue - that.uvalue;
       //  cout << "in _ - _" << endl;
         return result;}
   }
   if(!is_negative and !that.is_negative){
      ubigint result = uvalue + that.uvalue;
     // cout << "in + - _" << endl;
      return result;
   }
   if(!is_negative and that.is_negative){
      if(uvalue < that.uvalue){
         ubigint result = that.uvalue - uvalue;
        // cout << "in + - +" << endl;
         return result;}
      else if(uvalue > that.uvalue){
         ubigint result = uvalue - that.uvalue;
        // cout << "in + - +" << endl;
         return {result,true};}
   }
}

bigint bigint::operator- (const bigint& that) const {
   if(is_negative and that.is_negative){
      if(uvalue > that.uvalue){
         ubigint result = uvalue - that.uvalue;
        // cout << "in _ - _" << endl;
         return {result, true};
       }else if(uvalue < that.uvalue){
         ubigint result = uvalue - that.uvalue;
       //  cout << "in _ - _" << endl;
         return result;}
   }
   if(is_negative and !that.is_negative){
      ubigint result = uvalue + that.uvalue;
     // cout << "in _ - +" << endl;
      return {result, true};
   }
   if(!is_negative and that.is_negative){
      ubigint result = uvalue + that.uvalue;
     // cout << "in + - _" << endl;
      return result;
   }
   if(!is_negative and !that.is_negative){
      if(uvalue < that.uvalue){
         ubigint result = uvalue - that.uvalue;
        // cout << "in + - +" << endl;
         return {result, true};}
      else if(uvalue > that.uvalue){
         ubigint result = uvalue - that.uvalue;
        // cout << "in + - +" << endl;
         return result;}
   }
}

bigint bigint::operator* (const bigint& that) const {
   bigint result = uvalue * that.uvalue;
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "") << that.uvalue;
}

