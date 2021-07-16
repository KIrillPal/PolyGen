#pragma once
#include "MainMap.h"
#include <queue>
#include <cstring>
#include <algorithm>
sf::RenderWindow window;

MainMap::MainMap(uint16_t width, uint16_t height, const char* programName)
{
    window.create(sf::VideoMode(width, height), programName);
    currentWidth = width;
    currentHeight = height;
}

sf::RenderWindow* MainMap::getWindow()
{
    return &window;
}

void MainMap::initBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H, sf::Font &xF, sf::Font &bF, ownUtilities *ow)
{
    xf = xF, tf = bF;
    bf.loadFromFile(boldFontFamily);
    owt = ow;
    frontColors['k'] = sf::Color(0, 12, 159);
    frontColors['x'] = sf::Color(0, 0, 0);
    frontColors['c'] = sf::Color(0, 0, 0);
    frontColors['p'] = sf::Color(0, 101, 175);
    frontColors['d'] = sf::Color(0, 112, 69);
    frontColors['-'] = sf::Color(124, 124, 124);
    MI_W = WINDOW_W - MI_L - MI_R;
    miScroll = new ScrollWindow(sf::FloatRect(MI_L, MI_CH + predHeight, WINDOW_W - MI_L - MI_R + MI_A, MI_H),
        sf::FloatRect(0, 0, WINDOW_W - MI_L - MI_R + MI_A, MI_H));
    initInputBoxes(WINDOW_W, WINDOW_H);
    initBigButtons(WINDOW_W, WINDOW_H);
    initOperations(WINDOW_W, WINDOW_H);
    updateMainColorMap();
}

void MainMap::initInputBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    miInputBorders = owt->createBackground(sf::FloatRect(10, 10, MI_W - 20, 48), "", 0);
    miInputBorders->setColor(sf::Color(150, 150, 150));
    miInputBack = owt->createBackground(sf::FloatRect(13, 13, MI_W - 26, 42), "textures/white.png", 0);
    miInputBack->setScale((MI_W - 26) / 16.f, 42 / 16.f);
    miShowBack = owt->createBackground(sf::FloatRect(13, 80, MI_W - 20, MI_SH), "textures/white.png", 0);
    miShowBack->setScale((MI_W - 20) / 16.f, MI_SH / 16.f);
    miShowBack->setColor(sf::Color(0, 0, 50));

    miInput = new ClrBox("", sf::FloatRect(28, 18, MI_W - 55, 42), 24, bf);
    miInput->isActive = true;
    miInput->setMovingView(true);
    miInput->colorDictionary = &frontColors;
    miInputLb = new ClrBox("Write by keyboard or paste from library", sf::FloatRect(14, 58, MI_W, 42), 12, tf);
    miInputLb->resultBox.setFillColor(sf::Color(0, 148, 255));

    miFormatBtn = new PxButton(sf::FloatRect(MI_W, 22, 24, 24), sf::Color::White, 12, "", bf);
    miFormatBtn->setColor(sf::Color::White, 1, 0.93, 0.85);
    formatOff = owt->createBackground(miFormatBtn->control, "textures/format_off.png", 0);
    formatOn = owt->createBackground(miFormatBtn->control, "textures/format_on.png", 0);
    miFormatBtn->setSprite(formatOn);
    miFormatBtn->hasSpecialSprite = true;
    miToA = new PxButton(sf::FloatRect(MI_W + 28, 22, 24, 24), sf::Color::White, 12, "", bf);
    miToA->setColor(sf::Color::White, 1, 0.93, 0.85);
    miToA->setSprite(owt->createBackground(miToA->control, "textures/to_a.png", 0));
    miToB = new PxButton(sf::FloatRect(MI_W + 56, 22, 24, 24), sf::Color::White, 12, "", bf);
    miToB->setColor(sf::Color::White, 1, 0.93, 0.85);
    miToB->setSprite(owt->createBackground(miToB->control, "textures/to_b.png", 0));
    miToB->offColor = sf::Color::Transparent;
}

