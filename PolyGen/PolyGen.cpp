#pragma once
#include <chrono>
#include <set>
#include <iostream>
#include "MainMap.h"
#include "ScrollWindow.h"
#include "ownUtilities.h"
#include "DataBase.h"
#include "GraphMap.h"

float SCROLL_OFFSET = 0.9f;

const char* PROGRAM_NAME = "PolyPolia";
uint16_t WINDOW_W = 820, WINDOW_H = 600;
const uint16_t LEFT_W = 265, MAX_PLAYERS = 10, FIELD_W = 500;
const float USS = 2, ProgramFrameTime = 1000 / 240; // 1000ms / 60frames
MainMap mainMap(WINDOW_W, WINDOW_H, PROGRAM_NAME);
DataBase mainDB;
auto gameFrameTime = std::chrono::steady_clock::now();
int windowMode = 1, length = 200, doubleClickTimer = -1;
float mouseX = 0, mouseY = 0;
uint64_t frameTime = 0;
sf::Font bF, tF;

DataBase::Polynomial frontModel, * drawA, * drawB;
sf::RenderWindow *window;
ownUtilities owt;

sf::FloatRect mindFrame(-6, -6, 12, 12), frame(0, 0, 228, 228);

std::pair<double, double> getres(double x)
{
    std::string acs = "A: ";
    double a = mainDB.getvalue(*drawA, x);
    /*if (abs(a) < 1e-3)
    {
        int rk = std::round(x);
        if (mainDB.getvalue(*drawA, rk) == 0)
            acs += std::to_string(rk) + " ";
    }*/
    acs += "| B: ";
    double b = mainDB.getvalue(*drawB, x);
   /* if (abs(b) < 1e-3)
    {
        int rk = std::round(x);
        if (mainDB.getvalue(*drawB, rk) == 0)
            acs += std::to_string(rk) + " ";
    }
    printf("%s\n", acs);*/
    return std::make_pair(a, b);

}

