#include "Floater.hpp"
#include "GameUtilities.hpp"
#include "ShaderProgram.h"
#include "SatCollision.h"

Bullet::Bullet(const SheetSprite& sprite) {
    this->sprite = new SheetSprite(sprite);
    shape = new Rectangle(2 * this->sprite->width * this->sprite->size,
                          2 * this->sprite->height * this->sprite->size);
    active = false;
}

Bullet::~Bullet() {
    delete shape;
    delete sprite;
}

void Bullet::Update(float elapsed) {
    position.x += velocity.x * elapsed;
    position.y += velocity.y * elapsed;
}

void Bullet::Render(ShaderProgram& program) {
    Matrix modelMatrix;
    modelMatrix.Identity();
    modelMatrix.Translate(position.x, position.y, position.z);
    program.SetModelMatrix(modelMatrix);
    
    sprite->Render(program);
}

bool Bullet::CollideWithMap(const FlareMap& map, const std::unordered_set<unsigned int>& solidTiles) {
    if (!active) return false;
    
    int x, y;
    map.worldToTileCoordinates(position.x, position.y, x, y);
    if (x < 0 || y < 0 ||
        map.mapData[y][x] == 0 ||
        solidTiles.find(map.mapData[y][x] - 1) == solidTiles.end()) return false;
    
    active = false;
    return true;
}

bool Bullet::CollidesWith(Entity& entity) {
    if (!active) return false;
    bool collided = position.x > entity.position.x - entity.shape->size.x / 2 &&
                    position.x < entity.position.x + entity.shape->size.x / 2 &&
                    position.y < entity.position.y + entity.shape->size.y / 2 &&
                    position.y > entity.position.y - entity.shape->size.y / 2;
    
    if (collided) active = false;
    return collided;
}

/*---------------------------------------- Floater ------------------------------------------*/

Floater::Floater(float x, float y, float range, float shootSpeed, float bulletSize)
: Entity(x, y, Rectangle(0.0f, 0.0f), ENTITY_FLOATING), range(range),
    shootSpeed(shootSpeed), bulletSize(bulletSize) {
    bulletIndex = 0;
    secondsSinceShot = 1 / shootSpeed;
    state = STATE_IDLE;
    currentAction = ACTION_DEFENDING;
    bullets.reserve(MAX_BULLETS);
    
    for (int i = 0 ; i < MAX_BULLETS; i++) {
        float x, y, width, height;
        textureAtlas.GetSpriteData("redGem", x, y, width, height);
        SheetSprite sprite(textures[OBJECTS], x / 1024, y / 1024, width / 1024, height / 1024, bulletSize);
        bullets.emplace_back(sprite);
    }
}

void Floater::ShootBullet(float x, float y, float velX, float velY) {
    Bullet& bullet = bullets[bulletIndex];
    bullet.position.x = x;
    bullet.position.y = y;
    bullet.velocity.x = velX;
    bullet.velocity.y = velY;
    bullet.active = true;
    
    ++bulletIndex;
    if (bulletIndex > MAX_BULLETS - 1) {
        bulletIndex = 0;
    }
}

void Floater::Shoot() {
    float leftX = position.x - shape->size.x / 2;
    float rightX = position.x + shape->size.x / 2;
    float topY = position.y + shape->size.y / 2;
    float botY = position.y - shape->size.y / 2;
    
    ShootBullet(leftX, topY, -BULLET_SPEED, BULLET_SPEED);
    ShootBullet(rightX, topY, BULLET_SPEED, BULLET_SPEED);
    ShootBullet(leftX, botY, -BULLET_SPEED, -BULLET_SPEED);
    ShootBullet(rightX, botY, BULLET_SPEED, -BULLET_SPEED);
}

void Floater::Update(Entity& target, float elapsed) {
    switch (state) {
        case STATE_IDLE:
            if (distance(target.position, position) < range) {
                state = STATE_ATTACKING;
                currentAction = ACTION_ATTACKING;
            }
            break;
        case STATE_ATTACKING:
            if (secondsSinceShot >= 1 / shootSpeed) {
                Shoot();
                secondsSinceShot -= 1 / shootSpeed;
            }
            else {
                secondsSinceShot += elapsed;
            }
            if (distance(target.position, position) > range) {
                secondsSinceShot = 1 / shootSpeed;
                state = STATE_IDLE;
                currentAction = ACTION_DEFENDING;
            }
            break;
        default:
            break;
    }
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].Update(elapsed);
    }
}

void Floater::Render(ShaderProgram& program) {
    Entity::Render(program);
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].Render(program);
        }
    }
}

void Floater::CollideWithBullets(Entity& target) {
    for (Bullet& bullet : bullets) {
        bool collided = bullet.CollidesWith(target);
        if (collided) {
            // Play sound
            if (target.currentAction != ACTION_DEFENDING) {
                // TODO: Target loses hp
                // Invulnerability mode
                // Maybe play a hurt sound
                return;
            }
        }
    }
}

