#ifndef SINE_H
#define SINE_H

#include <math.h>
#include <list>
#include <vector>
#define                 PI          3.1415926536

#define SINE_FULL       1           /*    _  
                                     *  /   \
                                     *       \ _ /   
                                     *
                                     *****************/
#define SINE_RISE       2           /*    _
                                     *  /   
                                     *
                                     *****************/
#define SINE_FALL       3           /*    _    
                                     *      \
                                     *               
                                     *****************/
#define SINE_MOUNTAIN   4           /*   _      
                                     * /   \
                                     *               
                                     *****************/
#define SINE_BOUNCE     5           /*    _     
                                     *      \   
                                     *       \ _
                                     *
                                     *****************/
using namespace std;                                   
                                     
class sine{
  // Associations
  // Attributes
  // Operations
  private:

  public:
  vector<float> sinetable;
  
  sine( );
  ~sine();
  void setSine( float amplitude, int type, int slices );
  float getSine(int slice);
  
};

#endif
