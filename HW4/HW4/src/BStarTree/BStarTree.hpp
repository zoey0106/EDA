#pragma once
#include <vector>
#include <limits>
#include <memory>

/**
 * @brief A very simple segment tree for RMQ from TAs
 */
template <typename T>
class SegmentTree
{
    struct Node
    {
        T data, tag;
        bool hasTag;

        Node() : data(0), tag(0), hasTag(false) {}
    };

    size_t n;
    std::vector<Node> seg;

    T getVal(size_t id)
    {
        return (seg[id].hasTag) ? seg[id].tag : seg[id].data;
    }

    void pull(size_t id)
    {
        seg[id].data = std::max(getVal(id * 2), getVal(id * 2 + 1));
    }

    void push(size_t id)
    {
        if (seg[id].hasTag)
        {
            seg[id].data = getVal(id);
            seg[id].hasTag = false;
            seg[id * 2].tag = seg[id].tag;
            seg[id * 2].hasTag = true;
            seg[id * 2 + 1].tag = seg[id].tag;
            seg[id * 2 + 1].hasTag = true;
        }
    }

    T query(size_t ql, size_t qr, size_t l, size_t r, size_t id = 1)
    {
        if (ql > r || qr < l)
            return std::numeric_limits<T>::min();
        if (ql <= l && qr >= r)
            return getVal(id);

        push(id);
        int mid = (l + r) / 2;
        return std::max(query(ql, qr, l, mid, id * 2), query(ql, qr, mid + 1, r, id * 2 + 1));
    }

    void update(T val, size_t ql, size_t qr, size_t l, size_t r, size_t id = 1)
    {
        if (ql > r || qr < l)
            return;
        if (ql <= l && qr >= r)
        {
            seg[id].tag = val;
            seg[id].hasTag = true;
            return;
        }

        push(id);
        size_t mid = (l + r) / 2;
        update(val, ql, qr, l, mid, id * 2);
        update(val, ql, qr, mid + 1, r, id * 2 + 1);
        pull(id);
    }

public:
    SegmentTree() : n(0) {}

    void init(size_t n_)
    {
        n = n_;
        seg.assign(n_ * 4, {});
    }

    T query(size_t ql, size_t qr)
    {
        return query(ql, qr, 0LL, n - 1);
    }

    void update(size_t ql, size_t qr, T val)
    {
        update(val, ql, qr, 0LL, n - 1);
    }
};

/**
 * @brief The node structure of the B*-tree
 */
template <typename T>
struct Node
{
    using ptr = std::unique_ptr<Node>;

    T x, y;
    T width, height;
    Node *lchild, *rchild;

    Node() : x(0), y(0), width(0), height(0), lchild(nullptr), rchild(nullptr) {}

    void setPosition(T x_, T y_)
    {
        x = x_;
        y = y_;
    }

    void setShape(T width_, T height_)
    {
        width = width_;
        height = height_;
    }
};

/**
 * @brief A B*-tree to calculate the coordinates of nodes and the area of placement
 */
template <typename T>
class BStarTree
{
    SegmentTree<T> contourH;

    std::pair<T, T> getWidthHeight(Node<T> *root)
    {
        if (!root)
            return {0, 0};

        auto [lMaxWidth, lMaxHeight] = getWidthHeight(root->lchild);
        auto [rMaxWidth, rMaxHeight] = getWidthHeight(root->rchild);
        T maxWidth = std::max(std::max(lMaxWidth, rMaxWidth), root->x + root->width);
        T maxHeight = std::max(std::max(lMaxHeight, rMaxHeight), root->y + root->height);
        return {maxWidth, maxHeight};
    }

public:
    BStarTree() {}

    Node<T> *buildTree(const std::vector<Node<T> *> &admissible)
    {
        if (admissible.empty())
            return nullptr;

        T sum = 0;
        size_t n = admissible.size();
        for (size_t i = 0; i < n; ++i)
        {
            size_t l = i * 2 + 1;
            size_t r = i * 2 + 2;
            admissible[i]->lchild = (l < n) ? admissible[l] : nullptr;
            admissible[i]->rchild = (r < n) ? admissible[r] : nullptr;
            sum += admissible[i]->width;
        }

        contourH.init(sum);
        return admissible[0];
    }

    void setPosition(Node<T> *root, T startX = 0)
    {
        if (!root)
            return;

        T endX = startX + root->width;
        T y = contourH.query(startX, endX - 1);
        contourH.update(startX, endX - 1, y + root->height);
        root->x = startX;
        root->y = y;
        setPosition(root->lchild, endX);
        setPosition(root->rchild, startX);
    }

    T getArea(Node<T> *root)
    {
        auto [width, height] = getWidthHeight(root);
        return width * height;
    }
};
