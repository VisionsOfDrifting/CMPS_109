// $Id: listmap.tcc,v 1.9 2016-07-20 21:00:33-07 - - $

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   while (!empty()) { erase(begin());}
   TRACE ('l', reinterpret_cast<const void*> (this));
}
/*
//
// listmap::empty()
//
template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty() const {
   return anchor_.next == &anchor_;
}

//
// listmap::iterator listmap::begin()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin() {
  return iterator (anchor_.next);
}

//
// listmap::iterator listmap::end()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end() {
   return iterator (anchor());
}

template<typename Key, typename Value, class Less>
const typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::cbegin() const{
   return iterator(anchor_.next);
}

template<typename Key, typename Value, class Less>
const typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::cend() const{
   return iterator(anchor());
}
*/

//
// listmap::insert(const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& that) {
   Less less;
   listmap<Key,Value,Less>::iterator itor = begin();
   //Search map, determine if key exists or not
   while (itor!=end()){
      if(less(pair.first,itor->first)){
         break;
      }else if( not less(pair.first,itor->first)){ 
         ++itor;
      }
   }
   //Update value if key exists
   if(itor!=end()){
      if(itor->first == pair.first) {
          itor->second = pair.second;
          return itor;
      }
   }
   
   //For adding the first node
   if(itor.where == anchor() && itor.where->next == anchor()) {
     node* temp = new node(itor.where->next, itor.where, pair);
     itor.where->next = temp;
     itor.where->prev = temp;
     return iterator(temp);
   }
   
   //Key does not already exist. Add to map
   node* temp = new node(itor.where, itor.where->prev, pair);
   itor.where->prev->next = temp;
   itor.where->prev = temp;  
   return iterator(temp);
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   listmap<Key,Value,Less>::iterator itor = begin();
   while(itor!=end() && itor->first != that) {
     ++itor;}
   TRACE ('l', that);
   return itor;
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   listmap<Key,Value,Less>::iterator itor = position;
   ++itor;
   position.erase();
   TRACE ('l', &*position);
   return itor;
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//void listmap::iterator::erase()
template <typename Key, typename Value, class Less>
void listmap<Key,Value,Less>::iterator::erase() {
   //Reassign pointer then delete contents at itor's pointer
   if (where != nullptr)
   {
      where->prev->next = where->next;
      where->next->prev = where->prev;
      delete where;
  }   
}


//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

