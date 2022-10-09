#include "pch.h"
#include "Core/Window.h"
#include "Core/ManagerTexture.h"

#define WINDOW_WIDTH_START (640)
#define WINDOW_HEIGHT_START (480)

#define WINDOW_FLAGS (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL)
#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

#define MAP_SIZE 1
#define SQUARE 1
#define N_ITERATIONS 4
#define DISCARD_BY_RATIO true
#define H_RATIO 0.45
#define W_RATIO 0.45

#include <list>

float random(float min, float max) {
	return floor(rand() % int(abs(max - min) + 1) + min);
}

class Point {
public:
	Point(float x, float y)
		: x(x), y(y) {}

	float distance(const Point& point) {
		return sqrt((point.x - this->x) * (point.x - this->x) + (point.y - this->y) * (point.y - this->y));
	}

	float x = 0.f;
	float y = 0.f;
};

class Container {
public:
	Container() = default;

	Container(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h), center({ x + (w / 2.f), y + (h / 2.f) }) {}

	void paint(SDL_Renderer* renderer) {
		SDL_FRect frect { x * SQUARE, y * SQUARE,
						 w * SQUARE, h * SQUARE };
		SDL_RenderDrawRectF(renderer, &frect);
	}

	void drawPath(SDL_Renderer* ctx, const Container& container) {
		float scale = 5.f;
		SDL_RenderSetScale(ctx, scale, scale);

		SDL_RenderDrawLineF(ctx, center.x * SQUARE / scale, center.y * SQUARE / scale,
							container.center.x * SQUARE / scale, container.center.y * SQUARE / scale);

		SDL_RenderSetScale(ctx, 1.f, 1.f);
	}

	float x = 0.f;
	float y = 0.f;
	float w = 0.f;
	float h = 0.f;

	Point center { 0.f , 0.f };
};

class Node {
public:
	Node(const Container& leaf) : leaf(leaf) {}

	void getLeafs(list<Node>& nodes) {
		if (!left && !right)
			nodes.push_back(leaf);
		else {
			left->getLeafs(nodes);
			right->getLeafs(nodes);
		}
	}

	void getLevel(int level, list<Node>& nodes) {
		if (level == 1) {
			nodes.push_back(*this);
		} else {
			if (left)
				left->getLevel(level - 1, nodes);
			if (right)
				right->getLevel(level - 1, nodes);
		}
	}

	void paint(SDL_Renderer* renderer) {
		leaf.paint(renderer);
		if (left)
			left->paint(renderer);
		if (right)
			right->paint(renderer);
	}

	Container leaf { 0.f, 0.f, 0.f, 0.f };
	Node* left = nullptr;
	Node* right = nullptr;
};

pair<Container*, Container*> random_split(const Container& container);

Node* split_container(Container* container, int iter) {
	Node* root = new Node(*container);
	if (iter) {
		auto sr = random_split(*container);
		root->left = split_container(sr.first, iter - 1);
		root->right = split_container(sr.second, iter - 1);
	}

	return root;
}

pair<Container*, Container*> random_split(const Container& container) {
	Container* r1, * r2;
	if (!random(0, 1)) {
		// Vertical
		r1 = new Container(
			container.x, container.y,             // r1.x, r1.y
			random(1, container.w), container.h   // r1.w, r1.h
		);
		r2 = new Container(
			container.x + r1->w, container.y,      // r2.x, r2.y
			container.w - r1->w, container.h       // r2.w, r2.h
		);

		if (DISCARD_BY_RATIO) {
			auto r1_w_ratio = r1->w / r1->h;
			auto r2_w_ratio = r2->w / r2->h;
			if (r1_w_ratio < W_RATIO || r2_w_ratio < W_RATIO) {
				return random_split(container);
			}
		}
	} else {
		// Horizontal
		r1 = new Container(
			container.x, container.y,             // r1.x, r1.y
			container.w, random(1, container.h)   // r1.w, r1.h
		);
		r2 = new Container(
			container.x, container.y + r1->h,      // r2.x, r2.y
			container.w, container.h - r1->h       // r2.w, r2.h
		);

		if (DISCARD_BY_RATIO) {
			auto r1_h_ratio = r1->h / r1->w;
			auto r2_h_ratio = r2->h / r2->w;
			if (r1_h_ratio < H_RATIO || r2_h_ratio < H_RATIO) {
				return random_split(container);
			}
		}
	}
	return{ r1, r2 };
}

class Room {
public:
	Room(const Container& container)
		: x(container.x + random(0, floor(container.w / 3))),
		y(container.y + random(0, floor(container.h / 3))),
		w(container.w - (x - container.x)),
		h(container.h - (y - container.y)) {
		w -= random(0, w / 3);
		h -= random(0, w / 3);
	}

	void paint(SDL_Renderer* renderer) {
		SDL_FRect frect { x * SQUARE, y * SQUARE,
						 w * SQUARE, h * SQUARE };
		SDL_RenderFillRectF(renderer, &frect);
	}

	float x = 0.f;
	float y = 0.f;
	float w = 0.f;
	float h = 0.f;
};

void draw_paths(SDL_Renderer* ctx, Node* tree) {
	if (!tree->left || !tree->right)
		return;
	tree->left->leaf.drawPath(ctx, tree->right->leaf);
	draw_paths(ctx, tree->left);
	draw_paths(ctx, tree->right);
}

int main(int /*argc*/, char** /*argv*/) {
	Window window(
		"Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH_START, WINDOW_HEIGHT_START,
		WINDOW_FLAGS, RENDERER_FLAGS
	);

	bool isRunning = true;
	SDL_Event event {};

	/*ManagerTexture managerTexture;
	SDL_Texture* textureWall = managerTexture.Load(window.GetRenderer(), "wall.png");*/

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

		//managerTexture.Render(window.GetRenderer(), textureWall, { 0, 0 });

		Container* main_container = new Container(0, 0, WINDOW_WIDTH_START, WINDOW_HEIGHT_START);
		auto container_tree = split_container(main_container, N_ITERATIONS);

		//container_tree->paint(window.GetRenderer());
		SDL_RenderPresent(window.GetRenderer());
		draw_paths(window.GetRenderer(), container_tree);
		list<Node> leafs;
		container_tree->getLeafs(leafs);
		for (const auto& leaf : leafs) {
			Room(leaf.leaf).paint(window.GetRenderer());
		}

		SDL_RenderPresent(window.GetRenderer());
		SDL_SetRenderDrawColor(window.GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

		cin.get();
	}

	return EXIT_SUCCESS;
}