#include "pch.h"

#include "Core/Window.h"
#include "Core/ManagerTexture.h"

#include "Utility/Random.h"

#define WINDOW_WIDTH_START (640)
#define WINDOW_HEIGHT_START (480)

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

template<typename T>
class Point {
public:
	inline Point() = default;

	inline Point(const T& x, const T& y)
		: mX(x), mY(y) {}

	inline T Distance(const Point& point) const {
		return sqrt(pow(point.mX - mX, (T)2) + pow(point.mY - mY, (T)2));
	}

	inline const T& GetX() const {
		return mX;
	}

	inline const T& GetY() const {
		return mY;
	}

	inline void SetX(const T& x) {
		mX = x;
	}

	inline void SetY(const T& y) {
		mY = y;
	}

	inline bool operator==(const Point<T>& point) const {
		return mX == point.mX && mY == point.mY;
	}

	inline bool operator!=(const Point<T>& point) const {
		return !(*this == point);
	}

private:
	T mX {};
	T mY {};
};

template<typename T>
class Rectangle {
public:
	inline Rectangle() = default;

	inline Rectangle(const T& x, const T& y, const T& w, const T& h)
		: mX(x), mY(y), mW(w), mH(h) {}

	inline const T& GetX() const {
		return mX;
	}

	inline const T& GetY() const {
		return mY;
	}

	inline const T& GetW() const {
		return mW;
	}

	inline const T& GetH() const {
		return mH;
	}

	inline Point<T> GetCenter() const {
		return { mX + (mW / (T)2), mY + (mH / (T)2) };
	}

	inline void SetX(const T& x) {
		mX = x;
	}

	inline void SetY(const T& y) {
		mY = y;
	}

	inline void SetW(const T& w) {
		mW = w;
	}

