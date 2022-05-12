#ifndef __dts__utils
#define __dst__utils

#include <utility>
#include <functional>

namespace dst {

// remove_reference -- strips lvalue and rvalue references

      template <typename T>
      struct remove_reference {
            typedef T type;
      };

      template <typename T>
      struct remove_reference<T&> {
            typedef T type;
      };

      template <typename T>
      struct remove_reference<T&&> {
            typedef T type;
      };

}; // namespace dst


namespace dst {

// move (value) -- transforms lvalue reference to an rvalue reference
      
      template <typename T>
      typename remove_reference<T>::type&& move (T&& value) {
            return static_cast<typename remove_reference<T>::type&&> (value);
      }

}; // namespace dst


namespace dst {

// forward - cast argument to the template type
// For the template deduction to work argument must be
// specified explicitly, which is a feature.

      template <typename T>
      T&& forward (typename remove_reference<T>::type& value) { // accepts only lvalue references
            return static_cast<T&&> (value);
      }

      template <typename T>
      T&& forward (typename remove_reference<T>::type&& value) { // accepts only rvalue references
            return static_cast<T&&> (value);
      }

}; // namespace dst

namespace dst {

// struct pair -- very useful structure

      template <typename T, typename U>
      using pair = std::pair<T,U>; 
 /*

      template <typename T, typename U> 
      struct pair {
            T first;
            U second;

            pair (const pair& other)
                 : first (other.first),
                   second (other.second) {}

            pair (pair&& other)
                 : first (dst::move (other.first)),
                   second (dst::move (other.second)) {}

            pair (const T& first, const T& second)
                 : first (first),
                   second (second) {}

            pair (const T& first, T&& second)
                 : first (first),
                   second (dst::move(second)) {}

            pair (T&& first, const T& second)
                 : first (dst::move(first)),
                   second (second) {}

            pair (T&& first, T&& second)
                 : first (dst::move(first)),
                   second (dst::move(second)) {}

            template <typename... Args>
            pair (const T& first, Args... args)
                 : first (first),
                   second (dst::forward<Args>(args)...) {}

            pair& operator= (const pair& other) {
                  first = other.first;
                  second = other.second;

                  return *this;
            }

            pair& operator= (pair&& other) {
                  first = dst::move (other.first);
                  second = dst::move (other.second);

                  return *this;
            }
      }; // struct pair
      
      template <typename T, typename U>
      bool operator== (const dst::pair<T,U>& left, const dst::pair<T,U>& right) {
            return (left.first == right.first) && (left.second == right.second);
      }

      template <typename T, typename U>
      bool operator!= (const dst::pair<T,U>& left, const dst::pair<T,U>& right) {
            return !(left.first == right.first) || !(left.second == right.second);
      }

      template <typename T, typename U>
      bool operator< (const dst::pair<T,U>& left, const dst::pair<T,U>& right) {
            if (left.first < right.first)
                  return true;
            
            if (right.first < left.first)
                  return false;
            
            return left.second < right.second;
      }

      template <typename T, typename U>
      bool operator> (const dst::pair<T,U>& left, const dst::pair<T,U>& right) {
            if (right.first < left.first)
                  return true;

            if (left.first < right.first)
                  return false;

            return right.second < left.second;
      } 
//      */

} // namespace dst

namespace dst {

      template <typename T>
      struct equal_to {
            bool operator() (const T& left, const T& right) const {
                  return left == right;
            }
      };
      
} //namespace dst


namespace dst {
      template <typename T>
      void swap (T& left, T& right) {
            T tmp (left);
            left = right;
            right = tmp;
      }
} // namespace dst 

namespace dst {

      template <bool b, typename T, typename U>
      struct conditional {
            typedef U type;
      };

      template <typename T, typename U>
      struct conditional<true, T, U> {
            typedef T type;
      };

      template <bool b, typename T, typename U>
      using conditional_t = typename conditional<b,T,U>::type;

} // namespace dst

#endif


#ifndef __dst__allocator_traits
#define __dst__allocator_traits

#include <memory>

namespace dst {

      template <typename Alloc>
      using allocator_traits = std::allocator_traits<Alloc>;

} // namespace dst

namespace dst {
      
      template <typename Alloc>
      using allocator = std::allocator<Alloc>;

} // namespace dst

#endif

#ifndef __dst__hash
#define __dst__hash

namespace dst {

      template <typename Key>
      using hash = std::hash<Key>;

} // namespace dst

#endif
#ifndef __dst__list
#define __dst__list

#include <memory>

namespace dst {
      template <typename T, class A = std::allocator<T>>
            class List {
                  private:
                        template <bool is_const>
                              class base_iterator;
                        struct node;

                        typedef typename std::allocator_traits<A>::template rebind_alloc<node> _A;

                        void __construct (const List& other, _A& allocator);
                        List (const List& other, _A& allocator);
                        List (List&& other, _A& allocator);

                  public:

                        using iterator = base_iterator<false>;
                        using const_iterator = base_iterator<true>;
                        using reverse_iterator = std::reverse_iterator<iterator>;
                        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

                        List ();
                        List (A allocator);
                        List (size_t size, A allocator = A());
                        List (size_t size, const T& value, A allocator = A());

                        _A get_allocator () const;

                        List (const List& other);
                        List (List&& other);
                        ~List ();

                        List& operator= (const List& other);
                        List& operator= (List&& other);

                        size_t size () const;

                        void push_back (const T& value);
                        void push_front (const T& value);

                        void pop_back ();
                        void pop_front ();

                        T& front ();

                        iterator begin ();
                        const_iterator begin () const;

                        iterator end ();
                        const_iterator end () const;

                        const_iterator cbegin () const;
                        const_iterator cend () const;

                        reverse_iterator rbegin ();
                        reverse_iterator rend ();

                        const_reverse_iterator rbegin () const;
                        const_reverse_iterator rend () const;

                        const_reverse_iterator crbegin () const;
                        const_reverse_iterator crend () const;

                        void insert (const_iterator it, const T&);
                        void erase (const_iterator it);
                        
