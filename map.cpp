#include "map.h"
#include "statemanager.h"

Map::Map(SharedContext* l_context)
    : m_defaultTile(l_context)
    , m_maxMapSize(32, 32)
    , m_playerID(-1)
    , m_tileCount(0)
    , m_tileSetCount(0)
    , m_context(l_context)
{
    m_context->m_gameMap = this;
    loadTiles("tiles.cfg");
}

Map::~Map()
{
    purgeMap();
    purgeTileSet();
    m_context->m_gameMap = nullptr;
}

Tile *Map::getTile(unsigned int l_x, unsigned int l_y, unsigned int l_layer)
{
    if (l_x < 0 || l_y < 0 || l_x >= m_maxMapSize.x || l_y >= m_maxMapSize.y || l_layer < 0 || l_layer >= Sheet::Num_Layers) {
        return nullptr;
    }

    auto itr = m_tileMap.find(convertCoords(l_x, l_y, l_layer));
    if (itr == m_tileMap.end()) {
        return nullptr;
    }

    return itr->second;
}

TileInfo *Map::getDefaultTile()
{
    return &m_defaultTile;
}

unsigned int Map::getTileSize() const
{
    return Sheet::Tile_Size;
}

// converts from 2D coordinates to an integer id
unsigned int Map::convertCoords(const unsigned int& l_x, const unsigned int& l_y, unsigned int l_layer) const
{
    return ((l_layer * m_maxMapSize.y + l_y) * m_maxMapSize.x + l_x);
}

void Map::update(float l_dT)
{
}

// uses a technique called culling (draws tiles only in the view) to draw the tiles on the screen
void Map::draw(unsigned int l_layer)
{
    if (l_layer >= Sheet::Num_Layers) {
        return;
    }

    sf::RenderWindow *l_wind = m_context->m_wind->getRenderWindow();
    sf::FloatRect viewSpace = m_context->m_wind->getViewSpace();

    sf::Vector2i tileBegin(floor(viewSpace.left / Sheet::Tile_Size),
                           floor(viewSpace.top / Sheet::Tile_Size));
    sf::Vector2i tileEnd(ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size),
                         ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size));

    unsigned int count = 0;
    for (int x = tileBegin.x; x <= tileEnd.x; ++x) {
        for (int y = tileBegin.y; y <= tileEnd.y; ++y) {
            Tile *tile = getTile(x, y, l_layer);
            if (!tile) {
                continue;
            }

            sf::Sprite &sprite = tile->m_properties->m_sprite;
            sprite.setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
            l_wind->draw(sprite);
            ++count;

            // DEBUG
            if (!m_context->m_debugOverlay.getDebug()) {
                continue;
            }

            if (!tile->m_properties->m_deadly && !tile->m_warp && !tile->m_solid) {
                continue;
            }

            sf::RectangleShape *tileMarker = new sf::RectangleShape(sf::Vector2f(Sheet::Tile_Size, Sheet::Tile_Size));
            tileMarker->setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
            if (tile->m_properties->m_deadly) {
                tileMarker->setFillColor(sf::Color(255, 0, 0, 100));
            }
            else if (tile->m_warp) {
                tileMarker->setFillColor(sf::Color(0, 255, 0, 150));
            }
            else if (tile->m_solid) {
                tileMarker->setFillColor(sf::Color(0, 0, 255, 150));
            }
            m_context->m_debugOverlay.add(tileMarker);
        }
    }
}

void Map::purgeMap()
{
    while (m_tileMap.begin() != m_tileMap.end()) {
        delete m_tileMap.begin()->second;
        m_tileMap.erase(m_tileMap.begin());
    }
    m_tileCount = 0;
    m_context->m_entityManager->purge();
}

void Map::purgeTileSet()
{
    while (m_tileSet.begin() != m_tileSet.end()) {
        delete m_tileSet.begin()->second;
        m_tileSet.erase(m_tileSet.begin());
    }
    m_tileSetCount = 0;
}

