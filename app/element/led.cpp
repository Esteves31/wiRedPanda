// Copyright 2015 - 2022, GIBIS-Unifesp and the WiRedPanda contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "led.h"

#include "common.h"
#include "qneport.h"

#include <bitset>

namespace
{
int id = qRegisterMetaType<Led>();
}

/* Color pallets:
 *
 * 2 bit
 *     00: dark gray,
 *     01: light blue,
 *     10: light green,
 *     11: light red
 *
 * 3 bit
 *     000: dark gray,
 *     001: light blue,
 *     010: light green,
 *     011: yellow,
 *     100: light red,
 *     101: magenta,
 *     110: cyan,
 *     111: white
 *
 * 4 bit
 *     0000: black,
 *     0001: blue,
 *     0010: green,
 *     0011: teal,
 *     0100: red,
 *     0101: magenta,
 *     0110: orange,
 *     0111: light gray
 *     1000: dark gray,
 *     1001: light blue,
 *     1010: light green,
 *     1011: cyan,
 *     1100: light red,
 *     1101: pink,
 *     1110: yellow,
 *     1111: white
 */

Led::Led(QGraphicsItem *parent)
    : GraphicElement(ElementType::Led, ElementGroup::Output, 1, 4, 0, 0, parent)
{
    qCDebug(zero) << "Creating led.";

    m_pixmapSkinName = QStringList{
        ":/output/WhiteLedOff.png",             // Single input values: 0
        ":/output/WhiteLedOn.png",              // 1
        ":/output/RedLedOff.png",               // 2
        ":/output/RedLedOn.png",                // 3
        ":/output/GreenLedOff.png",             // 4
        ":/output/GreenLedOn.png",              // 5
        ":/output/BlueLedOff.png",              // 6
        ":/output/BlueLedOn.png",               // 7
        ":/output/PurpleLedOff.png",            // 8
        ":/output/PurpleLedOn.png",             // 9
        ":/output/16colors/BlackLedOn.png",     // Multiple input values: 10
        ":/output/16colors/NavyBlueLedOn.png",  // 11
        ":/output/16colors/GreenLedOn.png",     // 12
        ":/output/16colors/TealLedOn.png",      // 13
        ":/output/16colors/DarkRedLedOn.png",   // 14
        ":/output/16colors/MagentaLedOn.png",   // 15
        ":/output/16colors/OrangeLedOn.png",    // 16
        ":/output/16colors/LightGrayLedOn.png", // 17
        ":/output/16colors/DarkGrayLedOn.png",  // 18
        ":/output/16colors/RoyalLedOn.png",     // 19
        ":/output/16colors/LimeGreenLedOn.png", // 20
        ":/output/16colors/AquaLightLedOn.png", // 21
        ":/output/16colors/RedLedOn.png",       // 22
        ":/output/16colors/HotPinkLedOn.png",   // 23
        ":/output/16colors/YellowLedOn.png",    // 24
        ":/output/16colors/WhiteLedOn.png",     // 25
    };
    setPixmap(m_pixmapSkinName.first());

    setOutputsOnTop(true);
    setRotatable(false);
    setHasColors(true);
    Led::updatePorts();
    setHasLabel(true);
    setCanChangeSkin(true);
    setPortName("Led");
    setToolTip(m_translatedName);
}

void Led::refresh()
{
    int index = 0;
    if (isValid()) {
        std::bitset<4> indexBit;
        for (int i = 0; i < inputSize(); ++i) {
            indexBit[i] = input(inputSize() - i - 1)->value();
        }
        index = static_cast<int>(indexBit.to_ulong());
    }
    switch (inputSize()) {
    case 1: { /* 1 bit */
        setPixmap(m_pixmapSkinName[m_colorNumber + index]);
        input(0)->setName("");
        input(0)->setPos(32, bottomPosition());
        break;
    }
    case 2: { /* 2 bits */ // TODO: add option to select dark/light colors according to the theme.
        if (index == 3) {
            setPixmap(m_pixmapSkinName[22]);
        } else {
            setPixmap(m_pixmapSkinName[18 + index]);
        }
        input(0)->setName("0");
        input(0)->setPos(24, bottomPosition());
        input(1)->setName("1");
        input(1)->setPos(40, bottomPosition());
        break;
    }
    case 3: { /* 3 bits */ // TODO: add option to select dark/light colors according to the theme.
        setPixmap(m_pixmapSkinName[18 + index]);
        input(0)->setName("0");
        input(0)->setPos(16, bottomPosition());
        input(1)->setName("1");
        input(1)->setPos(32, bottomPosition());
        input(2)->setName("2");
        input(2)->setPos(48, bottomPosition());
        break;
    }
    case 4: { /* 4 bits */
        setPixmap(m_pixmapSkinName[10 + index]);
        input(0)->setName("0");
        input(0)->setPos(8, bottomPosition());
        input(1)->setName("1");
        input(1)->setPos(24, bottomPosition());
        input(2)->setName("2");
        input(2)->setPos(40, bottomPosition());
        input(3)->setName("3");
        input(3)->setPos(56, bottomPosition());
        break;
    }
    }
}

