#include <array>
#include <list>
#include <algorithm>

namespace my {

template<typename T,size_t N = 1>
class allocator {

  struct memory_control_struct {
    memory_control_struct(T* p) {
      is_free.fill(false);
      ptr_begin = p;
      count++;
      is_free[0] = true;
    }
    T* ptr_begin;
    uint32_t count = 0;
    std::array<bool,N> is_free;
  };

  std::list<memory_control_struct> control;

public:
  using value_type = T;

  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  template<typename U>
  struct rebind {
    using other = allocator<U,N>;
  };

  T* allocate(std::size_t n) {
    
    T* ptr;

    auto free_block = std::find_if(control.begin(),control.end(),[](memory_control_struct element) {
      return element.count != N;
    });
    
    if(free_block == control.end()) {
      ptr = reinterpret_cast<T *>(std::malloc(N*n*sizeof(T)));
      if (!ptr)
        throw std::bad_alloc();
      control.emplace_back(ptr);
    } else {
      auto free_cell = std::find_if(free_block->is_free.begin(),free_block->is_free.end(),[](bool element){
        return !element;
      });

      int number_free_cell = free_cell - free_block->is_free.begin();

      ptr = free_block->ptr_begin + number_free_cell;

      *free_cell = true;
      free_block->count++;
    }
    return ptr;
  }

  void deallocate(T* ptr,std::size_t n) {

    if (control.empty()) 
      return;

    auto block = std::find_if(control.begin(),control.end(),[ptr](memory_control_struct element) {
      return (ptr >= element.ptr_begin && ptr < element.ptr_begin + N);
    });

    if (block->count == 1) {
      std::free(block->ptr_begin);
      control.erase(block);
    } else {
      block->count--;
      block->is_free[ptr - block->ptr_begin] = false;
    }
  }

  template<typename U, typename ... Args>
  void construct(U *ptr, Args&& ... args) {
    new(ptr) U(std::forward<Args>(args)...);
  }

  void destroy(T* ptr) {
    ptr->~T();
  }
};

}