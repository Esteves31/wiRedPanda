// Copyright 2015 - 2022, GIBIS-UNIFESP and the WiRedPanda contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "graphicelement.h"

class Text : public GraphicElement
{
    Q_OBJECT

public:
    explicit Text(QGraphicsItem *parent = nullptr);
};

Q_DECLARE_METATYPE(Text)
