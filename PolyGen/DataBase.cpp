#include "DataBase.h"

DataBase::DataBase()
{
}


void DataBase::loadFromFile(std::wstring fileName, std::string name, std::wstring path, uint16_t pack_id)
{
    std::ifstream fin(fileName);
    std::list<Polynomial> base;
    std::string r_name = name;
    if (!fin.is_open())
    {
        if (name.size() > 7)
            name = name.substr(0, 7) + "...";
        name += " : Can't find or open";
        char * header = (char *)name.c_str();
        auto rbs = addDataHead(header, buttonHeads.size(),
            sf::Color(255, 200, 200), sf::Color(200, 204, 227));
        dataBase.push_back(Library(rbs.first, rbs.second, base, r_name, path));
        return;
    }
    std::string input;
    std::getline(fin, name), r_name = name;
    uint16_t id = 0;
    while (std::getline(fin, input))
    {
        Polynomial p;
        if (input.size() < 2 || (input.size() - 2) % 12 != 0)
        {
            if (name.size() > 7)
                name = name.substr(0, 7) + "...";
            name += " : Invalid file syntax";
            char * header = (char *)name.c_str();
            base.clear();
            auto rbs = addDataHead(header, buttonHeads.size(),
                sf::Color(255, 200, 200), sf::Color(200, 204, 227));
            dataBase.push_back(Library(rbs.first, rbs.second, base, r_name, path));
            return;
        }
        p.serial = convert_from(&(input[0]), 2);
        int n = (input.size() - 2) / 12;
        for (int i = 0; i < n; ++i)
        {
            int coeff = convert_from(&(input[12 * i + 2]), 6);
            int power = convert_from(&(input[12 * i + 8]), 6);
            p.chars.push_back({ coeff, power });
        }
        p.pack_id = pack_id;
        p.item_id = id++;
        p.inlist = addInList(p);
        base.push_back(p);
    }
    sortList(base);
    auto rbs = addDataHead((char *)name.c_str(), buttonHeads.size());
    dataBase.push_back(Library(rbs.first, rbs.second, base, r_name, path, 1));
    fin.close();
}

void DataBase::saveToFile(Polynomial p, char * fileName)
{
    std::ofstream fout(fileName);
    fout.close();
}



DataBase::Polynomial DataBase::polyFromString(std::string raw, size_t serial, std::string &clm)
{
    std::string mask = std::string(raw.size(), '-');
    Polynomial pol;
    pol.serial = serial;
    int l = 0;
    std::map<int, int> pm;
    while (l < raw.size())
    {
        int r1 = raw.find('+', l + 1);
        int r2 = raw.find('-', l + 1), r = raw.size();
        if (r1 != -1) r = std::min(r, r1);
        if (r2 != -1) r = std::min(r, r2);
        int kb = -1, xb = -1, ml = l, k = 0, p = 0, kl = 0;
        bool nonx = (raw.substr(l + 1, r - l - 1).find_first_of('x') == -1);
        for (l = l; l < r; ++l)
        {
            if (xb == -1 && raw[l] >= '0' && raw[l] <= '9')
            {
                if (kb == -1) kb = l;
                k = 10 * k + raw[l] - '0';
                ++kl;
            }
            else if (xb == -1 && raw[l] == 'x')
            {
                xb = l;
                if (kb != -1 && l - kb < 10) 
                    std::fill(mask.begin() + kb, mask.begin() + l, 'k');
            }
            else if (xb == -1 && kb != -1 && (raw[l] < '0' || raw[l] > '9'))
            {
                if (nonx && kl < 10)
                    break;
                kb = -1, k = 0, kl = 0;
            }
            else if (xb != -1 && raw[l] >= '0' && raw[l] <= '9')
            {
                p = 10 * p + raw[l] - '0';
            }
            else if (xb != -1) break;
        }
        if (nonx && kl < 10 && kb != -1) std::fill(mask.begin() + kb, mask.begin() + l, 'k');
        if (xb != -1)
        {
            mask[xb] = 'x';
            if (l != xb + 1 && l - xb - 1 < 10) 
                std::fill(mask.begin() + xb + 1, mask.begin() + l, 'p');
            else if (l == xb + 1) p = 1;
            if (kb == -1 || kl >= 10) k = 1;
        }
        if (kl < 10 || xb != -1)
        {
            if (raw[ml] == '+') mask[ml] = 'c';
            else if (raw[ml] == '-') mask[ml] = 'k', k = -k;
            if (xb != -1 && l - xb - 1 >= 10) p = 1;
            pm[-p] += k;
        }
        l = r;
    }
    for (auto it : pm)
        if (it.second) pol.chars.push_back({it.second, -it.first});
    clm = mask;
    return pol;
}

void DataBase::focusAtHead(std::list<Library>::iterator it)
{
    if (selectedType == 2 && selectedPolyC != dataBase.end())
    {
        selectedPolyC = dataBase.end();
        (*selectedPoly).inlist->isPressed = false;
        (*selectedPoly).inlist->update();
        dbToMain->setIsEnabled(false);
        dbSettings->setIsEnabled(true);
    }
    selectedType = 1;
    if (selectedHead != dataBase.end()) {
        (*selectedHead).head->isPressed = false;
        (*selectedHead).head->update();
    }
	if (it != dataBase.end())
	{
		(*it).head->isPressed = true;
		(*it).head->update();
		selectedHead = it;
		dbLb->mainText.setString((*it).name);
	}
}

void DataBase::focusAtPoly(std::list<Library>::iterator cont, std::list<Polynomial>::iterator it)
{
    if (selectedType == 1)
    {
        (*selectedHead).head->isPressed = false;
        (*selectedHead).head->update();
        dbToMain->setIsEnabled(true);
        dbSettings->setIsEnabled(false);
    }
    selectedType = 2;
    if (selectedPolyC != dataBase.end()) {
        selectedPoly->inlist->isPressed = false;
        selectedPoly->inlist->update();
    }
    selectedPoly = it;
    selectedPolyC = cont;
    selectedPoly->inlist->isPressed = true;
    selectedPoly->inlist->update();
    dbLb->mainText.setString(it->astext);
}

int DataBase::convert_from(char * inp, size_t n)
{
    int result = 0;
    for (int i = 0; i < n; ++i)
    {
        result <<= 6;
        result += (*inp - 32), ++inp;
    }
    return result;
}

