#pragma once
#include "ScrollWindow.h"
#include "ownUtilities.h"
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

class DataBase
{
public:
    DataBase();
    ~DataBase();
    struct Polynomial {
        uint16_t serial = 0, pack_id = 0, item_id = 0;
        std::list<std::pair<int, int> > chars;
        PxButton *inlist = nullptr;
        std::string astext;
    };
    struct Library {
        PxButton * head, * cls;
        std::list<Polynomial> base;
        std::string name = "Unknown";
        std::wstring path;
        uint16_t id = 0;
        bool connected = false, inner = true;
        Library(PxButton * h, PxButton * c, std::list<Polynomial> l, std::string n, std::wstring p, bool d = 0) : 
            head(h), cls(c), base(l), name(n), path(p), connected(d)
        {}
    };
    void loadFromFile(std::wstring fileName, std::string name, std::wstring path, uint16_t pack_id);
    void saveToFile(Polynomial p, char * fileName);
    int windowMode = 1;
    // DataBaseWindow
    const std::string configDir = "data/";
    sf::Font xF, bf, df;
    sf::RenderTexture * rt = nullptr;
    std::wstring dataPath = L"databases/", swPathName;
    std::string outText = "";
    std::list <DataBase::Library> dataBase;
    Polynomial polyFromString(std::string raw, size_t num, std::string &clm);
    ScrollWindow *dbScroll, *polyScroll, * qdScroll, * swScroll;
    PxButton * dbClose, * dbLb, * dbDelete, * dbSettings, * dbUp, * dbDown, * dbToMain, * dbOpen;
    PxButton * dbAddFile, * dbAddNew, * dbByK, * dbByP, * dbByC;
    PxButton * qdOK, *qdCancel;
    PxButton * swOK, * swCancel, *swGetPath;
    ClrBox * qdQuestion;
    ClrBox * swHeader, * swNameLb, *swPathLb, * swName, * swPath;
    sf::Sprite * drawPolynomial(DataBase::Polynomial p, float args[8], uint16_t W, uint16_t H, sf::Color txtClr = sf::Color::Black);
    PxButton * addInList(Polynomial & p);
    void initBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H, sf::Font &F, ownUtilities *owt);
    void initQuestion(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void initSettings(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void initBottomButtons(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void initTopButtons(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void deleteBoxes();
    void drawBoxes(sf::RenderWindow *window);
    void resizeBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H);
    void loadBindedBases(char * path);
    void checkPressed(int X, int Y);
    void checkMoved(int X, int Y, float dx, float dy);
    void checkReleased(int X, int Y);
    void checkDoubleClick(int X, int Y);
    void checkKeyPressed(sf::Event::KeyEvent key);
    void checkTextEntered(sf::Event::TextEvent text);
    void openQuestion(std::string text, void(*react)(DataBase * db, bool answer), bool hascancel = true);
    void openSettings(std::string nm, std::string pt, void (*react)(DataBase * db, bool answer));
    void closeQuestion();
    void createLibrary();
    void sortList(std::list<Polynomial> & base);
    bool saveLibrary(std::list<Library>::iterator lib);
    bool savePaths();

    Polynomial sum(Polynomial a, Polynomial b);
    Polynomial subtract(Polynomial a, Polynomial b);
    Polynomial multiply(Polynomial a, Polynomial b);
    Polynomial derivative(Polynomial p);
    double getvalue(Polynomial p, double x);
private:
    std::list <PxButton *> buttonHeads, closeHeads, listButtons;
    std::list<std::wstring> dataPaths, dataNames;
    std::list<Polynomial>::iterator selectedPoly;
    std::list<Library>::iterator selectedHead = dataBase.end(), selectedPolyC = dataBase.end();
    int selectedType = 1, sortType = 1, qdType = 0;
    void(*DataBase::qDialReact)(DataBase * db, bool answer) = nullptr;
    void (*DataBase::swDialReact)(DataBase * db, bool answer) = nullptr;
    const uint16_t DB_W = 200, BT_H = 24, PL_T = 30, PL_B = 60, QD_W = 400, QD_H = 135, SW_W = 400, SW_H = 220, SW_TXTW = 330, SW_TNY = 50, SW_TPY = 120;
    uint16_t DB_H = 500;
    sf::Sprite * qdBack, *qdWarning;
    sf::Sprite * swBack, * swCorners, * swTextBody;
    sf::Sprite * dbBackGround, * dbLeftBound, * dbTopBorder;
    int convert_from(char * inp, size_t n);
    float frontArgs[8] = { 15, 10, 1, -3, 4, 5, 0, 0 };
    std::pair<PxButton *, PxButton* > addDataHead(char * name, uint16_t num, sf::Color bck = sf::Color::White, sf::Color pclr = sf::Color(224, 241, 255));
    ownUtilities * owt;
    void updateSortType(int type);
    void focusAtHead(std::list<Library>::iterator it);
    void focusAtPoly(std::list<Library>::iterator cont, std::list<Polynomial>::iterator it);
    void moveFocusedUp();
    void moveFocusedDown();
    Polynomial getFromMap(std::map<int, int> &pm);
    double pow(double x, int p, double max = 1e19);
    friend void deleteFocused(DataBase * db, bool answer);
    friend void deletePoly(DataBase * db, bool answer);
    friend void updateFocused(DataBase * db, bool answer);
    friend void addLibrary(DataBase * db, bool answer);
    friend void loadLibrary(DataBase * db, bool answer);
};

