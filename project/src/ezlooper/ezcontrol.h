#ifndef EZCONTROL_H
#define EZCONTROL_H

#include "ezlooper.h"

class ezcontrol {
  private:
  public:
    ezlooper *ez;
    ezcontrol( ezlooper *ez );
    ~ezcontrol();
    virtual void init() = 0;
    virtual void process( EZEvent e, int arg1 ) = 0;
};

#endif
