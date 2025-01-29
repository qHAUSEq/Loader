#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <future>

class Loader {
private:
    Loader(sf::RenderWindow& appWindow);
    ~Loader();
    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;

    void run();
    void update(float deltaTime);   // Update animation
    void render();                  // Render loading animation
    void startLoading();            // Start loading animation
    void stopLoading();             // Stop loading animation

public:
    static Loader& getInstance(sf::RenderWindow& appWindow);                // Get the singleton instance, ensuring it has a reference to sf::RenderWindow
    static Loader& getInstance();                                           // Overloaded version of getInstance without parameters, assumes window is already set

    void setLoaderScreenImage(const std::string& path);
    void setLoaderScreenImage(const void* data, size_t size, const sf::IntRect& area = sf::IntRect());
    void setLoaderScreenText(const std::string& text, const sf::Font& font, const sf::Vector2f& position, unsigned int characterSize = 24, sf::Color textColor = sf::Color::White, sf::Color outlineColor = sf::Color::Black, float outlineThickness = 0.0f);

    static void setResolution(unsigned int height = sf::VideoMode::getDesktopMode().height, unsigned int width = sf::VideoMode::getDesktopMode().width);
    static void setCheckpoint(const std::string& label);                    // Static method to set a checkpoint (start loading screen)
    static void clearCheckpoint(const std::string& label);                  // Static method to stop loading when tasks are done

    // Work in progress
    //template <typename Func>
    //std::future<void> performHeavyTask(Func task)  // The performHeavyTask method that returns std::future
    //{
    //    return std::async(std::launch::async, [this, task] {
    //        startLoading();
    //        task();
    //        stopLoading();
    //        });
    //}

private:
    sf::RenderWindow& window;
    std::thread worker;
    std::mutex mutex_;
    //std::mutex setActiveMutex; // A mutex for managing SetActive
    std::condition_variable cv_;
    std::atomic<bool> running;
    std::atomic<bool> loading;
    std::unordered_map<std::string, bool> checkpoints;
    static inline Loader* instance = nullptr;
    static inline bool instanceSet = false;

    std::vector<sf::CircleShape> segments;
    float angle;
    sf::Text loading_text;
    sf::Texture loading_background_texture;
    sf::Sprite loading_background_sprite;

    inline static unsigned int resolution_height = sf::VideoMode::getDesktopMode().height;
    inline static unsigned int resolution_width = sf::VideoMode::getDesktopMode().width;
};