void MainMap::initBigButtons(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    bigCornerL = owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/big_btn.png", 0);
    bigCornerR = owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/big_btn.png", 0);
    bigBody = owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0);

    miAddPoly = new PxButton(sf::FloatRect(MI_L, MI_CH + MI_SH + 70, 200, 56), sf::Color(255, 0, 85), 140, "ADD TO LIBRARY", bf);
    miAddPoly->mainText.setFillColor(sf::Color::White);
    miAddPoly->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miAddPoly->backGround->setScale(0, 0);
    miAddPoly->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miAddPoly->offColor = sf::Color(165, 0, 54);
    initAddPoly(WINDOW_W, WINDOW_H);
    miCopyPoly = new PxButton(sf::FloatRect(MI_L + 210, MI_CH + MI_SH + 70, 200, 56), sf::Color(255, 0, 85), 93, "COPY TEXT", bf);
    miCopyPoly->mainText.setFillColor(sf::Color::White);
    miCopyPoly->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miCopyPoly->backGround->setScale(0, 0);
    miCopyPoly->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miCopyPoly->offColor = sf::Color(165, 0, 54);
    miGraph = new PxButton(sf::FloatRect(MI_L + 360, MI_CH + MI_SH + 70, 130, 56), sf::Color(255, 0, 85), 12, "", bf);
    miGraph->mainText.setFillColor(sf::Color::White);
    miGraph->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/graph.png", 0));
    miGraph->backGround->setOrigin(-35, 0);
    miGraph->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miGraph->offColor = sf::Color(165, 0, 54);

    miAdd = new PxButton(sf::FloatRect(MI_L + MI_OPL, MI_CH + MI_SH + 70, 80, 56), sf::Color(255, 0, 85), 46, "A + B", bf);
    miAdd->mainText.setFillColor(sf::Color::White);
    miAdd->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miAdd->backGround->setScale(0, 0);
    miAdd->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miAdd->offColor = sf::Color(165, 0, 54);
    miSubt = new PxButton(sf::FloatRect(MI_L + MI_OPL + 85, MI_CH + MI_SH + 70, 80, 56), sf::Color(255, 0, 85), 46, "A - B", bf);
    miSubt->mainText.setFillColor(sf::Color::White);
    miSubt->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miSubt->backGround->setScale(0, 0);
    miSubt->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miSubt->offColor = sf::Color(165, 0, 54);
    miMult = new PxButton(sf::FloatRect(MI_L + MI_OPL + 170, MI_CH + MI_SH + 70, 80, 56), sf::Color(255, 0, 85), 46, "A x B", bf);
    miMult->mainText.setFillColor(sf::Color::White);
    miMult->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miMult->backGround->setScale(0, 0);
    miMult->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miMult->offColor = sf::Color(165, 0, 54);
    miValue = new PxButton(sf::FloatRect(MI_L + MI_OPL + 255, MI_CH + MI_SH + 70, 100, 56), sf::Color(255, 0, 85), 59, "VALUE", bf);
    miValue->mainText.setFillColor(sf::Color::White);
    miValue->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miValue->backGround->setScale(0, 0);
    miValue->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miValue->offColor = sf::Color(165, 0, 54);
    miDeriv = new PxButton(sf::FloatRect(MI_L + MI_OPL + 360, MI_CH + MI_SH + 70, 130, 56), sf::Color(255, 0, 85), 80, "DERIVATIVE", bf);
    miDeriv->mainText.setFillColor(sf::Color::White);
    miDeriv->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    miDeriv->backGround->setScale(0, 0);
    miDeriv->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    miDeriv->offColor = sf::Color(165, 0, 54);
    setBigButtonsPos();
}

