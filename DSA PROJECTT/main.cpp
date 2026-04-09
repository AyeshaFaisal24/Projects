#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;
using namespace sf;

struct SongNode {
    string song;
    string artist;
    string duration;
    SongNode* next;
    SongNode* prev;
    SongNode(const string& s, const string& a = "Unknown Artist", const string& d = "0:00") 
        : song(s), artist(a), duration(d), next(NULL), prev(NULL) {}
};

// Global variables
SongNode* playlistHead = NULL;
SongNode* currentSongNode = NULL;
Music music;
bool isPlaying = false;
bool isPaused = false;
bool shuffleMode = false;
bool repeatMode = false;
float visualizerData[64];
Clock visualizerClock;
vector<string> recentSongs;
Font font;
Texture backgroundTexture;
Texture buttonTexture;
Texture musicIconTexture;

// Colors
Color primaryColor(70, 130, 180);    // Steel Blue
Color secondaryColor(30, 30, 50);    // Dark Blue
Color accentColor(255, 105, 180);    // Hot Pink
Color textColor(255, 255, 255);      // White

// Function declarations
void initializeGUI(RenderWindow& window);
void loadFont();
void loadTextures();
void addSongToPlaylist(const string& songName, const string& artist = "Unknown Artist", const string& duration = "0:00");
void loadPlaylistFromFile();
void savePlaylistToFile();
void playSelectedSong();
void drawGUI(RenderWindow& window);
void handleEvents(RenderWindow& window);
void cleanup();
void updateVisualizer();
void drawVisualizer(RenderWindow& window);
void drawProgressBar(RenderWindow& window);
string formatTime(float seconds);
void createSamplePlaylist();

// GUI Elements
RectangleShape background;
RectangleShape headerPanel;
RectangleShape controlPanel;
RectangleShape playlistPanel;
RectangleShape visualizerPanel;
vector<Text> playlistTexts;
vector<Text> controlTexts;
vector<RectangleShape> controlButtons;
Text titleText;
Text statusText;
Text currentSongText;
Text timeText;
Text volumeText;

// Button states
bool playButtonHover = false;
bool pauseButtonHover = false;
bool stopButtonHover = false;
bool nextButtonHover = false;
bool prevButtonHover = false;
bool shuffleButtonHover = false;
bool repeatButtonHover = false;

int selectedSongIndex = -1;

void loadFont() {
    if (!font.loadFromFile("arial.ttf")) {
        cout << "Error loading font! Using default." << endl;
    }
}

void loadTextures() {
    if (!backgroundTexture.loadFromFile("background.jpg")) {
        // Create gradient background
        background.setFillColor(secondaryColor);
    } else {
        background.setTexture(&backgroundTexture);
    }
}

void createSamplePlaylist() {
    // Add some sample songs if playlist is empty
    addSongToPlaylist("Morning Bliss", "Chill Vibes", "3:45");
    addSongToPlaylist("Electric Dreams", "Synth Wave", "4:20");
    addSongToPlaylist("Ocean Waves", "Nature Sounds", "2:30");
    addSongToPlaylist("City Lights", "Urban Beats", "3:15");
    savePlaylistToFile();
}

