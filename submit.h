#include <cstddef>
#include <algorithm>
#include <initializer_list>


template<class T>
class Set {
    struct Node {
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        T value;
        int height = 0;
        explicit Node(const T _value) {
            value = _value;
        }
    };
public:
    Set() {
        _root = nullptr;
        _size = 0;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) : Set() {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    Set(std::initializer_list<T> elems): Set() {
        for (const auto& elem : elems) {
            insert(elem);
        }
    }

    Set(const Set& other_set) : Set() {
        _root = copyNode(other_set._root, _root);
        _size = other_set._size;
    }

    Set& operator=(const Set& other_set) {
        if (&other_set == this) {
            return *this;
        }
        freeTheMemory(_root);
        _root = copyNode(other_set._root, _root);
        _size = other_set._size;
        return *this;
    }

    ~Set() {
        freeTheMemory(_root);
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        if (_size)
            return false;
        return true;
    }

    void insert(const T& elem)
    {
        if (find(_root, elem) != nullptr) {
            return;
        }
        _root = insert(_root, elem);
        ++_size;
    }

    void erase(const T& elem) {
        if (find(_root, elem) == nullptr) {
            return;
        }
        _root = erase(_root, elem);
        --_size;
    }


    class AVL_iterator {
    public:
        AVL_iterator() = default;

        explicit AVL_iterator(Node* x, bool isEnd = false) {
            _x = x;
            _isEnd = isEnd;
        }

        const T *operator->() const {
            return &_x->value;
        }

        const T &operator*() const {
            return _x->value;
        }

        Node* next(Node* cur_v) {
            if (cur_v->right != nullptr) {
                Node* temp = cur_v->right;
                while (temp->left != nullptr) {
                    temp = temp->left;
                }
                return temp;
            }
            while (true) {
                if (cur_v->parent == nullptr) {
                    return nullptr;
                }
                if (cur_v->parent->left == cur_v) {
                    return cur_v->parent;
                }
                cur_v = cur_v->parent;
            }
        }

        Node* previous(Node* cur_v) {
            if (cur_v->left != nullptr) {
                Node* temp = cur_v->left;
                while (temp->right != nullptr) {
                    temp = temp->right;
                }
                return temp;
            }
            while (true) {
                if (cur_v->parent == nullptr) {
                    return nullptr;
                }
                if (cur_v->parent->right == cur_v) {
                    return cur_v->parent;
                }
                cur_v = cur_v->parent;
            }
        }

        AVL_iterator& operator++() {
            Node* temp = next(this->_x);
            if (temp == nullptr) {
                this->_isEnd = true;
                return *this;
            }
            *this = AVL_iterator(temp);
            return *this;
        }

        AVL_iterator operator++(int) {
            AVL_iterator temp = *this;
            ++*this;
            return temp;
        }

        AVL_iterator& operator--(){
            Node* temp;
            if (this->_isEnd != true) {
                temp = previous(this->_x);
            } else {
                this->_isEnd = false;
                return *this;
            }
            *this = AVL_iterator(temp);
            return *this;
        }

        AVL_iterator operator--(int) {
            AVL_iterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(AVL_iterator other_it) {
            if (_x == other_it._x && _isEnd == other_it._isEnd) {
                return true;
            }
            return false;
        }

        bool operator!=(AVL_iterator other_it) {
            if (_x == other_it._x && _isEnd == other_it._isEnd) {
                return false;
            }
            return true;
        }



    private:
        Node* _x = nullptr;
        bool _isEnd = false;
    };

    typedef typename Set::AVL_iterator iterator;

    iterator begin() const {
        Node* temp = go_to_begin(_root);
        if (temp == nullptr) {
            return AVL_iterator(temp, true);
        }
        return AVL_iterator(temp);
    }

    iterator end() const {
        return AVL_iterator(go_to_end(_root), true);
    }

    iterator find(const T& elem) const {
        Node* v = find(_root, elem);
        if (v == nullptr) {
            return end();
        }
        return AVL_iterator(v);
    }

    iterator lower_bound(const T& elem) const {
        Node* lb = lower_bound(_root, elem);
        if (lb == nullptr) {
            return end();
        }
        return AVL_iterator(lb);
    }
private:
    Node* _root;
    size_t _size;

    void freeTheMemory(Node* cur_v) {
        if (cur_v == nullptr) {
            return;
        }
        freeTheMemory(cur_v->left);
        freeTheMemory(cur_v->right);
        delete cur_v;
    }

    Node* copyNode(Node* from, Node* to) {
        if (from == nullptr) {
            return nullptr;
        }
        to = new Node(from->value);
        to->height = from->height;
        to->left = copyNode(from->left, to->left);
        to->right = copyNode(from->right, to->right);
        if (to->left != nullptr) {
            to->left->parent = to;
        }
        if (to->right != nullptr) {
            to->right->parent = to;
        }
        return to;
    }

    Node* find(Node* cur_v, const T& elem) const {
        if (cur_v == nullptr) {
            return nullptr;
        }
        if (!(cur_v->value < elem) && !(elem < cur_v->value)) {
            return cur_v;
        }
        if (elem < cur_v->value) {
            return find(cur_v->left, elem);
        } else {
            return find(cur_v->right, elem);
        }
    }

    int getHeight(Node* v) {
        if (v == nullptr) {
            return -1;
        } else {
            return v->height;
        }
    }

    void updateHeight(Node* v) {
        v->height = std::max(getHeight(v->left), getHeight(v->right)) + 1;
    }

    Node* l_l_rotation(Node* new_root) {
        Node* old_root = new_root->parent;
        new_root->parent = old_root->parent;
        if (new_root->parent != nullptr) {
            if (new_root->parent->left == old_root) {
                new_root->parent->left = new_root;
            } else {
                new_root->parent->right = new_root;
            }
        }

        old_root->left = new_root->right;
        if (old_root->left != nullptr) {
            old_root->left->parent = old_root;
        }
        old_root->parent = new_root;
        new_root->right = old_root;
        updateHeight(old_root);
        updateHeight(new_root);
        return new_root;
    }

    Node* l_r_rotation(Node* new_root) {
        Node* old_left = new_root->parent;
        Node* old_root = old_left->parent;
        new_root->parent = old_root->parent;
        if (new_root->parent != nullptr) {
            if (new_root->parent->left == old_root) {
                new_root->parent->left = new_root;
            } else {
                new_root->parent->right = new_root;
            }
        }

        old_left->right = new_root->left;
        if (old_left->right != nullptr) {
            old_left->right->parent = old_left;
        }
        old_root->left = new_root->right;
        if (old_root->left != nullptr) {
            old_root->left->parent = old_root;
        }
        new_root->left = old_left;
        new_root->right = old_root;
        old_left->parent = new_root;
        old_root->parent = new_root;
        updateHeight(old_root);
        updateHeight(old_left);
        updateHeight(new_root);
        return new_root;
    }

    Node* r_r_rotation(Node* new_root) {
        Node* old_root = new_root->parent;
        new_root->parent = old_root->parent;
        if (new_root->parent != nullptr) {
            if (new_root->parent->left == old_root) {
                new_root->parent->left = new_root;
            } else {
                new_root->parent->right = new_root;
            }
        }

        old_root->right = new_root->left;
        if (old_root->right != nullptr) {
            old_root->right->parent = old_root;
        }
        old_root->parent = new_root;
        new_root->left = old_root;
        updateHeight(old_root);
        updateHeight(new_root);
        return new_root;
    }

    Node* r_l_rotation(Node* new_root) {
        Node* old_right = new_root->parent;
        Node* old_root = old_right->parent;
        new_root->parent = old_root->parent;
        if (new_root->parent != nullptr) {
            if (new_root->parent->left == old_root) {
                new_root->parent->left = new_root;
            } else {
                new_root->parent->right = new_root;
            }
        }

        old_right->left = new_root->right;
        if (old_right->left != nullptr) {
            old_right->left->parent = old_right;
        }
        old_root->right = new_root->left;
        if (old_root->right != nullptr) {
            old_root->right->parent = old_root;
        }
        new_root->left = old_root;
        new_root->right = old_right;
        old_right->parent = new_root;
        old_root->parent = new_root;
        updateHeight(old_root);
        updateHeight(old_right);
        updateHeight(new_root);
        return new_root;
    }

    Node* balanceNode(Node* cur_v) {
        Node* left_of_v = cur_v->left;
        Node* right_of_v = cur_v->right;
        Node* old_cur_v = cur_v;
        if (getHeight(right_of_v) + 1 < getHeight(left_of_v)) {
            if (getHeight(left_of_v->left) < getHeight(left_of_v->right)) {
                cur_v = l_r_rotation(left_of_v->right);
                cur_v->parent = old_cur_v->parent->parent;
            } else {
                cur_v = l_l_rotation(left_of_v);
                cur_v->parent = old_cur_v->parent->parent;
            }
        } else if (getHeight(left_of_v) + 1 < getHeight(right_of_v)) {
            if (getHeight(right_of_v->right) < getHeight(right_of_v->left)) {
                cur_v = r_l_rotation(right_of_v->left);
                cur_v->parent = old_cur_v->parent->parent;
            } else {
                cur_v = r_r_rotation(right_of_v);
                cur_v->parent = old_cur_v->parent->parent;
            }
        }
        updateHeight(cur_v);
        return cur_v;
    }

    Node* insert(Node* cur_v, const T& elem) {
        if (cur_v == nullptr) {
            cur_v = new Node(elem);
        } else if (elem < cur_v->value) {
            cur_v->left = insert(cur_v->left, elem);
            cur_v->left->parent = cur_v;
            cur_v = balanceNode(cur_v);
        } else {
            cur_v->right = insert(cur_v->right, elem);
            cur_v->right->parent = cur_v;
            cur_v = balanceNode(cur_v);
        }
        return cur_v;
    }

    Node* remove_next(Node* cur_v) {
        if (cur_v->left == nullptr) {
            return cur_v->right;
        }
        cur_v->left = remove_next(cur_v->left);
        if (cur_v->left != nullptr) {
            cur_v->left->parent = cur_v;
        }
        cur_v = balanceNode(cur_v);
        return cur_v;
    }

    Node* erase(Node* cur_v, const T& elem) {
        if (!(cur_v->value < elem) && !(elem < cur_v->value)) {
            if (cur_v->right == nullptr) {
                Node* to_return = cur_v->left;
                delete cur_v;
                return to_return;
            } else if (cur_v->left == nullptr) {
                Node* to_return = cur_v->right;
                delete cur_v;
                return to_return;
            } else {
                Node* to_return = lower_bound(cur_v->right, elem);
                to_return->right = remove_next(cur_v->right);
                if (to_return->right != nullptr) {
                    to_return->right->parent = to_return;
                }
                to_return->left = cur_v->left;
                if (to_return->left != nullptr) {
                    to_return->left->parent = to_return;
                }
                to_return->parent = cur_v->parent;
                if (to_return->parent != nullptr && to_return->parent->left == cur_v) {
                    to_return->parent->left = to_return;
                }
                if (to_return->parent != nullptr && to_return->parent->right == cur_v) {
                    to_return->parent->right = to_return;
                }
                delete cur_v;
                to_return = balanceNode(to_return);
                return to_return;
            }
        } else if (elem < cur_v->value) {
            cur_v->left = erase(cur_v->left, elem);
            if (cur_v->left != nullptr) {
                cur_v->left->parent = cur_v;
            }
            cur_v = balanceNode(cur_v);
        } else {
            cur_v->right = erase(cur_v->right, elem);
            if (cur_v->right != nullptr) {
                cur_v->right->parent = cur_v;
            }
            cur_v = balanceNode(cur_v);
        }
        return cur_v;
    }

    Node* lower_bound(Node* cur_v, const T& elem) const {
        if (cur_v == nullptr) {
            return nullptr;
        }
        if (cur_v->value < elem) {
            return lower_bound(cur_v->right, elem);
        } else {
            Node* temp = lower_bound(cur_v->left, elem);
            if (temp == nullptr) {
                return cur_v;
            }
            return temp;
        }
    }

    Node* go_to_begin(Node* cur_v) const {
        if (cur_v == nullptr) {
            return cur_v;
        }
        if (cur_v->left == nullptr) {
            return cur_v;
        }
        return go_to_begin(cur_v->left);
    }

    Node* go_to_end(Node* cur_v) const {
        if (cur_v == nullptr) {
            return cur_v;
        }
        if (cur_v->right == nullptr) {
            return cur_v;
        }
        return go_to_end(cur_v->right);
    }
};



