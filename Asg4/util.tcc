/**************
*nhpappas
*CMPS 109 Summer 2017 
*Asg4
*************/

#include <memory>

template <typename item_t>
ostream& operator<< (ostream& out, const vector<item_t>& vec) {
   bool want_space = false;
   for (const auto& item: vec) {
      if (want_space) cout << " ";
      out << item;
      want_space = true;
   }
   return out;
}

template <typename iterator>
ostream& operator<< (ostream& out, pair<iterator,iterator> range) {
   bool want_space = false;
   while (range.first != range.second) {
      if (want_space) cout << " ";
      out << *range.first++;
      want_space = true;
   }
   return out;
}


template <typename item_t>
string to_string (const item_t& that) {
   ostringstream stream;
   stream << that;
   return stream.str();
}

template <typename item_t>
item_t from_string (const string& that) {
   stringstream stream;
   stream << that;
   item_t result;
   if (not (stream >> result and stream.eof())) {
      throw range_error (demangle (result)
            + " from_string (" + that + ")");
   }
   return result;
}


//
// Demangle a class name.
// For __GNUC__, use __cxa_demangle.
// As a fallback, just use typeid.name()
// The functions are fully specified in this header as non-inline
// functions in order to avoid the need for explicit instantiation.
// http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
//
#ifdef __GNUC__

#include <cxxabi.h>

template <typename type>
string demangle (const type& object) {
   const char* const name = typeid (object).name();
   int status {0}; 
   using deleter = void (*) (void*);
   unique_ptr<char,deleter> result {
      abi::__cxa_demangle (name, nullptr, nullptr, &status),
      std::free,
   };
   return status == 0 ? result.get() : name; 
}

#else

template <typename type>
string demangle (const type& object) {
   return typeid (object).name();
}

#endif