void initializeGUI(RenderWindow& window) {
    loadFont();
    loadTextures();
    
    // Setup background with gradient effect
    background.setSize(Vector2f(1000, 700));
    
    // Setup header panel
    headerPanel.setSize(Vector2f(980, 80));
    headerPanel.setPosition(10, 10);
    headerPanel.setFillColor(Color(primaryColor.r, primaryColor.g, primaryColor.b, 180));
    headerPanel.setOutlineThickness(2);
    headerPanel.setOutlineColor(accentColor);
    
    // Setup title with shadow effect
    titleText.setFont(font);
    titleText.setString("Music Player");
    titleText.setCharacterSize(36);
    titleText.setFillColor(textColor);
    titleText.setStyle(Text::Bold);
    titleText.setPosition(350, 25);
    
    // Setup status text
    statusText.setFont(font);
    statusText.setString("Ready to play music");
    statusText.setCharacterSize(16);
    statusText.setFillColor(Color::Green);
    statusText.setPosition(50, 100);
    
    // Setup current song text
    currentSongText.setFont(font);
    currentSongText.setString("Now Playing: Select a song to begin");
    currentSongText.setCharacterSize(18);
    currentSongText.setFillColor(accentColor);
    currentSongText.setPosition(50, 130);
    
    // Setup time text
    timeText.setFont(font);
    timeText.setString("0:00 / 0:00");
    timeText.setCharacterSize(14);
    timeText.setFillColor(textColor);
    timeText.setPosition(850, 130);
    
    // Setup playlist panel
    playlistPanel.setSize(Vector2f(680, 350));
    playlistPanel.setPosition(10, 170);
    playlistPanel.setFillColor(Color(40, 40, 60, 200));
    playlistPanel.setOutlineThickness(2);
    playlistPanel.setOutlineColor(primaryColor);
    
    // Setup visualizer panel
    visualizerPanel.setSize(Vector2f(280, 150));
    visualizerPanel.setPosition(700, 170);
    visualizerPanel.setFillColor(Color(30, 30, 40, 180));
    visualizerPanel.setOutlineThickness(2);
    visualizerPanel.setOutlineColor(accentColor);
    
    // Setup control panel
    controlPanel.setSize(Vector2f(980, 120));
    controlPanel.setPosition(10, 540);
    controlPanel.setFillColor(Color(50, 50, 70, 200));
    controlPanel.setOutlineThickness(2);
    controlPanel.setOutlineColor(primaryColor);
    
    // Initialize control buttons
    string controlLabels[] = {
        "Play", "Pause", "Stop", "Next", "Prev", 
        "Shuffle", "Repeat", "Vol+", "Vol-", "Add Song"
    };
    
    for (int i = 0; i < 10; ++i) {
        RectangleShape button(Vector2f(80, 30));
        button.setPosition(50 + (i % 5) * 190, 560 + (i / 5) * 40);
        button.setFillColor(primaryColor);
        button.setOutlineThickness(1);
        button.setOutlineColor(accentColor);
        controlButtons.push_back(button);
        
        Text text;
        text.setFont(font);
        text.setString(controlLabels[i]);
        text.setCharacterSize(12);
        text.setFillColor(textColor);
        text.setPosition(55 + (i % 5) * 190, 565 + (i / 5) * 40);
        controlTexts.push_back(text);
    }
    
    // Initialize visualizer data
    for (int i = 0; i < 64; ++i) {
        visualizerData[i] = 0.0f;
    }
}

void addSongToPlaylist(const string& songName, const string& artist, const string& duration) {
    SongNode* newNode = new SongNode(songName, artist, duration);
    
    if (!playlistHead) {
        playlistHead = newNode;
        currentSongNode = playlistHead;
    } else {
        SongNode* temp = playlistHead;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
    
    // Add to GUI list
    Text songText;
    songText.setFont(font);
    
    stringstream songInfo;
    songInfo << playlistTexts.size() + 1 << ". " << songName << " - " << artist << " [" << duration << "]";
    songText.setString(songInfo.str());
    
    songText.setCharacterSize(14);
    songText.setFillColor(textColor);
    songText.setPosition(30, 185 + (int)playlistTexts.size() * 25);
    playlistTexts.push_back(songText);
}

void loadPlaylistFromFile() {
    ifstream file("playlist.txt");
    if (!file.is_open()) {
        cout << "No playlist found. Creating sample playlist..." << endl;
        createSamplePlaylist();
        return;
    }
    
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            // Simple parsing - in real app you'd want better format
            addSongToPlaylist(line, "Unknown Artist", "0:00");
        }
    }
    file.close();
}

void savePlaylistToFile() {
    ofstream file("playlist.txt");
    SongNode* temp = playlistHead;
    
    while (temp) {
        file << temp->song << endl;
        temp = temp->next;
    }
    file.close();
}

void updateVisualizer() {
    if (isPlaying && !isPaused) {
        float time = visualizerClock.getElapsedTime().asSeconds();
        for (int i = 0; i < 64; ++i) {
            // Create moving wave pattern
            visualizerData[i] = (sin(time * 2.0f + i * 0.3f) + 1.0f) * 0.5f * 50.0f;
            visualizerData[i] += (cos(time * 1.5f + i * 0.2f) + 1.0f) * 0.3f * 30.0f;
        }
    } else {
        // Fade out effect when not playing
        for (int i = 0; i < 64; ++i) {
            visualizerData[i] *= 0.95f;
        }
    }
}

