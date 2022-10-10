#include "pch.h"

#include "Core/Window.h"
#include "Core/ManagerTexture.h"

#include "Utility/Random.h"

#define WINDOW_WIDTH_START (640)
#define WINDOW_HEIGHT_START (480)

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

#define MAP_SIZE (1.)
#define SQUARE (1.)
#define N_ITERATIONS (4.)
#define DISCARD_BY_RATIO (1)
#define H_RATIO (0.45)
#define W_RATIO (0.45)

template<typename T>
inline T random(const T& min, const T& max) {
	return (T)floor(Random().GetReal((T)0, (T)1) * (max - min + (T)1) + min);
}

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

private:
	T mX {};
	T mY {};
};

template<typename T>
class Rectangle {
public:
	inline Rectangle() = default;

	inline Rectangle(const T& x, const T& y, const T& w, const T& h)
		: mX(x), mY(y), mW(w), mH(h), mCenter({ x + (w / (T)2), y + (h / (T)2) }) {}

	inline void RenderRectangle(SDL_Renderer* renderer) const {
		const auto squareAsT = (T)SQUARE;

		SDL_FRect frect { float(mX * squareAsT), float(mY * squareAsT),
						  float(mW * squareAsT), float(mH * squareAsT) };
		SDL_RenderDrawRectF(renderer, &frect);
	}

	inline void RenderPath(SDL_Renderer* renderer, const Rectangle& rectangle, const T& scale = (T)SQUARE) const {
		const auto scaledSquareAsT = (T)SQUARE / scale;

		SDL_RenderSetScale(renderer, (float)scale, (float)scale);
		SDL_RenderDrawLineF(renderer, float(mCenter.GetX() * scaledSquareAsT), float(mCenter.GetY() * scaledSquareAsT),
							float(rectangle.mCenter.GetX() * scaledSquareAsT), float(rectangle.mCenter.GetY() * scaledSquareAsT));
		SDL_RenderSetScale(renderer, 1.f, 1.f);
	}

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

	inline const Point<T>& GetCenter() const {
		return mCenter;
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

	inline void SetCenter(const Point<T>& center) {
		mCenter = center;
	}

private:
	T mX {};
	T mY {};
	T mW {};
	T mH {};

	Point<T> mCenter {};
};

template<typename T>
class NodeTreeBinary {
public:
	inline NodeTreeBinary() = default;

	inline NodeTreeBinary(const Rectangle<T>& leaf) : mLeaf(leaf) {}

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

	inline void GetLevel(const size_t level, list<NodeTreeBinary<T>>& nodes) const {
		if (level == 1) {
			nodes.push_back(*this);
		} else {
			if (mLeft) {
				mLeft->GetLevel(level - 1, nodes);
			}

			if (mRight) {
				mRight->GetLevel(level - 1, nodes);
			}
		}
	}

	inline void Render(SDL_Renderer* renderer) const {
		mLeaf.RenderRectangle(renderer);

		if (mLeft) {
			mLeft->Render(renderer);
		}

		if (mRight) {
			mRight->Render(renderer);
		}
	}

	inline const Rectangle<T>& GetLeaf() const {
		return mLeaf;
	}

	inline NodeTreeBinary<T>* const GetLeft() const {
		return mLeft;
	}

	inline NodeTreeBinary<T>* const GetRight() const {
		return mRight;
	}

	inline void SetLeaf(const Rectangle<T>& leaf) {
		mLeaf = leaf;
	}

	inline void SetLeft(NodeTreeBinary<T>* const left) {
		mLeft = left;
	}

	inline void SetRight(NodeTreeBinary<T>* const right) {
		mRight = right;
	}

private:
	Rectangle<T> mLeaf {};
	NodeTreeBinary<T>* mLeft = nullptr;
	NodeTreeBinary<T>* mRight = nullptr;
};

template<typename T>
class Room {
public:
	inline Room() = default;

	inline Room(const Rectangle<T>& rectangle) {
		mRectangle.SetX(rectangle.GetX() + random((T)0, floor(rectangle.GetW() / (T)3)));
		mRectangle.SetY(rectangle.GetY() + random((T)0, floor(rectangle.GetH() / (T)3)));
		mRectangle.SetW(rectangle.GetW() - (mRectangle.GetX() - rectangle.GetX()));
		mRectangle.SetH(rectangle.GetH() - (mRectangle.GetY() - rectangle.GetY()));
		mRectangle.SetW(mRectangle.GetW() - random((T)0, mRectangle.GetW() / (T)3));
		mRectangle.SetH(mRectangle.GetH() - random((T)0, mRectangle.GetW() / (T)3));
	}

