#include "MyEvent.h"

ClassImp(MyEvent) // ROOT macro for dictionary generation

MyEvent::MyEvent() : fV0A(0), fV0C(0), fCL1(0) {}

MyEvent::MyEvent(int V0A, int V0C, int CL1) : fV0A(V0A), fV0C(V0C), fCL1(CL1) {}