void drawVisualizer(RenderWindow& window) {
    float barWidth = visualizerPanel.getSize().x / 64.0f;
    
    for (int i = 0; i < 64; ++i) {
        RectangleShape bar(Vector2f(barWidth - 1, visualizerData[i]));
        bar.setPosition(visualizerPanel.getPosition().x + i * barWidth, 
                       visualizerPanel.getPosition().y + visualizerPanel.getSize().y - visualizerData[i]);
        
        // Color gradient from blue to pink
        float ratio = visualizerData[i] / 80.0f;
        Color barColor(primaryColor.r * (1 - ratio) + accentColor.r * ratio,
                      primaryColor.g * (1 - ratio) + accentColor.g * ratio,
                      primaryColor.b * (1 - ratio) + accentColor.b * ratio);
        
        bar.setFillColor(barColor);
        window.draw(bar);
    }
}

void drawProgressBar(RenderWindow& window) {
    if (isPlaying && music.getDuration().asSeconds() > 0) {
        float progress = music.getPlayingOffset().asSeconds() / music.getDuration().asSeconds();
        float barWidth = 600.0f;
        
        // Background bar
        RectangleShape backgroundBar(Vector2f(barWidth, 8));
        backgroundBar.setPosition(200, 530);
        backgroundBar.setFillColor(Color(100, 100, 100, 150));
        window.draw(backgroundBar);
        
        // Progress bar
        RectangleShape progressBar(Vector2f(barWidth * progress, 8));
        progressBar.setPosition(200, 530);
        progressBar.setFillColor(accentColor);
        window.draw(progressBar);
        
        // Update time text
        stringstream timeStream;
        timeStream << formatTime(music.getPlayingOffset().asSeconds()) << " / " 
                  << formatTime(music.getDuration().asSeconds());
        timeText.setString(timeStream.str());
    }
}

string formatTime(float seconds) {
    int minutes = (int)seconds / 60;
    int secs = (int)seconds % 60;
    stringstream timeStream;
    timeStream << minutes << ":" << (secs < 10 ? "0" : "") << secs;
    return timeStream.str();
}

void playSelectedSong() {
    if (selectedSongIndex < 0 || selectedSongIndex >= (int)playlistTexts.size()) return;
    
    SongNode* temp = playlistHead;
    for (int i = 0; i < selectedSongIndex && temp; i++) {
        temp = temp->next;
    }
    
    if (temp) {
        currentSongNode = temp;
        
        if (isPlaying) {
            music.stop();
        }
        
        string extensionsArray[] = {".wav", ".ogg", ".flac", ".aiff", ".au"};
        vector<string> extensions(extensionsArray, extensionsArray + sizeof(extensionsArray) / sizeof(extensionsArray[0]));
        bool loaded = false;
        
        for (int i = 0; i < (int)extensions.size(); ++i) {
            string fullPath = temp->song + extensions[i];
            if (music.openFromFile(fullPath)) {
                loaded = true;
                music.play();
                isPlaying = true;
                isPaused = false;
                visualizerClock.restart();
                
                statusText.setString("Status: Playing");
                currentSongText.setString("Now Playing: " + temp->song + " - " + temp->artist);
                
                recentSongs.push_back(temp->song);
                if ((int)recentSongs.size() > 5) {
                    recentSongs.erase(recentSongs.begin());
                }
                break;
            }
        }
        
        if (!loaded) {
            statusText.setString("Error: Could not load audio file");
            statusText.setFillColor(Color::Red);
        } else {
            statusText.setFillColor(Color::Green);
        }
    }
}

void playNextSong() {
    if (!currentSongNode) return;
    
    if (shuffleMode) {
        // Random song selection
        int totalSongs = 0;
        SongNode* temp = playlistHead;
        while (temp) {
            totalSongs++;
            temp = temp->next;
        }
        
        if (totalSongs > 0) {
            int randomIndex = rand() % totalSongs;
            temp = playlistHead;
            for (int i = 0; i < randomIndex && temp; i++) {
                temp = temp->next;
            }
            currentSongNode = temp;
            selectedSongIndex = randomIndex;
        }
    } else {
        // Normal next song
        if (currentSongNode->next) {
            currentSongNode = currentSongNode->next;
            selectedSongIndex++;
        } else if (repeatMode) {
            // Loop to beginning
            currentSongNode = playlistHead;
            selectedSongIndex = 0;
        } else {
            return; // No next song
        }
    }
    
    playSelectedSong();
}

void playPreviousSong() {
    if (!currentSongNode || !currentSongNode->prev) return;
    
    currentSongNode = currentSongNode->prev;
    selectedSongIndex--;
    playSelectedSong();
}

