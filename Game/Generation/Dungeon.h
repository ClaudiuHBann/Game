#pragma once

#include "Game/Core/Types.h"

class Room {
public:
    Room(const Rectangle<float>& rectangle);

    const Rectangle<float>& GetRectangle() const;

private:
    Rectangle<float> mRectangle {};
};

class Path {
public:
    Path(const Rectangle<float>& rectOne, const Rectangle<float>& rectTwo, const float width = 1.f);

    void SetWidth(const float width);

    const Rectangle<float>& GetRectangle() const;

private:
    Rectangle<float> mRectangle {};
    float mWidth {};
};

class Dungeon {
public:
    Dungeon(
        const size_t iterations,
        const Point<float>& size,
        const Point<float>& ratioToDiscard = { 0.45f, 0.45f }
    );

    void Render(SDL_Renderer* renderer, const Point<float>& scale = { 1.f, 1.f }, const float scalePath = 1.f);

    ~Dungeon();

private:
    void RenderRoom(SDL_Renderer* renderer, const Room& room) const;

    void RenderRooms(SDL_Renderer* renderer) const;

    void RenderPath(SDL_Renderer* renderer, const Path& path) const;

    void RenderPaths(SDL_Renderer* renderer, const float width /* = 1.f */);

    void RenderRectangle(SDL_Renderer* renderer, const Rectangle<float>& rectangle) const;

    void RenderTree(SDL_Renderer* renderer, NodeTreeBinary<Rectangle<float>>* const tree) const;

    pair<Rectangle<float>, Rectangle<float>> SplitRandom(const Rectangle<float>& rectangle) const;

    NodeTreeBinary<Rectangle<float>>* SplitRectangle(const Rectangle<float>& container, const size_t iterations) const;

    void GeneratePaths(NodeTreeBinary<Rectangle<float>>* const tree, list<Path>& paths);

    void DeleteTree(NodeTreeBinary<Rectangle<float>>* tree);

    Rectangle<float> mCanvas {};
    Point<float> mRatioToDiscard {};

    NodeTreeBinary<Rectangle<float>>* mTree = nullptr;
    list<Room> mRooms {};
    list<Path> mPaths {};
};