void MainMap::initOperations(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    opScroll = new ScrollWindow(sf::FloatRect(MI_L, MI_CH, WINDOW_W - MI_L - MI_R + 60, OP_H),
        sf::FloatRect(0, 0, WINDOW_W - MI_L - MI_R + 60, OP_H));
    opHeader = new ClrBox("Sum Operation", sf::FloatRect(10, 10, 300, 40), 18, bf);

    opBordersA = owt->createBackground(sf::FloatRect(10 + OP_L, 45, MI_W - 20 - OP_L, 48), "", 0);
    opBordersA->setColor(sf::Color(150, 150, 150));
    opBackA = owt->createBackground(sf::FloatRect(13 + OP_L, 48, MI_W - 26 - OP_L, 42), "textures/white.png", 0);
    opBackA->setScale((MI_W - 26 - OP_L) / 16.f, 42 / 16.f);

    opBordersB = owt->createBackground(sf::FloatRect(10 + OP_L, 105, MI_W - 20 - OP_L, 48), "", 0);
    opBordersB->setColor(sf::Color(150, 150, 150));
    opBackB = owt->createBackground(sf::FloatRect(13 + OP_L, 108, MI_W - 26 - OP_L, 42), "textures/white.png", 0);
    opBackB->setScale((MI_W - 26 - OP_L) / 16.f, 42 / 16.f);
    opTextureA = owt->createBackground(sf::FloatRect(10, 45, 48, 48), "textures/a_label.png", 0);
    opTextureB = owt->createBackground(sf::FloatRect(10, 105, 48, 48), "textures/b_label.png", 0);
    opTextureF = owt->createBackground(sf::FloatRect(10, 45, 48, 48), "textures/f_label.png", 0);
    opTextureX = owt->createBackground(sf::FloatRect(10, 105, 48, 48), "textures/x_label.png", 0);

    opSwap = new PxButton(sf::FloatRect(MI_W, 84, 24, 24), sf::Color::White, 12, "", bf);
    opSwap->setColor(sf::Color::White, 1, 0.93, 0.85);
    opSwap->setSprite(owt->createBackground(opSwap->control, "textures/swap.png", 0));
    opSwap->offColor = sf::Color::Transparent;

    opA = new ClrBox("", sf::FloatRect(28 + OP_L, 53, MI_W - 106, 42), 24, bf);
    opA->setMovingView(true);
    opB = new ClrBox("", sf::FloatRect(28 + OP_L, 113, MI_W - 106, 42), 24, bf);
    opB->setMovingView(true);
    opA->isActive = opB->isActive = true;
    opA->colorDictionary = opB->colorDictionary = &frontColors;

    opClose = new PxButton(sf::FloatRect(MI_W, 14, 24, 24), sf::Color::White, 12, "", bf);
    opClose->setColor(sf::Color::White, 0.85, 0.9, 0.85);
    opClose->setSprite(owt->createBackground(opClose->control, "textures/op_close.png", 0));

    opStart = new PxButton(sf::FloatRect((MI_W - 300 + 60) / 2, 163, 300, 56), sf::Color(255, 0, 85), 140, "GET ANSWER", bf);
    opStart->mainText.setFillColor(sf::Color::White);
    opStart->setSprite(owt->createBackground(sf::FloatRect(0, 0, 56, 56), "textures/white.png", 0));
    opStart->backGround->setScale(0, 0);
    opStart->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    opStart->offColor = sf::Color(165, 0, 54);
    opStart->setIsEnabled(false);

    opAdd = owt->createBackground(sf::FloatRect(10, 167, 48, 48), "textures/plus_label.png", 0);
    opSubt = owt->createBackground(sf::FloatRect(10, 167, 48, 48), "textures/minus_label.png", 0);
    opMult = owt->createBackground(sf::FloatRect(10, 167, 48, 48), "textures/dot_label.png", 0);
    opValue = owt->createBackground(sf::FloatRect(10, 167, 48, 48), "textures/value_label.png", 0);
    opDeriv = owt->createBackground(sf::FloatRect(10, 105, 48, 48), "textures/deriv_label.png", 0);
    opGraph = owt->createBackground(sf::FloatRect(10, 167, 48, 48), "textures/graph_label.png", 0);
}

void MainMap::initAddPoly(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    addPolyScroll = new ScrollWindow(sf::FloatRect((WINDOW_W - AP_W) / 2, (WINDOW_H - AP_H) / 2, AP_W, AP_H),
        sf::FloatRect(0, 0, AP_W, AP_H));
    polyAddSprite = owt->createBackground(sf::FloatRect(0, 0, AP_W, AP_H), "", 0);
    listPolyScroll = new ScrollWindow(sf::FloatRect((WINDOW_W - AP_W) / 2 + 30, (WINDOW_H - AP_H) / 2 + 40, AP_W - 60, AP_H - 80),
        sf::FloatRect(0, 0, AP_W - 60, AP_H - 80));
    polyListSprite = owt->createBackground(sf::FloatRect(0, 0, AP_W - 60, AP_H - 80), "textures/white.png", 0);
    polyListSprite->setColor(sf::Color(230, 230, 230));
    polyListSprite->setScale((AP_W - 60) / 16.f, (AP_H - 80) / 16.f);
    miAddText = new ClrBox("SELECT LIBRARY", sf::FloatRect(55, 10, AP_W, 30), 18, tf);
    addOK = new PxButton(sf::FloatRect(AP_W - 130, AP_H - 32, 72, 24), sf::Color::Green, 24, "OK", bf);
    addOK->setSprite(owt->createBackground(addOK->control, "textures/dial_btn.png", 0));
    addOK->mainText.setFillColor(sf::Color::White);
    addOK->offColor = sf::Color(0, 200, 0, 128);
    addOK->setIsEnabled(false);

    addCancel = new PxButton(sf::FloatRect(AP_W - 204, AP_H - 32, 72, 24), sf::Color::Red, 52, "Cancel", bf);
    addCancel->setSprite(owt->createBackground(addCancel->control, "textures/dial_btn.png", 0));
    addCancel->mainText.setFillColor(sf::Color::White);
    addCancel->offColor = sf::Color::Transparent;
}