PxButton * DataBase::addInList(Polynomial & p)
{
    // polynomial button
    PxButton * btn = new PxButton(sf::FloatRect(0, 0, DB_W, BT_H), sf::Color::White, 12, "", bf);
    btn->setSprite(drawPolynomial(p, frontArgs, DB_W, BT_H, sf::Color(0, 0, 0, 255)));
    btn->setColor(sf::Color(255, 255, 255, 128), 1, 1, 1);
    btn->pressColor = sf::Color(255, 255, 255, 255);
    for (auto m : p.chars)
    {
        if (p.astext.size() > 20)
        {
            p.astext += "...";
            break;
        }
        if (p.astext.size()) p.astext += m.first > 0 ? " + " : " - ";
        else if (m.first < 0) p.astext += '-';
        if(abs(m.first) != 1) p.astext += std::to_string(abs(m.first));
        if (m.second) p.astext += 'x' + std::to_string(m.second);
    }
    listButtons.push_back(btn);
    return btn;
}

std::pair<PxButton *, PxButton* > DataBase::addDataHead(char * name, uint16_t num, sf::Color bck, sf::Color pclr)
{
    // headline
    PxButton * btn = new PxButton(sf::FloatRect(0, 0, DB_W, BT_H), bck, DB_W/4, name, bf);
    btn->setCharacterSize(12);
    btn->mainText.setFillColor(sf::Color(0, 0, 0, 255));
    btn->setSprite(owt->createBackground(btn->control, "textures/head.png", 0));
    btn->backGround->setScale(DB_W / 16.f, 1.5);
    btn->setColor(bck, 1);
    btn->pressColor = pclr;
    btn->meta = num;
    buttonHeads.push_back(btn);
    // close button
    PxButton * cls = new PxButton(sf::FloatRect(0, 0, 16, BT_H), sf::Color(200, 200, 200), 12, "", bf);
    cls->setSprite(owt->createBackground(btn->control, "textures/close_up.png", 0));
    cls->backGround->setOrigin(0, -4);
    cls->setColor(sf::Color(150, 150, 150), 1, 0, 0.5);
    cls->meta = 1;
    closeHeads.push_back(cls);
    return{ btn, cls };
}

sf::Sprite * DataBase::drawPolynomial(DataBase::Polynomial p, float args[8], uint16_t width, uint16_t height, sf::Color txtClr)
{
    if (rt) delete rt;
    rt = new sf::RenderTexture();
    rt->create(width, height);
    rt->clear(sf::Color(255, 255, 255, 0));
    int x = 15;
    if (p.chars.empty()) p.chars.push_back({ 0, 0 });
    auto pp = p.chars.begin();
    while (pp != p.chars.end())
    {
        int k = pp->first, pw = pp->second;
        std::string st = "";
        if (pp != p.chars.begin() && k < 0)
            k = -k, st = " - ";
        else if (pp != p.chars.begin() && k > 0) st = " + ";
        if (k != 1 && k != -1 || pw == 0) st += std::to_string(k);
        else if (k == -1) st += "-";
        sf::Text t(st + (pw ? "x" : ""), xF, args[0]);
        t.setFillColor(txtClr);
        t.setPosition(x, args[4]);
        x += t.getLocalBounds().width + args[2];
        sf::Text upt(pw > 1 ? std::to_string(pw) : "", xF, args[1]);
        upt.setFillColor(txtClr);
        upt.setPosition(x, args[5]);

        x += std::max(0.f, upt.getLocalBounds().width + args[3]);
        if (x <= width - 10)
        {
            rt->draw(t);
            rt->draw(upt);
        }
        else
        {
            t.setString("...");
            rt->draw(t);
            break;
        }
        ++pp;
    }
    sf::Texture * txtr = new sf::Texture(rt->getTexture());
    sf::Sprite * poly = new sf::Sprite(*txtr);
    poly->setOrigin(0, height);
    poly->setScale(1, -1);
    poly->setPosition(args[6], args[7]);
    return poly;
}

void DataBase::initBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H, sf::Font &F, ownUtilities *ow)
{
    DB_H = WINDOW_H;
    owt = ow;
    bf = F;
    dbScroll = new ScrollWindow(sf::FloatRect(WINDOW_W - DB_W, 0, DB_W, WINDOW_H),
        sf::FloatRect(0, 0, DB_W, WINDOW_H));
    dbBackGround = owt->createBackground(sf::FloatRect(0, 0, DB_W, WINDOW_H), "textures/white.png", 0);
    dbBackGround->setScale(DB_W / 16.f, WINDOW_H / 16.f);
    dbTopBorder = owt->createBackground(sf::FloatRect(0, PL_T-2, DB_W, WINDOW_H), "textures/white.png", 0);
    dbTopBorder->setColor(sf::Color(240, 240, 240));
    dbTopBorder->setScale(DB_W / 16.f, 2 / 16.f);
    dbLeftBound = owt->createBackground(sf::FloatRect(WINDOW_W - 4, 0, 4, WINDOW_H), "textures/left_bound.png", 0);
    dbLeftBound->setScale(1, WINDOW_H / 4.f);
    dbClose = new PxButton(sf::FloatRect(-2, 7, 16, 16), sf::Color(150, 150, 150), 12, "", F);
    dbClose->setSprite(owt->createBackground(dbClose->control, "textures/close_right.png", 0));
    polyScroll = new ScrollWindow(sf::FloatRect(WINDOW_W - DB_W, PL_T, DB_W, WINDOW_H - PL_T - PL_B),
        sf::FloatRect(0, 0, DB_W, WINDOW_H - PL_T - PL_B));
    initQuestion(WINDOW_W, WINDOW_H);
    initSettings(WINDOW_W, WINDOW_H);
    initBottomButtons(WINDOW_W, WINDOW_H);
    initTopButtons(WINDOW_W, WINDOW_H);
    updateSortType(0);
}

void DataBase::initQuestion(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    int x = WINDOW_W - QD_W, y = WINDOW_H - QD_H;
    qdScroll = new ScrollWindow(sf::FloatRect(0, 0, WINDOW_W, WINDOW_H),
        sf::FloatRect((WINDOW_W-QD_W) / 2, (WINDOW_H-QD_H) / 2, WINDOW_W * 2 - QD_W, WINDOW_H * 2 - QD_H));
    qdBack = owt->createBackground(sf::FloatRect(x, y, QD_W, QD_H), "", 0);
    qdWarning = owt->createBackground(sf::FloatRect(x+QD_W-100, y+5, 80, 80), "textures/triangle.png", 0);
    qdWarning->setScale(0.4, 0.4);
    qdQuestion = new ClrBox("A you sure to delete this file?", sf::FloatRect(x + 25, y + 35, 225, 50), 20, bf);
    qdOK = new PxButton(sf::FloatRect(x + QD_W - 80, y + QD_H - 32, 72, 24), sf::Color::Green, 24, "OK", bf);
    qdOK->setSprite(owt->createBackground(qdOK->control, "textures/dial_btn.png", 0));
    qdOK->mainText.setFillColor(sf::Color::White);

    qdCancel = new PxButton(sf::FloatRect(x + QD_W - 154, y + QD_H - 32, 72, 24), sf::Color::Red, 52, "Cancel", bf);
    qdCancel->setSprite(owt->createBackground(qdCancel->control, "textures/dial_btn.png", 0));
    qdCancel->mainText.setFillColor(sf::Color::White);
    qdCancel->offColor = sf::Color::Transparent;
}

