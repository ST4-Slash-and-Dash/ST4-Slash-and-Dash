#include "Player.h"
#include <iostream>

Player::Player(unsigned int joystickId)
    : joystickId(joystickId)
    , speed(100)
    , is_walking(false)
    , is_alive(true)
    , is_attacking(false)
    , sword_position(1)
    , wPressed(false)
    , sPressed(false)
    , facing_right(joystickId == 0) // Player 1 faces right by default
{
    initializeTextures();
    initializeSprite();
    initializeColliders();
    setupAnimations();
}

void Player::initializeTextures() {
    // Load initial texture based on player ID
    std::string basePath = (joystickId == 0) ?
        "assets/Texture/walking2/animation-walking2.1.png" :
        "assets/Texture/walking1/animation-walking1.1.png";

    tx.loadFromFile(basePath);
}

void Player::initializeSprite() {
    sp.setTexture(tx);
    sp.setScale(0.8f, 0.8f);

    sf::FloatRect bounds = sp.getLocalBounds();
    sp.setOrigin(bounds.width / 2.0f, 0);

    if (joystickId == 0) {
        sp.setPosition(50, 140);
    }
    else {
        sp.setScale(-0.8f, 0.8f);
    }
}

void Player::initializeColliders() {
    // Set hitbox size based on player direction
    float hitboxWidth = facing_right ? 150.0f : -150.0f;
    float attackWidth = facing_right ? 100.0f : -100.0f;

    hitbox.setSize(sf::Vector2f(hitboxWidth, 300.0f));
    attack_range.setSize(sf::Vector2f(attackWidth, 302.0f));

    // Debug visualization - this should be optional in a production game
    hitbox.setFillColor(sf::Color(255, 0, 0, 100));
    hitbox.setOutlineColor(sf::Color::Red);
    hitbox.setOutlineThickness(1);

    attack_range.setFillColor(sf::Color(0, 255, 0, 100));
    attack_range.setOutlineColor(sf::Color::Green);
    attack_range.setOutlineThickness(1);
}

void Player::load_animations() {
    // Load textures for different animations
    loadTextureSet(walkingTextures1, "assets/Texture/walking1/animation-walking1.", 1, 21, ".png"); // 7 f�r oben, f�r unten und f�r oben 
    loadTextureSet(walkingTextures2, "assets/Texture/walking2/animation-walking2.", 1, 21, ".png");// 7 f�r oben, f�r unten und f�r oben 

    loadTextureSet(attackTextures1, "assets/Texture/sword-swing1/animation-sword-swing1.", 1, 9, ".png");
    loadTextureSet(attackTextures2, "assets/Texture/sword-swing2/animation-sword-swing1.", 1, 9, ".png");

    loadTextureSet(dyingTextures1, "assets/Texture/dying1/dying1.", 1, 8, ".png");
    loadTextureSet(dyingTextures2, "assets/Texture/dying2/dying2.", 1, 8, ".png");
}

void Player::loadTextureSet(std::vector<sf::Texture>& textureVec, const std::string& basePath,
    int startIdx, int endIdx, const std::string& suffix) {
    textureVec.reserve(endIdx - startIdx + 1);
    for (int i = startIdx; i <= endIdx; ++i) {
        sf::Texture tex;
        tex.loadFromFile(basePath + std::to_string(i) + suffix);
        textureVec.push_back(tex);
    }
}