void MainMap::setBigButtonsPos()
{
    if (MI_W > BB_L + BB_R)
    {
        miDeriv->setPosition(MI_W - 140, MI_CH + MI_SH + 70);
        miValue->setPosition(MI_W - 245, MI_CH + MI_SH + 70);
        miMult->setPosition(MI_W - 330, MI_CH + MI_SH + 70);
        miSubt->setPosition(MI_W - 415, MI_CH + MI_SH + 70);
        miAdd->setPosition(MI_W - 500, MI_CH + MI_SH + 70);
        miAddPoly->setPosition(MI_L, MI_CH + MI_SH + 70);
        miCopyPoly->setPosition(MI_L + 210, MI_CH + MI_SH + 70);
        miGraph->setPosition(MI_L + 420, MI_CH + MI_SH + 70);
    }
    else 
    {
        if (MI_W > BB_L)
        {
            int x = (MI_W - BB_L) / 2;
            miAddPoly->setPosition(x, MI_CH + MI_SH + 70);
            miCopyPoly->setPosition(x + 210, MI_CH + MI_SH + 70);
            miGraph->setPosition(x + 420, MI_CH + MI_SH + 70);
        }
        else
        {
            int x = (MI_W - BB_L + 140) / 2;
            miAddPoly->setPosition(x, MI_CH + MI_SH + 70);
            miCopyPoly->setPosition(x + 210, MI_CH + MI_SH + 70);
            miGraph->setPosition(MI_W / 2 - 65, MI_CH + MI_SH + 192);
        }
        int x = std::max((MI_W - BB_R) / 2, 10);
        miDeriv->setPosition(x + 360, MI_CH + MI_SH + 131);
        miValue->setPosition(x + 255, MI_CH + MI_SH + 131);
        miMult->setPosition(x + 170, MI_CH + MI_SH + 131);
        miSubt->setPosition(x + 85, MI_CH + MI_SH + 131);
        miAdd->setPosition(x, MI_CH + MI_SH + 131);
    }
}

void MainMap::drawBoxes()
{
    miScroll->openDrawing();
    miScroll->draw(miShowBack, 10, 74);
    miScroll->draw(miInputBorders);
    miScroll->draw(miInputBack);

    miInput->update();
    miScroll->draw(miInput, 4);
    miScroll->draw(polyShowSprite, 28, 80);
    miScroll->draw(miInputLb, 1);
    miScroll->draw(miFormatBtn);
    if (opMode)
    {
        miScroll->draw(miToA);
        miScroll->draw(miToB);
    }
    drawBigButton(miScroll, miAddPoly, bigCornerL, bigCornerR, bigBody);
    drawBigButton(miScroll, miCopyPoly, bigCornerL, bigCornerR, bigBody);
    drawBigButton(miScroll, miGraph, bigCornerL, bigCornerR, bigBody);
    miGraph->setColor(sf::Color::White, 1, 0.9, 0.8);
    miScroll->draw(miGraph->backGround);
    miGraph->setColor(sf::Color(206, 0, 68), 1, 0.9, 0.8);
    drawBigButton(miScroll, miAdd, bigCornerL, bigCornerR, bigBody);
    drawBigButton(miScroll, miSubt, bigCornerL, bigCornerR, bigBody);
    drawBigButton(miScroll, miMult, bigCornerL, bigCornerR, bigBody);
    drawBigButton(miScroll, miValue, bigCornerL, bigCornerR, bigBody);
    drawBigButton(miScroll, miDeriv, bigCornerL, bigCornerR, bigBody);
    miScroll->uniteDrawing(&window);
    if (opMode)
    {
        opA->update(); opB->update();

        opScroll->openDrawing();
        opScroll->draw(opHeader, 0);
        opScroll->draw(opBordersA);
        opScroll->draw(opBackA);
        if (opMode != 5) opScroll->draw(opBordersB);
        if (opMode != 5) opScroll->draw(opBackB);
        opScroll->draw(opLabelA);
        if (opMode != 5) opScroll->draw(opLabelB);
        opScroll->draw(opSwap);
        opScroll->draw(opA, 4);
        if (opMode != 5) opScroll->draw(opB, 4);
        opScroll->draw(opClose);
        if(opMode == 5) opStart->setPosition((MI_W - 300 + 60) / 2, 103);
        else opStart->setPosition((MI_W - 300 + 60) / 2, 163);
        drawBigButton(opScroll, opStart, bigCornerL, bigCornerR, bigBody);
        opScroll->draw(opNow);
        opScroll->uniteDrawing(&window);
    }
    if (windowMode == 2)
    {
        addPolyScroll->openDrawing();
        addPolyScroll->draw(polyAddSprite);
        addPolyScroll->draw(miAddText, 1);
        addPolyScroll->draw(addOK);
        addPolyScroll->draw(addCancel);
        addPolyScroll->uniteDrawing(&window);
        listPolyScroll->update();
        listPolyScroll->openDrawing();
        listPolyScroll->draw(polyListSprite);
        int num = 0;
        for (auto lib : dataBase->dataBase)
        {
            auto bufpos = lib.head->control;
            lib.head->setPosition(0, BT_H * num);
            auto bufcol = lib.head->backGround->getColor();
            if (num == selecetdLibrary)
                lib.head->backGround->setColor(lib.head->pressColor);
            else lib.head->backGround->setColor(lib.head->boxColor);
            listPolyScroll->draw(lib.head);
            lib.head->backGround->setColor(bufcol);
            lib.head->setPosition(bufpos.left, bufpos.top);
            ++num;
        }
        listPolyScroll->uniteDrawing(&window);
    }
}

