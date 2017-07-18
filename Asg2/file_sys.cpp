// $Id: file_sys.cpp,v 1.5 2016-01-14 16:16:52-08 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   root = inode::inode(DIRECTORY_TYPE, "/");
   root.contents.set_parent(root);
   set_cwd_to_root();
   
   cout << "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() { return prompt_; }

void inode_state::setprompt(const string &newprompt){
   prompt = newprompt;
}

void inode_state::set_cwd_to_root(){
   cwd = root;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type, string name): inode_nr (next_inode_nr++){
   switch (type) {
      case file_type::PLAIN_TYPE:
           file_name = name;
           contents = make_shared<plain_file>();
           isDir = false;
           break;
      case file_type::DIRECTORY_TYPE:
           file_name = name;
           contents = make_shared<directory>();
           contents.dirents["."] = this;
           isDir = true;
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

inode_ptr inode::get_child_dir(const string &dirname){
   inode_ptr target = nullptr;
   if ( type == FILE_INODE ) return target;
   //map<string, inode*>
   directory::const_iterator itor = contents.dirents.begin();
   directory::const_iterator end = contents.dirents.end();
   for (; itor != end; ++itor) {
      if ( dirname.compare(itor.first) == 0)
         return itor.second;
   }
   return NULL;
}

inode_ptr inode::get_parent(){
   directory::iterator p = contents.dirents.begin();
   ++p;
   return p.second;
}


const string inode::name(){
   inode_ptr parent = get_child_dir("..");
   if ( parent == this ) return "/";
   else {
      directory::const_iterator itor =
         parent.contents.dirents->begin();
      directory::const_iterator end =
         parent.contents.dirents->end();
      for (; itor != end; ++itor) {
         if ( itor.second == this )
            return string(itor.first);
      }
   }
   return "";
}

ostream &operator<< (ostream &out, inode_ptr node) {
   directory::const_iterator itor = node.contents.dirents.begin();
   directory::const_iterator end = node.contents.dirents.end();
   int i = 0;
   for (; itor != end;) {
      out << setw(6) << itor->second.get_inode_nr() << setw(6)
         << itor.second.size() << "  " << itor.first;
      if ( i < 2 ){
         ++i;
      } else if ( itor.second.isDir ) 
         out << "/";
      ++itor;
      if ( itor != end ) out << "\n";
   }
   return out;


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   wordvec_itor i = data->begin();
   wordvec_itor end = data->end();
   while( i != end ) size += i++.size();
   size += data.size();
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data.clear();
   //words[0] = make, words[1] = filename,
   //words[2] = new file information.
   wordvec_itor itor = words.begin()+2;
   while (itor != words.end()){
      contents.data->push_back(*itor++);
   }
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}


size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   size = dirents.size();
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode_ptr dir = inode(DIRECTORY_TYPE, dirname);
   return dir;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   inode_ptr file = inode(PLAIN_TYPE, filename);
   return file;
}

void directory::set_parent (inode_ptr parent) {
   dirents[".."] = parent;
}

void directory::add_dirent(const string& name, inode_ptr addition){
   dirents[name] = addition;
   addition.set_parent(this);
}

void directory::add_file(const string& name, inode_ptr newfile){
   dirents[name] = newfile;