void DataBase::initSettings(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    int x = WINDOW_W - SW_W, y = WINDOW_H - SW_H;
    swScroll = new ScrollWindow(sf::FloatRect(0, 0, WINDOW_W, WINDOW_H),
        sf::FloatRect((WINDOW_W - SW_W) / 2, (WINDOW_H - SW_H) / 2, WINDOW_W * 2 - SW_W, WINDOW_H * 2 - SW_H));
    swBack = owt->createBackground(sf::FloatRect(x, y, SW_W, SW_H), "", 0);
    swHeader = new ClrBox("Library settings", sf::FloatRect(x + 160, y + 5, 225, 20), 12, bf);
    swHeader->resultBox.setFillColor(sf::Color(150, 150, 150));
    swNameLb = new ClrBox("Library name:", sf::FloatRect(x + 18, y + 30, 225, 20), 12, bf);
    swPathLb = new ClrBox("Absolute path(txt format):", sf::FloatRect(x + 18, y + 100, 225, 20), 12, bf);
    
    swOK = new PxButton(sf::FloatRect(x + SW_W - 80, y + SW_H - 32, 72, 24), sf::Color::Green, 24, "OK", bf);
    swOK->setSprite(owt->createBackground(swOK->control, "textures/dial_btn.png", 0));
    swOK->mainText.setFillColor(sf::Color::White);
    swOK->offColor = sf::Color(20, 120, 20);

    swCancel = new PxButton(sf::FloatRect(x + SW_W - 154, y + SW_H - 32, 72, 24), sf::Color::Red, 52, "Cancel", bf);
    swCancel->setSprite(owt->createBackground(swCancel->control, "textures/dial_btn.png", 0));
    swCancel->mainText.setFillColor(sf::Color::White);

    swGetPath = new PxButton(sf::FloatRect(x + 15 + SW_TXTW + 6, y + SW_TPY, 32, 32), sf::Color::White, 12, "", bf);
    swGetPath->setColor(sf::Color::White, 1, 0.8, 0.8);
    swGetPath->overColor = sf::Color(224, 241, 255);
    swGetPath->setSprite(owt->createBackground(swGetPath->control, "textures/get_path.png", 0));
    swGetPath->backGround->setScale(32 / 24.f, 32 / 24.f);

    swCorners = owt->createBackground(sf::FloatRect(x + 5, y + 50, 24, 24), "textures/min_back.png", 0);
    swTextBody = owt->createBackground(sf::FloatRect(x + 5, y + 50, 24, 24), "textures/white.png", 0);
    swCorners->setColor(sf::Color(240, 240, 240));
    swTextBody->setColor(sf::Color(240, 240, 240));
    swTextBody->setScale(SW_TXTW / 16.f - 2, 2);
    swCorners->setScale(32 / 24.f, 32 / 24.f);

    swName = new ClrBox("Base", sf::FloatRect(x + 20, y + SW_TNY+6, SW_TXTW - 15, 32), 15, bf);
    swName->isActive = true;
    swName->setMovingView(true);
    swPath = new ClrBox("C:\Prigram Files(x86)\base.txt", sf::FloatRect(x + 20, y + SW_TPY + 6, SW_TXTW - 15, 32), 15, bf);
    swPath->isActive = true;
    swPath->setMovingView(true);

}

void DataBase::initBottomButtons(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    dbLb = new PxButton(sf::FloatRect(0, WINDOW_H - PL_B, DB_W, 16), sf::Color::White, DB_W / 4, "My base : Library", bf);
    dbLb->setCharacterSize(12);
    dbLb->setColor(sf::Color(240, 240, 240), 1);
    dbLb->mainText.setFillColor(sf::Color(0, 148, 255));
    dbLb->setSprite(owt->createBackground(dbLb->control, "textures/white.png", 0));
    dbLb->backGround->setScale(DB_W / 16.f, 1);

    dbDelete = new PxButton(sf::FloatRect(DB_W - 32, WINDOW_H - PL_B + 20, 24, 24), sf::Color::White, 12, "", bf);
    dbDelete->setColor(sf::Color::White, 1, 0.8, 0.8);
    dbDelete->overColor = sf::Color(224, 241, 255);
    dbDelete->setSprite(owt->createBackground(dbDelete->control, "textures/delete.png", 0));
    dbSettings = new PxButton(sf::FloatRect(DB_W - 60, WINDOW_H - PL_B + 20, 24, 24), sf::Color::White, 12, "", bf);
    dbSettings->setColor(sf::Color::White, 1, 0.8, 0.8);
    dbSettings->overColor = sf::Color(224, 241, 255);
    dbSettings->setSprite(owt->createBackground(dbSettings->control, "textures/settings.png", 0));
    dbUp = new PxButton(sf::FloatRect(DB_W - 88, WINDOW_H - PL_B + 20, 24, 24), sf::Color::White, 12, "", bf);
    dbUp->setColor(sf::Color::White, 1, 0.8, 0.8);
    dbUp->overColor = sf::Color(224, 241, 255);
    dbUp->setSprite(owt->createBackground(dbUp->control, "textures/up.png", 0));
    dbDown = new PxButton(sf::FloatRect(DB_W - 116, WINDOW_H - PL_B + 20, 24, 24), sf::Color::White, 12, "", bf);
    dbDown->setColor(sf::Color::White, 1, 0.8, 0.8);
    dbDown->overColor = sf::Color(224, 241, 255);
    dbDown->setSprite(owt->createBackground(dbDown->control, "textures/down.png", 0));
    dbToMain = new PxButton(sf::FloatRect(10, WINDOW_H - PL_B + 20, 24, 24), sf::Color::White, 12, "", bf);
    dbToMain->setColor(sf::Color::White, 1, 0.8, 0.8);
    dbToMain->overColor = sf::Color(224, 241, 255);
    dbToMain->setSprite(owt->createBackground(dbToMain->control, "textures/to_main.png", 0));
    dbToMain->setIsEnabled(false);
    dbDelete->offColor = dbSettings->offColor = dbUp->offColor = dbDown->offColor = sf::Color(255, 255, 255, 128);
    dbToMain->offColor = sf::Color(255, 255, 255, 128);
}