                        void swap (List<T,A>& other);
                        void clear ();

                        bool empty () { return list_size == 0; }

                        template <typename... Args>
                        void emplace_front (Args&&... args);

                        void splice (const_iterator pos, List& other, const_iterator it);
                        void splice (const_iterator pos, List&& other, const_iterator it);

                  private:
                        struct base_node {
                              base_node* next;
                              base_node* prev;
                        };

                        struct node : public base_node {
                              T value;
                              
                              template <typename... Args>
                              node (Args&&... args);
                              ~node () = default;

                              node& operator= (const node& other) = delete;
                        };
                        
                        A alloc;
                        _A allocator;

                        mutable base_node void_node; // This is the end of all the things. And the beginning
                        size_t list_size;

            };

      template <typename T, class A>
      template <typename... Args>
            List<T,A>::node::node (Args&&... args)
                                  : value (dst::forward<Args> (args)...) {}

      template <typename T, class A>
            List<T,A>::List () : alloc (), allocator () {
                  
                  list_size = 0;

                  void_node.prev = &void_node;
                  void_node.next = &void_node;
            }

      template <typename T, class A>
            List<T,A>::List (A allocator) : alloc (allocator), allocator (allocator) {

                  list_size = 0;

                  void_node.prev = &void_node;
                  void_node.next = &void_node;
            }

      template <typename T, class A>
            List<T,A>::List (size_t size, A allocator) : alloc (allocator), allocator (allocator) {

                  list_size = size;

                  base_node* first = &void_node;
                  void_node.prev = &void_node;
                  void_node.next = &void_node;

                  for (size_t i = 0; i < size; ++i) {
                        
                        node* Node;

                        try {
                              Node = std::allocator_traits<_A>::allocate (this->allocator, 1);
                        } catch (...) { clear (); throw; }
                        
                        try {
                              std::allocator_traits<A>::construct (alloc, &(Node->value));
                        } catch (...) {
                              std::allocator_traits<_A>::deallocate (this->allocator, Node, 1);
                              clear (); throw; }

                        first->prev = Node;
                        Node->next = first;

                        void_node.next = Node;
                        Node->prev = &void_node;

                        first = Node;
                  }
            }

      template <typename T, class A>
            List<T,A>::List (size_t size, const T& value, A allocator) : alloc (allocator), 
                                                                         allocator (allocator) {
                  
                  list_size = size;

                  base_node* first = &void_node;
                  void_node.prev = &void_node;
                  void_node.next = &void_node;

                  for (size_t i = 0; i < size; ++i) {

                        node* Node;
                        
                        try {
                              Node = std::allocator_traits<_A>::allocate (this->allocator, 1);
                        } catch (...) { clear (); throw; }

                        try {
                              std::allocator_traits<A>::construct (alloc, &(Node->value), value);
                        } catch (...) {
                              std::allocator_traits<_A>::deallocate (this->allocator, Node, 1);
                              clear (); throw; }

                        first->prev = Node;
                        Node->next = first;

                        void_node.next = Node;
                        Node->prev = &void_node;

                        first = Node;
                  }
            }

      template <typename T, class A>
            typename List<T,A>::_A List<T,A>::get_allocator () const {
                  return allocator;
            }

      template <typename T, class A>
            void List<T,A>::__construct (const List<T,A>& other, List<T,A>::_A& allocator) {
                  
                  list_size = other.list_size;

                  void_node.prev = &void_node;
                  void_node.next = &void_node;

                  base_node* prev = &void_node;

                  base_node* src = other.void_node.next;

                  while (src != &other.void_node) {
                        node* Node;

                        try {
                              Node = std::allocator_traits<_A>::allocate (allocator, 1);
                        } catch (...) { clear (); throw; }

                        try {
                              std::allocator_traits<A>::construct
                              (alloc, &(Node->value), static_cast<node*>(src)->value);
                        
                        } catch (...) {
                              std::allocator_traits<_A>::deallocate (allocator, Node, 1);
                              clear (); throw; }

                        Node->prev = prev;
                        Node->next = &void_node;

                        prev->next = Node;
                        void_node.prev = Node;

                        src = src->next;
                        prev = Node;
                  }
            }

      template <typename T, class A>
            List<T,A>::List (const List<T,A>& other) : allocator
                  (std::allocator_traits<_A>::
                        select_on_container_copy_construction (other.allocator))
            {
                  __construct (other, this->allocator);
            }

      template <typename T, class A>
            List<T,A>::List (List&& other, _A& allocator) : alloc (other.alloc), allocator (allocator)
            {
                  list_size = other.list_size;

                  void_node.prev = other.void_node.prev;
                  (void_node.prev)->next = &(void_node);

                  void_node.next = other.void_node.next;
                  (void_node.next)->prev = &(void_node);

                  other.void_node.prev = &(other.void_node);
                  other.void_node.next = &(other.void_node);
            }

      template <typename T, class A>
            List<T,A>::List (List&& other) : 
                        alloc
                        (std::allocator_traits<A>::
                        select_on_container_copy_construction (other.alloc)),
                        
                        allocator
                        (std::allocator_traits<_A>::
                        select_on_container_copy_construction (other.allocator))
            {
                  list_size = other.list_size;

                  void_node.prev = other.void_node.prev;
                  (void_node.prev)->next = &(void_node);

                  void_node.next = other.void_node.next;
                  (void_node.next)->prev = &(void_node);

                  other.void_node.prev = &(other.void_node);
                  other.void_node.next = &(other.void_node);
            }


      template <typename T, class A>
            void List<T,A>::clear () {
                  base_node* PrevNode;
                  node* Node;

                  PrevNode = void_node.next;

                  while (PrevNode != &void_node) {
                        Node = static_cast<node*> (PrevNode);
                        PrevNode = Node->next;

                        std::allocator_traits<A>::destroy (alloc, &(Node->value));
                        std::allocator_traits<_A>::deallocate (allocator, Node, 1);
                  }

                  list_size = 0;
            }

