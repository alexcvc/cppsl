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
 * \file        interlockProperty.hh
 * \brief       any intern lockable property.
 * \date        2021-10-24
 *****************************************************************************/

#ifndef E2F72EB7_0A8D_43F1_9098_74FCA82E55E3
#define E2F72EB7_0A8D_43F1_9098_74FCA82E55E3

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <mutex>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

namespace cppsl::threading {

  /**
   * Class implements object inter locked object with type T .
   */
  template<typename T>
  class InterlockedProperty {
   public:

    InterlockedProperty() : InterlockedProperty(nullptr, nullptr) {}

    InterlockedProperty(const T &value) : InterlockedProperty(nullptr, &value) {}

    InterlockedProperty(std::mutex &sharedMutex) : InterlockedProperty(&sharedMutex, nullptr) {}

    InterlockedProperty(std::mutex &sharedMutex, const T &value) : InterlockedProperty(&sharedMutex, &value) {}

    /// operator=
    InterlockedProperty &operator=(InterlockedProperty &other) {
      this->m_mutex = other.m_mutex;
      this->m_value = other.m_value;
      return *this;
    }

    void useSharedMutex(std::mutex &mutex) {
      this->m_mutex = mutex;
    }

    /// operator getter
    operator T() const {
      std::lock_guard<std::mutex> lock(*m_mutex);
      return m_value;
    }

    /// operator setter
    T operator=(const T &value) {
      std::lock_guard<std::mutex> lock(*m_mutex);
      return this->m_value = value;
    }

   private:

    InterlockedProperty(std::mutex *sharedMutex, const T *value) {
      if (sharedMutex == nullptr)
        m_mutex = &m_uniqueMutex;
      else
        m_mutex = sharedMutex;
      if (value != nullptr) // don't use mutex to interlock value here: it could be not yet fully constructed
        this->m_value = *value;
    }

    std::mutex m_uniqueMutex;
    std::mutex *m_mutex;
    T m_value;

  };

} /* namespace cppsl::threading */

#endif /* E2F72EB7_0A8D_43F1_9098_74FCA82E55E3 */
