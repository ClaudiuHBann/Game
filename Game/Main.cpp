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

inline float RandomFloat(const float min, const float max) {
	static Random random {};
	return floor(random.GetReal(0.f, 1.f) * (max - min + 1.f) + min);
}

class Room {
public:
	inline Room(const Rectangle<float>& rectangle, const Point<float> scale = { 1.f, 1.f }) {
		mRectangle.SetX(rectangle.GetX() + RandomFloat(0.f, floor(rectangle.GetW() / 3.f)));
		mRectangle.SetY(rectangle.GetY() + RandomFloat(0.f, floor(rectangle.GetH() / 3.f)));
		mRectangle.SetW(rectangle.GetW() - (mRectangle.GetX() - rectangle.GetX()));
		mRectangle.SetH(rectangle.GetH() - (mRectangle.GetY() - rectangle.GetY()));
		mRectangle.SetW(mRectangle.GetW() - RandomFloat(0.f, mRectangle.GetW() / 3.f));
		mRectangle.SetH(mRectangle.GetH() - RandomFloat(0.f, mRectangle.GetH() / 3.f));

		mRectangle.SetW(mRectangle.GetW() * scale.GetX());
		mRectangle.SetH(mRectangle.GetH() * scale.GetY());
	}

	inline const Rectangle<float>& GetRectangle() const {
		return mRectangle;
	}

private:
	Rectangle<float> mRectangle {};
};

class Path {
public:
	inline Path(const Rectangle<float>& rectOne, const Rectangle<float>& rectTwo, const float width = 1.f) {
		const auto centerRO = rectOne.GetCenter();
		const auto centerRT = rectTwo.GetCenter();
		const auto centerDistance = centerRO.Distance(centerRT);
		const auto widthHalf = width / 2.f;

		if (centerRO.GetY() == centerRT.GetY()) {
			const auto centerLower = min(centerRO.GetX(), centerRT.GetX());

			mRectangle = Rectangle<float>(centerLower, centerRO.GetY() - widthHalf, centerDistance, width);
		} else {
			const auto centerLower = min(centerRO.GetY(), centerRT.GetY());

			mRectangle = Rectangle<float>(centerRO.GetX() - widthHalf, centerLower, width, centerDistance);
		}
	}

	inline const Rectangle<float>& GetRectangle() const {
		return mRectangle;
	}

private:
	Rectangle<float> mRectangle {};
};

class Dungeon {
public:
	inline Dungeon(
		const size_t iterations,
		const Point<float>& size,
		const Point<float>& scaleRoom = { 1.f, 1.f },
		const float scalePath = 1.f,
		const Point<float>& ratioToDiscard = { 0.45f, 0.45f }
	)
		: mRectangle(0.f, 0.f, size.GetX(), size.GetY()),
		mRatioToDiscard(ratioToDiscard),
		mTree(SplitRectangle(mRectangle, iterations, scaleRoom)) {

		list<NodeTreeBinary<Rectangle<float>>> leafs;
		mTree->GetLeafs(leafs);
		for (const auto& leaf : leafs) {
			mRooms.push_back(leaf.GetLeaf());
		}

		GeneratePaths(mTree, mPaths, scalePath);
	}

	inline void Render(SDL_Renderer* renderer) const {
		RenderTree(renderer, mTree);
		RenderPaths(renderer);
		RenderRooms(renderer);
	}

	inline ~Dungeon() {
		DeleteTree(mTree);
		mTree = nullptr;
	}

private:
	inline void RenderRoom(SDL_Renderer* renderer, const Room& room) const {
		const SDL_FRect frect { room.GetRectangle().GetX(), room.GetRectangle().GetY(),
								room.GetRectangle().GetW(), room.GetRectangle().GetH() };
		SDL_RenderFillRectF(renderer, &frect);
	}

	void RenderRooms(SDL_Renderer* renderer) const {
		for (const auto& room : mRooms) {
			RenderRoom(renderer, room);
		}
	}

	inline void RenderPath(SDL_Renderer* renderer, const Path& path) const {
		const SDL_FRect frect { path.GetRectangle().GetX(), path.GetRectangle().GetY(),
								path.GetRectangle().GetW(), path.GetRectangle().GetH() };
		SDL_RenderFillRectF(renderer, &frect);
	}

	void RenderPaths(SDL_Renderer* renderer) const {
		for (const auto& path : mPaths) {
			RenderPath(renderer, path);
		}
	}

	inline void RenderRectangle(SDL_Renderer* renderer, const Rectangle<float>& rectangle) const {
		const SDL_FRect frect { rectangle.GetX(), rectangle.GetY(),
								rectangle.GetW(), rectangle.GetH() };
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

	inline pair<Rectangle<float>, Rectangle<float>> SplitRandom(const Rectangle<float>& rectangle, const Point<float>& scale) const {
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
					return SplitRandom(rectangle, scale);
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
					return SplitRandom(rectangle, scale);
				}
			}
		}

		rectOne.SetW(rectOne.GetW() * scale.GetX());
		rectOne.SetH(rectOne.GetH() * scale.GetY());

		rectTwo.SetW(rectTwo.GetW() * scale.GetX());
		rectTwo.SetH(rectTwo.GetH() * scale.GetY());

		return { rectOne, rectTwo };
	}

	inline NodeTreeBinary<Rectangle<float>>* SplitRectangle(const Rectangle<float>& container, const size_t iterations, const Point<float>& scale) const {
		NodeTreeBinary<Rectangle<float>>* root = new NodeTreeBinary<Rectangle<float>>(container);
		if (iterations) {
			const auto pair = SplitRandom(container, scale);

			root->SetLeft(SplitRectangle(pair.first, iterations - 1, scale));
			root->SetRight(SplitRectangle(pair.second, iterations - 1, scale));
		}

		return root;
	}

	inline void GeneratePaths(NodeTreeBinary<Rectangle<float>>* const tree, list<Path>& paths, const float width) {
		if (!tree->GetLeft() || !tree->GetRight()) {
			return;
		}

		paths.push_back(Path(tree->GetLeft()->GetLeaf(), tree->GetRight()->GetLeaf(), width));

		GeneratePaths(tree->GetLeft(), paths, width);
		GeneratePaths(tree->GetRight(), paths, width);
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
	Point<float> mRatioToDiscard {};

	NodeTreeBinary<Rectangle<float>>* mTree = nullptr;
	list<Room> mRooms {};
	list<Path> mPaths {};
};

int main(int /*argc*/, char** /*argv*/) {
	Window window(
		"BSP Dungeon Generation",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
		WINDOW_FLAGS, RENDERER_FLAGS
	);

	Dungeon dungeon(4, { WINDOW_WIDTH_START, WINDOW_HEIGHT_START }, { 1.25f, 1.25f }, 5.f);

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

		SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(window.GetRenderer());
		SDL_SetRenderDrawColor(window.GetRenderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);

		dungeon.Render(window.GetRenderer());

		SDL_RenderPresent(window.GetRenderer());
	}

	return EXIT_SUCCESS;
}

/*
	TO DO:
			- make the alg iterative
			- check every shit
*/