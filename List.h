#include <stdint.h>
#include <cstddef>
#include <new>
#include <type_traits>
#include <memory>
#include <iterator>
#include <algorithm>

#ifndef INC_1_6_LISTSTACKALLOCATOR_ALLOCATOR_H
#define INC_1_6_LISTSTACKALLOCATOR_ALLOCATOR_H

template <size_t N>
struct alignas(std::max_align_t) StackStorage {
    uint8_t data[N];
    size_t filled;
    StackStorage(): data(), filled(0) {}
};

template<typename T, size_t N>
class StackAllocator {
    StackStorage<N>* stack;

    template <typename T1, typename T2, size_t N1, size_t N2>
    friend bool operator==(const StackAllocator<T1, N1>& lhs, const StackAllocator<T2, N2>& rhs) noexcept;

public:

    using value_type = T;
    //     select_on_container_copy_construction - impossible to provide an allocator with a new stackstorage, so
    // the same allocator will be returned
    //     propagate_on_container_copy_assignment - how to choose?? will be false for now
    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    StackStorage<N>* get_stack() const noexcept { // or should I make stack public?
        return stack;
    }

    explicit StackAllocator(StackStorage<N>& stack) noexcept : stack(&stack) {}

    template <typename U>
    StackAllocator(const StackAllocator<U, N>& other) noexcept : stack(other.get_stack()) {}

    StackAllocator& operator=(const StackAllocator& other) noexcept {
        stack = other.stack;
        return *this;
    }

    T* allocate(size_t n) {
        size_t align = alignof(T);
        size_t begin = (stack->filled + align - 1) / align * align;
        if (begin + n * sizeof(T) > N)
            throw std::bad_alloc();
        stack->filled = begin + n * sizeof(T);
        return reinterpret_cast<T*>(stack->data + begin);
    }
    void deallocate(T* , size_t) noexcept {}



};

template <typename T1, typename T2, size_t N1, size_t N2>
bool operator==(const StackAllocator<T1, N1>& lhs, const StackAllocator<T2, N2>& rhs) noexcept {
    return std::is_same_v<T1, T2> && (N1 == N2) && lhs.stack == rhs.stack;
}
template <typename T1, typename T2, size_t N1, size_t N2>
bool operator!=(const StackAllocator<T1, N1>& lhs, const StackAllocator<T2, N2>& rhs) noexcept {
    return !(lhs == rhs);
}

#endif //INC_1_6_LISTSTACKALLOCATOR_ALLOCATOR_H

#ifndef INC_1_6_LISTSTACKALLOCATOR_LIST_H
#define INC_1_6_LISTSTACKALLOCATOR_LIST_H

template <typename T, typename AllocT=std::allocator<T>>
class List {
    struct BaseNode {
        BaseNode* prev;
        BaseNode* next;
    };

    struct Node: BaseNode {
        using value_type = Node;
        T value;
        Node(const T& value): BaseNode(), value(value) {}
        Node(): value() {}
    };

    using Alloc = typename std::allocator_traits<AllocT>::template rebind_alloc<Node>;
    using a_tr = std::allocator_traits<Alloc>;
    using a_trT = std::allocator_traits<AllocT>; // specially for weird Ilya's tests
    size_t size_;
    BaseNode end_;
    Alloc alloc;

    static void link(BaseNode* lhs, BaseNode* rhs) noexcept {
        lhs->next = rhs;
        rhs->prev = lhs;
    }

    template <typename... Args>
    void emplace(BaseNode* pos, const Args&... args) {
        Node* add = a_tr::allocate(alloc, 1);
        try {
            a_tr::construct(alloc, add, args...);
            link(pos->prev, add);
            link(add, pos);
            size_++;
        } catch (...) {
            a_tr::deallocate(alloc, add, 1);
            throw;
        }
    }

    void remove(BaseNode* pos) noexcept {
        link(pos->prev, pos->next);
        a_tr::destroy(alloc, static_cast<Node*>(pos));
        a_tr::deallocate(alloc, static_cast<Node*>(pos), 1);
        size_--;
    }

    void clear() noexcept {
        BaseNode* curr = end_.next;
        while (curr != &end_) {
            curr = curr->next;
            remove(curr->prev);
        }
        end_.prev = end_.next = &end_;
    }

    void fill(const List& other) {
        end_.prev = end_.next = &end_;
        BaseNode* other_node = other.end_.next;
        for (size_t i = 0; i < other.size_; i++) {
            try {
                emplace<T>(&end_, static_cast<Node*>(other_node)->value);
                other_node = other_node->next;
            } catch (...) {
                clear();
                throw;
            }
        }
    }

    static void correct_link(BaseNode* curr) noexcept {
        curr->prev->next = curr;
        curr->next->prev = curr;
    }

