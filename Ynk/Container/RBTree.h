//
// file ynk/container/rbtree.h
// author Maximilien M. Cura
//

#ifndef __YNK_CONTAINER_RBTREE
#define __YNK_CONTAINER_RBTREE

#include <Ynk/Num/Integers.h>
#include <Ynk/Box/Box.h>
#include <Ynk/Lang/Move.h>
#include <Ynk/Utility.h>
#include <Ynk/Panic/Panic.h>

// Red-Black tree, as in Robert Lafore's Data Structures and Algorithms in Java
// https://en.wikipedia.org/wiki/Red-black_tree

namespace Ynk::Container {
    template <class K = usize, class T>
    struct RBTree
    {
        enum class NodeColor : bool {
            Red,
            Black
        };

        struct Node
        {
            NodeColor color;
            K key;
            Box<T> inner;

            Node * parent;
            Node * left;
            Node * right;

            Node (NodeColor col, K key, T inner)
                : color { col }
                , key { key }
                , inner { Ynk::Move (Box<T>::take (inner)) }
                , parent { nullptr }
                , left { nullptr }
                , right { nullptr }
            {}

            Node * parent () { return parent; }
            Node * grandparent ()
            {
                if (parent == nullptr)
                    return nullptr;
                return parent->parent;
            }

            Node * sibling ()
            {
                if (parent == nullptr)
                    return nullptr;
                return (this == parent->left ? parent->right : parent->left);
            }

            Node * parent_sibling ()
            {
                if (parent == nullptr)
                    return nullptr;
                return (this->parent == this->parent->parent->left
                            ? this->parent->parent->right
                            : this->parent->parent->left);
            }

            void rotate_left ()
            {
                Node * replacement = this->right;
                Node * parent      = this->parent;
                if (replacement == nullptr) {
                    panic ("RBTree: Cannot rotate left with null right child");
                }

                this->right       = replacement->left;
                replacement->left = this;
                this->parent      = replacement;

                if (this->right != nullptr) {
                    this->right->parent = this;
                }

                if (parent != nullptr) {
                    if (this == parent->right)
                        parent->right = replacement;
                    if (this == parent->left)
                        parent->left = replacement;
                }
                replacement->parent = parent;
            }

            void rotate_right ()
            {
                Node * replacement = this->left;
                Node * parent      = this->parent;
                if (replacement == nullptr) {
                    panic ("RBTree: Cannot rotate right with null left child");
                }

                this->left         = replacement->right;
                replacement->right = this;
                this->parent       = replacement;

                if (this->left != nullptr) {
                    this->left->parent = this;
                }

                if (parent != nullptr) {
                    if (this == parent->right)
                        parent->right = replacement;
                    if (this == parent->left)
                        parent->left = replacement;
                }
                replacement->parent = parent;
            }
        };

        Node * root = nullptr;

        void insert (Node * node)
        {
            this->insert_impl (node, root);
            this->repair_against (node);
        }

        void repair_against (Node * node)
        {
            if (node->parent == nullptr) {
                node->color = NodeColor::Black;
            } else if (node->parent->color == NodeColor::Black) {
                // nothing...
            } else if (node->parent_sibling () != nullptr
                       && node->parent_sibling ()->color == NodeColor::Red) {
                node->parent->color            = NodeColor::Black;
                node->parent_sibling ()->color = NodeColor::Black;
                // Can't violate black-path-length rule
                node->grandparent ()->color = NodeColor::Red;
                // Unfortunately, we might have just turned the root node red, violating the
                // black-root or the red-black rule, so we do this again
                this->repair_against (node->grandparent ());
            } else {
                // left node of a right node (inside)
                if (node == node->parent->right && parent == node->grandparent ()->left) {
                    node->parent->rotate_left ();
                    node = node->left;
                } else if (node == node->parent->left && parent == node->grandparent ()->right) {
                    node->parent->rotate_right ();
                    node = node->right;
                }

                Node * parent      = node->parent ();
                Node * grandparent = node->grandparent ();

                if (node == node->parent->left) {
                    node->grandparent ()->rotate_right ();
                } else {
                    node->grandparent ()->rotate_left ();
                }

                parent->color      = NodeColor::Black;
                grandparent->color = NodeColor::Red;
            }
        }

        void insert_impl (Node * node, Node * context)
        {
            if (context != nullptr)
                if (node->key < context->key) {
                    if (context->left != nullptr) {
                        this->insert_impl (node, context->left);
                        return;
                    } else {
                        context->left = node;
                    }
                } else {
                    if (context->right != nullptr) {
                        this->insert_impl (node, context->right);
                        return;
                    } else {
                        context->right = node;
                    }
                }

            node->parent = context;
            node->left   = nullptr;
            node->right  = nullptr;
            node->color  = NodeColor::Red;
        }
    };
}

#endif /* !@__YNK_CONTAINER_RBTREE */