int main()
{
    bF.loadFromFile(owt.fontFamily);
    tF.loadFromFile(owt.textFontFamily);
    mainDB.df.loadFromFile(owt.textFontFamily);
    mainDB.xF.loadFromFile(mainMap.fontFamily);
    mainDB.initBoxes(WINDOW_W, WINDOW_H, bF, &owt);
    mainDB.loadBindedBases("data/paths.ini");
    mainMap.dataBase = &mainDB;
    mainMap.initBoxes(WINDOW_W, WINDOW_H, mainDB.xF, bF, &owt);
    window = mainMap.getWindow();
    window->setFramerateLimit(60);
    while (window->isOpen())
    {
        sf::Event e;
        while (window->pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                mainDB.deleteBoxes();
                window->close();
                return 0;
            }
            int X = e.mouseButton.x, Y = e.mouseButton.y;
            if (windowMode == 1)
            {
                if (e.type == sf::Event::MouseMoved) { 
                    mainDB.checkMoved(e.mouseMove.x, e.mouseMove.y, mouseX - e.mouseMove.x, mouseY - e.mouseMove.y);
                    if (mainDB.windowMode < 2)
                        mainMap.checkMoved(e.mouseMove.x, e.mouseMove.y, mouseX - e.mouseMove.x, mouseY - e.mouseMove.y);
                }
                else if (e.type == sf::Event::MouseWheelScrolled) {
                }
                else if (e.type == sf::Event::MouseButtonPressed) {
                    mainDB.checkPressed(X, Y);
                    if(mainDB.windowMode < 2 || mainDB.windowMode == 4)
                        mainMap.checkPressed(X, Y);
                }
                else if (e.type == sf::Event::MouseButtonReleased) {
                    if (doubleClickTimer > 0)
                    {
                        mainDB.checkDoubleClick(X, Y);
                    }
                    else doubleClickTimer = 20;
                    mainDB.checkReleased(X, Y);
                    if (mainDB.windowMode < 2 || mainDB.windowMode == 4)
                        mainMap.checkReleased(X, Y);
                    if (mainMap.outCode == 1)
                    {
                        drawA = mainMap.outA;
                        drawB = mainMap.outB;
                        GraphMap * graphMap = new GraphMap(frame.width, frame.height, "Comparation");
                        sf::RenderWindow* grw = graphMap->getWindow();
                        graphMap->fclr = sf::Color(240, 0, 0);
                        graphMap->sclr = sf::Color(0, 0, 240);
                        graphMap->hasTwoGrs = true;
                        bool mouseLeftPressed = false;
                        while (grw->isOpen())
                        {
                            sf::Event e;
                            while (grw->pollEvent(e))
                            {
                                if (e.type == sf::Event::Closed)
                                    grw->close();
                                else if (e.type == sf::Event::Resized)
                                {
                                    if (e.size.height < 100) 
                                        grw->setSize(sf::Vector2u(e.size.width, 100)), e.size.height = 100;
                                    float delta = (e.size.height + 0.f - frame.height) * mindFrame.height / frame.height;
                                    mindFrame.top -= delta;
                                    mindFrame.height += delta;
                                    mindFrame.width += (e.size.width + 0.f - frame.width) * mindFrame.width / frame.width;
                                    frame.width = e.size.width;
                                    frame.height = e.size.height;
                                }
                                else if (e.type == sf::Event::MouseWheelScrolled)
                                {
                                    float offset = powf(SCROLL_OFFSET, e.mouseWheelScroll.delta);
                                    float dx = mindFrame.width * offset - mindFrame.width;
                                    float dy = mindFrame.height * offset - mindFrame.height;
                                    mindFrame.left -= e.mouseWheelScroll.x * dx / frame.width;
                                    mindFrame.top -= (frame.height - e.mouseWheelScroll.y) * dy / frame.height;
                                    mindFrame.width *= offset;
                                    mindFrame.height *= offset;
                                }
                                else if (e.type == sf::Event::MouseButtonPressed)
                                {
                                    if (e.mouseButton.button == sf::Mouse::Left)
                                        mouseLeftPressed = true;
                                    mouseX = e.mouseButton.x;
                                    mouseY = e.mouseButton.y;
                                }
                                else if (e.type == sf::Event::MouseButtonReleased)
                                {
                                    if (e.mouseButton.button == sf::Mouse::Left)
                                        mouseLeftPressed = false;
                                    mouseX = e.mouseButton.x;
                                    mouseY = e.mouseButton.y;
                                }
                                else if (e.type == sf::Event::MouseEntered)
                                {
                                    mouseX = e.mouseButton.x;
                                    mouseY = e.mouseButton.y;
                                }
                                else if (e.type == sf::Event::MouseMoved)
                                {
                                    if (mouseLeftPressed)
                                    {
                                        float delta_x = e.mouseMove.x - mouseX;
                                        float delta_y = e.mouseMove.y - mouseY;
                                        mindFrame.left -= delta_x / frame.width * mindFrame.width;
                                        mindFrame.top += delta_y / frame.height * mindFrame.height;
                                    }
                                    mouseX = e.mouseMove.x;
                                    mouseY = e.mouseMove.y;
                                }
                            }
                            graphMap->updateGraph(frame, mindFrame, getres);
                        }
                        delete graphMap;
                        mainMap.outCode = 0;
                    }
                }
                else if (e.type == sf::Event::KeyPressed) { 
                    mainDB.checkKeyPressed(e.key);
                    mainMap.checkKeyPressed(e.key);
                }
                else if (e.type == sf::Event::TextEntered)
                { 
                    mainDB.checkTextEntered(e.text);
                    mainMap.checkTextEntered(e.text);
                }
            }
            else if (windowMode == 2)
            {
                if (e.type == sf::Event::KeyPressed) {}
                else if (e.type == sf::Event::MouseButtonPressed) { }
                else if (e.type == sf::Event::MouseButtonReleased) { }
            }
            else if (windowMode == 3)
            {
                if (e.type == sf::Event::MouseMoved) { }
                else if (e.type == sf::Event::MouseButtonPressed) { }
                else if (e.type == sf::Event::MouseButtonReleased) { }
                else if (e.type == sf::Event::KeyPressed) { }
                else if (e.type == sf::Event::TextEntered) { }
            }
            if (e.type == sf::Event::MouseMoved)
            {
                mouseX = e.mouseMove.x;
                mouseY = e.mouseMove.y;
            }
            else if (e.type == sf::Event::Resized)
            {
                WINDOW_W = e.size.width, WINDOW_H = e.size.height;
                window->setView(sf::View(sf::FloatRect(0, 0, WINDOW_W, WINDOW_H)));
                mainDB.resizeBoxes(WINDOW_W, WINDOW_H);
                mainMap.resizeBoxes(WINDOW_W, WINDOW_H);
            }
        }
        --doubleClickTimer;
        window->clear(sf::Color(215, 215, 215, 255));
        mainMap.drawBoxes();
        mainDB.drawBoxes(window);
        window->display();
    }
}