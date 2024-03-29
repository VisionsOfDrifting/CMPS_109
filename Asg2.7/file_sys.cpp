/**************
*nhpappas
*kbcrum
*CMPS 109 Summer 2017 
*Asg2
*************/

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"
#include "commands.h"
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

string inode::path(const inode_ptr& current) {
   vector<string> path;
   map<string, inode_ptr> dirents = current->contents->get_contents();
   inode_ptr parent = dirents.at("..");
   string dir_path;
   bool extraSlash = true;
   string name_fix = current->get_name();
   if(name_fix != "/"){
   name_fix.pop_back();
   path.push_back(name_fix);
   }else{path.push_back(name_fix);}
      
   if(current->get_inode_nr() == 1 && parent->get_inode_nr() == 1)
   {extraSlash = false; }
   
   while(parent->get_inode_nr() > 1){
      path.push_back(parent->get_name());
      map<string, inode_ptr> dirents = parent->contents->get_contents();
      parent = dirents.at("..");
   }
   if(extraSlash){path.push_back(parent->get_name());}
   
   for(auto i = path.cend() - 1; i != path.cbegin() - 1; --i){
      dir_path += *i;
   }
   return dir_path;
}

void lsr(inode_ptr& dir){
   map<string, inode_ptr> dirents = dir->contents->get_contents();
   cout << dir->path(dir) << ":" << endl;
   for (auto i = dirents.cbegin(); i != dirents.cend(); ++i) {
      cout << setw(6) << i->second->get_inode_nr() << "  " << setw(6)
               << i->second->contents->size() << "  " << i->first
               << endl;
   }
   for(auto i = dirents.begin(); i != dirents.end(); ++i){
       if(i->first.compare(".") == 0 or i->first.compare("..") == 0);
       else{
          if(i->second->contents->is_dir()){
             lsr(i->second);
          }
       }
    }
}

inode::inode(file_type type): inode_nr (next_inode_nr++){
   switch (type) {
      case file_type::PLAIN_TYPE:
           //file_name = name;
           contents = make_shared<plain_file>();
           //isDir = false;
           break;
      case file_type::DIRECTORY_TYPE:
           //file_name = name;
           contents = make_shared<directory>();
           //contents->dirents["."] = this;
           //isDir = true;
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}
/*
bool inode::is_dir(){
  return isDir;
}*/

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}
/*
ostream &operator<< (ostream &out, inode_ptr node) {
   auto itor = node.contents.dirents.begin();
   auto end = node.contents.dirents.end();
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
*/

file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   size = data.size();     // Accounts for spaces removed by delimiter.
   for (auto word = data.begin();
             word != data.end();
             word++) {
       size += word->size();     // Counts the characters per word.
   }
   if (size > 1) size -= 1;
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   wordvec new_data;
   for(size_t i = 2; i < words.size(); ++i){
      new_data.push_back(words.at(i));
   }
   data = new_data;
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

void plain_file::set_dir(inode_ptr cwd, inode_ptr parent){
   throw file_error("is a plain file");
}

map<string, inode_ptr>& plain_file::get_contents(){
   throw file_error("is a plain file");
}

void plain_file::set_contents(const map<string, inode_ptr>&){
   throw file_error("is a plain file");
}
   

directory::directory() {
   dirents.insert(pair<string, inode_ptr>(".", nullptr));
   dirents.insert(pair<string, inode_ptr>("..", nullptr));
}

void directory::set_dir(inode_ptr cwd, inode_ptr parent){
   map<string, inode_ptr>::iterator i = dirents.begin();
   i->second = cwd; ++i;
   i->second = parent;
}

map<string, inode_ptr>& directory::get_contents(){
   return dirents;
}

void directory::set_contents(const map<string, inode_ptr>& new_map){
   dirents = new_map;
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

void directory::set_data(const wordvec& d){
   throw file_error("is a directory");
}
   
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode_ptr dir = make_shared<inode>(file_type::DIRECTORY_TYPE);
   dir->set_name(dirname + "/");
   return dir;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   inode_ptr file = make_shared<inode>(file_type::PLAIN_TYPE);
   file->set_name(filename);
   return file;
}
   
   
inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root; parent = root;
   root->contents->set_dir(cwd, parent);
   root->set_name("/");
}

const string& inode_state::prompt() { return prompt_; }