void drawGUI(RenderWindow& window) {
    window.clear();
    
    // Draw background
    window.draw(background);
    
    // Draw panels
    window.draw(headerPanel);
    window.draw(playlistPanel);
    window.draw(visualizerPanel);
    window.draw(controlPanel);
    
    // Draw texts
    window.draw(titleText);
    window.draw(statusText);
    window.draw(currentSongText);
    window.draw(timeText);
    
    // Draw control buttons with hover effects
    for (int i = 0; i < (int)controlButtons.size(); ++i) {
        // Update button colors based on state
        if ((i == 0 && playButtonHover) || (i == 1 && pauseButtonHover) || 
            (i == 2 && stopButtonHover) || (i == 3 && nextButtonHover) ||
            (i == 4 && prevButtonHover) || (i == 5 && shuffleButtonHover) ||
            (i == 6 && repeatButtonHover)) {
            controlButtons[i].setFillColor(accentColor);
        } else if ((i == 5 && shuffleMode) || (i == 6 && repeatMode)) {
            controlButtons[i].setFillColor(Color::Yellow);
        } else {
            controlButtons[i].setFillColor(primaryColor);
        }
        
        window.draw(controlButtons[i]);
        window.draw(controlTexts[i]);
    }
    
    // Draw playlist songs with selection highlight
    for (int i = 0; i < (int)playlistTexts.size(); ++i) {
        if (i == selectedSongIndex) {
            // Highlight selected song
            RectangleShape highlight(Vector2f(660, 22));
            highlight.setPosition(15, 180 + i * 25);
            highlight.setFillColor(Color(primaryColor.r, primaryColor.g, primaryColor.b, 100));
            window.draw(highlight);
            
            playlistTexts[i].setFillColor(accentColor);
            playlistTexts[i].setStyle(Text::Bold);
        } else {
            playlistTexts[i].setFillColor(textColor);
            playlistTexts[i].setStyle(Text::Regular);
        }
        window.draw(playlistTexts[i]);
    }
    
    // Draw mode indicators
    Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(12);
    modeText.setFillColor(textColor);
    
    if (shuffleMode) {
        modeText.setString("SHUFFLE: ON");
        modeText.setPosition(700, 330);
        window.draw(modeText);
    }
    
    if (repeatMode) {
        modeText.setString("REPEAT: ON");
        modeText.setPosition(800, 330);
        window.draw(modeText);
    }
    
    // Draw visualizer and progress bar
    updateVisualizer();
    drawVisualizer(window);
    drawProgressBar(window);
    
    // Draw volume info
    stringstream volumeStream;
    volumeStream << "Volume: " << (int)music.getVolume() << "%";
    Text volumeDisplay;
    volumeDisplay.setFont(font);
    volumeDisplay.setString(volumeStream.str());
    volumeDisplay.setCharacterSize(14);
    volumeDisplay.setFillColor(textColor);
    volumeDisplay.setPosition(800, 530);
    window.draw(volumeDisplay);
    
    window.display();
}

