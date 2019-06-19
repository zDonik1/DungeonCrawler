#pragma once

#include <SFML/Graphics.hpp>
#include "sharedcontext.h"

// defines standard sizes for map
enum Sheet
{
    Tile_Size = 32,
    Sheet_Width = 256,
    Sheet_Height = 256,
    Num_Layers = 4
};

using TileID = unsigned int;

// stores necessary information about a tile
struct TileInfo
{
    // assigns texture to the sprite from TextureManager and calculates tileBoundaries
    TileInfo(SharedContext* l_context, const std::string& l_texture = "", TileID l_id = 0) :
        m_id(0), m_deadly(false), m_context(l_context)
    {
        TextureManager* tmgr = l_context->m_textureManager;
        if (l_texture == "") {
            m_id = l_id;
            return;
        }
        if (!tmgr->requireResource(l_texture))
            return;

        m_texture = l_texture;
        m_id = l_id;
        m_sprite.setTexture(*tmgr->getResource(m_texture));

        sf::IntRect tileBoundaries(m_id % (Sheet::Sheet_Width / Sheet::Tile_Size) * Sheet::Tile_Size,
                                   m_id / (Sheet::Sheet_Height / Sheet::Tile_Size) * Sheet::Tile_Size,
                                   Sheet::Tile_Size,
                                   Sheet::Tile_Size);
        m_sprite.setTextureRect(tileBoundaries);
    }

    ~TileInfo()
    {
        if (m_texture == "") // texture wasnt loaded into sprite
            return;

        m_context->m_textureManager->releaseResource(m_texture);
    }


    sf::Sprite m_sprite;

    TileID m_id;
    std::string m_name;
    sf::Vector2f m_friction;
    bool m_deadly;

    SharedContext* m_context;
    std::string m_texture; // name of the texture used
};

// we use flyweight pattern to simply store a pointer to TileInfo
struct Tile
{
    TileInfo* m_properties;
    bool m_warp; // this is unique to each tile therefore not in TileInfo
    bool m_solid;
};

using TileMap = std::unordered_map<TileID, Tile*>;
using TileSet = std::unordered_map<TileID, TileInfo*>;

class Map
{
public:
    Map(SharedContext* l_context);
    ~Map();

    Tile *getTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer);
    TileInfo *getDefaultTile();
    unsigned int getTileSize() const;
    const sf::Vector2u& getMapSize() const;
    const sf::Vector2f& getPlayerStart() const;
    int getPlayerID() const;

    void loadMap(const std::string& l_path);

    void update(float l_dT);
    void draw(unsigned int l_layer);

private:
    unsigned int convertCoords(const unsigned int& l_x, const unsigned int& l_y, unsigned int l_layer) const;
    void loadTiles(const std::string& l_path);
    void purgeMap();
    void purgeTileSet();

    TileSet m_tileSet; // types of tiles
    TileMap m_tileMap; // tile pointers and positions in the map

    TileInfo m_defaultTile; // desribes the default tile information

    sf::Vector2u m_maxMapSize;
    sf::Vector2f m_playerStart; // player starting position
    int m_playerID;
    unsigned int m_tileCount; // number of tiles on the map
    unsigned int m_tileSetCount; // number of types of tiles
    SharedContext* m_context;
};