void Led::setColor(const QString &color)
{
    m_color = color;
    if (color == "White")  { m_colorNumber = 0; }
    if (color == "Red")    { m_colorNumber = 2; }
    if (color == "Green")  { m_colorNumber = 4; }
    if (color == "Blue")   { m_colorNumber = 6; }
    if (color == "Purple") { m_colorNumber = 8; }
    refresh();
}

QString Led::color() const
{
    return m_color;
}

void Led::save(QDataStream &stream) const
{
    GraphicElement::save(stream);
    stream << color();
}

void Led::load(QDataStream &stream, QMap<quint64, QNEPort *> &portMap, const double version)
{
    GraphicElement::load(stream, portMap, version);
    if (version >= 1.1) {
        QString clr;
        stream >> clr;
        setColor(clr);
    }
}

QString Led::genericProperties()
{
    return color();
}

void Led::updatePorts()
{
    setHasColors(inputSize() == 1);
    // GraphicElement::updatePorts();
}

void Led::setSkin(const bool defaultSkin, const QString &fileName)
{
    int index = 0;
    if (isValid()) {
        std::bitset<4> indexBit;
        for (int i = 0; i < inputSize(); ++i) {
            indexBit[i] = input(inputSize() - i - 1)->value();
        }
        index = static_cast<int>(indexBit.to_ulong());
    }
    int valueIndex = 0;
    switch (inputSize()) {
    case 1: /* 1 bit */
        valueIndex = m_colorNumber + index;
        break;

    case 2: /* 2 bits */ // TODO: add option to select dark/light colors according to the theme.
        valueIndex = (index == 3) ? 22 : 18 + index;
        break;

    case 3: /* 3 bits */ // TODO: add option to select dark/light colors according to the theme.
        valueIndex = 18 + index;
        break;

    case 4: /* 4 bits */
        valueIndex = 10 + index;
        break;
    }

    if (defaultSkin) {
        resetLedPixmapName(valueIndex);
    } else {
        m_pixmapSkinName[valueIndex] = fileName;
    }
    setPixmap(m_pixmapSkinName[valueIndex]);
}

void Led::resetLedPixmapName(const int ledNumber)
{
    switch (ledNumber) {
    case 0:  m_pixmapSkinName[0]  = ":/output/WhiteLedOff.png"; break;
    case 1:  m_pixmapSkinName[1]  = ":/output/WhiteLedOn.png"; break;
    case 2:  m_pixmapSkinName[2]  = ":/output/RedLedOff.png"; break;
    case 3:  m_pixmapSkinName[3]  = ":/output/RedLedOn.png"; break;
    case 4:  m_pixmapSkinName[4]  = ":/output/GreenLedOff.png"; break;
    case 5:  m_pixmapSkinName[5]  = ":/output/GreenLedOn.png"; break;
    case 6:  m_pixmapSkinName[6]  = ":/output/BlueLedOff.png"; break;
    case 7:  m_pixmapSkinName[7]  = ":/output/BlueLedOn.png"; break;
    case 8:  m_pixmapSkinName[8]  = ":/output/PurpleLedOff.png"; break;
    case 9:  m_pixmapSkinName[9]  = ":/output/PurpleLedOn.png"; break;
    case 10: m_pixmapSkinName[10] = ":/output/16colors/BlackLedOn.png"; break;
    case 11: m_pixmapSkinName[11] = ":/output/16colors/NavyBlueLedOn.png"; break;
    case 12: m_pixmapSkinName[12] = ":/output/16colors/GreenLedOn.png"; break;
    case 13: m_pixmapSkinName[13] = ":/output/16colors/TealLedOn.png"; break;
    case 14: m_pixmapSkinName[14] = ":/output/16colors/DarkRedLedOn.png"; break;
    case 15: m_pixmapSkinName[15] = ":/output/16colors/MagentaLedOn.png"; break;
    case 16: m_pixmapSkinName[16] = ":/output/16colors/OrangeLedOn.png"; break;
    case 17: m_pixmapSkinName[17] = ":/output/16colors/LightGrayLedOn.png"; break;
    case 18: m_pixmapSkinName[18] = ":/output/16colors/DarkGrayLedOn.png"; break;
    case 19: m_pixmapSkinName[19] = ":/output/16colors/RoyalLedOn.png"; break;
    case 20: m_pixmapSkinName[20] = ":/output/16colors/LimeGreenLedOn.png"; break;
    case 21: m_pixmapSkinName[21] = ":/output/16colors/AquaLightLedOn.png"; break;
    case 22: m_pixmapSkinName[22] = ":/output/16colors/RedLedOn.png"; break;
    case 23: m_pixmapSkinName[23] = ":/output/16colors/HotPinkLedOn.png"; break;
    case 24: m_pixmapSkinName[24] = ":/output/16colors/YellowLedOn.png"; break;
    case 25: m_pixmapSkinName[25] = ":/output/16colors/WhiteLedOn.png"; break;
    }
}