void DataBase::initTopButtons(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    dbAddFile = new PxButton(sf::FloatRect(30, 2, 24, 24), sf::Color::White, 12, "", xF);
    dbAddFile->setColor(sf::Color(237, 237, 237), 1, 0.8, 0.8);
    dbAddFile->overColor = sf::Color(224, 241, 255);
    dbAddFile->setSprite(owt->createBackground(dbAddFile->control, "textures/add_library.png", 0));
    dbAddNew = new PxButton(sf::FloatRect(58, 2, 24, 24), sf::Color::White, 12, "", xF);
    dbAddNew->setColor(sf::Color(237, 237, 237), 1, 0.8, 0.8);
    dbAddNew->overColor = sf::Color(224, 241, 255);
    dbAddNew->setSprite(owt->createBackground(dbAddNew->control, "textures/new_library.png", 0));

    dbByK = new PxButton(sf::FloatRect(DB_W - 88, 2, 24, 24), sf::Color::White, 12, "k", xF);
    dbByK->setColor(sf::Color(237, 237, 237), 1, 0.8, 0.8);
    dbByK->overColor = sf::Color(224, 241, 255);
    dbByK->setSprite(owt->createBackground(dbByK->control, "textures/min_back.png", 0));
    dbByP = new PxButton(sf::FloatRect(DB_W - 60, 2, 24, 24), sf::Color::White, 12, "p", xF);
    dbByP->setColor(sf::Color(237, 237, 237), 1, 0.8, 0.8);
    dbByP->overColor = sf::Color(224, 241, 255);
    dbByP->setSprite(owt->createBackground(dbByP->control, "textures/min_back.png", 0));
    dbByC = new PxButton(sf::FloatRect(DB_W - 32, 2, 24, 24), sf::Color::White, 12, "#", xF);
    dbByC->setColor(sf::Color(237, 237, 237), 1, 0.8, 0.8);
    dbByC->overColor = sf::Color(224, 241, 255);
    dbByC->setSprite(owt->createBackground(dbByC->control, "textures/min_back.png", 0));
    dbByK->pressColor = dbByP->pressColor = dbByC->pressColor = sf::Color(198, 229, 255);
    dbByK->overColor = dbByP->overColor = dbByC->overColor = sf::Color(224, 241, 255);

    dbOpen = new PxButton(sf::FloatRect(DB_W - 25, 2, 25, 30), sf::Color::White, 12, "", xF);
    dbOpen->setColor(sf::Color::White, 1, 0.9, 0.8);
    dbOpen->setSprite(owt->createBackground(dbOpen->control, "textures/open_base.png", 0));
}

void DataBase::deleteBoxes()
{
    delete dbScroll;
    delete polyScroll;
}

void DataBase::drawBoxes(sf::RenderWindow * window)
{
    window->draw(*dbLeftBound);
    if (windowMode >= 0)
    {
        dbScroll->update();
        dbScroll->openDrawing();
        dbScroll->draw(dbBackGround);
        dbScroll->draw(dbClose);
        dbScroll->draw(dbLb, -1, 5);
        dbScroll->draw(dbTopBorder);
        dbScroll->draw(dbByK);
        dbScroll->draw(dbByP);
        dbScroll->draw(dbByC);
        dbScroll->draw(dbAddFile);
        dbScroll->draw(dbAddNew);

        dbScroll->draw(dbDelete);
        dbScroll->draw(dbSettings);
        dbScroll->draw(dbUp);
        dbScroll->draw(dbDown);
        dbScroll->draw(dbToMain);
        dbScroll->uniteDrawing(window);

        polyScroll->update();
        polyScroll->openDrawing();
        int y = 0;
        for (auto it = dataBase.begin(); it != dataBase.end(); ++it)
        {
            bool canshow = 1;
            if (it->head)
            {
                it->head->setPosition(0, y);
                polyScroll->draw(it->head, -1, 8, 0, 1);
                (it->cls)->setPosition(DB_W - 24, y);
                polyScroll->draw(it->cls);
                canshow = (it->cls)->meta;
                y += BT_H;
            }
            if (!canshow) continue;
            for (auto p : it->base)
            {
                if (p.inlist)
                {
                    p.inlist->setPosition(0, y);
                    polyScroll->draw(p.inlist);
                    y += BT_H;
                }
            }
        }
        polyScroll->mindSize.y = std::max(y + BT_H, DB_H - PL_T - PL_B);
        polyScroll->uniteDrawing(window);
    }
    else
    {
        dbScroll->update();
        dbScroll->openDrawing();
        dbScroll->draw(dbOpen);
        dbScroll->uniteDrawing(window);
    }

    if (windowMode == 2) {
        qdScroll->update();
        qdScroll->openDrawing();
        qdScroll->draw(qdBack);
        qdScroll->draw(qdWarning);
        qdScroll->draw(qdOK);
        qdScroll->draw(qdCancel);
        qdScroll->draw(qdQuestion, 0);
        qdScroll->uniteDrawing(window);
    }
    else if (windowMode == 3) {
        swScroll->update();
        swScroll->openDrawing();
        swScroll->draw(swBack);
        swScroll->draw(swHeader, 1);
        swScroll->draw(swNameLb, 1);
        swScroll->draw(swPathLb, 1);
        swScroll->draw(swOK);
        swScroll->draw(swCancel);
        int swx = swBack->getPosition().x;
        int swy = swBack->getPosition().y;
        swScroll->draw(swCorners, swx + 15, swy + SW_TNY);
        swScroll->draw(swCorners, swx + 15 + SW_TXTW - 32, swy + SW_TNY);
        swScroll->draw(swTextBody, swx + 15 + 16, swy + SW_TNY);
        swScroll->draw(swCorners, swx + 15, swy + SW_TPY);
        swScroll->draw(swCorners, swx + 15 + SW_TXTW - 32, swy + SW_TPY);
        swScroll->draw(swTextBody, swx + 15 + 16, swy + SW_TPY);

        swName->update();
        swScroll->draw(swName, 0);
        swPath->update();
        swScroll->draw(swPath, 0);
        swScroll->draw(swGetPath);
        swScroll->uniteDrawing(window);
    }
}