      template <typename T, class A>
            List<T,A>::~List () {
                  clear ();
            }

      template <typename T, class A>
            void List<T,A>::swap (List<T,A>& other) {

                  base_node* Node;
                  base_node* OtherNode;

                  Node = void_node.prev;
                  OtherNode = other.void_node.prev;

                  Node->next = &(other.void_node);
                  OtherNode->next = &void_node;

                  Node = void_node.next;
                  OtherNode = other.void_node.next;

                  Node->prev = &(other.void_node);
                  OtherNode->prev = &void_node;

                  std::swap (void_node.prev, other.void_node.prev);
                  std::swap (void_node.next, other.void_node.next);

                  std::swap (list_size, other.list_size);
                  
//                  std::swap (allocator, other.allocator);
            }

      template <typename T, class A>
            List<T,A>::List (const List<T,A>& other, _A& allocator) : allocator (allocator) {
                  __construct (other, allocator);
            }

      template <typename T, class A>
            List<T,A>& List<T,A>::operator= (const List<T,A>& other) {
                  
                  if (std::allocator_traits<_A>::
                      propagate_on_container_copy_assignment::value &&
                      (other.allocator != allocator)) {

                        allocator = other.allocator;

                        List tmp (other, allocator);
                        swap (tmp);
                  }

                  else {
                        List tmp (other);
                        swap (tmp);
                  }

                  return *this;
            }

      template <typename T, class A>
            List<T,A>& List<T,A>::operator= (List&& other) {
                  
                  list_size = other.list_size;

                  void_node.prev = other.void_node.prev;
                  (void_node.prev)->next = &(void_node);

                  void_node.next = other.void_node.next;
                  (void_node.next)->prev = &(void_node);

                  other.void_node.prev = &(other.void_node);
                  other.void_node.next = &(other.void_node);
            }

      template <typename T, class A>
            size_t List<T,A>::size () const {
                  return list_size;
            }

      template <typename T, class A>
            void List<T,A>::push_back (const T& value) {

                  base_node* last = void_node.prev;
                  node* Node;
                  
                  try {
                        Node = std::allocator_traits<_A>::allocate (allocator, 1);
                  } catch (...) { throw; }

                  try {
                        std::allocator_traits<A>::construct (alloc, &(Node->value), value);
                  } catch (...) {
                        std::allocator_traits<_A>::deallocate (allocator, Node, 1);
                        throw;
                  }

                  Node->next = &void_node;
                  void_node.prev = Node;

                  Node->prev = last;
                  last->next = Node;

                  ++list_size;
            }

      template <typename T, class A>
            void List<T,A>::push_front (const T& value) {
                  
                  base_node* first = void_node.next;
                  node* Node;
                  
                  try {
                        Node = std::allocator_traits<_A>::allocate (allocator, 1);
                  } catch (...) { throw; }

                  try {
                        std::allocator_traits<A>::construct (alloc, &(Node->value), value);
                  } catch (...) {
                        std::allocator_traits<_A>::deallocate (allocator, Node, 1);
                        throw;
                  }

                  Node->prev = &void_node;
                  void_node.next = Node;

                  Node->next = first;
                  first->prev = Node;

                  ++list_size;
            }

      template <typename T, class A>
            T& List<T,A>::front () {
                  
                  node* first = static_cast<node*> (void_node.next);
                  return first->value;
            }

      template <typename T, class A>
            void List<T,A>::pop_back () {
                  
                  node* Node = static_cast<node*> (void_node.prev);
                  base_node* last = Node->prev;

                  last->next = &void_node;
                  void_node.prev = last;

                  std::allocator_traits<A>::destroy (alloc, &(Node->value));
                  std::allocator_traits<_A>::deallocate (allocator, Node, 1);

                  --list_size;
            }

      template <typename T, class A>
            void List<T,A>::pop_front () {
                  
                  node* Node = static_cast<node*> (void_node.next);
                  base_node* first = Node->next;

                  first->prev = &void_node;
                  void_node.next = first;
                  
                  std::allocator_traits<A>::destroy (alloc, &(Node->value));
                  std::allocator_traits<_A>::deallocate (allocator, Node, 1);

                  --list_size;
            }

      template <typename T, class A>
            typename List<T,A>::iterator List<T,A>::begin () {
                  iterator ans (void_node.next);

                  return ans;
            }

      template <typename T, class A>
            typename List<T,A>::iterator List<T,A>::end () {
                  iterator ans (&void_node);

                  return ans;
            }
      
      template <typename T, class A>
            typename List<T,A>::const_iterator List<T,A>::begin () const {
                  const_iterator ans (void_node.next);

                  return ans;
            }

      template <typename T, class A>
            typename List<T,A>::const_iterator List<T,A>::end () const {
                  const_iterator ans (&void_node);

                  return ans;
            }

      template <typename T, class A>
            typename List<T,A>::const_iterator List<T,A>::cbegin () const{
                  const_iterator ans (void_node.next);

                  return ans;
            }

      template <typename T, class A>
            typename List<T,A>::const_iterator List<T,A>::cend () const{
                  const_iterator ans (&void_node);

                  return ans;
            }

      template <typename T, class A>
            typename List<T,A>::reverse_iterator List<T,A>::rbegin () {
                  
                  return std::reverse_iterator<iterator> (end());
            }

      template <typename T, class A>
            typename List<T,A>::reverse_iterator List<T,A>::rend () {
                  
                  return std::reverse_iterator<iterator> (begin());
            }

      template <typename T, class A>
            typename List<T,A>::const_reverse_iterator List<T,A>::rbegin () const {
                  
                  return std::reverse_iterator<const_iterator> (cend());
            }

      template <typename T, class A>
            typename List<T,A>::const_reverse_iterator List<T,A>::rend () const {
                  
                  return std::reverse_iterator<const_iterator> (cbegin());
            }
      
      template <typename T, class A>
            typename List<T,A>::const_reverse_iterator List<T,A>::crbegin () const {
                  
                  return std::reverse_iterator<const_iterator> (cend());
            }