void Player::setupAnimations() {
    load_animations();

    // Setup animation references based on player ID
    setupAnimationRefs(attackRefs, joystickId == 0 ? attackTextures2 : attackTextures1);
    setupAnimationRefs(swordRefs, joystickId == 0 ? swordPullingTextures2 : swordPullingTextures1);
    setupAnimationRefs(dyingRefs, joystickId == 0 ? dyingTextures2 : dyingTextures1);

    // Initialisiere drei Animation Player f�r die verschiedenen Schwertpositionen
    walkingAnimations.resize(3);
    for (int pos = 0; pos < 3; pos++) {
        std::vector<sf::Texture*> posRefs;
        std::vector<sf::Texture>& currentTextures = joystickId == 0 ? walkingTextures2 : walkingTextures1;

        int startFrame = pos * 7; // 0, 7 oder 14 je nach Position
        for (int i = 0; i < 7; i++) {
            int frameIndex = startFrame + i;
            if (frameIndex < currentTextures.size()) {
                posRefs.push_back(&currentTextures[frameIndex]);
            }
        }

        walkingAnimations[pos] = new Animation_Player(&sp, posRefs, 0.2f);
    }

    // Initialisiere den aktuellen walkingAnimation mit der Startposition (sword_position = 1)
    walkingAnimation = walkingAnimations[sword_position];

    // Create other animation controllers
    attackAnimation = new Animation_Player(&sp, attackRefs, 0.07f);
    swordPullingAnimation = new Animation_Player(&sp, swordRefs, 0.2f);
    dyingAnimation = new Animation_Player(&sp, dyingRefs, 0.2f);
}

void Player::setupAnimationRefs(std::vector<sf::Texture*>& refs, std::vector<sf::Texture>& textures) {
    refs.reserve(textures.size());
    for (auto& tex : textures) {
        refs.push_back(&tex);
    }
}

void Player::update(sf::Time deltaTime) {
    if (!is_alive) {
        updateAnimations();
    }

    move(deltaTime);
    updateColliders();
    updateAnimations();
}

void Player::updateColliders() {
    sf::Vector2f spriteCenter = sp.getPosition();

    hitbox.setPosition(spriteCenter.x - hitbox.getSize().x / 2.0f, 0);

    float offsetX = hitbox.getSize().x * 0.87f;
    attack_range.setPosition(hitbox.getPosition().x + offsetX, 0);
}

void Player::updateAnimations() {
    if (is_alive) {
        if (is_attacking) {
            attackAnimation->update();
            if (attackAnimation->isFinished()) {
                // Wenn die Angriffs-Animation beendet ist, setze den ersten Frame der Walking-Animation
                walkingAnimation = walkingAnimations[sword_position];
                walkingAnimation->setToFrame(0);  // Verwende die neue Methode
                is_attacking = false;
            }
        }
        else if (is_walking) {
            // Stelle sicher, dass wir die richtige Animation f�r die aktuelle Schwertposition verwenden
            walkingAnimation = walkingAnimations[sword_position];

            if (!walkingAnimation->isPlaying()) {
                walkingAnimation->play(true);
            }
            walkingAnimation->update();
        }
        else {
            // Bei Stillstand die Animation anhalten, aber den Sprite im ersten Frame belassen
            walkingAnimation->stop();
            // Stelle sicher, dass walkingAnimation zur aktuellen Schwertposition passt
            walkingAnimation = walkingAnimations[sword_position];
            // Setze den ersten Frame der aktuellen Walking-Animation
            walkingAnimation->setToFrame(0);  // Verwende die neue Methode
        }

        if (swordPullingAnimation->isPlaying()) {
            swordPullingAnimation->update();
        }
    }
    else {
        if (dyingAnimation->isFinished()) sp.setPosition(sf::Vector2f(10000, 10000));
        if (dyingAnimation->isPlaying()) dyingAnimation->update();
    }
}


sf::Vector2f Player::get_Position() {
    return sp.getPosition();
}

float Player::get_PositionX() {
    return sp.getPosition().x;
}

float Player::get_PositionY() {
    return sp.getPosition().y;
}

int Player::getJoystickID() {
    return joystickId;
}

void Player::start_animation(int index) {
    switch (index) {
    case 0:
        swordPullingAnimation->play(false);
        break;
    case 1:
        break;
    case 2:
        walkingAnimation->play(true);
        break;
    case 3:
        dyingAnimation->play(false);
        break;
    default:
        break;
    }
}