void DataBase::resizeBoxes(uint16_t WINDOW_W, uint16_t WINDOW_H)
{
    DB_H = WINDOW_H;
    delete dbScroll;
    dbScroll = new ScrollWindow(sf::FloatRect(WINDOW_W - DB_W, 0, DB_W, WINDOW_H),
        sf::FloatRect(0, 0, DB_W, WINDOW_H));
    delete polyScroll;
    polyScroll = new ScrollWindow(sf::FloatRect(WINDOW_W - DB_W, PL_T, DB_W, WINDOW_H - PL_T - PL_B),
        sf::FloatRect(0, 0, DB_W, WINDOW_H - PL_T - PL_B));
    dbLeftBound->setPosition(WINDOW_W - (windowMode >= 0) * DB_W - 4, 0);
    dbBackGround->setScale(DB_W / 16.f, WINDOW_H / 16.f);
    dbLeftBound->setScale(1, WINDOW_H / 4.f);
    dbLb->setPosition(0, WINDOW_H - PL_B);
    auto tmp = qdScroll->mindPos;
    delete qdScroll;
    qdScroll = new ScrollWindow(sf::FloatRect(0, 0, WINDOW_W, WINDOW_H),
        sf::FloatRect(WINDOW_W / 2, WINDOW_H / 2, WINDOW_W * 2 - QD_W, WINDOW_H * 2 - QD_H));
    qdScroll->mindPos = tmp;
    int x = WINDOW_W - QD_W, y = WINDOW_H - QD_H;
    qdBack->setPosition(x, y);
    qdWarning->setPosition(x + QD_W - 100, y + 5);
    qdQuestion->setPosition(x + 25, y + 35);
    qdOK->setPosition(x + QD_W - 80, y + QD_H - 32);
    qdCancel->setPosition(x + QD_W - 154, y + QD_H - 32);

    dbDelete->setPosition(DB_W - 32, WINDOW_H - PL_B + 20);
    dbSettings->setPosition(DB_W - 60, WINDOW_H - PL_B + 20);
    dbUp->setPosition(DB_W - 88, WINDOW_H - PL_B + 20);
    dbDown->setPosition(DB_W - 116, WINDOW_H - PL_B + 20);
    dbToMain->setPosition(10, WINDOW_H - PL_B + 20);

    delete swScroll;
    swScroll = new ScrollWindow(sf::FloatRect(0, 0, WINDOW_W, WINDOW_H),
        sf::FloatRect((WINDOW_W - SW_W) / 2, (WINDOW_H - SW_H) / 2, WINDOW_W * 2 - SW_W, WINDOW_H * 2 - SW_H));
    x = WINDOW_W - SW_W, y = WINDOW_H - SW_H;
    swBack->setPosition(x, y);
    swHeader->setPosition(x + 160, y + 5);
    swNameLb->setPosition(x + 18, y + 30);
    swPathLb->setPosition(x + 18, y + 100);

    swOK->setPosition(x + SW_W - 80, y + SW_H - 32);
    swCancel->setPosition(x + SW_W - 154, y + SW_H - 32);
    swGetPath->setPosition(x + 15 + SW_TXTW + 6, y + SW_TPY);
    swName->setPosition(x + 20, y + SW_TNY + 6);
    swPath->setPosition(x + 20, y + SW_TPY + 6);
}

void DataBase::loadBindedBases(char * path)
{
    std::wifstream fin(path);
    std::wstring file, name = L"Unknown";
    uint16_t id = 0;
    while (std::getline(fin, file))
    {
        dataPaths.push_back(file);
        name = file;
        std::getline(fin, name);
        dataNames.push_back(name);
        loadFromFile(file, std::string(name.begin(), name.end()), file, id++);
    }
    focusAtHead(dataBase.begin());
    fin.close();
}

void DataBase::checkPressed(int X, int Y)
{
    if (windowMode == 1) {
        if (polyScroll->checkPressed(X, Y))
        {
            for (auto clh : closeHeads)
            {
                clh->checkPressed(polyScroll->control, polyScroll->mindPos, X, Y);
            }
            int bths = 0;
            for (auto it = dataBase.begin(); it != dataBase.end(); ++it)
            {
                if ((*it).head->checkPressed(polyScroll->control, polyScroll->mindPos, X, Y))
                {
                    focusAtHead(it);
                }
                if (bths * BT_H > Y + polyScroll->mindPos.y - polyScroll->control.top) continue;
                bths += (*it).base.size() * (*it).cls->meta + 1;
                if (bths * BT_H > Y + polyScroll->mindPos.y - polyScroll->control.top)
                {
                    for (auto itp = it->base.begin(); itp != it->base.end(); ++itp)
                        if ((*itp).inlist->checkPressed(polyScroll->control, polyScroll->mindPos, X, Y))
                            focusAtPoly(it, itp);
                }
            }
        }
        if (dbByK->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y)) updateSortType(1);
        if (dbByP->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y)) updateSortType(2);
        if (dbByC->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y)) updateSortType(0);
        dbClose->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbAddFile->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbAddNew->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbDelete->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbSettings->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbUp->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbDown->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
        dbToMain->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
    }
    else if (windowMode == 2){
        qdScroll->checkPressed(X, Y);
        qdOK->checkPressed(qdScroll->control, qdScroll->mindPos, X, Y);
        qdCancel->checkPressed(qdScroll->control, qdScroll->mindPos, X, Y);
    }
    else if (windowMode == 3) {
        swScroll->checkPressed(X, Y);
        swOK->checkPressed(swScroll->control, swScroll->mindPos, X, Y);
        swCancel->checkPressed(swScroll->control, swScroll->mindPos, X, Y);
        swName->offFocused();
        swName->checkPressed(swScroll->control, swScroll->mindPos, X, Y, false);
        swPath->offFocused();
        swPath->checkPressed(swScroll->control, swScroll->mindPos, X, Y, false);
        swGetPath->checkPressed(swScroll->control, swScroll->mindPos, X, Y);
    }
    else if (windowMode == -1) 
        dbOpen->checkPressed(dbScroll->control, dbScroll->mindPos, X, Y);
}

void DataBase::checkMoved(int X, int Y, float dx, float dy)
{
    if (windowMode == 1)
    {
        polyScroll->checkMove(dx, dy);
        for (auto clh : closeHeads)
        {
            clh->checkOver(polyScroll->control, polyScroll->mindPos, X, Y);
        }
        dbByK->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbByP->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbByC->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbClose->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbAddFile->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbAddNew->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);

        dbDelete->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbSettings->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbUp->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbDown->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
        dbToMain->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
    }
    else if (windowMode == 2) {
        qdScroll->checkMove(dx, dy);
        qdOK->checkOver(qdScroll->control, qdScroll->mindPos, X, Y);
        qdCancel->checkOver(qdScroll->control, qdScroll->mindPos, X, Y);
    }
    else if (windowMode == 3) {
        swScroll->checkMove(dx, dy);
        swOK->checkOver(swScroll->control, swScroll->mindPos, X, Y);
        swCancel->checkOver(swScroll->control, swScroll->mindPos, X, Y);
        swGetPath->checkOver(swScroll->control, swScroll->mindPos, X, Y);
    }
    else if (windowMode == -1) 
        dbOpen->checkOver(dbScroll->control, dbScroll->mindPos, X, Y);
}

