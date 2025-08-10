#ifndef MyEvent__H
#define MyEvent__H
#include "TObject.h"
class MyEvent : public TObject {
  public:
    MyEvent();
    MyEvent(int V0A, int V0C, int CL1); 
    void Setup(int V0A, int V0C, int CL1) { fV0A = V0A; fV0C = V0C; fCL1 = CL1; };
    ClassDef(MyEvent,1);
    int fV0A;
    int fV0C;
    int fCL1;
};
#endif