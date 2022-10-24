#include "Game/pch.h"
#include "Dungeon.h"

float RandomFloat(const float min, const float max) {
    static Random random {};
    return floor(random.GetReal(0.f, 1.f) * (max - min + 1.f) + min);
}

Room::Room(const Rectangle<float>& rectangle) {
    mRectangle.SetX(rectangle.GetX() + RandomFloat(0.f, floor(rectangle.GetW() / 3.f)));
    mRectangle.SetY(rectangle.GetY() + RandomFloat(0.f, floor(rectangle.GetH() / 3.f)));
    mRectangle.SetW(rectangle.GetW() - (mRectangle.GetX() - rectangle.GetX()));
    mRectangle.SetH(rectangle.GetH() - (mRectangle.GetY() - rectangle.GetY()));
    mRectangle.SetW(mRectangle.GetW() - RandomFloat(0.f, mRectangle.GetW() / 3.f));
    mRectangle.SetH(mRectangle.GetH() - RandomFloat(0.f, mRectangle.GetH() / 3.f));
}

const Rectangle<float>& Room::GetRectangle() const {
    return mRectangle;
}

Path::Path(const Rectangle<float>& rectOne, const Rectangle<float>& rectTwo, const float width /* = 1.f */) {
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

const Rectangle<float>& Path::GetRectangle() const {
    return mRectangle;
}

Dungeon::Dungeon(const size_t iterations, const Point<float>& size, const Point<float>& scale /* = { 1.f, 1.f } */, const float scalePath /* = 1.f */, const Point<float>& ratioToDiscard /* = { 0.45f, 0.45f } */)
    : mCanvas(0.f, 0.f, size.GetX(), size.GetY()),
    mScale(scale),
    mRatioToDiscard(ratioToDiscard),
    mTree(SplitRectangle(mCanvas, iterations)) {

    list<NodeTreeBinary<Rectangle<float>>> leafs;
    mTree->GetLeafs(leafs);
    for (const auto& leaf : leafs) {
        mRooms.push_back(leaf.GetLeaf());
    }

    GeneratePaths(mTree, mPaths, scalePath);
}

void Dungeon::Render(SDL_Renderer* renderer) const {
    SDL_FPoint scaleLast {};
    SDL_RenderGetScale(renderer, &scaleLast.x, &scaleLast.y);

    SDL_RenderSetScale(renderer, mScale.GetX(), mScale.GetY());
    //RenderTree(renderer, mTree);
    RenderPaths(renderer);
    RenderRooms(renderer);
    SDL_RenderSetScale(renderer, scaleLast.x, scaleLast.y);
}

Dungeon::~Dungeon() {
    DeleteTree(mTree);
    mTree = nullptr;
}

void Dungeon::RenderRoom(SDL_Renderer* renderer, const Room& room) const {
    const SDL_FRect frect { room.GetRectangle().GetX(), room.GetRectangle().GetY(),
        room.GetRectangle().GetW(), room.GetRectangle().GetH() };
    SDL_RenderFillRectF(renderer, &frect);
}

void Dungeon::RenderRooms(SDL_Renderer* renderer) const {
    for (const auto& room : mRooms) {
        RenderRoom(renderer, room);
    }
}

void Dungeon::RenderPath(SDL_Renderer* renderer, const Path& path) const {
    const SDL_FRect frect { path.GetRectangle().GetX(), path.GetRectangle().GetY(),
        path.GetRectangle().GetW(), path.GetRectangle().GetH() };
    SDL_RenderFillRectF(renderer, &frect);
}

void Dungeon::RenderPaths(SDL_Renderer* renderer) const {
    for (const auto& path : mPaths) {
        RenderPath(renderer, path);
    }
}

void Dungeon::RenderRectangle(SDL_Renderer* renderer, const Rectangle<float>& rectangle) const {
    const SDL_FRect frect { rectangle.GetX(), rectangle.GetY(),
        rectangle.GetW(), rectangle.GetH() };
    SDL_RenderDrawRectF(renderer, &frect);
}

void Dungeon::RenderTree(SDL_Renderer* renderer, NodeTreeBinary<Rectangle<float>>* const tree) const {
    RenderRectangle(renderer, tree->GetLeaf());

    if (tree->GetLeft()) {
        RenderTree(renderer, tree->GetLeft());
    }

    if (tree->GetRight()) {
        RenderTree(renderer, tree->GetRight());
    }
}

pair<Rectangle<float>, Rectangle<float>> Dungeon::SplitRandom(const Rectangle<float>& rectangle) const {
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

NodeTreeBinary<Rectangle<float>>* Dungeon::SplitRectangle(const Rectangle<float>& container, const size_t iterations) const {
    NodeTreeBinary<Rectangle<float>>* root = new NodeTreeBinary<Rectangle<float>>(container);
    if (iterations) {
        const auto pair = SplitRandom(container);

        root->SetLeft(SplitRectangle(pair.first, iterations - 1));
        root->SetRight(SplitRectangle(pair.second, iterations - 1));
    }

    return root;
}

void Dungeon::GeneratePaths(NodeTreeBinary<Rectangle<float>>* const tree, list<Path>& paths, const float width) {
    if (!tree->GetLeft() || !tree->GetRight()) {
        return;
    }

    paths.push_back(Path(tree->GetLeft()->GetLeaf(), tree->GetRight()->GetLeaf(), width));

    GeneratePaths(tree->GetLeft(), paths, width);
    GeneratePaths(tree->GetRight(), paths, width);
}

void Dungeon::DeleteTree(NodeTreeBinary<Rectangle<float>>* tree) {
    if (!tree) {
        return;
    }

    DeleteTree(tree->GetLeft());
    DeleteTree(tree->GetRight());

    delete tree;
}