void DataBase::checkReleased(int X, int Y)
{
    polyScroll->isPressed = false;
    qdScroll->isPressed = false;
    swScroll->isPressed = false;
    if (windowMode == 1) {
        for (auto clh : closeHeads)
        {
            if (clh->checkReleased(polyScroll->control, polyScroll->mindPos, X, Y))
            {
                clh->backGround->setScale(1, clh->meta ? -1 : 1);
                clh->backGround->setOrigin(0, clh->meta ? 21 : -5);
                clh->meta ^= 1;
            }
        }
        if (dbDelete->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y)) {
			if (selectedHead != dataBase.end())
			{
				if (selectedType == 1)
					openQuestion("Are you sure to delete     this library?", deleteFocused);
				else if (selectedType == 2)
					openQuestion("Are you sure to delete     this item?", deletePoly);
			}
        }
        if (dbUp->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y)) {
            moveFocusedUp();
        }
        if (dbDown->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y)) {
            moveFocusedDown();
        }
        if (dbSettings->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y)) {
			if (selectedHead != dataBase.end())
			{
				openSettings("Library name:", "Absolute path(txt format):", updateFocused);
			}
        }
        if (dbClose->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y))
        {
            windowMode = -1;
            dbLeftBound->setPosition(dbLeftBound->getPosition().x + DB_W, 0);
        }
        if (dbToMain->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y))
        {
            outText = "";
            for (auto m : selectedPoly->chars)
            {
                if (outText.size()) outText += m.first > 0 ? " + " : " - ";
                else if (m.first < 0) outText += '-';
                if (abs(m.first) != 1 || m.second == 0) outText += std::to_string(abs(m.first));
                if (m.second)outText += 'x' + std::to_string(m.second);
            }
        }
        if (dbAddFile->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y)) {
            auto newres = swScroll->openFile();
            if (newres.first)
            {
                if (newres.first < 0)
                    openQuestion("Sorry! We can't open the file", [](DataBase * db, bool b) { db->windowMode = 1; }, 0);
            }
            else if (newres.second.substr(newres.second.size() - 3) != L"txt")
            {
                openQuestion("Sorry! The file must be in  the 'txt' format", [](DataBase * db, bool b) { db->windowMode = 1; }, 0);
            }
            else 
            {
                swName->text = "My library" + std::to_string(dataBase.size() + 1);
                std::ifstream fin(newres.second);
                if (fin.is_open())
                {
                    std::string name;
                    fin >> name;
                    if (name.size() == 0 || name.size() > 32)
                        openQuestion("Sorry! Invalid file syntax", [](DataBase * db, bool b) { db->windowMode = 1; }, 0);
                    else
                    {
                        openSettings("Library name:", "Absolute path(txt format):", addLibrary);
                        swName->text = name;
                        swName->updateText();
                        swName->redrawTexture();
                        swPathName = newres.second;
                        swPath->text = newres.second;
                        swPath->updateText();
                        swPath->redrawTexture();
                    }
                }
                else openQuestion("Sorry! We couldn't open the file", [](DataBase * db, bool b) { db->windowMode = 1; }, 0);
            }
        }
        else if (dbAddNew->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y))
        {
            createLibrary();
        }
    }
    else if (windowMode == 2) {
        if (qdOK->checkReleased(qdScroll->control, qdScroll->mindPos, X, Y))
        {
            closeQuestion();
            qDialReact(this, true);
        }
        if (qdCancel->checkReleased(qdScroll->control, qdScroll->mindPos, X, Y))
        {
            closeQuestion();
            qDialReact(this, false);
        }
    }
    else if (windowMode == 3) {
        if (swOK->checkReleased(swScroll->control, swScroll->mindPos, X, Y))
        {
            windowMode = 1;
            swDialReact(this, 1);
        }
        if (swCancel->checkReleased(swScroll->control, swScroll->mindPos, X, Y))
        {
            windowMode = 1;
            swDialReact(this, 0);
        }
        if (swGetPath->checkReleased(swScroll->control, swScroll->mindPos, X, Y))
        {
            auto res = swScroll->openFile();
            if (res.first)
            {
                if (res.first < 0)
                    openQuestion("Sorry! We can't open the file", [](DataBase * db, bool b) { db->windowMode = 3; }, 0);
            }
            else if (res.second.substr(res.second.size() - 3) != L"txt")
            {
                openQuestion("Sorry! The file must be in  the 'txt' format", [](DataBase * db, bool b) { db->windowMode = 3; }, 0);
            }
            else
            {
                swPath->text = res.second;
                swPath->updateText();
                swPath->redrawTexture();
                swPathName = res.second;
            }
        }
    }
    else if (windowMode == -1 && dbOpen->checkReleased(dbScroll->control, dbScroll->mindPos, X, Y))
    {
        windowMode = 1;
        dbLeftBound->setPosition(dbLeftBound->getPosition().x - DB_W, 0);
    }
}

void DataBase::checkDoubleClick(int X, int Y)
{
    if (selectedType == 2)
    {
        int x = X + polyScroll->mindPos.x - polyScroll->control.left;
        int y = Y + polyScroll->mindPos.y - polyScroll->control.top;
        if (selectedPoly->inlist->control.contains(x, y))
        {
            outText = "";
            for (auto m : selectedPoly->chars)
            {
                if (outText.size()) outText += m.first > 0 ? " + " : " - ";
                else if (m.first < 0) outText += '-';
                if (abs(m.first) != 1 || m.second == 0) outText += std::to_string(abs(m.first));
                if (m.second)outText += 'x' + std::to_string(m.second);
            }
        }
    }
}

void DataBase::checkKeyPressed(sf::Event::KeyEvent key)
{
    if (swName->checkKeyPressed(key, [](char c) {
        return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9'
            || c == ' ' || c == '-' || c == '_';
    }))
        swOK->setIsEnabled(!swName->text.isEmpty());
    if (swPath->checkKeyPressed(key, [](char c) {
        return c >= 32;
    }))
        swOK->setIsEnabled(!swPath->text.isEmpty());
}

void DataBase::checkTextEntered(sf::Event::TextEvent text)
{
    if (swName->checkInput(text.unicode, [](char c) {
        return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9'
            || c == ' ' || c == '-' || c == '_';
    })) swOK->setIsEnabled(true);
    if (swPath->checkInput(text.unicode, [](char c) {
        return c >= 32;
    })) swOK->setIsEnabled(true);
}

void DataBase::openQuestion(std::string text, void (*react)(DataBase * db, bool answer), bool hascancel)
{
    qDialReact = react;
    windowMode = 2;
    qdQuestion->text = text;
    qdQuestion->updateText();
    qdCancel->setIsEnabled(hascancel);
}

