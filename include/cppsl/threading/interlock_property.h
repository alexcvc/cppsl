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
 * \file        InterlockProperty.hpp
 * \brief       any intern lockable property.
 * \date        2021-10-24
 *****************************************************************************/

#ifndef C4B7D4ED_1C1B_4825_879B_F02919F82E03
#define C4B7D4ED_1C1B_4825_879B_F02919F82E03

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
  class InterlockProperty {
   public:

    InterlockProperty() : InterlockProperty(nullptr, nullptr) {}

    InterlockProperty(const T &value) : InterlockProperty(nullptr, &value) {}

    InterlockProperty(std::mutex &sharedMutex) : InterlockProperty(&sharedMutex, nullptr) {}

    InterlockProperty(std::mutex &sharedMutex, const T &value) : InterlockProperty(&sharedMutex, &value) {}

    /// operator=
    InterlockProperty &operator=(InterlockProperty &other) {
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

    InterlockProperty(std::mutex *sharedMutex, const T *value) {
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

#endif /* C4B7D4ED_1C1B_4825_879B_F02919F82E03 */