void MainMap::drawBigButton(ScrollWindow * plot, PxButton * btn, sf::Sprite * cornerL, sf::Sprite * cornerR, sf::Sprite * body)
{
    btn->prepareBack(cornerL, cornerR, body);
    plot->draw(cornerL);
    plot->draw(cornerR);
    plot->draw(body);
    plot->draw(btn);
}

void MainMap::checkPressed(int X, int Y)
{
    if (windowMode == 1)
    {
        miInput->offFocused();
        opA->offFocused();
        opB->offFocused();
        if (miInput->checkPressed(miScroll->control, miScroll->mindPos, X, Y, 0)) lastInput = 1;
        miAddPoly->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miCopyPoly->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miGraph->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miAdd->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miSubt->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miMult->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miValue->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miDeriv->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        miFormatBtn->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        if (opMode)
        {
            if (opA->checkPressed(opScroll->control, opScroll->mindPos, X, Y, 0)) lastInput = 2;
            if (opB->checkPressed(opScroll->control, opScroll->mindPos, X, Y, 0)) lastInput = 3;
            opSwap->checkPressed(opScroll->control, opScroll->mindPos, X, Y);
            opClose->checkPressed(opScroll->control, opScroll->mindPos, X, Y);
            opStart->checkPressed(opScroll->control, opScroll->mindPos, X, Y);
            miToA->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
            miToB->checkPressed(miScroll->control, miScroll->mindPos, X, Y);
        }
    }
    else if (windowMode == 2)
    {
        addOK->checkPressed(addPolyScroll->control, addPolyScroll->mindPos, X, Y);
        addCancel->checkPressed(addPolyScroll->control, addPolyScroll->mindPos, X, Y);
        if (listPolyScroll->checkPressed(X, Y))
        {
            int y = Y + listPolyScroll->mindPos.y - listPolyScroll->control.top;
            int sel = y / BT_H;
            if (sel < dataBase->dataBase.size())
            {
                if (selecetdLibrary == -1)
                    addOK->setIsEnabled(true);
                selecetdLibrary = sel;
            }
        }
    }
}

void MainMap::checkMoved(int X, int Y, float dx, float dy)
{
    if (windowMode == 1)
    {
        miAddPoly->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miCopyPoly->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miGraph->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miAdd->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miSubt->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miMult->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miValue->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miDeriv->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        miFormatBtn->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        if (opMode)
        {
            opSwap->checkOver(opScroll->control, opScroll->mindPos, X, Y);
            opClose->checkOver(opScroll->control, opScroll->mindPos, X, Y);
            opStart->checkOver(opScroll->control, opScroll->mindPos, X, Y);
            miToA->checkOver(miScroll->control, miScroll->mindPos, X, Y);
            miToB->checkOver(miScroll->control, miScroll->mindPos, X, Y);
        }
    }
    else if (windowMode == 2)
    {
        addOK->checkOver(addPolyScroll->control, addPolyScroll->mindPos, X, Y);
        addCancel->checkOver(addPolyScroll->control, addPolyScroll->mindPos, X, Y);
        listPolyScroll->checkMove(dx, dy);
    }
}

