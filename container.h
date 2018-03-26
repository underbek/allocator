  #include "iterator.h"
  
  template<typename T>
  struct element {
    element* next = nullptr;
    T data;
  };

template<
  typename T,
  class Allocator = std::allocator<element<T>>
> class my_container {

  Allocator alloc;
  size_t count = 0;
  element<T>* front_el = nullptr;
  element<T>* end_el = nullptr;

public:

  using iterator = my_iterator<element<T>>;

  my_container(std::initializer_list<T> values) {
    for (auto value : values)
      push(value);
  }

  ~my_container() {
    while (count) {
      auto next_el = front_el;
      for (int i = 1; i < count; i++) {
        next_el = next_el->next;
      }
      end_el = next_el;
      alloc.destroy(end_el);
      alloc.deallocate(end_el,1);
      count--;
    }
  }

  void push(T data) {
    if (count) {
      end_el->next = alloc.allocate(1);
      alloc.construct(end_el->next, *end_el->next);
      end_el = end_el->next;
      end_el->data = data;
    } else {
      front_el = alloc.allocate(1);
      alloc.construct(front_el, *front_el);
      front_el->data = data;
      end_el = front_el;
    }
    count++;
  }

  T front() {
    if (count)
      return front_el->data;
  }

  T back() {
    if (count)
      return end_el->data;
  }

  iterator begin() {
    return iterator(front_el);
  }

  iterator end() {
    return iterator(end_el->next);
  }

  size_t size() {
    return count;
  }
};