      template <typename T, class A>
            typename List<T,A>::const_reverse_iterator List<T,A>::crend () const {
                  
                  return std::reverse_iterator<const_iterator> (cbegin());
            }

      template <typename T, class A>
            void List<T,A>::insert (typename List<T,A>::const_iterator it, const T& value) {
                  
                  base_node* next = it.Node;

                  base_node* prev = next->prev;
                  node* new_node;

                  try {
                        new_node = std::allocator_traits<_A>::allocate (allocator, 1);
                  } catch (...) { throw; }

                  try {
                        std::allocator_traits<A>::construct (alloc, &(new_node->value), value);
                  } catch (...) {
                        std::allocator_traits<_A>::deallocate (allocator, new_node, 1);
                        throw;
                  }

                  new_node->prev = prev;
                  prev->next = new_node;

                  new_node->next = next;
                  next->prev = new_node;

                  ++list_size;
            }

      template <typename T, class A>
            void List<T,A>::erase (typename List<T,A>::const_iterator it) {
                  node* Node = reinterpret_cast<node*> (it.Node);

                  base_node* prev = Node->prev;
                  base_node* next = Node->next;

                  std::allocator_traits<A>::destroy (alloc, &(Node->value));
                  std::allocator_traits<_A>::deallocate (allocator, Node, 1);

                  prev->next = next;
                  next->prev = prev;

                  --list_size;
            }

      template <typename T, class A>
            void List<T,A>::splice (const_iterator pos, List& other, const_iterator it) {
                  base_node* prev = (pos.Node)->prev;
                  base_node* next = pos.Node;

                  base_node* other_prev = (it.Node)->prev;
                  base_node* other_next = (it.Node)->next;

                  base_node* moved = it.Node;
                  
                  other_prev->next = other_next;
                  other_next->prev = other_prev;

                  prev->next = moved;
                  next->prev = moved;

                  moved->prev = prev;
                  moved->next = next;

                  ++list_size;
                  --other.list_size;
            }

      template <typename T, class A>
            void List<T,A>::splice (const_iterator pos, List&& other, const_iterator it) {
                  base_node* prev = (pos.Node)->prev;
                  base_node* next = pos.Node;

                  base_node* other_prev = (it.Node)->prev;
                  base_node* other_next = (it.Node)->next;

                  base_node* moved = it.Node;
                  
                  other_prev->next = other_next;
                  other_next->prev = other_prev;

                  prev->next = moved;
                  next->prev = moved;

                  moved->prev = prev;
                  moved->next = next;

                  ++list_size;
                  --other.list_size;
            }

      template <typename T, class A>
      template <typename... Args>
            void List<T,A>::emplace_front (Args&&... args) {
                  base_node* first = void_node.next;
                  node* Node;
                  
                  try {
                        Node = std::allocator_traits<_A>::allocate (allocator, 1);
                  } catch (...) { throw; }

                  try {
                        std::allocator_traits<A>::construct (alloc, &(Node->value),
                                                              dst::forward<Args> (args)...);
                  } catch (...) {
                        std::allocator_traits<_A>::deallocate (allocator, Node, 1);
                        throw;
                  }

                  Node->prev = &void_node;
                  void_node.next = Node;

                  Node->next = first;
                  first->prev = Node;

                  ++list_size;
            }

/*********************************************************************************************\
 *                                                                                           *
 *                                                                                           *
 *                                                                                           *
 *                                                                                           *
\*********************************************************************************************/

      template <typename T, class A>
      template <bool is_const>
            class List<T,A>::base_iterator {
                  friend class List;

                  friend class base_iterator<true>;

                  public:

                        using iterator_category = std::bidirectional_iterator_tag;
                        using difference_type = long long;

                        using value_type = conditional_t<is_const, const T, T>;
                        using pointer = value_type*;
                        using reference = value_type&;

                        base_iterator () = default;
                        base_iterator (base_node*);
                        base_iterator (const base_iterator& other);
                        ~base_iterator () = default;

                        base_iterator& operator= (const base_iterator& other);

                        reference operator* ();
                        pointer operator-> ();

                        base_iterator& operator++ ();
                        base_iterator  operator++ (int);

                        base_iterator& operator-- ();
                        base_iterator  operator-- (int);

                        bool operator== (const base_iterator& other) const;
                        bool operator!= (const base_iterator& other) const;
                        
                        operator base_iterator<true> () const;
                  private:

