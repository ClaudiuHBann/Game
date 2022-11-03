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

Path::Path(const Rectangle<float>& rectOne, const Rectangle<float>& rectTwo, const float width /* = 1.f */)
    : mWidth(width) {
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

void Path::AddOffset(const Point<float>& offset) {
    mRectangle.SetX(mRectangle.GetX() + offset.GetX());
    mRectangle.SetY(mRectangle.GetY() + offset.GetY());
}

void Path::AddSize(const Point<float>& size) {
    mRectangle.SetW(mRectangle.GetW() + size.GetX());
    mRectangle.SetH(mRectangle.GetH() + size.GetY());
}

void Path::SetWidth(const float width) {
    if (mWidth == width) {
        return;
    }

    const auto widthHalfOld = mWidth / 2.f;
    const auto widthHalfNew = width / 2.f;

    if (mRectangle.GetW() == mWidth) {
        mRectangle.SetY(mRectangle.GetY() - widthHalfOld + widthHalfNew);
        mRectangle.SetW(width);
    } else {
        mRectangle.SetX(mRectangle.GetX() - widthHalfOld + widthHalfNew);
        mRectangle.SetH(width);
    }

    mWidth = width;
}

const Rectangle<float>& Path::GetRectangle() const {
    return mRectangle;
}

Dungeon::Dungeon(
    const size_t iterations,
    const Point<float>& size,
    const float tileSize /* = 5.f */,
    const Point<float>& ratioToDiscard /* = { 0.45f, 0.45f } */
) :
    mCanvas(0.f, 0.f, size.GetX(), size.GetY()),
    mTileSize(tileSize),
    mRatioToDiscard(ratioToDiscard),
    mTree(SplitRectangle(mCanvas, iterations)) {

    GenerateRooms();
    GeneratePaths(mTree, mPaths);
}

void Dungeon::Render(
    SDL_Renderer* renderer,
    const Point<float>& scale /* = { 1.f, 1.f } */,
    const Point<float>& offset /* = {} */
) {
    SDL_FPoint scaleLast {};
    SDL_RenderGetScale(renderer, &scaleLast.x, &scaleLast.y);

    SDL_RenderSetScale(renderer, scale.GetX(), scale.GetY());

    //RenderTree(renderer, mTree, offset);
    RenderPaths(renderer, offset);
    RenderRooms(renderer, offset);

    SDL_RenderSetScale(renderer, scaleLast.x, scaleLast.y);
}

Dungeon::~Dungeon() {
    DeleteTree(mTree);
    mTree = nullptr;
}

void Dungeon::RenderRoom(
    SDL_Renderer* renderer,
    const Room& room,
    const Point<float>& offset /* = {} */
) const {
    SDL_FRect frect { room.GetRectangle().GetX(), room.GetRectangle().GetY(),
                      room.GetRectangle().GetW(), room.GetRectangle().GetH() };
    AddOffset(frect, offset);

    SDL_RenderFillRectF(renderer, &frect);
}

void Dungeon::RenderRooms(SDL_Renderer* renderer, const Point<float>& offset /* = {} */) const {
    for (const auto& room : mRooms) {
        RenderRoom(renderer, room, offset);
    }
}

void Dungeon::RenderPath(
    SDL_Renderer* renderer,
    const Path& path,
    const Point<float>& offset /* = {} */
) const {
    SDL_FRect frect { path.GetRectangle().GetX(), path.GetRectangle().GetY(),
                      path.GetRectangle().GetW(), path.GetRectangle().GetH() };
    AddOffset(frect, offset);

    SDL_RenderFillRectF(renderer, &frect);
}

void Dungeon::RenderPaths(
    SDL_Renderer* renderer,
    const Point<float>& offset /* = {} */
) {
    for (auto& path : mPaths) {
        RenderPath(renderer, path, offset);
    }
}

void Dungeon::RenderRectangle(
    SDL_Renderer* renderer,
    const Rectangle<float>& rectangle,
    const Point<float>& offset /* = {} */
) const {
    SDL_FRect frect { rectangle.GetX(), rectangle.GetY(),
                      rectangle.GetW(), rectangle.GetH() };
    AddOffset(frect, offset);

    SDL_RenderDrawRectF(renderer, &frect);
}

void Dungeon::RenderTree(
    SDL_Renderer* renderer,
    NodeTreeBinary<Rectangle<float>>* const tree,
    const Point<float>& offset /* = {} */
) const {
    RenderRectangle(renderer, tree->GetLeaf(), offset);

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

void Dungeon::GenerateRooms() {
    list<NodeTreeBinary<Rectangle<float>>> leafs;
    mTree->GetLeafs(leafs);
    for (const auto& leaf : leafs) {
        Rectangle<float> room(leaf.GetLeaf());

        const auto trimLeft = fmod(room.GetX(), mTileSize);
        if (trimLeft) {
            room.SetX(room.GetX() + (mTileSize - trimLeft));
        }

        const auto trimRight = fmod(room.GetW(), mTileSize);
        room.SetW(room.GetW() - trimRight);

        const auto trimTop = fmod(room.GetY(), mTileSize);
        if (trimTop) {
            room.SetY(room.GetY() + (mTileSize - trimTop));
        }

        const auto trimBottom = fmod(room.GetH(), mTileSize);
        room.SetH(room.GetH() - trimBottom);

        mRooms.push_back(room);
    }
}

void Dungeon::GeneratePaths(NodeTreeBinary<Rectangle<float>>* const tree, list<Path>& paths) {
    if (!tree->GetLeft() || !tree->GetRight()) {
        return;
    }

    Path path(tree->GetLeft()->GetLeaf(), tree->GetRight()->GetLeaf(), mTileSize);

    const auto offsetX = fmod(path.GetRectangle().GetX(), mTileSize);
    if (offsetX > 5.f) {
        path.AddOffset({ mTileSize - offsetX, 0.f });
    } else {
        path.AddOffset({ -offsetX, 0.f });
    }

    const auto offsetY = fmod(path.GetRectangle().GetY(), mTileSize);
    if (offsetY > 5.f) {
        path.AddOffset({ 0.f, mTileSize - offsetY });
    } else {
        path.AddOffset({ 0.f, -offsetY });
    }

    if (path.GetRectangle().GetH() == mTileSize) {
        const auto trimW = fmod(path.GetRectangle().GetW(), mTileSize);
        if (trimW > 5.f) {
            path.AddSize({ mTileSize - trimW, 0.f });
        } else {
            path.AddSize({ -trimW, 0.f });
        }
    } else {
        const auto trimH = fmod(path.GetRectangle().GetH(), mTileSize);
        if (trimH > 5.f) {
            path.AddSize({ 0.f, mTileSize - trimH });
        } else {
            path.AddSize({ 0.f, -trimH });
        }
    }

    paths.push_back(path);

    GeneratePaths(tree->GetLeft(), paths);
    GeneratePaths(tree->GetRight(), paths);
}

void Dungeon::DeleteTree(NodeTreeBinary<Rectangle<float>>* tree) {
    if (!tree) {
        return;
    }

    DeleteTree(tree->GetLeft());
    DeleteTree(tree->GetRight());

    delete tree;
}

void Dungeon::AddOffset(SDL_FRect& rect, const Point<float>& offset) const {
    rect.x += offset.GetX();
    rect.y += offset.GetY();
}