void handleEvents(RenderWindow& window) {
    Event event;
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    
    // Reset button hover states
    playButtonHover = pauseButtonHover = stopButtonHover = false;
    nextButtonHover = prevButtonHover = shuffleButtonHover = repeatButtonHover = false;
    
    // Check button hover
    for (int i = 0; i < (int)controlButtons.size(); ++i) {
        if (controlButtons[i].getGlobalBounds().contains(mousePos)) {
            switch(i) {
                case 0: playButtonHover = true; break;
                case 1: pauseButtonHover = true; break;
                case 2: stopButtonHover = true; break;
                case 3: nextButtonHover = true; break;
                case 4: prevButtonHover = true; break;
                case 5: shuffleButtonHover = true; break;
                case 6: repeatButtonHover = true; break;
            }
        }
    }
    
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        }
        
        if (event.type == Event::MouseButtonPressed) {
            if (event.mouseButton.button == Mouse::Left) {
                // Check control button clicks
                for (int i = 0; i < (int)controlButtons.size(); ++i) {
                    if (controlButtons[i].getGlobalBounds().contains(mousePos)) {
                        switch(i) {
                            case 0: // Play
                                if (selectedSongIndex >= 0) playSelectedSong();
                                break;
                            case 1: // Pause
                                if (isPlaying) {
                                    if (isPaused) {
                                        music.play();
                                        isPaused = false;
                                        statusText.setString("Status: Playing");
                                    } else {
                                        music.pause();
                                        isPaused = true;
                                        statusText.setString("Status: Paused");
                                    }
                                }
                                break;
                            case 2: // Stop
                                music.stop();
                                isPlaying = false;
                                isPaused = false;
                                statusText.setString("Status: Stopped");
                                break;
                            case 3: // Next
                                playNextSong();
                                break;
                            case 4: // Previous
                                playPreviousSong();
                                break;
                            case 5: // Shuffle
                                shuffleMode = !shuffleMode;
                                break;
                            case 6: // Repeat
                                repeatMode = !repeatMode;
                                break;
                            case 7: // Volume Up
                                {
                                    float newVolume = music.getVolume() + 10;
                                    if (newVolume > 100) newVolume = 100;
                                    music.setVolume(newVolume);
                                }
                                break;
                            case 8: // Volume Down
                                {
                                    float newVolume = music.getVolume() - 10;
                                    if (newVolume < 0) newVolume = 0;
                                    music.setVolume(newVolume);
                                }
                                break;
                            case 9: // Add Song
                                {
                                    string newSong;
                                    cout << "Enter song name (without extension): ";
                                    cin >> newSong;
                                    addSongToPlaylist(newSong);
                                    savePlaylistToFile();
                                }
                                break;
                        }
                    }
                }
                
                // Check playlist item clicks
                for (int i = 0; i < (int)playlistTexts.size(); ++i) {
                    FloatRect textBounds = playlistTexts[i].getGlobalBounds();
                    if (textBounds.contains(mousePos)) {
                        selectedSongIndex = i;
                        break;
                    }
                }
            }
        }
        
        if (event.type == Event::KeyPressed) {
            switch (event.key.code) {
                case Keyboard::P:
                    if (selectedSongIndex >= 0) playSelectedSong();
                    break;
                case Keyboard::Space:
                    if (isPlaying) {
                        if (isPaused) {
                            music.play();
                            isPaused = false;
                            statusText.setString("Status: Playing");
                        } else {
                            music.pause();
                            isPaused = true;
                            statusText.setString("Status: Paused");
                        }
                    }
                    break;
                case Keyboard::S:
                    music.stop();
                    isPlaying = false;
                    isPaused = false;
                    statusText.setString("Status: Stopped");
                    break;
                case Keyboard::N:
                    playNextSong();
                    break;
                case Keyboard::B:
                    playPreviousSong();
                    break;
                case Keyboard::H:
                    shuffleMode = !shuffleMode;
                    break;
                case Keyboard::R:
                    repeatMode = !repeatMode;
                    break;
                case Keyboard::Add:
                case Keyboard::Equal:
                    {
                        float newVolume = music.getVolume() + 10;
                        if (newVolume > 100) newVolume = 100;
                        music.setVolume(newVolume);
                    }
                    break;
                case Keyboard::Subtract:
                case Keyboard::Dash:
                    {
                        float newVolume = music.getVolume() - 10;
                        if (newVolume < 0) newVolume = 0;
                        music.setVolume(newVolume);
                    }
                    break;
            }
        }
        
        // Auto-play next song when current finishes
        if (isPlaying && music.getStatus() == Music::Stopped && !isPaused) {
            if (repeatMode) {
                playSelectedSong(); // Repeat current song
            } else {
                playNextSong(); // Play next song
            }
        }
    }
}

void cleanup() {
    SongNode* temp = playlistHead;
    while (temp) {
        SongNode* next = temp->next;
        delete temp;
        temp = next;
    }
    playlistHead = NULL;
}

int main() {
    srand(time(NULL)); // For shuffle mode
    
    RenderWindow window(VideoMode(1000, 700), "Music Player");
    window.setFramerateLimit(60);
    
    initializeGUI(window);
    loadPlaylistFromFile();
    music.setVolume(50);
    
    cout << "=== Music Player ===" << endl;
    cout << "Mouse Controls:" << endl;
    cout << "- Click songs in playlist to select" << endl;
    cout << "- Use buttons for playback control" << endl;
    cout << "Keyboard Shortcuts:" << endl;
    cout << "P - Play | Space - Pause/Resume | S - Stop" << endl;
    cout << "N - Next | B - Previous | H - Shuffle | R - Repeat" << endl;
    cout << "+/- - Volume Control" << endl;
    cout << "=============================" << endl;
    
    while (window.isOpen()) {
        handleEvents(window);
        drawGUI(window);
    }
    
    cleanup();
    return 0;
}
