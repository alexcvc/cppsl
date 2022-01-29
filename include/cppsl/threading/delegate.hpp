/*
	Thread Wrapper v.2.0
	Delegates

	Copyright (C) 2017 by Sergey A Kryukov
   The article, along with any associated source code and files, is licensed under
   The Code Project Open License (CPOL)

   http://www.SAKryukov.org
	http://www.codeproject.com/Members/SAKryukov

	Original publications:
	https://www.codeproject.com/Articles/1177478/Thread-Wrapper-CPP
	https://www.codeproject.com/Articles/1177869/Conveyor-Thread-Wrapper-CPP
*/

/*************************************************************************//**
 * \file        delegate.hh
 * \brief       The delegate type is used to store in the queue, and reflects
 * the profile of the synchronization object and its thread.
 * A synchronization-based delegate instance is passed as the first parameter
 * to a task function to allow the application to keep the task responsive to
 * thread throttling and interrupts.
 * The delegate implementation does not have to be implemented in a derived
 * from the thread wrapper class, but can be implemented in a custom class
 * where the synchronization function itself is not available.
 *
 * \date        2021-10-24
 *****************************************************************************/

#ifndef FBD1FFCB_9AD2_49DE_9E83_A5212FE3F926
#define FBD1FFCB_9AD2_49DE_9E83_A5212FE3F926

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/threading/delegateBase.hpp>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

namespace cppsl::threading {

  template <typename T> class delegate;
  template <typename T> class multicast_delegate;

  template<typename RET, typename ...PARAMS>
  class delegate<RET(PARAMS...)> final : private delegate_base<RET(PARAMS...)> {
   public:

    delegate() = default;

    bool isNull() const { return invocation.stub == nullptr; }
    bool operator ==(void* ptr) const {
      return (ptr == nullptr) && this->isNull();
    } //operator ==
    bool operator !=(void* ptr) const {
      return (ptr != nullptr) || (!this->isNull());
    } //operator !=

    delegate(const delegate& another) { another.invocation.Clone(invocation); }

    template <typename LAMBDA>
    delegate(const LAMBDA& lambda) {
      assign((void*)(&lambda), lambda_stub<LAMBDA>);
    } //delegate

    delegate& operator =(const delegate& another) {
      another.invocation.Clone(invocation);
      return *this;
    } //operator =

    template <typename LAMBDA> // template instantiation is not needed, will be deduced (inferred):
    delegate& operator =(const LAMBDA& instance) {
      assign((void*)(&instance), lambda_stub<LAMBDA>);
      return *this;
    } //operator =

    bool operator == (const delegate& another) const { return invocation == another.invocation; }
    bool operator != (const delegate& another) const { return invocation != another.invocation; }

    bool operator ==(const multicast_delegate<RET(PARAMS...)>& another) const { return another == (*this); }
    bool operator !=(const multicast_delegate<RET(PARAMS...)>& another) const { return another != (*this); }

    template <class T, RET(T::*TMethod)(PARAMS...)>
    static delegate create(T* instance) {
      return delegate(instance, method_stub<T, TMethod>);
    } //create

    template <class T, RET(T::*TMethod)(PARAMS...) const>
    static delegate create(T const* instance) {
      return delegate(const_cast<T*>(instance), const_method_stub<T, TMethod>);
    } //create

    template <RET(*TMethod)(PARAMS...)>
    static delegate create() {
      return delegate(nullptr, function_stub<TMethod>);
    } //create

    template <typename LAMBDA>
    static delegate create(const LAMBDA & instance) {
      return delegate((void*)(&instance), lambda_stub<LAMBDA>);
    } //create

    RET operator()(PARAMS... arg) const {
      return (*invocation.stub)(invocation.object, arg...);
    } //operator()

   private:

    delegate(void* anObject, typename delegate_base<RET(PARAMS...)>::stub_type aStub) {
      invocation.object = anObject;
      invocation.stub = aStub;
    } //delegate

    void assign(void* anObject, typename delegate_base<RET(PARAMS...)>::stub_type aStub) {
      this->invocation.object = anObject;
      this->invocation.stub = aStub;
    } //assign

    template <class T, RET(T::*TMethod)(PARAMS...)>
    static RET method_stub(void* this_ptr, PARAMS... params) {
      T* p = static_cast<T*>(this_ptr);
      return (p->*TMethod)(params...);
    } //method_stub

    template <class T, RET(T::*TMethod)(PARAMS...) const>
    static RET const_method_stub(void* this_ptr, PARAMS... params) {
      T* const p = static_cast<T*>(this_ptr);
      return (p->*TMethod)(params...);
    } //const_method_stub

    template <RET(*TMethod)(PARAMS...)>
    static RET function_stub(void* this_ptr, PARAMS... params) {
      return (TMethod)(params...);
    } //function_stub

    template <typename LAMBDA>
    static RET lambda_stub(void* this_ptr, PARAMS... arg) {
      LAMBDA* p = static_cast<LAMBDA*>(this_ptr);
      return (p->operator())(arg...);
    } //lambda_stub

    friend class multicast_delegate<RET(PARAMS...)>;
    typename delegate_base<RET(PARAMS...)>::InvocationElement invocation;

  }; //class delegate

} /* namespace cppsl::threading */

#endif /* FBD1FFCB_9AD2_49DE_9E83_A5212FE3F926 */
