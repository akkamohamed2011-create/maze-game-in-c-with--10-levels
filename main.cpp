#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream> // المكتبة الجديدة المضافة

using namespace sf;

// دالة تقوم بحفظ المرحلة الحالية ونوع الصعوبة في ملف نصي
void saveGame(int level, const std::string& difficulty) {
    std::ofstream saveFile("save.txt");
    if (saveFile.is_open()) {
        saveFile << level << "\n";
        saveFile << difficulty << "\n";
        saveFile.close();
    }
}

// دالة تقوم بقراءة المرحلة والصعوبة المحفوظة لتطبيقها عند العودة للعبة
bool loadGame(int& level, std::string& difficulty) {
    std::ifstream saveFile("save.txt");
    if (saveFile.is_open()) {
        saveFile >> level;
        saveFile >> difficulty;
        saveFile.close();
        return true; // نجح التحميل
    }
    return false; // لا يوجد ملف حفظ سابق
}

enum GameState { MAIN_MENU, DIFFICULTY_MENU, SETTINGS, GAMEPLAY };

// متغير عام: كيسمح لـ Easy/Middle/Hard يحددو لأي شاشة يرجعو بعد ما يسدو النافذة ديالهم
// (بالافتراض MAIN_MENU، لكن مثلا بعد إكمال آخر مستوى كنبدلوه لـ DIFFICULTY_MENU)
GameState g_nextState = MAIN_MENU;

// ===================================================================
// هيكل الفخ الجديد: يحتوي على الشكل + حالة الظهور + حالة التفعيل
// revealed = false يعني الفخ مخفي حتى يقترب اللاعب منه
// active = false يعني الفخ اختفى نهائياً بعد لمسه
// ===================================================================
struct Trap {
    RectangleShape shape;
    bool revealed = false;
    bool active = true;
};

// ===================================================================
// هيكل العملة: دائرة صفراء صغيرة، كل عملة = +100 نقطة
// collected = true يعني اللاعب جمعها (كتختفي وكتزاد للنقاط)
// الباب (الهدف) ما يفتحش حتى يجمع اللاعب 5 عملات فنفس المستوى
// ===================================================================
struct Coin {
    CircleShape shape;
    bool collected = false;
};

// ===================================================================
// تصريح مسبق بالدوال الثلاث (كل صعوبة لها دالة كاملة منفصلة بنافذتها
// ولوبها الخاص، تماماً كما كانت Middle() و Hard() في الكود الأصلي)
// ===================================================================
void Easy();
void Middle();
void Hard();

int main()
{
    while (true)
    {

    RenderWindow window(VideoMode(800, 600), "Simple Game");
    window.setFramerateLimit(60);

    GameState state = g_nextState;
    g_nextState = MAIN_MENU; // نعاود نرجعها للافتراضي، غير إلا بغا شي وضع آخر يبدلها من جديد

    // FONT
    Font font;
    if (!font.loadFromFile("Hibo.otf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    // ================= MAIN MENU =================
    RectangleShape playButton(Vector2f(250.f, 80.f));
    playButton.setPosition(250.f, 160.f);
    playButton.setFillColor(Color::Green);

    Text playText;
    playText.setFont(font);
    playText.setString("PLAY");
    playText.setCharacterSize(35);
    playText.setFillColor(Color::White);
    playText.setPosition(310.f, 180.f);

    RectangleShape settingsButton(Vector2f(250.f, 80.f));
    settingsButton.setPosition(250.f, 270.f);
    settingsButton.setFillColor(Color::Cyan);

    Text settingsText;
    settingsText.setFont(font);
    settingsText.setString("SETTINGS");
    settingsText.setCharacterSize(35);
    settingsText.setFillColor(Color::White);
    settingsText.setPosition(290.f, 290.f);

    RectangleShape exitButton(Vector2f(250.f, 80.f));
    exitButton.setPosition(250.f, 380.f);
    exitButton.setFillColor(Color::Red);

    Text exitText;
    exitText.setFont(font);
    exitText.setString("EXIT");
    exitText.setCharacterSize(35);
    exitText.setFillColor(Color::White);
    exitText.setPosition(310.f, 400.f);

    // ================= DIFFICULTY MENU =================
    RectangleShape easyButton(Vector2f(200.f, 50.f));
    easyButton.setPosition(300.f, 85.f);
    easyButton.setFillColor(Color::Blue);

    Text easyText;
    easyText.setFont(font);
    easyText.setString("easy");
    easyText.setCharacterSize(35);
    easyText.setFillColor(Color::White);
    easyText.setPosition(315.f, 90.f);

    RectangleShape middleButton(Vector2f(200.f, 50.f));
    middleButton.setPosition(300.f, 165.f);
    middleButton.setFillColor(Color::Yellow);

    Text middleText;
    middleText.setFont(font);
    middleText.setString("middle");
    middleText.setCharacterSize(35);
    middleText.setFillColor(Color::White);
    middleText.setPosition(315.f, 170.f);

    RectangleShape difficultButton(Vector2f(200.f, 50.f));
    difficultButton.setPosition(300.f, 300.f);
    difficultButton.setFillColor(Color::Green);

    Text diffculText;
    diffculText.setFont(font);
    diffculText.setString("difficult");
    diffculText.setCharacterSize(35);
    diffculText.setFillColor(Color::White);
    diffculText.setPosition(300.f, 305.f);

    // ================= BACK BUTTONS =================
    Text backButton;
    backButton.setFont(font);
    backButton.setString("< Back");
    backButton.setCharacterSize(30);
    backButton.setFillColor(Color::White);
    backButton.setPosition(20.f, 20.f);

    // ================= SETTINGS MENU =================
    Text settingsTitle;
    settingsTitle.setFont(font);
    settingsTitle.setString("Choose Maze Theme Color:");
    settingsTitle.setCharacterSize(35);
    settingsTitle.setFillColor(Color::White);
    settingsTitle.setPosition(200.f, 100.f);

    RectangleShape themeBlue(Vector2f(120.f, 60.f)); themeBlue.setPosition(160.f, 250.f); themeBlue.setFillColor(Color::Blue);
    RectangleShape themeGreen(Vector2f(120.f, 60.f)); themeGreen.setPosition(300.f, 250.f); themeGreen.setFillColor(Color::Green);
    RectangleShape themeRed(Vector2f(120.f, 60.f)); themeRed.setPosition(440.f, 250.f); themeRed.setFillColor(Color::Red);
    RectangleShape themeMagenta(Vector2f(120.f, 60.f)); themeMagenta.setPosition(580.f, 250.f); themeMagenta.setFillColor(Color::Magenta);

    Text settingsBackBtn;
    settingsBackBtn.setFont(font);
    settingsBackBtn.setString("< Back");
    settingsBackBtn.setCharacterSize(30);
    settingsBackBtn.setFillColor(Color::White);
    settingsBackBtn.setPosition(20.f, 20.f);

    while (window.isOpen())
    {
        Vector2i pixelPos = Mouse::getPosition(window);
        Vector2f mouse = window.mapPixelToCoords(pixelPos);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) {
                window.close();
                return 0;
            }

            if (event.type == Event::MouseButtonPressed)
            {
                if (state == MAIN_MENU)
                {
                    if (playButton.getGlobalBounds().contains(mouse))
                        state = DIFFICULTY_MENU;

                    if (settingsButton.getGlobalBounds().contains(mouse))
                        state = SETTINGS;

                    if (exitButton.getGlobalBounds().contains(mouse)) {
                        window.close();
                        return 0;
                    }
                }
                else if (state == DIFFICULTY_MENU)
                {
                    if (backButton.getGlobalBounds().contains(mouse))
                    {
                        state = MAIN_MENU;
                    }
                    if (easyButton.getGlobalBounds().contains(mouse)) {
                        window.close();
                        Easy();
                        goto restart_menu;
                    }
                    if (middleButton.getGlobalBounds().contains(mouse)) {
                        window.close();
                        Middle();
                        goto restart_menu;
                    }
                    if (difficultButton.getGlobalBounds().contains(mouse)) {
                        window.close();
                        Hard();
                        goto restart_menu;
                    }
                }
                else if (state == SETTINGS)
                {
                    if (settingsBackBtn.getGlobalBounds().contains(mouse))
                    {
                        state = MAIN_MENU;
                    }
                }
            }
        }

        if (state == DIFFICULTY_MENU) {
            if (backButton.getGlobalBounds().contains(mouse)) backButton.setFillColor(Color::Yellow);
            else backButton.setFillColor(Color::White);
        }
        if (state == SETTINGS) {
            if (settingsBackBtn.getGlobalBounds().contains(mouse)) settingsBackBtn.setFillColor(Color::Yellow);
            else settingsBackBtn.setFillColor(Color::White);
        }

        window.clear(Color::Black);

        if (state == MAIN_MENU)
        {
            window.draw(playButton);
            window.draw(playText);
            window.draw(settingsButton);
            window.draw(settingsText);
            window.draw(exitButton);
            window.draw(exitText);
        }
        else if (state == DIFFICULTY_MENU)
        {
            window.draw(easyButton);
            window.draw(easyText);
            window.draw(middleButton);
            window.draw(middleText);
            window.draw(difficultButton);
            window.draw(diffculText);
            window.draw(backButton);
        }
        else if (state == SETTINGS)
        {
            window.draw(settingsTitle);
            window.draw(themeBlue);
            window.draw(themeGreen);
            window.draw(themeRed);
            window.draw(themeMagenta);
            window.draw(settingsBackBtn);
        }

        window.display();
    }

    restart_menu: ;
    }

    return 0;
}

