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
 * \file        delegateBase.hh
 * \brief       The delegate base type is used to store in the queue, and reflects
 * the profile of the synchronization object and its thread.
 * \ingroup     Thread Wrapper for Modern C++
 * \date        2021-10-24
 *****************************************************************************/

#ifndef A0C4EFF7_43C9_4303_A815_9C2361E280C5
#define A0C4EFF7_43C9_4303_A815_9C2361E280C5

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

namespace cppsl::threading {

  template<typename T>
  class delegate_base;

  template<typename RET, typename ...PARAMS>
  class delegate_base<RET(PARAMS...)> {

   protected:

    using stub_type = RET(*)(void *this_ptr, PARAMS...);

    struct InvocationElement {
      InvocationElement() = default;
      InvocationElement(void *this_ptr, stub_type aStub) : object(this_ptr), stub(aStub) {}
      void Clone(InvocationElement &target) const {
        target.stub = stub;
        target.object = object;
      } //Clone
      bool operator==(const InvocationElement &another) const {
        return another.stub == stub && another.object == object;
      } //==
      bool operator!=(const InvocationElement &another) const {
        return another.stub != stub || another.object != object;
      } //!=
      void *object = nullptr;
      stub_type stub = nullptr;
    }; //InvocationElement

  }; //class delegate_base

} /* namespace cppsl::threading */

#endif /* A0C4EFF7_43C9_4303_A815_9C2361E280C5 */
