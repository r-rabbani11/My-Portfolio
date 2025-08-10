#include "MyEvent.h"
ClassImp(MyEvent)
MyEvent::MyEvent():
  TObject(),
  fV0A(0),
  fV0C(0),
  fCL1(0)
{};
MyEvent::MyEvent(int V0A, int V0C, int CL1):
  TObject(),
  fV0A(V0A),
  fV0C(V0C),
  fCL1(CL1)
{};