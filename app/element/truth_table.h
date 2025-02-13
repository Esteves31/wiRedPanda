// Copyright 2015 - 2025, GIBIS-UNIFESP and the wiRedPanda contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "graphicelement.h"

class TruthTable : public GraphicElement
{
    Q_OBJECT

public:
    explicit TruthTable(QGraphicsItem *parent = nullptr);

    QBitArray &key();
    void load(QDataStream &stream, QMap<quint64, QNEPort *> &portMap, const QVersionNumber version) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void save(QDataStream &stream) const override;
    void setkey(const QBitArray &key);
    void update();
    void updatePortsProperties() override;

private:
    void generatePixmap();

    QBitArray m_key;
};

Q_DECLARE_METATYPE(TruthTable)
