**************
*nhpappas
*kbcrum
*CMPS 109 Summer 2017 
*Asg1
*************

#ifndef __REL_OPS_H__
#define __REL_OPS_H__

template <typename value>
inline bool operator!= (const value& left, const value& right) {
   return not (left == right); 
}

template <typename value>
inline bool operator>  (const value& left, const value& right) {
   return right < left; 
}

template <typename value>
inline bool operator<= (const value& left, const value& right) {
   return not (right < left);
}

template <typename value>
inline bool operator>= (const value& left, const value& right) {
   return not (left < right); 
}

#endif

