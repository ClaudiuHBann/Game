#pragma once

template<typename T>
class Point {
public:
    Point() = default;

    Point(const T& x, const T& y)
        : mX(x), mY(y) {}

    T Distance(const Point& point) const {
        return sqrt(pow(point.mX - mX, (T)2) + pow(point.mY - mY, (T)2));
    }

    const T& GetX() const {
        return mX;
    }

    const T& GetY() const {
        return mY;
    }

    void SetX(const T& x) {
        mX = x;
    }

    void SetY(const T& y) {
        mY = y;
    }

    Point<T> operator+(const Point<T>& point) const {
        return { mX + point.mX,
                 mY + point.mY };
    }

    Point<T> operator+(const T scalar) const {
        return { mX + scalar,
                 mY + scalar };
    }

    void operator+=(const Point<T>& point) {
        mX += point.mX;
        mY += point.mY;
    }

    void operator+=(const T scalar) {
        mX += scalar;
        mY += scalar;
    }

    Point<T> operator-(const Point<T>& point) const {
        return { mX - point.mX,
                 mY - point.mY };
    }

    Point<T> operator-(const T scalar) const {
        return { mX - scalar,
                 mY - scalar };
    }

    void operator-=(const Point<T>& point) {
        mX -= point.mX;
        mY -= point.mY;
    }

    void operator-=(const T scalar) {
        mX -= scalar;
        mY -= scalar;
    }

    Point<T> operator*(const Point<T>& point) const {
        return { mX * point.mX,
                 mY * point.mY };
    }

    Point<T> operator*(const T scalar) const {
        return { mX * scalar,
                 mY * scalar };
    }

    Point<T> operator/(const Point<T>& point) const {
        return { mX / point.mX,
                 mY / point.mY };
    }

    Point<T> operator/(const T scalar) const {
        return { mX / scalar,
                 mY / scalar };
    }

    bool operator==(const Point<T>& point) const {
        return mX == point.mX && mY == point.mY;
    }

    bool operator!=(const Point<T>& point) const {
        return !(*this == point);
    }

private:
    T mX {};
    T mY {};
};

template<typename T>
class Rectangle {
public:
    Rectangle() = default;

    Rectangle(const T& x, const T& y, const T& w, const T& h)
        : mX(x), mY(y), mW(w), mH(h) {}

    const T& GetX() const {
        return mX;
    }

    const T& GetY() const {
        return mY;
    }

    const T& GetW() const {
        return mW;
    }

    const T& GetH() const {
        return mH;
    }

    Point<T> GetCenter() const {
        return { mX + (mW / (T)2), mY + (mH / (T)2) };
    }

    void SetX(const T& x) {
        mX = x;
    }

    void SetY(const T& y) {
        mY = y;
    }

    void SetW(const T& w) {
        mW = w;
    }

    void SetH(const T& h) {
        mH = h;
    }

private:
    T mX {};
    T mY {};
    T mW {};
    T mH {};
};

template<typename T>
class NodeTreeBinary {
public:
    NodeTreeBinary() = default;

    NodeTreeBinary(const T& leaf) : mLeaf(leaf) {}

    void GetLeafs(list<NodeTreeBinary<T>>& nodes) const {
        if (!mLeft && !mRight) {
            nodes.push_back(mLeaf);
        } else {
            if (mLeft) {
                mLeft->GetLeafs(nodes);
            }

            if (mRight) {
                mRight->GetLeafs(nodes);
            }
        }
    }

    const T& GetLeaf() const {
        return mLeaf;
    }

    NodeTreeBinary<T>* const GetLeft() const {
        return mLeft;
    }

    NodeTreeBinary<T>* const GetRight() const {
        return mRight;
    }

    void SetLeaf(const T& leaf) {
        mLeaf = leaf;
    }

    void SetLeft(NodeTreeBinary<T>* const left) {
        mLeft = left;
    }

    void SetRight(NodeTreeBinary<T>* const right) {
        mRight = right;
    }

private:
    T mLeaf {};
    NodeTreeBinary<T>* mLeft = nullptr;
    NodeTreeBinary<T>* mRight = nullptr;
};