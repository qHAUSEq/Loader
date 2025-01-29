#include "Loader.h"

Loader::Loader(sf::RenderWindow& appWindow) :
    window(appWindow), running(true), loading(false), angle(0.0f)
{
    instance = this;
    instanceSet = true;

    // Initialize segments for the loading indicator
    int segmentCount = 8; // Number of segments
    float radius = 50.0f;  // Radius of the loading circle
    float segmentRadius = 5.0f; // Radius of each segment

    for (int i = 0; i < segmentCount; ++i) {
        sf::CircleShape segment(segmentRadius);
        segment.setOrigin(segmentRadius, segmentRadius);
        segment.setFillColor(sf::Color(255, 255, 255, 100 + 155 * (i % 2))); // Alternate transparency
        segment.setOutlineColor(sf::Color::Black);
        segment.setOutlineThickness(2);

        // Calculate initial position of each segment around the circle
        float angle = i * 2 * 3.14159f / segmentCount;
        float x = std::cos(angle) * radius + resolution_width / 2.0f;
        float y = std::sin(angle) * radius + resolution_height / 2.0f;
        segment.setPosition(x, y);

        segments.push_back(segment);
    }

    // Start the worker thread for render and update
    worker = std::thread(&Loader::run, this);
}
Loader::~Loader() {
    running = false;
    if (worker.joinable()) {
        worker.join();
    }
}

void Loader::run() {
    sf::Clock clock;

    while (running) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return loading || !running; });

        if (!running) break;

        if (loading) {
            window.setActive(true); // Activate window in loader thread
            float deltaTime = clock.restart().asSeconds();
            update(deltaTime);
            render();
            window.setActive(false); // Deactivate after rendering
        }
    }
}

void Loader::update(float deltaTime) {
    angle += 90.0f * deltaTime;
    if (angle >= 360.0f) angle -= 360.0f;

    float rotationAngle = angle * 3.14159f / 180.0f;
    float radius = 30.0f;
    size_t segmentCount = segments.size();

    for (int i = 0; i < segmentCount; ++i) {
        float currentAngle = i * 2 * 3.14159f / segmentCount + rotationAngle;
        float x = std::cos(currentAngle) * radius + window.getSize().x - 100;
        float y = std::sin(currentAngle) * radius + window.getSize().y - 100;
        segments[i].setPosition(x, y);
    }
}
void Loader::render() {
    window.clear();
    // Draw each segment of the circle
    window.draw(loading_background_sprite);
    for (const auto& segment : segments) {
        window.draw(segment);
    }
    window.draw(loading_text);
    window.display();
}

void Loader::startLoading() {
    window.setActive(false);  // Deactivate in main thread
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loading = true;
    }
    cv_.notify_all();
}
void Loader::stopLoading()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loading = false;
    }
    cv_.notify_all();
    window.setActive(true);  // Reactivate in main thread
}

Loader& Loader::getInstance(sf::RenderWindow& appWindow) {
    static Loader instance(appWindow);
    return instance;
}
Loader& Loader::getInstance() {
    if (!instanceSet) {
        Logger::getInstance().log("Loader instance is not initialized with sf::RenderWindow.", LogLevel::Error);
    }
    return *instance;
}

void Loader::setLoaderScreenImage(const std::string& path) {
    if (!loading_background_texture.loadFromFile(path)) {
        Logger::getInstance().log("[LOADER] Texture loading error", LogLevel::Error);
    }
    else {
        loading_background_sprite.setTexture(loading_background_texture);
        loading_background_sprite.scale(
            resolution_width / loading_background_sprite.getLocalBounds().width,
            resolution_height / loading_background_sprite.getLocalBounds().height
        );
    }
}
void Loader::setLoaderScreenImage(const void* data, size_t size, const sf::IntRect& area) {
    if (!loading_background_texture.loadFromMemory(data, size, area)) {
        Logger::getInstance().log("[LOADER] Texture loading error", LogLevel::Error);
    }
    else {
        loading_background_sprite.setTexture(loading_background_texture);
        loading_background_sprite.scale(
            resolution_width / loading_background_sprite.getLocalBounds().width,
            resolution_height / loading_background_sprite.getLocalBounds().height
        );
    }
}
void Loader::setLoaderScreenText(const std::string& text, const sf::Font& font, const sf::Vector2f& position, unsigned int characterSize, sf::Color textColor, sf::Color outlineColor, float outlineThickness) {
    loading_text.setFont(font);
    loading_text.setString(text);
    loading_text.setPosition(position);
    loading_text.setCharacterSize(characterSize);
    loading_text.setFillColor(textColor);
    loading_text.setOutlineColor(outlineColor);
    loading_text.setOutlineThickness(outlineThickness);
}

void Loader::setResolution(unsigned int height, unsigned int width) {
    resolution_height = height;
    resolution_width = width;
}
void Loader::setCheckpoint(const std::string& label) {
    Loader& loader = getInstance();
    {
        std::lock_guard<std::mutex> lock(loader.mutex_);
        loader.checkpoints[label] = true;
    }
    loader.startLoading();
}

void Loader::clearCheckpoint(const std::string& label) {
    Loader& loader = getInstance();
    {
        std::lock_guard<std::mutex> lock(loader.mutex_);
        loader.checkpoints.erase(label);
    }
    loader.stopLoading();
}