	inline void Render(SDL_Renderer* renderer) const {
		const T squareAsT = (T)SQUARE;

		SDL_FRect frect { float(mRectangle.GetX() * squareAsT), float(mRectangle.GetY() * squareAsT),
						  float(mRectangle.GetW() * squareAsT), float(mRectangle.GetH() * squareAsT) };
		SDL_RenderFillRectF(renderer, &frect);
	}

private:
	Rectangle<T> mRectangle {};
};

class Map {
public:
	void Render(SDL_Renderer* renderer) {
		container_tree->Render(renderer);
		SDL_RenderPresent(renderer);
		RenderPaths(renderer, container_tree);
		list<NodeTreeBinary<float>> leafs;
		container_tree->GetLeafs(leafs);
		for (const auto& leaf : leafs) {
			Room(leaf.GetLeaf()).Render(renderer);
		}
	}

private:
	pair<Rectangle<float>*, Rectangle<float>*> random_split(const Rectangle<float>& container) {
		Rectangle<float>* r1, * r2;
		if (!random(0., 1.)) {
			// Vertical
			r1 = new Rectangle<float>(
				container.GetX(), container.GetY(),             // r1.x, r1.y
				random(1.f, container.GetW()), container.GetH()   // r1.w, r1.h
				);
			r2 = new Rectangle<float>(
				container.GetX() + r1->GetW(), container.GetY(),      // r2.x, r2.y
				container.GetW() - r1->GetW(), container.GetH()       // r2.w, r2.h
				);

			if (DISCARD_BY_RATIO) {
				auto r1_w_ratio = r1->GetW() / r1->GetH();
				auto r2_w_ratio = r2->GetW() / r2->GetH();
				if (r1_w_ratio < W_RATIO || r2_w_ratio < W_RATIO) {
					return random_split(container);
				}
			}
		} else {
			// Horizontal
			r1 = new Rectangle<float>(
				container.GetX(), container.GetY(),             // r1.x, r1.y
				container.GetW(), random(1.f, container.GetH())   // r1.w, r1.h
				);
			r2 = new Rectangle<float>(
				container.GetX(), container.GetY() + r1->GetH(),      // r2.x, r2.y
				container.GetW(), container.GetH() - r1->GetH()       // r2.w, r2.h
				);

			if (DISCARD_BY_RATIO) {
				auto r1_h_ratio = r1->GetH() / r1->GetW();
				auto r2_h_ratio = r2->GetH() / r2->GetW();
				if (r1_h_ratio < H_RATIO || r2_h_ratio < H_RATIO) {
					return random_split(container);
				}
			}
		}
		return{ r1, r2 };
	}

	NodeTreeBinary<float>* split_container(Rectangle<float>* container, int iter) {
		NodeTreeBinary<float>* root = new NodeTreeBinary<float>(*container);
		if (iter) {
			auto sr = random_split(*container);
			root->SetLeft(split_container(sr.first, iter - 1));
			root->SetRight(split_container(sr.second, iter - 1));
		}

		return root;
	}

	void RenderPaths(SDL_Renderer* renderer, NodeTreeBinary<float>* const tree) {
		if (!tree->GetLeft() || !tree->GetRight()) {
			return;
		}

		tree->GetLeft()->GetLeaf().RenderPath(renderer, tree->GetRight()->GetLeaf(), 5);
		RenderPaths(renderer, tree->GetLeft());
		RenderPaths(renderer, tree->GetRight());
	}

	Rectangle<float> main_container = Rectangle<float>(0, 0, WINDOW_WIDTH_START, WINDOW_HEIGHT_START);
	NodeTreeBinary<float>* container_tree = split_container(&main_container, (int)N_ITERATIONS);
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

		Map map;
		map.Render(window.GetRenderer());

		SDL_RenderPresent(window.GetRenderer());
		SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

		//SDL_Delay(1000);
	}

	return EXIT_SUCCESS;
}

/*
	TO DO:
			- finish the refactoring
			- fix the memory leak
			- make the nodetreebinary a useful class
			- think about not template for specific classes for Map
*/