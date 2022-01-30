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
 * \file        DelegateBase.hpp
 * \brief       The Delegate base type is used to store in the queue, and reflects
 * the profile of the synchronization object and its thread.
 * \ingroup     Thread Wrapper for Modern C++
 * \date        2021-10-24
 *****************************************************************************/

#ifndef E3BE096C_F46D_4BA5_9B07_4B3ED78BEE5D
#define E3BE096C_F46D_4BA5_9B07_4B3ED78BEE5D

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
  class DelegateBase;

  template<typename RET, typename ...PARAMS>
  class DelegateBase<RET(PARAMS...)> {

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

  }; //class DelegateBase

} /* namespace cppsl::threading */

#endif /* E3BE096C_F46D_4BA5_9B07_4B3ED78BEE5D */
