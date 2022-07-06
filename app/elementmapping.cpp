// Copyright 2015 - 2022, GIBIS-Unifesp and the WiRedPanda contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "elementmapping.h"

#include "clock.h"
#include "common.h"
#include "elementfactory.h"
#include "graphicelement.h"
#include "graphicelementinput.h"
#include "ic.h"
#include "qneconnection.h"
#include "qneport.h"

ElementMapping::ElementMapping(const QVector<GraphicElement *> &elms)
    : m_elements(elms)
{
    qCDebug(three) << tr("Generate Map.");
    generateMap();
    qCDebug(three) << tr("Connect.");
    connectElements();
}

ElementMapping::~ElementMapping()
{
    clear();
}

void ElementMapping::clear()
{
    m_globalGND.clearSucessors();
    m_globalVCC.clearSucessors();
    qDeleteAll(m_deletableLogics);
}

void ElementMapping::generateMap()
{
    for (auto *elm : qAsConst(m_elements)) {
        if (elm->elementType() == ElementType::IC) {
            auto *ic = qobject_cast<IC *>(elm);
            m_logicElms.append(ic->generateMap()->m_logicElms);
            continue;
        }

        generateLogic(elm);
    }
}

void ElementMapping::generateLogic(GraphicElement *elm)
{
    auto *logicElm = ElementFactory::buildLogicElement(elm);
    elm->setLogic(logicElm);
    m_deletableLogics.append(logicElm);
    m_logicElms.append(logicElm);
}

void ElementMapping::connectElements()
{
    for (auto *elm : qAsConst(m_elements)) {
        for (auto *inputPort : elm->inputs()) {
            applyConnection(elm, inputPort);
        }
    }
}

void ElementMapping::applyConnection(GraphicElement *elm, QNEInputPort *inputPort)
{
    LogicElement *currentLogElm;
    int inputIndex = 0;

    if (elm->elementType() == ElementType::IC) {
        auto *ic = qobject_cast<IC *>(elm);
        currentLogElm = ic->icInput(inputPort->index());
    } else {
        currentLogElm = elm->logic();
        inputIndex = inputPort->index();
    }

    const int connections = inputPort->connections().size();

    if ((connections == 0) && !inputPort->isRequired()) {
        LogicElement *predecessor = (inputPort->defaultValue() == Status::Active) ? &m_globalVCC : &m_globalGND;
        currentLogElm->connectPredecessor(inputIndex, predecessor, 0);
    }

    // TODO: and if there is more than one input? use std::any_of?
    if (connections == 1) {
        if (QNEPort *otherPort = inputPort->connections().first()->otherPort(inputPort)) {
            if (GraphicElement *predecessor = otherPort->graphicElement()) {
                LogicElement *predOutElm;
                int predOutIndex = 0;

                if (auto *ic = qobject_cast<IC *>(predecessor)) {
                    predOutElm = ic->icOutput(otherPort->index());
                } else {
                    predOutElm = predecessor->logic();
                    predOutIndex = otherPort->index();
                }

                currentLogElm->connectPredecessor(inputIndex, predOutElm, predOutIndex);
            }
        }
    }
}

void ElementMapping::sort()
{
    sortLogicElements();
    validateElements();
}

void ElementMapping::sortLogicElements()
{
    for (auto *logic : qAsConst(m_logicElms)) {
        logic->calculatePriority();
    }

    std::sort(m_logicElms.begin(), m_logicElms.end(), [](const auto &logic1, const auto &logic2) {
        return *logic2 < *logic1;
    });
}

void ElementMapping::validateElements()
{
    for (auto *logic : qAsConst(m_logicElms)) {
        logic->validate();
    }
}

QVector<GraphicElement *> ElementMapping::sortGraphicElements(QVector<GraphicElement *> elms)
{
    QHash<GraphicElement *, bool> beingVisited;
    QHash<GraphicElement *, int> priorities;

    for (auto *elm : elms) {
        calculatePriority(elm, beingVisited, priorities);
    }

    std::sort(elms.begin(), elms.end(), [priorities](const auto &e1, const auto &e2) {
        return priorities[e2] < priorities[e1];
    });

    return elms;
}

int ElementMapping::calculatePriority(GraphicElement *elm, QHash<GraphicElement *, bool> &beingVisited, QHash<GraphicElement *, int> &priorities)
{
    if (!elm) {
        return 0;
    }

    if (beingVisited.contains(elm) && (beingVisited.value(elm))) {
        return 0;
    }

    if (priorities.contains(elm)) {
        return priorities.value(elm);
    }

    beingVisited[elm] = true;
    int max = 0;

    for (auto *port : elm->outputs()) {
        for (auto *conn : port->connections()) {
            if (auto *successor = conn->otherPort(port)) {
                max = qMax(calculatePriority(successor->graphicElement(), beingVisited, priorities), max);
            }
        }
    }

    int priority = max + 1;
    priorities[elm] = priority;
    beingVisited[elm] = false;
    return priority;
}

const QVector<LogicElement *> &ElementMapping::logicElms() const
{
    return m_logicElms;
}
