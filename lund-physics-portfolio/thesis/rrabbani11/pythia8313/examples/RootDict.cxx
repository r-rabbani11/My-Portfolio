// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME RootDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "RootEvent.h"
#include "MyEvent.h"
#include "MyParticle.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_RootTrack(void *p = nullptr);
   static void *newArray_RootTrack(Long_t size, void *p);
   static void delete_RootTrack(void *p);
   static void deleteArray_RootTrack(void *p);
   static void destruct_RootTrack(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RootTrack*)
   {
      ::RootTrack *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RootTrack >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("RootTrack", ::RootTrack::Class_Version(), "RootEvent.h", 18,
                  typeid(::RootTrack), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RootTrack::Dictionary, isa_proxy, 4,
                  sizeof(::RootTrack) );
      instance.SetNew(&new_RootTrack);
      instance.SetNewArray(&newArray_RootTrack);
      instance.SetDelete(&delete_RootTrack);
      instance.SetDeleteArray(&deleteArray_RootTrack);
      instance.SetDestructor(&destruct_RootTrack);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RootTrack*)
   {
      return GenerateInitInstanceLocal(static_cast<::RootTrack*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::RootTrack*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_RootEvent(void *p = nullptr);
   static void *newArray_RootEvent(Long_t size, void *p);
   static void delete_RootEvent(void *p);
   static void deleteArray_RootEvent(void *p);
   static void destruct_RootEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RootEvent*)
   {
      ::RootEvent *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::RootEvent >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("RootEvent", ::RootEvent::Class_Version(), "RootEvent.h", 42,
                  typeid(::RootEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::RootEvent::Dictionary, isa_proxy, 4,
                  sizeof(::RootEvent) );
      instance.SetNew(&new_RootEvent);
      instance.SetNewArray(&newArray_RootEvent);
      instance.SetDelete(&delete_RootEvent);
      instance.SetDeleteArray(&deleteArray_RootEvent);
      instance.SetDestructor(&destruct_RootEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RootEvent*)
   {
      return GenerateInitInstanceLocal(static_cast<::RootEvent*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::RootEvent*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MyEvent(void *p = nullptr);
   static void *newArray_MyEvent(Long_t size, void *p);
   static void delete_MyEvent(void *p);
   static void deleteArray_MyEvent(void *p);
   static void destruct_MyEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MyEvent*)
   {
      ::MyEvent *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MyEvent >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("MyEvent", ::MyEvent::Class_Version(), "MyEvent.h", 4,
                  typeid(::MyEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MyEvent::Dictionary, isa_proxy, 4,
                  sizeof(::MyEvent) );
      instance.SetNew(&new_MyEvent);
      instance.SetNewArray(&newArray_MyEvent);
      instance.SetDelete(&delete_MyEvent);
      instance.SetDeleteArray(&deleteArray_MyEvent);
      instance.SetDestructor(&destruct_MyEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MyEvent*)
   {
      return GenerateInitInstanceLocal(static_cast<::MyEvent*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::MyEvent*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MyParticle(void *p = nullptr);
   static void *newArray_MyParticle(Long_t size, void *p);
   static void delete_MyParticle(void *p);
   static void deleteArray_MyParticle(void *p);
   static void destruct_MyParticle(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MyParticle*)
   {
      ::MyParticle *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MyParticle >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("MyParticle", ::MyParticle::Class_Version(), "MyParticle.h", 5,
                  typeid(::MyParticle), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MyParticle::Dictionary, isa_proxy, 4,
                  sizeof(::MyParticle) );
      instance.SetNew(&new_MyParticle);
      instance.SetNewArray(&newArray_MyParticle);
      instance.SetDelete(&delete_MyParticle);
      instance.SetDeleteArray(&deleteArray_MyParticle);
      instance.SetDestructor(&destruct_MyParticle);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MyParticle*)
   {
      return GenerateInitInstanceLocal(static_cast<::MyParticle*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::MyParticle*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr RootTrack::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *RootTrack::Class_Name()
{
   return "RootTrack";
}

//______________________________________________________________________________
const char *RootTrack::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RootTrack*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int RootTrack::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RootTrack*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RootTrack::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RootTrack*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RootTrack::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RootTrack*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr RootEvent::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *RootEvent::Class_Name()
{
   return "RootEvent";
}

//______________________________________________________________________________
const char *RootEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RootEvent*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int RootEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::RootEvent*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *RootEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RootEvent*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *RootEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::RootEvent*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MyEvent::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *MyEvent::Class_Name()
{
   return "MyEvent";
}

//______________________________________________________________________________
const char *MyEvent::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyEvent*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int MyEvent::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyEvent*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MyEvent::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyEvent*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MyEvent::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyEvent*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MyParticle::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *MyParticle::Class_Name()
{
   return "MyParticle";
}

//______________________________________________________________________________
const char *MyParticle::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyParticle*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int MyParticle::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyParticle*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MyParticle::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyParticle*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MyParticle::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyParticle*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void RootTrack::Streamer(TBuffer &R__b)
{
   // Stream an object of class RootTrack.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RootTrack::Class(),this);
   } else {
      R__b.WriteClassBuffer(RootTrack::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RootTrack(void *p) {
      return  p ? new(p) ::RootTrack : new ::RootTrack;
   }
   static void *newArray_RootTrack(Long_t nElements, void *p) {
      return p ? new(p) ::RootTrack[nElements] : new ::RootTrack[nElements];
   }
   // Wrapper around operator delete
   static void delete_RootTrack(void *p) {
      delete (static_cast<::RootTrack*>(p));
   }
   static void deleteArray_RootTrack(void *p) {
      delete [] (static_cast<::RootTrack*>(p));
   }
   static void destruct_RootTrack(void *p) {
      typedef ::RootTrack current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::RootTrack

//______________________________________________________________________________
void RootEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class RootEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(RootEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(RootEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_RootEvent(void *p) {
      return  p ? new(p) ::RootEvent : new ::RootEvent;
   }
   static void *newArray_RootEvent(Long_t nElements, void *p) {
      return p ? new(p) ::RootEvent[nElements] : new ::RootEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_RootEvent(void *p) {
      delete (static_cast<::RootEvent*>(p));
   }
   static void deleteArray_RootEvent(void *p) {
      delete [] (static_cast<::RootEvent*>(p));
   }
   static void destruct_RootEvent(void *p) {
      typedef ::RootEvent current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::RootEvent

//______________________________________________________________________________
void MyEvent::Streamer(TBuffer &R__b)
{
   // Stream an object of class MyEvent.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MyEvent::Class(),this);
   } else {
      R__b.WriteClassBuffer(MyEvent::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MyEvent(void *p) {
      return  p ? new(p) ::MyEvent : new ::MyEvent;
   }
   static void *newArray_MyEvent(Long_t nElements, void *p) {
      return p ? new(p) ::MyEvent[nElements] : new ::MyEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_MyEvent(void *p) {
      delete (static_cast<::MyEvent*>(p));
   }
   static void deleteArray_MyEvent(void *p) {
      delete [] (static_cast<::MyEvent*>(p));
   }
   static void destruct_MyEvent(void *p) {
      typedef ::MyEvent current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::MyEvent

//______________________________________________________________________________
void MyParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class MyParticle.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MyParticle::Class(),this);
   } else {
      R__b.WriteClassBuffer(MyParticle::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MyParticle(void *p) {
      return  p ? new(p) ::MyParticle : new ::MyParticle;
   }
   static void *newArray_MyParticle(Long_t nElements, void *p) {
      return p ? new(p) ::MyParticle[nElements] : new ::MyParticle[nElements];
   }
   // Wrapper around operator delete
   static void delete_MyParticle(void *p) {
      delete (static_cast<::MyParticle*>(p));
   }
   static void deleteArray_MyParticle(void *p) {
      delete [] (static_cast<::MyParticle*>(p));
   }
   static void destruct_MyParticle(void *p) {
      typedef ::MyParticle current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::MyParticle

namespace ROOT {
   static TClass *vectorlERootTrackgR_Dictionary();
   static void vectorlERootTrackgR_TClassManip(TClass*);
   static void *new_vectorlERootTrackgR(void *p = nullptr);
   static void *newArray_vectorlERootTrackgR(Long_t size, void *p);
   static void delete_vectorlERootTrackgR(void *p);
   static void deleteArray_vectorlERootTrackgR(void *p);
   static void destruct_vectorlERootTrackgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<RootTrack>*)
   {
      vector<RootTrack> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<RootTrack>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<RootTrack>", -2, "vector", 428,
                  typeid(vector<RootTrack>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlERootTrackgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<RootTrack>) );
      instance.SetNew(&new_vectorlERootTrackgR);
      instance.SetNewArray(&newArray_vectorlERootTrackgR);
      instance.SetDelete(&delete_vectorlERootTrackgR);
      instance.SetDeleteArray(&deleteArray_vectorlERootTrackgR);
      instance.SetDestructor(&destruct_vectorlERootTrackgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<RootTrack> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<RootTrack>","std::vector<RootTrack, std::allocator<RootTrack> >"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<RootTrack>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlERootTrackgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<RootTrack>*>(nullptr))->GetClass();
      vectorlERootTrackgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlERootTrackgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlERootTrackgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<RootTrack> : new vector<RootTrack>;
   }
   static void *newArray_vectorlERootTrackgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<RootTrack>[nElements] : new vector<RootTrack>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlERootTrackgR(void *p) {
      delete (static_cast<vector<RootTrack>*>(p));
   }
   static void deleteArray_vectorlERootTrackgR(void *p) {
      delete [] (static_cast<vector<RootTrack>*>(p));
   }
   static void destruct_vectorlERootTrackgR(void *p) {
      typedef vector<RootTrack> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<RootTrack>

namespace {
  void TriggerDictionaryInitialization_RootDict_Impl() {
    static const char* headers[] = {
"RootEvent.h",
"MyEvent.h",
"MyParticle.h",
nullptr
    };
    static const char* includePaths[] = {
"/snap/root-framework/943/usr/local/include/",
"/home/rrabbani11/pythia8313/examples/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "RootDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$RootEvent.h")))  RootTrack;
class __attribute__((annotate("$clingAutoload$RootEvent.h")))  RootEvent;
class __attribute__((annotate("$clingAutoload$MyEvent.h")))  MyEvent;
class __attribute__((annotate("$clingAutoload$MyParticle.h")))  MyParticle;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "RootDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "RootEvent.h"
#include "MyEvent.h"
#include "MyParticle.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"MyEvent", payloadCode, "@",
"MyParticle", payloadCode, "@",
"RootEvent", payloadCode, "@",
"RootTrack", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("RootDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_RootDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_RootDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_RootDict() {
  TriggerDictionaryInitialization_RootDict_Impl();
}