void DataBase::openSettings(std::string nm, std::string pt, void(*react)(DataBase *db, bool answer))
{
    swDialReact = react;
    windowMode = 3;
    swNameLb->text = nm;
    swNameLb->updateText();
    swPathLb->text = pt;
    swPathLb->updateText();
    if (selectedHead != dataBase.end())
    {
        swName->text = selectedHead->name;
        swName->updateText();
        swName->redrawTexture();
        swPath->text = selectedHead->path;
        swPath->updateText();
        swPath->redrawTexture();
        swPathName = selectedHead->path;
    }
}

void DataBase::closeQuestion()
{
    windowMode = 1;
}

void DataBase::createLibrary()
{
    openSettings("Library name:", "Absolute path(txt format):", loadLibrary);
    swName->text = "My library " + std::to_string(dataBase.size() + 1);
    swName->updateText();
    swName->redrawTexture();
    swPathName = dataPath + L"base" + std::to_wstring(dataBase.size() + 1);
    std::wifstream swtest(swPathName + L".txt");
    while (swtest.is_open())
    {
        swtest.close();
        swPathName += '_';
        swtest.open(swPathName + L".txt");
    }
    swtest.close();
    swPathName += L".txt";
    swPath->text = swPathName;
    swPath->updateText();
    swPath->redrawTexture();
}

bool DataBase::saveLibrary(std::list<Library>::iterator lib)
{
    if (!lib->connected) return false;
    std::ofstream fout(lib->path);
    bool is_open = fout.is_open();
    if (is_open)
    {
        fout << lib->name << '\n';
        for (auto p : lib->base)
        {
            auto pp = p.chars.begin();
            for (int i = 0; i < 2; ++i)
                fout << char((p.serial >> (6 - 6 * i)) % 64 + 32);
            while (pp != p.chars.end())
            {
                unsigned int n = pp->first, pow = pp->second;
                for (int j = 0; j < 2; ++j, n = pow)
                    for (int i = 0; i < 6; ++i)
                        fout << char((n >> (30 - 6 * i)) % 64 + 32);
                ++pp;
            }
            fout << '\n';
        }
    }
    fout.close();
    return is_open;
}

bool DataBase::savePaths()
{
    std::wofstream fout(L"data/paths.ini");
    if (fout.is_open())
    {
        for (auto it = dataBase.begin(); it != dataBase.end(); ++it)
            fout << it->path << '\n' << sf::String(it->name).toWideString() << '\n';
    }
    fout.close();
    return true;
}

DataBase::~DataBase()
{
    for (auto p : listButtons)
        if(p) delete p;
    for (auto h : buttonHeads)
        if (h) delete h;
    for (auto c : closeHeads)
        if (c) delete c;
    delete rt; delete dbBackGround->getTexture();
    delete dbBackGround; delete dbLeftBound->getTexture();
    delete dbLeftBound; delete dbClose; delete dbLb;
    delete qdScroll; delete qdBack; delete qdOK;
    delete qdCancel; delete qdWarning; delete qdQuestion;
    delete dbDelete; delete dbSettings; delete dbUp;
    delete dbDown; delete dbTopBorder; delete dbByK;
    delete dbByP; delete dbByC; delete swScroll; delete swBack;
    delete swNameLb; delete swPathLb; delete swHeader; delete swOK;
    delete swCancel; delete swCorners; delete swTextBody; 
    delete swName; delete swPath; delete swGetPath; 
    delete dbAddFile; delete dbAddNew; delete dbToMain; delete dbOpen;
}

void DataBase::moveFocusedUp()
{
    if (selectedType == 1)
    {
        if (selectedHead == dataBase.begin()) return;
        auto toward = selectedHead;
        --toward;
        std::swap(*toward, *selectedHead);
        selectedHead = toward;
    }
    else if (selectedType == 2)
    {
        updateSortType(0);
        if (selectedPoly == selectedPolyC->base.begin())
        {
            if (selectedPolyC == dataBase.begin()) return;
            auto itc = selectedPolyC;
            --itc;
            itc->base.push_back(*selectedPoly);
            selectedPolyC->base.erase(selectedPoly);
            selectedPoly = itc->base.end(), --selectedPoly;
            saveLibrary(selectedPolyC);
            saveLibrary(itc);
            selectedPolyC = itc;
            return;
        }
        auto toward = selectedPoly;
        --toward;
        std::swap(*toward, *selectedPoly);
        selectedPoly = toward;
        saveLibrary(selectedPolyC);
    }
}

void DataBase::moveFocusedDown()
{
    if (selectedType == 1)
    {
        auto toward = selectedHead;
        ++toward;
        if (toward == dataBase.end()) return;
        std::swap(*toward, *selectedHead);
        selectedHead = toward;
    }
    else if(selectedType == 2)
    {
        updateSortType(0);
        auto toward = selectedPoly;
        ++toward;
        if (toward == selectedPolyC->base.end())
        {
            auto itc = selectedPolyC;
            ++itc;
            if (itc == dataBase.end()) return;
            itc->base.push_front(*selectedPoly);
            selectedPolyC->base.erase(selectedPoly);
            selectedPoly = itc->base.begin();
            saveLibrary(selectedPolyC);
            saveLibrary(itc);
            selectedPolyC = itc;
            return;
        }
        std::swap(*toward, *selectedPoly);
        selectedPoly = toward;
        saveLibrary(selectedPolyC);
    }
}

void DataBase::sortList(std::list<Polynomial> & base)
{
    if (sortType == 1)
        base.sort([](const DataBase::Polynomial & a, const DataBase::Polynomial & b) {
        int ka = a.chars.size() ? (*a.chars.begin()).first : 0;
        int kb = b.chars.size() ? (*b.chars.begin()).first : 0;
        return ka < kb;
    });
    else if (sortType == 2)
        base.sort([](const DataBase::Polynomial & a, const DataBase::Polynomial & b) {
        int pa = a.chars.size() ? (*a.chars.begin()).second : 0;
        int pb = b.chars.size() ? (*b.chars.begin()).second : 0;
        return pa < pb;
    });
}

void DataBase::updateSortType(int type)
{
    if (sortType != type)
    {
        dbByK->isPressed = dbByP->isPressed = dbByC->isPressed = false;
        dbByK->isOver = dbByP->isOver = dbByC->isOver = false;
        if (type == 0) dbByC->isPressed = true;
        else if (type == 1) dbByK->isPressed = true;
        else if (type == 2) dbByP->isPressed = true;
        dbByK->update();
        dbByP->update();
        dbByC->update();
        sortType = type;
        for (auto btn = dataBase.begin(); btn != dataBase.end(); ++btn)
            sortList((*btn).base);
    }
}

