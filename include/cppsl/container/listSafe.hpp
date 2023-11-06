//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
* @file    list_threadsafe.h
* @brief   thread safe list class.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#ifndef INCLUDE_CPPSL_CONCURENCY_LIST_LOCK_HPP
#define INCLUDE_CPPSL_CONCURENCY_LIST_LOCK_HPP

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <memory>
#include <mutex>

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::container {

    template<typename T>
    class ListSafe {
        struct node {
            std::mutex            m_mutex;
            std::shared_ptr<T>    m_dataSp;
            std::unique_ptr<node> m_next;

            node(): m_next() {}

            node(T const& value) : m_dataSp(std::make_shared<T>(value)) {}
        };

        node head;

    public:
     ListSafe() {}

        ~ListSafe()
        {
            remove_if( [](T const&) {return true;} );
        }

        /// @breaf    clean.
        ///
        /// @details  removes all from list
        ///
        void clean()
        {
            remove_if( [](T const&) {return true;} );
        }

        /// @breaf    pushes in front.
        ///
        /// @details  implementation of threadsafe_list<T>::push_front
        ///

        void push_front(T const& value)
        {
            std::unique_ptr<node> new_node( new node(value) );

            std::lock_guard<std::mutex> lk(head.m_mutex);
            new_node->m_next = std::move(head.m_next);
            head.m_next      = std::move(new_node);
        }

        /// @breaf    runs for each.
        ///
        /// @details  implementation of for each in stack
        ///
        template<typename Function>
        void for_each(Function f)
        {
            node *current = &head;

            std::unique_lock<std::mutex> lk(head.m_mutex);
            while (node *const m_next = current->m_next.get()) {
                std::unique_lock<std::mutex> next_lk(m_next->m_mutex);
                lk.unlock();
                f(*m_next->m_dataSp);
                current = m_next;
                lk = std::move(next_lk);
            }
        }

        /// @breaf    finds first if condition.
        ///
        /// @details  implementation of find first by condition
        ///
        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            node *current = &head;

            std::unique_lock<std::mutex> lk(head.m_mutex);
            while (node *const m_next = current->m_next.get()) {
                std::unique_lock<std::mutex> next_lk(m_next->m_mutex);
                lk.unlock();
                if ( p(*m_next->m_dataSp) ) {
                    return m_next->m_dataSp;
                }
                current = m_next;
                lk = std::move(next_lk);
            }
            return std::shared_ptr<T>();
        }

        /// @breaf    removes if condition.
        ///
        /// @details  implementation of remove by condition
        ///
        template<typename Predicate>
        void remove_if(Predicate p)
        {
            node *current = &head;
            std::unique_lock<std::mutex> lk(head.m_mutex);
            while (node *const m_next = current->m_next.get()) {
                std::unique_lock<std::mutex> next_lk(m_next->m_mutex);
                if (p(*m_next->m_dataSp)) {
                    std::unique_ptr<node> old_next = std::move(current->m_next);
                    current->m_next = std::move(m_next->m_next);
                    next_lk.unlock();
                } else {
                    lk.unlock();
                    current = m_next;
                    lk = std::move(next_lk);
                }
            }
        }
    private:
     ListSafe(ListSafe const& other)
        {
            throw std::logic_error("The method not implemented.");
        }
     ListSafe& operator=(ListSafe const& other)
        {
            throw std::logic_error("The method not implemented.");
        }
    };

} // namespace cppsl::container

#endif /* INCLUDE_CPPSL_CONCURENCY_LIST_LOCK_HPP */