// loads tile types from a file
void Map::loadTiles(const std::string& l_path)
{
    std::ifstream tileSetFile;
    tileSetFile.open(Utils::getWorkingDirectory() + "config/" + l_path);
    if (!tileSetFile.is_open()) {
        std::cout << "! Failed loading tile set file: " << l_path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(tileSetFile, line)) {
        if (line[0] == '|')
            continue;

        std::stringstream keystream(line);

        int tileId;
        keystream >> tileId;
        if (tileId < 0)
            continue;

        TileInfo* tile = new TileInfo(m_context, "TileSheet", tileId);
        keystream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >> tile->m_deadly;
        if (!m_tileSet.emplace(tileId, tile).second) {
            std::cout << "! Duplicate tile type: " << tile->m_name << std::endl;
            delete tile;
            tile = nullptr;
        }
    }

    tileSetFile.close();
}

// loads the map of tiles from a file
void Map::loadMap(const std::string& l_path)
{
    std::ifstream mapFile;
    mapFile.open(Utils::getWorkingDirectory() + l_path);
    if (!mapFile.is_open()) {
        std::cout << "! Failed loading map file: " << l_path << std::endl;
        return;
    }

    std::string line;
    std::cout << "Loading a map: " << l_path << std::endl;

    while (std::getline(mapFile, line)) {
        if (line[0] == '|')
            continue;

        std::stringstream keystream(line);

        std::string type;
        keystream >> type;
        if (type == "TILE") { // loads tile information
            int tileId = 0;
            keystream >> tileId;
            if (tileId < 0) {
                std::cout << "! Bad tile id: " << tileId << std::endl;
                continue;
            }
            auto itr = m_tileSet.find(tileId);
            if (itr == m_tileSet.end()) {
                std::cout << "! Tile id(" << tileId << ") was not found in tilesheet." << std::endl;
                continue;
            }
            sf::Vector2i tileCoords;
            unsigned int tileLayer = 0;
            unsigned int tileSolidity = 0;
            keystream >> tileCoords.x >> tileCoords.y >> tileLayer >> tileSolidity;
            if (tileCoords.x > m_maxMapSize.x || tileCoords.y > m_maxMapSize.y || tileLayer >= Sheet::Num_Layers) {
                std::cout << "! Tile is out of range: " << tileCoords.x << " " << tileCoords.y << std::endl;
                continue;
            }
            Tile* tile = new Tile();
            // Bind properties of a tile from a set.
            tile->m_properties = itr->second;
            tile->m_solid = (bool)tileSolidity;
            if (!m_tileMap.emplace(convertCoords(tileCoords.x, tileCoords.y, tileLayer), tile).second) {
                std::cout << "! Duplicate tile! : " << tileCoords.x << " " << tileCoords.y << std::endl;
                delete tile;
                tile = nullptr;
                continue;
            }
            std::string warp;
            keystream >> warp;
            tile->m_warp = false;
            if (warp == "WARP") {
                tile->m_warp = true;
            }
        }
        else if (type == "SIZE") {
            keystream >> m_maxMapSize.x >> m_maxMapSize.y;
        }
        else if (type == "DEFAULT_FRICTION") {
            keystream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
        }
        else if (type == "ENTITY") {
            std::string name;
            keystream >> name;
            if (name == "Player" && m_playerID != -1) {
                continue;
            }
            int entityID = m_context->m_entityManager->addEntity(name);
            if (entityID < 0) {
                continue;
            }

            if (name == "Player") {
                m_playerID = entityID;
            }

            C_Base *position = m_context->m_entityManager->getComponent<C_Position>(entityID, Component::Position);
            if (position) {
                keystream >> *position;
            }
        }
        else {
            std::cout << "! Unknown type \"" << type << "\"." << std::endl;
        }
    }

    mapFile.close();
    std::cout << "--- Map loaded! ---" << std::endl;
}

const sf::Vector2u& Map::getMapSize() const
{
    return m_maxMapSize;
}

const sf::Vector2f& Map::getPlayerStart() const
{
    return m_playerStart;
}

int Map::getPlayerID() const
{
    return m_playerID;
}
