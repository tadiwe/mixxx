#include "controllers/controllerlearningeventfilter.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QtDebug>

#include "control/pollingcontrolproxy.h"
#include "moc_controllerlearningeventfilter.cpp"
#include "widget/wknob.h"
#include "widget/wknobcomposed.h"
#include "widget/wslidercomposed.h"
#include "widget/wwidget.h"

ControllerLearningEventFilter::ControllerLearningEventFilter(QObject* pParent)
        : QObject(pParent),
          m_bListening(false) {
}

ControllerLearningEventFilter::~ControllerLearningEventFilter() {
}

bool ControllerLearningEventFilter::eventFilter(QObject* pObject, QEvent* pEvent) {
    // qDebug() << "ControllerLearningEventFilter::eventFilter" << pObject << pEvent;

    WWidget* pWidget = qobject_cast<WWidget*>(pObject);
    if (!pWidget || !m_bListening) {
        return false;
    }

    WKnob* pKnob = qobject_cast<WKnob*>(pObject);
    WKnobComposed* pKnobComposed = qobject_cast<WKnobComposed*>(pObject);
    WSliderComposed* pSlider = qobject_cast<WSliderComposed*>(pObject);
    bool has_right_click_reset = pKnob || pKnobComposed || pSlider;

    if (pEvent->type() == QEvent::KeyPress) {
        // TODO(XXX): handle keypresses?
    } else if (pEvent->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = reinterpret_cast<QMouseEvent*>(pEvent);
        qDebug() << "MouseButtonPress" << pWidget;

        const ControlInfo& info = m_widgetControlInfo[pWidget];
        if (mouseEvent->button() & Qt::LeftButton) {
            if (info.leftClickControl) {
                ConfigKey key = info.leftClickControl->getKey();
                qDebug() << "Left-click maps MIDI to:" << key.group << key.item;
                emit controlClicked(key);
            } else if (info.clickControl) {
                ConfigKey key = info.clickControl->getKey();
                emit controlClicked(key);
                qDebug() << "Default-click maps MIDI to:" << key.group << key.item;
            } else {
                qDebug() << "No control bound to left-click for" << pWidget;
            }
        }
        if (mouseEvent->button() & Qt::RightButton) {
            if (info.rightClickControl) {
                ConfigKey key = info.rightClickControl->getKey();
                qDebug() << "Right-click maps MIDI to:" << key.group << key.item;
                emit controlClicked(key);
            } else if (has_right_click_reset && (info.leftClickControl || info.clickControl)) {
                // WKnob and WSliderComposed emits a reset signal on
                // right-click. For controls that are derived from
                // ControlPotmeter, we can hack this by appending "_set_default"
                // to the ConfigKey item.
                ControlObject* pControl = info.leftClickControl;
                if (!pControl) {
                    pControl = info.clickControl;
                }
                ConfigKey key = pControl->getKey();
                key.item = key.item + "_set_default";
                if (ControlObject::exists(key)) {
                    qDebug() << "Right-click reset maps MIDI to:" << key.group << key.item;
                    emit controlClicked(key);
                }
            } else if (info.clickControl) {
                ConfigKey key = info.clickControl->getKey();
                qDebug() << "Default-click maps MIDI to:" << key.group << key.item;
                emit controlClicked(key);
            } else {
                qDebug() << "No control bound to right-click for" << pWidget;
            }
        }
    } else if (pEvent->type() == QEvent::MouseButtonRelease) {
        qDebug() << "MouseButtonRelease" << pWidget;
    } else if (pEvent->type() == QEvent::MouseMove) {
        qDebug() << "MouseMoveEvent" << pWidget;
    }
    return false;
}

void ControllerLearningEventFilter::addWidgetClickInfo(QWidget* pWidget,
        Qt::MouseButton buttonState,
        ControlObject* pControl,
        ControlParameterWidgetConnection::EmitOption emitOption) {
    ControlInfo& info = m_widgetControlInfo[pWidget];

    if (buttonState == Qt::LeftButton) {
        info.leftClickControl = pControl;
        info.leftEmitOption = emitOption;
    } else if (buttonState == Qt::RightButton) {
        info.rightClickControl = pControl;
        info.rightEmitOption = emitOption;
    } else {
        info.clickControl = pControl;
        info.emitOption = emitOption;
    }
}

void ControllerLearningEventFilter::startListening() {
    m_bListening = true;
}

void ControllerLearningEventFilter::stopListening() {
    m_bListening = false;
}
