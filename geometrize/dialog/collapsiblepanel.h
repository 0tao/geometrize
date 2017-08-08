#pragma once

#include <memory>

#include <QFrame>

namespace geometrize
{

namespace dialog
{

/**
 * @brief The CollapsiblePanel class models the UI for a collapsible container.
 */
class CollapsiblePanel : public QFrame
{
    Q_OBJECT

public:
    CollapsiblePanel(QWidget* parent = nullptr);
    ~CollapsiblePanel();

    void setup(const QString& title);

private:
    class CollapsiblePanelImpl;
    std::unique_ptr<CollapsiblePanelImpl> d;
};

}

}