void inode_state::print_path(const inode_ptr& current) const {
   vector<string> path;
   map<string, inode_ptr> dirents = current->contents->get_contents();
   inode_ptr parent = dirents.at("..");
   bool extraSlash = true;
   string name_fix = current->get_name();
   name_fix.pop_back();
   path.push_back(name_fix);
   
   if(current->get_inode_nr() == 1 && parent->get_inode_nr() == 1)
   {extraSlash = false; }
   
   while(parent->get_inode_nr() > 1){
      path.push_back(parent->get_name());
      map<string, inode_ptr> dirents = parent->contents->get_contents();
      parent = dirents.at("..");
   }
   
   if(extraSlash){path.push_back(parent->get_name());}
   
   string temp = path.front();
   temp.substr(0,temp.length()-1);
   path.front() = temp;
   for(auto i = path.cend() - 1; i != path.cbegin() - 1; --i){
      //if(i == path.cend() - 1) cout << *i;
      //else if(i > path.cbegin()) cout << *i << "/";
      cout << *i;
   }
   cout << endl;
}
void inode_state::print_directory
(const inode_ptr& current, const wordvec& args) const {
   map<string, inode_ptr> dirents = current->contents->get_contents();
   if(args.size() == 1){
      string name_fix = current->get_name();
      name_fix.pop_back();
      name_fix = "/" + name_fix;
      cout << name_fix << ":" << endl;
      for(auto i = dirents.cbegin(); i != dirents.cend(); ++i){
         cout << setw(6) << i->second->get_inode_nr() << "  " << setw(6)
             << i->second->contents->size() << "  " << i->first << endl;
      }
   }
   else{
      wordvec path_name = split(args.at(1), "/");
      inode_ptr ls_dir = current; bool found = false;
      for(size_t i = 0; i < path_name.size(); ++i){
         found = false;
         for(auto j = dirents.cbegin(); j != dirents.cend(); ++j){
            if(j->first == path_name.at(i) + "/"){
               ls_dir = j->second;
               found = true;
            }
            else if(path_name.at(0) == j->first){
               ls_dir = j->second;
               found = true;
               break;
            }
         }
         if(found == false){
            throw command_error("print_directory: invalid pathname");
         }
         dirents = ls_dir->contents->get_contents();
      }
      string name_fix = ls_dir->get_name();
      name_fix.pop_back();
      name_fix = "/" + name_fix;
      cout << name_fix << ":" << endl;
      for (auto i = dirents.cbegin(); i != dirents.cend(); ++i) {
         cout << setw(6) << i->second->get_inode_nr() << "  " << setw(6)
                  << i->second->contents->size() << "  " << i->first
                  << endl;
      }
   }
}

void inode_state::list_recursively
(inode_state& state, const wordvec& args) {
   inode_ptr lr; map<string, inode_ptr> dirents;
   if(args.size() == 1){
      lr = state.get_cwd();
      dirents = lr->contents->get_contents();
      cout << lr->path(lr) << ":" << endl;
      for (auto i = dirents.cbegin(); i != dirents.cend(); ++i) {
         cout << setw(6) << i->second->get_inode_nr() << "  " << setw(6)
                  << i->second->contents->size() << "  " << i->first
                  << endl;
      }
      for (auto i = dirents.begin(); i != dirents.end(); ++i) {
         if (i->first.compare(".") == 0 or i->first.compare("..") == 0)
            ;
         else {
            if (i->second->contents->is_dir()) {
               lsr(i->second);
            }
         }
      }
   }
   else{
      wordvec path_name = split(args.at(1), "/");
      string path = args.at(1);
      path = path.at(0);
      if (path == "/")
         lr = root;
      else
         lr = state.get_cwd();
      bool found = false;
      dirents = lr->contents->get_contents();
      for (size_t i = 0; i < path_name.size(); ++i) {
         found = false;
         for (auto j = dirents.cbegin(); j != dirents.cend(); ++j) {
            if (j->first == path_name.at(i) + "/") {
               lr = j->second;
               found = true;
            }
         }
         if (found == false) {
            throw command_error("list_recursively: invalid pathname");
         }
         dirents = lr->contents->get_contents();
      }
      //print top layer dir
      cout << lr->path(lr) << ":" << endl;
      for (auto i = dirents.cbegin(); i != dirents.cend(); ++i) {
         cout << setw(6) << i->second->get_inode_nr() << "  " << setw(6)
                  << i->second->contents->size() << "  " << i->first
                  << endl;
      }
   //Pass in each directory to be recursively listed, from the top layer
      for (auto i = dirents.begin(); i != dirents.end(); ++i) {
         if (i->first.compare(".") == 0 or i->first.compare("..") == 0)
            ;
         else {
            if (i->second->contents->is_dir()) {
               lsr(i->second);
            }
         }
      }
   }
}

