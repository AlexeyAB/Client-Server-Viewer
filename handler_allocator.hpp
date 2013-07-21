/**
 * @file   handler_allocator.hpp
 * @author Alexey Bochkovskiy <alexeyab84@gmail.com>
 *
 * @brief Custom memory allocator for reuse once allocated memory.
 *
 *
 */
// ----------------------------------------------------------------------------
#ifndef HANDLER_ALLOCATOR_HPP
#define HANDLER_ALLOCATOR_HPP
// ----------------------------------------------------------------------------
#include <boost/bind.hpp>
#include <boost/aligned_storage.hpp>
#include <boost/noncopyable.hpp>

// ----------------------------------------------------------------------------

/// Class to manage the memory to be used for handler-based custom allocation.
/// It contains a single block of memory which may be returned for allocation
/// requests. If the memory is in use when an allocation request is made, the
/// allocator delegates allocation to the global heap.
template<size_t size = 1024>
class T_handler_allocator
  : private boost::noncopyable
{
public:
  T_handler_allocator()
    : in_use_(false)
  {
  }

  void* allocate(std::size_t size)
  {
    if (!in_use_ && size < storage_.size)
    {
      in_use_ = true;
      return storage_.address();
    }
    else
    {
      return ::operator new(size);
    }
  }

  void deallocate(void* pointer)
  {
    if (pointer == storage_.address())
    {
      in_use_ = false;
    }
    else
    {
      ::operator delete(pointer);
    }
  }

private:
  // Storage space used for handler-based custom memory allocation.
  boost::aligned_storage<size> storage_;

  // Whether the handler-based custom allocation storage has been used.
  bool in_use_;
};
// ----------------------------------------------------------------------------

/// Wrapper class template for handler objects to allow handler memory
/// allocation to be customised. Calls to operator() are forwarded to the
/// encapsulated handler.
template <typename T_handler>
class T_custom_alloc_handler
{
public:
  T_custom_alloc_handler(T_handler_allocator<>& a, T_handler h)
    : allocator_(a),
      handler_(h)
  {
  }

  template <typename Arg1>
  void operator()(Arg1 arg1)
  {
    handler_(arg1);
  }

  template <typename Arg1, typename Arg2>
  void operator()(Arg1 arg1, Arg2 arg2)
  {
    handler_(arg1, arg2);
  }

  friend void* asio_handler_allocate(std::size_t size,
      T_custom_alloc_handler<T_handler>* this_handler)
  {
    return this_handler->allocator_.allocate(size);
  }

  friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/,
      T_custom_alloc_handler<T_handler>* this_handler)
  {
    this_handler->allocator_.deallocate(pointer);
  }

private:
  T_handler_allocator<>& allocator_;
  T_handler handler_;
};
// ----------------------------------------------------------------------------

/// Helper function to wrap a handler object to add custom allocation.
template <typename T_handler>
inline T_custom_alloc_handler<T_handler> make_custom_alloc_handler(
    T_handler_allocator<>& a, T_handler h)
{
  return T_custom_alloc_handler<T_handler>(a, h);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
#endif // HANDLER_ALLOCATOR_HPP