    static void swap_fakes(BaseNode& lhs, BaseNode& rhs) noexcept {
        std::swap(lhs, rhs);
        if (lhs.next == &rhs)
            lhs.prev = lhs.next = &lhs;
        if (rhs.next == &lhs)
            rhs.prev = rhs.next = &rhs;
        correct_link(&lhs);
        correct_link(&rhs);
    }

    void swap(List& copy, const List& other) noexcept {
        if (a_tr::propagate_on_container_copy_assignment::value)
            alloc = other.alloc;
        std::swap(size_, copy.size_);
        swap_fakes(end_, copy.end_);
    }

public:
    // () or (AllocT)
    List(const AllocT& alloc_t=AllocT()): size_(0), end_(BaseNode()), alloc(alloc_t) {
        end_.prev = end_.next = &end_;
    }

    // (n, value) or (n, value, AllocT)
    List(size_t n, const T& value, const AllocT& alloc_t=AllocT()): size_(0), end_(BaseNode()), alloc(alloc_t) {
        end_.prev = end_.next = &end_;
        for (size_t i = 0; i < n; i++)
            try {
                emplace(&end_, value);
            } catch (...) {
                clear();
                throw;
            }
    }

    // (n) or (n, AllocT)
    List(size_t n, const AllocT& alloc_t=AllocT()): size_(0), end_(BaseNode()), alloc(alloc_t) {
        end_.prev = end_.next = &end_;
        for (size_t i = 0; i < n; i++)
            try {
                emplace(&end_);
            } catch (...) {
                clear();
                throw;
            }
    }

    const Alloc& get_allocator() const noexcept {
        return alloc;
    }

    List(const List& other): size_(0), end_(BaseNode()),
                             alloc(a_trT::select_on_container_copy_construction(other.alloc)) {
        fill(other);
    }

    ~List() noexcept {
        clear();
    }

//    List& operator=(const List& other) {
//        if (this == &other)
//            return *this;
//
//        clear();
//        size_ = 0;
//        if (a_tr::propagate_on_container_copy_assignment::value)
//            alloc = other.alloc;
//        fill(other);
//        return *this;
//    }

    List& operator=(const List& other) {
        List copy(other);
        swap(copy, other);
        return *this;
    }

    size_t size() const noexcept {
        return size_;
    }

    void push_back(const T& value) {
        emplace<T>(&end_, value);
    };

    void push_front(const T& value) {
        emplace<T>(end_.next, value);
    };

    void pop_back() {
        remove(end_.prev);
    };

    void pop_front() {
        remove(end_.next);
    };

    template <bool is_const>
    class Iter {
        BaseNode* node;

        using iterator = Iter<false>;

        friend List;
    public:
        using difference_type = long long;
        using value_type = typename std::conditional<is_const, const T, T>::type;
        using pointer = typename std::conditional<is_const, const  T*, T*>::type;
        using reference	= typename std::conditional<is_const, const T&, T&>::type;
        using iterator_category= std::bidirectional_iterator_tag;

        explicit Iter(const BaseNode* node): node(const_cast<BaseNode*>(node)) {} //TODO

        Iter(const Iter& other): node(other.node) {}

        void swap(const Iter& other) {
            node = other.node;
        }

        Iter& operator=(const Iter& other) {
            Iter copy(other);
            swap(other);
            return *this;
        }

        operator Iter<true>() const {
            return Iter<true>(node);
        }

        reference operator*() {
            return (static_cast<Node*>(node))->value;
        }

        friend bool operator==(const Iter& lhs, const Iter& rhs) {
            return lhs.node == rhs.node;
        }

        friend bool operator!=(const Iter& lhs, const Iter& rhs) {
            return lhs.node != rhs.node;
        }

        pointer operator->() {
            value_type& value = (static_cast<Node*>(node))->value;
            return &value;
        }

        Iter& operator++() { // ++it
            node = node->next;
            return *this;
        }

        Iter operator++(int) { // it++
            Iter copy(*this);
            node = node->next;
            return copy;
        }

        Iter& operator--() {
            node = node->prev;
            return *this;
        }

        Iter operator--(int) {
            Iter copy(*this);
            node = node->prev;
            return copy;
        }
    };

    using iterator = Iter<false>;
    using const_iterator = Iter<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(end_.next);
    }
    const_iterator begin() const {
        return const_iterator(end_.next);
    }
    iterator end() {
        return iterator(&end_);
    }
    const_iterator end() const {
        return const_iterator(&end_);
    }

    const_iterator cbegin() const {
        return const_iterator(begin());
    }
    const_iterator cend() const {
        return const_iterator(end());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    template <bool is_constant>
    void insert(Iter<is_constant> it, const T& value) {
        emplace(it.node, value);
    }
    template <bool is_constant>
    void erase(Iter<is_constant> it) {
        remove(it.node);
    }
};


#endif //INC_1_6_LISTSTACKALLOCATOR_LIST_H