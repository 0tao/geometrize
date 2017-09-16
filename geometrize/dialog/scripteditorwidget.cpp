#include "scripteditorwidget.h"
#include "ui_scripteditorwidget.h"

#include <cassert>

#include <QEvent>
#include <QFontMetrics>
#include <QSize>

#include "localization/strings.h"

namespace geometrize
{

namespace dialog
{

class ScriptEditorWidget::ScriptEditorWidgetImpl
{
public:
    ScriptEditorWidgetImpl(ScriptEditorWidget* pQ, const std::string& title, const std::string& functionName, const std::string& defaultCode) : q{pQ}, m_functionName{functionName}, m_defaultCode{defaultCode}, ui{std::make_unique<Ui::ScriptEditorWidget>()}
    {
        ui->setupUi(q);
        populateUi();

        q->setTitle(QString::fromStdString(title));

        const QString contents{QString::fromStdString(defaultCode)};
        ui->scriptTextEdit->insertPlainText(contents);

        QFontMetrics metrics(ui->scriptTextEdit->font());
        const QSize size{metrics.size(0, contents)};
        const int padding = 30;
        ui->scriptTextEdit->setMinimumSize(ui->scriptTextEdit->width() + padding, size.height() + padding);

        q->connect(ui->applyScriptButton, &QPushButton::clicked, [this]() {
            const std::string code{ui->scriptTextEdit->toPlainText().toStdString()};
            q->signal_scriptApplied(q, code);
        });

        q->connect(ui->resetToDefaultButton, &QPushButton::clicked, [this]() {
            ui->scriptTextEdit->setPlainText(QString::fromStdString(m_defaultCode));
            q->signal_scriptReset(q, m_defaultCode);
        });
    }

    ScriptEditorWidgetImpl operator=(const ScriptEditorWidgetImpl&) = delete;
    ScriptEditorWidgetImpl(const ScriptEditorWidgetImpl&) = delete;
    ~ScriptEditorWidgetImpl()
    {
    }

    void onLanguageChange()
    {
        ui->retranslateUi(q);
        populateUi();
    }

private:
    void populateUi()
    {

    }

    ScriptEditorWidget* q;
    std::unique_ptr<Ui::ScriptEditorWidget> ui;
    const std::string m_functionName;
    const std::string m_defaultCode;
};

ScriptEditorWidget::ScriptEditorWidget(const std::string& title, const std::string& functionName, const std::string& defaultCode, QWidget* parent) : QGroupBox(parent), d{std::make_unique<ScriptEditorWidgetImpl>(this, title, functionName, defaultCode)}
{
}

ScriptEditorWidget::~ScriptEditorWidget()
{
}

void ScriptEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        d->onLanguageChange();
    }
    QGroupBox::changeEvent(event);
}

}

}
