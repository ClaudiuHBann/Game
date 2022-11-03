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

    void AddOffset(const Point<float>& offset);

    void AddSize(const Point<float>& size);

    void SetWidth(const float width);

    const Rectangle<float>& GetRectangle() const;

private:
    Rectangle<float> mRectangle {};
    float mWidth {};
};

class Dungeon {
public:
    enum class Tile : uint8_t {
        NONE,
        ROOM,
        PATH
    };

    Dungeon(
        const size_t iterations,
        const Point<float>& size,
        const float tileSize = 5.f,
        const Point<float>& ratioToDiscard = { 0.45f, 0.45f }
    );

    void GenerateTileMatrix(vector<vector<Tile>>& tileMatrix);

    void Render(
        SDL_Renderer* renderer,
        const Point<float>& scale = { 1.f, 1.f },
        const Point<float>& offset = {}
    );

    ~Dungeon();

private:
    void RenderRoom(
        SDL_Renderer* renderer,
        const Room& room,
        const Point<float>& offset = {}
    ) const;

    void RenderRooms(SDL_Renderer* renderer, const Point<float>& offset = {}) const;

    void RenderPath(
        SDL_Renderer* renderer,
        const Path& path,
        const Point<float>& offset = {}
    ) const;

    void RenderPaths(
        SDL_Renderer* renderer,
        const Point<float>& offset = {}
    );

    void RenderRectangle(
        SDL_Renderer* renderer,
        const Rectangle<float>& rectangle,
        const Point<float>& offset = {}
    ) const;

    void RenderTree(
        SDL_Renderer* renderer,
        NodeTreeBinary<Rectangle<float>>* const tree,
        const Point<float>& offset = {}
    ) const;

    pair<Rectangle<float>, Rectangle<float>> SplitRandom(const Rectangle<float>& rectangle) const;

    NodeTreeBinary<Rectangle<float>>* SplitRectangle(const Rectangle<float>& container, const size_t iterations) const;

    void GenerateRooms();

    void GeneratePaths(NodeTreeBinary<Rectangle<float>>* const tree, list<Path>& paths);

    void DeleteTree(NodeTreeBinary<Rectangle<float>>* tree);

    void AddOffset(SDL_FRect& rect, const Point<float>& offset) const;

    Rectangle<float> mCanvas {};
    Point<float> mRatioToDiscard {};

    NodeTreeBinary<Rectangle<float>>* mTree = nullptr;
    list<Room> mRooms {};
    list<Path> mPaths {};

    float mTileSize {};
};