	inline void SetH(const T& h) {
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
	inline NodeTreeBinary() = default;

	inline NodeTreeBinary(const T& leaf) : mLeaf(leaf) {}

	inline void GetLeafs(list<NodeTreeBinary<T>>& nodes) const {
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

	inline const T& GetLeaf() const {
		return mLeaf;
	}

	inline NodeTreeBinary<T>* const GetLeft() const {
		return mLeft;
	}

	inline NodeTreeBinary<T>* const GetRight() const {
		return mRight;
	}

	inline void SetLeaf(const T& leaf) {
		mLeaf = leaf;
	}

	inline void SetLeft(NodeTreeBinary<T>* const left) {
		mLeft = left;
	}

	inline void SetRight(NodeTreeBinary<T>* const right) {
		mRight = right;
	}

private:
	T mLeaf {};
	NodeTreeBinary<T>* mLeft = nullptr;
	NodeTreeBinary<T>* mRight = nullptr;
};

class Room {
public:
	inline Room() = default;

	inline Room(const Rectangle<float>& rectangle) {
		mRectangle.SetX(rectangle.GetX() + RandomFloat(0.f, floor(rectangle.GetW() / 3.f)));
		mRectangle.SetY(rectangle.GetY() + RandomFloat(0.f, floor(rectangle.GetH() / 3.f)));
		mRectangle.SetW(rectangle.GetW() - (mRectangle.GetX() - rectangle.GetX()));
		mRectangle.SetH(rectangle.GetH() - (mRectangle.GetY() - rectangle.GetY()));
		mRectangle.SetW(mRectangle.GetW() - RandomFloat(0.f, mRectangle.GetW() / 3.f));
		mRectangle.SetH(mRectangle.GetH() - RandomFloat(0.f, mRectangle.GetH() / 3.f));
	}

	inline void Render(SDL_Renderer* renderer, const float scale) const {
		SDL_FRect frect { mRectangle.GetX() * scale, mRectangle.GetY() * scale,
						  mRectangle.GetW() * scale, mRectangle.GetH() * scale };
		SDL_RenderFillRectF(renderer, &frect);
	}

protected:
	inline float RandomFloat(const float min, const float max) const {
		static Random random {};
		return floor(random.GetReal(0.f, 1.f) * (max - min + 1.f) + min);
	}

private:
	Rectangle<float> mRectangle {};
};

class Dungeon : public Room {
public:
	inline Dungeon(const size_t iterations = 4Ui64,
				   const Point<float>& size = { (float)WINDOW_WIDTH_START, (float)WINDOW_HEIGHT_START },
				   const float scale = 1.f,
				   const Point<float>& ratioToDiscard = { 0.45f, 0.45f })
		: mRectangle(0.f, 0.f, size.GetX(), size.GetY()),
		mScale(scale),
		mRatioToDiscard(ratioToDiscard),
		mTree(SplitRectangle(mRectangle, iterations)) {}

	void Render(SDL_Renderer* renderer) const {
		//RenderTree(renderer, mTree);

		RenderPaths(renderer, mTree);

		list<NodeTreeBinary<Rectangle<float>>> leafs;
		mTree->GetLeafs(leafs);
		for (const auto& leaf : leafs) {
			Room(leaf.GetLeaf()).Render(renderer, mScale);
		}
	}

	inline ~Dungeon() {
		DeleteTree(mTree);
		mTree = nullptr;
	}

private:
	inline void RenderPath(SDL_Renderer* renderer, const Rectangle<float>& rectangleOne, const Rectangle<float>& rectangleTwo) const {
		SDL_RenderSetScale(renderer, mScale, mScale);
		SDL_RenderDrawLineF(renderer, rectangleTwo.GetCenter().GetX() * mScale, rectangleTwo.GetCenter().GetY() * mScale,
							rectangleOne.GetCenter().GetX() * mScale, rectangleOne.GetCenter().GetY() * mScale);
		SDL_RenderSetScale(renderer, 1.f, 1.f);
	}

	inline void RenderPaths(SDL_Renderer* renderer, NodeTreeBinary<Rectangle<float>>* const tree) const {
		if (!tree->GetLeft() || !tree->GetRight()) {
			return;
		}

		RenderPath(renderer, tree->GetLeft()->GetLeaf(), tree->GetRight()->GetLeaf());

		RenderPaths(renderer, tree->GetLeft());
		RenderPaths(renderer, tree->GetRight());
	}

	inline void RenderRectangle(SDL_Renderer* renderer, const Rectangle<float>& rectangle) const {
		SDL_FRect frect { rectangle.GetX() * mScale, rectangle.GetY() * mScale,
						  rectangle.GetW() * mScale, rectangle.GetH() * mScale };
		SDL_RenderDrawRectF(renderer, &frect);
	}

	inline void RenderTree(SDL_Renderer* renderer, NodeTreeBinary<Rectangle<float>>* const tree) const {
		RenderRectangle(renderer, tree->GetLeaf());

		if (tree->GetLeft()) {
			RenderTree(renderer, tree->GetLeft());
		}

		if (tree->GetRight()) {
			RenderTree(renderer, tree->GetRight());
		}
	}

	inline pair<Rectangle<float>, Rectangle<float>> SplitRandom(const Rectangle<float>& rectangle) const {
		Rectangle<float> rectOne, rectTwo;

		if (RandomFloat(0.f, 1.f)) {
			rectOne = Rectangle<float>(rectangle.GetX(), rectangle.GetY(),
									   RandomFloat(1.f, rectangle.GetW()), rectangle.GetH());
			rectTwo = Rectangle<float>(rectangle.GetX() + rectOne.GetW(), rectangle.GetY(),
									   rectangle.GetW() - rectOne.GetW(), rectangle.GetH());

			if (mRatioToDiscard != Point(0.f, 0.f)) {
				const auto rectOneRatioW = rectOne.GetW() / rectOne.GetH();
				const auto rectTwoRatioW = rectTwo.GetW() / rectTwo.GetH();

				if (rectOneRatioW < mRatioToDiscard.GetX() ||
					rectTwoRatioW < mRatioToDiscard.GetX()) {
					return SplitRandom(rectangle);
				}
			}
		} else {
			rectOne = Rectangle<float>(rectangle.GetX(), rectangle.GetY(),
									   rectangle.GetW(), RandomFloat(1.f, rectangle.GetH()));
			rectTwo = Rectangle<float>(rectangle.GetX(), rectangle.GetY() + rectOne.GetH(),
									   rectangle.GetW(), rectangle.GetH() - rectOne.GetH());

			if (mRatioToDiscard != Point(0.f, 0.f)) {
				const auto rectOneRatioH = rectOne.GetH() / rectOne.GetW();
				const auto rectTwoRatioH = rectTwo.GetH() / rectTwo.GetW();

				if (rectOneRatioH < mRatioToDiscard.GetY() ||
					rectTwoRatioH < mRatioToDiscard.GetY()) {
					return SplitRandom(rectangle);
				}
			}
		}

		return { rectOne, rectTwo };
	}

	inline NodeTreeBinary<Rectangle<float>>* SplitRectangle(const Rectangle<float>& container, const size_t iterations) const {
		NodeTreeBinary<Rectangle<float>>* root = new NodeTreeBinary<Rectangle<float>>(container);
		if (iterations) {
			const auto pair = SplitRandom(container);

			root->SetLeft(SplitRectangle(pair.first, iterations - 1));
			root->SetRight(SplitRectangle(pair.second, iterations - 1));
		}

		return root;
	}

	void DeleteTree(NodeTreeBinary<Rectangle<float>>* tree) {
		if (!tree) {
			return;
		}

		DeleteTree(tree->GetLeft());
		DeleteTree(tree->GetRight());

		delete tree;
	}

	Rectangle<float> mRectangle {};
	float mScale = 0.f;
	Point<float> mRatioToDiscard {};
	NodeTreeBinary<Rectangle<float>>* mTree = nullptr;
};

int main(int /*argc*/, char** /*argv*/) {
	Window window(
		"Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
		WINDOW_FLAGS, RENDERER_FLAGS
	);

	bool isRunning = true;
	SDL_Event event {};

	while (isRunning) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					isRunning = false;
					break;
			}
		}

		SDL_RenderClear(window.GetRenderer());
		SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);

		Dungeon().Render(window.GetRenderer());

		SDL_RenderPresent(window.GetRenderer());
		SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

		//SDL_Delay(1234);
	}

	return EXIT_SUCCESS;
}

/*
	TO DO:
			- make the alg iterative
			- regroup things up a little more?
*/