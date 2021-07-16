#pragma once
#include "ScrollWindow.h"
#include "DataBase.h"
#include <queue>
#include <vector>
class MainMap
{
private:
    const int SHUFFLE_NUM = 3, APPL = 505, TAIL = 404;
    int size = 10;
    const float KK = 0.3;
public:
    const sf::String fontFamily = "fonts/ufonts.com_ydp-math-bold-italic.ttf";
    const sf::String boldFontFamily = "fonts/Nunito-bold.ttf";
    MainMap(uint16_t width, uint16_t height, const char* programName);
    ~MainMap();
    sf::RenderWindow* getWindow();
    sf::RenderWindow window;
    sf::Image frontMap;
    sf::Sprite mainGame;
    DataBase::Polynomial *outA = nullptr, *outB = nullptr;
// Left window
    DataBase * dataBase;
    ownUtilities * owt;
    int outCode = 0;
    void initBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H, sf::Font &xF, sf::Font &bF, ownUtilities *ow);
    void drawBoxes();
    void drawBigButton(ScrollWindow * plot, PxButton * btn, sf::Sprite * cornerL, sf::Sprite * cornerR, sf::Sprite * body);
    void checkPressed(int X, int Y);
    void checkMoved(int X, int Y, float dx, float dy);
    void checkReleased(int X, int Y);
    void resizeBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void checkKeyPressed(sf::Event::KeyEvent key);
    void checkTextEntered(sf::Event::TextEvent text);
    void updateMainColorMap();
    void updateAColorMap();
    void updateBColorMap();
private:
    uint16_t MI_W = 400, MI_H = 360, MI_CH = 10, MI_SH = 100, MI_L = 10, MI_R = 300;
    uint16_t AP_W = 260, AP_H = 500, BT_H = 24, OP_H = 300, OP_L = 50, MI_A = 80, MI_OPL = 530;
    uint16_t BB_L = 540, BB_R = 490;
    float frontArgs[8] = { 60, 23, 3, -10, 4, 10, 0, 0 };
    std::map<char, sf::Color> frontColors;
    int windowMode = 1, selecetdLibrary = -1, lastInput = 1, predHeight = 0, opMode = 0;
    bool isFormatted = true;
    uint16_t currentWidth, currentHeight;
    sf::Font tf, xf, bf;
    sf::Sprite * miInputBack, * miInputBorders, * miShowBack, * formatOff, * formatOn;
    sf::Sprite * opBackA, *opBordersA, *opBackB, *opBordersB, *opLabelA, *opLabelB;
    sf::Sprite * opTextureA, *opTextureB, *opTextureF, *opTextureX;
    sf::Sprite * opAdd, *opSubt, *opMult, *opValue, * opDeriv, * opGraph, *opNow;
    sf::Sprite * polyShowSprite = nullptr, * polyAddSprite, * polyListSprite;
    sf::Sprite * bigCornerL, * bigCornerR, *bigBody;
    ScrollWindow * miScroll, * addPolyScroll, * listPolyScroll, *opScroll;
    ClrBox * miInput, * miInputLb, * miAddText;
    ClrBox * opHeader, *opA, *opB;
    PxButton * miAddPoly, *miCopyPoly, * miAdd, * miSubt, * miMult, * miValue, * miDeriv, * miGraph;
    PxButton * miFormatBtn, *miToA, *miToB;
    PxButton * opSwap, * opClose, * opStart, * opCompare;
    PxButton * addOK, *addCancel;
    DataBase::Polynomial currentPoly, currentA, currentB;
    bool isFloat(std::string str);
    void initInputBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void initBigButtons(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void initOperations(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void initAddPoly(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void setBigButtonsPos();
    void setOperation(int mode);
};