                        base_node* Node;
            };

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>::base_iterator (List<T,A>::base_node* Node) {

                  this->Node = Node;
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>::base_iterator
                                          (const List<T,A>::base_iterator<is_const>& other) {
                  Node = other.Node;
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>& List<T,A>::base_iterator<is_const>::operator=
                                          (const List<T,A>::base_iterator<is_const>& other) {
                  Node = other.Node;
                  return *this;
            }

      template <typename T, class A>
      template <bool is_const>
            typename List<T,A>::template base_iterator<is_const>::reference
                  List<T,A>::base_iterator<is_const>::operator* () {
                  
                  node* res_node = static_cast<node*> (Node);
                  return res_node->value;
            }

      template <typename T, class A>
      template <bool is_const>
            typename List<T,A>::template base_iterator<is_const>::pointer
                  List<T,A>::base_iterator<is_const>::operator-> () {
                  
                  node* res_node = static_cast<node*> (Node);

                  return &(res_node->value);
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>&
                  List<T,A>::base_iterator<is_const>::operator++ () {
                  
                  Node = Node->next;
                  return *this;
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>
                  List<T,A>::base_iterator<is_const>::operator++ (int) {
                  
                  base_iterator res (*this);
                  Node = Node->next;

                  return res;
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>&
                  List<T,A>::base_iterator<is_const>::operator-- () {
                  
                  Node = Node->prev;
                  return *this;
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>
                  List<T,A>::base_iterator<is_const>::operator-- (int) {
                  
                  base_iterator res (*this);
                  Node = Node->prev;

                  return res;
            }

      template <typename T, class A>
      template <bool is_const>
            bool List<T,A>::base_iterator<is_const>::operator==
                                          (const List<T,A>::base_iterator<is_const>& other) const {
                  
                  return (Node == other.Node);
            }

      template <typename T, class A>
      template <bool is_const>
            bool List<T,A>::base_iterator<is_const>::operator!=
                                          (const List<T,A>::base_iterator<is_const>& other) const {
                  
                  return (Node != other.Node);
            }

      template <typename T, class A>
      template <bool is_const>
            List<T,A>::base_iterator<is_const>::operator List<T,A>::base_iterator<true> () const {
                  return base_iterator<true> (Node);
            }
}

namespace dst {
      template <typename T, class A = std::allocator<T>>
      using list = List<T,A>;
}

#endif
#ifndef __dst__unordered_map
#define __dst__unordered_map

//#include "utility.h"
//#include "allocator_traits.h"
//#include "list.h"
//#include "hash.h"

#include <cmath>
#include <iostream>

 /*

#include <list>

namespace dst {
      template <typename T, typename Alloc = dst::allocator<T>>
      using list = std::list<T,Alloc>;
};

// */

namespace dst {
      
//      namespace {
            template <typename Key, typename T,
                      typename Hash = dst::hash<Key>,
                      typename Equal = dst::equal_to<Key>,
                      typename Allocator = dst::allocator< dst::pair<const Key, T> >
            > class __hash_table {

                  protected:

                        static const size_t __default_bucket_count = 10;
                        static constexpr float __max_load_factor = 1.0;
                        static constexpr float __rehash_factor = 2.0;

                        float __load_factor;

                  private:

                        template <bool is_const>
                        class __base_iterator;

                  public:
                        using value_type = dst::pair<const Key, T>;
                        //typedef dst::pair<const Key, T> value_type;
                        typedef value_type& reference_type;
                        
                        typedef __base_iterator<false> iterator;
                        typedef __base_iterator<true> const_iterator;

                        __hash_table ( size_t bucket_count,
                                       const Hash& hash = Hash(),
                                       const Equal& equal = Equal(),
                                       const Allocator& alloc = Allocator() )

                                     : hash (hash), equal (equal), v_alloc (alloc),
                                       b_alloc (alloc), s_alloc (alloc), i_alloc (alloc),
                                       __used (s_alloc), __swp_list (b_alloc),
                                       __iter_list (i_alloc) {

                              __array_size = bucket_count;
                              __array = bucket_traits::allocate (b_alloc, __array_size);

                              for (size_t bucket = 0; bucket < __array_size; ++bucket)
                                    __array[bucket].it = __used.end();

                              __empty = __used.end();

                              __size = 0;
                              __load_factor = 0.0;
                        }

                        __hash_table (const __hash_table& other)
                                     
                                     : hash (other.hash), equal (other.equal),
                                       v_alloc (other.v_alloc), b_alloc (other.b_alloc),
                                       s_alloc (other.s_alloc), i_alloc (other.i_alloc),
                                       __used (other.__used), __swp_list (other.__swp_list),
                                       __iter_list (other.__iter_list) {
                              
                              __array_size = other.__array_size;
                              __array = bucket_traits::allocate (b_alloc, __array_size);

                              for (size_t bucket = 0; bucket < __array_size; ++bucket)
                                    __array[bucket].it = __used.end();

                              for (size_t bucket = 0; bucket < __array_size; ++bucket) {
                                    if (other.__array[bucket].it != other.__empty) {
                                          bucket_traits::construct (b_alloc, __array + bucket,
                                                                    *(other.__array + bucket));
                                    }
                              }
                              
                              __empty = __used.end();

                              __size = other.__size;
                              __load_factor = other.__load_factor;
                        }
// */
                        __hash_table (__hash_table&& other)
                                     : hash (dst::move (other.hash)),
                                       equal (dst::move (other.equal)),
                                       v_alloc (dst::move (other.v_alloc)),
                                       b_alloc (dst::move (other.b_alloc)),
                                       s_alloc (dst::move (other.s_alloc)),
                                       i_alloc (dst::move (other.i_alloc)),
                                       __used (dst::move (other.__used)),
                                       __swp_list (dst::move (other.__swp_list)),
                                       __iter_list (other.__iter_list) {
                              
                              __array_size = other.__array_size;
                              __array = other.__array;

                              __empty = other.__empty;

                              __size = other.__size;
                              __load_factor = other.__load_factor;
                        }

                  private:

                        template <bool is_moved>
                        using swp_ref = typename dst::conditional<is_moved,
                                                         __hash_table&&,
                                                         __hash_table&
                        >::type;
                  
                  public:
                        void swap (__hash_table& other) {

                              dst::swap (hash, other.hash);
                              dst::swap (equal, other.equal);

                              dst::swap (v_alloc, other.v_alloc);
                              dst::swap (b_alloc, other.b_alloc);
                              dst::swap (s_alloc, other.s_alloc);
                              dst::swap (i_alloc, other.i_alloc);

                              __used.swap (other.__used);
                              __swp_list.swap (other.__swp_list);
                              __iter_list.swap (other.__iter_list);

                              dst::swap (__array_size, other.__array_size);
                              dst::swap (__array, other.__array);

                              dst::swap (__empty, other.__empty);
                              dst::swap (__size, other.__size);
                              dst::swap (__load_factor, other.__load_factor);
                        }

                  public:

                        __hash_table& operator= (const __hash_table& other) {
                              __hash_table swp (other);
                              swp.swap (*this);

                              return *this;
                        }

                        __hash_table& operator= (__hash_table&& other) {
                              __hash_table swp(dst::move (other));
                              swp.swap (*this);

                              return *this;
                        }
/*
                        __hash_table& operator= (const __hash_table& other) {
                              
                        } */

                        // void rehash (size_t count);
                        
                        iterator find (const Key& key) { return __find<false> (key).first; };
                        iterator find (Key&& key) { return __find<true> (dst::move(key)).first; };

                        const_iterator find (const Key& key) const 
                                                { return __find<false> (key).first; };

                        const_iterator find (Key&& key) const 
                                                { return __find<true> (dst::move(key)).first; };

                        // returns iterator to value_type if key is in table
                        // or creates it.

                        iterator create (const Key& key);
                        iterator create (Key&& key);

                        template <typename... Args>
                        dst::pair<iterator, bool> emplace (const value_type& v, Args&&... args);

                        // template <typename... Args>
                        // dst::pair<bool, iterator> emplace (Args&&... args);

                        // void erase (iterator iter);
                        // void erase (iterator first, iterator last);

                        // template <typename InputIt>
                        // void insert (InputIt first, InputIt last);

                        /*
                        dst::pair<bool, iterator> insert (const value_type& v) {
                              return emplace (v);
                        }

                        dst::pair<bool, iterator> insert (value_type&& v) {
                              return emplace (dst::move (v));
                        }*/

                        struct exception {};

                        size_t size () const { return __size; };

                  private:
                        struct bucket_type;

                        typedef typename dst::allocator_traits<Allocator>::template
                                         rebind_alloc<value_type> value_alloc;

                        typedef typename dst::allocator_traits<Allocator>::template
                                         rebind_alloc<bucket_type> bucket_alloc;

                        typedef typename dst::allocator_traits<bucket_alloc> bucket_traits;

                        typedef typename dst::allocator_traits<Allocator>::template
                                         rebind_alloc<size_t> size_alloc;

                        typedef typename dst::allocator_traits<size_alloc> size_traits;

                        typedef typename dst::allocator_traits<Allocator>::template
                                         rebind_alloc<bucket_type*> ptr_alloc;

                        typedef typename dst::allocator_traits<Allocator>::template
                                         rebind_alloc<iterator> iter_alloc;

                        Hash hash;
                        Equal equal;
                        
                        value_alloc  v_alloc;
                        bucket_alloc b_alloc;
                        size_alloc   s_alloc;
                        ptr_alloc    p_alloc;
                        iter_alloc   i_alloc;

                        dst::list<bucket_type*, ptr_alloc> __used;
                        typename decltype(__used)::iterator __empty;

                        typedef dst::list<value_type, value_alloc> list_type;
                        typename list_type::iterator __empty_bucket;

                        struct bucket_type {
                              list_type list;
                              typename decltype(__used)::iterator it;

                              bucket_type (const value_alloc& v_alloc) : list (v_alloc) {}
                        };

                        list_type __swp_list;
                        dst::list<iterator, iter_alloc> __iter_list;
                        
                        size_t __array_size;
                        bucket_type* __array;

                        size_t __size;

                        template <bool is_const>
                        class __base_iterator {
                              friend __hash_table;
                              friend __base_iterator<true>;

                              public:
                                    using value_type =  typename dst::conditional<is_const,
                                                        const dst::pair<const Key, T>,
                                                        dst::pair<const Key, T>>::type;

                                    using reference = value_type&;
                                    using pointer = value_type*;

                                    using iterator_category = std::forward_iterator_tag;
                                    using difference_type = size_t;

                                    __base_iterator () {};

                                    __base_iterator (const __base_iterator& other)
                                                    : it (other.it),
                                                      bucket_it (other.bucket_it),
                                                      end_it (other.end_it) {}

                                    __base_iterator& operator= (const __base_iterator& other) {
                                          it = other.it;
                                          bucket_it = other.bucket_it;
                                          end_it = other.end_it;

                                          return *this;
                                    }

                                    __base_iterator& operator++ () {
                                          if (bucket_it == (*it)->list.end()) {
                                                ++it;
                                                bucket_it = (*it)->list.begin();

                                                return *this;
                                          }

                                          ++bucket_it;

                                          if (bucket_it == (*it)->list.end()) {
                                                ++it;

                                                if (it != end_it)
                                                      bucket_it = (*it)->list.begin();
                                          }

                                          return *this;
                                    }

                                    __base_iterator operator++ (int) {
                                          iterator res (*this);
                                          ++(*this);

                                          return res;
                                    }

                                    reference operator* () {
                                          return *bucket_it;
                                    }

                                    pointer operator-> () {
                                          return &(*bucket_it);
                                    }

                                    operator __base_iterator<true> () {
                                          return __base_iterator<true> (it, bucket_it, end_it);
                                    }

                              private:
                                    typename dst::conditional<is_const,
                                                 typename decltype(__used)::const_iterator,
                                                 typename decltype(__used)::iterator
                                    >::type it;

                                    typename dst::conditional<is_const,
                                                 typename list_type::const_iterator,
                                                 typename list_type::iterator
                                    >::type bucket_it;

                                    decltype(it) end_it;

                                    __base_iterator (const decltype(it)& it,
                                                     const decltype(bucket_it)& bucket_it,
                                                     const decltype(end_it)& end_it)
                                                    : it (it), bucket_it (bucket_it),
                                                      end_it (end_it) {}

                              friend bool operator== (const __base_iterator& left,
                                                      const __base_iterator& right) {
                                    
                                    if (left.end_it != right.end_it)
                                          return false;

                                    if (left.it == left.end_it && right.it == right.end_it)
                                          return true;

                                    return (left.it == right.it) &&
                                           (left.bucket_it == right.bucket_it);
                              }

                              friend bool operator!= (const __base_iterator& left,
                                                      const __base_iterator& right) {
                                    return !(left == right);
                              };
                        };
                        

                        void __create_bucket (size_t bucket,
                                              decltype (__array) array,
                                              decltype (__used)& used) {

                              used.push_front (array + bucket);
                              try {
                                    bucket_traits::construct (b_alloc, 
                                                              array + bucket, v_alloc);

//                                    array[bucket].it = used.front ();
                              } catch (...) {
                                    used.pop_front ();
                                    throw;
                              }
                        }

                        void __create_bucket (size_t bucket) {
                              __create_bucket (bucket, __array, __used);
                        }

                        void __destroy_bucket (decltype(__used)& used) {
                              bucket_traits::destroy (b_alloc, used.front());
                              used.pop_front();
                        }

                        void __destroy_bucket () {
                              __destroy_bucket (__used);
                        }

                        template <bool is_moved>
                        struct KeyRef {
                              typedef typename dst::conditional<is_moved,
                                                                Key&&,
                                                                const Key&
                              >::type type;
                        };

                        template <bool is_moved>
                        struct VTRef {
                              typedef typename dst::conditional<is_moved,
                                                                Key&&,
                                                                const Key&
                              >::type type;
                        };

                  public:

                        void rehash (size_t count) {
                              size_t __tmp_array_size = count;
                              bucket_type* __tmp_array;

                              decltype(__used) __tmp_used (p_alloc);

                              bool tracked = !__iter_list.empty();

                              __tmp_array = bucket_traits::allocate (b_alloc, __tmp_array_size);

                              for (size_t bucket = 0; bucket < __tmp_array_size; ++bucket)
                                    __tmp_array[bucket].it = __tmp_used.end ();

                              try {
                                    size_t* hash_array = size_traits::allocate (s_alloc, __size);
                                    size_t hash_top = 0;
                                    
                                    try {
                                          for (auto it = __used.begin(); it != __used.end(); ++it) {
                                                auto& list = (*it)->list;

                                                while (!list.empty()) {
                                                      auto it = list.begin();
                                                      size_t hash_value = hash (it->first);

                                                      hash_array[hash_top++] = hash_value;

                                                      size_t bucket_number = 
                                                            hash_value % __tmp_array_size;

                                                      if (__tmp_array[bucket_number].it ==
                                                          __tmp_used.end()) {
                                                            
                                                            __create_bucket (bucket_number,
                                                                             __tmp_array,
                                                                             __tmp_used);

                                                            __tmp_array[bucket_number].it =
                                                                              __tmp_used.begin ();
                                                      }

                                                      auto& tmp_list = 
                                                            __tmp_array[bucket_number].list;
                                                      tmp_list.splice (tmp_list.begin(), list, it);

                                                      if (tracked)
                                                            if (it == __iter_list.front().bucket_it)
                                                                  __iter_list.front().it = 
                                                                  __tmp_array[bucket_number].it;
                                                }
                                          }
                                    } catch (...) {
                                          for (auto it = __tmp_used.begin();
                                                    it != __tmp_used.end(); ++it) {
                                                
                                                auto& tmp_list = (*it)->list;

                                                while (!tmp_list.empty()) {
                                                      auto it = tmp_list.begin();
                                                      size_t hash_value = hash_array[--hash_top];

                                                      size_t bucket = hash_value % __array_size;
                                                      auto& list = __array[bucket].list;

                                                      list.splice (list.begin(), tmp_list, it);
                                                }

                                                bucket_traits::destroy (b_alloc, *it);
                                          }

                                          size_traits::deallocate (s_alloc, hash_array, __size);
                                          throw;
                                    }

                              } catch (...) {
                                    bucket_traits::deallocate (b_alloc, __tmp_array,
                                                               __tmp_array_size);
                                    throw;
                              }

                              for (size_t bucket = 0; bucket < __array_size; ++bucket)
                                    if (__array[bucket].it != __empty)
                                          bucket_traits::destroy (b_alloc, __array + bucket);

                              __empty = __tmp_used.end();

                              __used.swap (__tmp_used);
                              dst::swap (__array_size, __tmp_array_size);
                              dst::swap (__array, __tmp_array);

                              bucket_traits::deallocate (b_alloc, __tmp_array, __tmp_array_size);

                              if (tracked)
                                    __iter_list.front().end_it = __used.end ();

                              __load_factor = float(__size) / __array_size;
                        }

                  private:
// emplace without rehashing
                        template <typename... Args>
                        dst::pair<iterator, bool> __emplace (Args&&... args) {

                              __swp_list.emplace_front (dst::forward<Args>(args)...);

                              const Key& key = __swp_list.front().first;
                              try {
                                    auto p = __find<false> (key);

                                    auto it = p.first;
                                    size_t bucket = p.second;
      
                                    if (it != end())
                                          return {it, false};
      
                                    bool constructed = false;
                                    if (__array[bucket].it == __empty) {
                                          __create_bucket (bucket);
                                          __array[bucket].it = __used.begin();
                                          constructed = true;
                                    }

                                    try {
                                          auto& list = __array[bucket].list;
                                          list.splice (list.begin(), __swp_list, __swp_list.begin());

                                          it.it = __array[bucket].it;
                                          it.bucket_it = list.begin ();
      
                                          ++__size;
                                          __load_factor = float (__size) / __array_size;
                                    
                                          return {it, true};
                                    } catch (...) {
                                         if (constructed) 
                                               __destroy_bucket ();
                                         __array[bucket].it = __empty;
                                         throw;
                                    }

                              } catch (...) {
                                    __swp_list.pop_front ();
                                    throw;
                              }
                        }

                  public:
                        
                        template <typename... Args>
                        dst::pair<iterator, bool> emplace (Args&&... args) {
                              auto p = __emplace<Args...> (dst::forward<Args> (args)...);

                              if (__load_factor > __max_load_factor) {
                                    try {
                                          __iter_list.push_front (p.first);

                                          try {
                                                rehash (static_cast<size_t>
                                                      (__size * __rehash_factor) + 1);

                                                p.first = __iter_list.front ();
                                                __iter_list.pop_front ();

                                          } catch (...) {
                                                __iter_list.pop_front ();
                                                throw;
                                          }

                                    } catch (...) {
                                          erase (p.first);
                                          throw;
                                    }
                              }

                              return p;
                        }

                  private:

                        template <bool is_moved>
                        dst::pair<iterator, size_t> __find (typename KeyRef<is_moved>::type key) {
                              size_t hash_value = hash (key);
                              size_t bucket = hash_value % __array_size;

                              if (__array[bucket].it == __empty)
                                    return {end(), bucket};

                              auto& list = __array[bucket].list;

                              for (auto it = list.begin(); it != list.end(); ++it)
                                    if (equal (it->first, dst::forward<typename KeyRef<is_moved>::
                                                          type> (key)))
                                          return {iterator (__array[bucket].it,
                                                            it, __used.end()), bucket};
                              return {end(), bucket};
                        }
                        
                        template <bool is_moved>
                        dst::pair<const_iterator, size_t> 
                        __find (typename KeyRef<is_moved>::type key) const {

                              size_t hash_value = hash (key);
                              size_t bucket = hash_value % __array_size;

                              if (__array[bucket].it == __empty)
                                    return {cend(), bucket};

                              auto& list = __array[bucket].list;

                              for (auto it = list.begin(); it != list.end(); ++it)
                                    if (equal (it->first, dst::forward<typename KeyRef<is_moved>::
                                                          type> (key)))
                                          return { const_iterator (__array[bucket].it,
                                                                   it, __used.end()), bucket};
                              return {cend(), bucket};
                        }

                  protected:
                        template <typename InputIt>
                        void __insert (InputIt first, InputIt last) {
                              try {
                                    for (; first != last; ++first) {
                                          auto p = __emplace (*first);
                                          
                                          if (p.second) {
                                                try {
                                                      __iter_list.push_front (p.first);
                                                } catch (...) {
                                                      erase (p.first);
                                                      throw;
                                                }
                                          }
                                    }

                                    if (__load_factor > __max_load_factor) {
                                          rehash (static_cast<size_t> 
                                                      (2*__size / __max_load_factor));
                                    }

                              } catch (...) {
                                    while (!__iter_list.empty ()) {
                                          erase (__iter_list.front ());
                                          __iter_list.pop_front ();
                                    }

                                    throw;
                              }
                        }
                  public:

                        void erase (iterator iter) {
                              auto& list = (*(iter.it))->list;
                              list.erase (iter.bucket_it);

                              if (list.size() == 0) {
                                    __used.erase((*(iter.it))->it);
                                    bucket_traits::destroy (b_alloc, *(iter.it));
                                    (*(iter.it))->it = __empty;
                              }

                              --__size;
                        }

                        void erase (iterator first, iterator last) {
                              while (first != last) {
                                    auto tmp = first;
                                    ++tmp;

                                    erase (first);
                                    first = tmp;
                              }
                        }

                        iterator begin () {
                              return iterator (__used.begin(), (*__used.begin())->list.begin(),
                                               __used.end());
                        }

                        const_iterator cbegin () const {
                              return const_iterator (__used.begin(), (*__used.begin())->list.begin(),
                                                     __used.end());
                        }

                        const_iterator begin () const {
                              return cbegin ();
                        }

                        iterator end () {
                              return iterator (__used.end (), __empty_bucket, __used.end());
                        }

                        const_iterator cend () const {
                              return const_iterator (__used.end (), __empty_bucket, __used.end());
                        }

                        const_iterator end () const { return cend (); }
            };
//      }

      template <typename Key, typename T,
                typename Hash = dst::hash<Key>,
                typename Equal = dst::equal_to<Key>,
                typename Allocator = dst::allocator< dst::pair<const Key, T> >
      > class unordered_map : public __hash_table<Key,T,Hash,Equal,Allocator> {

//            using hash_table = __hash_table<Key,T,Hash,Equal,Allocator>;
            
            using mutable_type = dst::pair<Key, T>;

            public:

                  typedef typename __hash_table<Key,T,Hash,Equal,Allocator>::iterator iterator;
                  typedef typename __hash_table<Key,T,Hash,Equal,Allocator>::
                                                                 const_iterator const_iterator;

                  typedef typename __hash_table<Key,T,Hash,Equal,Allocator>::value_type value_type;
                  typedef typename __hash_table<Key,T,Hash,Equal,Allocator>::exception exception;

                  unordered_map () : unordered_map ( __hash_table<Key,T,Hash,Equal,Allocator>::
                                                     __default_bucket_count ) {};

                  unordered_map ( size_t bucket_count,
                                  const Hash& hash = Hash (),
                                  const Equal& equal = Equal(),
                                  const Allocator& alloc = Allocator() )
                                : __hash_table<Key,T,Hash,Equal,Allocator>
                                               ( bucket_count,
                                                 hash, equal, alloc ) {}
                  
                  template <typename InputIt>
                  void insert (InputIt first, InputIt last) {
                        this->__insert (first, last);
                  }

                  /*
                  dst::pair<iterator, bool> insert (value_type v) {
                        return this->emplace (dst::move (v));
                  } */

                  dst::pair<iterator, bool> insert (const mutable_type& v) {
                        return this->emplace (v);
                  }

                  dst::pair<iterator, bool> insert (mutable_type&& v) {
                        return this->emplace (std::move (v));
                  }

                  T& operator[] (const Key& key) {
                        auto p = this->emplace (key, T());
                        return p.first->second;
                  }

                  T& operator[] (Key&& key) {
                        auto p = this->emplace (dst::move (key), T());
                        return p.first->second;
                  }

                  T& at (const Key& key) {
                        auto it = this->find (key);
                        if (it == this->end ())
                              throw exception ();

                        return it->second;
                  }

                  T& at (Key&& key) {
                        auto it = this->find (dst::move (key));
                        if (it == this->end ())
                              throw exception ();

                        return it->second;
                  }

                  const T& at (const Key& key) const {
                        auto it = this->find (key);
                        if (it == this->end ())
                              throw exception ();

                        return it->second;
                  }

                  const T& at (Key&& key) const {
                        auto it = this->find (dst::move (key));
                        if (it == this->end())
                              throw exception ();

                        return it->second;
                  }

                  void reserve (size_t count) {
                        this->rehash(std::ceil(count / this->__max_load_factor));
                  }
      };

} // namespace dst

template <typename Key, typename T,
          typename Hash = dst::hash<Key>,
          typename Equal = dst::equal_to<Key>,
          typename Allocator = dst::allocator< dst::pair<const Key, T> >
> using UnorderedMap = dst::unordered_map<Key,T,Hash,Equal,Allocator>;

#endif
