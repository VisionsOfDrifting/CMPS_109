/**************
*nhpappas
*CMPS 109 Summer 2017 
*Asg4
*************/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
      friend class window;
   private:
      shared_ptr<shape> pshape;
      vertex center;
      rgbcolor color;
   public:
      // Default copiers, movers, dtor all OK.
      object (const shared_ptr<shape>&, vertex&, rgbcolor&);
      void draw() { pshape->draw (center, color); }
      void move (GLfloat delta_x, GLfloat delta_y);
      vertex get_center() {return center;}
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
      friend class object;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
     // static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void move_selected_object(int dx, int dy);
      static void next_object();
      static void prev_object();
      static void select_object(size_t obj);   
      static void main();
      static int delta;
      static int border_width;
      static string border_color;
      static bool selected;
      static int get_width() {return width;}
      static int get_height() {return height;}
};

#endif