void inode_state::create_file
(const inode_ptr& current, const wordvec& words) const {
   wordvec path_name = split(words.at(1), "/");
   map<string, inode_ptr> dirents = current->
            contents->get_contents();
   //newFile points to the dir that the file will be created in
   inode_ptr newFile = current; bool found = false;
   //same_file points to the duplicate file, if there is one
   inode_ptr same_file = nullptr; bool file_match = false;
   for (size_t i = 0; i < path_name.size() - 1; ++i) {
      found = false;
      for (auto j = dirents.cbegin(); j != dirents.cend(); ++j) {
         if (j->first == path_name.at(i) + "/") {
            newFile = j->second;
            found = true;
         }
      }
      if (found == false) {
         throw command_error("fn_make: invalid pathname");
      }
      dirents = newFile->contents->get_contents();
   }
   //Check to see if a dir or a file with the same name exists
   for(auto i = dirents.cbegin(); i != dirents.cend(); ++i){
      // If the file has the same name as a directory, throw an error.
      if (i->first == words.at(1) + "/") {
         throw command_error("fn_make: "
                  "directory has same name");
         // If the file has the same name as an existing file, replace
         // the existing file with the new one (including new data).
      }
      else if(i->first == words.at(1)){
         file_match = true;
         same_file = i->second;
      }
   }
   if(file_match){
      same_file ->contents->writefile(words);
      dirents.insert(pair<string, inode_ptr>
      (same_file->get_name(), same_file));
      newFile->contents->set_contents(dirents);
   }
   else{
      inode_ptr new_file = newFile->contents->
                  mkfile(path_name.at(path_name.size() - 1));
      new_file->contents->writefile(words);
      dirents.insert(pair<string, inode_ptr>
      (new_file->get_name(), new_file));
      newFile->contents->set_contents(dirents);
   }
}

void inode_state::read_file
(const inode_ptr& current, const wordvec& words) const {
   for (size_t k = 1; k != words.size(); ++k) {
      bool found = false;      // Flags true if file found.
      map<string, inode_ptr> dirents =
               current->contents->get_contents();
      for (auto i = dirents.cbegin(); i != dirents.cend(); ++i) {
         // Search to see if a file or directory shares the name.
         if (i->first == words.at(k)) {
            // See if the matching file is a directory.
            if (i->second->contents->is_dir() == false) {
               found = true;
               for (auto j = i->second->contents->readfile().begin();
                        j != i->second->contents->readfile().end();
                        ++j) {
                  cout << *j << " ";
               }
            } else if (i->second->contents->is_dir() == true) {
               throw command_error("fn_cat: cannot read directories.");
            }
            cout << endl;
         }
      }
      if (!found) {
         throw command_error("fn_cat: No such file or directory.");
      }
   }
}

void inode_state::make_directory
(const inode_ptr& current, const wordvec& path) const {
      wordvec path_name = split(path.at(1), "/");
      map<string, inode_ptr> dirents = current->
         contents->get_contents();
      inode_ptr newDir = current; bool found = false;
      for(size_t i = 0; i < path_name.size() - 1; ++i){
         found = false;
         for(auto j = dirents.cbegin(); j != dirents.cend(); ++j){
            if(j->first == path_name.at(i) + "/"){
               newDir = j->second;
               found = true;
            }
         }
         if(found == false){
            throw command_error("fn_mkdir: invalid pathname");
         }
         dirents = newDir->contents->get_contents();
      }
      //Check to see if a dir with that name already exists
      for(auto i = dirents.cbegin(); i != dirents.cend(); ++i){
         if(i->first == path_name.at(path_name.size() - 1) + "/"){
            throw command_error
            ("fn_mkdir: a directory already exists with that name");
         }
      }
      inode_ptr dir = newDir->contents->mkdir
               (path_name.at(path_name.size() - 1));
      dir->contents->set_dir(dir, current);
      dirents.insert(pair<string, inode_ptr>
      (dir->get_name(), dir));
      newDir->contents->set_contents(dirents);
}

void inode_state::change_directory
(inode_state& state, const wordvec& args){
   if(args.size() == 1) cwd = state.get_root();
   else{
      wordvec path_name = split(args.at(1), "/");
      string path = args.at(1); inode_ptr cd;
      path = path.at(0);
      if(path == "/") cd = state.get_root();
      else cd = state.get_cwd();
      bool found = false;
      map<string, inode_ptr> dirents = cd->contents->get_contents();
      for(size_t i = 0; i < path_name.size(); ++i){
          found = false;
          for(auto j = dirents.cbegin(); j != dirents.cend(); ++j){
             if(j->first == path_name.at(i) + "/"){
                cd = j->second;
                found = true;
             }
          }
          if(found == false){
             throw command_error("fn_cd: invalid pathname");
          }
          dirents = cd->contents->get_contents();
       }
      cwd = cd;
   }
}

void inode_state::remove(const inode_ptr& current,
         const wordvec& args) const {
   for (size_t k = 1; k != args.size(); ++k) {
      bool found = false;      // Flags true if file found.
      map<string, inode_ptr> dirents =
               current->contents->get_contents();
      for (auto i = dirents.cbegin(); i != dirents.cend(); ++i) {
         // Search to see if a file or directory shares the name.
         if (i->first == args.at(k)) {
            // See if the matching file is a directory.
            if (i->second->contents->is_dir() == false) {
               found = true;
               current->contents->get_contents().erase(i);
            } else if (i->second->contents->is_dir() == true) {
               throw command_error("fn_rm: cannot read directories.");
            }
         }
      }
      if (!found) {
         throw command_error("fn_rm: No such file or directory.");
      }
   }
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