DataBase::Polynomial DataBase::sum(Polynomial a, Polynomial b)
{
    std::map<int, int> pm;
    for (auto m : a.chars)
        pm[-m.second] += m.first;
    for (auto m : b.chars)
        pm[-m.second] += m.first;
    Polynomial p = getFromMap(pm);
    p.serial = a.serial + b.serial;
    return p;
}

DataBase::Polynomial DataBase::subtract(Polynomial a, Polynomial b)
{
    std::map<int, int> pm;
    for (auto m : a.chars)
        pm[-m.second] += m.first;
    for (auto m : b.chars)
        pm[-m.second] -= m.first;
    Polynomial p = getFromMap(pm);
    p.serial = a.serial - b.serial;
    return p;
}

DataBase::Polynomial DataBase::multiply(Polynomial a, Polynomial b)
{
    std::map<int, int> pm;
    for (auto xz : a.chars)
        for (auto y : b.chars)
        {
            auto x = xz;
            x.first *= y.first;
            x.second += y.second;
            pm[-x.second] += x.first;
        }
    Polynomial p = getFromMap(pm);
    p.serial = a.serial * b.serial;
    return p;
}

DataBase::Polynomial DataBase::derivative(Polynomial a)
{
    std::map<int, int> pm;
    for (auto m : a.chars)
        if (m.second) pm[-m.second + 1] += m.first * m.second;
    return getFromMap(pm);
}

double DataBase::getvalue(Polynomial p, double x)
{
    double y = 0;
    for (auto m : p.chars)
    {
        double t = m.first * pow(x, m.second);
        if (t <= -1e18 || t >= 1e18) return INFINITY;
        y += t;
        if (y <= -1e18 || y >= 1e18) return INFINITY;
    }
    return y;
}

DataBase::Polynomial DataBase::getFromMap(std::map<int, int>& pm)
{
    Polynomial p;
    for (auto m : pm)
        if (m.second) p.chars.push_back({ m.second, -m.first });
    for (auto m : p.chars)
    {
        if (p.astext.size())p.astext += m.first > 0 ? " + " : " - ";
        else if (m.first < 0)p.astext += '-';
        if (abs(m.first) != 1 || m.second == 0)p.astext += std::to_string(abs(m.first));
        if (m.second && m.first)p.astext += 'x' + (m.second != 1 ? std::to_string(m.second) : "");
    }
    if (p.astext == "") p.astext = "0";
    return p;
}

double DataBase::pow(double x, int p, double max)
{
    double ans = 1;
    while (p)
    {
        if (p % 2) ans *= x, --p;
        else x *= x, p >>= 1;
    }
    return ans;
}

void deleteFocused(DataBase * db, bool answer)
{
    if (db->selectedType == 1 && answer)
    {
        auto itb = db->selectedHead, nxt = itb;
        if (++nxt != db->dataBase.end())
            db->focusAtHead(nxt);
        else if (itb == db->dataBase.begin())
            db->selectedHead = db->dataBase.end();
        else nxt = itb, db->focusAtHead(--nxt);
		if (itb != db->dataBase.end())
			db->dataBase.erase(itb);
        db->savePaths();
        if (db->dataBase.empty())
        {
            db->dbDelete->setIsEnabled(false);
            db->dbSettings->setIsEnabled(false);
            db->dbUp->setIsEnabled(false);
            db->dbDown->setIsEnabled(false);
            db->dbLb->mainText.setString("Sorry, no libraries");
        }
    }
}

void deletePoly(DataBase * db, bool answer)
{
    if (db->selectedType == 2 && answer)
    {
        auto itb = db->selectedPoly, nxt = itb;
        if (++nxt != db->selectedPolyC->base.end())
            db->focusAtPoly(db->selectedPolyC, nxt);
        else if (itb == db->selectedPolyC->base.begin())
            db->selectedPoly = db->selectedPolyC->base.end();
        else nxt = itb, db->focusAtPoly(db->selectedPolyC, --nxt);
        db->selectedPolyC->base.erase(itb);
        db->saveLibrary(db->selectedPolyC);
        if (db->selectedPolyC->base.empty())
        {
            auto tmp = db->selectedPolyC;
            db->selectedPolyC = db->dataBase.end();
            db->focusAtHead(tmp);
        }
    }
}


void updateFocused(DataBase * db, bool answer)
{
    if (db->selectedType == 1 && answer)
    {
        auto itb = db->selectedHead;
        db->loadFromFile(db->swPathName, db->swName->text, db->swPathName, itb->id);
        auto lb = db->dataBase.end();
        --lb;
        itb->base.swap(lb->base);
        itb->cls = lb->cls;
        itb->head = lb->head;
        if (lb->head->mainText.getString() == lb->name && db->swName->text != lb->name)
        {
            itb->head->mainText.setString(db->swName->text);
            itb->name = db->swName->text;
        }
        else itb->name = lb->name;
        itb->path = lb->path;
        db->dataBase.pop_back();
        db->saveLibrary(db->selectedHead);
        db->savePaths();
        db->focusAtHead(db->selectedHead);
    }
}

void addLibrary(DataBase * db, bool answer)
{
    if (db->selectedType == 1 && answer)
    {
        if (db->dataBase.empty())
        {
            db->dbDelete->setIsEnabled(true);
            db->dbSettings->setIsEnabled(true);
            db->dbUp->setIsEnabled(true);
            db->dbDown->setIsEnabled(true);
        }
        db->loadFromFile(db->swPathName, db->swName->text, db->swPathName, db->dataBase.size());
        auto lb = db->dataBase.end();
        --lb;
        lb->inner = false;
        db->focusAtHead(lb);
        db->savePaths();
    }
}

void loadLibrary(DataBase * db, bool answer)
{
    if (answer)
    {
        if (db->dataBase.empty())
        {
            db->dbDelete->setIsEnabled(true);
            db->dbSettings->setIsEnabled(true);
            db->dbUp->setIsEnabled(true);
            db->dbDown->setIsEnabled(true);
        }
        std::string name = db->swName->text.toAnsiString();
        auto rbs = db->addDataHead((char *)name.c_str(), db->buttonHeads.size());
        std::list<DataBase::Polynomial> base;
        DataBase::Library lib(rbs.first, rbs.second, base, name, db->swPathName, 1);
        db->dataBase.push_back(lib);
        auto lb = db->dataBase.end();
        --lb;
        db->focusAtHead(lb);
        db->saveLibrary(lb);
        db->savePaths();
    }
}

bool cmp(const DataBase::Polynomial & a, const DataBase::Polynomial & b)
{
    return a.chars.size() > b.chars.size();
}