void MainMap::checkReleased(int X, int Y)
{
    listPolyScroll->isPressed = false;
    if (windowMode == 1)
    {
        if (miAddPoly->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
        {
            windowMode = 2;
            dataBase->windowMode = 4;
            listPolyScroll->mindSize.y = std::max(dataBase->dataBase.size() * BT_H, AP_H - 80ULL);
        }
        else if (miCopyPoly->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
        {
            sf::Clipboard::setString(miInput->text);
        }
        else if (miAdd->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            setOperation(1);
        else if (miSubt->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            setOperation(2);
        else if (miMult->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            setOperation(3);
        else if (miValue->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            setOperation(4);
        else if (miDeriv->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            setOperation(5);
        else if (miGraph->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            setOperation(6);
        else if (miFormatBtn->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
        {
            miFormatBtn->setSprite(isFormatted ? formatOff : formatOn);
            isFormatted ^= 1;
            updateMainColorMap();
            updateAColorMap();
            updateBColorMap();
        }
        else if (!dataBase->outText.empty())
        {
            if (lastInput == 1)
            {
                miInput->text = dataBase->outText;
                miInput->updateText();
                updateMainColorMap();
            }
            else if (opMode)
            {
                if (lastInput == 2)
                {
                    opA->text = dataBase->outText;
                    opA->updateText();
                    updateAColorMap();
                }
                else if (lastInput == 3)
                {
                    opB->text = dataBase->outText;
                    opB->updateText();
                    updateBColorMap();
                }
            }
            dataBase->outText = "";
        }
        else if(opMode)
        {
            if (opSwap->checkReleased(opScroll->control, opScroll->mindPos, X, Y))
            {
                std::swap(opA->text, opB->text);
                std::swap(opA->colorMap, opB->colorMap);
                opA->updateText();
                opB->updateText();
                updateAColorMap();
                updateBColorMap();
            }
            else if (opClose->checkReleased(opScroll->control, opScroll->mindPos, X, Y))
            {
                setOperation(0);
            }
            else if (opStart->checkReleased(opScroll->control, opScroll->mindPos, X, Y))
            {
                if (opMode == 1)
                    miInput->text = dataBase->sum(currentA, currentB).astext;
                else if (opMode == 2)
                    miInput->text = dataBase->subtract(currentA, currentB).astext;
                else if (opMode == 3)
                    miInput->text = dataBase->multiply(currentA, currentB).astext;
                else if (opMode == 4)
                {
                    double res = dataBase->getvalue(currentA, atof(opB->text.toAnsiString().c_str()));
                    miInput->text = res == INFINITY ? "INF" : std::to_string(res);
                    //miInput->text = isFloat(opB->text) ? "Yes" : "No";
                }
                else if (opMode == 5)
                    miInput->text = dataBase->derivative(currentA).astext;
                else if (opMode == 6)
                {
                    outCode = 1;
                    outA = &currentA;
                    outB = &currentB;
                }
                updateMainColorMap();
                miInput->updateText();
                miInput->redrawTexture();
            }
            if (miToA->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            {
                opA->text = miInput->text;
                updateAColorMap();
                opA->updateText();
                opA->redrawTexture();
                miInput->text = "";
                miInput->colorMap = "";
                miInput->updateText();
                miInput->redrawTexture();
            }
            else if (miToB->checkReleased(miScroll->control, miScroll->mindPos, X, Y))
            {
                opB->text = miInput->text;
                updateBColorMap();
                opB->updateText();
                opB->redrawTexture();
                miInput->text = "";
                miInput->colorMap = "";
                miInput->updateText();
                miInput->redrawTexture();
            }
        }
    }
    else if (windowMode == 2)
    {
        if (addOK->checkReleased(addPolyScroll->control, addPolyScroll->mindPos, X, Y))
        {
            auto it = dataBase->dataBase.begin();
            std::advance(it, selecetdLibrary);
            currentPoly.inlist = dataBase->addInList(currentPoly);
            it->base.push_back(currentPoly);
            dataBase->sortList(it->base);
            dataBase->saveLibrary(it);
            windowMode = 1;
            dataBase->windowMode = 1;
        }
        if (addCancel->checkReleased(addPolyScroll->control, addPolyScroll->mindPos, X, Y))
        {
            selecetdLibrary = -1;
            windowMode = 1;
            dataBase->windowMode = 1;
        }
    }
}

void MainMap::checkKeyPressed(sf::Event::KeyEvent key)
{
    if (windowMode == 1)
    {
        if (miInput->checkKeyPressed(key, [](char c) {return true;}))
            updateMainColorMap();
        if (opMode)
        {
            if (opA->checkKeyPressed(key, [](char c) {return true;}))
                updateAColorMap();
            if (opB->checkKeyPressed(key, [](char c) {return true;}))
                updateBColorMap();
        }
    }
}

void MainMap::checkTextEntered(sf::Event::TextEvent text)
{
    if (windowMode == 1)
    {
        if (miInput->checkInput(text.unicode, [](char c) {return c >= 32 && c <= 128;}))
            updateMainColorMap();
        if (opMode)
        {
            if (opA->checkInput(text.unicode, [](char c) {return c >= 32 && c <= 128;}))
                updateAColorMap();
            auto f1 = [](char c) {return c >= 32 && c <= 128;};
            auto f2 = [](char c) {return c >= '0' && c <= '9' || c == ' ' || c == '-' || c == '+' || c == '.';};
            if ((opMode != 4) && opB->checkInput(text.unicode, f1))
                updateBColorMap();
            else if ((opMode >= 4) && opB->checkInput(text.unicode, f2))
                updateBColorMap();
        }
    }
}

void MainMap::resizeBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    delete miScroll;
    MI_W = std::max(WINDOW_W - MI_L - MI_R, 430);
    miScroll = new ScrollWindow(sf::FloatRect(MI_L, MI_CH + predHeight, MI_W + MI_A, MI_H),
        sf::FloatRect(0, 0, MI_W + MI_A, MI_H));
    miInputBorders = owt->createBackground(sf::FloatRect(10, 10, MI_W - 20, 48), "", 0);
    miInputBorders->setColor(sf::Color(150, 150, 150));
    miInputBack->setScale((MI_W - 26) / 16.f, 42 / 16.f);
    miShowBack->setScale((MI_W - 20) / 16.f, MI_SH / 16.f);
    miFormatBtn->setPosition(MI_W, 22);
    miToA->setPosition(MI_W + 28, 22);
    miToB->setPosition(MI_W + 56, 22);

    miInput->bounds.width = MI_W - 55;
    miInput->setMovingView(false);
    miInput->setMovingView(true);
    miInput->redrawTexture();

    addPolyScroll->control.left = (WINDOW_W - AP_W) / 2;
    addPolyScroll->control.top = (WINDOW_H - AP_H) / 2;
    listPolyScroll->control.left = (WINDOW_W - AP_W) / 2 + 30;
    listPolyScroll->control.top = (WINDOW_H - AP_H) / 2 + 40;

    delete opScroll;
    opScroll = new ScrollWindow(sf::FloatRect(MI_L, MI_CH, MI_W + MI_A, OP_H),
        sf::FloatRect(0, 0, MI_W + MI_A, OP_H));
    opBordersA = owt->createBackground(sf::FloatRect(10 + OP_L, 45, MI_W - 20 - OP_L, 48), "", 0);
    opBordersA->setColor(sf::Color(150, 150, 150));
    opBordersB = owt->createBackground(sf::FloatRect(10 + OP_L, 105, MI_W - 20 - OP_L, 48), "", 0);
    opBordersB->setColor(sf::Color(150, 150, 150));
    opBackA->setScale((MI_W - 26 - OP_L) / 16.f, 42 / 16.f);
    opBackB->setScale((MI_W - 26 - OP_L) / 16.f, 42 / 16.f);
    opSwap->setPosition(MI_W, 84);
    opClose->setPosition(MI_W, 14);

    opA->bounds.width = opB->bounds.width = MI_W - OP_L - 55;
    opA->setMovingView(false);
    opB->setMovingView(false);
    opA->setMovingView(true);
    opB->setMovingView(true);
    opA->redrawTexture();
    opB->redrawTexture();
    opStart->setPosition((MI_W - 300 + 60) / 2, 163);

    setBigButtonsPos();
    updateMainColorMap();
}


void MainMap::updateMainColorMap()
{
    currentPoly = dataBase->polyFromString(miInput->text, 0, miInput->colorMap);
    if (!isFormatted) miInput->colorMap = "";
    bool isb = !currentPoly.chars.empty() || miInput->text[0] == '0';
    if (opMode == 4) isb = 0;
    if (opMode == 4 && isFloat(miInput->text))
        miInput->colorMap = std::string(miInput->colorMap.size(), 'd');
    else if (opMode == 4) miInput->colorMap = std::string(miInput->colorMap.size(), '-');
    miAddPoly->setIsEnabled(isb);
    miCopyPoly->setIsEnabled(isb);
    if (miInput->text.isEmpty())
        miInputLb->text = "Write by keyboard or paste from library";
    else if (miInput->text.getSize() < 5 || miInput->colorMap[0] == '-')
            miInputLb->text = "Example: 3x2+14x-3 | Blue - coefficient | Cyan - power | Gray - symbols not included";
        else miInputLb->text = "";
    miInputLb->updateText();
    miInput->redrawTexture();
    if (polyShowSprite) delete polyShowSprite;
    polyShowSprite = dataBase->drawPolynomial(currentPoly, frontArgs, MI_W - 50, MI_SH, sf::Color::White);
}

void MainMap::updateAColorMap()
{
    currentA = dataBase->polyFromString(opA->text, 0, opA->colorMap);
    bool isa = !currentA.chars.empty() || opA->text[0] == '0', isb;
    if (opMode != 4)
        isb = !currentB.chars.empty() || opB->text[0] == '0';
    else isb = currentB.chars.size() == 1 && currentB.chars.begin()->second == 0;
    if (opMode == 5) isb = true;
    opStart->setIsEnabled(isa && isb);
    if (opMode == 6)
        opStart->setIsEnabled(isa || isb);
    if (!isFormatted) opA->colorMap = "";
    opA->redrawTexture();
}

void MainMap::updateBColorMap()
{
    currentB = dataBase->polyFromString(opB->text, 0, opB->colorMap);
    bool isa = !currentA.chars.empty() || opA->text[0] == '0', isb = false;
    if (opMode != 4)
        isb = !currentB.chars.empty() || opB->text[0] == '0';
    else if (isFloat(opB->text))
    {
        isb = true;
        opB->colorMap = std::string(opB->colorMap.size(), 'd');
    }
    else opB->colorMap = std::string(opB->colorMap.size(), '-');
    opStart->setIsEnabled(isa && isb);
    if (opMode == 6)
        opStart->setIsEnabled(isa || isb);
    if (!isFormatted) opB->colorMap = "";
    opB->redrawTexture();
}

void MainMap::setOperation(int mode)
{
    if (mode == opMode) return;
    if (mode == 0)
        predHeight = 0;
    else if (opMode == 0)
    {
        opA->text = miInput->text;
        updateAColorMap();
        opA->updateText();
        opA->redrawTexture();
        miInput->text = "";
        miInput->colorMap = "";
        miInput->updateText();
        miInput->redrawTexture();
        opB->text = "";
        opB->colorMap = "";
        opB->updateText();
        opB->redrawTexture();
        updateAColorMap();
        updateBColorMap();
        updateMainColorMap();
    }
    if (opMode == 1)
        miAdd->setIsEnabled(true);
    else if (opMode == 2) miSubt->setIsEnabled(true);
    else if (opMode == 3) miMult->setIsEnabled(true);
    else if (opMode == 4) miValue->setIsEnabled(true);
    else if (opMode == 5) miDeriv->setIsEnabled(true);
    else if (opMode == 6)
    {
        miGraph->setIsEnabled(true);
        opStart->mainText.setString("GET ANSWER");
    }
    if (mode == 1)
        miAdd->setIsEnabled(false);
    else if (mode == 2) miSubt->setIsEnabled(false);
    else if (mode == 3) miMult->setIsEnabled(false);
    else if (mode == 4) miValue->setIsEnabled(false);
    else if (mode == 5) miDeriv->setIsEnabled(false);
    else if (mode == 6) miGraph->setIsEnabled(false);
    if (opMode == 5 || opMode == 6)
    {
        opB->isActive = true;
        opSwap->setIsEnabled(true);
        miToB->setIsEnabled(true);
    }
    if (mode < 4 || mode == 6)
    {
        opLabelA = opTextureA;
        opLabelB = opTextureB;
    }
    else
    {
        opLabelA = opTextureF;
        opLabelB = opTextureX;
    }
    if (mode == 1)
    {
        predHeight = 220;
        opNow = opAdd;
        opHeader->text = "Addition operation";
    }
    else if (mode == 2)
    {
        predHeight = 220;
        opNow = opSubt;
        opHeader->text = "Subtraction operation";
    }
    else if (mode == 3)
    {
        predHeight = 220;
        opNow = opMult;
        opHeader->text = "Multiply operation";
    }
    else if (mode == 4)
    {
        opB->text = "";
        opB->updateText();
        updateBColorMap();
        predHeight = 220;
        opNow = opValue;
        opHeader->text = "Value at a point";
    }
    else if (mode == 5)
    {
        predHeight = 160;
        opNow = opDeriv;
        opHeader->text = "Derivative";
        opB->isActive = false;
        opSwap->setIsEnabled(false);
        miToB->setIsEnabled(false);
    }
    else if (mode == 6)
    {
        predHeight = 220;
        opNow = opGraph;
        opHeader->text = "Comparation";
        opStart->mainText.setString("PLOT RESULT");
        opB->isActive = true;
        opSwap->setIsEnabled(true);
        miToB->setIsEnabled(true);
    }
    updateAColorMap();
    updateBColorMap();
    updateMainColorMap();
    opHeader->updateText();
    miScroll->control.top = MI_CH + predHeight;
    opMode = mode;
}

bool MainMap::isFloat(std::string str)
{
    int l = 0, r = str.size();
    while (l < str.size() && str[l] == ' ') ++l;
    while (r >= 1 && str[r-1] == ' ') --r;
    if (r <= l) return false;
    str = str.substr(l, r - l);
    if (str.find_first_not_of("0123456789.+-") != -1)
        return false;
    int d = str.find_last_not_of("0123456789.");
    if (d > 0) return false;
    int p = str.find_first_of('.');
    if (str.size() - (d > -1) - (p > -1) > 9) return false;
    if (p == -1) return true;
    if (p <= d + 1 || p == str.size() - 1) return false;
    str[p] = '0';
    if (str.find_first_of('.') != -1) return false;
    return true;
}

MainMap::~MainMap()
{
    delete miScroll; delete miInputBack; delete miInputBorders;
    delete miShowBack; delete polyShowSprite; delete miInputLb;
    delete bigCornerL; delete bigCornerR; delete bigBody; delete miAddPoly;
    delete polyAddSprite; delete addPolyScroll; delete miAddText;
    delete polyListSprite; delete addOK; delete addCancel; 
    delete miCopyPoly; delete opScroll; delete opHeader; delete opBackA;
    delete opBordersA; delete opBackB; delete opBordersB; 
    delete opTextureA; delete opTextureB; delete opSwap; delete opA; 
    delete opB; delete miFormatBtn; delete formatOff; delete formatOn; 
    delete opClose; delete opStart; delete opAdd; delete opSubt; 
    delete opMult; delete miToA; delete miToB; delete miAdd; delete miSubt; 
    delete miMult; delete miValue; delete miDeriv; delete opTextureF;
    delete opTextureX; delete opValue; delete miGraph; delete opGraph;
}