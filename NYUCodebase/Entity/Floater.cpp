#include "Floater.hpp"
#include "GameUtilities.hpp"
#include "ShaderProgram.h"

Bullet::Bullet(const SheetSprite& sprite, float size) {
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
    position.x += velocity.x;
    position.y += velocity.y;
}

void Bullet::Render(ShaderProgram& program, Matrix& matrix) {
    Matrix modelMatrix = matrix;
    modelMatrix.Translate(position.x, position.y, position.z);
    program.SetModelMatrix(modelMatrix);
    
    sprite->Render(program);
}

/*---------------------------------------- Floater ------------------------------------------*/

Floater::Floater(float x, float y, float range, float shootSpeed)
: Entity(x, y, Rectangle(0.0f, 0.0f), ENTITY_FLOATING), range(range), shootSpeed(shootSpeed) {
    bulletIndex = 0;
    secondsSinceShot = 0.0f;
    bulletSize = 0.1f;
    state = STATE_IDLE;
    currentAction = ACTION_DEFENDING;
    bullets.reserve(MAX_BULLETS);
    
    for (int i = 0 ; i < MAX_BULLETS; i++) {
        float x, y, width, height;
        textureAtlas.GetSpriteData("redGem", x, y, width, height);
        SheetSprite sprite(textures[OBJECTS], x / 1024, y / 1024, width / 1024, height / 1024, 3);
        bullets.emplace_back(sprite, bulletSize * 10);
        bullets.back().parent = this;
    }
}

void Floater::ShootBullet(float x, float y, float velX, float velY) {
    Bullet& bullet = bullets[bulletIndex];
    bullet.shape->SetSize(bulletSize, bulletSize);
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
    float topY = position.y - shape->size.y / 2;
    float botY = position.y + shape->size.y / 2;
    
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
            if (secondsSinceShot >= shootSpeed) {
                Shoot();
                secondsSinceShot -= shootSpeed;
            }
            else {
                secondsSinceShot += elapsed;
            }
            if (distance(target.position, position) > range) {
                secondsSinceShot = 0;
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
            bullets[i].Render(program, matrix);
        }
    }
}

