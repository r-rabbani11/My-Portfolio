// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME MyDict
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
#include "MyEvent.h"
#include "MyParticle.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

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

namespace {
  void TriggerDictionaryInitialization_MyDict_Impl() {
    static const char* headers[] = {
"MyEvent.h",
"MyParticle.h",
nullptr
    };
    static const char* includePaths[] = {
"/snap/root-framework/943/usr/local/include/",
"/home/rrabbani11/Thermal-FIST/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "MyDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$MyEvent.h")))  MyEvent;
class __attribute__((annotate("$clingAutoload$MyParticle.h")))  MyParticle;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "MyDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "MyEvent.h"
#include "MyParticle.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"MyEvent", payloadCode, "@",
"MyParticle", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MyDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MyDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MyDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MyDict() {
  TriggerDictionaryInitialization_MyDict_Impl();
}