void Player::move(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();
    float movementX = 0.0f;

    movementX = handleInput(dt);

    is_walking = (movementX != 0);
    sp.move(movementX, 0);
}

float Player::handleInput(float deltaTime) {
    float movementX = 0.0f;

    // Joystick input handling
    if (sf::Joystick::isConnected(joystickId)) {
        float axisX = sf::Joystick::getAxisPosition(joystickId, sf::Joystick::X);
        if (axisX > 15) {
            movementX = speed * deltaTime;
            setFacingDirection(true);
        }
        else if (axisX < -15) {
            movementX = speed * -deltaTime;
            setFacingDirection(false);
        }

        // Controller-Steuerung f�r Schwertposition
        float axisY = sf::Joystick::getAxisPosition(joystickId, sf::Joystick::Y);
        // Nach oben
        if (axisY < -20 && sword_position != 2 && !wPressed) {
            sword_position++;
            wPressed = true;
            std::cout << "sword position: " << sword_position << std::endl;
        }
        else if (axisY >= -20) {
            wPressed = false;
        }

        // Nach unten
        if (axisY > 20 && sword_position != 0 && !sPressed) {
            sword_position--;
            sPressed = true;
            std::cout << "sword position: " << sword_position << std::endl;
        }
        else if (axisY <= 20) {
            sPressed = false;
        }
    }
    // Keyboard input for player 1
    else if (joystickId == 0) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && (sp.getPosition().x + 20) < 960) {
            movementX = speed * deltaTime;
            setFacingDirection(true);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sp.getPosition().x > 0) {
            movementX = speed * -deltaTime;
            setFacingDirection(false);
        }
        // sword pos
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sword_position != 2 && !wPressed) {
            sword_position++;
            wPressed = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            wPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sword_position != 0 && !sPressed) {
            sword_position--;
            sPressed = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            sPressed = false;
        }
    }
    // Keyboard input for player 2
    else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) && (sp.getPosition().x + 20) < 960) {
            movementX = speed * deltaTime;
            setFacingDirection(true);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H) && sp.getPosition().x > 0) {
            movementX = speed * -deltaTime;
            setFacingDirection(false);
        }
        // sword pos
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U) && sword_position != 2 && !wPressed) {
            sword_position++;
            wPressed = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
            wPressed = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && sword_position != 0 && !sPressed) {
            sword_position--;
            sPressed = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
            sPressed = false;
        }
    }

    return movementX;
}

void Player::setFacingDirection(bool faceRight) {
    facing_right = faceRight;

    // Update sprite scale and collider sizes based on direction
    if (faceRight) {
        sp.setScale(0.8f, 0.8f);
        hitbox.setSize(sf::Vector2f(150.0f, 300.0f));
        attack_range.setSize(sf::Vector2f(100.0f, 350.0f));
    }
    else {
        sp.setScale(-0.8f, 0.8f);
        hitbox.setSize(sf::Vector2f(-150.0f, 300.0f));
        attack_range.setSize(sf::Vector2f(-100.0f, 350.0f));
    }
}

void Player::attack() {
    if (is_alive && !is_attacking) {
        attackAnimation->play(false);
        is_attacking = true;
    }
}

void Player::die() {
    if (is_alive) {
        std::cout << "Player " << joystickId << " died!" << std::endl;
        is_alive = false;
        start_animation(3);
    }
}

void Player::setPosition(int x, int y) {
    sp.setPosition(x, y);
}

void Player::render(sf::RenderWindow* target) {
    target->draw(sp);

    target->draw(hitbox);
    target->draw(attack_range);
}

bool Player::is_dying_animation_finished() {
    return !is_alive && dyingAnimation->isFinished();
}

Player::~Player() {
    for (auto anim : walkingAnimations) {
        delete anim;
    }
    delete swordPullingAnimation;
    delete attackAnimation;
    delete dyingAnimation;
}
