
/*************************************************************************//**
* @file    ThreadSafeList.hh
* @brief   thread safe list class.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#ifndef C9070EDD_CBB7_440E_BD9A_B13FC9BFA1A3
#define C9070EDD_CBB7_440E_BD9A_B13FC9BFA1A3

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
    class ThreadSafeList
    {
        struct node {
            std::mutex            mtx;
            std::shared_ptr<T>    data_ptr;
            std::unique_ptr<node> next;

            node(): next() {}

            node(T const& value) : data_ptr(std::make_shared<T>(value)) {}
        };

        node head;

    public:
        ThreadSafeList() {}

        ~ThreadSafeList()
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

            std::lock_guard<std::mutex> lk(head.mtx);
            new_node->next = std::move(head.next);
            head.next      = std::move(new_node);
        }

        /// @breaf    runs for each.
        ///
        /// @details  implementation of for each in stack
        ///
        template<typename Function>
        void for_each(Function f)
        {
            node *current = &head;

            std::unique_lock<std::mutex> lk(head.mtx);
            while (node *const next = current->next.get()) {
                std::unique_lock<std::mutex> next_lk(next->mtx);
                lk.unlock();
                f(*next->data_ptr);
                current = next;
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

            std::unique_lock<std::mutex> lk(head.mtx);
            while (node *const next = current->next.get()) {
                std::unique_lock<std::mutex> next_lk(next->mtx);
                lk.unlock();
                if ( p(*next->data_ptr) ) {
                    return next->data_ptr;
                }
                current = next;
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
            std::unique_lock<std::mutex> lk(head.mtx);
            while (node *const next = current->next.get()) {
                std::unique_lock<std::mutex> next_lk(next->mtx);
                if (p(*next->data_ptr)) {
                    std::unique_ptr<node> old_next = std::move(current->next);
                    current->next = std::move(next->next);
                    next_lk.unlock();
                } else {
                    lk.unlock();
                    current = next;
                    lk = std::move(next_lk);
                }
            }
        }
    private:
        ThreadSafeList(ThreadSafeList const& other)
        {
            throw std::logic_error("The method not implemented.");
        }
        ThreadSafeList& operator=(ThreadSafeList const& other)
        {
            throw std::logic_error("The method not implemented.");
        }
    };

} // namespace cppsl::container

#endif /* C9070EDD_CBB7_440E_BD9A_B13FC9BFA1A3 */