// ===================================================================
// ================= EASY: كل منطق المستوى السهل (gameplay) =========
// ===================================================================
void Easy()
{
    RenderWindow window(VideoMode(800, 600), "Simple Game");
    window.setFramerateLimit(60);

    GameState state = GAMEPLAY;
    int level = 1;
    std::string selectedDifficulty = "easy";
    float playerSpeed = 7.f;

    // ===================== نظام الفخاخ (Traps) =====================
    float normalSpeed = 7.f;   // السرعة العادية
    float slowSpeed = 2.5f;    // السرعة كي يلمس اللاعب فخ
    float slowDuration = 3.f;  // مدة الإبطاء بالثواني - بدّلها كيفما بغيتي
    bool isSlowed = false;
    Clock slowClock;
    float trapRevealDistance = 90.f; // المسافة اللي كيبان فيها الفخ - بدّلها كيفما بغيتي
    // ==================================================================

    // متغيرات الألوان الافتراضية للجدران
    Color easyLevel2Color = Color::Blue;
    Color easyLevel1Color = Color::White;
    Color easyLevel3Color = Color::Green;
    Color easyLevel4Color = Color::Red;

    // FONT
    Font font;
    if (!font.loadFromFile("Hibo.otf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    // ===================== صوت فتح الباب (door open sound) =====================
    SoundBuffer doorSoundBuffer;
    Sound doorSound;
    if (!doorSoundBuffer.loadFromFile("mo.ogg"))
    {
        std::cout << "Error loading door sound!" << std::endl;
    }
    else
    {
        doorSound.setBuffer(doorSoundBuffer);
        doorSound.setVolume(80.f);
        doorSound.play(); // === تجربة مؤقتة: كيتشغل مباشرة عند بداية المستوى باش نتأكدو أن الصوت خدام - حيدها من بعد ===
    }
    // ==============================================================================

    // ===================== نظام الوقت (Timer) لكل المستويات =====================
    Clock levelTimer;
    Clock animClock; // ساعة مخصصة للأنيميشن (نبضان العملات، توهج الباب)
    // مدة كل مستوى بالثواني - بدّل الأرقام كيفما بغيتي
    // index: 0 غير مستعمل, 1..10 = مستويات اللعبة القابلة للعب, 11 = شاشة الفوز (بدون وقت)
    float levelTimeLimits[12] = { 0.f, 20.f, 20.f, 20.f, 20.f, 20.f, 20.f, 20.f, 15.f, 20.f, 25.f, 999999.f };

    Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(26);
    timerText.setFillColor(Color::White);
    timerText.setPosition(20.f, 15.f);
    // ==============================================================================

    // زر العودة الجديد المخصص لداخل المتاهة أثناء اللعب
    Text gameplayBackButton;
    gameplayBackButton.setFont(font);
    gameplayBackButton.setString("< Back");
    gameplayBackButton.setCharacterSize(25);
    gameplayBackButton.setFillColor(Color::White);
    gameplayBackButton.setPosition(360.f, 15.f);


    // زر الحفظ الجديد (SAVE GAME)
    RectangleShape menuSaveButton(Vector2f(250.f, 60.f));
    menuSaveButton.setPosition(250.f, 260.f);
    menuSaveButton.setFillColor(Color::Magenta);
    Text menuSaveText; menuSaveText.setFont(font); menuSaveText.setString("SAVE GAME");

    // زر التحميل الجديد (LOAD GAME)
    RectangleShape menuLoadButton(Vector2f(250.f, 60.f));
    menuLoadButton.setPosition(250.f, 340.f);
    menuLoadButton.setFillColor(Color::Blue);
    Text menuLoadText; menuLoadText.setFont(font); menuLoadText.setString("LOAD GAME");

    // ================= TOUCH CONTROLS =================
    float btnSize = 50.f;
    Color btnColor(255, 255, 255, 130);

    RectangleShape upBtn(Vector2f(btnSize, btnSize));
    upBtn.setPosition(700.f, 410.f);
    upBtn.setFillColor(btnColor);

    RectangleShape downBtn(Vector2f(btnSize, btnSize));
    downBtn.setPosition(700.f, 510.f);
    downBtn.setFillColor(btnColor);

    RectangleShape leftBtn(Vector2f(btnSize, btnSize));
    leftBtn.setPosition(40.f, 460.f);
    leftBtn.setFillColor(btnColor);

    RectangleShape rightBtn(Vector2f(btnSize, btnSize));
    rightBtn.setPosition(160.f, 460.f);
    rightBtn.setFillColor(btnColor);

    Text upTxt; upTxt.setFont(font); upTxt.setString("^"); upTxt.setCharacterSize(25); upTxt.setFillColor(Color::Black); upTxt.setPosition(718.f, 415.f);
    Text downTxt; downTxt.setFont(font); downTxt.setString("v"); downTxt.setCharacterSize(20); downTxt.setFillColor(Color::Black); downTxt.setPosition(720.f, 520.f);
    Text leftTxt; leftTxt.setFont(font); leftTxt.setString("<"); leftTxt.setCharacterSize(25); leftTxt.setFillColor(Color::Black); leftTxt.setPosition(55.f, 470.f);
    Text rightTxt; rightTxt.setFont(font); rightTxt.setString(">"); rightTxt.setCharacterSize(25); rightTxt.setFillColor(Color::Black); rightTxt.setPosition(175.f, 470.f);

    // ================= PLAYER =================
    CircleShape player(15.f);
    player.setFillColor(Color::Blue);
    player.setPosition(100.f, 150.f);

    auto resetPlayerPosition = [&]() {
        if (level == 1) { player.setPosition(100.f, 150.f); player.setFillColor(Color::Blue); }
        else if (level == 2) { player.setPosition(20.f, 20.f); player.setFillColor(Color::Red); }
        else if (level == 3) { player.setPosition(30.f, 50.f); player.setFillColor(Color::Magenta); }
        else if (level == 4) { player.setPosition(50.f, 40.f); player.setFillColor(Color::Yellow); }
        else if (level == 5) { player.setPosition(50.f, 65.f); player.setFillColor(Color::Red); }
        else if (level == 6) { player.setPosition(50.f, 65.f); player.setFillColor(Color::Red); }
        else if (level == 7) { player.setPosition(390.f, 40.f); player.setFillColor(Color::Cyan); }
        else if (level == 8) { player.setPosition(50.f, 65.f); player.setFillColor(Color::Yellow); } // متاهة 8 الجديدة
        else if (level == 9) { player.setPosition(390.f, 40.f); player.setFillColor(Color::Blue); } // متاهة 9 الجديدة
        else if (level == 10) { player.setPosition(390.f, 40.f); player.setFillColor(Color::White); } // متاهة 10 الأصعب الجديدة
        else if (level == 11) { player.setPosition(390.f, 40.f); player.setFillColor(Color::Green); } // شاشة الفوز
    };

    // دالة مساعدة: تعيد ضبط اللاعب فمكانو + تعاود تشغيل الوقت لنفس المستوى
    auto restartCurrentLevel = [&]() {
        resetPlayerPosition();
        levelTimer.restart();
    };

    // دالة مساعدة لتسهيل العودة خطوة للخلف
    auto handleBackNavigation = [&]() {
        if (level == 1) {
            window.close();
        }
        else if (level == 2) {
            level = 1;
            player.setPosition(100.f, 150.f);
            levelTimer.restart(); // جديد
        }
        else if (level == 3) {
            level = 2;
            player.setPosition(20.f, 20.f);
            player.setFillColor(Color::Blue);
            levelTimer.restart(); // جديد
        }
        else if (level == 4) {
            level = 3;
            player.setPosition(20.f, 29.f);
            player.setFillColor(Color::Magenta);
            levelTimer.restart(); // جديد
        }
        else if (level == 5) {
            level = 4;
            player.setPosition(20, 20);
            player.setFillColor(Color::Red);
            levelTimer.restart(); // جديد
        }
        else if (level == 6) {
            level = 5;
            player.setPosition(390, 40);
            player.setFillColor(Color::Red);
            levelTimer.restart(); // جديد
        }
                else if (level == 7) {
            level = 6;
            player.setPosition(390, 40);
            player.setFillColor(Color::Red);
            levelTimer.restart(); // جديد
        }
        else if (level == 8) {
            level = 7;
            player.setPosition(390, 40);
            player.setFillColor(Color::Cyan);
            levelTimer.restart(); // جديد
        }
        else if (level == 9) {
            level = 8;
            player.setPosition(50.f, 65.f);
            player.setFillColor(Color::Yellow);
            levelTimer.restart();
        }
        else if (level == 10) {
            level = 9;
            player.setPosition(390.f, 40.f);
            player.setFillColor(Color::Blue);
            levelTimer.restart();
        }
        else if (level == 11) {
            level = 10;
            player.setPosition(390.f, 40.f);
            player.setFillColor(Color::White);
            levelTimer.restart();
        }
    };

    // ================= LEVEL 1 WALLS =================
    std::vector<RectangleShape> wallsLevel1;
    std::vector<RectangleShape> wallsLevel3Easy;
    std::vector<RectangleShape> wallsLevel7;
    auto addWall = [&](std::vector<RectangleShape>& wallVec, Vector2f size, Vector2f pos, Color color = Color::Yellow) {
        RectangleShape wall(size);
        wall.setPosition(pos);
        wall.setFillColor(color);
        wallVec.push_back(wall);
    };

    // ================= نظام الفخاخ: متجهات + دالة الإضافة =================
    std::vector<Trap> trapsLevel1;
    std::vector<Trap> trapsLevel2;
    std::vector<Trap> trapsLevel3;
    std::vector<Trap> trapsLevel4;
    std::vector<Trap> trapsLevel5;
    std::vector<Trap> trapsLevel6;
    std::vector<Trap> trapsLevel7;
    std::vector<Trap> trapsLevel8;
    std::vector<Trap> trapsLevel9;
    std::vector<Trap> trapsLevel10;

    auto addTrap = [&](std::vector<Trap>& trapVec, Vector2f pos, Vector2f size = Vector2f(24.f, 50.f)) {
        Trap trap;
        trap.shape.setSize(size);
        trap.shape.setPosition(pos);
        trap.shape.setFillColor(Color(255, 140, 0)); // برتقالي = فخ
        trap.revealed = false;
        trap.active = true;
        trapVec.push_back(trap);
    };

    // مواقع الفخاخ لكل مستوى (تقديرية - إلا لقيتي فخ فوق جدار بدّل الرقمين بسهولة)
    addTrap(trapsLevel1, Vector2f(250.f, 250.f));
    addTrap(trapsLevel1, Vector2f(450.f, 300.f));
    addTrap(trapsLevel1, Vector2f(650.f, 500.f));

    addTrap(trapsLevel2, Vector2f(150.f, 150.f));
    addTrap(trapsLevel2, Vector2f(400.f, 150.f));
    addTrap(trapsLevel2, Vector2f(600.f, 300.f));

    addTrap(trapsLevel3, Vector2f(200.f, 400.f));
    addTrap(trapsLevel3, Vector2f(400.f, 250.f));
    addTrap(trapsLevel3, Vector2f(600.f, 450.f));

    addTrap(trapsLevel4, Vector2f(250.f, 180.f));
    addTrap(trapsLevel4, Vector2f(450.f, 320.f));
    addTrap(trapsLevel4, Vector2f(650.f, 470.f));

    addTrap(trapsLevel5, Vector2f(200.f, 300.f));
    addTrap(trapsLevel5, Vector2f(400.f, 150.f));
    addTrap(trapsLevel5, Vector2f(600.f, 400.f));

    addTrap(trapsLevel6, Vector2f(400.f, 100.f));
    addTrap(trapsLevel6, Vector2f(700.f, 200.f));
    addTrap(trapsLevel6, Vector2f(400.f, 460.f));

    addTrap(trapsLevel7, Vector2f(300.f, 40.f));
    addTrap(trapsLevel7, Vector2f(720.f, 250.f));
    addTrap(trapsLevel7, Vector2f(400.f, 470.f));

    addTrap(trapsLevel8, Vector2f(250.f, 230.f));
    addTrap(trapsLevel8, Vector2f(400.f, 230.f));
    addTrap(trapsLevel8, Vector2f(300.f, 480.f));

    addTrap(trapsLevel9, Vector2f(200.f, 100.f));
    addTrap(trapsLevel9, Vector2f(500.f, 300.f));
    addTrap(trapsLevel9, Vector2f(650.f, 500.f));

    // فخاخ المستوى 10 (الأصعب): موزعة قرب الفتحات الضيقة باش تزيد صعوبة العبور
    addTrap(trapsLevel10, Vector2f(745.f, 95.f));
    addTrap(trapsLevel10, Vector2f(30.f, 210.f));
    addTrap(trapsLevel10, Vector2f(745.f, 325.f));
    addTrap(trapsLevel10, Vector2f(30.f, 440.f));

    // دالة فحص التصادم مع الفخاخ + تفعيل تأثير الإبطاء + كشف الفخ عند الاقتراب
    auto checkTraps = [&](std::vector<Trap>& traps) {
        Vector2f playerCenter = player.getPosition() + Vector2f(player.getRadius(), player.getRadius());

        for (auto& trap : traps)
        {
            if (!trap.active) continue; // الفخ اختفى نهائياً، تجاوزه

            // حساب المسافة بين اللاعب ومركز الفخ باش نقرروا نبينوه ولا لا
            Vector2f trapCenter = trap.shape.getPosition() + trap.shape.getSize() / 2.f;
            float dx = playerCenter.x - trapCenter.x;
            float dy = playerCenter.y - trapCenter.y;
            float distance = std::sqrt(dx * dx + dy * dy);
           trap.revealed = (distance <= trapRevealDistance);

            if (player.getGlobalBounds().intersects(trap.shape.getGlobalBounds()))
            {
                if (!isSlowed)
                {
                    isSlowed = true;
                    playerSpeed = slowSpeed;
                    slowClock.restart();
                }
                trap.active = false; // الفخ يختفي نهائياً بعد لمسه
            }
        }
    };
    // =======================================================================

    // ===================== نظام العملات (Coins) =====================
    int score = 0;
    const int coinsRequired = 5;

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(Color::Yellow);
    scoreText.setPosition(20.f, 45.f);

    auto addCoin = [&](std::vector<Coin>& coinVec, Vector2f pos) {
        Coin c;
        c.shape.setRadius(9.f);
        c.shape.setOrigin(9.f, 9.f); // نخلي نقطة الأصل فمركز الدائرة باش النبضان يكون متمركز
        c.shape.setFillColor(Color::Yellow);
        c.shape.setOutlineColor(Color(150, 100, 0));
        c.shape.setOutlineThickness(2.f);
        c.shape.setPosition(pos); // pos هي المركز مباشرة دابا
        c.collected = false;
        coinVec.push_back(c);
    };

    auto checkCoins = [&](std::vector<Coin>& coins) {
        for (auto& c : coins)
        {
            if (!c.collected && player.getGlobalBounds().intersects(c.shape.getGlobalBounds()))
            {
                c.collected = true;
                score += 100;
            }
        }
    };

    auto countCollectedCoins = [&](std::vector<Coin>& coins) -> int {
        int n = 0;
        for (const auto& c : coins) if (c.collected) n++;
        return n;
    };

    auto drawCoins = [&](std::vector<Coin>& coins) {
        // نبضان بسيط: العملة كتكبر وتصغر بشوية (Pulse Animation) باش تبان حية
        float pulse = 1.f + 0.18f * std::sin(animClock.getElapsedTime().asSeconds() * 4.f);
        for (auto& c : coins)
        {
            if (!c.collected)
            {
                c.shape.setScale(pulse, pulse);
                c.shape.rotate(0.6f); // دوران خفيف مستمر
                window.draw(c.shape);
            }
        }
    };

    // عملات كل مستوى (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsLevel1, coinsLevel2, coinsLevel3, coinsLevel4, coinsLevel5,
                       coinsLevel6, coinsLevel7, coinsLevel8, coinsLevel9, coinsLevel10;

    addCoin(coinsLevel1, Vector2f(250.f, 250.f));
    addCoin(coinsLevel1, Vector2f(450.f, 300.f));
    addCoin(coinsLevel1, Vector2f(650.f, 500.f));
    addCoin(coinsLevel1, Vector2f(250.f, 300.f));
    addCoin(coinsLevel1, Vector2f(450.f, 350.f));

    addCoin(coinsLevel2, Vector2f(150.f, 150.f));
    addCoin(coinsLevel2, Vector2f(400.f, 150.f));
    addCoin(coinsLevel2, Vector2f(600.f, 300.f));
    addCoin(coinsLevel2, Vector2f(150.f, 200.f));
    addCoin(coinsLevel2, Vector2f(400.f, 200.f));

    addCoin(coinsLevel3, Vector2f(620.f, 200.f));
    addCoin(coinsLevel3, Vector2f(400.f, 250.f));
    addCoin(coinsLevel3, Vector2f(600.f, 450.f));
    addCoin(coinsLevel3, Vector2f(620.f, 100.f));
    addCoin(coinsLevel3, Vector2f(400.f, 300.f));

    addCoin(coinsLevel4, Vector2f(250.f, 180.f));
    addCoin(coinsLevel4, Vector2f(450.f, 320.f));
    addCoin(coinsLevel4, Vector2f(650.f, 470.f));
    addCoin(coinsLevel4, Vector2f(250.f, 230.f));
    addCoin(coinsLevel4, Vector2f(450.f, 370.f));

    addCoin(coinsLevel5, Vector2f(200.f, 300.f));
    addCoin(coinsLevel5, Vector2f(400.f, 150.f));
    addCoin(coinsLevel5, Vector2f(600.f, 400.f));
    addCoin(coinsLevel5, Vector2f(200.f, 350.f));
    addCoin(coinsLevel5, Vector2f(400.f, 200.f));

    addCoin(coinsLevel6, Vector2f(400.f, 100.f));
    addCoin(coinsLevel6, Vector2f(700.f, 200.f));
    addCoin(coinsLevel6, Vector2f(400.f, 460.f));
    addCoin(coinsLevel6, Vector2f(400.f, 150.f));
    addCoin(coinsLevel6, Vector2f(700.f, 250.f));

    addCoin(coinsLevel7, Vector2f(300.f, 40.f));
    addCoin(coinsLevel7, Vector2f(720.f, 250.f));
    addCoin(coinsLevel7, Vector2f(400.f, 470.f));
    addCoin(coinsLevel7, Vector2f(300.f, 90.f));
    addCoin(coinsLevel7, Vector2f(720.f, 300.f));

    addCoin(coinsLevel8, Vector2f(250.f, 230.f));
    addCoin(coinsLevel8, Vector2f(400.f, 230.f));
    addCoin(coinsLevel8, Vector2f(300.f, 480.f));
    addCoin(coinsLevel8, Vector2f(250.f, 280.f));
    addCoin(coinsLevel8, Vector2f(400.f, 280.f));

    addCoin(coinsLevel9, Vector2f(200.f, 100.f));
    addCoin(coinsLevel9, Vector2f(500.f, 300.f));
    addCoin(coinsLevel9, Vector2f(650.f, 500.f));
    addCoin(coinsLevel9, Vector2f(200.f, 150.f));
    addCoin(coinsLevel9, Vector2f(500.f, 350.f));

    addCoin(coinsLevel10, Vector2f(745.f, 95.f));
    addCoin(coinsLevel10, Vector2f(30.f, 210.f));
    addCoin(coinsLevel10, Vector2f(745.f, 325.f));
    addCoin(coinsLevel10, Vector2f(30.f, 440.f));
    addCoin(coinsLevel10, Vector2f(745.f, 145.f));
    // ===================================================================

    addWall(wallsLevel1, Vector2f(40.f, 150.f), Vector2f(5.f, 5.f)); addWall(wallsLevel1, Vector2f(40.f, 150.f), Vector2f(6.f, 150.f)); addWall(wallsLevel1, Vector2f(40.f, 150.f), Vector2f(6.f, 300.f)); addWall(wallsLevel1, Vector2f(40.f, 150.f), Vector2f(6.f, 450.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(45.f, 5.f)); addWall(wallsLevel1, Vector2f(205.f, 40.f), Vector2f(195.f, 5.f)); addWall(wallsLevel1, Vector2f(180.f, 40.f), Vector2f(400.f, 5.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(400.f, 5.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(45.f, 5.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(550.f, 5.f)); addWall(wallsLevel1, Vector2f(200.f, 40.f), Vector2f(600.f, 5.f)); addWall(wallsLevel1, Vector2f(50.f, 200.f), Vector2f(750.f, 30.f)); addWall(wallsLevel1, Vector2f(50.f, 150.f), Vector2f(750.f, 225.f)); addWall(wallsLevel1, Vector2f(50.f, 150.f), Vector2f(750.f, 350.f)); addWall(wallsLevel1, Vector2f(50.f, 150.f), Vector2f(750.f, 450.f)); addWall(wallsLevel1, Vector2f(150.f, 50.f), Vector2f(750.f, 550.f)); addWall(wallsLevel1, Vector2f(150.f, 50.f), Vector2f(46.f, 550.f)); addWall(wallsLevel1, Vector2f(196.f, 50.f), Vector2f(196.f, 550.f)); addWall(wallsLevel1, Vector2f(300.f, 50.f), Vector2f(300.f, 550.f)); addWall(wallsLevel1, Vector2f(150.f, 50.f), Vector2f(400.f, 550.f)); addWall(wallsLevel1, Vector2f(150.f, 50.f), Vector2f(500.f, 550.f)); addWall(wallsLevel1, Vector2f(150.f, 50.f), Vector2f(600.f, 550.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(45.f, 90.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(195.f, 90.f)); addWall(wallsLevel1, Vector2f(300.f, 40.f), Vector2f(300.f, 90.f)); addWall(wallsLevel1, Vector2f(40.f, 150.f), Vector2f(600.f, 90.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(490.f, 200.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(350.f, 200.f)); addWall(wallsLevel1, Vector2f(190.f, 40.f), Vector2f(100.f, 200.f)); addWall(wallsLevel1, Vector2f(54.f, 40.f), Vector2f(46.f, 200.f)); addWall(wallsLevel1, Vector2f(40.f, 150.f), Vector2f(600.f, 200.f)); addWall(wallsLevel1, Vector2f(150.f, 40.f), Vector2f(490.f, 350.f)); addWall(wallsLevel1, Vector2f(400.f, 40.f), Vector2f(100.f, 350.f)); addWall(wallsLevel1, Vector2f(40.f, 120.f), Vector2f(100.f, 380.f)); addWall(wallsLevel1, Vector2f(400.f, 40.f), Vector2f(400.f, 459.f)); addWall(wallsLevel1, Vector2f(250.f, 40.f), Vector2f(100.f, 459.f));

    // ================= LEVEL 2: EASY MAZE WALLS =================
    RectangleShape rectbleu(Vector2f(780, 10)); rectbleu.setPosition(5, 5); rectbleu.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_a(Vector2f(10, 580));rectbleu_a.setPosition(785, 5);rectbleu_a.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_z(Vector2f(795, 10));rectbleu_z.setPosition(5, 580);rectbleu_z.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_e(Vector2f(10, 580));rectbleu_e.setPosition(5, 11);rectbleu_e.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_r(Vector2f(100, 10));rectbleu_r.setPosition(6, 80);rectbleu_r.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_t(Vector2f(10, 300));rectbleu_t.setPosition(105, 80);rectbleu_t.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_y(Vector2f(200, 10));rectbleu_y.setPosition(105, 380);rectbleu_y.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_u(Vector2f(10, 100));rectbleu_u.setPosition(200, 290);rectbleu_u.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_i(Vector2f(10, 100));rectbleu_i.setPosition(305, 380);rectbleu_i.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_o(Vector2f(350, 10));rectbleu_o.setPosition(305, 480);rectbleu_o.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_p(Vector2f(140, 10));rectbleu_p.setPosition(650, 380);rectbleu_p.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_q(Vector2f(10, 200));rectbleu_q.setPosition(200, 11);rectbleu_q.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_s(Vector2f(100, 10));rectbleu_s.setPosition(200, 201);rectbleu_s.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_d(Vector2f(10, 100));rectbleu_d.setPosition(300, 201);rectbleu_d.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_f(Vector2f(340, 10));rectbleu_f.setPosition(300, 300);rectbleu_f.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_g(Vector2f(10, 100));rectbleu_g.setPosition(470, 300);rectbleu_g.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_h(Vector2f(10, 100));rectbleu_h.setPosition(640, 210);rectbleu_h.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_j(Vector2f(490, 10));rectbleu_j.setPosition(300, 80);rectbleu_j.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_k(Vector2f(10, 150));rectbleu_k.setPosition(470, 80);rectbleu_k.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_l(Vector2f(140, 10));rectbleu_l.setPosition(11, 480);rectbleu_l.setFillColor(easyLevel2Color);
    RectangleShape rectbleu_m(Vector2f(10, 100));rectbleu_m.setPosition(200, 480);rectbleu_m.setFillColor(easyLevel2Color);

    // متاهة الثالثة في السهل
    RectangleShape rectmagenta(Vector2f(770, 30));rectmagenta.setPosition(1, 0);rectmagenta.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_a(Vector2f(30, 570));rectmagenta_a.setPosition(770, 0);rectmagenta_a.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_z(Vector2f(30, 600));rectmagenta_z.setPosition(1, 30);rectmagenta_z.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_e(Vector2f(770, 30));rectmagenta_e.setPosition(30, 570);rectmagenta_e.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_r(Vector2f(30, 200));rectmagenta_r.setPosition(80, 30);rectmagenta_r.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_t(Vector2f(450, 30));rectmagenta_t.setPosition(30, 300);rectmagenta_t.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_y(Vector2f(100, 30));rectmagenta_y.setPosition(80, 200);rectmagenta_y.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_u(Vector2f(30, 155));rectmagenta_u.setPosition(180, 75);rectmagenta_u.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_i(Vector2f(400, 30));rectmagenta_i.setPosition(180, 75);rectmagenta_i.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_o(Vector2f(30, 120));rectmagenta_o.setPosition(680, 30);rectmagenta_o.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_p(Vector2f(430, 30));rectmagenta_p.setPosition(280, 150);rectmagenta_p.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_q(Vector2f(30, 100));rectmagenta_q.setPosition(280, 150);rectmagenta_q.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_s(Vector2f(270, 30));rectmagenta_s.setPosition(280,220 );rectmagenta_s.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_d(Vector2f(30, 540));rectmagenta_d.setPosition(450, 300);rectmagenta_d.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_f(Vector2f(30, 170));rectmagenta_f.setPosition(550, 220);rectmagenta_f.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_g(Vector2f(100, 30));rectmagenta_g.setPosition(550, 390);rectmagenta_g.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_h(Vector2f(30, 150));rectmagenta_h.setPosition(650, 330);rectmagenta_h.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_j(Vector2f(100, 30));rectmagenta_j.setPosition(450, 490);rectmagenta_j.setFillColor(easyLevel3Color);
    RectangleShape rectmagenta_k(Vector2f(30, 100));rectmagenta_k.setPosition(580, 500);rectmagenta_k.setFillColor(easyLevel3Color);

    // متاحبة الرابعة في السهل
    RectangleShape rectwhite(Vector2f(790, 10));rectwhite.setPosition(0, 0);rectwhite.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_a(Vector2f(10, 590));rectwhite_a.setPosition(790, 0);rectwhite_a.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_z(Vector2f(10, 590));rectwhite_z.setPosition(0, 10);rectwhite_z.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_e(Vector2f(790, 10));rectwhite_e.setPosition(10, 590);rectwhite_e.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_r(Vector2f(10, 250));rectwhite_r.setPosition(100, 10);rectwhite_r.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_t(Vector2f(300, 10));rectwhite_t.setPosition(200, 50);rectwhite_t.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_y(Vector2f(10, 100));rectwhite_y.setPosition(200, 50);rectwhite_y.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_u(Vector2f(100, 10));rectwhite_u.setPosition(200, 150);rectwhite_u.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_i(Vector2f(10, 60));rectwhite_i.setPosition(300, 150);rectwhite_i.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_o(Vector2f(200, 10));rectwhite_o.setPosition(100, 200);rectwhite_o.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_p(Vector2f(200, 10));rectwhite_p.setPosition(600, 50);rectwhite_p.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_q(Vector2f(10, 200));rectwhite_q.setPosition(600, 50);rectwhite_q.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_s(Vector2f(300, 10));rectwhite_s.setPosition(300, 100);rectwhite_s.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_d(Vector2f(100, 10));rectwhite_d.setPosition(600, 250);rectwhite_d.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_f(Vector2f(10, 50));rectwhite_f.setPosition(700, 250);rectwhite_f.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_g(Vector2f(10, 150));rectwhite_g.setPosition(100, 250);rectwhite_g.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_h(Vector2f(200, 10));rectwhite_h.setPosition(10, 450);rectwhite_h.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_j(Vector2f(10, 200));rectwhite_j.setPosition(200, 250);rectwhite_j.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_k(Vector2f(300, 10));rectwhite_k.setPosition(200, 250);rectwhite_k.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_l(Vector2f(10, 60));rectwhite_l.setPosition(500, 200);rectwhite_l.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_m(Vector2f(130, 10));rectwhite_m.setPosition(370, 200);rectwhite_m.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_w(Vector2f(10, 50));rectwhite_w.setPosition(370, 150);rectwhite_w.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_x(Vector2f(180, 10));rectwhite_x.setPosition(370, 150);rectwhite_x.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_c(Vector2f(10, 100));rectwhite_c.setPosition(550, 150);rectwhite_c.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_v(Vector2f(310, 10));rectwhite_v.setPosition(400, 300);rectwhite_v.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_b(Vector2f(10, 50));rectwhite_b.setPosition(400, 300);rectwhite_b.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_n(Vector2f(100, 10));rectwhite_n.setPosition(400, 350);rectwhite_n.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_A(Vector2f(10, 160));rectwhite_A.setPosition(500, 350);rectwhite_A.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_Z(Vector2f(100, 10));rectwhite_Z.setPosition(500, 510);rectwhite_Z.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_E(Vector2f(10, 170));rectwhite_E.setPosition(700, 450);rectwhite_E.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_R(Vector2f(150, 10));rectwhite_R.setPosition(550, 450);rectwhite_R.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_T(Vector2f(10, 50));rectwhite_T.setPosition(550, 405);rectwhite_T.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_Y(Vector2f(150, 10));rectwhite_Y.setPosition(550, 405);rectwhite_Y.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_U(Vector2f(250, 10));rectwhite_U.setPosition(550, 355);rectwhite_U.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_I(Vector2f(150, 10));rectwhite_I.setPosition(650, 155);rectwhite_I.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_O(Vector2f(10, 50));rectwhite_O.setPosition(650, 105);rectwhite_O.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_P(Vector2f(100, 10));rectwhite_P.setPosition(650, 105);rectwhite_P.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_Q(Vector2f(10, 190));rectwhite_Q.setPosition(400, 400);rectwhite_Q.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_S(Vector2f(100, 10));rectwhite_S.setPosition(300, 400);rectwhite_S.setFillColor(easyLevel4Color);
    RectangleShape rectwhite_D(Vector2f(10, 190));rectwhite_D.setPosition(300, 350);rectwhite_D.setFillColor(easyLevel4Color);

    // متاهة الخامس في السهل
    RectangleShape level_a(Vector2f(790, 10));level_a.setPosition(1, 1);level_a.setFillColor(Color::Blue);
    RectangleShape level_z(Vector2f(10, 590));level_z.setPosition(790, 1);level_z.setFillColor(Color::Blue);
    RectangleShape level_e(Vector2f(10, 590));level_e.setPosition(0, 10);level_e.setFillColor(Color::Blue);
    RectangleShape level_r(Vector2f(790, 10));level_r.setPosition(10, 590);level_r.setFillColor(Color::Blue);
    RectangleShape level_t(Vector2f(300, 10));level_t.setPosition(10, 50);level_t.setFillColor(Color::Blue);
    RectangleShape level_y(Vector2f(10, 200));level_y.setPosition(400, 10);level_y.setFillColor(Color::Blue);
    RectangleShape level_u(Vector2f(10, 100));level_u.setPosition(100, 50);level_u.setFillColor(Color::Blue);
    RectangleShape level_i(Vector2f(200, 10));level_i.setPosition(100, 100);level_i.setFillColor(Color::Blue);
    RectangleShape level_o(Vector2f(200, 10));level_o.setPosition(10, 200);level_o.setFillColor(Color::Blue);
    RectangleShape level_p(Vector2f(10, 50));level_p.setPosition(200, 150);level_p.setFillColor(Color::Blue);
    RectangleShape level_q(Vector2f(100, 10));level_q.setPosition(200, 150);level_q.setFillColor(Color::Blue);
    RectangleShape level_s(Vector2f(100, 10));level_s.setPosition(300, 200);level_s.setFillColor(Color::Blue);
    RectangleShape level_d(Vector2f(10, 150));level_d.setPosition(300, 200);level_d.setFillColor(Color::Blue);
    RectangleShape level_f(Vector2f(110, 10));level_f.setPosition(200, 350);level_f.setFillColor(Color::Blue);
    RectangleShape level_g(Vector2f(10, 100));level_g.setPosition(200, 350);level_g.setFillColor(Color::Blue);
    RectangleShape level_h(Vector2f(50, 10));level_h.setPosition(200, 450);level_h.setFillColor(Color::Blue);
    RectangleShape level_j(Vector2f(10, 60));level_j.setPosition(250, 400);level_j.setFillColor(Color::Blue);
    RectangleShape level_k(Vector2f(100, 10));level_k.setPosition(250, 400);level_k.setFillColor(Color::Blue);
    RectangleShape level_l(Vector2f(10, 160));level_l.setPosition(350, 250);level_l.setFillColor(Color::Blue);
    RectangleShape level_m(Vector2f(100, 10));level_m.setPosition(350, 250);level_m.setFillColor(Color::Blue);
    RectangleShape level_w(Vector2f(10, 160));level_w.setPosition(450, 100);level_w.setFillColor(Color::Blue);
    RectangleShape level_x(Vector2f(150, 10));level_x.setPosition(450, 100);level_x.setFillColor(Color::Blue);
    RectangleShape level_c(Vector2f(10, 60));level_c.setPosition(600, 50);level_c.setFillColor(Color::Blue);
    RectangleShape level_v(Vector2f(150, 10));level_v.setPosition(450, 50);level_v.setFillColor(Color::Blue);
    RectangleShape level_b(Vector2f(10, 140));level_b.setPosition(300, 450);level_b.setFillColor(Color::Blue);
    RectangleShape level_n(Vector2f(210, 10));level_n.setPosition(100, 500);level_n.setFillColor(Color::Blue);
    RectangleShape level_A(Vector2f(10, 250));level_A.setPosition(100, 250);level_A.setFillColor(Color::Blue);
    RectangleShape level_Z(Vector2f(120, 10));level_Z.setPosition(100, 250);level_Z.setFillColor(Color::Blue);
    RectangleShape level_E(Vector2f(100, 10));level_E.setPosition(100, 300);level_E.setFillColor(Color::Blue);
    RectangleShape level_R(Vector2f(100, 10));level_R.setPosition(300, 450);level_R.setFillColor(Color::Blue);
    RectangleShape level_T(Vector2f(10, 160));level_T.setPosition(400, 300);level_T.setFillColor(Color::Blue);
    RectangleShape level_Y(Vector2f(100, 10));level_Y.setPosition(400, 300);level_Y.setFillColor(Color::Blue);
    RectangleShape level_U(Vector2f(10, 160));level_U.setPosition(500, 150);level_U.setFillColor(Color::Blue);
    RectangleShape level_I(Vector2f(200, 10));level_I.setPosition(500, 150);level_I.setFillColor(Color::Blue);
    RectangleShape level_O(Vector2f(10, 50));level_O.setPosition(700, 150);level_O.setFillColor(Color::Blue);
    RectangleShape level_P(Vector2f(60, 10));level_P.setPosition(650, 200);level_P.setFillColor(Color::Blue);
    RectangleShape level_Q(Vector2f(10, 150));level_Q.setPosition(650, 200);level_Q.setFillColor(Color::Blue);
    RectangleShape level_S(Vector2f(10, 200));level_S.setPosition(450, 350);level_S.setFillColor(Color::Blue);
    RectangleShape level_D(Vector2f(210, 10));level_D.setPosition(450, 350);level_D.setFillColor(Color::Blue);
    RectangleShape level_F(Vector2f(110, 10));level_F.setPosition(350, 550);level_F.setFillColor(Color::Blue);
    RectangleShape level_G(Vector2f(70, 10));level_G.setPosition(720, 260);level_G.setFillColor(Color::Blue);
    RectangleShape level_H(Vector2f(10, 150));level_H.setPosition(720, 260);level_H.setFillColor(Color::Blue);
    RectangleShape level_J(Vector2f(200, 10));level_J.setPosition(520, 410);level_J.setFillColor(Color::Blue);
    RectangleShape level_K(Vector2f(10, 130));level_K.setPosition(520, 410);level_K.setFillColor(Color::Blue);
    RectangleShape level_L(Vector2f(120, 10));level_L.setPosition(680, 510);level_L.setFillColor(Color::Blue);

    // ================= LEVEL 6: SPIRAL MAZE (نفس شكل الصورة) =================
    // --- الإطار 0 (الخارجي - بلا فتحة) ---
    RectangleShape lvl6_f0_top(Vector2f(760.f, 10.f));    lvl6_f0_top.setPosition(20.f, 10.f);    lvl6_f0_top.setFillColor(Color::Blue);
    RectangleShape lvl6_f0_bottom(Vector2f(760.f, 10.f)); lvl6_f0_bottom.setPosition(20.f, 580.f);lvl6_f0_bottom.setFillColor(Color::Blue);
    RectangleShape lvl6_f0_left(Vector2f(10.f, 580.f));   lvl6_f0_left.setPosition(20.f, 10.f);   lvl6_f0_left.setFillColor(Color::Blue);
    RectangleShape lvl6_f0_right(Vector2f(10.f, 580.f));  lvl6_f0_right.setPosition(770.f, 10.f); lvl6_f0_right.setFillColor(Color::Blue);
    RectangleShape lvl6_f1_top(Vector2f(630.f, 10.f));    lvl6_f1_top.setPosition(85.f, 75.f);    lvl6_f1_top.setFillColor(Color::Blue);
    RectangleShape lvl6_f1_bottom(Vector2f(630.f, 10.f)); lvl6_f1_bottom.setPosition(85.f, 515.f);lvl6_f1_bottom.setFillColor(Color::Blue);
    RectangleShape lvl6_f1_left(Vector2f(10.f, 450.f));   lvl6_f1_left.setPosition(85.f, 75.f);   lvl6_f1_left.setFillColor(Color::Blue);
    RectangleShape lvl6_f1_right_a(Vector2f(10.f, 200.f)); lvl6_f1_right_a.setPosition(705.f, 75.f);  lvl6_f1_right_a.setFillColor(Color::Blue);
    RectangleShape lvl6_f1_right_b(Vector2f(10.f, 200.f)); lvl6_f1_right_b.setPosition(705.f, 325.f); lvl6_f1_right_b.setFillColor(Color::Blue);
    RectangleShape lvl6_f2_top(Vector2f(500.f, 10.f));    lvl6_f2_top.setPosition(150.f, 140.f);  lvl6_f2_top.setFillColor(Color::Blue);
    RectangleShape lvl6_f2_left(Vector2f(10.f, 320.f));   lvl6_f2_left.setPosition(150.f, 140.f); lvl6_f2_left.setFillColor(Color::Blue);
    RectangleShape lvl6_f2_right(Vector2f(10.f, 320.f));  lvl6_f2_right.setPosition(640.f, 140.f);lvl6_f2_right.setFillColor(Color::Blue);
    RectangleShape lvl6_f2_bottom_a(Vector2f(225.f, 10.f)); lvl6_f2_bottom_a.setPosition(150.f, 450.f); lvl6_f2_bottom_a.setFillColor(Color::Blue);
    RectangleShape lvl6_f2_bottom_b(Vector2f(225.f, 10.f)); lvl6_f2_bottom_b.setPosition(425.f, 450.f); lvl6_f2_bottom_b.setFillColor(Color::Blue);
    RectangleShape lvl6_f3_top(Vector2f(370.f, 10.f));    lvl6_f3_top.setPosition(215.f, 205.f);  lvl6_f3_top.setFillColor(Color::Blue);
    RectangleShape lvl6_f3_bottom(Vector2f(370.f, 10.f)); lvl6_f3_bottom.setPosition(215.f, 385.f);lvl6_f3_bottom.setFillColor(Color::Blue);
    RectangleShape lvl6_f3_right(Vector2f(10.f, 190.f));  lvl6_f3_right.setPosition(575.f, 205.f);lvl6_f3_right.setFillColor(Color::Blue);
    RectangleShape lvl6_f3_left_a(Vector2f(10.f, 70.f)); lvl6_f3_left_a.setPosition(215.f, 205.f); lvl6_f3_left_a.setFillColor(Color::Blue);
    RectangleShape lvl6_f3_left_b(Vector2f(10.f, 70.f)); lvl6_f3_left_b.setPosition(215.f, 325.f); lvl6_f3_left_b.setFillColor(Color::Blue);
    RectangleShape lvl6_f4_top_a(Vector2f(95.f, 10.f)); lvl6_f4_top_a.setPosition(280.f, 270.f); lvl6_f4_top_a.setFillColor(Color::Blue);
    RectangleShape lvl6_f4_top_b(Vector2f(95.f, 10.f)); lvl6_f4_top_b.setPosition(425.f, 270.f); lvl6_f4_top_b.setFillColor(Color::Blue);
    RectangleShape lvl6_f4_bottom(Vector2f(240.f, 10.f)); lvl6_f4_bottom.setPosition(280.f, 320.f); lvl6_f4_bottom.setFillColor(Color::Blue);
    RectangleShape lvl6_f4_left(Vector2f(10.f, 60.f));  lvl6_f4_left.setPosition(280.f, 270.f); lvl6_f4_left.setFillColor(Color::Blue);
    RectangleShape lvl6_f4_right(Vector2f(10.f, 60.f)); lvl6_f4_right.setPosition(510.f, 270.f); lvl6_f4_right.setFillColor(Color::Blue);

    // ================= LEVEL 7: SPIRAL MAZE (شكل مختلف عن Level 6) =================
    // --- الإطار 0 (الخارجي) ---
    RectangleShape lvl7_f0_top(Vector2f(770.f, 10.f));    lvl7_f0_top.setPosition(15.f, 15.f);    lvl7_f0_top.setFillColor(Color::Cyan);
    RectangleShape lvl7_f0_bottom(Vector2f(770.f, 10.f)); lvl7_f0_bottom.setPosition(15.f, 575.f);lvl7_f0_bottom.setFillColor(Color::Cyan);
    RectangleShape lvl7_f0_left(Vector2f(10.f, 570.f));   lvl7_f0_left.setPosition(15.f, 15.f);   lvl7_f0_left.setFillColor(Color::Cyan);
    RectangleShape lvl7_f0_right(Vector2f(10.f, 570.f));  lvl7_f0_right.setPosition(775.f, 15.f); lvl7_f0_right.setFillColor(Color::Cyan);

    // --- الإطار 1 (رفيع جدا - فتحة فالأعلى يمين) ---
    RectangleShape lvl7_f1_left(Vector2f(10.f, 480.f));    lvl7_f1_left.setPosition(60.f, 40.f);   lvl7_f1_left.setFillColor(Color::Cyan);
    RectangleShape lvl7_f1_bottom(Vector2f(660.f, 10.f));  lvl7_f1_bottom.setPosition(60.f, 510.f); lvl7_f1_bottom.setFillColor(Color::Cyan);
    RectangleShape lvl7_f1_right(Vector2f(10.f, 470.f));   lvl7_f1_right.setPosition(720.f, 40.f);  lvl7_f1_right.setFillColor(Color::Cyan);
    RectangleShape lvl7_f1_top_a(Vector2f(300.f, 10.f));   lvl7_f1_top_a.setPosition(60.f, 40.f);   lvl7_f1_top_a.setFillColor(Color::Cyan);
    RectangleShape lvl7_f1_top_b(Vector2f(260.f, 10.f));   lvl7_f1_top_b.setPosition(470.f, 40.f);  lvl7_f1_top_b.setFillColor(Color::Cyan);

    // --- الإطار 2 (واسع - فتحة فاليسار تحت) ---
    RectangleShape lvl7_f2_top(Vector2f(540.f, 10.f));    lvl7_f2_top.setPosition(120.f, 100.f);   lvl7_f2_top.setFillColor(Color::Cyan);
    RectangleShape lvl7_f2_right(Vector2f(10.f, 340.f));  lvl7_f2_right.setPosition(650.f, 100.f); lvl7_f2_right.setFillColor(Color::Cyan);
    RectangleShape lvl7_f2_bottom(Vector2f(540.f, 10.f)); lvl7_f2_bottom.setPosition(120.f, 430.f);lvl7_f2_bottom.setFillColor(Color::Cyan);
    RectangleShape lvl7_f2_left_a(Vector2f(10.f, 100.f)); lvl7_f2_left_a.setPosition(120.f, 100.f); lvl7_f2_left_a.setFillColor(Color::Cyan);
    RectangleShape lvl7_f2_left_b(Vector2f(10.f, 190.f)); lvl7_f2_left_b.setPosition(120.f, 240.f); lvl7_f2_left_b.setFillColor(Color::Cyan);

    // --- الإطار 3 (ضيق - فتحة فاليمين فوق) ---
    RectangleShape lvl7_f3_left(Vector2f(10.f, 260.f));    lvl7_f3_left.setPosition(180.f, 160.f);  lvl7_f3_left.setFillColor(Color::Cyan);
    RectangleShape lvl7_f3_bottom(Vector2f(410.f, 10.f));  lvl7_f3_bottom.setPosition(180.f, 410.f);lvl7_f3_bottom.setFillColor(Color::Cyan);
    RectangleShape lvl7_f3_right(Vector2f(10.f, 200.f));   lvl7_f3_right.setPosition(580.f, 220.f); lvl7_f3_right.setFillColor(Color::Cyan);
    RectangleShape lvl7_f3_top_a(Vector2f(300.f, 10.f));   lvl7_f3_top_a.setPosition(180.f, 160.f); lvl7_f3_top_a.setFillColor(Color::Cyan);

    // --- الإطار 4 (متوسط - فتحة فتحت) ---
    RectangleShape lvl7_f4_top(Vector2f(300.f, 10.f));     lvl7_f4_top.setPosition(240.f, 220.f);  lvl7_f4_top.setFillColor(Color::Cyan);
    RectangleShape lvl7_f4_left(Vector2f(10.f, 180.f));    lvl7_f4_left.setPosition(240.f, 220.f); lvl7_f4_left.setFillColor(Color::Cyan);
    RectangleShape lvl7_f4_right(Vector2f(10.f, 130.f));   lvl7_f4_right.setPosition(530.f, 220.f);lvl7_f4_right.setFillColor(Color::Cyan);
    RectangleShape lvl7_f4_bottom_a(Vector2f(150.f, 10.f)); lvl7_f4_bottom_a.setPosition(240.f, 390.f); lvl7_f4_bottom_a.setFillColor(Color::Cyan);

    // --- الإطار 5 (الحجرة المركزية الصغيرة - فتحة فاليسار) ---
    RectangleShape lvl7_f5_top(Vector2f(150.f, 10.f));    lvl7_f5_top.setPosition(330.f, 280.f);  lvl7_f5_top.setFillColor(Color::Cyan);
    RectangleShape lvl7_f5_bottom(Vector2f(150.f, 10.f)); lvl7_f5_bottom.setPosition(330.f, 340.f);lvl7_f5_bottom.setFillColor(Color::Cyan);
    RectangleShape lvl7_f5_right(Vector2f(10.f, 70.f));   lvl7_f5_right.setPosition(470.f, 280.f); lvl7_f5_right.setFillColor(Color::Cyan);

    // ================= LEVEL 8: MAZE (شكل كلاسيكي عادي) =================
    RectangleShape lvl8_a(Vector2f(790, 10)); lvl8_a.setPosition(1, 1); lvl8_a.setFillColor(Color::Yellow);
    RectangleShape lvl8_b(Vector2f(10, 590)); lvl8_b.setPosition(780, 1); lvl8_b.setFillColor(Color::Yellow);
    RectangleShape lvl8_c(Vector2f(10, 590)); lvl8_c.setPosition(1, 1); lvl8_c.setFillColor(Color::Yellow);
    RectangleShape lvl8_d(Vector2f(790, 10)); lvl8_d.setPosition(1, 590); lvl8_d.setFillColor(Color::Yellow);

    RectangleShape lvl8_e(Vector2f(10, 200)); lvl8_e.setPosition(100, 1); lvl8_e.setFillColor(Color::Yellow);
    RectangleShape lvl8_f(Vector2f(200, 10)); lvl8_f.setPosition(100, 200); lvl8_f.setFillColor(Color::Yellow);
    RectangleShape lvl8_g(Vector2f(10, 150)); lvl8_g.setPosition(300, 100); lvl8_g.setFillColor(Color::Yellow);
    RectangleShape lvl8_h(Vector2f(200, 10)); lvl8_h.setPosition(300, 100); lvl8_h.setFillColor(Color::Yellow);

    RectangleShape lvl8_i(Vector2f(10, 250)); lvl8_i.setPosition(500, 1); lvl8_i.setFillColor(Color::Yellow);
    RectangleShape lvl8_j(Vector2f(150, 10)); lvl8_j.setPosition(500, 250); lvl8_j.setFillColor(Color::Yellow);
    RectangleShape lvl8_k(Vector2f(10, 150)); lvl8_k.setPosition(650, 150); lvl8_k.setFillColor(Color::Yellow);
    RectangleShape lvl8_l(Vector2f(140, 10)); lvl8_l.setPosition(650, 150); lvl8_l.setFillColor(Color::Yellow);

    RectangleShape lvl8_m(Vector2f(10, 300)); lvl8_m.setPosition(200, 300); lvl8_m.setFillColor(Color::Yellow);
    RectangleShape lvl8_n(Vector2f(210, 10)); lvl8_n.setPosition(200, 300); lvl8_n.setFillColor(Color::Yellow);
    RectangleShape lvl8_o(Vector2f(10, 150)); lvl8_o.setPosition(400, 300); lvl8_o.setFillColor(Color::Yellow);
    RectangleShape lvl8_p(Vector2f(150, 10)); lvl8_p.setPosition(400, 450); lvl8_p.setFillColor(Color::Yellow);

    RectangleShape lvl8_q(Vector2f(10, 250)); lvl8_q.setPosition(550, 340); lvl8_q.setFillColor(Color::Yellow);
    RectangleShape lvl8_r(Vector2f(180, 10)); lvl8_r.setPosition(550, 340); lvl8_r.setFillColor(Color::Yellow);
    RectangleShape lvl8_s(Vector2f(10, 200)); lvl8_s.setPosition(300, 350); lvl8_s.setFillColor(Color::Yellow);
    RectangleShape lvl8_t(Vector2f(100, 10)); lvl8_t.setPosition(100, 400); lvl8_t.setFillColor(Color::Yellow);
    RectangleShape lvl8_u(Vector2f(10, 150)); lvl8_u.setPosition(100, 400); lvl8_u.setFillColor(Color::Yellow);

    // ================= LEVEL 9: SNAKE MAZE (أصعب من المستوى 8 - ممرات ضيقة وطويلة) =================
    // إطار خارجي كامل (بلا أي فتحة) - لون Magenta ليتماشى مع باقي المتاهات
    RectangleShape lvl9_a(Vector2f(790, 10)); lvl9_a.setPosition(1, 1);   lvl9_a.setFillColor(Color::Magenta); // أعلى
    RectangleShape lvl9_b(Vector2f(10, 590)); lvl9_b.setPosition(780, 1); lvl9_b.setFillColor(Color::Magenta); // يمين
    RectangleShape lvl9_c(Vector2f(10, 590)); lvl9_c.setPosition(1, 1);   lvl9_c.setFillColor(Color::Magenta); // يسار
    RectangleShape lvl9_d(Vector2f(790, 10)); lvl9_d.setPosition(1, 590); lvl9_d.setFillColor(Color::Magenta); // أسفل

    // 7 حواجز أفقية متعرجة (Zigzag) بفتحات ضيقة ~70-80px تتناوب بين اليمين واليسار
    // اللاعب كيبدا فالأعلى (390, 40) وخاصو يتعرج للأسفل حتى الهدف
    RectangleShape lvl9_bar1(Vector2f(699, 10)); lvl9_bar1.setPosition(11, 80);   lvl9_bar1.setFillColor(Color::Magenta); // فتحة فاليمين (710-780)
    RectangleShape lvl9_bar2(Vector2f(689, 10)); lvl9_bar2.setPosition(91, 150);  lvl9_bar2.setFillColor(Color::Magenta); // فتحة فاليسار (11-91)
    RectangleShape lvl9_bar3(Vector2f(699, 10)); lvl9_bar3.setPosition(11, 220);  lvl9_bar3.setFillColor(Color::Magenta); // فتحة فاليمين
    RectangleShape lvl9_bar4(Vector2f(689, 10)); lvl9_bar4.setPosition(91, 290);  lvl9_bar4.setFillColor(Color::Magenta); // فتحة فاليسار
    RectangleShape lvl9_bar5(Vector2f(699, 10)); lvl9_bar5.setPosition(11, 360);  lvl9_bar5.setFillColor(Color::Magenta); // فتحة فاليمين
    RectangleShape lvl9_bar6(Vector2f(689, 10)); lvl9_bar6.setPosition(91, 430);  lvl9_bar6.setFillColor(Color::Magenta); // فتحة فاليسار
    RectangleShape lvl9_bar7(Vector2f(699, 10)); lvl9_bar7.setPosition(11, 500);  lvl9_bar7.setFillColor(Color::Magenta); // فتحة فاليمين

    // ================= LEVEL 10: ZIGZAG MAZE (أصعب بكثير من 9: 9 حواجز وفتحات 50px فقط) =================
    // إطار خارجي كامل - لون بنفسجي مميز
    Color lvl10Color(148, 0, 211);
    RectangleShape lvl10_a(Vector2f(790, 10)); lvl10_a.setPosition(1, 1);   lvl10_a.setFillColor(lvl10Color); // أعلى
    RectangleShape lvl10_b(Vector2f(10, 590)); lvl10_b.setPosition(780, 1); lvl10_b.setFillColor(lvl10Color); // يمين
    RectangleShape lvl10_c(Vector2f(10, 590)); lvl10_c.setPosition(1, 1);   lvl10_c.setFillColor(lvl10Color); // يسار
    RectangleShape lvl10_d(Vector2f(790, 10)); lvl10_d.setPosition(1, 590); lvl10_d.setFillColor(lvl10Color); // أسفل

    // 9 حواجز أفقية متعرجة بفتحات ضيقة 50px فقط (أضيق من المستوى 9) - أصعب وأطول
    RectangleShape lvl10_bar1(Vector2f(719, 10)); lvl10_bar1.setPosition(11, 70);   lvl10_bar1.setFillColor(lvl10Color); // فتحة فاليمين (730-780)
    RectangleShape lvl10_bar2(Vector2f(719, 10)); lvl10_bar2.setPosition(61, 128);  lvl10_bar2.setFillColor(lvl10Color); // فتحة فاليسار (11-61)
    RectangleShape lvl10_bar3(Vector2f(719, 10)); lvl10_bar3.setPosition(11, 185);  lvl10_bar3.setFillColor(lvl10Color); // فتحة فاليمين
    RectangleShape lvl10_bar4(Vector2f(719, 10)); lvl10_bar4.setPosition(61, 243);  lvl10_bar4.setFillColor(lvl10Color); // فتحة فاليسار
    RectangleShape lvl10_bar5(Vector2f(719, 10)); lvl10_bar5.setPosition(11, 300);  lvl10_bar5.setFillColor(lvl10Color); // فتحة فاليمين
    RectangleShape lvl10_bar6(Vector2f(719, 10)); lvl10_bar6.setPosition(61, 358);  lvl10_bar6.setFillColor(lvl10Color); // فتحة فاليسار
    RectangleShape lvl10_bar7(Vector2f(719, 10)); lvl10_bar7.setPosition(11, 415);  lvl10_bar7.setFillColor(lvl10Color); // فتحة فاليمين
    RectangleShape lvl10_bar8(Vector2f(719, 10)); lvl10_bar8.setPosition(61, 473);  lvl10_bar8.setFillColor(lvl10Color); // فتحة فاليسار
    RectangleShape lvl10_bar9(Vector2f(719, 10)); lvl10_bar9.setPosition(11, 530);  lvl10_bar9.setFillColor(lvl10Color); // فتحة فاليمين

    // Goal zone
    Texture texture;
    if (!texture.loadFromFile("mo.jpg"))
    {
        std::cout << "Error loading texture" << std::endl;
    }
    RectangleShape rectYellow_P(Vector2f(40.f, 40.f));
    rectYellow_P.setPosition(147.f, 150.f);
    rectYellow_P.setTexture(&texture);

    RectangleShape rectbleu_w(Vector2f(40, 40));
    rectbleu_w.setPosition(100, 40);
    rectbleu_w.setTexture(&texture);

    RectangleShape rectmagenta_l(Vector2f(50, 50));
    rectmagenta_l.setPosition(40, 80);
    rectmagenta_l.setTexture(&texture);

    RectangleShape rectwhite_F(Vector2f(50, 50));
    rectwhite_F.setPosition(50, 550);
    rectwhite_F.setTexture(&texture);

    RectangleShape level_M(Vector2f(50, 50));
    level_M.setPosition(730, 270);
    level_M.setTexture(&texture);

    RectangleShape rectCyan_Goal(Vector2f(50.f, 20.f));
    rectCyan_Goal.setPosition(450.f, 280.f);
    rectCyan_Goal.setTexture(&texture);

    RectangleShape rectGoal_Level7(Vector2f(40.f, 40.f));
    rectGoal_Level7.setPosition(345.f, 295.f);
    rectGoal_Level7.setTexture(&texture);

    RectangleShape rectGoal_Level8(Vector2f(45.f, 45.f));
    rectGoal_Level8.setPosition(720.f, 480.f);
    rectGoal_Level8.setTexture(&texture);

    RectangleShape rectGoal_Level9(Vector2f(45.f, 45.f));
    rectGoal_Level9.setPosition(40.f, 535.f);
    rectGoal_Level9.setTexture(&texture);

    RectangleShape rectGoal_Level10(Vector2f(40.f, 40.f));
    rectGoal_Level10.setPosition(735.f, 543.f);
    rectGoal_Level10.setTexture(&texture);



    auto applyThemeColors = [&]() {
        rectbleu.setFillColor(easyLevel2Color); rectbleu_a.setFillColor(easyLevel2Color); rectbleu_z.setFillColor(easyLevel2Color); rectbleu_e.setFillColor(easyLevel2Color); rectbleu_r.setFillColor(easyLevel2Color); rectbleu_t.setFillColor(easyLevel2Color); rectbleu_y.setFillColor(easyLevel2Color); rectbleu_u.setFillColor(easyLevel2Color); rectbleu_i.setFillColor(easyLevel2Color); rectbleu_o.setFillColor(easyLevel2Color); rectbleu_p.setFillColor(easyLevel2Color); rectbleu_q.setFillColor(easyLevel2Color); rectbleu_s.setFillColor(easyLevel2Color); rectbleu_d.setFillColor(easyLevel2Color); rectbleu_f.setFillColor(easyLevel2Color); rectbleu_g.setFillColor(easyLevel2Color); rectbleu_h.setFillColor(easyLevel2Color); rectbleu_j.setFillColor(easyLevel2Color); rectbleu_k.setFillColor(easyLevel2Color); rectbleu_l.setFillColor(easyLevel2Color); rectbleu_m.setFillColor(easyLevel2Color);
        rectmagenta.setFillColor(easyLevel3Color); rectmagenta_a.setFillColor(easyLevel3Color); rectmagenta_z.setFillColor(easyLevel3Color); rectmagenta_e.setFillColor(easyLevel3Color); rectmagenta_r.setFillColor(easyLevel3Color); rectmagenta_t.setFillColor(easyLevel3Color); rectmagenta_y.setFillColor(easyLevel3Color); rectmagenta_u.setFillColor(easyLevel3Color); rectmagenta_i.setFillColor(easyLevel3Color); rectmagenta_o.setFillColor(easyLevel3Color); rectmagenta_p.setFillColor(easyLevel3Color); rectmagenta_q.setFillColor(easyLevel3Color); rectmagenta_s.setFillColor(easyLevel3Color); rectmagenta_d.setFillColor(easyLevel3Color); rectmagenta_f.setFillColor(easyLevel3Color); rectmagenta_g.setFillColor(easyLevel3Color); rectmagenta_h.setFillColor(easyLevel3Color); rectmagenta_j.setFillColor(easyLevel3Color); rectmagenta_k.setFillColor(easyLevel3Color);
        rectwhite.setFillColor(easyLevel4Color); rectwhite_a.setFillColor(easyLevel4Color); rectwhite_z.setFillColor(easyLevel4Color); rectwhite_e.setFillColor(easyLevel4Color); rectwhite_r.setFillColor(easyLevel4Color); rectwhite_t.setFillColor(easyLevel4Color); rectwhite_y.setFillColor(easyLevel4Color); rectwhite_u.setFillColor(easyLevel4Color); rectwhite_i.setFillColor(easyLevel4Color); rectwhite_o.setFillColor(easyLevel4Color); rectwhite_p.setFillColor(easyLevel4Color); rectwhite_q.setFillColor(easyLevel4Color); rectwhite_s.setFillColor(easyLevel4Color); rectwhite_d.setFillColor(easyLevel4Color); rectwhite_f.setFillColor(easyLevel4Color); rectwhite_g.setFillColor(easyLevel4Color); rectwhite_h.setFillColor(easyLevel4Color); rectwhite_j.setFillColor(easyLevel4Color); rectwhite_k.setFillColor(easyLevel4Color); rectwhite_l.setFillColor(easyLevel4Color); rectwhite_m.setFillColor(easyLevel4Color); rectwhite_w.setFillColor(easyLevel4Color); rectwhite_x.setFillColor(easyLevel4Color); rectwhite_c.setFillColor(easyLevel4Color); rectwhite_v.setFillColor(easyLevel4Color); rectwhite_b.setFillColor(easyLevel4Color); rectwhite_n.setFillColor(easyLevel4Color); rectwhite_A.setFillColor(easyLevel4Color); rectwhite_E.setFillColor(easyLevel4Color); rectwhite_Z.setFillColor(easyLevel4Color); rectwhite_R.setFillColor(easyLevel4Color); rectwhite_T.setFillColor(easyLevel4Color); rectwhite_Y.setFillColor(easyLevel4Color); rectwhite_U.setFillColor(easyLevel4Color); rectwhite_I.setFillColor(easyLevel4Color); rectwhite_O.setFillColor(easyLevel4Color); rectwhite_P.setFillColor(easyLevel4Color); rectwhite_Q.setFillColor(easyLevel4Color); rectwhite_S.setFillColor(easyLevel4Color); rectwhite_D.setFillColor(easyLevel4Color);
    };

    // ================= LOOP =================
    while (window.isOpen())
    {
        Vector2i pixelPos = Mouse::getPosition(window);
        Vector2f mouse = window.mapPixelToCoords(pixelPos);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            // إدخال لوحة المفاتيح
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Space)
                {
                    handleBackNavigation();
                }
            }
            // إذا ضغط على زر الحفظ
            if (menuSaveButton.getGlobalBounds().contains(mouse)) {
                saveGame(level, selectedDifficulty);
            }

            // إذا ضغط على زر التحميل
            if (menuLoadButton.getGlobalBounds().contains(mouse)) {
                if (loadGame(level, selectedDifficulty)) {
                    resetPlayerPosition();
                    state = GAMEPLAY;
                    levelTimer.restart(); // جديد: إعادة تشغيل الوقت بعد التحميل
                }
            }

            // MOUSE INPUT: زر العودة الجديد أثناء اللعب
            if (event.type == Event::MouseButtonPressed)
            {
                if (state == GAMEPLAY)
                {
                    if (gameplayBackButton.getGlobalBounds().contains(mouse))
                    {
                        handleBackNavigation();
                    }
                }
            }
        }

        if (!window.isOpen())
            break;

        // تفاعل تأثير تمرير الماوس فوق زر العودة
        if (state == GAMEPLAY) {
            if (gameplayBackButton.getGlobalBounds().contains(mouse)) gameplayBackButton.setFillColor(Color::Yellow);
            else gameplayBackButton.setFillColor(Color::White);
        }

        // ================= GAMEPLAY UPDATE =================
        Vector2f movement(0.f, 0.f);
        Vector2f oldPos = player.getPosition();

        // حركة اللاعب باللوحة
        if (Keyboard::isKeyPressed(Keyboard::Up))    movement.y -= playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Down))  movement.y += playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Left))  movement.x -= playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Right)) movement.x += playerSpeed;

        // حركة اللاعب بالأزرار اللمسية
        if (state == GAMEPLAY && Mouse::isButtonPressed(Mouse::Left))
        {
            if (upBtn.getGlobalBounds().contains(mouse))    movement.y -= playerSpeed;
            if (downBtn.getGlobalBounds().contains(mouse))  movement.y += playerSpeed;
            if (leftBtn.getGlobalBounds().contains(mouse))  movement.x -= playerSpeed;
            if (rightBtn.getGlobalBounds().contains(mouse)) movement.x += playerSpeed;
        }

        player.move(movement);

        // ===== فحص انتهاء الوقت لأي مستوى (مشترك بين كل المستويات، ما عدا شاشة الفوز) =====
        float elapsedTime = levelTimer.getElapsedTime().asSeconds();
        if (level != 11 && elapsedTime >= levelTimeLimits[level])
        {
            restartCurrentLevel();
        }

        // ===== انتهاء تأثير الإبطاء (كي يخلص الوقت المخصص للفخ) =====
        if (isSlowed && slowClock.getElapsedTime().asSeconds() >= slowDuration)
        {
            isSlowed = false;
            playerSpeed = normalSpeed;
        }

        // ===== فحص لمس الفخاخ حسب المستوى الحالي =====
        if (level == 1)      checkTraps(trapsLevel1);
        else if (level == 2) checkTraps(trapsLevel2);
        else if (level == 3) checkTraps(trapsLevel3);
        else if (level == 4) checkTraps(trapsLevel4);
        else if (level == 5) checkTraps(trapsLevel5);
        else if (level == 6) checkTraps(trapsLevel6);
        else if (level == 7) checkTraps(trapsLevel7);
        else if (level == 8) checkTraps(trapsLevel8);
        else if (level == 9) checkTraps(trapsLevel9);
        else if (level == 10) checkTraps(trapsLevel10);

        if (level == 1)      checkCoins(coinsLevel1);
        else if (level == 2) checkCoins(coinsLevel2);
        else if (level == 3) checkCoins(coinsLevel3);
        else if (level == 4) checkCoins(coinsLevel4);
        else if (level == 5) checkCoins(coinsLevel5);
        else if (level == 6) checkCoins(coinsLevel6);
        else if (level == 7) checkCoins(coinsLevel7);
        else if (level == 8) checkCoins(coinsLevel8);
        else if (level == 9) checkCoins(coinsLevel9);
        else if (level == 10) checkCoins(coinsLevel10);

        // 1. فحص التصادم للمستوى الأول
        if (level == 1)
        {
            for (const auto& wall : wallsLevel1)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(rectYellow_P.getGlobalBounds()) && countCollectedCoins(coinsLevel1) >= coinsRequired)
            {
                doorSound.play();
                level = 2;
                player.setPosition(20.f, 20.f);
                player.setFillColor(Color::Red);
                levelTimer.restart(); // جديد
            }
        }
        // 2. فحص التصادم للمستوى الثاني
        else if (level == 2)
        {
            if (player.getGlobalBounds().intersects(rectbleu.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_z.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_e.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_r.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_t.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_y.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_u.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_i.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_o.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_p.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_s.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_d.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_f.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_g.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_h.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_j.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_k.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_l.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectbleu_m.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }

            if (player.getGlobalBounds().intersects(rectbleu_w.getGlobalBounds()) && countCollectedCoins(coinsLevel2) >= coinsRequired) {
                doorSound.play();
                level = 3;
                player.setPosition(30.f, 50.f);
                player.setFillColor(Color::Magenta);
                levelTimer.restart(); // جديد
            }
        }
        // 3. فحص التصادم للمستوى الثالث
        else if (level == 3)
        {
            if (player.getGlobalBounds().intersects(rectmagenta.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_z.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_e.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_r.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_t.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_y.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_u.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_i.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_o.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_p.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_s.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_d.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_f.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_g.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_h.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_j.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectmagenta_k.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }
            for (const auto& wall : wallsLevel3Easy)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds())) {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(rectmagenta_l.getGlobalBounds()) && countCollectedCoins(coinsLevel3) >= coinsRequired) {
                doorSound.play();
                level = 4;
                player.setPosition(50.f, 40.f);
                player.setFillColor(Color::Yellow);
                levelTimer.restart(); // جديد
            }
        }
        // 4. فحص التصادم للمستوى الرابع
        else if (level == 4)
        {
            if (player.getGlobalBounds().intersects(rectwhite.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_z.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_e.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_r.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_t.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_y.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_u.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_i.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_o.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_p.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_d.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_s.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_f.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_g.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_h.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_j.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_k.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_l.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_m.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_w.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_x.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_c.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_v.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_b.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_n.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_A.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_E.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_Z.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_R.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_T.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_Y.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_U.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_I.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_O.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_P.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_Q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(rectwhite_S.getGlobalBounds()) ||player.getGlobalBounds().intersects(rectwhite_D.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }

            if (player.getGlobalBounds().intersects(rectwhite_F.getGlobalBounds()) && countCollectedCoins(coinsLevel4) >= coinsRequired) {
                doorSound.play();
                level = 5;
                player.setPosition(50.f, 65.f);
                player.setFillColor(Color::Red);
                levelTimer.restart(); // جديد
            }
        }
        // 5. فحص التصادم للمستوى الخامس
        else if (level == 5) {
            if (player.getGlobalBounds().intersects(level_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_z.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_e.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_r.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_t.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_y.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_u.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_i.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_o.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_p.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_s.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_d.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_f.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_g.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_h.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_j.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_k.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_l.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_m.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_w.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_x.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_c.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_v.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_n.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_A.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_Z.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_E.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_R.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_T.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_Y.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_U.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_I.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_O.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_P.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_Q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_S.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_D.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_F.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_G.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_H.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_J.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_K.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(level_L.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }
             if (player.getGlobalBounds().intersects(level_M.getGlobalBounds()) && countCollectedCoins(coinsLevel5) >= coinsRequired) {
                doorSound.play();
                level = 6;
                player.setPosition(390.f, 40.f);
                player.setFillColor(Color::Red);
                levelTimer.restart(); // جديد
            }
        }
        // 6. فحص التصادم للمستوى السادس (المتاهة الحلزونية)
        else if (level == 6)
        {
            if (player.getGlobalBounds().intersects(lvl6_f0_top.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl6_f0_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f0_left.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl6_f0_right.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f1_top.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl6_f1_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f1_left.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl6_f1_right_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f1_right_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f2_top.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl6_f2_left.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl6_f2_right.getGlobalBounds())  ||
                player.getGlobalBounds().intersects(lvl6_f2_bottom_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f2_bottom_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f3_top.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl6_f3_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f3_right.getGlobalBounds())  ||
                player.getGlobalBounds().intersects(lvl6_f3_left_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f3_left_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f4_top_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f4_top_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f4_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl6_f4_left.getGlobalBounds())  ||
                player.getGlobalBounds().intersects(lvl6_f4_right.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }
            // فحص وصول اللاعب لهدف المستوى السادس (تصحيح: استعمال rectCyan_Goal الصحيح)
            if (player.getGlobalBounds().intersects(rectCyan_Goal.getGlobalBounds()) && countCollectedCoins(coinsLevel6) >= coinsRequired) {
                doorSound.play();
                level = 7;
                player.setPosition(390.f, 40.f);
                player.setFillColor(Color::Cyan);
                levelTimer.restart(); // جديد
            }
        }
        // 7. فحص التصادم للمستوى السابع (متاهة حلزونية مختلفة الشكل)
        else if (level == 7)
        {
            if (player.getGlobalBounds().intersects(lvl7_f0_top.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl7_f0_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f0_left.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl7_f0_right.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f1_left.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl7_f1_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f1_right.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl7_f1_top_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f1_top_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f2_top.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl7_f2_right.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl7_f2_bottom.getGlobalBounds())  ||
                player.getGlobalBounds().intersects(lvl7_f2_left_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f2_left_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f3_left.getGlobalBounds())    ||
                player.getGlobalBounds().intersects(lvl7_f3_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f3_right.getGlobalBounds())   ||
                player.getGlobalBounds().intersects(lvl7_f3_top_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f4_top.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f4_left.getGlobalBounds())  ||
                player.getGlobalBounds().intersects(lvl7_f4_right.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f4_bottom_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f5_top.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f5_bottom.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl7_f5_right.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }

            if (player.getGlobalBounds().intersects(rectGoal_Level7.getGlobalBounds()) && countCollectedCoins(coinsLevel7) >= coinsRequired)
            {
                doorSound.play();
                level = 8;
                player.setPosition(50.f, 65.f);
                player.setFillColor(Color::Yellow);
                levelTimer.restart();
            }
        }
        // 8. فحص التصادم للمستوى الثامن (متاهة كلاسيكية جديدة)
        else if (level == 8)
        {
            if (player.getGlobalBounds().intersects(lvl8_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_c.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_d.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_e.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_f.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_g.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_h.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_i.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_j.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_k.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_l.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_m.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_n.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_o.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_p.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_q.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_r.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_s.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_t.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl8_u.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }

            if (player.getGlobalBounds().intersects(rectGoal_Level8.getGlobalBounds()) && countCollectedCoins(coinsLevel8) >= coinsRequired)
            {
                doorSound.play();
                level = 9;
                player.setPosition(390.f, 40.f);
                player.setFillColor(Color::Blue);
                levelTimer.restart();
            }
        }
        // 9. فحص التصادم للمستوى التاسع (متاهة متعرجة Zigzag)
        // 9. فحص التصادم للمستوى التاسع (متاهة متعرجة Zigzag)
        else if (level == 9)
        {
            if (player.getGlobalBounds().intersects(lvl9_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_c.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_d.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar1.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar2.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar3.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar4.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar5.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar6.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl9_bar7.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }

            if (player.getGlobalBounds().intersects(rectGoal_Level9.getGlobalBounds()) && countCollectedCoins(coinsLevel9) >= coinsRequired)
            {
                doorSound.play();
                level = 10;
                player.setPosition(390.f, 40.f);
                player.setFillColor(Color::White);
                levelTimer.restart();
            }
        }
        // 10. فحص التصادم للمستوى العاشر (المستوى الأخير الصعب جداً)
        else if (level == 10)
        {
            if (player.getGlobalBounds().intersects(lvl10_a.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_b.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_c.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_d.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar1.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar2.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar3.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar4.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar5.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar6.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar7.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar8.getGlobalBounds()) ||
                player.getGlobalBounds().intersects(lvl10_bar9.getGlobalBounds()))
            {
                player.setPosition(oldPos);
            }

            // عند الفوز بالمستوى العاشر ننتقل للمستوى 11 (شاشة الفوز النهائية)
            if (player.getGlobalBounds().intersects(rectGoal_Level10.getGlobalBounds()) && countCollectedCoins(coinsLevel10) >= coinsRequired)
            {
                doorSound.play();
                level = 11;
                player.setPosition(390.f, 40.f);
                player.setFillColor(Color::Green);
                levelTimer.restart();
            }
        }

        // ================= DRAWING GAMEPLAY =================
        window.clear(Color::Black);

        if (state == GAMEPLAY)
        {
            // رسم الجدران والأهداف والعملات والفخاخ بناءً على المستوى الحالي
            if (level == 1)
            {
                for (const auto& wall : wallsLevel1) window.draw(wall);
                window.draw(rectYellow_P);
                drawCoins(coinsLevel1);
                for (const auto& trap : trapsLevel1) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 2)
            {
                window.draw(rectbleu); window.draw(rectbleu_a); window.draw(rectbleu_z); window.draw(rectbleu_e);
                window.draw(rectbleu_r); window.draw(rectbleu_t); window.draw(rectbleu_y); window.draw(rectbleu_u);
                window.draw(rectbleu_i); window.draw(rectbleu_o); window.draw(rectbleu_p); window.draw(rectbleu_q);
                window.draw(rectbleu_s); window.draw(rectbleu_d); window.draw(rectbleu_f); window.draw(rectbleu_g);
                window.draw(rectbleu_h); window.draw(rectbleu_j); window.draw(rectbleu_k); window.draw(rectbleu_l);
                window.draw(rectbleu_m);
                window.draw(rectbleu_w);
                drawCoins(coinsLevel2);
                for (const auto& trap : trapsLevel2) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 3)
            {
                window.draw(rectmagenta); window.draw(rectmagenta_a); window.draw(rectmagenta_z); window.draw(rectmagenta_e);
                window.draw(rectmagenta_r); window.draw(rectmagenta_t); window.draw(rectmagenta_y); window.draw(rectmagenta_u);
                window.draw(rectmagenta_i); window.draw(rectmagenta_o); window.draw(rectmagenta_p); window.draw(rectmagenta_q);
                window.draw(rectmagenta_s); window.draw(rectmagenta_d); window.draw(rectmagenta_f); window.draw(rectmagenta_g);
                window.draw(rectmagenta_h); window.draw(rectmagenta_j); window.draw(rectmagenta_k);
                for (const auto& wall : wallsLevel3Easy) window.draw(wall);
                window.draw(rectmagenta_l);
                drawCoins(coinsLevel3);
                for (const auto& trap : trapsLevel3) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 4)
            {
                window.draw(rectwhite); window.draw(rectwhite_a); window.draw(rectwhite_z); window.draw(rectwhite_e);
                window.draw(rectwhite_r); window.draw(rectwhite_t); window.draw(rectwhite_y); window.draw(rectwhite_u);
                window.draw(rectwhite_i); window.draw(rectwhite_o); window.draw(rectwhite_p); window.draw(rectwhite_q);
                window.draw(rectwhite_d); window.draw(rectwhite_s); window.draw(rectwhite_f); window.draw(rectwhite_g);
                window.draw(rectwhite_h); window.draw(rectwhite_j); window.draw(rectwhite_k); window.draw(rectwhite_l);
                window.draw(rectwhite_m); window.draw(rectwhite_w); window.draw(rectwhite_x); window.draw(rectwhite_c);
                window.draw(rectwhite_v); window.draw(rectwhite_b); window.draw(rectwhite_n); window.draw(rectwhite_A);
                window.draw(rectwhite_E); window.draw(rectwhite_Z); window.draw(rectwhite_R); window.draw(rectwhite_T);
                window.draw(rectwhite_Y); window.draw(rectwhite_U); window.draw(rectwhite_I); window.draw(rectwhite_O);
                window.draw(rectwhite_P); window.draw(rectwhite_Q); window.draw(rectwhite_S); window.draw(rectwhite_D);
                window.draw(rectwhite_F);
                drawCoins(coinsLevel4);
                for (const auto& trap : trapsLevel4) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 5)
            {
                window.draw(level_a); window.draw(level_z); window.draw(level_e); window.draw(level_r); window.draw(level_t);
                window.draw(level_y); window.draw(level_u); window.draw(level_i); window.draw(level_o); window.draw(level_p);
                window.draw(level_q); window.draw(level_s); window.draw(level_d); window.draw(level_f); window.draw(level_g);
                window.draw(level_h); window.draw(level_j); window.draw(level_k); window.draw(level_l); window.draw(level_m);
                window.draw(level_w); window.draw(level_x); window.draw(level_c); window.draw(level_v); window.draw(level_b);
                window.draw(level_n); window.draw(level_A); window.draw(level_Z); window.draw(level_E); window.draw(level_R);
                window.draw(level_T); window.draw(level_Y); window.draw(level_U); window.draw(level_I); window.draw(level_O);
                window.draw(level_P); window.draw(level_Q); window.draw(level_S); window.draw(level_D); window.draw(level_F);
                window.draw(level_G); window.draw(level_H); window.draw(level_J); window.draw(level_K); window.draw(level_L);
                window.draw(level_M);
                drawCoins(coinsLevel5);
                for (const auto& trap : trapsLevel5) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 6)
            {
                window.draw(lvl6_f0_top); window.draw(lvl6_f0_bottom); window.draw(lvl6_f0_left); window.draw(lvl6_f0_right);
                window.draw(lvl6_f1_top); window.draw(lvl6_f1_bottom); window.draw(lvl6_f1_left);
                window.draw(lvl6_f1_right_a); window.draw(lvl6_f1_right_b);
                window.draw(lvl6_f2_top); window.draw(lvl6_f2_left); window.draw(lvl6_f2_right);
                window.draw(lvl6_f2_bottom_a); window.draw(lvl6_f2_bottom_b);
                window.draw(lvl6_f3_top); window.draw(lvl6_f3_bottom); window.draw(lvl6_f3_right);
                window.draw(lvl6_f3_left_a); window.draw(lvl6_f3_left_b);
                window.draw(lvl6_f4_top_a); window.draw(lvl6_f4_top_b); window.draw(lvl6_f4_bottom);
                window.draw(lvl6_f4_left); window.draw(lvl6_f4_right);
                window.draw(rectCyan_Goal);
                drawCoins(coinsLevel6);
                for (const auto& trap : trapsLevel6) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 7)
            {
                window.draw(lvl7_f0_top); window.draw(lvl7_f0_bottom); window.draw(lvl7_f0_left); window.draw(lvl7_f0_right);
                window.draw(lvl7_f1_left); window.draw(lvl7_f1_bottom); window.draw(lvl7_f1_right);
                window.draw(lvl7_f1_top_a); window.draw(lvl7_f1_top_b);
                window.draw(lvl7_f2_top); window.draw(lvl7_f2_right); window.draw(lvl7_f2_bottom);
                window.draw(lvl7_f2_left_a); window.draw(lvl7_f2_left_b);
                window.draw(lvl7_f3_left); window.draw(lvl7_f3_bottom); window.draw(lvl7_f3_right);
                window.draw(lvl7_f3_top_a);
                window.draw(lvl7_f4_top); window.draw(lvl7_f4_left); window.draw(lvl7_f4_right);
                window.draw(lvl7_f4_bottom_a);
                window.draw(lvl7_f5_top); window.draw(lvl7_f5_bottom); window.draw(lvl7_f5_right);
                window.draw(rectGoal_Level7);
                drawCoins(coinsLevel7);
                for (const auto& trap : trapsLevel7) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 8)
            {
                window.draw(lvl8_a); window.draw(lvl8_b); window.draw(lvl8_c); window.draw(lvl8_d);
                window.draw(lvl8_e); window.draw(lvl8_f); window.draw(lvl8_g); window.draw(lvl8_h);
                window.draw(lvl8_i); window.draw(lvl8_j); window.draw(lvl8_k); window.draw(lvl8_l);
                window.draw(lvl8_m); window.draw(lvl8_n); window.draw(lvl8_o); window.draw(lvl8_p);
                window.draw(lvl8_q); window.draw(lvl8_r); window.draw(lvl8_s); window.draw(lvl8_t);
                window.draw(lvl8_u);
                window.draw(rectGoal_Level8);
                drawCoins(coinsLevel8);
                for (const auto& trap : trapsLevel8) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 9)
            {
                window.draw(lvl9_a); window.draw(lvl9_b); window.draw(lvl9_c); window.draw(lvl9_d);
                window.draw(lvl9_bar1); window.draw(lvl9_bar2); window.draw(lvl9_bar3); window.draw(lvl9_bar4);
                window.draw(lvl9_bar5); window.draw(lvl9_bar6); window.draw(lvl9_bar7);
                window.draw(rectGoal_Level9);
                drawCoins(coinsLevel9);
                for (const auto& trap : trapsLevel9) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 10)
            {
                window.draw(lvl10_a); window.draw(lvl10_b); window.draw(lvl10_c); window.draw(lvl10_d);
                window.draw(lvl10_bar1); window.draw(lvl10_bar2); window.draw(lvl10_bar3); window.draw(lvl10_bar4);
                window.draw(lvl10_bar5); window.draw(lvl10_bar6); window.draw(lvl10_bar7); window.draw(lvl10_bar8);
                window.draw(lvl10_bar9);
                window.draw(rectGoal_Level10);
                drawCoins(coinsLevel10);
                for (const auto& trap : trapsLevel10) {
                    if (trap.active && trap.revealed) window.draw(trap.shape);
                }
            }
            else if (level == 11) // شاشة الفوز النهائية
            {
                Text winText;
                winText.setFont(font);
                winText.setString("CONGRATULATIONS! YOU WON!");
                winText.setCharacterSize(40);
                winText.setFillColor(Color::Green);
                winText.setPosition(100.f, 250.f);
                window.draw(winText);
            }

            // رسم اللاعب
            window.draw(player);

            // رسم أزرار التحكم باللمس
            window.draw(upBtn); window.draw(upTxt);
            window.draw(downBtn); window.draw(downTxt);
            window.draw(leftBtn); window.draw(leftTxt);
            window.draw(rightBtn); window.draw(rightTxt);

            // رسم النصوص والمعلومات (الوقت، النقاط، زر الحفظ والتحميل)
            if (level != 11)
            {
                int currentCoins = 0;
                if (level == 1) currentCoins = countCollectedCoins(coinsLevel1);
                else if (level == 2) currentCoins = countCollectedCoins(coinsLevel2);
                else if (level == 3) currentCoins = countCollectedCoins(coinsLevel3);
                else if (level == 4) currentCoins = countCollectedCoins(coinsLevel4);
                else if (level == 5) currentCoins = countCollectedCoins(coinsLevel5);
                else if (level == 6) currentCoins = countCollectedCoins(coinsLevel6);
                else if (level == 7) currentCoins = countCollectedCoins(coinsLevel7);
                else if (level == 8) currentCoins = countCollectedCoins(coinsLevel8);
                else if (level == 9) currentCoins = countCollectedCoins(coinsLevel9);
                else if (level == 10) currentCoins = countCollectedCoins(coinsLevel10);

                // تحديث نص الوقت المتبقي
                float timeLeft = levelTimeLimits[level] - elapsedTime;
                if (timeLeft < 0.f) timeLeft = 0.f;
                timerText.setString("Time: " + std::to_string((int)timeLeft) + "s");
                window.draw(timerText);

                // تحديث النقاط والعملات المجموعة
                scoreText.setString("Score: " + std::to_string(score) + " | Coins: " + std::to_string(currentCoins) + "/5");
                window.draw(scoreText);

                // رسم أزرار حفظ وتحميل اللعبة في شاشة اللعب
                window.draw(menuSaveButton);
                window.draw(menuSaveText);
                window.draw(menuLoadButton);
                window.draw(menuLoadText);
            }

            // زر العودة
            window.draw(gameplayBackButton);
        }

        window.display();
    }
}

// ===================================================================
// ================= MIDDLE: متاهة شبكية كثيفة (Grid Maze) ===========
// ===================================================================
void Middle() {
    RenderWindow window(VideoMode(1000, 800), "Simple Game");
    window.setFramerateLimit(60);

    GameState state = GAMEPLAY;
    int level = 1;
    float playerSpeed = 7.f;

    // ===================== نظام الفخاخ (Traps) =====================
    float normalSpeed = 7.f;   // السرعة العادية
    float slowSpeed = 2.5f;    // السرعة كي يلمس اللاعب فخ
    float slowDuration = 3.f;  // مدة الإبطاء بالثواني - بدّلها كيفما بغيتي
    bool isSlowed = false;
    Clock slowClock;
    float trapRevealDistance = 90.f; // المسافة اللي كيبان فيها الفخ - بدّلها كيفما بغيتي
    // ==================================================================

    Font font;
    if (!font.loadFromFile("Hibo.otf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    // ===================== صوت فتح الباب (door open sound) =====================
    SoundBuffer doorSoundBuffer;
    Sound doorSound;
    if (!doorSoundBuffer.loadFromFile("mo.ogg"))
    {
        std::cout << "Error loading door sound!" << std::endl;
    }
    else
    {
        doorSound.setBuffer(doorSoundBuffer);
        doorSound.setVolume(80.f);
        doorSound.play(); // === تجربة مؤقتة: كيتشغل مباشرة عند بداية المستوى باش نتأكدو أن الصوت خدام - حيدها من بعد ===
    }
    // ==============================================================================

    // زر العودة الخاص باللعب
    Text gameplayBackButton;
    gameplayBackButton.setFont(font);
    gameplayBackButton.setString("< Back");
    gameplayBackButton.setCharacterSize(25);
    gameplayBackButton.setFillColor(Color::White);
    gameplayBackButton.setPosition(360.f, 15.f);

    // ================= PLAYER =================
    CircleShape player(15.f);
    player.setFillColor(Color::Blue);
    player.setPosition(46.f, 106.f);

    auto resetPlayerPosition = [&]() {
        if (level == 1) { player.setPosition(46.f, 106.f); player.setFillColor(Color::Blue); }
        else if (level == 2) { player.setPosition(480.f, 726.f); player.setFillColor(Color::Red); }
        else if (level == 3) { player.setPosition(46.f, 106.f); player.setFillColor(Color::Green); }
        else if (level == 4) { player.setPosition(46.f, 106.f); player.setFillColor(Color::Yellow); }
        else if (level == 5) { player.setPosition(46.f, 416.f); player.setFillColor(Color(255, 165, 0)); }
        else if (level == 6) { player.setPosition(46.f, 106.f); player.setFillColor(Color::Cyan); }
        else if (level == 7) { player.setPosition(46.f, 416.f); player.setFillColor(Color::Magenta); }
        else if (level == 8) { player.setPosition(46.f, 106.f); player.setFillColor(Color::White); }
        else if (level == 9) { player.setPosition(46.f, 416.f); player.setFillColor(Color::Red); }
        else if (level == 10) { player.setPosition(46.f, 106.f); player.setFillColor(Color::Green); }
    };

    // دالة العودة خطوة للخلف: من المستوى 10 نرجعو لـ 9، من 9 لـ 8، من 8 لـ 7، من 7 لـ 6، من 6 لـ 5، من 5 لـ 4، من 4 لـ 3، من 3 لـ 2، من 2 لـ 1، ومن 1 نسدو النافذة
    auto handleBackNavigation = [&]() {
        if (level == 1) {
            window.close();
        }
        else if (level == 2) {
            level = 1;
            resetPlayerPosition();
        }
        else if (level == 3) {
            level = 2;
            resetPlayerPosition();
        }
        else if (level == 4) {
            level = 3;
            resetPlayerPosition();
        }
        else if (level == 5) {
            level = 4;
            resetPlayerPosition();
        }
        else if (level == 6) {
            level = 5;
            resetPlayerPosition();
        }
        else if (level == 7) {
            level = 6;
            resetPlayerPosition();
        }
        else if (level == 8) {
            level = 7;
            resetPlayerPosition();
        }
        else if (level == 9) {
            level = 8;
            resetPlayerPosition();
        }
        else if (level == 10) {
            level = 9;
            resetPlayerPosition();
        }
    };

    // ================= TOUCH CONTROLS =================
    float btnSize = 50.f;
    Color btnColor(255, 255, 255, 130);

    RectangleShape upBtn(Vector2f(btnSize, btnSize));
    upBtn.setPosition(900.f, 610.f);
    upBtn.setFillColor(btnColor);

    RectangleShape downBtn(Vector2f(btnSize, btnSize));
    downBtn.setPosition(900.f, 710.f);
    downBtn.setFillColor(btnColor);

    RectangleShape leftBtn(Vector2f(btnSize, btnSize));
    leftBtn.setPosition(40.f, 660.f);
    leftBtn.setFillColor(btnColor);

    RectangleShape rightBtn(Vector2f(btnSize, btnSize));
    rightBtn.setPosition(160.f, 660.f);
    rightBtn.setFillColor(btnColor);

    Text upTxt; upTxt.setFont(font); upTxt.setString("^"); upTxt.setCharacterSize(25); upTxt.setFillColor(Color::Black); upTxt.setPosition(918.f, 615.f);
    Text downTxt; downTxt.setFont(font); downTxt.setString("v"); downTxt.setCharacterSize(20); downTxt.setFillColor(Color::Black); downTxt.setPosition(920.f, 720.f);
    Text leftTxt; leftTxt.setFont(font); leftTxt.setString("<"); leftTxt.setCharacterSize(25); leftTxt.setFillColor(Color::Black); leftTxt.setPosition(55.f, 670.f);
    Text rightTxt; rightTxt.setFont(font); rightTxt.setString(">"); rightTxt.setCharacterSize(25); rightTxt.setFillColor(Color::Black); rightTxt.setPosition(175.f, 670.f);

    // ===================== نظام الوقت (Timer) لكل مستوى =====================
    Clock levelTimer;
    Clock animClock; // ساعة مخصصة للأنيميشن (نبضان العملات، توهج الباب)
    float levelTimeLimits[11] = { 0.f, 60.f, 60.f, 70.f, 75.f, 80.f, 85.f, 90.f, 95.f, 100.f, 105.f }; // index 0 غير مستعمل, 1..10 = المستويات

    Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(26);
    timerText.setFillColor(Color::White);
    timerText.setPosition(20.f, 15.f);
    // ================================================================

    auto addWall = [&](std::vector<RectangleShape>& wallVec, Vector2f size, Vector2f pos, Color color) {
        RectangleShape wall(size);
        wall.setPosition(pos);
        wall.setFillColor(color);
        wallVec.push_back(wall);
    };

    auto addTrap = [&](std::vector<Trap>& trapVec, Vector2f pos, Vector2f size = Vector2f(24.f, 50.f)) {
        Trap trap;
        trap.shape.setSize(size);
        trap.shape.setPosition(pos);
        trap.shape.setFillColor(Color(255, 140, 0)); // برتقالي = فخ
        trap.revealed = false;
        trap.active = true;
        trapVec.push_back(trap);
    };

    auto checkTraps = [&](std::vector<Trap>& traps) {
        Vector2f playerCenter = player.getPosition() + Vector2f(player.getRadius(), player.getRadius());
        for (auto& trap : traps)
        {
            if (!trap.active) continue;
            Vector2f trapCenter = trap.shape.getPosition() + trap.shape.getSize() / 2.f;
            float dx = playerCenter.x - trapCenter.x;
            float dy = playerCenter.y - trapCenter.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            trap.revealed = (distance <= trapRevealDistance);

            if (player.getGlobalBounds().intersects(trap.shape.getGlobalBounds()))
            {
                if (!isSlowed)
                {
                    isSlowed = true;
                    playerSpeed = slowSpeed;
                    slowClock.restart();
                }
                trap.active = false;
            }
        }
    };

    auto drawTraps = [&](std::vector<Trap>& traps) {
        for (const auto& t : traps)
        {
            if (t.active && t.revealed)
                window.draw(t.shape);
        }
    };

    // ===================== نظام العملات (Coins) =====================
    // خاص اللاعب يجمع 5 عملات فكل مستوى قبل ما يفتح الباب (الهدف)
    // كل عملة = +100 نقطة
    int score = 0;
    const int coinsRequired = 5;

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(Color::Yellow);
    scoreText.setPosition(20.f, 45.f);

    auto addCoin = [&](std::vector<Coin>& coinVec, Vector2f pos) {
        Coin c;
        c.shape.setRadius(10.f);
        c.shape.setOrigin(10.f, 10.f); // نخلي نقطة الأصل فمركز الدائرة باش النبضان يكون متمركز
        c.shape.setFillColor(Color::Yellow);
        c.shape.setOutlineColor(Color(150, 100, 0));
        c.shape.setOutlineThickness(2.f);
        c.shape.setPosition(pos); // pos هي المركز مباشرة دابا
        c.collected = false;
        coinVec.push_back(c);
    };

    auto checkCoins = [&](std::vector<Coin>& coins) {
        for (auto& c : coins)
        {
            if (!c.collected && player.getGlobalBounds().intersects(c.shape.getGlobalBounds()))
            {
                c.collected = true;
                score += 100;
            }
        }
    };

    auto countCollectedCoins = [&](std::vector<Coin>& coins) -> int {
        int n = 0;
        for (const auto& c : coins) if (c.collected) n++;
        return n;
    };

    auto drawCoins = [&](std::vector<Coin>& coins) {
        // نبضان بسيط: العملة كتكبر وتصغر بشوية (Pulse Animation) باش تبان حية
        float pulse = 1.f + 0.18f * std::sin(animClock.getElapsedTime().asSeconds() * 4.f);
        for (auto& c : coins)
        {
            if (!c.collected)
            {
                c.shape.setScale(pulse, pulse);
                c.shape.rotate(0.6f); // دوران خفيف مستمر
                window.draw(c.shape);
            }
        }
    };
    // ===================================================================

    // ================= MIDDLE LEVEL 1: DENSE GRID MAZE =================
    // متاهة شبكية كثيفة (14x11 خانة) - المدخل أعلى اليسار، المخرج أسفل اليمين
    Color middleLevel1Color = Color::Cyan;
    std::vector<RectangleShape> wallsMiddleLevel1;

    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(26, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(88, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(398, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(460, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(584, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(770, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(832, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 86), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(88, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(150, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(212, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(336, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(460, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(460, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(646, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(770, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(770, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 148), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(26, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(150, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(274, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(522, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(770, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(832, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(832, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 210), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(212, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(336, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(460, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(460, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(522, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(584, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(708, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(770, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(832, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 272), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(88, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(150, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(274, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(336, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(522, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(584, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(646, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(708, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 334), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(150, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(274, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(460, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(522, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(646, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(770, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(832, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 396), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(150, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(212, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(460, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(522, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(584, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(708, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(832, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 458), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(150, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(336, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(460, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(522, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(708, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(770, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(770, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 520), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(88, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(212, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(398, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(460, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(770, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(832, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(832, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 582), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(88, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(274, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(398, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(460, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(460, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(646, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(770, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 644), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(26, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(26, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(88, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(88, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(150, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(212, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(274, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(336, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(336, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(398, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(398, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(460, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(522, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(584, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(584, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(646, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(708, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(708, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(770, 706), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(770, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(70, 8), Vector2f(832, 768), middleLevel1Color);
    addWall(wallsMiddleLevel1, Vector2f(8, 70), Vector2f(894, 706), middleLevel1Color);

    // هدف المستوى 1 (أسفل اليمين، عند فتحة المخرج)
    Texture middleGoalTexture;
    if (!middleGoalTexture.loadFromFile("mo.jpg"))
    {
        std::cout << "Error loading texture" << std::endl;
    }
    RectangleShape middleGoal1(Vector2f(40.f, 40.f));
    middleGoal1.setPosition(847.f, 721.f);
    middleGoal1.setTexture(&middleGoalTexture);

    // فخاخ المستوى 1
    std::vector<Trap> trapsMiddleLevel1;
    addTrap(trapsMiddleLevel1, Vector2f(545.f, 406.f));
    addTrap(trapsMiddleLevel1, Vector2f(669.f, 344.f));
    addTrap(trapsMiddleLevel1, Vector2f(793.f, 158.f));
    addTrap(trapsMiddleLevel1, Vector2f(483.f, 468.f));
    addTrap(trapsMiddleLevel1, Vector2f(111.f, 592.f));
    addTrap(trapsMiddleLevel1, Vector2f(483.f, 406.f));

    // عملات المستوى 1 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel1;
    addCoin(coinsMiddleLevel1, Vector2f(681.f, 617.f));
    addCoin(coinsMiddleLevel1, Vector2f(805.f, 617.f));
    addCoin(coinsMiddleLevel1, Vector2f(619.f, 679.f));
    addCoin(coinsMiddleLevel1, Vector2f(433.f, 307.f));
    addCoin(coinsMiddleLevel1, Vector2f(247.f, 617.f));

    // ================= MIDDLE LEVEL 2: متاهة بمدخل أسفل ومخرج أعلى اليمين =================
    // (مستوحاة من صورة: طفلة وطفل عند المدخل السفلي، والهدف (شوكولا + مال) أعلى اليمين)
    Color middleLevel2Color = Color::Magenta;
    std::vector<RectangleShape> wallsMiddleLevel2;

    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(26, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(88, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(336, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(770, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(832, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 86), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(150, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(274, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(398, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(770, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 148), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(212, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(398, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(460, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(770, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 210), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(150, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(274, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(336, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(460, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(522, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(646, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 272), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(26, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(88, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(212, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(460, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(522, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(584, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(770, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(770, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 334), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(88, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(212, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(274, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(398, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(522, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(708, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 396), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(88, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(150, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(212, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(336, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(584, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(770, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(770, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 458), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(150, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(212, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(398, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(584, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(708, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 520), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(150, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(274, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(336, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(646, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(708, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(770, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 582), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(88, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(274, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(522, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(646, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(770, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(832, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(832, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 644), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(26, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(26, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(88, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(150, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(150, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(212, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(274, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(336, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(398, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(398, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(460, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(460, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(522, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(584, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(584, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(646, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(708, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(770, 706), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(770, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(70, 8), Vector2f(832, 768), middleLevel2Color);
    addWall(wallsMiddleLevel2, Vector2f(8, 70), Vector2f(894, 706), middleLevel2Color);

    // هدف المستوى 2 (أعلى اليمين، عند فتحة المخرج - الشوكولا والمال)
    RectangleShape middleGoal2(Vector2f(40.f, 40.f));
    middleGoal2.setPosition(785.f, 101.f);
    middleGoal2.setTexture(&middleGoalTexture);

    // فخاخ المستوى 2
    std::vector<Trap> trapsMiddleLevel2;
    addTrap(trapsMiddleLevel2, Vector2f(731.f, 468.f));
    addTrap(trapsMiddleLevel2, Vector2f(359.f, 530.f));
    addTrap(trapsMiddleLevel2, Vector2f(111.f, 468.f));
    addTrap(trapsMiddleLevel2, Vector2f(173.f, 282.f));
    addTrap(trapsMiddleLevel2, Vector2f(545.f, 344.f));
    addTrap(trapsMiddleLevel2, Vector2f(111.f, 158.f));

    // عملات المستوى 2 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel2;
    addCoin(coinsMiddleLevel2, Vector2f(371.f, 245.f));
    addCoin(coinsMiddleLevel2, Vector2f(681.f, 183.f));
    addCoin(coinsMiddleLevel2, Vector2f(433.f, 617.f));
    addCoin(coinsMiddleLevel2, Vector2f(433.f, 369.f));
    addCoin(coinsMiddleLevel2, Vector2f(681.f, 555.f));

    // ================= MIDDLE LEVEL 3: متاهة كثيفة (Endless Mazes Style) =================
    // (مستوحاة من صورة "Endless Mazes" - مدخل أعلى اليسار، هدف (نقطة حمراء) قرب أعلى اليمين)
    Color middleLevel3Color = Color::White;
    std::vector<RectangleShape> wallsMiddleLevel3;

    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(26, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(88, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(150, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(212, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(274, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(336, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(398, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(460, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(522, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(584, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(832, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(832, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 86), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(150, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(274, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(336, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(398, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(584, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(646, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(708, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(770, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 148), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(26, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(150, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(274, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(398, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(460, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(584, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(708, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(832, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 210), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(150, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(212, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(274, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(336, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(398, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(522, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(646, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(770, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 272), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(88, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(150, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(336, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(460, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(522, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(584, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(770, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(832, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 334), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(88, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(150, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(274, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(336, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(398, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(646, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(770, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(832, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 396), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(26, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(336, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(398, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(522, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(646, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(708, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(832, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 458), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(150, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(212, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(336, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(336, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(398, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(398, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(460, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(584, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(646, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(770, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 520), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(26, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(150, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(150, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(336, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(398, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(584, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(708, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(832, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 582), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(150, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(212, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(274, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(336, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(522, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(832, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 644), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(26, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(26, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(88, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(88, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(150, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(150, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(212, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(274, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(336, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(336, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(398, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(398, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(460, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(460, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(522, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(522, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(584, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(646, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 706), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(708, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(770, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(70, 8), Vector2f(832, 768), middleLevel3Color);
    addWall(wallsMiddleLevel3, Vector2f(8, 70), Vector2f(894, 706), middleLevel3Color);

    // هدف المستوى 3 (نقطة حمراء صغيرة قرب أعلى اليمين، بحال الصورة)
    RectangleShape middleGoal3(Vector2f(40.f, 40.f));
    middleGoal3.setPosition(847.f, 225.f);
    middleGoal3.setTexture(&middleGoalTexture);

    // فخاخ المستوى 3 (7 فخاخ - أصعب من المستويين السابقين)
    std::vector<Trap> trapsMiddleLevel3;
    addTrap(trapsMiddleLevel3, Vector2f(173.f, 654.f));
    addTrap(trapsMiddleLevel3, Vector2f(483.f, 592.f));
    addTrap(trapsMiddleLevel3, Vector2f(173.f, 220.f));
    addTrap(trapsMiddleLevel3, Vector2f(669.f, 468.f));
    addTrap(trapsMiddleLevel3, Vector2f(173.f, 344.f));
    addTrap(trapsMiddleLevel3, Vector2f(669.f, 158.f));
    addTrap(trapsMiddleLevel3, Vector2f(731.f, 220.f));

    // عملات المستوى 3 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel3;
    addCoin(coinsMiddleLevel3, Vector2f(433.f, 617.f));
    addCoin(coinsMiddleLevel3, Vector2f(247.f, 369.f));
    addCoin(coinsMiddleLevel3, Vector2f(557.f, 183.f));
    addCoin(coinsMiddleLevel3, Vector2f(619.f, 617.f));
    addCoin(coinsMiddleLevel3, Vector2f(557.f, 307.f));

    // ================= MIDDLE LEVEL 4: متاهة مربعة كثيفة (مدخل يسار، مخرج أسفل اليمين) =================
    Color middleLevel4Color = Color::Yellow;
    std::vector<RectangleShape> wallsMiddleLevel4;

    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(26, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(88, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(212, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(522, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(584, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(708, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(832, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 86), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(274, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(336, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(522, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(646, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(832, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 148), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(26, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(212, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(336, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(398, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(584, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(708, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(832, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 210), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(88, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(150, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(274, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(398, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(584, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(770, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(832, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 272), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(88, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(212, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(274, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(398, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(646, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(770, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(832, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 334), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(26, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(212, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(336, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(460, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(770, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 396), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(88, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(212, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(274, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(584, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(646, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(832, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 458), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(88, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(150, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(274, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(336, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(460, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(646, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(708, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(832, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 520), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(88, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(150, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(212, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(274, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(336, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(460, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(584, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(708, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(770, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 582), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(150, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(212, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(336, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(398, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(460, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(522, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(584, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(646, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(832, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 644), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(26, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(26, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(88, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(150, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(212, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(212, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(274, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(336, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(398, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(398, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(460, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(460, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(522, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(584, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(646, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(708, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(708, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 706), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(770, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(70, 8), Vector2f(832, 768), middleLevel4Color);
    addWall(wallsMiddleLevel4, Vector2f(8, 70), Vector2f(894, 706), middleLevel4Color);

    // هدف المستوى 4 (أسفل اليمين، عند فتحة المخرج)
    RectangleShape middleGoal4(Vector2f(40.f, 40.f));
    middleGoal4.setPosition(847.f, 721.f);
    middleGoal4.setTexture(&middleGoalTexture);

    // فخاخ المستوى 4 (8 فخاخ - الأصعب حتى الآن)
    std::vector<Trap> trapsMiddleLevel4;
    addTrap(trapsMiddleLevel4, Vector2f(173.f, 344.f));
    addTrap(trapsMiddleLevel4, Vector2f(545.f, 220.f));
    addTrap(trapsMiddleLevel4, Vector2f(545.f, 468.f));
    addTrap(trapsMiddleLevel4, Vector2f(793.f, 654.f));
    addTrap(trapsMiddleLevel4, Vector2f(359.f, 220.f));
    addTrap(trapsMiddleLevel4, Vector2f(297.f, 282.f));
    addTrap(trapsMiddleLevel4, Vector2f(173.f, 406.f));
    addTrap(trapsMiddleLevel4, Vector2f(483.f, 406.f));

    // عملات المستوى 4 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel4;
    addCoin(coinsMiddleLevel4, Vector2f(433.f, 431.f));
    addCoin(coinsMiddleLevel4, Vector2f(805.f, 617.f));
    addCoin(coinsMiddleLevel4, Vector2f(123.f, 183.f));
    addCoin(coinsMiddleLevel4, Vector2f(433.f, 617.f));
    addCoin(coinsMiddleLevel4, Vector2f(371.f, 369.f));

    // ================= MIDDLE LEVEL 5: مدخل من الوسط اليسار، مخرج من الوسط اليمين (بحال الصورة: كلب وطفل) =================
    Color middleLevel5Color(255, 100, 0); // برتقالي مميز
    std::vector<RectangleShape> wallsMiddleLevel5;

    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(26, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(212, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(274, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(336, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(708, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(832, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 86), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(88, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(398, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(460, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(584, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(832, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 148), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(88, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(274, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(460, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(708, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 210), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(150, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(212, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(336, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(646, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(708, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(832, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 272), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(398, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(522, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(584, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(646, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(770, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(832, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 334), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(26, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(150, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(460, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(522, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(584, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(708, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(770, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 396), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(26, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(88, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(274, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(336, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(522, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(646, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(770, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(832, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 458), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(150, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(212, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(274, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(398, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(584, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(708, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 520), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(26, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(336, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(336, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(460, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(708, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 582), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(88, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(212, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(274, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(398, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(460, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(646, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(770, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(832, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 644), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(26, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(26, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(88, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(150, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(150, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(212, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(274, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(274, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(336, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(336, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(398, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(460, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(460, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(522, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(584, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(646, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(708, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 706), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(770, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(70, 8), Vector2f(832, 768), middleLevel5Color);
    addWall(wallsMiddleLevel5, Vector2f(8, 70), Vector2f(894, 706), middleLevel5Color);

    // هدف المستوى 5 (وسط اليمين، عند خانة المخرج)
    RectangleShape middleGoal5(Vector2f(40.f, 40.f));
    middleGoal5.setPosition(847.f, 411.f);
    middleGoal5.setTexture(&middleGoalTexture);

    // فخاخ المستوى 5 (9 فخاخ - الأصعب حتى الآن)
    std::vector<Trap> trapsMiddleLevel5;
    addTrap(trapsMiddleLevel5, Vector2f(173.f, 592.f));
    addTrap(trapsMiddleLevel5, Vector2f(669.f, 406.f));
    addTrap(trapsMiddleLevel5, Vector2f(483.f, 406.f));
    addTrap(trapsMiddleLevel5, Vector2f(111.f, 406.f));
    addTrap(trapsMiddleLevel5, Vector2f(793.f, 220.f));
    addTrap(trapsMiddleLevel5, Vector2f(297.f, 220.f));
    addTrap(trapsMiddleLevel5, Vector2f(235.f, 592.f));
    addTrap(trapsMiddleLevel5, Vector2f(235.f, 530.f));
    addTrap(trapsMiddleLevel5, Vector2f(793.f, 530.f));

    // عملات المستوى 5 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel5;
    addCoin(coinsMiddleLevel5, Vector2f(433.f, 555.f));
    addCoin(coinsMiddleLevel5, Vector2f(185.f, 183.f));
    addCoin(coinsMiddleLevel5, Vector2f(805.f, 493.f));
    addCoin(coinsMiddleLevel5, Vector2f(433.f, 431.f));
    addCoin(coinsMiddleLevel5, Vector2f(743.f, 431.f));

    // ================= MIDDLE LEVEL 6: مدخل أعلى اليسار (روبوت)، مخرج أسفل اليمين (Finish) =================
    Color middleLevel6Color(0, 200, 255); // أزرق سماوي مميز
    std::vector<RectangleShape> wallsMiddleLevel6;

    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(26, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(88, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(150, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(212, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(274, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(336, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(398, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(460, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(584, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(584, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(708, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(832, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 86), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(212, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(336, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(460, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(522, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(584, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(646, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(770, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(832, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 148), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(26, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(150, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(150, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(336, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(398, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(460, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(522, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(584, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(646, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(708, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(832, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 210), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(88, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(212, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(212, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(460, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(708, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(770, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 272), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(88, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(150, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(150, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(212, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(460, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(460, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(584, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(708, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(832, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 334), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(150, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(212, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(274, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(522, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(584, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(646, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 396), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(150, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(274, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(398, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(460, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(584, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(646, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(770, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 458), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(26, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(88, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(150, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(212, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(336, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(398, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(522, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(708, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(832, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(832, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 520), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(88, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(150, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(212, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(460, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(460, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(584, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(584, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(708, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 582), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(212, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(212, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(336, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(398, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(522, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(584, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(584, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 644), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(26, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(26, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(88, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(150, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(150, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(212, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(274, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(274, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(274, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(336, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(398, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(398, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(460, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(460, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(522, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(522, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(584, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(646, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(708, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(770, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(832, 706), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(70, 8), Vector2f(832, 768), middleLevel6Color);
    addWall(wallsMiddleLevel6, Vector2f(8, 70), Vector2f(894, 706), middleLevel6Color);

    // هدف المستوى 6 (أسفل اليمين، عند خانة المخرج - Finish)
    RectangleShape middleGoal6(Vector2f(40.f, 40.f));
    middleGoal6.setPosition(847.f, 721.f);
    middleGoal6.setTexture(&middleGoalTexture);

    // فخاخ المستوى 6 (10 فخاخ - الأصعب حتى الآن)
    std::vector<Trap> trapsMiddleLevel6;
    addTrap(trapsMiddleLevel6, Vector2f(607.f, 406.f));
    addTrap(trapsMiddleLevel6, Vector2f(731.f, 530.f));
    addTrap(trapsMiddleLevel6, Vector2f(669.f, 282.f));
    addTrap(trapsMiddleLevel6, Vector2f(483.f, 592.f));
    addTrap(trapsMiddleLevel6, Vector2f(793.f, 530.f));
    addTrap(trapsMiddleLevel6, Vector2f(111.f, 530.f));
    addTrap(trapsMiddleLevel6, Vector2f(359.f, 592.f));
    addTrap(trapsMiddleLevel6, Vector2f(421.f, 406.f));
    addTrap(trapsMiddleLevel6, Vector2f(607.f, 654.f));
    addTrap(trapsMiddleLevel6, Vector2f(111.f, 220.f));

    // عملات المستوى 6 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel6;
    addCoin(coinsMiddleLevel6, Vector2f(309.f, 493.f));
    addCoin(coinsMiddleLevel6, Vector2f(433.f, 555.f));
    addCoin(coinsMiddleLevel6, Vector2f(743.f, 245.f));
    addCoin(coinsMiddleLevel6, Vector2f(619.f, 307.f));
    addCoin(coinsMiddleLevel6, Vector2f(309.f, 431.f));

    // ================= MIDDLE LEVEL 7: مدخل وسط اليسار (طفل)، مخرج وسط اليمين (روبوت) =================
    Color middleLevel7Color(0, 128, 0); // أخضر داكن مميز
    std::vector<RectangleShape> wallsMiddleLevel7;

    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(212, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(336, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(522, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(646, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(832, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 86), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(150, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(212, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(274, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(336, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(460, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(522, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(708, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(770, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 148), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(274, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(398, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(584, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(646, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(770, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 210), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(212, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(460, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(522, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(584, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(708, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 272), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(212, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(336, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(584, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(646, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(708, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 334), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(150, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(336, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(584, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(646, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(708, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 396), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(150, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(398, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(646, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 458), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(150, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(398, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(522, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(646, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(770, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 520), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(88, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(212, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(336, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(398, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(522, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(646, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(708, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(832, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 582), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(212, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(274, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(398, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(460, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(584, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(646, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(832, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 644), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(26, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(26, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(88, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(150, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(212, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(274, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(336, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(398, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(398, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(460, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(522, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(522, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(584, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(646, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(646, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(708, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 706), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(770, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(70, 8), Vector2f(832, 768), middleLevel7Color);
    addWall(wallsMiddleLevel7, Vector2f(8, 70), Vector2f(894, 706), middleLevel7Color);

    // هدف المستوى 7 (وسط اليمين، عند خانة المخرج - الروبوت)
    RectangleShape middleGoal7(Vector2f(40.f, 40.f));
    middleGoal7.setPosition(847.f, 411.f);
    middleGoal7.setTexture(&middleGoalTexture);

    // فخاخ المستوى 7 (11 فخ - الأصعب حتى الآن)
    std::vector<Trap> trapsMiddleLevel7;
    addTrap(trapsMiddleLevel7, Vector2f(607.f, 406.f));
    addTrap(trapsMiddleLevel7, Vector2f(297.f, 158.f));
    addTrap(trapsMiddleLevel7, Vector2f(669.f, 282.f));
    addTrap(trapsMiddleLevel7, Vector2f(731.f, 530.f));
    addTrap(trapsMiddleLevel7, Vector2f(669.f, 592.f));
    addTrap(trapsMiddleLevel7, Vector2f(111.f, 282.f));
    addTrap(trapsMiddleLevel7, Vector2f(111.f, 220.f));
    addTrap(trapsMiddleLevel7, Vector2f(793.f, 158.f));
    addTrap(trapsMiddleLevel7, Vector2f(297.f, 468.f));
    addTrap(trapsMiddleLevel7, Vector2f(111.f, 158.f));
    addTrap(trapsMiddleLevel7, Vector2f(545.f, 344.f));

    // عملات المستوى 7 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel7;
    addCoin(coinsMiddleLevel7, Vector2f(619.f, 307.f));
    addCoin(coinsMiddleLevel7, Vector2f(433.f, 493.f));
    addCoin(coinsMiddleLevel7, Vector2f(433.f, 431.f));
    addCoin(coinsMiddleLevel7, Vector2f(557.f, 307.f));
    addCoin(coinsMiddleLevel7, Vector2f(681.f, 183.f));

    // ================= MIDDLE LEVEL 8: متاهة كثيفة كلاسيكية (Maze 11 - Medium Style) =================
    Color middleLevel8Color(128, 128, 128); // رمادي مميز
    std::vector<RectangleShape> wallsMiddleLevel8;

    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(26, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(88, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(212, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(274, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(336, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(398, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(460, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(770, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(832, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 86), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(88, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(150, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(274, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(336, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(398, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(522, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(584, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(708, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(770, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 148), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(88, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(212, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(398, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(460, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(522, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(646, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(770, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 210), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(274, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(274, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(398, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(460, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(584, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(770, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 272), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(26, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(398, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(708, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(770, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 334), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(88, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(212, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(274, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(336, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(398, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(646, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 396), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(26, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(150, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(212, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(274, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(274, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(398, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(398, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(522, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(584, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(770, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 458), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(88, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(274, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(336, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(460, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(522, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(646, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(770, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(832, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 520), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(88, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(212, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(398, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(398, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(460, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(522, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(646, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(708, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(770, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 582), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(150, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(212, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(336, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(398, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(460, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(646, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(708, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(770, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(832, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 644), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(26, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(26, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(88, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(150, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(150, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(212, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(274, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(274, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(336, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(398, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(460, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(522, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(584, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(646, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(708, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(770, 706), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(770, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(70, 8), Vector2f(832, 768), middleLevel8Color);
    addWall(wallsMiddleLevel8, Vector2f(8, 70), Vector2f(894, 706), middleLevel8Color);

    // هدف المستوى 8 (أسفل اليمين، عند خانة المخرج)
    RectangleShape middleGoal8(Vector2f(40.f, 40.f));
    middleGoal8.setPosition(847.f, 721.f);
    middleGoal8.setTexture(&middleGoalTexture);

    // فخاخ المستوى 8 (12 فخ - الأصعب حتى الآن)
    std::vector<Trap> trapsMiddleLevel8;
    addTrap(trapsMiddleLevel8, Vector2f(607.f, 220.f));
    addTrap(trapsMiddleLevel8, Vector2f(669.f, 468.f));
    addTrap(trapsMiddleLevel8, Vector2f(297.f, 344.f));
    addTrap(trapsMiddleLevel8, Vector2f(731.f, 158.f));
    addTrap(trapsMiddleLevel8, Vector2f(793.f, 282.f));
    addTrap(trapsMiddleLevel8, Vector2f(607.f, 158.f));
    addTrap(trapsMiddleLevel8, Vector2f(483.f, 468.f));
    addTrap(trapsMiddleLevel8, Vector2f(421.f, 592.f));
    addTrap(trapsMiddleLevel8, Vector2f(235.f, 158.f));
    addTrap(trapsMiddleLevel8, Vector2f(669.f, 282.f));
    addTrap(trapsMiddleLevel8, Vector2f(607.f, 468.f));
    addTrap(trapsMiddleLevel8, Vector2f(173.f, 530.f));

    // عملات المستوى 8 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel8;
    addCoin(coinsMiddleLevel8, Vector2f(495.f, 431.f));
    addCoin(coinsMiddleLevel8, Vector2f(495.f, 679.f));
    addCoin(coinsMiddleLevel8, Vector2f(805.f, 183.f));
    addCoin(coinsMiddleLevel8, Vector2f(681.f, 245.f));
    addCoin(coinsMiddleLevel8, Vector2f(371.f, 617.f));

    // ================= MIDDLE LEVEL 9: مدخل وسط اليسار، مخرج وسط اليمين =================
    Color middleLevel9Color(30, 60, 220); // أزرق داكن مميز
    std::vector<RectangleShape> wallsMiddleLevel9;

    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(26, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(88, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(212, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(336, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(336, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(398, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(522, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(584, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(646, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(708, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(832, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 86), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(88, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(212, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(274, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(336, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(398, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(522, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(708, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(832, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 148), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(212, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(274, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(398, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(398, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(522, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(584, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(708, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(832, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 210), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(88, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(336, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(336, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(460, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(646, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(708, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 272), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(212, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(212, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(336, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(398, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(460, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(522, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(708, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 334), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(26, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(212, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(274, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(398, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(398, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(522, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(584, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(646, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(708, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(708, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(832, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(832, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 396), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(26, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(150, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(212, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(336, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(522, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(708, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 458), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(88, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(150, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(212, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(274, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(336, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(398, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(460, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(646, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 520), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(150, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(212, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(274, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(336, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(336, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(398, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(522, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(522, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(646, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(832, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 582), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(26, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(212, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(336, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(398, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(398, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(522, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(584, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(584, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(708, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(708, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(770, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 644), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(26, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(26, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(88, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(150, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(212, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(212, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(274, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(336, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(398, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(460, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(460, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(522, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(584, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(646, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(646, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(708, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(770, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(832, 706), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(70, 8), Vector2f(832, 768), middleLevel9Color);
    addWall(wallsMiddleLevel9, Vector2f(8, 70), Vector2f(894, 706), middleLevel9Color);

    // هدف المستوى 9 (وسط اليمين، عند خانة المخرج)
    RectangleShape middleGoal9(Vector2f(40.f, 40.f));
    middleGoal9.setPosition(847.f, 411.f);
    middleGoal9.setTexture(&middleGoalTexture);

    // فخاخ المستوى 9 (13 فخ - الأصعب حتى الآن)
    std::vector<Trap> trapsMiddleLevel9;
    addTrap(trapsMiddleLevel9, Vector2f(545.f, 530.f));
    addTrap(trapsMiddleLevel9, Vector2f(297.f, 344.f));
    addTrap(trapsMiddleLevel9, Vector2f(607.f, 592.f));
    addTrap(trapsMiddleLevel9, Vector2f(173.f, 468.f));
    addTrap(trapsMiddleLevel9, Vector2f(731.f, 344.f));
    addTrap(trapsMiddleLevel9, Vector2f(111.f, 158.f));
    addTrap(trapsMiddleLevel9, Vector2f(483.f, 282.f));
    addTrap(trapsMiddleLevel9, Vector2f(235.f, 344.f));
    addTrap(trapsMiddleLevel9, Vector2f(111.f, 468.f));
    addTrap(trapsMiddleLevel9, Vector2f(359.f, 592.f));
    addTrap(trapsMiddleLevel9, Vector2f(235.f, 220.f));
    addTrap(trapsMiddleLevel9, Vector2f(111.f, 406.f));
    addTrap(trapsMiddleLevel9, Vector2f(545.f, 344.f));

    // عملات المستوى 9 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel9;
    addCoin(coinsMiddleLevel9, Vector2f(247.f, 431.f));
    addCoin(coinsMiddleLevel9, Vector2f(309.f, 493.f));
    addCoin(coinsMiddleLevel9, Vector2f(681.f, 493.f));
    addCoin(coinsMiddleLevel9, Vector2f(495.f, 431.f));
    addCoin(coinsMiddleLevel9, Vector2f(681.f, 245.f));

    // ================= MIDDLE LEVEL 10 (الأخير): مدخل أعلى اليسار، مخرج أسفل اليمين =================
    Color middleLevel10Color(200, 30, 30); // أحمر داكن مميز - آخر مستوى
    std::vector<RectangleShape> wallsMiddleLevel10;

    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(26, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(88, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(398, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(522, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(646, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(832, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 86), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(88, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(274, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(398, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(460, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(584, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(708, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 148), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(150, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(212, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(336, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(460, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(770, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(832, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 210), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(26, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(212, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(336, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(398, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(398, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(522, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(646, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(832, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 272), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(88, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(150, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(274, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(336, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(398, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(584, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(832, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 334), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(88, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(150, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(398, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(398, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(584, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(708, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(832, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 396), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(26, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(150, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(274, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(398, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(460, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(522, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(584, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(708, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(770, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 458), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(88, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(150, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(212, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(274, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(460, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(708, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(832, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 520), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(212, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(398, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(522, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 582), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(88, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(398, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(460, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(584, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(832, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 644), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(26, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(26, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(88, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(150, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(212, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(274, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(336, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(336, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(398, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(460, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(522, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(584, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(646, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(708, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 706), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(770, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(70, 8), Vector2f(832, 768), middleLevel10Color);
    addWall(wallsMiddleLevel10, Vector2f(8, 70), Vector2f(894, 706), middleLevel10Color);

    // هدف المستوى 10 (أسفل اليمين، عند خانة المخرج - آخر هدف فـ Middle)
    RectangleShape middleGoal10(Vector2f(40.f, 40.f));
    middleGoal10.setPosition(847.f, 721.f);
    middleGoal10.setTexture(&middleGoalTexture);

    // فخاخ المستوى 10 (14 فخ - الأصعب فـ Middle كامل)
    std::vector<Trap> trapsMiddleLevel10;
    addTrap(trapsMiddleLevel10, Vector2f(731.f, 220.f));
    addTrap(trapsMiddleLevel10, Vector2f(731.f, 654.f));
    addTrap(trapsMiddleLevel10, Vector2f(731.f, 158.f));
    addTrap(trapsMiddleLevel10, Vector2f(483.f, 530.f));
    addTrap(trapsMiddleLevel10, Vector2f(235.f, 406.f));
    addTrap(trapsMiddleLevel10, Vector2f(297.f, 220.f));
    addTrap(trapsMiddleLevel10, Vector2f(297.f, 406.f));
    addTrap(trapsMiddleLevel10, Vector2f(359.f, 654.f));
    addTrap(trapsMiddleLevel10, Vector2f(669.f, 158.f));
    addTrap(trapsMiddleLevel10, Vector2f(731.f, 344.f));
    addTrap(trapsMiddleLevel10, Vector2f(607.f, 220.f));
    addTrap(trapsMiddleLevel10, Vector2f(359.f, 220.f));
    addTrap(trapsMiddleLevel10, Vector2f(607.f, 468.f));
    addTrap(trapsMiddleLevel10, Vector2f(669.f, 468.f));

    // عملات المستوى 10 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsMiddleLevel10;
    addCoin(coinsMiddleLevel10, Vector2f(619.f, 617.f));
    addCoin(coinsMiddleLevel10, Vector2f(743.f, 493.f));
    addCoin(coinsMiddleLevel10, Vector2f(371.f, 369.f));
    addCoin(coinsMiddleLevel10, Vector2f(247.f, 555.f));
    addCoin(coinsMiddleLevel10, Vector2f(557.f, 679.f));









    while (window.isOpen())
    {
        Vector2i pixelPos = Mouse::getPosition(window);
        Vector2f mouse = window.mapPixelToCoords(pixelPos);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Space)
                {
                    handleBackNavigation();
                    levelTimer.restart();
                }
            }

            if (event.type == Event::MouseButtonPressed)
            {
                if (state == GAMEPLAY)
                {
                    if (gameplayBackButton.getGlobalBounds().contains(mouse))
                    {
                        handleBackNavigation();
                        levelTimer.restart();
                    }
                }
            }
        }
        if (!window.isOpen())
            break;

        // تأثير تمرير الماوس فوق زر العودة
        if (state == GAMEPLAY) {
            if (gameplayBackButton.getGlobalBounds().contains(mouse)) gameplayBackButton.setFillColor(Color::Yellow);
            else gameplayBackButton.setFillColor(Color::White);
        }

        // ================= حركة اللاعب =================
        Vector2f movement(0.f, 0.f);
        Vector2f oldPos = player.getPosition();

        if (Keyboard::isKeyPressed(Keyboard::Up))    movement.y -= playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Down))  movement.y += playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Left))  movement.x -= playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Right)) movement.x += playerSpeed;

        if (Mouse::isButtonPressed(Mouse::Left))
        {
            if (upBtn.getGlobalBounds().contains(mouse))    movement.y -= playerSpeed;
            if (downBtn.getGlobalBounds().contains(mouse))  movement.y += playerSpeed;
            if (leftBtn.getGlobalBounds().contains(mouse))  movement.x -= playerSpeed;
            if (rightBtn.getGlobalBounds().contains(mouse)) movement.x += playerSpeed;
        }

        player.move(movement);

        // ===== فحص انتهاء الوقت =====
        float elapsedTime = levelTimer.getElapsedTime().asSeconds();
        if (elapsedTime >= levelTimeLimits[level])
        {
            resetPlayerPosition();
            levelTimer.restart();
        }

        // ===== انتهاء تأثير الإبطاء =====
        if (isSlowed && slowClock.getElapsedTime().asSeconds() >= slowDuration)
        {
            isSlowed = false;
            playerSpeed = normalSpeed;
        }

        // ===== فحص لمس الفخاخ + التصادم مع الجدران + الوصول للهدف حسب المستوى =====
        if (level == 1)
        {
            checkTraps(trapsMiddleLevel1);
            checkCoins(coinsMiddleLevel1);

            for (const auto& wall : wallsMiddleLevel1)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal1.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel1) >= coinsRequired)
            {
                doorSound.play();
                level = 2;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 2)
        {
            checkTraps(trapsMiddleLevel2);
            checkCoins(coinsMiddleLevel2);

            for (const auto& wall : wallsMiddleLevel2)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal2.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel2) >= coinsRequired)
            {
                doorSound.play();
                level = 3;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 3)
        {
            checkTraps(trapsMiddleLevel3);
            checkCoins(coinsMiddleLevel3);

            for (const auto& wall : wallsMiddleLevel3)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal3.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel3) >= coinsRequired)
            {
                doorSound.play();
                level = 4;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 4)
        {
            checkTraps(trapsMiddleLevel4);
            checkCoins(coinsMiddleLevel4);

            for (const auto& wall : wallsMiddleLevel4)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal4.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel4) >= coinsRequired)
            {
                doorSound.play();
                level = 5;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 5)
        {
            checkTraps(trapsMiddleLevel5);
            checkCoins(coinsMiddleLevel5);

            for (const auto& wall : wallsMiddleLevel5)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal5.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel5) >= coinsRequired)
            {
                doorSound.play();
                level = 6;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 6)
        {
            checkTraps(trapsMiddleLevel6);
            checkCoins(coinsMiddleLevel6);

            for (const auto& wall : wallsMiddleLevel6)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal6.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel6) >= coinsRequired)
            {
                doorSound.play();
                level = 7;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 7)
        {
            checkTraps(trapsMiddleLevel7);
            checkCoins(coinsMiddleLevel7);

            for (const auto& wall : wallsMiddleLevel7)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal7.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel7) >= coinsRequired)
            {
                doorSound.play();
                level = 8;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 8)
        {
            checkTraps(trapsMiddleLevel8);
            checkCoins(coinsMiddleLevel8);

            for (const auto& wall : wallsMiddleLevel8)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal8.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel8) >= coinsRequired)
            {
                doorSound.play();
                level = 9;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 9)
        {
            checkTraps(trapsMiddleLevel9);
            checkCoins(coinsMiddleLevel9);

            for (const auto& wall : wallsMiddleLevel9)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal9.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel9) >= coinsRequired)
            {
                doorSound.play();
                level = 10;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 10)
        {
            checkTraps(trapsMiddleLevel10);
            checkCoins(coinsMiddleLevel10);

            for (const auto& wall : wallsMiddleLevel10)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(middleGoal10.getGlobalBounds()) && countCollectedCoins(coinsMiddleLevel10) >= coinsRequired)
            {
                doorSound.play();
                // كمل آخر مستوى فـ Middle - نرجعو مباشرة لواجهة اختيار الصعوبة (Easy/Middle/Difficult)
                g_nextState = DIFFICULTY_MENU;
                window.close();
            }
        }

        // تحديث نص الوقت المتبقي
        float remainingTime = levelTimeLimits[level] - levelTimer.getElapsedTime().asSeconds();
        if (remainingTime < 0.f) remainingTime = 0.f;
        timerText.setString("Time: " + std::to_string((int)remainingTime));

        // تحديث نص النقاط + عدد العملات المجموعة فالمستوى الحالي
        int currentCoinsCollected = 0;
        if (level == 1) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel1);
        else if (level == 2) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel2);
        else if (level == 3) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel3);
        else if (level == 4) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel4);
        else if (level == 5) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel5);
        else if (level == 6) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel6);
        else if (level == 7) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel7);
        else if (level == 8) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel8);
        else if (level == 9) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel9);
        else if (level == 10) currentCoinsCollected = countCollectedCoins(coinsMiddleLevel10);
        scoreText.setString("Score: " + std::to_string(score) + "   Coins: " + std::to_string(currentCoinsCollected) + "/" + std::to_string(coinsRequired));

        // ================= DRAWING =================
        window.clear(Color::Black);

        if (level == 1)
        {
            for (const auto& wall : wallsMiddleLevel1) window.draw(wall);
            middleGoal1.setFillColor(countCollectedCoins(coinsMiddleLevel1) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal1);
            drawTraps(trapsMiddleLevel1);
            drawCoins(coinsMiddleLevel1);
        }
        else if (level == 2)
        {
            for (const auto& wall : wallsMiddleLevel2) window.draw(wall);
            middleGoal2.setFillColor(countCollectedCoins(coinsMiddleLevel2) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal2);
            drawTraps(trapsMiddleLevel2);
            drawCoins(coinsMiddleLevel2);
        }
        else if (level == 3)
        {
            for (const auto& wall : wallsMiddleLevel3) window.draw(wall);
            middleGoal3.setFillColor(countCollectedCoins(coinsMiddleLevel3) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal3);
            drawTraps(trapsMiddleLevel3);
            drawCoins(coinsMiddleLevel3);
        }
        else if (level == 4)
        {
            for (const auto& wall : wallsMiddleLevel4) window.draw(wall);
            middleGoal4.setFillColor(countCollectedCoins(coinsMiddleLevel4) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal4);
            drawTraps(trapsMiddleLevel4);
            drawCoins(coinsMiddleLevel4);
        }
        else if (level == 5)
        {
            for (const auto& wall : wallsMiddleLevel5) window.draw(wall);
            middleGoal5.setFillColor(countCollectedCoins(coinsMiddleLevel5) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal5);
            drawTraps(trapsMiddleLevel5);
            drawCoins(coinsMiddleLevel5);
        }
        else if (level == 6)
        {
            for (const auto& wall : wallsMiddleLevel6) window.draw(wall);
            middleGoal6.setFillColor(countCollectedCoins(coinsMiddleLevel6) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal6);
            drawTraps(trapsMiddleLevel6);
            drawCoins(coinsMiddleLevel6);
        }
        else if (level == 7)
        {
            for (const auto& wall : wallsMiddleLevel7) window.draw(wall);
            middleGoal7.setFillColor(countCollectedCoins(coinsMiddleLevel7) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal7);
            drawTraps(trapsMiddleLevel7);
            drawCoins(coinsMiddleLevel7);
        }
        else if (level == 8)
        {
            for (const auto& wall : wallsMiddleLevel8) window.draw(wall);
            middleGoal8.setFillColor(countCollectedCoins(coinsMiddleLevel8) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal8);
            drawTraps(trapsMiddleLevel8);
            drawCoins(coinsMiddleLevel8);
        }
        else if (level == 9)
        {
            for (const auto& wall : wallsMiddleLevel9) window.draw(wall);
            middleGoal9.setFillColor(countCollectedCoins(coinsMiddleLevel9) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal9);
            drawTraps(trapsMiddleLevel9);
            drawCoins(coinsMiddleLevel9);
        }
        else if (level == 10)
        {
            for (const auto& wall : wallsMiddleLevel10) window.draw(wall);
            middleGoal10.setFillColor(countCollectedCoins(coinsMiddleLevel10) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(middleGoal10);
            drawTraps(trapsMiddleLevel10);
            drawCoins(coinsMiddleLevel10);
        }

        window.draw(gameplayBackButton);
        window.draw(timerText);
        window.draw(scoreText);

        // أزرار التحكم اللمسية
        window.draw(upBtn);    window.draw(upTxt);
        window.draw(downBtn);  window.draw(downTxt);
        window.draw(leftBtn);  window.draw(leftTxt);
        window.draw(rightBtn); window.draw(rightTxt);

        window.draw(player);
        window.display();
    }
}


// ===================================================================
// ================= HARD: كما كانت بالضبط في الكود الأصلي ===========
// ===================================================================
void Hard() {
    RenderWindow window(VideoMode(1000, 1000), "Simple Game");
    window.setFramerateLimit(60);

    GameState state = GAMEPLAY;
    int level = 1;
    float playerSpeed = 6.f; // أبطأ شوية من Middle لأن الممرات أضيق

    // ===================== نظام الفخاخ (Traps) =====================
    float normalSpeed = 6.f;
    float slowSpeed = 2.f;
    float slowDuration = 3.f;
    bool isSlowed = false;
    Clock slowClock;
    float trapRevealDistance = 70.f; // أقصر من Middle لأن الخانات أصغر
    // ==================================================================

    Font font;
    if (!font.loadFromFile("Hibo.otf"))
    {
        std::cout << "Error loading font!" << std::endl;
    }

    // ===================== صوت فتح الباب (door open sound) =====================
    SoundBuffer doorSoundBuffer;
    Sound doorSound;
    if (!doorSoundBuffer.loadFromFile("mo.ogg"))
    {
        std::cout << "Error loading door sound!" << std::endl;
    }
    else
    {
        doorSound.setBuffer(doorSoundBuffer);
        doorSound.setVolume(80.f);
        doorSound.play(); // === تجربة مؤقتة: كيتشغل مباشرة عند بداية المستوى باش نتأكدو أن الصوت خدام - حيدها من بعد ===
    }
    // ==============================================================================

    // زر العودة الخاص باللعب
    Text gameplayBackButton;
    gameplayBackButton.setFont(font);
    gameplayBackButton.setString("< Back");
    gameplayBackButton.setCharacterSize(25);
    gameplayBackButton.setFillColor(Color::White);
    gameplayBackButton.setPosition(360.f, 15.f);

    // ================= PLAYER =================
    CircleShape player(15.f);
    player.setFillColor(Color::Blue);
    player.setPosition(24.f, 104.f);

    auto resetPlayerPosition = [&]() {
        if (level == 1) { player.setPosition(24.f, 104.f); player.setFillColor(Color::Blue); }
        else if (level == 2) { player.setPosition(24.f, 104.f); player.setFillColor(Color::Red); }
        else if (level == 3) { player.setPosition(24.f, 104.f); player.setFillColor(Color::Green); }
    };

    // دالة العودة خطوة للخلف: من Level 3 نرجعو لـ 2، من 2 لـ 1، ومن Level 1 نسدو النافذة
    auto handleBackNavigation = [&]() {
        if (level == 1) {
            window.close();
        }
        else if (level == 2) {
            level = 1;
            resetPlayerPosition();
        }
        else if (level == 3) {
            level = 2;
            resetPlayerPosition();
        }
    };

    // ================= TOUCH CONTROLS =================
    float btnSize = 50.f;
    Color btnColor(255, 255, 255, 130);

    RectangleShape upBtn(Vector2f(btnSize, btnSize));
    upBtn.setPosition(900.f, 810.f);
    upBtn.setFillColor(btnColor);

    RectangleShape downBtn(Vector2f(btnSize, btnSize));
    downBtn.setPosition(900.f, 910.f);
    downBtn.setFillColor(btnColor);

    RectangleShape leftBtn(Vector2f(btnSize, btnSize));
    leftBtn.setPosition(40.f, 860.f);
    leftBtn.setFillColor(btnColor);

    RectangleShape rightBtn(Vector2f(btnSize, btnSize));
    rightBtn.setPosition(160.f, 860.f);
    rightBtn.setFillColor(btnColor);

    Text upTxt; upTxt.setFont(font); upTxt.setString("^"); upTxt.setCharacterSize(25); upTxt.setFillColor(Color::Black); upTxt.setPosition(918.f, 815.f);
    Text downTxt; downTxt.setFont(font); downTxt.setString("v"); downTxt.setCharacterSize(20); downTxt.setFillColor(Color::Black); downTxt.setPosition(920.f, 920.f);
    Text leftTxt; leftTxt.setFont(font); leftTxt.setString("<"); leftTxt.setCharacterSize(25); leftTxt.setFillColor(Color::Black); leftTxt.setPosition(55.f, 870.f);
    Text rightTxt; rightTxt.setFont(font); rightTxt.setString(">"); rightTxt.setCharacterSize(25); rightTxt.setFillColor(Color::Black); rightTxt.setPosition(175.f, 870.f);

    // ===================== نظام الوقت (Timer) لكل مستوى =====================
    Clock levelTimer;
    Clock animClock; // ساعة مخصصة للأنيميشن (نبضان العملات، توهج الباب)
    float levelTimeLimits[4] = { 0.f, 110.f, 120.f, 130.f }; // index 0 غير مستعمل, 1/2/3 = المستويات (متاهات كثيفة)

    Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(26);
    timerText.setFillColor(Color::White);
    timerText.setPosition(20.f, 15.f);
    // ================================================================

    auto addWall = [&](std::vector<RectangleShape>& wallVec, Vector2f size, Vector2f pos, Color color) {
        RectangleShape wall(size);
        wall.setPosition(pos);
        wall.setFillColor(color);
        wallVec.push_back(wall);
    };

    auto addTrap = [&](std::vector<Trap>& trapVec, Vector2f pos, Vector2f size = Vector2f(20.f, 40.f)) {
        Trap trap;
        trap.shape.setSize(size);
        trap.shape.setPosition(pos);
        trap.shape.setFillColor(Color(255, 140, 0));
        trap.revealed = false;
        trap.active = true;
        trapVec.push_back(trap);
    };

    auto checkTraps = [&](std::vector<Trap>& traps) {
        Vector2f playerCenter = player.getPosition() + Vector2f(player.getRadius(), player.getRadius());
        for (auto& trap : traps)
        {
            if (!trap.active) continue;
            Vector2f trapCenter = trap.shape.getPosition() + trap.shape.getSize() / 2.f;
            float dx = playerCenter.x - trapCenter.x;
            float dy = playerCenter.y - trapCenter.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            trap.revealed = (distance <= trapRevealDistance);

            if (player.getGlobalBounds().intersects(trap.shape.getGlobalBounds()))
            {
                if (!isSlowed)
                {
                    isSlowed = true;
                    playerSpeed = slowSpeed;
                    slowClock.restart();
                }
                trap.active = false;
            }
        }
    };

    auto drawTraps = [&](std::vector<Trap>& traps) {
        for (const auto& t : traps)
        {
            if (t.active && t.revealed)
                window.draw(t.shape);
        }
    };

    // ===================== نظام العملات (Coins) =====================
    int score = 0;
    const int coinsRequired = 5;

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(Color::Yellow);
    scoreText.setPosition(20.f, 45.f);

    auto addCoin = [&](std::vector<Coin>& coinVec, Vector2f pos) {
        Coin c;
        c.shape.setRadius(9.f);
        c.shape.setOrigin(9.f, 9.f); // نخلي نقطة الأصل فمركز الدائرة باش النبضان يكون متمركز
        c.shape.setFillColor(Color::Yellow);
        c.shape.setOutlineColor(Color(150, 100, 0));
        c.shape.setOutlineThickness(2.f);
        c.shape.setPosition(pos); // pos هي المركز مباشرة دابا
        c.collected = false;
        coinVec.push_back(c);
    };

    auto checkCoins = [&](std::vector<Coin>& coins) {
        for (auto& c : coins)
        {
            if (!c.collected && player.getGlobalBounds().intersects(c.shape.getGlobalBounds()))
            {
                c.collected = true;
                score += 100;
            }
        }
    };

    auto countCollectedCoins = [&](std::vector<Coin>& coins) -> int {
        int n = 0;
        for (const auto& c : coins) if (c.collected) n++;
        return n;
    };

    auto drawCoins = [&](std::vector<Coin>& coins) {
        // نبضان بسيط: العملة كتكبر وتصغر بشوية (Pulse Animation) باش تبان حية
        float pulse = 1.f + 0.18f * std::sin(animClock.getElapsedTime().asSeconds() * 4.f);
        for (auto& c : coins)
        {
            if (!c.collected)
            {
                c.shape.setScale(pulse, pulse);
                c.shape.rotate(0.6f); // دوران خفيف مستمر
                window.draw(c.shape);
            }
        }
    };
    // ===================================================================

    Texture hardGoalTexture;
    if (!hardGoalTexture.loadFromFile("mo.jpg"))
    {
        std::cout << "Error loading texture" << std::endl;
    }

    // ================= HARD LEVEL 1: متاهة كثيفة جدا (20x18 خانة) =================
    // المدخل أعلى اليسار، المخرج أسفل اليمين - بحال الصورة
    Color hardLevel1Color(220, 220, 220); // رمادي فاتح مميز على الخلفية السوداء
    std::vector<RectangleShape> wallsHardLevel1;

    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(12, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(924, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 92), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(12, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(396, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(588, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 140), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(156, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(396, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(924, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 188), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(924, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 236), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(444, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 284), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(12, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(444, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(588, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(924, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 332), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(588, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 380), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(156, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(924, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 428), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(156, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 476), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(12, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(156, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(444, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(924, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 524), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(396, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(588, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(924, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 572), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(444, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(588, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(924, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 620), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(156, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(396, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(444, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 668), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(156, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(828, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(924, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 716), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(636, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(876, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 764), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(60, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(204, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(300, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(588, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(684, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(924, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(924, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 812), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(108, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(252, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(348, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(492, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(732, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 860), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(12, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(12, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(60, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(108, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(156, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(204, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(252, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(300, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(348, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(396, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(444, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(444, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(492, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(540, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(540, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(588, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(636, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(684, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(732, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(780, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(780, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(828, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 908), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(876, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(54, 6), Vector2f(924, 956), hardLevel1Color);
    addWall(wallsHardLevel1, Vector2f(6, 54), Vector2f(972, 908), hardLevel1Color);

    // هدف المستوى 1 (أسفل اليمين، عند خانة المخرج)
    RectangleShape hardGoal1(Vector2f(40.f, 40.f));
    hardGoal1.setPosition(931.f, 915.f);
    hardGoal1.setTexture(&hardGoalTexture);

    // فخاخ المستوى 1 (18 فخ - كثيرة بسبب كثافة المتاهة)
    std::vector<Trap> trapsHardLevel1;
    addTrap(trapsHardLevel1, Vector2f(749.f, 243.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(605.f, 147.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(509.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(605.f, 483.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(701.f, 819.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(221.f, 387.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(893.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(125.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(557.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(173.f, 291.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(77.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(221.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(893.f, 531.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(125.f, 867.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(413.f, 867.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(461.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(509.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel1, Vector2f(509.f, 387.f), Vector2f(20.f, 40.f));

    // عملات المستوى 1 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsHardLevel1;
    addCoin(coinsHardLevel1, Vector2f(423.f, 647.f));
    addCoin(coinsHardLevel1, Vector2f(711.f, 167.f));
    addCoin(coinsHardLevel1, Vector2f(279.f, 743.f));
    addCoin(coinsHardLevel1, Vector2f(279.f, 455.f));
    addCoin(coinsHardLevel1, Vector2f(375.f, 215.f));

    // ================= HARD LEVEL 2: متاهة كثيفة أخرى (مستوحاة من صورة الحل الأحمر) =================
    Color hardLevel2Color(255, 215, 0); // ذهبي مميز
    std::vector<RectangleShape> wallsHardLevel2;

    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(924, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 92), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(252, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(348, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(828, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 140), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(876, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(924, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 188), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(252, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(348, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 236), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 284), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(876, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 332), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(876, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 380), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(108, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(732, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(828, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(876, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 428), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(828, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 476), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(252, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(732, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 524), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(108, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(348, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(876, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 572), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(780, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(924, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 620), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(252, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(732, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 668), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(348, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(828, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 716), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(108, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(252, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(348, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(732, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(828, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 764), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(60, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(540, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(684, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(876, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 812), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(108, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(156, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(252, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(300, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(396, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(492, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(588, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(636, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(924, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(924, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 860), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(12, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(12, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(60, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(108, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(156, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(204, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(204, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(252, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(300, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(348, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(396, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(444, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(444, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(492, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(540, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(588, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(636, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(684, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(732, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(780, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(828, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 908), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(876, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(54, 6), Vector2f(924, 956), hardLevel2Color);
    addWall(wallsHardLevel2, Vector2f(6, 54), Vector2f(972, 908), hardLevel2Color);

    // هدف المستوى 2 (أسفل اليمين، عند خانة المخرج)
    RectangleShape hardGoal2(Vector2f(40.f, 40.f));
    hardGoal2.setPosition(931.f, 915.f);
    hardGoal2.setTexture(&hardGoalTexture);

    // فخاخ المستوى 2 (20 فخ - الأصعب حتى الآن)
    std::vector<Trap> trapsHardLevel2;
    addTrap(trapsHardLevel2, Vector2f(653.f, 483.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(77.f, 195.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(221.f, 531.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(317.f, 243.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(797.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(221.f, 867.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(125.f, 675.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(173.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(605.f, 291.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(461.f, 435.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(605.f, 483.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(557.f, 579.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(701.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(845.f, 867.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(749.f, 483.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(77.f, 579.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(845.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(845.f, 531.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(557.f, 147.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel2, Vector2f(173.f, 291.f), Vector2f(20.f, 40.f));

    // عملات المستوى 2 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsHardLevel2;
    addCoin(coinsHardLevel2, Vector2f(471.f, 167.f));
    addCoin(coinsHardLevel2, Vector2f(807.f, 263.f));
    addCoin(coinsHardLevel2, Vector2f(759.f, 263.f));
    addCoin(coinsHardLevel2, Vector2f(327.f, 791.f));
    addCoin(coinsHardLevel2, Vector2f(711.f, 263.f));

    // ================= HARD LEVEL 3: متاهة كثيفة جدا (مدخل أعلى اليسار، مخرج أعلى اليمين) =================
    Color hardLevel3Color(0, 255, 200); // فيروزي مميز
    std::vector<RectangleShape> wallsHardLevel3;

    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(12, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(108, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(924, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 92), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(12, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(108, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 140), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(108, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(204, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 188), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(204, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(828, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 236), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(204, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 284), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(924, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 332), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(108, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(204, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 380), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(12, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(204, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(828, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 428), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(828, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 476), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(108, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(204, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 524), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(108, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 572), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(924, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 620), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 668), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(828, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(924, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 716), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(12, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(828, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 764), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(300, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(444, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 812), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(60, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(252, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(396, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(492, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(588, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(684, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(732, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(828, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(876, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 860), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(12, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(12, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(60, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(108, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(156, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(156, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(204, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(252, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(300, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(348, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(348, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(396, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(444, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(492, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(540, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(540, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(588, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(636, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(636, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(684, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(732, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(780, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(780, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(828, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(876, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(924, 908), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(54, 6), Vector2f(924, 956), hardLevel3Color);
    addWall(wallsHardLevel3, Vector2f(6, 54), Vector2f(972, 908), hardLevel3Color);

    // هدف المستوى 3 (أعلى اليمين، عند خانة المخرج)
    RectangleShape hardGoal3(Vector2f(40.f, 40.f));
    hardGoal3.setPosition(931.f, 99.f);
    hardGoal3.setTexture(&hardGoalTexture);

    // فخاخ المستوى 3 (22 فخ - الأصعب حتى الآن)
    std::vector<Trap> trapsHardLevel3;
    addTrap(trapsHardLevel3, Vector2f(221.f, 387.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(269.f, 339.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(413.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(77.f, 339.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(221.f, 819.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(413.f, 243.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(605.f, 387.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(317.f, 723.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(845.f, 819.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(605.f, 147.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(701.f, 531.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(749.f, 243.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(893.f, 579.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(461.f, 147.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(557.f, 771.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(77.f, 291.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(509.f, 435.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(317.f, 147.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(605.f, 627.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(797.f, 435.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(605.f, 243.f), Vector2f(20.f, 40.f));
    addTrap(trapsHardLevel3, Vector2f(653.f, 531.f), Vector2f(20.f, 40.f));

    // عملات المستوى 3 (5 عملات = +500 نقطة، خاص تجمعهم قبل ما يفتح الباب)
    std::vector<Coin> coinsHardLevel3;
    addCoin(coinsHardLevel3, Vector2f(471.f, 599.f));
    addCoin(coinsHardLevel3, Vector2f(711.f, 359.f));
    addCoin(coinsHardLevel3, Vector2f(807.f, 503.f));
    addCoin(coinsHardLevel3, Vector2f(903.f, 695.f));
    addCoin(coinsHardLevel3, Vector2f(567.f, 407.f));



    while (window.isOpen())
    {
        Vector2i pixelPos = Mouse::getPosition(window);
        Vector2f mouse = window.mapPixelToCoords(pixelPos);

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Space)
                {
                    handleBackNavigation();
                    levelTimer.restart();
                }
            }

            if (event.type == Event::MouseButtonPressed)
            {
                if (state == GAMEPLAY)
                {
                    if (gameplayBackButton.getGlobalBounds().contains(mouse))
                    {
                        handleBackNavigation();
                        levelTimer.restart();
                    }
                }
            }
        }
        if (!window.isOpen())
            break;

        // تأثير تمرير الماوس فوق زر العودة
        if (state == GAMEPLAY) {
            if (gameplayBackButton.getGlobalBounds().contains(mouse)) gameplayBackButton.setFillColor(Color::Yellow);
            else gameplayBackButton.setFillColor(Color::White);
        }

        // ================= حركة اللاعب =================
        Vector2f movement(0.f, 0.f);
        Vector2f oldPos = player.getPosition();

        if (Keyboard::isKeyPressed(Keyboard::Up))    movement.y -= playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Down))  movement.y += playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Left))  movement.x -= playerSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Right)) movement.x += playerSpeed;

        if (Mouse::isButtonPressed(Mouse::Left))
        {
            if (upBtn.getGlobalBounds().contains(mouse))    movement.y -= playerSpeed;
            if (downBtn.getGlobalBounds().contains(mouse))  movement.y += playerSpeed;
            if (leftBtn.getGlobalBounds().contains(mouse))  movement.x -= playerSpeed;
            if (rightBtn.getGlobalBounds().contains(mouse)) movement.x += playerSpeed;
        }

        player.move(movement);

        // ===== فحص انتهاء الوقت =====
        float elapsedTime = levelTimer.getElapsedTime().asSeconds();
        if (elapsedTime >= levelTimeLimits[level])
        {
            resetPlayerPosition();
            levelTimer.restart();
        }

        // ===== انتهاء تأثير الإبطاء =====
        if (isSlowed && slowClock.getElapsedTime().asSeconds() >= slowDuration)
        {
            isSlowed = false;
            playerSpeed = normalSpeed;
        }

        // ===== فحص لمس الفخاخ + التصادم مع الجدران + الوصول للهدف حسب المستوى =====
        if (level == 1)
        {
            checkTraps(trapsHardLevel1);
            checkCoins(coinsHardLevel1);

            for (const auto& wall : wallsHardLevel1)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(hardGoal1.getGlobalBounds()) && countCollectedCoins(coinsHardLevel1) >= coinsRequired)
            {
                doorSound.play();
                level = 2;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 2)
        {
            checkTraps(trapsHardLevel2);
            checkCoins(coinsHardLevel2);

            for (const auto& wall : wallsHardLevel2)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(hardGoal2.getGlobalBounds()) && countCollectedCoins(coinsHardLevel2) >= coinsRequired)
            {
                doorSound.play();
                level = 3;
                resetPlayerPosition();
                levelTimer.restart();
            }
        }
        else if (level == 3)
        {
            checkTraps(trapsHardLevel3);
            checkCoins(coinsHardLevel3);

            for (const auto& wall : wallsHardLevel3)
            {
                if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
                {
                    player.setPosition(oldPos);
                    break;
                }
            }

            if (player.getGlobalBounds().intersects(hardGoal3.getGlobalBounds()) && countCollectedCoins(coinsHardLevel3) >= coinsRequired)
            {
                doorSound.play();
                // آخر مستوى فـ Hard حاليا - يرجع لبداية المستوى 3 (تقدر تزيد مستوى 4 من هنا)
                resetPlayerPosition();
                levelTimer.restart();
            }
        }

        // تحديث نص الوقت المتبقي
        float remainingTime = levelTimeLimits[level] - levelTimer.getElapsedTime().asSeconds();
        if (remainingTime < 0.f) remainingTime = 0.f;
        timerText.setString("Time: " + std::to_string((int)remainingTime));

        // تحديث نص النقاط + عدد العملات المجموعة فالمستوى الحالي
        int currentCoinsCollected = 0;
        if (level == 1) currentCoinsCollected = countCollectedCoins(coinsHardLevel1);
        else if (level == 2) currentCoinsCollected = countCollectedCoins(coinsHardLevel2);
        else if (level == 3) currentCoinsCollected = countCollectedCoins(coinsHardLevel3);
        scoreText.setString("Score: " + std::to_string(score) + "   Coins: " + std::to_string(currentCoinsCollected) + "/" + std::to_string(coinsRequired));

        // ================= DRAWING =================
        window.clear(Color::Black);

        if (level == 1)
        {
            for (const auto& wall : wallsHardLevel1) window.draw(wall);
            hardGoal1.setFillColor(countCollectedCoins(coinsHardLevel1) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(hardGoal1);
            drawTraps(trapsHardLevel1);
            drawCoins(coinsHardLevel1);
        }
        else if (level == 2)
        {
            for (const auto& wall : wallsHardLevel2) window.draw(wall);
            hardGoal2.setFillColor(countCollectedCoins(coinsHardLevel2) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(hardGoal2);
            drawTraps(trapsHardLevel2);
            drawCoins(coinsHardLevel2);
        }
        else if (level == 3)
        {
            for (const auto& wall : wallsHardLevel3) window.draw(wall);
            hardGoal3.setFillColor(countCollectedCoins(coinsHardLevel3) >= coinsRequired ? Color(255, 255, (Uint8)(200 + 55 * std::sin(animClock.getElapsedTime().asSeconds() * 3.f))) : Color(90, 90, 90));
            window.draw(hardGoal3);
            drawTraps(trapsHardLevel3);
            drawCoins(coinsHardLevel3);
        }

        window.draw(gameplayBackButton);
        window.draw(timerText);
        window.draw(scoreText);

        // أزرار التحكم اللمسية
        window.draw(upBtn);    window.draw(upTxt);
        window.draw(downBtn);  window.draw(downTxt);
        window.draw(leftBtn);  window.draw(leftTxt);
        window.draw(rightBtn); window.draw(rightTxt);

        window.draw(player);
        window.display();
    }
}
